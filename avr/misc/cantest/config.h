//  File:       CONFIG.H
/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	2006-03-20
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 2006-2007 Ake Hedman, eurosource
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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <avr/io.h>

#define USE_UART1

// System clock in kHz
#define FOSC      16000  // Available value: 22118 16000 12000 11059 custom 
 
/*-------------- UART LIB CONFIGURATION ---------------*/
#define BAUDRATE    	9600    


#ifndef FALSE
#define FALSE            	0
#ifndef TRUE
#define TRUE                !FALSE
#endif
#endif

#ifndef ON
#define ON					!FALSE
#endif

#ifndef OFF
#define OFF					FALSE
#endif

#ifndef BIT
#define BIT(n)              (1 << (n))
#endif   

#endif

