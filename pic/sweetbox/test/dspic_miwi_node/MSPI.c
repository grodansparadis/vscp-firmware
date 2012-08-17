/********************************************************************
* FileName:		MSPI.c
* Dependencies: MSPI.h
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
*   Configures and enables usage of the SPI ports
*
* Change History:
*  Rev   Date         Description
*  0.1   11/09/2006   Initial revision
*  1.0   01/09/2007   Initial release
********************************************************************/

/************************ HEADERS **********************************/
#include "Compiler.h"
#include "GenericTypeDefs.h"

#if defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24H__) || defined(__PIC32MX__)

/************************ FUNCTIONS ********************************/

/*********************************************************************
* Function:         void SPIPut(BYTE v)
*
* PreCondition:     SPI has been configured 
*
* Input:		    v - is the byte that needs to be transfered
*
* Output:		    none
*
* Side Effects:	    SPI transmits the byte
*
* Overview:		    This function will send a byte over the SPI
*
* Note:			    None
********************************************************************/
void SPIPut(BYTE v)
{
    BYTE dummy;
    
    #if defined(__PIC32MX__)
        putcSPI1(v);
        dummy = (BYTE)getcSPI1();
    #else
        IFS0bits.SPI1IF = 0;
        dummy = SPI1BUF;
        SPI1BUF = v;
        while(IFS0bits.SPI1IF == 0){}
    #endif
}

/*********************************************************************
* Function:         BYTE SPIGet(void)
*
* PreCondition:     SPI has been configured 
*
* Input:		    none
*
* Output:		    BYTE - the byte that was last received by the SPI
*
* Side Effects:	    none
*
* Overview:		    This function will read a byte over the SPI
*
* Note:			    None
********************************************************************/
BYTE SPIGet(void)
{
    #if defined(__PIC32MX__)
        BYTE dummy;
        putcSPI1(0x00);
        dummy = (BYTE)getcSPI1();
        return(dummy);
    #else
        SPIPut(0x00);
        return SPI1BUF;
    #endif
}

#elif defined(__18CXX)

/************************ FUNCTIONS ********************************/
 
/*********************************************************************
* Function:         void SPIPut(BYTE v)
*
* PreCondition:     SPI has been configured 
*
* Input:		    v - is the byte that needs to be transfered
*
* Output:		    none
*
* Side Effects:	    SPI transmits the byte
*
* Overview:		    This function will send a byte over the SPI
*
* Note:			    None
********************************************************************/
 void SPIPut(BYTE v)
{
    PIR1bits.SSPIF = 0;
    do
    {
        SSPCON1bits.WCOL = 0;
        SSPBUF = v;
    } while( SSPCON1bits.WCOL );

    while( PIR1bits.SSPIF == 0 );
}

/*********************************************************************
* Function:         BYTE SPIGet(void)
*
* PreCondition:     SPI has been configured 
*
* Input:		    none
*
* Output:		    BYTE - the byte that was last received by the SPI
*
* Side Effects:	    none
*
* Overview:		    This function will read a byte over the SPI
*
* Note:			    None
********************************************************************/
BYTE SPIGet(void)
{
    SPIPut(0x00);
    return SSPBUF;
}

#else
    #error Unknown processor.  See Compiler.h
#endif




