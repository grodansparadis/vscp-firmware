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
*  This is the sample application that demonstrate the rich feature
*  of MiWi(TM) P2P stack. This demo should be used with FeatureDemoNode2
*  together. In this demo, following MiWi(TM) features
*  has been implemented:
*   - Active Scan 
*       This application will do an active scan to allocate all PANs
*       running in the neighborhood and choose the PAN that share the 
*       same PAN identifier to establish connection.
*   - Energy Scan
*       If no existing PAN that matches the desired PAN identifier, this
*       application will find a channel with least noise among the 16
*       channels available for IEEE 802.15.4. 
*   - Security Feature
*       This application is able to transmit and receive encrypted 
*       packet
*   - Indirect Message Feature 
*       This application is able to store the message to the device
*       with radio off during idle and deliever the message when that 
*       device wakes up and asking for it. This application is also
*       capable of delivering broadcast message to each individual 
*       device with radio off during idle
*   - Frequency Agility 
*       As a frequency agility starter, this application is able to 
*       decide the optimal channel and change operating channel. It
*       also has to broadcast and let other devices to change channel.
*
* Change History:
*  Rev   Date         Author    Description
*  0.1   03/01/2008    yfy       Initial revision
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
* Overview:		    This is the main function that runs the demo.  
*                   The device will first search for an existing
*                   network.  If a network exists and the parameters 
*                   of the network are acceptable to the device (in 
*                   this example simple RSSI minimum) then the device
*                   will join the existing network.  If the device 
*                   does not find an acceptable network and is a 
*                   coordinator then the device will perform an 
*                   energy scan on all of the channels available and 
*                   determine which channel has the lowest noise.  
*                   It will form a new network on this channel as 
*                   the PAN coordinator.
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
    
        myChannel = 0xFF;
        ConsolePutROMString((ROM char *)"\r\nStarting Active Scan...");
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
        i = ActiveScan(9, 0x07FFF800);
        
        if( i > 0 )
        {
            // now print out the scan result.
            printf("\r\nActive Scan Results: \r\n");
            for(j = 0; j < i; j++)
            {
                printf("Channel: ");
                PrintDec((ActiveScanResults[j].Channel >> 4) + 11);
                printf("   RSSI: ");
                PrintChar(ActiveScanResults[j].RSSIValue);
                printf("   PANID: ");
                PrintChar(ActiveScanResults[j].PANID.v[1]);
                PrintChar(ActiveScanResults[j].PANID.v[0]);
                printf("\r\n");
                
                if( ActiveScanResults[j].PANID.Val == myPANID.Val )
                {
                    myChannel = ActiveScanResults[j].Channel;
                }
            }
        }
    #endif


    #ifdef ENABLE_ED_SCAN
        // if there is no suitable PAN, we need to start a PAN ourselves
        if( myChannel == 0xFF )
        {
            ConsolePutROMString((ROM char*)"\r\nStarting Energy Scan...");
            /*******************************************************************/
            // Function OptimalChannel is used if energy scan feature is enabled
            // for this device. It will scan the available channels and returns
            // the channel that has least noise level. 
            // 
            // The first parameter is the scan duration for each channel. The 
            //     definition of scan duration is aligned with IEEE 802.15.4 
            //     specification. A longer scan duarion usually will create more 
            //     accurate scan results. To understand the time associated with
            //     the of the scan duration, here is the rough calculation:
            //       The scan duration of 10 is roughly one second. 
            //       Scan duration of 9 is about half second, and scan duration 
            //       of 8 is about a quarter second, etc. 
            //       On the other side, scan duration of 11 is about 2 second, 
            //       12 is about 4 second. The maximum scan duration time is 14, 
            //       or about 16 second for each channel.
            //
            // The second parameter of this function is the bit map of the allowed 
            //     channels. Bit 0 of the double word parameter represents channel 0. 
            //     For the 2.4GHz frequency band, the possible channels are channel 11 
            //     to channel 26. As the result, the bit map is 0x07FFF800.
            //
            // The third parameter of this function is the output parameter of the 
            //     RSSI value of the optimal channel. This parameter provides user
            //     the way to check the noise level of the optimal channel.
            /*******************************************************************/
            myChannel = OptimalChannel(10, 0x07FFF800, &i);
            printf("\r\nChannel ");
            PrintDec((myChannel>>4)+11);
            printf(" Has Least Noise with Maximum RSSI: ");
            PrintChar(i);
            printf("\r\nStart PAN at Channel ");
            PrintDec((myChannel>>4)+11);
            printf("\r\n");
        }
    #endif

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
    //     first device in the above procedure, thus we don't need to 
    //     call this function here.
    /*******************************************************************/
    #ifndef ENABLE_ED_SCAN
        i = CreateNewConnection(2);
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
    #if defined(ENABLE_DUMP) && !defined(ENABLE_ED_SCAN)
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
                    {
                        DWORD ChannelMap = ~((DWORD)0x00000800 << (currentChannel>>4));
                        
                        /*******************************************************************/
                        // Function InitChannelHopping will start the process of channel 
                        // hopping. This function can be only called by Frquency Agility 
                        // starter and the device must have the energy detection feature
                        // turned on. This function will do an energy detection scan of all
                        // input channels and start the process of jumping to the channel
                        // with least noise
                        //  The first parameter of this function is the bit map of the 
                        //  allowed channels. Bit 0 of the double word parameter represents 
                        //  channel 0. For the 2.4GHz frequency band, the possible channels 
                        //  are channel 11 to channel 26. As the result, the bit map is 
                        //  0x07FFF800. In this demo, we would like to hop to a channel
                        //  other than the current operating channel, so we have to AND
                        //  with the channel map to reset the bit for current operating 
                        //  channel.
                        //
                        // The MiWi(TM) P2P stack does not limit the application when to
                        // do channel hopping. The typical triggers for channel hopping are:
                        //  1.  Continuous data transmission failures, indicated by global 
                        //      variables CCAFailureTimes and AckFailureTimes
                        //  2.  Periodical try the channel hopping process every few hours
                        //      or once per day
                        //  3.  Receive a request to start the channel hopping process. This
                        //      demo is an example of manually issue the request. In the 
                        //      real application, a Frequency Agility follower can send a 
                        //      message to request channel hopping and the Frequency Agility
                        //      starter will decide if start the process.
                        /*******************************************************************/
                        InitChannelHopping(ChannelMap & 0x07FFF800);               
                    }
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
