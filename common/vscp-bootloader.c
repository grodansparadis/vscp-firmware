///////////////////////////////////////////////////////////////////////////////
// File: vscp_bootloader.c
//
// Implements the VSCP bootloader
//

/* ******************************************************************************
 *  VSCP (Very Simple Control Protocol)
 *  http://www.vscp.org
 *
 *  The MIT License (MIT)
 *
 *  Copyright (c) 2010-2016 Sven Zwiers
 *  Copyright (c) 2016-2024 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sub license, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 *	This file is part of VSCP - Very Simple Control Protocol
 *	http://www.vscp.org
 *
 * ******************************************************************************
 */

#define CRC_CCITT
#include "crc.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include <vscp.h>
#include <vscp_class.h>
#include <vscp_type.h>

#include "vscp-compiler.h"
#include "vscp-projdefs.h"

#include "vscp-bootloader.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// Globals
volatile uint16_t vscpboot_timer = 0;
uint8_t vscpboot_nickname        = 0xFE; // Assigned node nickname

void
vscpboot_init()
{
  // Initialize CRC lookup table.
  crcInit();
}

///////////////////////////////////////////////////////////////////////////////
// vscpboot_loader
//

void
vscpboot_loader(void)
{
  int rv;

  // Init hardware. If this stage fails we are doomed
  vscpboot_init_hardware();

  // Check if the application should be started.
  if (!vscpboot_getBootFlag()) {

    // Release any harware used by the bootloader.
    // this may not be needed as the firmware will
    // init the system from scratch when it start
    // *** Note that this is the only place this happens ***
    vscpboot_release_hardware();

    // This call expects the application to clear the stack as
    // we by this call put info on our stack.
    vscpboot_goApplication();
  }

  vscpEventEx ex;
  vscpboot_state state = STATE_BLOCKSTART; // We haven't done anything yet

  uint32_t block;                        // Current block to program
  uint8_t blockdata[(vscpboot_getConfig()->blockSize)]; // Block data buffer
  uint16_t offset;                       // offset into blockdata
  uint16_t crc;                          // CRC calculated over all received data
  uint8_t memtype;                       // optional memory type to program
  uint8_t membank;                       // optional memory bank to program

ENTER_BOOT_MODE:

  memset(&ex, 0, sizeof(vscpEventEx));

  // Confirm bootmode - Send bootmode ACK
  ex.head       = VSCP_PRIORITY_HIGH;
  ex.sizeData   = 8;
  ex.vscp_class = VSCP_CLASS1_PROTOCOL;
  ex.vscp_type  = VSCP_TYPE_PROTOCOL_ACK_BOOT_LOADER;
  ex.data[0]    = ((vscpboot_getConfig()->blockSize) >> 24) & 0xFF;
  ex.data[1]    = ((vscpboot_getConfig()->blockSize) >> 16) & 0xFF;
  ex.data[2]    = ((vscpboot_getConfig()->blockSize) >> 8) & 0xFF;
  ex.data[3]    = ((vscpboot_getConfig()->blockSize) & 0xFF);
  ex.data[4]    = ((vscpboot_getConfig()->blockCount) >> 24) & 0xFF;
  ex.data[5]    = ((vscpboot_getConfig()->blockCount) >> 16) & 0xFF;
  ex.data[6]    = ((vscpboot_getConfig()->blockCount) >> 8) & 0xFF;
  ex.data[7]    = ((vscpboot_getConfig()->blockCount) & 0xFF);
  if (VSCP_ERROR_SUCCESS != (rv = vscpboot_sendEventEx(&ex))) { 
    printf("Send error: rv=%d\n", rv);
  }

  while (vscpboot_run()) { // not only diamonds are forever...

    // Block until VSCP event is received
    if (VSCP_ERROR_SUCCESS != (rv = vscpboot_getEventEx(&ex))) {
      // VSCP_ERROR_FIFO_EMPTY if empty
      // VSCP_ERRROR_ERROR on other error
      continue;
    }

    switch (state) {

      // ----------------------------------------------------------------
      // We are in bootmode and waiting for something to happen. Anything
      //-----------------------------------------------------------------
      case STATE_BLOCKSTART:

        if ((ex.vscp_class == VSCP_CLASS1_PROTOCOL) &&
            (ex.vscp_type == VSCP_TYPE_PROTOCOL_START_BLOCK) &&
            ex.sizeData >= 4) {

          // No data received yet
          offset = 0;

          // Default memtype and bank
          memtype = membank = 0;

          // Get block to program
          block = construct_unsigned32(ex.data[0], ex.data[1], ex.data[2], ex.data[3]);

          // Set memtype if available
          if (ex.sizeData >= 5) {
            memtype = ex.data[4];
          }

          // Set membank if available
          if (ex.sizeData >= 6) {
            membank = ex.data[5];
          }

          if ((block >= (vscpboot_getConfig()->blockCount)) || !vscpboot_isMemTypeValid(memtype) || !vscpboot_isMemTypeValid(membank)) {
            // NACK block start
            ex.head       = VSCP_PRIORITY_HIGH;
            ex.sizeData   = 0;
            ex.vscp_class = VSCP_CLASS1_PROTOCOL;
            ex.vscp_type  = VSCP_TYPE_PROTOCOL_START_BLOCK_NACK;
            vscpboot_sendEventEx(&ex);
            goto ENTER_BOOT_MODE;
          }

          // ACK block start
          ex.head       = VSCP_PRIORITY_HIGH;
          ex.sizeData   = 0;
          ex.vscp_class = VSCP_CLASS1_PROTOCOL;
          ex.vscp_type  = VSCP_TYPE_PROTOCOL_START_BLOCK_ACK;
          if (VSCP_ERROR_SUCCESS == vscpboot_sendEventEx(&ex)) {
            goto ENTER_BOOT_MODE;
          }

          // init the block
          memset(blockdata, 0xff, (vscpboot_getConfig()->blockSize));

          state = STATE_BLOCKDATA;
        }
        else if ((ex.vscp_class == VSCP_CLASS1_PROTOCOL) &&
                 (ex.vscp_type == VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE) &&
                 (2 == ex.sizeData)) {
          if (construct_unsigned16(ex.data[0], ex.data[1]) == vscpboot_calcPrgCrc()) {
            // ACK activate image
            ex.head       = VSCP_PRIORITY_HIGH;
            ex.sizeData   = 0;
            ex.vscp_class = VSCP_CLASS1_PROTOCOL;
            ex.vscp_type  = VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE_ACK;

            if (VSCP_ERROR_SUCCESS != vscpboot_sendEventEx(&ex)) {
              goto ENTER_BOOT_MODE;
            }
          }
          else {
            // NACK activate image
            ex.head       = VSCP_PRIORITY_HIGH;
            ex.sizeData   = 0;
            ex.vscp_class = VSCP_CLASS1_PROTOCOL;
            ex.vscp_type  = VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE_NACK;

            if (VSCP_ERROR_SUCCESS != vscpboot_sendEventEx(&ex)) {
              goto ENTER_BOOT_MODE;
            }
          }
        }

        break;

      // ----------------------------------------------------------------
      case STATE_BLOCKDATA:

        if ((ex.vscp_class == VSCP_CLASS1_PROTOCOL) &&
            (ex.vscp_type == VSCP_TYPE_PROTOCOL_BLOCK_DATA)) {

          /*
            free space is ((vscpboot_getConfig()->blockSize) - offset)
            We accept an overrun once - NACK if there is more data
            received.
          */

          // Check if the block is full
          if (offset < (vscpboot_getConfig()->blockSize)) {

            // copy in data
            memcpy(blockdata + offset, ex.data, MIN(ex.sizeData, (vscpboot_getConfig()->blockSize) - offset));
            offset += MIN(ex.sizeData, (vscpboot_getConfig()->blockSize) - offset);

            // ACK the block data
            ex.head       = VSCP_PRIORITY_HIGH;
            ex.sizeData   = 0;
            ex.vscp_class = VSCP_CLASS1_PROTOCOL;
            ex.vscp_type  = VSCP_TYPE_PROTOCOL_BLOCK_DATA_ACK;
            vscpboot_sendEventEx(&ex);

            // ACK the data block
            ex.head       = VSCP_PRIORITY_HIGH;
            ex.sizeData   = 0;
            ex.vscp_class = VSCP_CLASS1_PROTOCOL;
            ex.vscp_type  = VSCP_TYPE_PROTOCOL_BLOCK_DATA_ACK;

            if (vscpboot_sendEventEx(&ex)) { // ACK program block request
              goto ENTER_BOOT_MODE;
            }
          }
          else {
            // NACK the data block
            ex.head       = VSCP_PRIORITY_HIGH;
            ex.sizeData   = 0;
            ex.vscp_class = VSCP_CLASS1_PROTOCOL;
            ex.vscp_type  = VSCP_TYPE_PROTOCOL_BLOCK_DATA_NACK;

            if (VSCP_ERROR_SUCCESS != vscpboot_sendEventEx(&ex)) {
              goto ENTER_BOOT_MODE;
            }
          }
        }
        break;

      // ----------------------------------------------------------------
      case STATE_BLOCKPRG:

        if ((ex.vscp_class == VSCP_CLASS1_PROTOCOL) &&
            (ex.vscp_type == VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA) &&
            (4 == ex.sizeData)) {
          if (block == construct_unsigned32(ex.data[0], ex.data[1], ex.data[2], ex.data[3])) {
            if (VSCP_ERROR_SUCCESS != vscpboot_programBlock(blockdata, memtype, membank)) {
              goto NACK_PRG_BLOCK;
            }
            // ACK the data block
            ex.head       = VSCP_PRIORITY_HIGH;
            ex.sizeData   = 0;
            ex.vscp_class = VSCP_CLASS1_PROTOCOL;
            ex.vscp_type  = VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA_ACK;

            if (VSCP_ERROR_SUCCESS != vscpboot_sendEventEx(&ex)) {
              goto ENTER_BOOT_MODE;
            }
          }
          else {
          NACK_PRG_BLOCK:
            // NACK the data block - Wrong program block
            ex.head       = VSCP_PRIORITY_HIGH;
            ex.sizeData   = 0;
            ex.vscp_class = VSCP_CLASS1_PROTOCOL;
            ex.vscp_type  = VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA_NACK;

            if (VSCP_ERROR_SUCCESS != vscpboot_sendEventEx(&ex)) {
              goto ENTER_BOOT_MODE;
            }
          }
          state = STATE_BLOCKSTART; // Start over
        }

        break;

      // ----------------------------------------------------------------
      default:
        state = STATE_BLOCKSTART;
        break;
    }

  } // while

  //---------------------------------------------------------------------------
  
  /*
    Normally note reaching this point but
    simulated hardware may come here
  */
  vscpboot_release_hardware();
  printf("VSCP Bootloader state machine ended");
}
