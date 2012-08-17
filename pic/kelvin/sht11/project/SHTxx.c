/* 
 * Author: MST
 * Copyright (c) Sensiron AG
 *
 * ============================================================================
 *           Changes Copyright (C) 2004,2005,206 Ake Hedman, eurosource
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

// $RCSfile: SHTxx.c,v $                                       
// $Date: 2006/03/23 19:57:43 $                                  
// $Author: akhe $                                              
// $Revision: 1.8 $ 

#include <p18cxxx.h>
#include <delays.h>
#include "shtxx.h"

const rom unsigned char CRC_Table[] = {
	0, 49, 98, 83, 196, 245,166, 151,185, 136,219, 234,125, 76, 31, 46, 67, 114,33, 16,
	135, 182,229, 212,250, 203,152, 169,62, 15, 92, 109,134, 183,228, 213,66, 115,32, 17,
	63, 14, 93, 108,251, 202,153, 168,197, 244,167, 150,1, 48, 99, 82, 124, 77, 30, 47,
	184, 137,218, 235,61, 12, 95, 110,249, 200,155, 170,132, 181,230, 215,64, 113,34, 19,
	126, 79, 28, 45, 186, 139,216, 233,199, 246,165, 148,3, 50, 97, 80, 187, 138,217, 232,
	127, 78, 29, 44, 2, 51, 96, 81, 198, 247,164, 149,248, 201,154, 171,60, 13, 94, 111,
	65, 112,35, 18, 133, 180,231, 214,122, 75, 24, 41, 190, 143,220, 237,195, 242,161, 144,
	7, 54, 101, 84, 57, 8, 91, 106,253, 204,159, 174,128, 177,226, 211,68, 117,38, 23,
	252, 205,158, 175,56, 9, 90, 107,69, 116,39, 22, 129, 176,227, 210,191, 142,221, 236,
	123, 74, 25, 40, 6, 55, 100, 85, 194, 243,160, 145,71, 118,37, 20, 131, 178,225, 208,
	254, 207,156, 173,58, 11, 88, 105,4, 53, 102, 87, 192, 241,162, 147,189, 140,223, 238,
	121, 72, 27, 42, 193, 240,163, 146,5, 52, 103, 86, 120, 73, 26, 43, 188, 141,222, 239,
	130, 179,224, 209,70, 119,36, 21, 59, 10, 89, 104,255, 206,157, 172 };

// Logarithm for h = 0 - 100 for ( log10( h ) - 2 )
const rom int log_Table[] = { 
	0, // zero - is not valid
	-2000, -1699, -1523, -1398, -1301, -1222, -1155,
	-1097, -1046, -1000, -959, -921, -886, -854,
	-824, -796, -770, -745, -721, -699, -678, -658,
	-638, -620, -602, -585, -569, -553, -538, -523,
	-509, -495, -481, -469, -456, -444, -432, -420,
	-409, -398, -387, -377, -367, -357, -347, -337,
	-328, -319, -310, -301, -292, -284, -276, -268,
	-260, -252, -244, -237, -229, -222, -215, -208,
	-201, -194, -187, -180, -174, -167, -161, -155,
	-149, -143, -137, -131, -125, -119, -114, -108,
	-102, -97, -92, -86, -81, -76, -71, -66, -60,
	-56, -51, -46, -41, -36, -32, -27, -22, -18
	-13, -9, -4, 0
};


//
//	This timer should be updated on 1 ms intervals
//	by a timer routine or simular.
//
volatile unsigned int sht_measure_timer;

///////////////////////////////////////////////////////////////////////////////
// calc_crc
//

unsigned char sht_calc_crc( unsigned char crc, unsigned char x )
{ 
	return CRC_Table[ ( x ^ crc ) ];
}

///////////////////////////////////////////////////////////////////////////////
// char sht_write_byte( unsigned char value )
//
// writes a byte on the Sensibus and checks the acknowledge 
//

char sht_write_byte( unsigned char value )
{ 
	unsigned char i;
 	unsigned char error= 0;  

	// Make RC5 output
	PORTCbits.RC5 = 1;					// DATA = 1
	TRISCbits.TRISC5 = 0;
	  	
	for ( i = 0x80; i>0; i/=2 ) {	// shift bit for masking

  		if ( i & value ) {          // masking value with i , write to SENSI-BUS
			PORTCbits.RC5 = 1;		// DATA = 1
		}
    	else { 
			PORTCbits.RC5 = 0;		// DATA = 0
		}

		PORTCbits.RC3 = 1;			// CLK = 1	

    	// pulswith approx. 5 us  	
		Delay10TCYx( 3 );
 
		PORTCbits.RC3 = 0;			// CLK = 0
  	}

	// Make RC5 input 
	PORTCbits.RC5 = 1;				// DATA = 1
	TRISCbits.TRISC5 = 1;

	PORTCbits.RC3 = 1;				// CLK = 1 clk #9 for ack
      
	Delay10TCYx( 1 );
	error = PORTCbits.RC5;			// check ack (DATA will be pulled down by SHT11)
 	
	PORTCbits.RC3 = 0;				// CLK = 0 

  	return error;                   // error=1 in case of no acknowledge
}


///////////////////////////////////////////////////////////////////////////////
// char sht_read_byte( unsigned char ack 
//
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1" 
//

char sht_read_byte( unsigned char ack )
{ 
	unsigned char i,val = 0;

	// Make RC5 input   
	TRISCbits.TRISC5 = 1;
	
  	for ( i = 0x80; i > 0; i /= 2 ) { 	// shift bit for masking
 
		PORTCbits.RC3 = 1;				// CLK = 1
		Delay10TCYx( 1 );

    	if ( PORTCbits.RC5 ) {
			val = (val | i );			// read bit  
		}

    	PORTCbits.RC3 = 0;				// CLK = 0  
					 
  	}
 
	// Make RC5 output   
	TRISCbits.TRISC5 = 0;

	// in case of "ack==1" pull down DATA-Line
	if ( ack ) {
		PORTCbits.RC5 = 0;				// DATA = 0
	}
	else {
		PORTCbits.RC5 = 1;				// DATA = 1
	}

	PORTCbits.RC3 = 1;					// CLK = 1 - clk #9 for ack

  	// pulswith approx. 5 us  	
	//Delay10TCYx( 3 );

  	PORTCbits.RC3 = 0;					// CLK = 0	

	// Make RC5 input   
	TRISCbits.TRISC5 = 1;					    

 	return val;
}


///////////////////////////////////////////////////////////////////////////////
// void sht_transstart( void )
//
// generates a transmission start 
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
//

void sht_transstart( void )
{  
	// Make RC5 output   
	PORTCbits.RC5 = 1;					// DATA = 1
	TRISCbits.TRISC5 = 0;

	PORTCbits.RC3 = 0;					// CLK = 0
   	Delay10TCYx( 1 );

   	PORTCbits.RC3 = 1;					// CLK = 1
   	Delay10TCYx( 1 );

   	PORTCbits.RC5 = 0;					// DATA = 0
   	Delay10TCYx( 1 );

   	PORTCbits.RC3 = 0;					// CLK = 0    	
	Delay10TCYx( 1 );					// pulswith approx. 5 us

   	PORTCbits.RC3 = 1;					// CLK = 1
   	Delay10TCYx( 1 );

   	PORTCbits.RC5 = 1;					// DATA = 1		   
   	Delay10TCYx( 1 );

   	PORTCbits.RC3 = 0;					// CLK = 0		   

	// Make RC5 input   
	TRISCbits.TRISC5 = 1;
}

///////////////////////////////////////////////////////////////////////////////
// void sht_connectionreset( void )
//
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
//

void sht_connectionreset( void )
{  
	unsigned char i; 

	// Make RC5 input  
	TRISCbits.TRISC5 = 1;
	PORTCbits.RC5 = 1;					// DATA = 1	
	PORTCbits.RC3 = 0;					// CLK = 0

  	for( i = 0; i < 9; i++ ) {          // 9 SCK cycles
		PORTCbits.RC3 = 1;				// CLK = 1
    	PORTCbits.RC3 = 0;				// CLK = 0
  	}

  	sht_transstart();                 	// transmission start
}

///////////////////////////////////////////////////////////////////////////////
// char sht_softreset( void )
//
// resets the sensor by a softreset 
//

char sht_softreset( void )
{ 
  	unsigned char error=0;  

  	sht_connectionreset();            		// reset communication
  	error += sht_write_byte( RESET );     	// send RESET-command to sensor

  	return error;                     		// error=1 in case of no response form the sensor
}

///////////////////////////////////////////////////////////////////////////////
// char sht_read_statusreg( unsigned char *p_value, unsigned char *p_checksum )
//
// reads the status register with checksum (8-bit)
//

char sht_read_statusreg( unsigned char *p_value, unsigned char *p_checksum )
{ 
  	unsigned char error = 0;

  	sht_transstart();                   	// transmission start
  	error = sht_write_byte( STATUS_REG_R ); // send command to sensor
  	*p_value = sht_read_byte( ACK );        // read status register (8-bit)
  	*p_checksum = sht_read_byte( noACK );   // read checksum (8-bit)  

  	return error;                     		// error=1 in case of no response form the sensor
}

///////////////////////////////////////////////////////////////////////////////
// char sht_write_statusreg( unsigned char *p_value )
//
// writes the status register with checksum (8-bit)
//

char sht_write_statusreg( unsigned char *p_value )
{ 
  	unsigned char error = 0;

  	sht_transstart();                   		// transmission start
  	error += sht_write_byte( STATUS_REG_W );	// send command to sensor
  	error += sht_write_byte( *p_value );    	// send value of status register

  	return error;                     		// error>=1 in case of no response form the sensor
}
 							   
///////////////////////////////////////////////////////////////////////////////
// char sht_measure( unsigned char *p_value, unsigned char *p_checksum, unsigned char mode )
//
// makes a measurement (humidity/temperature) with checksum
//

char sht_measure( unsigned char *p_value, unsigned char *p_checksum, unsigned char mode )
{ 
 	unsigned error=0;
  	unsigned long i;

  	sht_transstart();                  	// transmission start

  	switch( mode ) {                    // send command to sensor

    	case SHT_MODE_TEMP: 
			error += sht_write_byte( MEASURE_TEMP ); 
			break;

    	case SHT_MODE_HUMI: 
			error += sht_write_byte( MEASURE_HUMI ); 
			break;

    	default: 
			break;	 
  	}

	// wait until sensor has finished the measurement
	sht_measure_timer = 0;
	while ( sht_measure_timer < SHT_READ_TIMEOUT ) {
		if ( 0 == ( PORTCbits.RC5 ) ) {
			break; 
		}		
	}

  	if ( PORTCbits.RC5 ) {       		// or timeout (~2 sec.) is reached
		error += 1; 
	}

  	*(p_value)  = sht_read_byte( ACK );  	// read the first byte (MSB)
  	*(p_value+1)= sht_read_byte( ACK );  	// read the second byte (LSB)
  	*p_checksum = sht_read_byte( noACK );	// read checksum

  	return error;
}

///////////////////////////////////////////////////////////////////////////////
// Celsius2Fahrenheit
//

long Celsius2Fahrenheit( long tc )
{
	return ( ( 9 * tc + 16000 ) / 5 );
}

///////////////////////////////////////////////////////////////////////////////
// Fahrenheit2Celsius
//

long Fahrenheit2Celsius( long tf )
{
	return ( (5 * tf - 16000 ) / 9);
}


///////////////////////////////////////////////////////////////////////////////
// Celsius2Kelvin
//

long Celsius2Kelvin( long tc )
{
	return ( tc + 27316 );
}

///////////////////////////////////////////////////////////////////////////////
// Kelvin2Celsius
//

long Kelvin2Celsius( long tf )
{
	return ( tf - 27316 );
}
 

