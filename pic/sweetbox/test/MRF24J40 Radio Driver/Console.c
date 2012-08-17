/*********************************************************************
 *
 *                  Console Routines
 *
 *********************************************************************
 * FileName:        Console.c
 * Dependencies:
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     10/15/04    Original
 * Nilesh Rajbharti     11/1/04 Pre-release version
 * DF/KO                04/29/05 Microchip ZigBee Stack v1.0-2.0
 * DF/KO                07/18/05 Microchip ZigBee Stack v1.0-3.0
 * DF/KO                07/27/05 Microchip ZigBee Stack v1.0-3.1
 * DF/KO                08/19/05 Microchip ZigBee Stack v1.0-3.2
 * DF/KO                09/08/05 Microchip ZigBee Stack v1.0-3.3
 * DF/KO                01/09/06 Microchip ZigBee Stack v1.0-3.5
 * DF                   08/14/06 Microchip MiWi Stack v0.3
 ********************************************************************/
// Uncomment ENABLE_DEBUG line to enable debug mode for this file.
// Or you may also globally enable debug by defining this macro
// in zigbee.def file or from compiler command-line.

#include "Console.h"
#include "BasicDriverDefs.h"

#if defined( ENABLE_CONSOLE )

    #define USART_USE_BRGH_HIGH
    #if defined(USART_USE_BRGH_LOW)
        #define SPBRG_VAL   ( ((CLOCK_FREQ/BAUD_RATE)/64) - 1)
    #else
        #define SPBRG_VAL   ( ((CLOCK_FREQ/BAUD_RATE)/16) - 1)
    #endif
    
    #if SPBRG_VAL > 255
        #error "Calculated SPBRG value is out of range for currnet CLOCK_FREQ."
    #endif
    extern BOOL PrintMenus;
        
    void ConsoleInit(void)
    {
    #if defined(USART_USE_BRGH_HIGH)
        TXSTA = 0x24;
    #else
        TXSTA = 0x20;
    #endif
        RCSTA = 0x90; // 0b10010000;
        SPBRG = SPBRG_VAL;
    }
    
    void ConsolePutROMString(ROM char* str)
    {
        BYTE c;
    
        while( c = *str++ )
            ConsolePut(c);
        
    }
    
    
    BYTE ConsoleGetString( char *buffer, BYTE bufferLen )
    {
        BYTE v;
        BYTE count;
    
        count = 0;
        do
        {
            if ( bufferLen-- == 0 )
                break;
    
            while( !ConsoleIsGetReady() );
    
            v = RCREG;
    
            if ( v == '\r' || v == '\n' )
                break;
    
            count++;
            *buffer++ = v;
            *buffer = '\0';
        } while( 1 );
        
        return count;
    }
    
    void ConsolePut(BYTE c)
    {
        while( !ConsoleIsPutReady() );
        TXREG = c;
    }
    
    
    void ConsolePutString(BYTE *s)
    {
        BYTE c;
    
        while( (c = *s++) )
            ConsolePut(c);
    }
    
    
    BYTE ConsoleGet(void)
    {
        // Clear overrun error if it has occured
        // New bytes cannot be received if the error occurs and isn't cleared
        if(OERR)
        {
            CREN = 0;   // Disable UART receiver
            CREN = 1;   // Enable UART receiver
        }
    
        return RCREG;
    }
    
    
    ROM unsigned char CharacterArray[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    void PrintChar(BYTE toPrint)
    {
        BYTE PRINT_VAR;
        PRINT_VAR = toPrint;
        toPrint = (toPrint>>4)&0x0F;
        ConsolePut(CharacterArray[toPrint]);
        toPrint = (PRINT_VAR)&0x0F;
        ConsolePut(CharacterArray[toPrint]);
        return;
    }
    
    
    BYTE ConsoleInput(void)
    {
        BYTE in;
        
        while ( !ConsoleIsGetReady() );
        
        in = ConsoleGet();
        ConsolePut(in);
        if( PrintMenus )
        {
            nl();
        }
        return in;
    }

#endif

