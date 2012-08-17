/******************************************************************************
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  extract.h                                                       *
* Description : Headerfile for extract.c                                      *
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

#ifndef __EXTRACT_H_
#define __EXTRACT_H_

/*****************************************************************************/
#include "config.h"
#include "types.h"
/*****************************************************************************/

#if !(defined(HOST_LITTLE_ENDIAN)) && !(defined(HOST_BIG_ENDIAN))
#error Endianess undefined, see config.h
#elif defined(HOST_LITTLE_ENDIAN) && (defined(HOST_BIG_ENDIAN))
#error Endianess defined as little and big, see config.h
#endif

/*****************************************************************************/

#define end_conv16(x)  ((((euint16)(x) & 0xff00) >> 8) | \
                        (((euint16)(x) & 0x00ff) << 8))
#define end_conv32(x)  ((((euint32)(x) & 0xff000000) >> 24) | \
                       (((euint32)(x) & 0x00ff0000) >> 8)  | \
                       (((euint32)(x) & 0x0000ff00) << 8)  | \
                       (((euint32)(x) & 0x000000ff) << 24))

/*****************************************************************************/

#if defined(BYTE_ALIGNMENT)
	#define ex_getb16(buf) (*((euint16*)(buf)))
	#define ex_getb32(buf) (*((euint32*)(buf)))
        #define ex_setb16(buf,data) *((euint16*)(buf))=(data)
	#define ex_setb32(buf,data) *((euint32*)(buf))=(data)
#else
#if defined(HOST_LITTLE_ENDIAN)
        #define ex_getb16(buf) \
		((euint16)(*((euint8*)(buf)+1)<<8) + \
		 (euint16)(*((euint8*)(buf)+0)<<0))
        #define ex_getb32(buf) \
		((euint32)(*((euint8*)(buf)+3)<<24) + \
		 (euint32)(*((euint8*)(buf)+2)<<16) + \
		 (euint32)(*((euint8*)(buf)+1)<<8)  + \
		 (euint32)(*((euint8*)(buf)+0)<<0))
#elif defined(HOST_BIG_ENDIAN)
	#define ex_getb16(buf) \
		((euint16)(*((euint8*)(buf)+0)<<8) + \
		 (euint16)(*((euint8*)(buf)+1)<<0))
	#define ex_getb32(buf) \
		((euint32)(*((euint8*)(buf)+2)<<24) + \
		 (euint32)(*((euint8*)(buf)+3)<<16) + \
		 (euint32)(*((euint8*)(buf)+0)<<8)  + \
		 (euint32)(*((euint8*)(buf)+1)<<0))
#endif
	void ex_setb16(euint8* buf,euint16 data);
	void ex_setb32(euint8* buf,euint32 data);
#endif

#if defined(HOST_LITTLE_ENDIAN)
	#define ex_lth16(buf) ex_getb16(buf)
	#define ex_lth32(buf) ex_getb32(buf)
	#define ex_bth16(buf) end_conv16(ex_getb16(buf))
	#define ex_bth32(buf) end_conv32(ex_getb32(buf))
	
	#define ex_htl16(buf) ex_setb16(buf)
	#define ex_htl32(buf) ex_setb32(buf)
	#define ex_htb16(buf) ex_setb16(end_conv16(buf))
	#define ex_htb32(buf) ex_setb32(end_conv32(buf))
#elif defined(HOST_BIG_ENDIAN)
	#define ex_lth16(buf) end_conv16(ex_getb16(buf))
	#define ex_lth32(buf) end_conv32(ex_getb32(buf))
	#define ex_bth16(buf) ex_getb16(buf)
	#define ex_bth32(buf) ex_getb32(buf)
	
	#define ex_htl16(buf) ex_setb16(end_conv16(buf))
	#define ex_htl32(buf) ex_setb32(end_conv32(buf))
	#define ex_htb16(buf) ex_setb16(buf)
	#define ex_htb32(buf) ex_setb32(buf)
#endif

#endif

