/*********************************************************************
 *                                                                    
 * Software License Agreement                                         
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
 *********************************************************************/
#ifndef _P2P_DEFS_H_
#define _P2P_DEFS_H_

/*********************************************************************/
// ENABLE_CONSOLE will enable the print out on the hyper terminal
// this definition is very helpful in the debugging process
/*********************************************************************/
#define ENABLE_CONSOLE

/*********************************************************************/
// following codes defines the platforms as well as the hardware 
// configuration
/*********************************************************************/
#if defined(__18CXX)
    #define PICDEMZ
#endif

#if defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24H__) || defined(__PIC32MX__)
    #define EXPLORER16
#endif

#if defined(PICDEMZ)
    #if defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__dsPIC24F__) || defined(__dsPIC24H__) || defined(__PIC32MX__)
        #error "Incorrect board/processor combination."
    #endif
#endif

#if defined(EXPLORER16)
    #if defined(__18CXX)
        #error "Incorrect board/processor combination."
    #endif
#endif

#if defined(PICDEMZ)
    #define CLOCK_FREQ 16000000
    // Transceiver Configuration
    #define RFIF INTCONbits.INT0IF
    #define RFIE INTCONbits.INT0IE
    #define PHY_CS LATCbits.LATC0
    #define PHY_CS_TRIS TRISCbits.TRISC0
    #define PHY_RESETn LATCbits.LATC2
    #define PHY_RESETn_TRIS TRISCbits.TRISC2
    #define PHY_WAKE LATCbits.LATC1
    #define PHY_WAKE_TRIS TRISCbits.TRISC1
    
    #define PUSH_BUTTON_1 PORTBbits.RB5
    #define PUSH_BUTTON_2 PORTBbits.RB4
    #define LED_1 PORTAbits.RA0
    #define LED_2 PORTAbits.RA1
    
    #define PUSH_BUTTON_1_TRIS TRISB5
    #define PUSH_BUTTON_2_TRIS TRISB4
    #define LED_1_TRIS TRISA0
    #define LED_2_TRIS TRISA1
    
    #define RF_INT_PIN PORTBbits.RB0
    #define TMRL TMR0L
#endif
#if defined(EXPLORER16)
    
    #if defined(__PIC32MX__)
        #define CLOCK_FREQ 64000000    
        #define RFIF IFS0bits.INT1IF
        #define RFIE IEC0bits.INT1IE
    #else
        #define CLOCK_FREQ 8000000
        #define RFIF IFS1bits.INT1IF
        #define RFIE IEC1bits.INT1IE
    #endif
    
    #if defined(__PIC24F__)    
        #define RF_INT_PIN PORTEbits.RE8    
    #elif defined(__dsPIC33F__) || defined(__PIC24H__)
        #define RF_INT_PIN PORTAbits.RA12
    #endif

    #define TMRL TMR2
// Transceiver Configuration
    #define PHY_CS LATBbits.LATB2
    #define PHY_CS_TRIS TRISBbits.TRISB2
    #define PHY_RESETn LATGbits.LATG2
    #define PHY_RESETn_TRIS TRISGbits.TRISG2
    #define PHY_WAKE LATGbits.LATG3
    #define PHY_WAKE_TRIS TRISGbits.TRISG3
    
    #define PUSH_BUTTON_1 PORTDbits.RD6
    #define PUSH_BUTTON_2 PORTDbits.RD7
    #define LED_1 LATAbits.LATA7
    #define LED_2 LATAbits.LATA6
    
    #define PUSH_BUTTON_1_TRIS TRISDbits.TRISD6
    #define PUSH_BUTTON_2_TRIS TRISDbits.TRISD7
    #define LED_1_TRIS TRISAbits.TRISA7
    #define LED_2_TRIS TRISAbits.TRISA6
    
#endif

// MAC Address
#define EUI_7 0x11
#define EUI_6 0x22
#define EUI_5 0x33
#define EUI_4 0x44
#define EUI_3 0x55
#define EUI_2 0x66
#define EUI_1 0x77
#define EUI_0 0x03

// PICmicro Information
#define BAUD_RATE 19200
#define FREQUENCY_BAND 2400

// Message Buffers
#define TX_BUFFER_SIZE 127
#define RX_BUFFER_SIZE 127

#define SECURITY_KEY_00 0x00
#define SECURITY_KEY_01 0x01
#define SECURITY_KEY_02 0x02
#define SECURITY_KEY_03 0x03
#define SECURITY_KEY_04 0x04
#define SECURITY_KEY_05 0x05
#define SECURITY_KEY_06 0x06
#define SECURITY_KEY_07 0x07
#define SECURITY_KEY_08 0x08
#define SECURITY_KEY_09 0x09
#define SECURITY_KEY_10 0x0a
#define SECURITY_KEY_11 0x0b
#define SECURITY_KEY_12 0x0c
#define SECURITY_KEY_13 0x0d
#define SECURITY_KEY_14 0x0e
#define SECURITY_KEY_15 0x0f
#define KEY_SEQUENCE_NUMBER 0x00
#define SECURITY_LEVEL 0x04

/*********************************************************************/
// MY_PAN_ID defines the PAN identifier
/*********************************************************************/
#define MY_PAN_ID                       0x1234

/*********************************************************************/
// ADDITIONAL_CONNECTION_PAYLOAD defines the size of additional payload
// will be attached to the P2P Connection Request. Additional payload 
// is the information that the devices what to share with their peers
// on the P2P connection. The additional payload will be defined by 
// the application and defined in main.c
/*********************************************************************/
#define ADDITIONAL_CONNECTION_PAYLOAD   0

/*********************************************************************/
// P2P_CONNECTION_SIZE defines the maximum P2P connections that this 
// device allowes at the same time. 
/*********************************************************************/
#define P2P_CONNECTION_SIZE             10

/*********************************************************************/
// P2P_CAPACITY_INFO defines the capability of current device. The
// capability is defined as the bitmap of following definition:
// -----------------------------------------------------------------------------------
// |    Bit 0     |       Bit 1       |      Bit 2      |     Bit 3       | Bit 4 - 7|
// -----------------------------------------------------------------------------------
// | RXOnWhenIdle | DataRequestNeeded | TimeSynRequired | SecurityEnabled | Reserved |  
// -----------------------------------------------------------------------------------
/*********************************************************************/
#define P2P_CAPACITY_INFO               0x01

/*********************************************************************/
// TARGET_SMALL will remove the support of inter PAN communication
// and other minor features to save programming space
/*********************************************************************/
//#define TARGET_SMALL

/*********************************************************************/
// ENABLE_DUMP will enable the stack to be able to print out the 
// content of the P2P connection entry. It is useful in the debugging
// process
/*********************************************************************/
#define ENABLE_DUMP

/*********************************************************************/
// ENABLE_SLEEP will enable the device to go to sleep and wake up 
// from the sleep
/*********************************************************************/
#ifndef SIMPLE_EXAMPLE
    //#define ENABLE_SLEEP
#endif

/*********************************************************************/
// ENABLE_ED_SCAN will enable the device to do an energy detection scan
// to find out the channel with least noise and operate on that channel
/*********************************************************************/
#ifndef SIMPLE_EXAMPLE
    #define ENABLE_ED_SCAN
#endif

/*********************************************************************/
// ENABLE_ACTIVE_SCAN will enable the device to do an active scan to 
// to detect current existing connection. 
/*********************************************************************/
#ifndef SIMPLE_EXAMPLE
    #define ENABLE_ACTIVE_SCAN
#endif

/*********************************************************************/
// ENABLE_SECURITY will enable the device to encrypt and decrypt
// information transferred
/*********************************************************************/
#define ENABLE_SECURITY

/*********************************************************************/
// ENABLE_INDIRECT_MESSAGE will enable the device to store the packets
// for the sleeping devices temporily until they wake up and ask for
// the messages
/*********************************************************************/
#ifndef SIMPLE_EXAMPLE
    #define ENABLE_INDIRECT_MESSAGE
#endif

/*********************************************************************/
// ENABLE_BROADCAST will enable the device to broadcast messages for
// the sleeping devices until they wake up and ask for the messages
/*********************************************************************/
#ifndef SIMPLE_EXAMPLE
    #define ENABLE_BROADCAST
#endif

/*********************************************************************/
// INDIRECT_MESSAGE_SIZE defines the maximum number of packets that
// the device can store for the sleeping device(s)
/*********************************************************************/
#ifndef SIMPLE_EXAMPLE
    #define INDIRECT_MESSAGE_SIZE   2
#endif

/*********************************************************************/
// INDIRECT_MESSAGE_TIMEOUT defines the timeout interval in seconds
// for the stored packets for sleeping devices
/*********************************************************************/
#ifndef SIMPLE_EXAMPLE
    #define INDIRECT_MESSAGE_TIMEOUT 10
#endif

/*********************************************************************/
// ENABLE_FREQUENCY_AGILITY will enable the device to change operating
// channel to bypass the sudden change of noise
/*********************************************************************/
#ifndef SIMPLE_EXAMPLE
    #define ENABLE_FREQUENCY_AGILITY
#endif

/*********************************************************************/
// FREQUENCY_AGILITY_STARTER defines the device with the capability
// of starting the process of channel hopping. The device that can
// start the channel hopping must be able to do an energy scan
// to decide the optimal channel
/*********************************************************************/
#ifndef SIMPLE_EXAMPLE
    #define FREQUENCY_AGILITY_STARTER
#endif

#if defined(ENABLE_ACTIVE_SCAN) && defined(TARGET_SMALL)
    #error  Target_Small and Enable_Active_Scan cannot be defined together 
#endif

#if defined(FREQUENCY_AGILITY_STARTER) && !defined(ENABLE_ED_SCAN)
    #error The FREQUENCY_AGILITY_STARTER cannot be defined if energy scan is not enabled
#endif

// Constants Validation

// ZENA(TM) will automatically range check the entered values.  These range
// checks are included here in cases the application developer manually
// adjusts the values.

#if (RX_BUFFER_SIZE > 127)
    #error RX BUFFER SIZE too large. Must be <= 127.
#endif

#if (TX_BUFFER_SIZE > 127)
    #error TX BUFFER SIZE too large. Must be <= 127.
#endif

#if (RX_BUFFER_SIZE < 25)
    #error RX BUFFER SIZE too small. Must be >= 25.
#endif

#if (TX_BUFFER_SIZE < 25)
    #error TX BUFFER SIZE too small. Must be >= 25.
#endif

#if (NETWORK_TABLE_SIZE > 0xFE)
    #error NETWORK TABLE SIZE too large.  Must be < 0xFF.
#endif

#if (INDIRECT_BUFFER_SIZE > 0xFE)
    #error INDIRECT BUFFER SIZE too large.  Must be < 0xFF.
#endif

#endif
