/*****************************************************************************
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the "Company") is intended and supplied to you, the Company's
 * customer, for use solely and exclusively with products manufactured
 * by the Company.
 *
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *****************************************************************************/

//application specific (for demo purposes only)
#define BAUD_RATE 19200
#define ENABLE_CONSOLE


//IEEE EUI - globally unique number
#define EUI_7 0x00
#define EUI_6 0x01
#define EUI_5 0x02
#define EUI_4 0x03
#define EUI_3 0x04
#define EUI_2 0x05
#define EUI_1 0x06
#define EUI_0 0x07

#define RF_INT_PIN RB0

#if defined(__18F4620)  // PICDEM Z board
    #define CLOCK_FREQ 16000000
    // Transceiver Configuration
    #define RFIF INT0IF
    #define RFIE INT0IE
    #define PHY_CS LATC0
    #define PHY_CS_TRIS TRISC0
    #define PHY_RESETn LATC2
    #define PHY_RESETn_TRIS TRISC2
    #define PHY_WAKE LATC1
    #define PHY_WAKE_TRIS TRISC1
#else       // PICDEM Z plus board    
    #define CLOCK_FREQ      32000000
    // Transceiver Configuration
    #define RFIF            INT0IF
    #define RFIE            INT0IE
    #define PHY_CS          LATD7
    #define PHY_CS_TRIS     TRISD7
    #define PHY_RESETn      LATD3
    #define PHY_RESETn_TRIS TRISD3
    #define PHY_WAKE        LATD2
    #define PHY_WAKE_TRIS   TRISD2   
#endif


#define PA_LEVEL 0x00  // -0.00 dBm
#define FREQUENCY_BAND 2400
