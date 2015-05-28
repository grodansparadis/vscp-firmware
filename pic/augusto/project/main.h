// File:  main.h

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	Version: See project header
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 1995-2005 Ake Hedman, eurosource
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 
 *	This file is part of VSCP - Very Simple Control Protocol 	
 *	http://www.vscp.org
 *
 * ******************************************************************************
*/

#ifndef MAIN_H
#define MAIN_H

//Defines
#define	TRUE			1
#define	FALSE			0




//
// Size for array used for temp mean calculations
//
#define TEMPERATURE_ARRAY_SIZE 	0x0A

/* EEPROM Layout
 * FROM -  TO  SIZE(dec)   BLOCKNAME
 * 0000 - 0000     01   Bootloader check
 * 0001 - 0001     01   VSCP Address ID
 * 0002 - 0002     01   Eeprom checksum
 * 0003 - 0038     40   Augusto Driver
 *
 */

#define NEXT                            0x07


/* Register map
 * Page 0
 *
 */
/*

// The following can be stored in program ROM (recommended) or in EEPROM 

#define VSCP_EEPROM_REG_GUID			0x11	// Start of GUID MSB	
													// 		0x11 - 0x20				

#define VSCP_EEPROM_REG_DEVICE_URL		0x21	// Start of Device URL storage
													// 		0x21 - 0x40

#define VSCP_EEPROM_END				0x41	// marks end of VSCP EEPROM usage
													//   (next free position)

*/

// Function Prototypes

void doWork( void );
void init( void );
void init_app_ram( void );
void init_app_eeprom( void ); 
void read_app_register( unsigned char reg );
void write_app_register( unsigned char reg, unsigned char val );
void sendDMatrixInfo( void );
void SendInformationEvent( unsigned char idx, unsigned char eventClass, unsigned char eventTypeId, unsigned char keyAction );

void SendProbeAck (unsigned char buttontime);

void doDM( void );

void doActionOn( unsigned char dmflags, unsigned char arg );
void doActionOff( unsigned char dmflags, unsigned char arg );
void doActionToggle( unsigned char dmflags, unsigned char arg );

void doApplicationOneSecondWork( void );


#endif
