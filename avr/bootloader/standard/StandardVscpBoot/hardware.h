///////////////////////////////////////////////////////////////////////////////
// File: hardware.h
// 
// Hardware dependent defines in this file.
//

/* ******************************************************************************
 * VSCP (Very Simple Control Protocol) 
 * http://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2010-2016 Sven Sven Zwiers
 * Copyright (c) 2016-2026 Ake Hedman, the VSCP Project <info@grodansparadis.com>
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

#ifndef _VSCP_STANDARD_BOOTLOADER_H_
#define _VSCP_STANDARD_BOOTLOADER_H_

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "vscp_projdefs.h"

// application / bootloader memory defines
// for AT90CAN128 with bootsize 4096 (bootsz=00)
// AFS = 0x0000 -> 0xEFFF which are 0xF000 words to program (= 1E000 bytes)
// if we use blocksize 0xFF, there are 0x1E000 div 0xFF blocks to send (= 0x1E1 = 481)
#ifdef AT90CAN128
#define VSCPBOOT_BLOCKSIZE	0x00FF
#define VSCPBOOT_BLOCKS	    0xFF    //so far 0xFF is maximum blocks possible
#define VSCPBOOT_START      0x1E000
#endif

// for AT90CAN32 with bootsize 4096 (bootsz=00)
// AFS = 0x0000 -> 0x2FFF which are 0x3000 words to program (=0x6000 bytes)
// if we use blocksize 0xFF, there are 0x6000 div 0xFF blocks to send (= 0x60 = 96)
#ifdef AT90CAN32
#define VSCPBOOT_BLOCKSIZE	0x00FF
#define VSCPBOOT_BLOCKS	    0x60
#define VSCPBOOT_START      0x6000
#endif


#endif