/*****************************************************************************
 *
 *              Main.c -- user main program
 *
 *****************************************************************************
 * FileName:        main.c
 * Dependencies:
 * Processor:       PIC18F
 * Compiler:        C18 02.20.00 or higher
 * Linker:          MPLINK 03.40.00 or higher
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
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
 *****************************************************************************/

#include ".\Common\Console.h"
#include ".\MiWi\MiWi.h"
#include ".\MiWi\MRF24J40.h"
#include ".\MiWi\SymbolTime.h"

#if !defined(HI_TECH_C)
#pragma romdata longAddressLocation = 0x0E
#endif
ROM unsigned char myLongAddress[8] = {EUI_0,EUI_1,EUI_2,EUI_3,EUI_4,EUI_5,EUI_6,EUI_7};
#pragma romdata

#ifdef SUPPORT_SECURITY
#pragma romdata securityKey = 0x2A
ROM unsigned char mySecurityKey[16] = {SECURITY_KEY_0, SECURITY_KEY_1, SECURITY_KEY_2, SECURITY_KEY_3, SECURITY_KEY_4,
	SECURITY_KEY_5, SECURITY_KEY_6, SECURITY_KEY_7, SECURITY_KEY_8, SECURITY_KEY_9, SECURITY_KEY_10, SECURITY_KEY_11, 
	SECURITY_KEY_12, SECURITY_KEY_13, SECURITY_KEY_14, SECURITY_KEY_15};
#if !defined(HI_TECH_C)
#pragma romdata
#endif
#endif

#if defined(SUPPORT_SECURITY)
ROM unsigned char mySecurityLevel = SECURITY_LEVEL;
ROM unsigned char myKeySequenceNumber = KEY_SEQUENCE_NUMBER;
#endif

ROM BYTE availableChannels[AVAILABLE_CHANNELS_SIZE]={ALLOWED_CHANNELS};
ROM unsigned char myManufacturerString[]="Company ABC";


#define MAX_PA_OUTPUT 0x00	// 0dB output
//all values between 0x00 and 0x1F are valid and results in a -1.25dB output per unit above 0
#define MIN_PA_OUTPUT 0x1F	// -38.75dB output
#define MRF24J40PAOutputAdjust(a) {PHYSetLongRAMAddr(RFCTRL3,(a<<3));}


/********************** FUNCTION PROTOTYPES *************************/
void BoardInit(void);

/********************** CONSTANTS ***********************************/
#define MAX_DISCOVERY_ATTEMPTS 3
#define DEBOUNCE_TIME 0x00003FFF

#if !defined(P2P_ONLY)
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
        
    ConsoleInit();  
    BoardInit(); 
    MiWiInit();
    #if defined(PICDEMZ)
    INTCONbits.GIEH = 1;
    #elif defined(EXPLORER16)
    #else
        #error "Unknown board.  Please initialize board as required."
    #endif

    if(PUSH_BUTTON_1 == 0)
    {
        DumpNetworkTable();
        while(PUSH_BUTTON_1==0){}
        RejoinNetwork();
    }

    while(1)
    {
        MiWiTasks();
        
        if(MemberOfNetwork())
        {
            //If I am already part of the network
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
            
            #if defined(SUPPORT_CLUSTER_SOCKETS) || defined(SUPPORT_P2P_SOCKETS)
            if(requestedSocket == TRUE)
            {
                if(OpenSocketComplete())
                {
                    requestedSocket = FALSE;
                    
                    if(OpenSocketSuccessful())
                    {
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
            
            if(PUSH_BUTTON_1 == 0)
            {
                if(PUSH_BUTTON_1_pressed == FALSE)
                {
                    if(myFriend != 0xFF)
                    {                       
                        PUSH_BUTTON_1_pressed = TRUE;
                        
                        TxData = 11;
                        WriteData(USER_REPORT_TYPE);
                        WriteData(LIGHT_REPORT);
                        WriteData(LIGHT_TOGGLE);
                        //TODO: need a way to clear the buffer if they decide not to transmit
                        SendReportByHandle(myFriend,FALSE);
//                    ConsolePutROMString((ROM char*)"looking for node\r\n");
//                    tempLongAddress[0] = 0x91;
//                    tempLongAddress[1] = 0x78;
//                    tempLongAddress[2] = 0x56;
//                    tempLongAddress[3] = 0x34;
//                    tempLongAddress[4] = 0x12;
//                    tempLongAddress[5] = 0xA3;
//                    tempLongAddress[6] = 0x04;
//                    tempLongAddress[7] = 0x00;
//                    
//                    DiscoverNodeByEUI();
//
//						TxData = 11;
//                    WriteData(USER_REPORT_TYPE);
//                    WriteData(LIGHT_REPORT);
//                    WriteData(LIGHT_TOGGLE);

//                    tempLongAddress[0] = 0x99;
//                    tempLongAddress[1] = 0x78;
//                    tempLongAddress[2] = 0x56;
//                    tempLongAddress[3] = 0x34;
//                    tempLongAddress[4] = 0x12;
//                    tempLongAddress[5] = 0xA3;
//                    tempLongAddress[6] = 0x04;
//                    tempLongAddress[7] = 0x00;
//
//                    SendReportByLongAddress(tempLongAddress);
//                    if(myFriend != 0xFF)
//                    {
//                        SendReportByHandle(myFriend,FALSE);
//                    }
                    }
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
            
            if(PUSH_BUTTON_2 == 0)
            {
                if(PUSH_BUTTON_2_pressed == FALSE)
                {
                    PUSH_BUTTON_2_pressed = TRUE;
                    
                    requestedSocket = TRUE;
                    #if defined(SUPPORT_CLUSTER_SOCKETS)
                        OpenSocket(CLUSTER_SOCKET);
                    #elif defined(SUPPORT_P2P_SOCKETS)
                        ConsolePutROMString((ROM char*)"Opening P2P Socket\r\n");
                        OpenSocket(P2P_SOCKET);
                    #endif
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
            
            #if defined(I_AM_RFD)
                if(CheckForDataComplete())
                {
                    MRF24J40Sleep();
                    
                    #if defined(__18CXX)
                        WDTCONbits.SWDTEN = 1;      //enable the WDT to wake me up
                        INTCONbits.RBIE = 1;
                    #elif defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24H__)
                        RCONbits.SWDTEN = 1;        //enable the WDT to wake me up
                    #endif
                    
                    Sleep();            //goto sleep
                    
                    #if defined(__18CXX)
                        INTCONbits.RBIE = 0;
                        WDTCONbits.SWDTEN = 0;      //disable WDT
                    #elif defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24H__)
                        RCONbits.SWDTEN = 0;        //disable WDT
                    #endif

                    MRF24J40Wake();
                    
                    CheckForData();     //when I wake up do a data request
                }
            #endif
                        
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
                        #endif

                        #if !defined(P2P_ONLY)
                        //clear all of the network entries
                            ClearNetworkTable(CLEAR_NETWORKS);
                        #endif
                        
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

/**********************************************************************************
*
*                      DEMO code for P2P only devices
*
***********************************************************************************/
//DEMO for P2P Only devices
void main(void)
{
    BOOL PUSH_BUTTON_1_pressed;
    TICK PUSH_BUTTON_1_press_time, tickDifference;
    BOOL FoundSocket;
    BYTE myFriend;
    
    ConsoleInit();
    BoardInit();  
    MiWiInit();
    
    #if defined(PICDEMZ)
    INTCONbits.GIEH = 1;
    #elif defined(EXPLORER16)
    #else
        #error "Unknown board.  Please initialize board as required."
    #endif
    
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
 #if defined(PICDEMZ) && !defined(__PICC__)
void BoardInit(void)
{
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
}
#elif defined(__PICC__)
void BoardInit(void)
{
    // Switches S2 and S3 are on RB5 and RB4 respectively. We want interrupt-on-change
    INTCON = 0x00;

    // Make PORTB as input - this is the RESET default
    TRISB = 0xff;

    INTEDG = 0;
    
    // Set the SPI module
    SSPSTAT = 0xC0;
    SSPCON = 0x20;

    // D1 and D2 are on RA0 and RA1 respectively, and CS of TC77 is on RA2.
    // Make PORTA as digital I/O.
    // The TC77 temp sensor CS is on RA2.
    ADCON1 = 0x0F;

    // Make RA0, RA1, RA2 and RA4 as outputs.
    TRISA = 0xF8;
    
    TRISC = 0xD6;
    
    PHY_CS = 1;             //deselect the MRF24J40
    PHY_CS_TRIS = 0;        //make chip select an output   
    
    RFIF = 0;               //clear the interrupt flag
    
    if(RB0 == 0)
    {
        RFIF = 1;
    }
}
#elif defined(EXPLORER16)
void BoardInit(void)
{
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
}
#else
    #error "Unknown demo board.  Please properly initialize the part for the board."
#endif
