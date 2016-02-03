/*********************************************************************
 *              ECAN Demo Code
 *********************************************************************
 * FileName:        ECANDemo.c
 * Dependencies:    ECAN.h
 * Processor:       PIC18FXX8X
 * Complier:        Microchip C 2.10.06 or higher
 * Company:         Microchip Technology, Inc.
 *
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 *  Any use in violation of the foregoing restrictions may subject the
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
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Caio Gubel           5/5/03  Version 1.0 - Initial Release
 * Nilesh R.            7/22/03 Improved
 *********************************************************************/

#include "ecan.h"

#if defined(MCHP_C18)
// Configuration settings for PICDEM CAN-LIN3 - This setting will depend
// on your hardware.
#pragma romdata CONFIG

_CONFIG_DECL(_OSC_EC_1H, \
             _CONFIG2L_DEFAULT, \
             _WDT_OFF_2H, \
             _CONFIG3L_DEFAULT, \
             _CONFIG3H_DEFAULT, \
             _LVP_OFF_4L & _DEBUG_OFF_4L, \
             _CONFIG5L_DEFAULT, \
             _CONFIG5H_DEFAULT, \
             _CONFIG6L_DEFAULT, \
             _CONFIG6H_DEFAULT, \
             _CONFIG7L_DEFAULT, \
             _CONFIG7H_DEFAULT);

#pragma romdata
#endif

#if defined(MCHP_C18)
    #define PORTB_RB4   PORTBbits.RB4

#elif defined(HITECH_C18)
    #define PORTB_RB4   RB4
#endif

void RunTimeInitialization(void);

void main(void)
{
    unsigned long id;
    BYTE data[4];
    BYTE dataLen;
    ECAN_RX_MSG_FLAGS flags;


    ECANInitialize();

    // This function is for illustration purpose only.
    // It shows how an application would perform run-time initialization.
    // Normally, ECANInitialize() would initialize  ECAN module
    // as per ECAN.def options.  You would do run-time initialization
    // only if the ECAN optoins need to be changed at run-time.
    //RunTimeInitialization();

    while( !ECANSendMessage(0x123, data, 0, ECAN_TX_STD_FRAME) );


    do
    {
        while( !ECANReceiveMessage(&id, data, &dataLen, &flags) )
        {
            // Pushed down RB4 will fill-up FIFO/buffers.
            while( PORTB_RB4 == 0 );
        }


        id++;
        while( !ECANSendMessage(id, data, dataLen, flags) );

        // Delay so that when we are transmitting contents of FIFO all at once,
        // CANKing can receive it correctly without overflowing its buffer.
        id = 0xafff;
        while( id-- );
    } while(1);
}

// Uncomment this block if get run-time initialization demo.
#if 0
void RunTimeInitialization(void)
{
    // Must be in Config mode to change many of settings.
    ECANSetOperationMode(ECAN_OP_MODE_CONFIG);

    // Select Mode 1
    ECANSetFunctionalMode(ECAN_MODE_1);


    // Make B0 as receiver.
    ECANSetBnTxRxMode(B0, ECAN_BUFFER_RX);

    // RXB0 will receive Stadard messages only.
    ECANSetRxBnRxMode(RXB0, ECAN_RECEIVE_ALL_VALID);

    // B0 will receive Extended
    ECANSetBnRxMode(B0, ECAN_RECEIVE_ALL_VALID);

    // Link RXF0 to RXB0 and RXF1 to B0
    ECANLinkRXF0F1ToBuffer(RXB0, B0);
    ECANLinkRXF0Thru3ToMask(ECAN_RXM0, ECAN_RXM1, ECAN_RXM0, ECAN_RXM0);

    ECANSetRXF0Value(0,         ECAN_MSG_STD);
    ECANSetRXF1Value(0,         ECAN_MSG_XTD);

    ECANSetRXM0Value(0,         ECAN_MSG_STD);
    ECANSetRXM1Value(0,         ECAN_MSG_XTD);

    // Set 125kbps @ 25MHz
    ECANSetBaudRate(2, 4, 8, 8, 8);

    // Return to Normal mode to communicate.
    ECANSetOperationMode(ECAN_OP_MODE_NORMAL);
}
#endif
