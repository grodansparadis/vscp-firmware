/********************************************************************
* FileName:		ConfigFuses.c
* Dependencies:    
* Processor:	PIC18, PIC24F, PIC24H, dsPIC30, dsPIC33
*               tested with 18F4620, dsPIC33FJ256GP710	
* Hardware:		PICDEM Z, Explorer 16
* Complier:     Microchip C18 v3.04 or higher
*				Microchip C30 v2.03 or higher	
* Company:		Microchip Technology, Inc.
*
* Copyright and Disclaimer Notice for MiWi Software:
*
* Copyright © 2006-2007 Microchip Technology Inc.  All rights reserved.
*
* Microchip licenses to you the right to use, modify, copy and 
* distribute Software only when embedded on a Microchip
* microcontroller or digital signal controller and used with a 
* Microchip radio frequency transceiver, which are integrated into
* your product or third party product (pursuant to the sublicense 
* terms in the accompanying license agreement).  
*
* You should refer to the license agreement accompanying this 
* Software for additional information regarding your rights 
* and obligations.
*
* SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION,
* ANY WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND
* FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL MICROCHIP OR
* ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE,
* STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL
* EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
* INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT,
* PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST
* OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
* CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
* THEREOF), OR OTHER SIMILAR COSTS.
*
*********************************************************************
* File Description:
*
*
*
* Change History:
*  Rev   Date         Description
*  0.1   11/09/2006   Initial revision
*  1.0   01/09/2007   Initial release
********************************************************************/

/************************ HEADERS **********************************/
#include "Compiler.h"

/************************ DEFINITIONS ******************************/
#if !defined(HI_TECH_C) && defined(__18F4620)
        #pragma romdata CONFIG1H = 0x300001
        const rom unsigned char config1H = 0b00000110;      // HSPLL oscillator
    
        #pragma romdata CONFIG2L = 0x300002
        const rom unsigned char config2L = 0b00011111;      // Brown-out Reset Enabled in hardware @ 2.0V, PWRTEN disabled
    
        #pragma romdata CONFIG2H = 0x300003
        const rom unsigned char config2H = 0b00001010;      // HW WD disabled, 1:32 prescaler
    
        #pragma romdata CONFIG3H = 0x300005
        const rom unsigned char config3H = 0b10000000;      // PORTB digital on RESET
    
        #pragma romdata CONFIG4L = 0x300006
        const rom unsigned char config4L = 0b10000001;      // DEBUG disabled,
                                                            // XINST disabled
                                                            // LVP disabled
                                                            // STVREN enabled
#elif defined(HI_TECH_C) && !defined(__PICC__) && defined(__18F4620)
    __CONFIG(1,HSPLL);
    __CONFIG(2,PWRTDIS & BORDIS & WDTDIS);
    __CONFIG(3,CCP2RB3);
    __CONFIG(4,STVRDIS & XINSTDIS & LVPDIS);
    __CONFIG(5,UNPROTECT);

#elif defined(__PICC__) && defined(_16F877A)
    __CONFIG(0x3F3A);
#elif defined(__PIC24F__)
	// Explorer 16 board
	_CONFIG2(FNOSC_PRI & POSCMOD_XT)		// Primary XT OSC with 4x PLL
	_CONFIG1(JTAGEN_OFF & FWDTEN_OFF)		// JTAG off, watchdog timer off
#elif defined(__dsPIC33F__) || defined(__PIC24H__)
	// Explorer 16 board
	_FOSCSEL(FNOSC_PRI);    //primary osc
	_FOSC(OSCIOFNC_OFF & POSCMD_XT)	// XT Osc
	_FWDT(FWDTEN_OFF)				// Disable Watchdog timer
	// JTAG should be disabled as well
#elif defined(__dsPIC30F__)
	// dsPICDEM 1.1 board
	_FOSC(XT_PLL16)					// XT Osc + 16X PLL
	_FWDT(WDT_OFF)					// Disable Watchdog timer
	_FBORPOR(MCLR_EN & PBOR_OFF & PWRT_OFF)
#endif

#if !defined(__18F4620) && defined(__18CXX)
    #error "Please update the configuration words for your device."
#endif
