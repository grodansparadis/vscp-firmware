/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          ---------------------------------------------------------          *
*                                                                             *
* Filename :  types.h                                                         *
* Description : This file contains the crossplatform data types               *
*                                                                             *
* This library is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU Lesser General Public                  *
* License as published by the Free Software Foundation; either                *
* version 2.1 of the License, or (at your option) any later version.          *
*                                                                             *
* This library is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           *
* Lesser General Public License for more details.                             *
*                                                                             *
*                                                    (c)2005 Michael De Nil   *
*                                                    (c)2005 Lennart Yseboodt *
******************************************************************************/

#ifndef __EFS_TYPES_H__
#define __EFS_TYPES_H__

/*****************************************************************************/
#include "config.h"
/*****************************************************************************/

/* VARIABLE SIZES
   ==============

   * eint8   -> 1 byte
   * eint16  -> 2 bytes
   * eint32  -> 4 bytes
   *
   * VARSIZE_LINUX32 is suitable for 32bit Linux
   * VARSIZE_LINUX64 is suitable for 64bit Linux
   * VARSIZE_ATMEGA  is suitable for AVR's
   * VARSIZE_TMS67XX is suitable for the TI TMS67XX
   *
   * If nothing is selected a default is used, that should work on
   * 32 bit systems
   */

#if defined(VARSIZE_LINUX32)
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16;
	typedef long eint32;
	typedef signed long esint32;
	typedef unsigned long euint32;
#elif defined(VARSIZE_LINUX64)
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16;
	typedef int eint32;
	typedef signed int esint32;
	typedef unsigned int euint32;
#elif defined (VARSIZE_ATMEGA)
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16;
	typedef long eint32;
	typedef signed long esint32;
	typedef unsigned long euint32;
#elif defined(VARSIZE_TMS67XX)
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16;
	typedef int eint32;
	typedef signed int esint32;
	typedef unsigned int euint32;
#elif defined(VARSIZE_ARM_2K)
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16;
	typedef long eint32;
	typedef signed long esint32;
	typedef unsigned long euint32;
#else
#warning "No VARSIZE selection has been made, beware!"
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16;
	typedef long eint32;
	typedef signed long esint32;
	typedef unsigned long euint32;
#endif

#endif
