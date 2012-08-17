/* ******************************************************************************
 *      VSCP (Very Simple Control Protocol)
 *      http://www.vscp.org
 *
 *  Copyright (C) 1995-2011 Ake Hedman, eurosource, <akhe@eurosource.se>
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
 *      This file is part of VSCP - Very Simple Control Protocol
 *      http://www.vscp.org
 *
 * ******************************************************************************
*/

#ifndef _VSCP_PROJDEFS_H
#define _VSCP_PROJDEFS_H


// System clock in kHz
#ifndef FOSC
#define FOSC                16000
#endif

// UART channel to use
//#define USE_UART1
//#define USE_UART2

// Baudrate
#ifndef BAUDRATE
#define BAUDRATE            38400
#endif

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


// VSCP Defines
// ============
//
// Described in vscp.h

// If GUID should be in EEPROM define the following
#define GUID_IN_EEPROM

// If manufacturing D and GUID should bw possible to write when page_select = 0xffff
// undef the following
#define ENABLE_WRITE_2PROTECTED_LOCATIONS

// Disable initial nickname discovery by undefining the following
//#define DISABLE_AUTOMATIC_NICKNAME_DISCOVERY

#endif /* _VSCP_PROJDEFS_H_ */
