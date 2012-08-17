/********************************************************************
* FileName:		FeatureDemoNode2.c
* Dependencies: none   
* Processor:	PIC18, PIC24F, PIC24H, dsPIC30, dsPIC33
*               tested with 18F4620, dsPIC33FJ256GP710	
* Hardware:		PICDEM Z, Explorer 16
* Complier:     Microchip C18 v3.04 or higher
*				Microchip C30 v2.03 or higher	
* Company:		Microchip Technology, Inc.
*
* Copyright and Disclaimer Notice for MiWi Software:
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
*  This is the sample application that demonstrate the rich feature
*  of MiWi(TM) P2P stack. This demo should be used with FeatureDemoNode1
*  together. In this demo, following MiWi(TM) features
*  has been implemented:
*   - Active Scan 
*       This application will do an active scan to allocate all PANs
*       running in the neighborhood and choose the PAN that share the 
*       same PAN identifier to establish connection.
*   - Security Feature
*       This application is able to transmit and receive encrypted 
*       packet
*   - Sleep Feature 
*       This application is able to put the radio and MCU into sleep 
*       mode to conserve power. After waking up from the sleep, this
*       application is also capable of sending out Data Request command
*       to retrieve possible data from its associated node. The sleeping
*       period for this application is roughly 8 seconds, depends on 
*       the MCU family and operating frequency setting.
*   - Frequency Agility 
*       As a frequency agility follower, this application is able to 
*       change operating channel when receiving a Channel Hopping 
*       command. This application is also capable of resynchronize
*       the connection by scanning possible channels.
*
* Change History:
*  Rev   Date         Author    Description
*  0.1   03/01/2008   yfy       Initial revision
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
// long address of this device, this variable array will be stored 
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
// in this demo. They are bitmap of English word "HELLO" and "P2P"
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
* Overview:		    This is the main function that runs the demo.  
*                   The device will first search for a P2P connection
*                   Once the connection is established, pressing 
*                   button one (RB5 on PICDEM Z or RD6 on Explorer 16) 
*                   or button two (RB4 on PICDEM Z or RD7 on Explorer16) 
*                   will send out broadcast and unicast packets.
*
* Note:			    
********************************************************************/
#if defined(__18CXX)
void main(void)
#else
int main(void)
#endif
{   
    BYTE i, j;
    BYTE OperatingChannel = 0xFF;
    
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
    
    #ifdef ENABLE_ACTIVE_SCAN
    
        ConsolePutROMString((ROM char*)"\r\nStarting Active Scan...");    
        while(1)
        {
            /*******************************************************************/
            // Function ActiveScan will return the number of existing connections
            // in all 16 channels. It will help to decide which channel to operate
            // on and which connection to add
            // The return value is the number of connections. The connection data
            //     are stored in global variable ActiveScanResults. Maximum active
            //     scan result is defined as ACTIVE_SCAN_RESULT_SIZE
            // The first parameter is the scan duration, which has the same definition
            //     in Energy Scan. 10 is roughly 1 second. 9 is a half second and 11 
            //     is 2 seconds. Maximum of 14 or 16 seconds.
            // The second parameter is the channel map. Bit 0 of the 
            //     double word parameter represents channel 0. For the 2.4GHz 
            //     frequency band, the possible channels are channel 11 to channel 
            //     26. As the result, the bit map is 0x07FFF800.
            /*******************************************************************/
            j = ActiveScan(9, 0x07FFF800);
            
            if( j > 0 )
            {
                // now print out the scan result.
                printf("\r\nActive Scan Results: \r\n");
                for(i = 0; i < j; i++)
                {
                    printf("Channel: ");
                    PrintDec((ActiveScanResults[i].Channel >> 4) + 11);
                    printf("   RSSI: ");
                    PrintChar(ActiveScanResults[i].RSSIValue);
                    printf("   PANID: ");
                    PrintChar(ActiveScanResults[i].PANID.v[1]);
                    PrintChar(ActiveScanResults[i].PANID.v[0]);
                    printf("\r\n");
                    // locate the PAN that matches the desired PAN identifier
                    if( ActiveScanResults[i].PANID.Val == myPANID.Val )
                    {
                        OperatingChannel = ActiveScanResults[i].Channel;
                    }
                }
            }
        
            if( OperatingChannel != 0xFF )
            {
                SetChannel(OperatingChannel);
                break;
            }
            
            printf("\r\nNo Suitable PAN, Rescanning...");
        }
    #endif
    
    
    
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
    // If the device allows energy scan, the second parameter of this 
    //     function is the bit map of the allowed channels. Bit 0 of the 
    //     double word parameter represents channel 0. For the 2.4GHz 
    //     frequency band, the possible channels are channel 11 to channel 
    //     26. As the result, the bit map is 0x07FFF800.
    //     
    // If the device allows energy scan, the procedure of the P2P connection
    //     is that the first device scans all available channels and decide
    //     the optimal channel (the channel with least noise). The other 
    //     device will send out P2P Connection Request in every available 
    //     channels to check which channel the first device operate on. 
    //     The current device in this piece of demo code operates as the 
    //     second device in the above procedure.
    /*******************************************************************/
    #ifndef ENABLE_ED_SCAN
        i = CreateNewConnection(2);
        printf("\r\nConnection Created");
    #endif
    
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
        DumpConnection(0xFF);
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
            // Macro isDataRequesting returns the boolean to indicate if
            // the Data Request command has received any feedback from its 
            // associated device. If there won't be any message from the associate
            // device to the device with radio off during idle, the RFD device
            // will not required to send out Data Request command, thus the return
            // value of macro isDataRequesting is always FALSE
            /*******************************************************************/
            if( isDataRequesting() == FALSE )
            {
                /*******************************************************************/
                // If Data Request has been finished, it is now time to check if 
                // we want to send out any packet.
                // Function ButtonPressed will return if any of the two buttons
                // (RB5 and RB4 on PICDEM Z or RD6 and RD7 on Explorer 16) has been
                // pushed.
                /*******************************************************************/
                BYTE PressedButton = ButtonPressed();
                
                switch( PressedButton )
                {
                    case 1: 
                        /*******************************************************************/                
                        // Button 1 (RB5 on PICDEM Z or RD6 on Explorer 16) pressed
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
                        #ifdef ENABLE_SLEEP
                            ReadyToSleep();
                        #endif
                        break;
                        
                    case 2:
                        /*******************************************************************/                
                        // Button 2 (RB4 on PICDEM Z or RD7 on Explorer 16) pressed
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
                        //       secure the frame. If security is applied, the security
                        //       level and security key are defined in P2PDefs.h
                        //
                        // Another way to unicast a message is by calling function
                        // UnicastLongAddress. Instead of supplying the index of the P2P 
                        // Connection Entry of the peer device, this function requires the 
                        // input parameter of destination PAN identifier and destination
                        // long address.
                        /*******************************************************************/
                        i = UnicastConnection(0, FALSE, TRUE);
                        #ifdef ENABLE_SLEEP
                            ReadyToSleep();
                        #endif
                        break;
                        
                    default:
                        #ifdef ENABLE_SLEEP
                            ReadyToSleep();
                        #endif
                        break;
                }
                
                #ifdef ENABLE_FREQUENCY_AGILITY
                    /***********************************************************************
                    * AckFailureTimes is the global variable to track the transmission 
                    * failure because no acknowledgement frame is received. Typically,
                    * this is the indication of either very strong noise, or the PAN
                    * has hopped to another channel. Here we call function ResyncConnection
                    * to resynchronize the connection.
                    *   The first parameter is the destination long address of the peer node
                    *   that we would like to resynchronize to.
                    *   The second parameter is the bit map of channels to be scanned
                    *************************************************************************/
                    if( AckFailureTimes > ACK_FAILURE_TIMES )
                    {
                        ConsolePutROMString((ROM char*)"\r\nResynchronizing the Connection...");
                        ResyncConnection(P2PConnections[0].PeerLongAddress, 0x07FFF800);    
                    }
                
                #endif
                
                #ifdef ENABLE_SLEEP
                    /*******************************************************************/
                    // If Data Request command and data transmision has been handled,
                    // as the RFD device, it is time to consider put both the radio and 
                    // MCU into sleep mode to conserve power.
                    /*******************************************************************/
                    if( isReadyToSleep() )
                    {
                        ClearReadyToSleep();
                        
                        /*******************************************************************/
                        // Put MRF24J40 radio into sleep. The radio is set to be waken up by
                        // the MCU.
                        /*******************************************************************/
                        MRF24J40Sleep();
                        
                        while(ConsoleIsPutReady() == 0); 
                        
                        /*******************************************************************/
                        // Prepare the condition to wake up the MCU. The MCU can either be
                        // waken up by the timeout of watch dog timer, or by the pin change
                        // notification interrupt by pushing the button.
                        /*******************************************************************/
                        #if defined(__18CXX)
                            WDTCONbits.SWDTEN = 1;      // enable watch dog timer
                            INTCONbits.RBIF = 0;        // clear pin change notification interrupt
                            INTCONbits.RBIE = 1;        // enable pin change notification interrupt
                            ClrWdt();
                        #elif defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24H__)
                            ClrWdt();
                            RCONbits.SWDTEN = 1;        // enable watch dog timer
                            IFS1bits.CNIF = 0;
                            IEC1bits.CNIE = 1;          // enable pin change notification interrupt
                        #elif defined(__PIC32MX__)
                            Enable_PB_1_2_Interrupts(); // enable pin change notification interrupt
                            EnableWDT();                // enable watch dog timer
                            ClearWDT();                 // clear watch dog timer
                        #endif
                        
                        // Put MCU into sleep mode
                        Sleep();
                        
                        #if defined(__18CXX)
                            INTCONbits.RBIE = 0;        // disable pin change notification interrupt
                            WDTCONbits.SWDTEN = 0;      // disable watch dog timer
                        #elif defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24H__)
                            RCONbits.SWDTEN = 0;        // disable watch dog timer
                            //IEC1bits.CNIE = 0;          // disable pin change notification interrupt
                        #elif defined(__PIC32MX__)
                            RCONCLR = RCON_SLEEP_MASK | RCON_IDLE_MASK;   // clear the Sleep and Idle bits
                            DisableWDT();
                        #endif
                        
                        // wake up the MCU
                        MRF24J40Wake();

                        /*******************************************************************/
                        // If there may be message from the peer node to the RFD device
                        // a Data Request command needs to be send out to acquire the message
                        // stored by the peer device. 
                        // Function CheckForData will initiate the process of sending out 
                        // Data request command and waiting for the response from the peer
                        // device
                        /*******************************************************************/
                        if( !CheckForData())
                        {
                            printf("\r\nData Request Failed");
                        }
                    }
                #endif   
            }   // end of data requesting
             
        }
    }
}
