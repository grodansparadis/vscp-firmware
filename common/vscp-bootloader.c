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
#include <stdlib.h>
#include <string.h>

#include <vscp-class.h>
#include <vscp-type.h>
#include <vscp.h>

#include "vscp-compiler.h"
#include "vscp-projdefs.h"

#include "vscp-bootloader.h"

// Uncomment to enable debug output
#define VSCP_BOOTLOADER_DEBUG

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

// Globals
volatile uint16_t g_vscpboot_timer = 0;
uint8_t g_vscpboot_nickname        = 0xFE; // Assigned node nickname
crc g_full_crc                     = 0;    // CRC for all memory blocks transferred to us

////////////////////////////////////////////////////////////===================
// vscpboot_initEventEx
//
// Setup event for sending
//

void
vscpboot_initEventEx(vscpEventEx* const pex)
{
  if (NULL == pex) {
    return;
  }

  memset(pex, 0, sizeof(vscpEventEx));

  /*
    Set GUID or just nickname
  */
  if (VSCP_LEVEL1 == vscpboot_getConfig()->vscpLevel) {
    pex->GUID[15] = vscpboot_getGUID()[15];
    pex->GUID[14] = vscpboot_getGUID()[14];
  }
  else {
    memcpy(pex->GUID, vscpboot_getGUID(), 16);
  }
  pex->head = VSCP_PRIORITY_HIGH;
}

////////////////////////////////////////////////////////////===================
// vscpboot_init
//

void
vscpboot_init(void)
{
  // Initialize CRC lookup table.
  crcInit();
}

////////////////////////////////////////////////////////////===================
// send_bootload_ack
//

static int
send_bootload_ack(void)
{
  vscpEventEx sndex;

  vscpboot_initEventEx(&sndex);
  sndex.sizeData   = 8;
  sndex.vscp_class = VSCP_CLASS1_PROTOCOL;
  sndex.vscp_type  = VSCP_TYPE_PROTOCOL_ACK_BOOT_LOADER;
  sndex.data[0]    = ((vscpboot_getConfig()->blockSize) >> 24) & 0xFF;
  sndex.data[1]    = ((vscpboot_getConfig()->blockSize) >> 16) & 0xFF;
  sndex.data[2]    = ((vscpboot_getConfig()->blockSize) >> 8) & 0xFF;
  sndex.data[3]    = ((vscpboot_getConfig()->blockSize) & 0xFF);
  sndex.data[4]    = ((vscpboot_getConfig()->blockCount) >> 24) & 0xFF;
  sndex.data[5]    = ((vscpboot_getConfig()->blockCount) >> 16) & 0xFF;
  sndex.data[6]    = ((vscpboot_getConfig()->blockCount) >> 8) & 0xFF;
  sndex.data[7]    = ((vscpboot_getConfig()->blockCount) & 0xFF);
  vscpboot_sendEventEx(&sndex);
#ifdef VSCP_BOOTLOADER_DEBUG
  printf("Sent ACK for boot loader\n");
#endif
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscpboot_loader
//

void
vscpboot_loader(void)
{
  int rv;

  crcInit();

  // Init hardware. If this stage fails we are doomed
  // We skip the callback as we are in the bootloader
  vscpboot_init_hardware();

  // Check if the application should be started.
  if (!vscpboot_getBootFlag()) {

    // Release any hardware used by the bootloader.
    // this may not be needed as the firmware will
    // init the system from scratch when it start
    // *** Note that this is the only place this happens ***
    vscpboot_release_hardware();

    // This call expects the application to clear the stack as
    // we by this call put info on our stack.
    vscpboot_goApplication();
  }

  // We are in boot mode

  vscpEventEx sndex;
  vscpboot_state state = STATE_BLOCKSTART; // We haven't done anything yet

  uint32_t block;                                       // Current block to program
  uint8_t blockdata[(vscpboot_getConfig()->blockSize)]; // Block data buffer
  uint16_t offset;                                      // offset into blockdata
  uint16_t crc_block;                                   // CRC calculated over block data
  uint8_t memtype;                                      // optional memory type to program
  uint8_t membank;                                      // optional memory bank to program

  // Confirm bootmode - Send bootmode ACK
  send_bootload_ack();

  vscpEventEx ex;
  while (vscpboot_run()) { // not only diamonds are forever...

    // Block until VSCP event is received
    if (VSCP_ERROR_SUCCESS != (rv = vscpboot_getEventEx(&ex))) {
      // VSCP_ERROR_FIFO_EMPTY if empty
      // VSCP_ERROR_ERROR on other error
      continue;
    }

    // if (NULL == pex) {
    //   continue;
    // }

#ifdef VSCP_BOOTLOADER_DEBUG
    printf("Bootevent class=%02X type=%02X\n", ex.vscp_class, ex.vscp_type);
#endif
    switch (state) {

      // ----------------------------------------------------------------
      // We are in bootmode and waiting for something to happen. Anything
      //-----------------------------------------------------------------
      case STATE_BLOCKSTART:

        if ((ex.vscp_class == VSCP_CLASS1_PROTOCOL) &&
            (ex.vscp_type == VSCP_TYPE_PROTOCOL_START_BLOCK) &&
            ex.sizeData >= 4) {

#ifdef VSCP_BOOTLOADER_DEBUG
          printf("STATE_BLOCKSTART\n");
#endif
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

          if ((block >= (vscpboot_getConfig()->blockCount)) || !vscpboot_isMemTypeValid(memtype) || !vscpboot_isMemBankValid(membank)) {

            // NACK block start
            vscpboot_initEventEx(&sndex);
            sndex.sizeData   = 0;
            sndex.vscp_class = VSCP_CLASS1_PROTOCOL;
            sndex.vscp_type  = VSCP_TYPE_PROTOCOL_START_BLOCK_NACK;
            vscpboot_sendEventEx(&sndex);
#ifdef VSCP_BOOTLOADER_DEBUG
            printf("Sent NACK for block start\n");
#endif
            state = STATE_BLOCKSTART;
            break;
          }

          // ACK block start
          vscpboot_initEventEx(&sndex);
          sndex.sizeData   = 0;
          sndex.vscp_class = VSCP_CLASS1_PROTOCOL;
          sndex.vscp_type  = VSCP_TYPE_PROTOCOL_START_BLOCK_ACK;
          vscpboot_sendEventEx(&sndex);
#ifdef VSCP_BOOTLOADER_DEBUG
          printf("Sent ACK for block start\n");
#endif

          // init the block
          memset(blockdata, 0xff, (vscpboot_getConfig()->blockSize));

          // Go to block data state
          state = STATE_BLOCKDATA;
        }
        else if ((ex.vscp_class == VSCP_CLASS1_PROTOCOL) &&
                 (ex.vscp_type == VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE) &&
                 (2 == ex.sizeData)) {
          if (construct_unsigned16(ex.data[0], ex.data[1]) == vscpboot_calcPrgCrc()) {
            // ACK activate image
            vscpboot_initEventEx(&sndex);
            sndex.sizeData   = 0;
            sndex.vscp_class = VSCP_CLASS1_PROTOCOL;
            sndex.vscp_type  = VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE_ACK;
            vscpboot_sendEventEx(&sndex);
#ifdef VSCP_BOOTLOADER_DEBUG
            printf("Activate image ACK\n");
#endif
          }
          else {
            // NACK activate image
            vscpboot_initEventEx(&sndex);
            sndex.sizeData   = 0;
            sndex.vscp_class = VSCP_CLASS1_PROTOCOL;
            sndex.vscp_type  = VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE_NACK;
            vscpboot_sendEventEx(&sndex);
#ifdef VSCP_BOOTLOADER_DEBUG
            printf("Activate image NACK\n");
#endif
          } // CRC check
        }
        else if ((ex.vscp_class == VSCP_CLASS1_PROTOCOL) &&
                 (ex.vscp_type == VSCP_TYPE_PROTOCOL_BOOT_LOADER_CHECK) &&
                 (0 == ex.sizeData)) {
          // Confirm bootmode - Send bootmode ACK
          send_bootload_ack(); // No check for errors here
        }
        break;

      // ----------------------------------------------------------------
      case STATE_BLOCKDATA:

#ifdef VSCP_BOOTLOADER_DEBUG
        printf("STATE_BLOCKDATA\n");
#endif

        if ((ex.vscp_class == VSCP_CLASS1_PROTOCOL) &&
            (ex.vscp_type == VSCP_TYPE_PROTOCOL_BLOCK_DATA)) {

          /*
            free space is ((vscpboot_getConfig()->blockSize) - offset)
            We accept an overrun once - NACK if there is more data
            received.
          */

#ifdef VSCP_BOOTLOADER_DEBUG
          printf("VSCP_TYPE_PROTOCOL_BLOCK_DATA offset=%d\n", (int)offset);
#endif

          // Check if the block is full
          if (offset < vscpboot_getConfig()->blockSize) {

            // Print block data
            // {
            //   for (size_t i = 0; i < MIN(ex.sizeData, vscpboot_getConfig()->blockSize - offset); i++) {
            //     printf("%02X ", blockdata[offset + i]);
            //   }
            //   printf("\n");
            // }

            // copy in data
            memcpy(blockdata + offset, ex.data, MIN(ex.sizeData, vscpboot_getConfig()->blockSize - offset));
            offset += MIN(ex.sizeData, vscpboot_getConfig()->blockSize - offset);
#ifdef VSCP_BOOTLOADER_DEBUG
            printf("offset=%d\n", offset);
#endif

            // ACK the chunk
            vscpboot_initEventEx(&sndex);
            sndex.sizeData   = 0;
            sndex.vscp_class = VSCP_CLASS1_PROTOCOL;
            sndex.vscp_type  = VSCP_TYPE_PROTOCOL_BLOCK_CHUNK_ACK;
            vscpboot_sendEventEx(&sndex);
#ifdef VSCP_BOOTLOADER_DEBUG
            printf("Sent ACK for chunk\n");
#endif

            // If we have a full block we should program it
            if (offset == (vscpboot_getConfig()->blockSize)) {

              // Calculate CRC
              crc_block = crcFast(blockdata, (vscpboot_getConfig()->blockSize));

              // Print block data
              // {
              //   for (size_t i = 0; i < (vscpboot_getConfig()->blockSize); i++) {
              //     printf("%02X ", blockdata[i]);
              //     if (i % 16 == 15) {
              //       printf("\n");
              //     }
              //   }
              // }

              // ACK the block data
              vscpboot_initEventEx(&sndex);
              sndex.sizeData   = 6;
              sndex.data[0]    = (crc_block >> 8) & 0xff;
              sndex.data[1]    = crc_block & 0xff;
              sndex.data[2]    = (block >> 24) & 0xff;
              sndex.data[3]    = (block >> 16) & 0xff;
              sndex.data[4]    = (block >> 8) & 0xff;
              sndex.data[5]    = block & 0xff;
              sndex.vscp_class = VSCP_CLASS1_PROTOCOL;
              sndex.vscp_type  = VSCP_TYPE_PROTOCOL_BLOCK_DATA_ACK;
              vscpboot_sendEventEx(&sndex);
#ifdef VSCP_BOOTLOADER_DEBUG
              printf("Sent ACK for block %X\n", crc_block);
#endif
              offset = 0;
              state  = STATE_BLOCKPRG;
            }
          }
          else {
            // Offset bigger than block size

            // NACK the data block
            vscpboot_initEventEx(&sndex);
            sndex.sizeData   = 5;
            sndex.data[0]    = 0; // No special error code
            sndex.data[1]    = (block >> 24) & 0xff;
            sndex.data[2]    = (block >> 16) & 0xff;
            sndex.data[3]    = (block >> 8) & 0xff;
            sndex.data[4]    = block & 0xff;
            sndex.vscp_class = VSCP_CLASS1_PROTOCOL;
            sndex.vscp_type  = VSCP_TYPE_PROTOCOL_BLOCK_DATA_NACK;
            vscpboot_sendEventEx(&sndex);
#ifdef VSCP_BOOTLOADER_DEBUG
            printf("Sent NACK for block\n");
#endif
            offset = 0;
            state  = STATE_BLOCKSTART; // Start over with this block
          }
        } // BLOCK_DATA
        // We can break state with BLOCK_START
        else if ((ex.vscp_class == VSCP_CLASS1_PROTOCOL) &&
                 (ex.vscp_type == VSCP_TYPE_PROTOCOL_START_BLOCK) &&
                 ex.sizeData >= 4) {
          state = STATE_BLOCKSTART;
        }
        else if ((ex.vscp_class == VSCP_CLASS1_PROTOCOL) &&
                 (ex.vscp_type == VSCP_TYPE_PROTOCOL_BOOT_LOADER_CHECK) &&
                 (0 == ex.sizeData)) {
          // Confirm bootmode - Send bootmode ACK
          send_bootload_ack(); // No check for errors here
        }
        break;

      // ----------------------------------------------------------------
      case STATE_BLOCKPRG:

#ifdef VSCP_BOOTLOADER_DEBUG
        printf("STATE_BLOCKPRG\n");
#endif

        if ((ex.vscp_class == VSCP_CLASS1_PROTOCOL) &&
            (ex.vscp_type == VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA) &&
            (4 == ex.sizeData)) {

          // Check if the block is the one we are working on
          if (block == construct_unsigned32(ex.data[0], ex.data[1], ex.data[2], ex.data[3])) {
            if (VSCP_ERROR_SUCCESS != vscpboot_programBlock(blockdata, memtype, membank)) {
              // NACK the data block - Wrong program block
              vscpboot_initEventEx(&sndex);
              sndex.sizeData   = 0;
              sndex.vscp_class = VSCP_CLASS1_PROTOCOL;
              sndex.vscp_type  = VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA_NACK;
              vscpboot_sendEventEx(&sndex);
#ifdef VSCP_BOOTLOADER_DEBUG
              printf("Sent NACK for program block\n");
#endif
            }

            // ACK program block
            vscpboot_initEventEx(&sndex);
            sndex.sizeData   = 0;
            sndex.vscp_class = VSCP_CLASS1_PROTOCOL;
            sndex.vscp_type  = VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA_ACK;
            vscpboot_sendEventEx(&sndex);
#ifdef VSCP_BOOTLOADER_DEBUG
            printf("Sent ACK for program block\n");
#endif
            state = STATE_BLOCKSTART; 
          }
          else {

            // Wrong block

#ifdef VSCP_BOOTLOADER_DEBUG
            printf("NACK_PRG_BLOCK\n");
#endif

            // NACK the data block - Wrong program block
            vscpboot_initEventEx(&sndex);
            sndex.sizeData   = 0;
            sndex.vscp_class = VSCP_CLASS1_PROTOCOL;
            sndex.vscp_type  = VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA_NACK;
            vscpboot_sendEventEx(&sndex);
#ifdef VSCP_BOOTLOADER_DEBUG
            printf("Sent NACK for program block\n");
#endif
            state = STATE_BLOCKSTART; // Start over
          }
        }
        // We can break state with BLOCK_START
        else if ((ex.vscp_class == VSCP_CLASS1_PROTOCOL) &&
                 (ex.vscp_type == VSCP_TYPE_PROTOCOL_START_BLOCK) &&
                 ex.sizeData >= 4) {
          state = STATE_BLOCKSTART;
        }
        else if ((ex.vscp_class == VSCP_CLASS1_PROTOCOL) &&
                 (ex.vscp_type == VSCP_TYPE_PROTOCOL_BOOT_LOADER_CHECK) &&
                 (0 == ex.sizeData)) {
          // Confirm bootmode - Send bootmode ACK
          send_bootload_ack(); // No check for errors here
        }
        break;

      // ----------------------------------------------------------------
      default:
        state = STATE_BLOCKSTART;
        break;
    }

    // Cleanup event
    // vscpboot_deleteEventEx(&pex);

  } // while

  //---------------------------------------------------------------------------

  /*
    Normally note reaching this point but
    simulated hardware may come here
  */
  vscpboot_release_hardware();
}
