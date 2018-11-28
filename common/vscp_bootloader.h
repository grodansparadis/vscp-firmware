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
 * Copyright (c) 2016-2018 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
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
 */

#ifndef FALSE
#define FALSE               0
#ifndef TRUE
#define TRUE                !FALSE
#endif
#endif

#ifndef ON
#define ON                  !FALSE
#endif

#ifndef OFF
#define OFF                 FALSE
#endif

#ifndef BIT
#define BIT(n)              (1 << (n))
#endif

#ifndef BOOL
#define BOOL                int
#endif

#define VSCPBOOT_FLAG_BOOT          0xFF    // Value of bootglas that makes device go into bootloadeer
#define VSCPBOOT_NICKNAME_BOOT      0xFE	// nickname reserved for booting device


#define TIMOUT_ANNOUNCE             10000   // Timeout waiting for node responding with ACK after we
                                            // sent out new node on line

typedef enum vscpboot_state_t {
    STATE_VIRGIN = 0,   // Haven't done anything yet
    STATE_ANNOUNCE,     // New node on line sent, testing
                        // for ACK showing other node already
                        // have this nickname
    STATE_BOOTWAIT,     // Waiting for boot initiating  
    STATE_BLOCKWAIT,    // Waiting for program block
    STATE_BLOCKDATA,    // Block data (filling the block with data)
} vscpboot_state;

// Prototypes
void vscpboot_init( void );
void vscpboot_loader( void );

//int readEEPROM( uint8_t addr );
//int writeEEPROM( uint8_t addr, uint8_t data );

/*void SendInformationEventExtended
	(uint8_t priority, uint8_t zone, uint8_t subzone, uint8_t idx, 
	uint8_t eventClass, uint8_t eventTypeId );
*/
void boot_program_page (uint32_t page, uint8_t *buf);

//*****************************************************************************
//        The following methods should be defined by the implementor
//*****************************************************************************

///////////////////////////////////////////////////////////////////////////////
//  init_hardware
//
// Initialize hardware. This should be the fits method called.

void init_hardware( void );

///////////////////////////////////////////////////////////////////////////////
//  vscpboot_turnOnStatusLED
//
// Turn on status LED. If no LED is present on the board just implement
// as an empty method.
//
 
void vscpboot_turnOnStatusLED( void );

///////////////////////////////////////////////////////////////////////////////
//  vscpboot_turnOffStatusLED
//
// Turn off status LED. If no LED is present on the board just implement
// as an empty method.
//

void vscpboot_turnOffStatusLED( void );

///////////////////////////////////////////////////////////////////////////////
//  vscpboot_readInitButton
//
// Read state of init. button. Return zero for an active button or
// non zero for an inactive button.
// If the board does not have a button always return non-zero.
//
// @return Zero for active button. Non zero for inactive button.
//

uint8_t vscpboot_readInitButton( void );

///////////////////////////////////////////////////////////////////////////////
// vscpboot_getBootFlag
//
// Get Device bootflag from persistent storage.
//
// @return Bootflag for node. Zero means application should start. Nonzero
// means bootloader should start. Application can use other values for
// bootflag or multibyte storage as long as this routine get expected state.
//

uint8_t vscpboot_getBootFlag( void );

///////////////////////////////////////////////////////////////////////////////
// vscpboot_setBootFlag
//
// Set Device bootflag in persistent storage. Application can use other values for 
// bootflag or multibyte storage as long as this routine set proper state.
//
// @param bootflag. New bootflag to set. Zero means application should start. Nonzero
// means bootloader should start. 
//

void vscpboot_setBootFlag( uint8_t bootflag );

///////////////////////////////////////////////////////////////////////////////
// vscpboot_getNickname
//
// Get Device nickname from persistent storage.
//
// @return Nickname for node.
//

uint8_t vscpboot_getNickname( void );

///////////////////////////////////////////////////////////////////////////////
// vscpboot_setNickname
//
// Set Device nickname in persistent storage.
// A new nickname is set if the init. button is active upon startup.
//
// @param New nickname for node.

void vscpboot_setNickname( uint8_t nickname );




///////////////////////////////////////////////////////////////////////////////
//  vscpboot_reboot
//
// Reboot the board. This method should never return.

void vscpboot_reboot( void );


///////////////////////////////////////////////////////////////////////////////
//  vscpboot_getGUID
//
// Get bytes of GUID.
//
// @param idxGUID Bytes of GUID 0-15 MSB -> LSB
// @param return GUID for position.
//

uint8_t vscpboot_getGUID( uint8_t idxGUID );


///////////////////////////////////////////////////////////////////////////////
//  vscpboot_getPage
//
// Get current VSCP page
//
// @param return 16-bit page
//

uint16_t vscpboot_getPage( void );


///////////////////////////////////////////////////////////////////////////////
// vscpboot_sendEvent
//
// Send VSCP event
//
// @param Pointer to VSCP event structure.
// @param Return TRUE on success.
//

int vscpboot_sendEvent( vscpevent *pmsg );


///////////////////////////////////////////////////////////////////////////////
// vscpboot_getEvent
//
// Get VSCP event
//
// @param Pointer to VSCP event structure.
// @param Return TRUE if event is received.
//

int vscpboot_getEvent( vscpevent *pmsg );


