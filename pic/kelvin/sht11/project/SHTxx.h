/* 
 * Author: MST
 * Copyright (c) Sensiron AG
 *
 * ============================================================================
 *           Changes Copyright (C) 2004,2005 Ake Hedman, eurosource
 * ============================================================================
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

// $RCSfile: SHTxx.h,v $                                       
// $Date: 2005/01/05 12:50:54 $                                  
// $Author: akhe $                                              
// $Revision: 1.5 $ 

//----------------------------------------------------------------------------------
// modul-var
//----------------------------------------------------------------------------------

enum { SHT_MODE_TEMP, SHT_MODE_HUMI };

#define noACK 	0
#define ACK   	1
                            	//	adr  command  r/w
#define STATUS_REG_W 	0x06   	//	000   0011    0
#define STATUS_REG_R 	0x07   	//	000   0011    1
#define MEASURE_TEMP 	0x03   	//	000   0001    1
#define MEASURE_HUMI 	0x05   	//	000   0010    1
#define RESET        	0x1e   	//	000   1111    0

// SHT measurement timeout in ms
#define SHT_READ_TIMEOUT	500

#define TEMP_UNIT_KELVIN 		0
#define TEMP_UNIT_CELSIUS		1
#define TEMP_UNIT_FAHRENHEIT	2

// Prototypes
unsigned char sht_calc_crc( unsigned char crc, unsigned char x );
char sht_write_byte( unsigned char value );
char sht_read_byte( unsigned char ack );
void sht_transstart( void );
void sht_connectionreset( void );
char sht_softreset( void );
char sht_read_statusreg( unsigned char *p_value, unsigned char *p_checksum );
char sht_write_statusreg( unsigned char *p_value );
char sht_measure( unsigned char *p_value, unsigned char *p_checksum, unsigned char mode );
long Celsius2Fahrenheit( long tc );
long Fahrenheit2Celsius( long tf );
long Celsius2Kelvin( long tc );
long Kelvin2Celsius( long tf );

extern volatile unsigned int sht_measure_timer;
