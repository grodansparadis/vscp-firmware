/*

UART2 Driver Header File for PIC24.

********************************************************************************
 FileName:        uart2.c
 Dependencies:    HardwareProfile.h
 Processor:       PIC24
 Compiler:        MPLAB C30
 Linker:          MPLAB LINK30
 Company:         Microchip Technology Incorporated

Author                Date      Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Anton Alkhimenok   18-Oct-2005
KO                 11-Oct-2006  v1.0

********************************************************************************
Software License Agreement

Microchip Technology Inc. ("Microchip") licenses to you the right to use, copy,
modify and distribute the software - including source code - only for use with
Microchip microcontrollers or Microchip digital signal controllers; provided
that no open source or free software is incorporated into the Source Code
without Microchip’s prior written consent in each instance.

The software is owned by Microchip and its licensors, and is protected under
applicable copyright laws.  All rights reserved.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING
BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

********************************************************************************
*/

//******************************************************************************
// Function Prototypes
//******************************************************************************

char UART2GetChar();
void UART2Init();
char UART2IsPressed();
void UART2PrintString( char *str );
void UART2PutChar( char ch );
void UART2PutDec( unsigned char dec );
void UART2PutHex( int toPrint );

#if defined( __C30__ ) || defined( __PIC32MX__ )
void UART2PutHexWord( unsigned int toPrint );
#endif

