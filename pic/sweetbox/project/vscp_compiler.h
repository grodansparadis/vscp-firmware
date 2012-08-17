/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  File:vscp_compiler.h
 *
 *  SweetBox Module
 * 	Version information in version.h
 * 	akhe@eurosource.se
 *
 * Copyright (C) 1995-2011 Ake Hedman, eurosource, <akhe@eurosource.se>
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

#ifndef ____VSCP_COMPILER___
#define ____VSCP_COMPILER___

// Include proper device header file
// INSTR_FREQ is used to calculate Tick counter value, set UART 
// baud rate, etc. based on CLOCK_FREQ, defined in 
// (HardwareProfile.h)
#if defined(__18CXX)
	// All PIC18 processors
	// Microchip C18 compiler
	#include <p18cxxx.h>
	#define INSTR_FREQ			(CLOCK_FREQ/4)
#elif defined(__PIC24F__)	// Microchip C30 compiler
	// PIC24F processor
	#include <p24Fxxxx.h>
	#define INSTR_FREQ			(CLOCK_FREQ/2)
#elif defined(__PIC24H__)	// Microchip C30 compiler
	// PIC24H processor
	#include <p24Hxxxx.h>
	#define INSTR_FREQ			(CLOCK_FREQ/2)
#elif defined(__dsPIC33F__)	// Microchip C30 compiler
	// dsPIC33F processor
	#include <p33Fxxxx.h>
	#define INSTR_FREQ			(CLOCK_FREQ/2)
#elif defined(__dsPIC30F__)	// Microchip C30 compiler
	// dsPIC30F processor
	#include <p30fxxxx.h>
	#define INSTR_FREQ			(CLOCK_FREQ/4)
#elif defined(__PIC32MX__)
    #include <p32xxxx.h>
    #include <plib.h>
    #define INSTR_FREQ          (CLOCK_FREQ/4)
    #define __C32__
#else
	#error Unknown processor or compiler.  See Compiler.h
#endif

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#if defined(__18CXX)
    #define ROM                 	rom
	#define strcpypgm2ram(a, b)		strcpypgm2ram(a,(far rom char*)b)
#endif

#if defined(__18CXX)
	#define	__attribute__(a)

	#if defined(__18F8720) || defined(__18F87J10) || defined(__18F97J60)
	    #define TXSTAbits       	TXSTA1bits
	    #define TXREG           	TXREG1
	    #define TXSTA           	TXSTA1
	    #define RCSTA           	RCSTA1
	    #define SPBRG           	SPBRG1
	    #define RCREG           	RCREG1
		#define SPICON				SSP1CON1
		#define SPISTATbits			SPI1STATbits
	#endif

#elif defined(__C30__)
    #define ROM						const
	#define memcmppgm2ram(a,b,c)	memcmp(a,b,c)
	#define memcpypgm2ram(a,b,c)	memcpy(a,b,c)
	#define strcpypgm2ram(a, b)		strcpy(a,b)
	#define	strlenpgm(a)			strlen(a)
	#define strstrrampgm(a,b)		strstr(a,b)
	#define Reset()					asm("reset")

	#if defined(__dsPIC33F__) || defined(__PIC24H__)
		#define AD1PCFGbits			AD1PCFGLbits
		#define AD1CHS				AD1CHS0
	#elif defined(__dsPIC30F__)
		#define ADC1BUF0			ADCBUF0
		#define AD1CHS				ADCHS
		#define	AD1CON1				ADCON1
		#define AD1CON2				ADCON2
		#define AD1CON3				ADCON3
		#define AD1PCFGbits			ADPCFGbits
		#define AD1CSSL				ADCSSL
		#define AD1IF				ADIF
		#define AD1IE				ADIE
		#define _ADC1Interrupt		_ADCInterrupt
	#endif

#elif defined(__C32__)
    #define ROM                     const
    #define Nop()                   asm("NOP");
#endif



#endif


