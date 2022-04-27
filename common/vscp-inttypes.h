/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	https://www.vscp.org
 *
 *  Copyright (C) 1995-2018 Ake Hedman, Grodans Paradis AB, 
 *  akhe@paradiseofthefrog.com
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
 *	https://www.vscp.org
 *
 * ******************************************************************************
*/

/*
    Include in vscp_project.h if your project does not have inttypes.h
*/

#ifndef	_MACHTYPES_H_
#define	_MACHTYPES_H_

#ifndef __ICCPIC18__
#ifndef int64_t
	typedef long long       int64_t;
#endif
#endif

#ifndef int32_t
typedef long                int32_t;
#endif

#ifndef int16_t
typedef short               int16_t;
#endif

#ifndef int8_t
typedef char                int8_t;
#endif

#ifndef __ICCPIC18__
#ifndef uint64_t
	typedef unsigned long long 	uint64_t;
#endif
#endif

#ifndef uint32_t
typedef unsigned long      	uint32_t;
#endif

#ifndef uint16_t
typedef unsigned short     	uint16_t;
#endif

#ifndef uint8_t
typedef unsigned char       uint8_t;
#endif


#endif
