/*

UART2 Driver File for PIC24.

********************************************************************************
 FileName:        uart2.c
 Dependencies:    HardwareProfile.h
 Processor:       PIC24
 Compiler:        MPLAB C30
 Linker:          MPLAB LINK30
 Company:         Microchip Technology Incorporated

Author                Date      Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
KO                 12-Feb-2008  Modified to use HardwareProfile.h
KO                 11-Oct-2006  v1.0
Anton Alkhimenok   18-Oct-2005

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

#include "Compiler.h"
#include "HardwareProfile.h"
#include "uart2.h"

//******************************************************************************
// Constants
//******************************************************************************

//U2BRG register value and baudrate mistake calculation

#if defined (__C30__)
    #define BAUDRATEREG2        (((GetSystemClock()/2)+(BRG_DIV2/2*BAUDRATE2))/BRG_DIV2/BAUDRATE2-1)
#elif defined (__PIC32MX__)
    #define BAUDRATEREG2        ((GetPeripheralClock()+(BRG_DIV2/2*BAUDRATE2))/BRG_DIV2/BAUDRATE2-1)
#else
    #error Cannot calculate BRG value
#endif    

#if defined (__C30__)
    #define BAUD_ACTUAL         ((GetSystemClock()/2)/BRG_DIV2/(BAUDRATEREG2+1))
#elif defined (__PIC32MX__)
    #define BAUD_ACTUAL         (GetPeripheralClock()/BRG_DIV2/(BAUDRATEREG2+1))
#else
    #error Cannot calculate actual baud rate
#endif    

#define BAUD_ERROR              ((BAUD_ACTUAL > BAUDRATE2) ? BAUD_ACTUAL-BAUDRATE2 : BAUDRATE2-BAUD_ACTUAL)
#define BAUD_ERROR_PRECENT      ((BAUD_ERROR*100+BAUDRATE2/2)/BAUDRATE2)

#if (BAUD_ERROR_PRECENT > 3)
    #error UART frequency error is worse than 3%
#elif (BAUD_ERROR_PRECENT > 2)
    #warning UART frequency error is worse than 2%
#endif

/*******************************************************************************
Function: UART2GetChar

Precondition:
    UART2Init must be called prior to calling this routine.

Overview:
    This routine waits for a byte to be received.  It then returns that byte.

Input: None.

Output: Byte received.

*******************************************************************************/
char UART2GetChar()
{
    char Temp;

    while(IFS1bits.U2RXIF == 0);

    Temp = U2RXREG;
    IFS1bits.U2RXIF = 0;
    return Temp;
}

/*******************************************************************************
Function: UART2Init

Precondition: None.

Overview:
    This routine sets up the UART2 module.

Input: None.

Output: None.

Notes:
    Allow the peripheral to set the I/O pin directions.  If we set the TRIS
    bits manually, then when we disable the UART, the shape of the stop bit
    changes, and some terminal programs have problems.
*******************************************************************************/
void UART2Init()
{
    U2BRG = BAUDRATEREG2;
    U2MODE = 0;
    U2MODEbits.BRGH = BRGH2;
    U2STA = 0;
    U2MODEbits.UARTEN = 1;
    U2STAbits.UTXEN = 1;
    IFS1bits.U2RXIF = 0;
    
    #if defined (__PIC32MX__)
        U2STAbits.URXEN = 1;
    #endif
}

/*******************************************************************************
Function: UART2IsPressed

Precondition:
    UART2Init must be called prior to calling this routine.

Overview:
    This routine checks to see if there is a new byte in UART reception buffer.

Input: None.

Output:
    0 : No new data received.
    1 : Data is in the receive buffer

*******************************************************************************/
char UART2IsPressed()
{
    if(IFS1bits.U2RXIF == 1)
        return 1;
    return 0;
}

/*******************************************************************************
Function: UART2PrintString

Precondition:
    UART2Init must be called prior to calling this routine.

Overview:
    This function prints a string of characters to the UART.

Input: Pointer to a null terminated character string.

Output: None.

*******************************************************************************/
void UART2PrintString( char *str )
{
    unsigned char c;

    while( (c = *str++) )
        UART2PutChar(c);
}

/*******************************************************************************
Function: UART2PutChar

Precondition:
    UART2Init must be called prior to calling this routine.

Overview:
    This routine writes a character to the transmit FIFO, and then waits for the
    transmit FIFO to be empty.

Input: Byte to be sent.

Output: None.

*******************************************************************************/
void UART2PutChar( char ch )
{
    U2TXREG = ch;
    #if !defined(__PIC32MX__)
        Nop();
    #endif
    while(U2STAbits.TRMT == 0);
}

/*******************************************************************************
Function: UART2PutDec

Precondition:
    UART2Init must be called prior to calling this routine.

Overview:
    This function converts decimal data into a string and outputs it to UART.

Input: Binary data.

Output: None.

*******************************************************************************/
void  UART2PutDec(unsigned char dec)
{
    unsigned char res;

    res = dec;

    if (res/100)
    {
        UART2PutChar( res/100 + '0' );
    }
    res = res - (res/100)*100;

    if (res/10)
    {
        UART2PutChar( res/10 + '0' );
    }
    res = res - (res/10)*10;

    UART2PutChar( res + '0' );
}

/*******************************************************************************
Function: UART2PutHex

Precondition:
    UART2Init must be called prior to calling this routine.

Overview:
    This function converts hex data into a string and outputs it to UART.

Input: Binary data.

Output: None.

*******************************************************************************/

const unsigned char CharacterArray[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void UART2PutHex( int toPrint )
{
    int printVar;

    printVar = toPrint;
    toPrint = (toPrint>>4) & 0x0F;
    UART2PutChar( CharacterArray[toPrint] );

    toPrint = printVar & 0x0F;
    UART2PutChar( CharacterArray[toPrint] );

    return;
}

#if defined( __C30__ ) || defined( __PIC32MX__ )
void UART2PutHexWord( unsigned int toPrint )
{
    unsigned int printVar;

    printVar = (toPrint>>12) & 0x0F;
    UART2PutChar( CharacterArray[printVar] );

    printVar = (toPrint>>8) & 0x0F;
    UART2PutChar( CharacterArray[printVar] );

    printVar = (toPrint>>4) & 0x0F;
    UART2PutChar( CharacterArray[printVar] );

    printVar = toPrint & 0x0F;
    UART2PutChar( CharacterArray[printVar] );

    return;
}
#endif

