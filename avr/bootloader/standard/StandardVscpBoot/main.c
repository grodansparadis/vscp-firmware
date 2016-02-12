///////////////////////////////////////////////////////////////////////////////
// File: main.c
// 
// Implements the VSCP standard boot loader for AVR
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
 *  This file is part of VSCP - Very Simple Control Protocol 	
 *	http://www.vscp.org
 *
 * ******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <vscp_firmware.h>
#include <vscp_bootloader.h>

//#include "vscp_registers.h"

#include "vscp_projdefs.h"
#include "vscp_compiler.h"
#include "hardware.h"
//#include "version.h"

#define CRC16
#include "crc.h"

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/boot.h>
#include <avr/wdt.h>
//#include <util/delay.h>

// Externals
extern uint16_t vscpboot_timer;
extern uint8_t vscpboot_nickname;



///////////////////////////////////////////////////////////////////////////////
// main
//

int main( void )
{
    vscpboot_init();        // Initialize boot code helpers
    init_hardware();        // Initialize hardware
    
    // if init. button is down when a board starts we force 
    // bootloader start. We also set bootloader flags and
    // id to 0xFE
    if ( 0 == vscpboot_readInitButton()  )  {
        vscpboot_setBootFlag( VSCPBOOT_FLAG_BOOT );
        vscpboot_setNickname( VSCPBOOT_NICKNAME_BOOT );
        vscpboot_loader();   // Will not return    
    }

    if ( vscpboot_getBootFlag() ) {
        // We should enter bootloader
        vscpboot_loader();   // Will not return
    }
    else {
        // We should start application code
        asm volatile("jmp 0x00" "\n\t");
    }

	while (1);  // should not be reached

}








