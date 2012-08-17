/*****************************************************************************
 *
 *              Main.c -- user main program
 *
 *****************************************************************************
 * FileName:        ConfigFuses.c
 * Dependencies:
 * Processor:       PIC18F
 * Compiler:        C18 02.20.00 or higher
 *                  PICC 9.50.00 or higher
 *                  C30 02.03.00 or higher
 * Linker:          MPLINK 03.40.00 or higher
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the "Company") is intended and supplied to you, the Company's
 * customer, for use solely and exclusively with products manufactured
 * by the Company.
 *
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *****************************************************************************/

#include "Compiler.h"

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
#elif defined(HI_TECH_C) && !defined(__PICC__)
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
	_CONFIG1(JTAGEN_OFF & FWDTEN_OFF & WDTPS_PS2)		// JTAG off, watchdog timer off
#elif defined(__dsPIC33F__) || defined(__PIC24H__)
	// Explorer 16 board
	_FOSCSEL(FNOSC_PRI);    //primary osc
	_FOSC(OSCIOFNC_OFF & POSCMD_XT)	// XT Osc
	_FWDT(FWDTEN_OFF & WDTPOST_PS2)				// Disable Watchdog timer
	// JTAG should be disabled as well
#elif defined(__dsPIC30F__)
	// dsPICDEM 1.1 board
	_FOSC(XT_PLL16)					// XT Osc + 16X PLL
	_FWDT(WDT_OFF & WDTPOST_PS2)					// Disable Watchdog timer
	_FBORPOR(MCLR_EN & PBOR_OFF & PWRT_OFF)
#endif
