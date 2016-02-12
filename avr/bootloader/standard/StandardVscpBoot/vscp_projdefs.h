///////////////////////////////////////////////////////////////////////////////
// File: vscp_projdef.c
// 
// Project definitions
//

/* ******************************************************************************
 * VSCP (Very Simple Control Protocol) 
 * http://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2010-2016 Sven Zwiers
 * Copyright (c) 2016 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sub license, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 *	This file is part of VSCP - Very Simple Control Protocol 	
 *	http://www.vscp.org
 *
 * ******************************************************************************
 */

#ifndef VSCP_BOOTLOADER_PROJDEFS_H
#define VSCP_BOOTLOADER_PROJDEFS_H

#include <avr/io.h>

// Define processor
#define AT90CAN128
//#define AT90CAN64
//#define AT90CAN32

//
// Uncomment one of the following suitable for the
// platform you want to compile for or define your own 
// board and define it in hardware.c/h

#define VSCPBOOT_BOARD  ATMEL_STK501
//#define VSCPBOOT_BOARD    OLIMEX_AT90CAN128
//#define VSCPBOOT_BOARD    OLIMEX_AT90CAN32
//#define VSCPBOOT_BOARD    AAMINI0


// System clock in kHz
//defined in makefile
#define FOSC                16000  // Available value: 22118 16000 12000 11059 custom 
//#define FOSC                14300  // Available value: 22118 16000 12000 11059 custom 

// UART channel to use for debugging
#define USE_UART1 
//#define USE_UART2

// Baudrate for UART debug  channel
#define BAUDRAT             9600 
 




#endif
