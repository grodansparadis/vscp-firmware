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
 * Copyright (C) 2008-2011 Ake Hedman, eurosource, <akhe@eurosource.se>
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

#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <inttypes.h>

/*!
    // Convert dot-address string to big-endian dword
*/
uint32_t IPStr2Bin( char *sz, uint32_t* ip );

/*!
    Convert one hexadecimal character to its value
    \param c Character to convert.
    \return Converted value.
*/
uint8_t Hex2Dec( char c );

/*!
    Converts string of hex numbers to byte array in Big Endian
    \param sz String to convert
    \param pMac Array that will hold result in big endian format.
*/
void MACStrToBin( char *sz, uint8_t *pMAC );

/*!
    Convert unsigned integer to string
    \param dat Integer to convert.
    \param str Resulting string.
*/
void itoa( uint16_t dat, char *str );

/*!
    Convert string to upper case
    \param s pointer to string that should be converted.
    \return Pointer to converted string.
*/
char *strupr( char *s );

/*!
    Simple delay. Should be used for VERY short delays (microseconds)
    \param Pseudo delay time
*/
void delay( uint32_t delay_count );

#endif  // __HELPERS_H__

