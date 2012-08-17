// File:  main.h

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	2006-03-23
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

#ifndef VSCPTEST_H
#define VSCPTEST_H

#define THIS_MODULE_MAJOR_VERSION		0
#define THIS_MODULE_MINOR_VERSION		0
#define THIS_MODULE_SUBMINOR_VERSION	1

#define DESCION_MATRIX_ELEMENTS			64  # elements in DM
#define REG_DESCION_MATRIX				120  # Paged matrix

// Module registers
#define REG_ZONE						2    // Must be last used pos!!!!!!!!

// * * * Actions * * *
#define ACTION_NOOP						0
#define ACTION_ON						1
#define ACTION_OFF						2
#define ACTION_PULSE					3
// 4-9 reserved
#define ACTION_STATUS					10
// 11-15 reserved
#define ACTION_DISABLE					16

#endif