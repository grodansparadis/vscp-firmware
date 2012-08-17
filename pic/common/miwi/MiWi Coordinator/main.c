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
* Copyright and Disclaimer Notice for MiWi Software:
*
* Copyright © 2006-2007 Microchip Technology Inc.  All rights reserved.
*
* Microchip licenses to you the right to use, modify, copy and 
* distribute Software only when embedded on a Microchip
* microcontroller or digital signal controller and used with a 
* Microchip radio frequency transceiver, which are integrated into
* your product or third party product (pursuant to the sublicense 
* terms in the accompanying license agreement).  
*
* You should refer to the license agreement accompanying this 
* Software for additional information regarding your rights 
* and obligations.
*
* SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION,
* ANY WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND
* FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL MICROCHIP OR
* ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE,
* STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL
* EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
* INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT,
* PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST
* OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
* CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
* THEREOF), OR OTHER SIMILAR COSTS.
*
*********************************************************************
* File Description:
*
*  This is the main example file.  Since there are only 2 buttons 
*   available on the PICDEM Z, we have created several defintions
*   in the definitions section of this file to select between
*   several different demos showing off different ways to use features
*   of the stack.  Please read each explination carefully.
*
* Change History:
*  Rev   Date         Description
*  0.1   11/09/2006   Initial revision
*  1.0   01/09/2007   Initial release
********************************************************************/

/************************ HEADERS **********************************/
#include ".\Common\Console.h"
#include ".\MiWi\MiWi.h"
#include ".\MiWi\MRF24J40.h"
#include ".\MiWi\SymbolTime.h"

/************************ VARIABLES ********************************/
#pragma romdata longAddressLocation = 0x0E
ROM unsigned char myLongAddress[8] = {EUI_0,EUI_1,EUI_2,EUI_3,EUI_4,EUI_5,EUI_6,EUI_7};
#pragma romdata

#ifdef SUPPORT_SECURITY
#pragma romdata securityKey = 0x2A
ROM unsigned char mySecurityKey[16] = {SECURITY_KEY_0, SECURITY_KEY_1, SECURITY_KEY_2, SECURITY_KEY_3, SECURITY_KEY_4,
	SECURITY_KEY_5, SECURITY_KEY_6, SECURITY_KEY_7, SECURITY_KEY_8, SECURITY_KEY_9, SECURITY_KEY_10, SECURITY_KEY_11, 
	SECURITY_KEY_12, SECURITY_KEY_13, SECURITY_KEY_14, SECURITY_KEY_15};
#pragma romdata
ROM unsigned char mySecurityLevel = SECURITY_LEVEL;
ROM unsigned char myKeySequenceNumber = KEY_SEQUENCE_NUMBER;
#endif


ROM BYTE availableChannels[AVAILABLE_CHANNELS_SIZE]={ALLOWED_CHANNELS};
ROM unsigned char myManufacturerString[]="Company ABC";

/********************** FUNCTION PROTOTYPES *************************/
void BoardInit(void);

/********************** CONSTANTS/DEFINITIONS ***********************/
#define MAX_DISCOVERY_ATTEMPTS 3
#define DEBOUNCE_TIME 0x00008FFF

/*  Example Descriptions

Define EXAMPLE = 1 for example 1, 2 for example 2, etc.

Example 1:____________________________________________________________
Press RB4 any two boards that have joined to the network within a second
of each other.  Their RA1 LEDs should turn on.  This indicates that they
have formed a socket with each other.  By pressing the RB5 button on one board
the light (RA0 LED) should turn on the other board.  

Example 2:____________________________________________________________
By pressing RB5 the device will send out a EUI address search request.  
If a EUI address search response comes back the device will update its
network table with the devices information.

Example 3:____________________________________________________________
By pressing RB5 the device will send a light toggle message directly to
the long address specified (provided it is available in the network table).

*/
#define EXAMPLE 1

#if (EXAMPLE>3)
    #error "There are only 3 available examples.  Please define _EXAMPLE to be <= 3.  To define your own examples please comment out the _EXAMPLE definition."
#endif

#if !defined(P2P_ONLY)
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
* Note:			    This header only applies if P2P_ONLY is not
*                   defined.  If it is then the appropriate header
*                   is located below
********************************************************************/
#if defined(__18CXX)
void main(void)
#else
int main(void)
#endif
{   
    BYTE i;
    BYTE numDiscoveryAttempts;
    BOOL requestedSocket;
    BOOL PUSH_BUTTON_1_pressed;
    BOOL PUSH_BUTTON_2_pressed;
    TICK PUSH_BUTTON_1_press_time;
    TICK PUSH_BUTTON_2_press_time;
    TICK tickDifference;
    BYTE myFriend = 0xFF;
   
    requestedSocket = FALSE;
    
    numDiscoveryAttempts = 0;

    //initialize the system
    BoardInit();         
    ConsoleInit();  
    MiWiInit();
    #if defined(PICDEMZ)
        INTCONbits.GIEH = 1;
    #elif defined(EXPLORER16)
    #else
        #error "Unknown board.  Please initialize board as required."
    #endif

    //if they are holding down PUSH_BUTTON_1 on startup
    //then try to rejoin the network
    if(PUSH_BUTTON_1 == 0)
    {
        while(PUSH_BUTTON_1==0){}
        RejoinNetwork();
    }

    while(1)
    {
        //call the MiWiTask handler
        MiWiTasks();
        
        //if we are a memeber of a network
        if(MemberOfNetwork())
        {
            //If there is a packet waiting for the application level
            if(RxPacket())
            {
                BYTE *pRxData2;
                BYTE i;
                
                //insert user code here for processing packets as they come in.
                
                //This is a light example
                pRxData2 = pRxData;
                *pRxData++;     //take off the seq number
                
                switch(*pRxData++)      //report type
                {
                    case USER_REPORT_TYPE:
                        switch(*pRxData++)      //report id
                        {
                            case LIGHT_REPORT:
                                switch(*pRxData++)      //first byte of payload
                                {
                                    case LIGHT_ON:
                                        LED_2 = 1;
                                        break;
                                    case LIGHT_OFF:
                                        LED_2 = 0;
                                        break;
                                    case LIGHT_TOGGLE:
                                        LED_2 ^= 1;
                                        break;
                                }
                                break;
                        }
                        break;
                    case MIWI_STACK_REPORT_TYPE:
                        switch(*pRxData)
                        {
                            case ACK_REPORT_TYPE:
                                //ConsolePutROMString((ROM char*)"Got MiWi ACK for my packet\r\n");
                                break;
                        }
                        break;
                }
                
                //need to discard this packet before we are able to receive any other packets
                DiscardPacket();
            }
            
            #if defined(SUPPORT_CLUSTER_SOCKETS) || defined(SUPPORT_P2P_SOCKETS)
            //if we requested a socket
            if(requestedSocket == TRUE)
            {
                //if that request completed
                if(OpenSocketComplete())
                {
                    //then we are no longer requesting a socket
                    requestedSocket = FALSE;
                    
                    //see if it was successful
                    if(OpenSocketSuccessful())
                    {
                        //if successful turn on LED and save the handle of the socketed partner
                        LED_1 = 1;
                        ConsolePutROMString((ROM char*)"Found a socket: ");
                        myFriend = OpenSocketHandle();
                        PrintChar(myFriend);
                        ConsolePutROMString((ROM char*)"\r\n");
                    }
                    else
                    {
                        LED_1 = 0;
                        myFriend = 0xFF;
                        ConsolePutROMString((ROM char*)"socket request failed\r\n");
                    }
                }
            }
            #endif
            
            //if button pressed
            if(PUSH_BUTTON_1 == 0)
            {
                //if the button was previously not pressed
                if(PUSH_BUTTON_1_pressed == FALSE)
                {
                    #if (EXAMPLE == 1)
                    //if we have a socket formed already
                    if(myFriend != 0xFF)
                    #endif
                    {                      
                        //mark the button as having been pressed already
                        PUSH_BUTTON_1_pressed = TRUE;
                        
                        #if (EXAMPLE==1)
                            //Send a light toggle report to myFriend (the one we created the socket with)
    						TxData = 11;
                            WriteData(USER_REPORT_TYPE);
                            WriteData(LIGHT_REPORT);
                            WriteData(LIGHT_TOGGLE);
                            //TODO: need a way to clear the buffer if they decide not to transmit
                            SendReportByHandle(myFriend,FALSE);
                        #endif

                        #if (EXAMPLE==2)
                            //look in the network table for the device with the following long address
                            ConsolePutROMString((ROM char*)"looking for node\r\n");
                            tempLongAddress[0] = 0x07;
                            tempLongAddress[1] = 0x06;
                            tempLongAddress[2] = 0x05;
                            tempLongAddress[3] = 0x04;
                            tempLongAddress[4] = 0x03;
                            tempLongAddress[5] = 0x02;
                            tempLongAddress[6] = 0x01;
                            tempLongAddress[7] = 0x55;
                            
                            DiscoverNodeByEUI();
                        #endif

                        #if (EXAMPLE == 3)
                            //send a long toggle address to the node
                            //with the following address (must be already located in
                            //the network table
    					    TxData = 11;
                            WriteData(USER_REPORT_TYPE);
                            WriteData(LIGHT_REPORT);
                            WriteData(LIGHT_TOGGLE);
    
                            tempLongAddress[0] = 0x99;
                            tempLongAddress[1] = 0x78;
                            tempLongAddress[2] = 0x56;
                            tempLongAddress[3] = 0x34;
                            tempLongAddress[4] = 0x12;
                            tempLongAddress[5] = 0xA3;
                            tempLongAddress[6] = 0x04;
                            tempLongAddress[7] = 0x00;
    
                            SendReportByLongAddress(tempLongAddress);
                            if(myFriend != 0xFF)
                            {
                                SendReportByHandle(myFriend,FALSE);
                            }
                        #endif
                        
                        //get a time stamp for when the button was pressed
                        PUSH_BUTTON_1_press_time = TickGet();
                    }
                }

            }
            else
            {
                //get the current time
                TICK t = TickGet();
                
                //if the button has been released long enough
                tickDifference.Val = TickGetDiff(t,PUSH_BUTTON_1_press_time);
                
                //then we can mark it as not pressed
                if(tickDifference.Val > DEBOUNCE_TIME)
                {
                    PUSH_BUTTON_1_pressed = FALSE;
                }
            }
            
            if(PUSH_BUTTON_2 == 0)
            {
                if(PUSH_BUTTON_2_pressed == FALSE)
                {
                    PUSH_BUTTON_2_pressed = TRUE;
                    
                    requestedSocket = TRUE;
                    #if defined(SUPPORT_CLUSTER_SOCKETS)
                        OpenSocket(CLUSTER_SOCKET);
                    #else
                        ConsolePutROMString((ROM char*)"Function not supported\r\n");
                        OpenSocket(P2P_SOCKET);
                    #endif
                    
                    PUSH_BUTTON_2_press_time = TickGet();
                }
            }
            else
            {
                TICK t = TickGet();
                
                tickDifference.Val = TickGetDiff(t,PUSH_BUTTON_2_press_time);
                
                if(tickDifference.Val > DEBOUNCE_TIME)
                {
                    PUSH_BUTTON_2_pressed = FALSE;
                }
            }
                                    
            //END OF THE CODE AFTER YOU BECAME A MEMBER
        }
        else
        {  
            #if !defined(P2P_ONLY)
            //If I don't have a network yet and I am not currently trying to join a network
            if((!SearchingForNetworks()) && (!AttemptingToJoinNetwork()) && (!AttemptingToRejoinNetwork()))
            {
                //I am not actively searching for a network so lets choise one from the list
                BYTE handleOfBestNetwork;
                BYTE i;
                //I will show the example of picking a network based on RSSI
                BYTE maxRSSI;
                
                //initialize the handle to none(0xFF)
                handleOfBestNetwork = 0xFF;
                //RSSI example
                maxRSSI = 0x00;
                
                //pick which network to join here
                for(i=0;i<NETWORK_TABLE_SIZE;i++)
                {
                    if(networkTable[i].status.bits.isValid)
                    {
                        if(networkTable[i].status.bits.NeighborOrNetwork == NETWORK)
                        {
                            //make your PAN choice here
                            if(networkTable[i].info.networkInfo.Protocol == MIWI_PROTOCOL_ID)
                            {
                                //make sure they are allowing joining
                                if(networkTable[i].info.networkInfo.flags.bits.associationPermit == 1)
                                {
                                    //first I want to make sure it is a MiWi network
                                    if(networkTable[i].info.networkInfo.sampleRSSI >= maxRSSI)
                                    {
                                        handleOfBestNetwork = i;
                                        maxRSSI = networkTable[i].info.networkInfo.sampleRSSI;
                                    }
                                }
                            }
                        }
                    }
                }
                
                //now that I picked a network let me join to it
                if(handleOfBestNetwork == 0xFF)
                {
                    //I couldn't find a suitable network
                    ConsolePutROMString((ROM char*)"I couldn't find a suitable network\r\n");
                   
                    if(numDiscoveryAttempts++ > MAX_DISCOVERY_ATTEMPTS)
                    {
                        #if defined(I_AM_COORDINATOR_CAPABLE)
                            //form a network with the specified PANID
                            FormNetwork(0xFFFF);
                        #endif

                        #if !defined(P2P_ONLY)
                        //clear all of the network entries
                            ClearNetworkTable(CLEAR_NETWORKS);
                        #endif
                        
                        {
                            WORD i;
                            
                            LED_1 = 1;
                            
                            for(i=0;i<20000;i++)
                            {
                            }
                            
                            LED_1 = 0;
                            
                            for(i=0;i<20000;i++)
                            {
                            }
                            
                            LED_1 = 1;

                            for(i=0;i<20000;i++)
                            {
                            }
                            
                            LED_1 = 0;                            
                        }
                        
                    }
                    else
                    {
                        #if !defined(P2P_ONLY)
                            //clear all of the network entries
                            ClearNetworkTable(CLEAR_NETWORKS);
                        #endif
                        
                        //and start a new search
                        DiscoverNetworks();
                    }
                }
                else
                {
                    #if !defined(P2P_ONLY)
                        //I found a network I would like to join                   
                        //Join the network that you found to be the best
                        ConsolePutROMString((ROM char*)"Trying to join network: ");
                        PrintChar(handleOfBestNetwork);
                        ConsolePutROMString((ROM char*)"\r\n");
                        JoinNetwork(handleOfBestNetwork);
                        
                        {
                            WORD i;
                            
                            LED_2 = 1;
                            
                            for(i=0;i<20000;i++)
                            {
                            }
                            
                            LED_2 = 0;
                            
                            for(i=0;i<20000;i++)
                            {
                            }
                            
                            LED_2 = 1;

                            for(i=0;i<20000;i++)
                            {
                            }
                            
                            LED_2 = 0;                            
                        }
                    #endif
                }
            }
            #endif //P2P_ONLY
            //if I am searching for a network then I will leave it alone so I can continue searching
            //or if I am trying to join a network then I will let that join proccess finish
        }
    }
}

#else

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
* Note:			    This header only applies if P2P_ONLY is defined.
*                   If it is not then the appropriate header
*                   is located above
********************************************************************/
void main(void)
{
    BOOL PUSH_BUTTON_1_pressed;
    TICK PUSH_BUTTON_1_press_time, tickDifference;
    BOOL FoundSocket;
    BYTE myFriend;
    
    ConsoleInit();
    BoardInit();  
    MiWiInit();
    
    SetChannel(CHANNEL_12);
    
    FoundSocket = FALSE;
    
    while(1)
    {
        MiWiTasks();
        
        if(FoundSocket == TRUE)
        {
            //TODO: insert user application
            if(RxPacket())
            {
                BYTE *pRxData2;
                BYTE i;
                
                //insert user code here for processing packets as they come in.
                
                pRxData2 = pRxData;
                *pRxData++;     //take off the seq number
                
                switch(*pRxData++)      //report type
                {
                    case USER_REPORT_TYPE:
                        switch(*pRxData++)      //report id
                        {
                            case LIGHT_REPORT:
                                switch(*pRxData++)      //first byte of payload
                                {
                                    case LIGHT_ON:
                                        LED_2 = 1;
                                        break;
                                    case LIGHT_OFF:
                                        LED_2 = 0;
                                        break;
                                    case LIGHT_TOGGLE:
                                        LED_2 ^= 1;
                                        break;
                                }
                                break;
                        }
                        break;
                    case MIWI_STACK_REPORT_TYPE:
                        switch(*pRxData)
                        {
                            case ACK_REPORT_TYPE:
                                //ConsolePutROMString((ROM char*)"Got MiWi ACK for my packet\r\n");
                                break;
                        }
                        break;
                }
                
                //need to discard this packet before we are able to receive any other packets
                DiscardPacket();
            }
            
            if(PUSH_BUTTON_1 == 0)
            {
                if(PUSH_BUTTON_1_pressed == FALSE)
                {                   
                    PUSH_BUTTON_1_pressed = TRUE;

					TxData = 11;
                    WriteData(USER_REPORT_TYPE);
                    WriteData(LIGHT_REPORT);
                    WriteData(LIGHT_TOGGLE);

                    SendReportByLongAddress(openSocketInfo.LongAddress1);

                }
                PUSH_BUTTON_1_press_time = TickGet();
            }
            else
            {
                TICK t = TickGet();
                
                tickDifference.Val = TickGetDiff(t,PUSH_BUTTON_1_press_time);
                
                if(tickDifference.Val > DEBOUNCE_TIME)
                {
                    PUSH_BUTTON_1_pressed = FALSE;
                }
            }
            
        }
        else
        {
            if(OpenSocketComplete())
            {                
                if(OpenSocketSuccessful())
                {
                    FoundSocket = TRUE;
                    LED_1 = 1;
                    ConsolePutROMString((ROM char*)"Found a socket: ");
                    ConsolePutROMString((ROM char*)"\r\n");
                }
                else
                {
                    ConsolePutROMString((ROM char*)"sending request\r\n");
                    //else we need to try again (or for the first time)
                    OpenSocket(P2P_SOCKET);
                }
            }
        }
    }
}
#endif

/*********************************************************************
 * Function:        void BoardInit( void )
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Board is initialized for MiWi usage
 *
 * Overview:        This function configures the board for the PICDEM-z
 *                  MRF24J40 usage 
 *
 * Note:            This routine needs to be called before the function 
 *                  to initialize MiWi stack or any other function that
 *                  operates on the stack
 ********************************************************************/

void BoardInit(void)
{
    #if defined(PICDEMZ)
    WDTCONbits.SWDTEN = 0; //disable WDT
    
    // Switches S2 and S3 are on RB5 and RB4 respectively. We want interrupt-on-change
    INTCON = 0x00;
    
    // There is no external pull-up resistors on S2 and S3. We will use internal pull-ups.
    // The MRF24J40 is using INT0 for interrupts
    // Enable PORTB internal pullups
    INTCON2 = 0x00;
    INTCON3 = 0x00;

    // Make PORTB as input - this is the RESET default
    TRISB = 0xff;

    // Set PORTC control signal direction and initial states
    // disable chip select
    LATC = 0xfd;

    // Set the SPI module for use by Stack
    TRISC = 0xD0;

    // Set the SPI module
    SSPSTAT = 0xC0;
    SSPCON1 = 0x20;

    // D1 and D2 are on RA0 and RA1 respectively, and CS of TC77 is on RA2.
    // Make PORTA as digital I/O.
    // The TC77 temp sensor CS is on RA2.
    ADCON1 = 0x0F;

    // Deselect TC77 (RA2)
    LATA = 0x04;

    // Make RA0, RA1, RA2 and RA4 as outputs.
    TRISA = 0xF8;
    
    PHY_CS = 1;             //deselect the MRF24J40
    PHY_CS_TRIS = 0;        //make chip select an output   
    
    RFIF = 0;               //clear the interrupt flag

    RCONbits.IPEN = 1;
    
    INTCON2bits.INTEDG0 = 0;
#elif defined(EXPLORER16)
    SPI1CON1 = 0b0000000100111110;
    SPI1STAT = 0x8000;
    
    PHY_RESETn = 0;
    PHY_RESETn_TRIS = 0;
    PHY_CS = 1;
    PHY_CS_TRIS = 0;
    LED_1_TRIS = 0;
    LED_2_TRIS = 0;
    PUSH_BUTTON_1_TRIS = 1;
    PUSH_BUTTON_2_TRIS = 1;
    RFIF = 0;
    RFIE = 1;
#else
    #error "Unknown demo board.  Please properly initialize the part for the board."
#endif
}
