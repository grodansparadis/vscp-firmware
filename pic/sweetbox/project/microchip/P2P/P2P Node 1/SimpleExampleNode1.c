/********************************************************************
* FileName:		main.c
* Dependencies: none   
* Processor:	PIC18, PIC24F, PIC24H, dsPIC30, dsPIC33
*               tested with 18F4620, dsPIC33FJ256GP710	
* Hardware:		PICDEM Z, Explorer 16
* Complier:     Microchip C18 v3.04 or higher
*				Microchip C30 v2.03 or higher	
* Company:		Microchip Technology, Inc.
*
* Copyright and Disclaimer Notice for P2P Software:
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
*  This is the main example file.  
*
* Change History:
*  Rev   Date         Author    Description
*  0.1   1/03/2008    yfy       Initial revision
********************************************************************/

/************************ HEADERS **********************************/
#include ".\Common\Console.h"
#include ".\P2P\P2P.h"
#include ".\P2P\MRF24J40.h"
#include ".\P2P\SymbolTime.h"

/************************ VARIABLES ********************************/

/*******************************************************************/
// AdditionalConnectionPayload variable array defines the additional 
// information to identify a device on a P2P connection. This array
// will be transmitted with the P2P_CONNECTION_REQUEST command to 
// initiate the connection between the two devices. Along with the 
// long address of this device, this  variable array will be stored 
// in the P2P Connection Entry structure of the partner device. The 
// size of this array is ADDITIONAL_CONNECTION_PAYLOAD, defined in 
// P2PDefs.h.
// In this demo, this variable array is set to be empty.
/******************************************************************/
#if ADDITIONAL_CONNECTION_PAYLOAD > 0
    BYTE AdditionalConnectionPayload[ADDITIONAL_CONNECTION_PAYLOAD];
#endif

/*******************************************************************/
// The variable myChannel defines the channel that the P2P connection
// is operate on. This variable will be only effective if energy scan
// (ENABLE_ED_SCAN) is not turned on. Once the energy scan is turned
// on, the operating channel will be one of the channels available with
// least amount of energy (or noise).
/*******************************************************************/
BYTE myChannel = CHANNEL_25;

/*******************************************************************/
// the following two variable arrays are the data to be transmitted
// in this demo. They are bit map of English word "HELLO" and "P2P"
// respectively.
/*******************************************************************/
ROM const BYTE Hello[100] = 
{
    0xB2,0x20,0xB2,0x20,0xB2,0xB2,0xB2,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0x20,0xB2,0xB2,0x20,0x0D,0x0A,
    0xB2,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x0D,0x0A,
    0xB2,0xB2,0xB2,0x20,0xB2,0xB2,0xB2,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x0D,0x0A,
    0xB2,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x0D,0x0A,
    0xB2,0x20,0xB2,0x20,0xB2,0xB2,0xB2,0x20,0xB2,0xB2,0xB2,0xB2,0xB2,0xB2,0x20,0xB2,0xB2,0x20,0x0D,0x0A
};
ROM const BYTE P2P[65] =
{
    0xB2,0xB2,0xB2,0x20,0xB2,0xB2,0xB2,0x20,0xB2,0xB2,0xB2,0x0D,0x0A,
    0xB2,0x20,0xB2,0x20,0x20,0x20,0xB2,0x20,0xB2,0x20,0xB2,0x0D,0x0A,
    0xB2,0xB2,0xB2,0x20,0xB2,0xB2,0xB2,0x20,0xB2,0xB2,0xB2,0x0D,0x0A,
    0xB2,0x20,0x20,0x20,0xB2,0x20,0x20,0x20,0xB2,0x20,0x20,0x0D,0x0A,
    0xB2,0x20,0x20,0x20,0xB2,0xB2,0xB2,0x20,0xB2,0x20,0x20,0x0D,0x0A
};

/*********************************************************************
* Function:         void main(void)
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    none
*
* Side Effects:	    none
*
* Overview:		    This is the main function that runs the simple 
*                   example demo. The purpose of this example is to
*                   demonstrate the simple application programming
*                   interface for the MiWi(TM) P2P stack. By virtually
*                   total of less than 30 lines of code, we can develop
*                   a complete application using MiWi(TM) P2P APIs
*                   provided the stack. The application will first 
*                   try to establish a P2P link with another device
*                   and then process the received information as well
*                   as transmit its own information.
*                   MiWi(TM) P2P stack also support a set of rich 
*                   features. Example code RichFeatureExampleP2P will
*                   demonstrate how to implement the rich features with
*                   MiWi(TM) P2P stack application programming interfaces.
*
* Note:			    
********************************************************************/
#if defined(__18CXX)
void main(void)
#else
int main(void)
#endif
{   
    BYTE i;
    
    /*******************************************************************/
    // Initialize the system
    /*******************************************************************/
    BoardInit();         
    ConsoleInit();  
    P2PInit();   
    #if defined(PICDEMZ)
        INTCONbits.GIEH = 1;
    #elif defined(EXPLORER16)
    #else
        #error "Unknown board.  Please initialize board as required."
    #endif

    ConsolePutROMString((ROM char*)"\r\nStarting P2P Demo...");
    LED_1 = 0;

    // Set the device to operate on the optimal channel
    SetChannel(myChannel);

    /*******************************************************************/
    // Function EnableNewConnection will enable the device to accept
    // request to establish P2P connection from other devices. If a
    // P2P connection has been established, user can choose to call
    // function DisableNewConnection() to reject further request to 
    // establish P2P connection, if it is desired for the application
    /*******************************************************************/
    EnableNewConnection();

    /*******************************************************************/
    // Function CreateNewConnection will try to establish a P2P connection
    // before returning the index of P2P Connection Entry for the partner 
    // device. 
    // The first parameter is the retry interval in second, which defines
    //     the interval between two P2P_Connection_request command, if the 
    //     first one does not establish a valid P2P connection. There should 
    //     have interval between the requests to allow other device operating
    //     on the same channel to talk.
    //
    /*******************************************************************/
    i = CreateNewConnection(2);
    
    /*******************************************************************/
    // Function DumpConnection is used to print out the content of the
    // P2P Connection Entry on the hyperterminal. It may be useful in 
    // the debugging phase.
    // The only parameter of this function is the index of the P2P 
    // Connection Entry. The value of 0xFF means to print out all
    // valid P2P Connection Entry; otherwise, the P2P Connection Entry
    // of the input index will be printed out.
    /*******************************************************************/
    #ifdef ENABLE_DUMP
        DumpConnection(i);
    #endif

    // Turn on LED 1 to indicate P2P connection established
    LED_1 = 1;

    while(1)
    {
        /*******************************************************************/
        // Function ReceivedPacket will return a boolean to indicate if a 
        // packet has been received by the transceiver. If a packet has been
        // received, all information will be stored in the rxFrame, structure  
        // of RECEIVED_FRAME.
        /*******************************************************************/
        if( ReceivedPacket() )
        {
            /*******************************************************************/
            // If a packet has been received, following code prints out some of
            // the information available in rxFrame.
            /*******************************************************************/
            ConsolePutROMString((ROM char *)"\r\nReceived Packet ");
            #ifdef ENABLE_SECURITY
                if( rxFrame.flags.bits.security )
                {
                    ConsolePutROMString((ROM char *)"Secured ");
                }
            #endif
            #ifndef TARGET_SMALL
                if( rxFrame.flags.bits.broadcast )
                {
                    ConsolePutROMString((ROM char *)"Broadcast with RSSI ");
                }
                else
                {
                    ConsolePutROMString((ROM char *)"Unicast with RSSI ");
                }
                PrintChar(rxFrame.PacketRSSI);
                ConsolePutROMString((ROM char *)" from ");
                for(i = 0; i < 8; i++)
                {
                    PrintChar(rxFrame.SourceLongAddress[7-i]);
                }
            #endif
            ConsolePutROMString((ROM char *)": \r\n");
            for(i = 0; i < rxFrame.PayLoadSize; i++)
            {
                ConsolePut(rxFrame.PayLoad[i]);
            }
            
            // Toggle LED2 to indicate receiving a packet.
            LED_2 ^= 1;
            
            /*******************************************************************/
            // Function DiscardPacket is used to release the current received packet.
            // After calling this function, the stack can start to process the
            // next received frame 
            /*******************************************************************/        
            DiscardPacket();
        }
        else
        {
            /*******************************************************************/
            // If no packet received, now we can check if we want to send out
            // any information.
            // Function ButtonPressed will return if any of the two buttons
            // (RB5 and RB4 on PICDEM Z or RD6 and RD7 on Explorer 16) has been
            // pushed.
            /*******************************************************************/
            BYTE PressedButton = ButtonPressed();
            switch( PressedButton )
            {
                case 1:                 
                    /*******************************************************************/                
                    // Button 1 (RB5 on PICDEM Z or RD6 on Explorer 16) pressed. We need
                    // to send out the bitmap of word "HELLO"
                    // First FlushTx to reset the Transmit buffer. Then fill the buffer
                    // by calling function WriteData
                    /*******************************************************************/
                    FlushTx();
                    for(i = 0; i < 100; i++)
                    {
                        WriteData(Hello[i]);
                    }
                    
                    /*******************************************************************/
                    // Function BroadcastPacket is used to broadcast a message
                    //    The first parameter is the target PAN Identifier
                    //    The second parameter is the boolean to indicate if this is a
                    //       command frame
                    //    The third parameter is the boolean to indicate if we need to
                    //       secure the frame
                    /*******************************************************************/
                    BroadcastPacket(myPANID, FALSE, FALSE);
                    break;
                    
                case 2:
                    /*******************************************************************/                
                    // Button 2 (RB4 on PICDEM Z or RD7 on Explorer 16) pressed. We need
                    // to send out the bitmap of word "P2P" encrypted.
                    // First FlushTx to reset the Transmit buffer. Then fill the buffer
                    // by calling function WriteData
                    /*******************************************************************/
                    FlushTx();
                    for(i = 0; i < 65; i++)
                    {
                        WriteData(P2P[i]);
                    }
                    
                    /*******************************************************************/
                    // Function UnicastPacket is one of the functions to unicast a 
                    // message.
                    //    The first parameter is the index of P2P Connection Entry for 
                    //       the peer device. In this demo, since there are only two
                    //       devices involved, the peer device must be stored in the 
                    //       first P2P Connection Entry
                    //    The second parameter is the boolean to indicate if this is a
                    //       command frame
                    //    The third parameter is the boolean to indicate if we need to
                    //       secure the frame. If encryption is applied, the security
                    //       level and security key are defined in P2PDefs.h
                    //
                    // Another way to unicast a message is by calling function
                    // UnicastLongAddress. Instead of supplying the index of the P2P 
                    // Connection Entry of the peer device, this function requires the 
                    // input parameter of destination PAN identifier and destination
                    // long address.
                    /*******************************************************************/
                    UnicastConnection(0, FALSE, TRUE);  
                    break;
                    
                default:
                    break;
            }    
        }
    }
}
