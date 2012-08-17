/*********************************************************************
 *
 *                  Compiler and hardware specific definitions
 *
 *********************************************************************
 * FileName:        Compiler.h
 * Dependencies:    None
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F
 * Complier:        Microchip C18 v3.02 or higher
 *					Microchip C30 v2.01 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * This software is owned by Microchip Technology Inc. ("Microchip") 
 * and is supplied to you for use exclusively as described in the 
 * associated software agreement.  This software is protected by 
 * software and other intellectual property laws.  Any use in 
 * violation of the software license may subject the user to criminal 
 * sanctions as well as civil liability.  Copyright 2006 Microchip
 * Technology Inc.  All rights reserved.
 *
 * This software is provided "AS IS."  MICROCHIP DISCLAIMS ALL 
 * WARRANTIES, EXPRESS, IMPLIED, STATUTORY OR OTHERWISE, NOT LIMITED 
 * TO MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
 * INFRINGEMENT.  Microchip shall in no event be liable for special, 
 * incidental, or consequential damages.
 *
 *
 * Author               Date    	Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder		10/03/2006	Original, copied from old Compiler.h
 ********************************************************************/
#ifndef __COMPILER_H
#define __COMPILER_H

// Include proper device header file
// INSTR_FREQ is used to calculate Tick counter value, set UART 
// baud rate, etc. based on CLOCK_FREQ, defined in 
// (HardwareProfile.h)
#if defined(__18CXX) || defined(HI_TECH_C)	
	// All PIC18 processors
	#if defined(HI_TECH_C)	// HI TECH PICC-18 compiler
		#define __18CXX
		#include <pic18.h>
	#else					// Microchip C18 compiler
	    #include <p18cxxx.h>
	#endif
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
#else
	#error Unknown processor or compiler.  See Compiler.h
#endif

#include "HardwareProfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__18CXX) && !defined(HI_TECH_C)
    #define ROM                 	rom
	#define strcpypgm2ram(a, b)		strcpypgm2ram(a,(far rom char*)b)
#elif defined(__18CXX) && defined(HI_TECH_C)
    #define ROM                 	const
	#define rom
    #define Nop()               	asm("NOP");
    #define Reset()					asm("RESET");
	#define memcmppgm2ram(a,b,c)	memcmp(a,b,c)
	#define memcpypgm2ram(a,b,c)	memcpy(a,b,c)
	#define strcpypgm2ram(a, b)		strcpy(a,b)
	#define strcpypgm2ram(a, b)		strcpy(a,b)
	#define	strlenpgm(a)			strlen(a)
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
#endif



#endif
