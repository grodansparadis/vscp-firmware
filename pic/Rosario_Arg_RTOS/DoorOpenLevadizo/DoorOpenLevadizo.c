//*******************************************************************************                                                      
// File name: DoorOpenLevadizo.c                                         
//                                                                    
// Since:     2007-Marzo-07                                           
//                                                                    
// Version:   DoorOpenBatiente v1.0                            			          
//            Copyright (C) 2007, MAPito							                
//                                                                    
// Author:    Designed by Marcelo Poma                                
//                                                                    
//                                                                    
// Purpose:   Application Specific 																	  
//                                                                    
//                                                                    
// History:                                                           
// 2007/03/07 [MAP] Create this file                                  
//
// Some portions of code and ideas are taken from 
// VSCP (Very Simple Control Protocol) 
// http://www.vscp.org
// akhe@eurosource.se
// 
// Copyright (C) 1995-2005 Ake Hedman, eurosource
// 
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
// 
// This file is part of VSCP - Very Simple Control Protocol 	
// http://www.vscp.org
//******************************************************************************

#ifdef __ICCPIC18__
#else
#include <p18cxxx.h>
#endif
#include <TypeDefs.h>
#include <VSCP_Functions.h>
#include <VSCP_Class.h>
#include <VSCP_Type.h>
#include "Registers.h" 
#include "Application.h"

#if defined(__DEBUG)

#pragma config WDT = OFF
#pragma config OSC = HSPLL
#if defined(__18F248) || defined(__18F258)
#pragma config PWRT = OFF
#pragma config STVR = OFF
#endif
#if defined(__18F2480) || defined(__18F2580)
#pragma config BOR = BOACTIVE
#pragma config STVREN = ON
#endif
#pragma config BORV = 27
#pragma config LVP = OFF
#pragma config CPB = OFF

#else

#pragma config WDT = ON, WDTPS = 128
#pragma config OSC = HSPLL
#if defined(__18F248) || defined(__18F258)
#pragma config PWRT = ON
#pragma config STVR = ON	        // Stack overflow reset on
#endif
#if defined(__18F2480) || defined(__18F2580)
#pragma config BOR = BOACTIVE
#pragma config STVREN = ON		// Stack overflow reset on
#endif
#pragma config BORV = 27		// Brown out reset at 2.7V
#pragma config LVP = OFF		// Disable low voltage programming: RB5 General I/O
#pragma config CPB = ON			// 

#endif

//*******************************************************************************                                                      
// Init - Initialization Routine
// Checked MAPi
//*******************************************************************************                                                      
void DoorOpen_init_app()
{
	// Initialize data
	init_app_ram();
	init_app_eeprom();

	// Initialize the uP

	//  ADCON1: A/D CONTROL REGISTER 1
	//PCFG AN7 	AN6 AN5 AN4 AN3 	AN2 	AN1 AN0 VREF+ VREF- 
	//0000 A 		A 	A 	A 	A 		A 		A 	A 	VDD 	VSS 	
	//0001 A 		A 	A 	A 	VREF+ A 		A 	A 	AN3 	VSS 	
	//0010 D 		D 	D 	A 	A 		A 		A 	A 	VDD 	VSS 	
	//0011 D 		D 	D 	A 	VREF+ A 		A 	A 	AN3 	VSS 	
	//0100 D 		D 	D 	D 	A 		D 		A 	A 	VDD 	VSS 	
	//0101 D 		D 	D 	D 	VREF+ D 		A 	A 	AN3 	VSS 	
	//011x D 		D 	D 	D 	D 		D 		D 	D 	— — 	0/0																																																														
	//1000 A 		A 	A 	A 	VREF+ VREF- A 	A 	AN3 	AN2 																																																															
	//1001 D 		D 	A 	A 	A 		A 		A 	A 	VDD 	VSS 	          																																																															
	//1010 D 		D 	A 	A 	VREF+ A 		A 	A 	AN3 	VSS 	      																																																															
	//1011 D 		D 	A 	A 	VREF+ VREF- A 	A 	AN3 	AN2 	  																																																															
	//1100 D 		D 	D 	A 	VREF+ VREF- A 	A 	AN3 	AN2 	  																																																															
	//1101 D 		D 	D 	D 	VREF+ VREF- A 	A 	AN3 	AN2 	  																																																																																					
	//1110 D 		D 	D 	D 	D 		D 		D 	A 	VDD 	VSS 	          																																																																																					
	//1111 D 		D 	D 	D 	VREF+ VREF- D 	A 	AN3 	AN2 																																																																																																											
	ADCON1 = 0b00000111; 		//All Analog or Digital are configured as DIGITAL

	// PortA                        
	// RA0 - Digital / Analog 0 - Input
	// RA1 - Digital / Analog 1 - Input
	// RA2 - Digital / Analog 2 - Input
	// RA3 - Digital / Analog 3 - Input
	// RA4 - Relay  0 - Output
	// RA5 - Digital / Analog 4 - Input
	// RA6 - OSC
	TRISA = 0b11101111;                        

	// PortB
	// RB0 - INT0   	- Input 
	// RB1 - INT1   	- Input 
	// RB2 - CAN TX 	- Output 
	// RB3 - CAN RX 	- Input 
	// RB4 - InitButton - Input
	// RB5 - Status LED - Output  
	// RB6 - ICD2		- Input  
	// RB7 - ICD2		- Input  
	TRISB = 0b11011011;
	
	// RC0 - Relay  1 - Output
	// RC1 - Relay  2 - Output
	// RC2 - Relay  3 - Output
	// RC3 - Relay  4 - Output
	// RC4 - Relay  5 - Output
	// RC5 - Relay  6 - Output
	// RC6 - Tx RS232 - Output
	// RC7 - Rx RS232 - Input
	TRISC = 0b10000000;

	PORTAbits.RA4 = 0;	//Initialize Relay0 in off 
	PORTCbits.RC0 = 0;	//Initialize Relay1 in off 
	PORTCbits.RC1 = 0;	//Initialize Relay2 in off 
	PORTCbits.RC2 = 0;	//Initialize Relay3 in off 
	PORTCbits.RC3 = 0;	//Initialize Relay4 in off 
	PORTCbits.RC4 = 0;	//Initialize Relay5 in off  
	PORTCbits.RC5 = 0;	//Initialize Relay6 in off 
	PORTBbits.RB5 = 0;	//Initialize Status LED in off	

	return;
}

//*******************************************************************************                                                      
// init_app_ram
//
//*******************************************************************************                                                      
void init_app_ram( void )
{
}

//*******************************************************************************                                                      
// Init_App_Eeprom
//
//*******************************************************************************                                                      
void init_app_eeprom( void )
{
	
} 
  
  
  
  
  
  