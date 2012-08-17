/*********************************************************************
 *
 *                  Master SPI routintes
 *
 *********************************************************************
 * FileName:        MSPI.c
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
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     7/12/04 Original
 * Nilesh Rajbharti     11/1/04 Pre-release version
 * DF/KO                04/29/05 Microchip ZigBee Stack v1.0-2.0
 * DF/KO                07/18/05 Microchip ZigBee Stack v1.0-3.0
 * DF/KO                07/27/05 Microchip ZigBee Stack v1.0-3.1
 * DF/KO                01/09/06 Microchip ZigBee Stack v1.0-3.5
 ********************************************************************/
#include "MSPI.h"

void SPIPut(BYTE v)
{
    SSPIF = 0;
    do
    {
        WCOL = 0;
        SSPBUF = v;
    } while( WCOL );

    while( SSPIF == 0 );
}

BYTE SPIGet(void)
{
    SPIPut(0x00);
    return SSPBUF;
}

// The ZMD44101 requires SS to be driven high between reading bytes,
// so these functions are not used and should not be compiled.
#if !defined(USE_ZMD44101)
void SPIGetArray(BYTE *buffer, BYTE len)
{
    while( len-- )
        *buffer++ = SPIGet();
}

void SPIPutArray(BYTE *buffer, BYTE len)
{
    while( len-- )
        SPIPut(*buffer++);
}
#endif



