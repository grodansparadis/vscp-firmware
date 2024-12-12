///////////////////////////////////////////////////////////////////////////////
// File: vscp_bootloader.h
//
// Implements the VSCP bootloader
//

/* ******************************************************************************
 * VSCP (Very Simple Control Protocol)
 * https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2010-2016 Sven Zwiers
 * Copyright (c) 2016-2024 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sub license, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *	This file is part of VSCP - Very Simple Control Protocol
 *	https://www.vscp.org
 *
 * ******************************************************************************
 *
 * vscpboot_loader is always the first code called when a system with the bootloader
 * installed is rebooted. It is up to this routine to check the bootloader flag
 * (vscpboot_getBootFlag) and. let the program flow take the right path. If the
 * bootloader flag is set to zero the application should be called (goApplication),
 * if not the bootloader should start it's work.
 *
 * This bootloader is constructed for devices that work in Level I and Level II
 * VSCP mode. This measn that data size can be 0-512 bytes (8 is maximum for level I)
 * The level is set with the VSCPBOOT_LEVEL define (see vscp_projdefs.h).
 *
 * The block size is the number of bytes the flash is programmed in. The bootloader should
 * collect this number of bytes and then expect a command to program the block. The
 * blocksize is defined by VSCPBOOT_BLOCKSIZE (see vscp_projdefs.h)
 * The blocksize can be less than the length of a communication frame or, more common,
 * larger. Data frames are received until a block is completely filled. Of there is excess
 * data in the last event the extra data is ignored. If there is more data frames coming
 * in after a block is full they are ignored. Only program-block and start-block can break
 * this.
 */

#ifndef __vscp_bootloader_h
#define __vscp_bootloader_h

#ifdef __cplusplus
extern "C" {
#endif

#include <crc.h>

// Macros

// This macro construct a signed integer from two unsigned chars in a safe way
#define construct_signed16(msb, lsb) ((int16_t)((((uint16_t)msb) << 8) + \
                                                (uint16_t)lsb))

// This macro construct a unsigned integer from two unsigned chars in a safe way
#define construct_unsigned16(msb, lsb) ((uint16_t)((((uint16_t)msb) << 8) + \
                                                   (uint16_t)lsb))

// This macro construct a signed long from four unsigned chars in a safe way
#define construct_signed32(b0, b1, b2, b3) ((int32_t)((((uint32_t)b0) << 24) + \
                                                      (((uint32_t)b1) << 16) + \
                                                      (((uint32_t)b2) << 8) +  \
                                                      (uint32_t)b3))

// This macro construct a unsigned long from four unsigned chars in a safe way
#define construct_unsigned32(b0, b1, b2, b3) ((uint32_t)((((uint32_t)b0) << 24) + \
                                                         (((uint32_t)b1) << 16) + \
                                                         (((uint32_t)b2) << 8) +  \
                                                         (uint32_t)b3))

#ifndef FALSE
#define FALSE 0
#ifndef TRUE
#define TRUE !FALSE
#endif
#endif

#ifndef ON
#define ON !FALSE
#endif

#ifndef OFF
#define OFF FALSE
#endif

#ifndef BIT
#define BIT(n) (1 << (n))
#endif

#ifndef BOOL
#define BOOL int
#endif

#define VSCPBOOT_FLAG_BOOT     0xFF // Value of bootflag that makes device go into bootloader
#define VSCPBOOT_NICKNAME_BOOT 0xFE // nickname reserved for booting device

#define TIMOUT_ANNOUNCE 10000 // Timeout in milliseconds waiting for node responding with ACK after we
                              // sent out new node on line

typedef enum vscpboot_state_t {
  STATE_BLOCKSTART = 0, // Haven't done anything yet
  STATE_BLOCKPRG,       // Waiting for program block or activate image
  STATE_BLOCKDATA,      // Block data (filling the block with data)
} vscpboot_state;

typedef struct vscpboot_config_t {
  uint8_t vscpLevel; /* 0=level I, 1=level II */
  uint32_t blockSize;
  uint32_t blockCount;
} vscpboot_config_t;

// Prototypes

/*!
  Setup send event
  @param pex Pointer to event ex that will get initialized for send
*/
void
vscpboot_initEventEx(vscpEventEx* const pex);

/*!
  Init VSCP boot
*/

void
vscpboot_init(void);

/*!
  VSCP bootloader state machine
*/
void
vscpboot_loader(void);

//*****************************************************************************
//        The following methods should be defined by the implementor
//*****************************************************************************

/*!
  This code jumps to the application. Typically this is code
  like

  void (*foo)(void) = (void (*)())0x12345678;
  foo();

  where 0x12345678 is the address to jump to.
*/
void
vscpboot_goApplication(void);


/*!
  Stop the execution of the bootloader state machine
  and release hardware.
  ONLY INTENDED FOR SIMULATION!!!
*/
int
vscpboot_run(void);

/*!
  Initialize hardware. This should be the first method called.
  The communication interface should be opened here and the system
  should be ready to receive firmware code events when done.
  @return VSCP_ERROR_SUCCESS if everything is OK.
*/
int
vscpboot_init_hardware(void);

/*!
  Release any hardware and memory resources here.

  This call may not be needed when jumping to new
  firmware but it may still be a good practice to
  call this method after a new firmware has been
  completely loaded into memory and before control is
  handed over to the new firmware image.
  @return VSCP_ERROR_SUCCESS if everything is OK.
*/
int
vscpboot_release_hardware(void);

/*!
  Get Device bootflag from persistent storage.
  @return Bootflag for node. Zero means application should start. Nonzero
    means bootloader should start. Application can use other values for
    bootflag or multibyte storage as long as this routine report expected state.
*/
uint8_t
vscpboot_getBootFlag(void);

/*!
  Set Device bootflag in persistent storage. Application can use other values for
  bootflag or multibyte storage as long as this routine set proper state.
  @param bootflag. New bootflag to set. Zero means application should start. Nonzero
    means bootloader should start.
  @return VSCP_ERROR_SUCCESS if OK.
*/
int
vscpboot_setBootFlag(uint8_t bootflag);

/*!
  Reboot the board. This method should never return.
*/
void
vscpboot_reboot(void);

/*!
  Get bootloader configuration (# blocks. blocksize)
  @return Pointer to boot configuration
*/
vscpboot_config_t*
vscpboot_getConfig(void);

/*!
  Get GUID for device.
  On some devices GUID may be programmed by the firmware load
  process. This is typical for devices that have a id/userid
  in flash. For devices using Ethernet etc construct GUID in
  the usual manner.
  @return Pointer to GUID
*/
uint8_t*
vscpboot_getGUID(void);

/*!
  Check if memory type is valid. type == 0 is always valid
  Return true if the memory type is valid on your hardware and
  can be programmed. See CLASS1.PROTOCOL, VSCP_TYPE_PROTOCOL_START_BLOCK for a list
  of types.
  @return non zero,  TRUE, if valid
*/
int
vscpboot_isMemTypeValid(uint8_t type);

/*!
  Check if memory bank is valid. type == 0 is always valid
  Return true if the bank is valid on your hardware.
  @return non zero, TRUE, if valid
*/
int
vscpboot_isMemBankValid(uint8_t bank);

/*!
  Program a block
  @param pblock Pointer to the block to program
  @param type Memory type
  @param bank Memory bank to program
  @return VSCP_ERROR_SUCCESS on success and else error code
*/
int
vscpboot_programBlock(const uint8_t* pblock, uint8_t type, uint8_t bank);

/*!
  The CRC for the loaded data is calculated here. This is the CRC
  over all blocks programmed calculated with CRC-CCITT. For a successful
  programming this value should be the same as the one provided in the
  activate new image event.
  @return crc CRC-CCITT for programmed area.
*/
crc
vscpboot_calcPrgCrc(void);

/*!
  Send VSCP event
  @param pex Pointer to VSCP event ex.
  @return VSCP_ERROR_SUCCESS on success
*/
int
vscpboot_sendEventEx(vscpEventEx* pex);

/*!
  Get VSCP event
  -----------------------------------------------------------
  IMPORTANT!

  This routine should translate all VSCP_CLASS2_LEVEL1_PROTOCOL
  events to VSCP_CLASS1_PROTOCOL events. That means dropping
  the interface that is stored in the first 16-bytes of data, and
  shifting data after it to base zero and decrease the event size by 16.

  It should block while waiting for event
  -----------------------------------------------------------
  @param Pointer to pointer to VSCP event structure.
   @return VSCP_ERROR_SUCCESS on success
           VSCP_ERROR_FIFO_EMPTY if empty.
           VSCP_ERROR_ERROR on other error.
*/
int
vscpboot_getEventEx(vscpEventEx* pex);

/*!
  Unallocate storage for VSCP event ex
  @param pex Pointer to pointer to event ex to delete
*/
// void
// vscpboot_deleteEventEx(vscpEventEx** pex);

#ifdef __cplusplus
}
#endif

#endif /* _bootloader_h */
