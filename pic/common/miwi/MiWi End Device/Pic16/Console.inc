;*********************************************************************
;* FileName:		Console.inc
;* Dependencies:    MiWiDefs.inc
;* Processor:   	PIC18F877A
;* Hardware:		PICDEM Z
;* Assembler:	    MPASMWIN 5.01 or higher
;* Linker:		    MPLINK 4.05 or higher
;* Company:		    Microchip Technology, Inc.
;*
;* Copyright and Disclaimer Notice for MiWi Software:
;*
;* Microchip Technology Inc. (“Microchip”) licenses this software to you 
;* solely for use with Microchip products, as described in the license 
;* agreement accompanying this software (“License”). The software is owned 
;* by Microchip, and is protected under applicable copyright laws.  
;* All rights reserved.
;* 
;* Distribution of this software (in object code or source code) is not 
;* permitted, except as described in Section 2 of the License.
;* 
;* SOFTWARE IS PROVIDED “AS IS.”  MICROCHIP EXPRESSLY DISCLAIM ANY 
;* WARRANTY OF ANY KIND, WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT 
;* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
;* PARTICULAR PURPOSE, OR NON-INFRINGEMENT. IN NO EVENT SHALL MICROCHIP 
;* BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, 
;* LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, 
;* TECHNOLOGY OR SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT 
;* LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, 
;* OR OTHER SIMILAR COSTS.
;*
;********************************************************************/
;* File Description:
;*   Provides UART functionality (optional)
;* 
;* Change History:
;*  Rev   Date         Description
;*  0.1   11/15/2006   Initial revision
;*  1.0   01/15/2007   Initial release
;********************************************************************/
#ifndef CONSOLE_H
#define CONSOLE_H

	#include "MiWiDefs.inc"
	
	#ifdef ENABLE_CONSOLE
	
		#ifdef CONSOLE_ASM
			GLOBAL ConsoleInit
			GLOBAL ConsolePut
			GLOBAL PrintChar
		#else
			EXTERN ConsoleInit
			EXTERN ConsolePut
			EXTERN PrintChar
		#endif
	
		;#define USART_USE_BRGH_HIGH
	
	#endif

#endif
