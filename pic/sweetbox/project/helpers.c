/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  File: main.c
 *
 *  The SweetBox
 * 	Version information in version.h
 * 	akhe@eurosource.se
 *
 * Original From Microchip TCP/IP stack
 * 
 * Changes/additions Copyright (C) 2008-2011 Ake Hedman, eurosource, <akhe@eurosource.se>
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
#include "FreeRTOS.h"
#include <inttypes.h>
#include "helpers.h"

///////////////////////////////////////////////////////////////////////////////
// IPStr2Bin
//

uint32_t IPStr2Bin( char *sz, uint32_t* ip )
{
	uint32_t dw;
	char* p;
	char c;

    uint8_t index = 0;
	uint8_t acc = 0;
	dw = 0;
	p = sz;
	
	while ( ( c = *p++ ) ) 
	{
        if ( c == '.' )
		{
            *((uint8_t *)(&dw + index++ )) = acc;
            acc = 0;
        }
		else if ( c == ' ' ) {
			continue;
		}	
		else { 
            acc = acc*10 + (c - '0');
        }          
    }

    *((uint8_t *)(&dw + 3 )) = acc;
    
    return  dw;
}

///////////////////////////////////////////////////////////////////////////////
// Hex2Dec
//

uint8_t Hex2Dec( char c )
{
	// Convert to upper case
	if ( (c >= 'a' && c <= 'z') ) {
        c -= ('a' - 'A');
    }
	
	if ( c >= 'A' && c <= 'F' ) {
		return c - 'A' + 10;
	}	
	else if ( c >= '0' && c <= '9' ) {
		return c - '0';
	}	
	else {
		return 0;
	}	
}

///////////////////////////////////////////////////////////////////////////////
// MACStrToBin
//

void MACStrToBin( char *sz, uint8_t *pMAC )
{
	char *p;
	char c;

    uint8_t index = 0;
	uint8_t acc = 0;
	
	p = sz;
	
	while ( ( c = *p++ ) ) 
	{
		if ( ' ' == c ) {
			continue;
		}	

        if ( ( '-' == c ) || ( ':'== c )  )
		{
            pMAC[ index++ ] = acc;
            acc = 0;
        }
		else
		{	c = Hex2Dec(c);
            acc = ( acc * 16 ) + c;
		}
    }

    pMAC[ 5 ] = acc;
}

///////////////////////////////////////////////////////////////////////////////
// itoa
//

void itoa( uint16_t dat, char *str )
{
	uint16_t digit, strLen = 1;
	uint16_t divs = 10;
	
	while ( dat/divs )
	{
		strLen++;
		divs *= 10;
	}
	
	divs = 1;
	do
	{
		digit = dat % 10;
		str[ strLen-divs++ ] = digit + '0';
	}
	while ( (dat=dat/10) );
	str[ divs - 1 ] = 0; //null terminate the string
}


///////////////////////////////////////////////////////////////////////////////
// strupr
//

char *strupr( char *s )
{
    char c;
    char *p;

    p = s;
    while( (c = *p) )
    {
        if ( (c >= 'a' && c <= 'z') ) {
            *p -= ('a' - 'A');
        } 
           
        p++;
        
    }
    return s;
}

///////////////////////////////////////////////////////////////////////////////
// delay
//

void delay( uint32_t delay_count ) 
{
	uint32_t temp;

    portENTER_CRITICAL();
	for ( temp = 0; temp < delay_count; temp++ )
	{
		//****
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
	}
	portEXIT_CRITICAL();
}
