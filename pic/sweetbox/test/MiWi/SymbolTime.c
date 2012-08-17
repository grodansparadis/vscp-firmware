/********************************************************************
* FileName:		SymbolTime.c
* Dependencies: SymbolTime.h
* Processor:	PIC18, PIC24F, PIC24H, dsPIC30, dsPIC33
*               tested with 18F4620, dsPIC33FJ256GP710	
* Hardware:		PICDEM Z, Explorer 16
* Complier:     Microchip C18 v3.04 or higher
*				Microchip C30 v2.03 or higher	
* Company:		Microchip Technology, Inc.
*
* Copyright and Disclaimer Notice
*
* Copyright © 2007-2008 Microchip Technology Inc.  All rights reserved.
*
* Microchip licenses to you the right to use, modify, copy and distribute 
* Software only when embedded on a Microchip microcontroller or digital 
* signal controller and used with a Microchip radio frequency transceiver, 
* which are integrated into your product or third party product (pursuant 
* to the terms in the accompanying license agreement). 
*
* You should refer to the license agreement accompanying this Software for 
* additional information regarding your rights and obligations.
*
* SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY 
* KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY 
* WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A 
* PARTICULAR PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE 
* LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, 
* CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY 
* DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO 
* ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, 
* LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, 
* TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT 
* NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
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
#include "SystemProfile.h"
#include ".\MiWi\SymbolTime.h"
#include "Compiler.h"
#include "GenericTypeDefs.h"

/************************ DEFINITIONS ******************************/

/************************ FUNCTION PROTOTYPES **********************/

/************************ VARIABLES ********************************/

volatile BYTE timerExtension1,timerExtension2;

/************************ FUNCTIONS ********************************/

/*********************************************************************
* Function:         void InitSymbolTimer()
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    none
*
* Side Effects:	    TMR0 for PIC18 is configured for calculating
*                   the correct symbol times.  TMR2/3 for PIC24/dsPIC
*                   is configured for calculating the correct symbol
*                   times
*
* Overview:		    This function will configure the UART for use at 
*                   in 8 bits, 1 stop, no flowcontrol mode
*
* Note:			    The timer interrupt is enabled causing the timer
*                   roll over calculations.  Interrupts are required
*                   to be enabled in order to extend the timer to
*                   4 bytes in PIC18.  PIC24/dsPIC version do not 
*                   enable or require interrupts
********************************************************************/
void InitSymbolTimer()
{
#if defined(__18CXX)
    T0CON = 0b00000000 | CLOCK_DIVIDER_SETTING;
    INTCON2bits.TMR0IP = 1;
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    T0CONbits.TMR0ON = 1;

    timerExtension1 = 0;
    timerExtension2 = 0;
#elif defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24H__)
    T2CON = 0b0000000000001000 | CLOCK_DIVIDER_SETTING;
    T2CONbits.TON = 1;
#elif defined(__PIC32MX__)
    CloseTimer2();
    WriteTimer2(0x00);
    WriteTimer3(0x00);
    WritePeriod3(0xFFFF);
    OpenTimer2((T2_ON|T2_32BIT_MODE_ON|CLOCK_DIVIDER_SETTING),0xFFFFFFFF);     
#else
    #error "Symbol timer implementation required for stack usage."
#endif
}


/*********************************************************************
* Function:         void InitSymbolTimer()
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    TICK - the current symbol time
*
* Side Effects:	    PIC18 only: the timer interrupt is disabled
*                   for several instruction cycles while the 
*                   timer value is grabbed.  This is to prevent a
*                   rollover from incrementing the timer extenders
*                   during the read of their values
*
* Overview:		    This function returns the current time
*
* Note:			    PIC18 only:
*                   caution: this function should never be called 
*                   when interrupts are disabled if interrupts are 
*                   disabled when this is called then the timer 
*                   might rollover and the byte extension would not 
*                   get updated.
********************************************************************/
TICK TickGet(void)
{
    TICK currentTime;

#if defined(__18CXX)
    
    /* disable the timer to prevent roll over of the lower 16 bits while before/after reading of the extension */
    INTCONbits.TMR0IE = 0;
    
    /* copy the byte extension */
    currentTime.byte.b2 = 0;
    currentTime.byte.b3 = 0;

    /* read the timer value */
    currentTime.byte.b0 = TMR0L;
    currentTime.byte.b1 = TMR0H;

    //if an interrupt occured after IE = 0, then we need to figure out if it was
    //before or after we read TMR0L
    if(INTCONbits.TMR0IF)
    {
        if(currentTime.byte.b0<10)
        {
            //if we read TMR0L after the rollover that caused the interrupt flag then we need
            //to increment the 3rd byte
            currentTime.byte.b2++;  //increment the upper most
            if(timerExtension1 == 0xFF)
            {
                currentTime.byte.b3++;
            }
        }
    }

    /* copy the byte extension */
    currentTime.byte.b2 += timerExtension1;
    currentTime.byte.b3 += timerExtension2;
    
    /* enable the timer*/
    INTCONbits.TMR0IE = 1;
    
#elif defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24H__) || defined(__PIC32MX__)
    currentTime.word.w0 = TMR2;
    currentTime.word.w1 = TMR3;
#else
    #error "Symbol timer implementation required for stack usage."
#endif
    return currentTime;
}
