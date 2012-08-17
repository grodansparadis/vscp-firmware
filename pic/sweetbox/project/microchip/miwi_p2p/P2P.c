/********************************************************************
* FileName:		
* Dependencies:    
* Processor:	PIC18, PIC24F, PIC24H, dsPIC30, dsPIC33
*               tested with 18F4620, dsPIC33FJ256GP710	
* Hardware:		PICDEM Z, Explorer 16
* Complier:     Microchip C18 v3.04 or higher
*				Microchip C30 v2.03 or higher		
* Company:		Microchip Technology, Inc.
*
* Copyright and Disclaimer Notice for P2P Software:
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
*  This is the P2P stack  
*
* Change History:
*  Rev   Date         Author    Description
*  0.1   1/03/2008    yfy       Initial revision
********************************************************************/

/************************ HEADERS **********************************/
#include ".\P2P\MSPI.h"
#include ".\P2P\P2P.h"
#include ".\P2P\MRF24J40.h"
#include "Compiler.h"
#include "GenericTypeDefs.h"
#include ".\Common\Console.h"
#include ".\P2P\SymbolTime.h"
#include "P2PDefs.h"

/************************ VARIABLES ********************************/
#pragma romdata longAddressLocation = 0x0E
    ROM unsigned char myLongAddress[8] = {EUI_0,EUI_1,EUI_2,EUI_3,EUI_4,EUI_5,EUI_6,EUI_7}; // The extended long address
                                                                                            // for the device
#pragma romdata

#ifdef ENABLE_SECURITY
    #pragma romdata securityKey = 0x2E
        ROM unsigned char mySecurityKey[16] = {SECURITY_KEY_00, SECURITY_KEY_01, SECURITY_KEY_02, SECURITY_KEY_03, SECURITY_KEY_04,
	        SECURITY_KEY_05, SECURITY_KEY_06, SECURITY_KEY_07, SECURITY_KEY_08, SECURITY_KEY_09, SECURITY_KEY_10, SECURITY_KEY_11, 
	        SECURITY_KEY_12, SECURITY_KEY_13, SECURITY_KEY_14, SECURITY_KEY_15}; // The 16-byte security key used in the 
	                                                                             // security module. 
    #pragma romdata
    ROM unsigned char mySecurityLevel = SECURITY_LEVEL;             // The security level used in the security module. 
                                                                    // The security level is defined in IEEE 802.15.4 specification
                                                                    // It can be from 1 to 7.
    ROM unsigned char myKeySequenceNumber = KEY_SEQUENCE_NUMBER;    // The sequence number of security key. Used to identify
                                                                    // the security key
	
	DWORD_VAL OutgoingFrameCounter;                                
#endif	

#if defined(ENABLE_ED_SCAN) || defined(ENABLE_ACTIVE_SCAN) || defined(ENABLE_FREQUENCY_AGILITY)
    // Scan Duration formula for P2P Connection:
    //  60 * (2 ^ n + 1) symbols
    #define SCAN_DURATION_0 SYMBOLS_TO_TICKS(120)
    #define SCAN_DURATION_1 SYMBOLS_TO_TICKS(180)
    #define SCAN_DURATION_2 SYMBOLS_TO_TICKS(300)
    #define SCAN_DURATION_3 SYMBOLS_TO_TICKS(540)
    #define SCAN_DURATION_4 SYMBOLS_TO_TICKS(1020)
    #define SCAN_DURATION_5 SYMBOLS_TO_TICKS(1980)
    #define SCAN_DURATION_6 SYMBOLS_TO_TICKS(3900)
    #define SCAN_DURATION_7 SYMBOLS_TO_TICKS(7740)
    #define SCAN_DURATION_8 SYMBOLS_TO_TICKS(15420)
    #define SCAN_DURATION_9 SYMBOLS_TO_TICKS(30780)
    #define SCAN_DURATION_10 SYMBOLS_TO_TICKS(61500)
    #define SCAN_DURATION_11 SYMBOLS_TO_TICKS(122940)
    #define SCAN_DURATION_12 SYMBOLS_TO_TICKS(245820)
    #define SCAN_DURATION_13 SYMBOLS_TO_TICKS(491580)
    #define SCAN_DURATION_14 SYMBOLS_TO_TICKS(983100)
    const ROM DWORD ScanTime[15] = {SCAN_DURATION_0,SCAN_DURATION_1,SCAN_DURATION_2,SCAN_DURATION_3,
        SCAN_DURATION_4,SCAN_DURATION_5,SCAN_DURATION_6,SCAN_DURATION_7,SCAN_DURATION_8,SCAN_DURATION_9,
        SCAN_DURATION_10, SCAN_DURATION_11, SCAN_DURATION_12, SCAN_DURATION_13,SCAN_DURATION_14
    };
#endif
#if defined(__18CXX)
#pragma udata TRX_BUFFER=0xA00
#endif
BYTE RxBuffer[RX_BUFFER_SIZE];
BYTE TxBuffer[TX_BUFFER_SIZE];
#if defined(__18CXX)
#pragma udata
#endif

#if defined(__18CXX)
#pragma udata INDIRECT_BUFFER=0xB00
#endif
#ifdef ENABLE_INDIRECT_MESSAGE
    INDIRECT_MESSAGE indirectMessages[INDIRECT_MESSAGE_SIZE];   // structure to store the indirect messages
                                                                // for nodes with radio off duing idle time
#endif
#if defined(__18CXX)
#pragma udata
#endif

BYTE TxData;
BYTE IEEESeqNum;


#if defined(__18CXX)
#pragma udata BIGvariables1 = 0x900
#endif
P2P_CONNECTION_ENTRY    P2PConnections[P2P_CONNECTION_SIZE];        // The peer device records for P2P connections
#if defined(__18CXX)
#pragma udata
#endif

WORD_VAL myPANID;                   // the PAN Identifier for the device
volatile BYTE failureCounter = 0;
BYTE currentChannel;                // current operating channel for the device

RECEIVED_FRAME rxFrame;             // structure to store information for the received packet
BYTE    LatestConnection;
volatile P2P_STATUS P2PStatus;
extern BYTE AdditionalConnectionPayload[];  // the additional information regarding the device
                                            // that would like to share with the peer on the 
                                            // other side of P2P connection. This information 
                                            // is applicaiton specific. 
extern BYTE myChannel;

BYTE AddConnection(void);
BOOL isSameAddress(INPUT BYTE *Address1, INPUT BYTE *Address2);
BOOL SendPacket(INPUT BOOL Broadcast, INPUT WORD_VAL DestinationPANID, 
                INPUT BYTE *DestinationAddress, INPUT BOOL isCommand, 
                INPUT BOOL SecurityEnabled);


#ifdef ENABLE_ACTIVE_SCAN
    BYTE    ActiveScanResultIndex;
    ACTIVE_SCAN_RESULT ActiveScanResults[ACTIVE_SCAN_RESULT_SIZE];  // The results for active scan, including
                                                                    // the PAN identifier, signal strength and 
                                                                    // operating channel
#endif

#ifdef VERIFY_TRANSMIT
    BYTE CCAFailureTimes = 0;   // Continuous failure times because of CSMA-CA failure
    BYTE AckFailureTimes = 0;   // Continuous failure times because of no acknowledgement received
#endif
    
#ifdef ENABLE_FREQUENCY_AGILITY
    void StartChannelHopping(INPUT BYTE OptimalChannel);
#endif

#ifdef ENABLE_SLEEP
    TICK DataRequestTimer;
#endif

/************************ FUNCTIONS ********************************/

/*********************************************************************
 * void P2PTasks( void )
 *
 * Overview:        This function maintains the operation of the stack
 *                  It should be called as often as possible. 
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    The stack receives, handles, buffers, and transmits 
 *                  packets.  It also handles all of the joining 
 * 
 ********************************************************************/
void P2PTasks(void)
{
    BYTE i;

    //set the interrupt flag just in case the interrupt was missed
    if(RF_INT_PIN == 0)
    {
        RFIF = 1;
    }
    
    //If the stack TX has been busy for a long time then
    //time out the TX because we may have missed the interrupt 
    //and don't want to lock up the stack forever
    if(P2PStatus.bits.TX_BUSY)
    {
        if(failureCounter == 0x20)
        {
            failureCounter = 0;
            P2PStatus.bits.TX_BUSY = 0;
        }
        else
        {
            failureCounter++;
        }
    }

    #ifdef ENABLE_INDIRECT_MESSAGE
        for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
        {
            if( indirectMessages[i].flags.bits.isValid )
            {
                TICK currentTick = TickGet();
                if( TickGetDiff(currentTick, indirectMessages[i].TickStart) > INDIRECT_MESSAGE_TIMEOUT * ONE_SECOND )
                {
                    indirectMessages[i].flags.Val = 0x00;   
                    printf("\r\nIndirect message expired");
                }    
            }    
        }
    #endif
    
    
    #ifdef ENABLE_SLEEP
        if( P2PStatus.bits.DataRequesting )
        {
            TICK currentTick = TickGet();
            if( TickGetDiff(currentTick, DataRequestTimer) > RFD_DATA_WAIT )
            {
                printf("\r\nData Request Expired");
                P2PStatus.bits.DataRequesting = 0;
            }
        }
    #endif

    //if there is a message pending and there isn't one already buffered
    if((P2PStatus.bits.RX_PENDING == 1) && (P2PStatus.bits.RX_BUFFERED == 0))
    {        
        while( P2PStatus.bits.TX_BUSY ) 
        {
            if(RF_INT_PIN == 0)
            {
                RFIF = 1;
            }
        }
  
        //disable the RF interrupt       
        RFIE = 0;     
        // disable the radio to receive new
        // packet
        PHYSetShortRAMAddr(WRITE_BBREG1, 0x04);
        
        // bypass MRF24J40 errata 5
        //PHYSetShortRAMAddr(WRITE_FFOEN, 0x08);
               
        //get the size of the packet
        //2 more bytes for RSSI and LQI reading 
        RxBuffer[0] = PHYGetLongRAMAddr(0x300) + 2;

        if(RxBuffer[0]<RX_BUFFER_SIZE)
        {
            //disable the ACKs until the buffer is cleared
            PHYSetShortRAMAddr(WRITE_RXMCR,0x20); 
            
            //indicate that data is now stored in the buffer
            P2PStatus.bits.RX_BUFFERED = 1;            
            
            //Disable RX of packets until this one is cleared
            P2PStatus.bits.RX_ENABLED = 0;
            //Clear the pending flag indicating that we removed the packet from the buffer
            P2PStatus.bits.RX_PENDING = 0;
            
            //copy all of the data from the FIFO into the TxBuffer, plus RSSI and LQI
            for(i=1;i<=RxBuffer[0]+2;i++)
            {
                RxBuffer[i] = PHYGetLongRAMAddr(0x300+i);
            }
            
            RFIE = 1;
        }
        else
        {
            //else it was a larger packet than we can receive
            //flush it
            PHYSetShortRAMAddr(WRITE_RXFLUSH,0x01); 
            P2PStatus.bits.RX_PENDING = 0;
        }
        
        // bypass MRF24J40 errata 5
        //PHYSetShortRAMAddr(WRITE_FFOEN, 0x88);
        
        // enable radio to receive next packet
        PHYSetShortRAMAddr(WRITE_BBREG1, 0x00);
    }
    
    if(P2PStatus.bits.RX_BUFFERED == 1)
    {
        BYTE addrMode;
        
        rxFrame.flags.Val = 0;
        //Determine the start of the MAC payload
        addrMode = RxBuffer[2] & 0xCC;
        switch(addrMode)
        {
            case 0xC8: //short dest, long source
                // for P2P only broadcast allows short destination address
                if( RxBuffer[6] == 0xFF && RxBuffer[7] == 0xFF )
                {
                    rxFrame.flags.bits.broadcast = 1;
                    #ifndef TARGET_SMALL
                        if( RxBuffer[1]&0x40 ) // check if it is intraPAN
                    #endif
                    {
                        rxFrame.flags.bits.intraPAN = 1;
                        #ifndef TARGET_SMALL
                            rxFrame.SourcePANID.v[0] = RxBuffer[4];
                            rxFrame.SourcePANID.v[1] = RxBuffer[5];
                        #endif
                        for(i = 0; i < 8; i++)
                        {
                            rxFrame.SourceLongAddress[i] = RxBuffer[8 + i];
                        }
                        
                        rxFrame.PayLoadSize = RxBuffer[0] - 19;
                        rxFrame.PayLoad = &(RxBuffer[16]);   
                    }
                    #ifndef TARGET_SMALL
                        else
                        {
                            rxFrame.SourcePANID.v[0] = RxBuffer[8];
                            rxFrame.SourcePANID.v[1] = RxBuffer[9];
                            for(i = 0; i < 8; i++)
                            {
                                rxFrame.SourceLongAddress[i] = RxBuffer[10 + i];
                            }
                            rxFrame.PayLoadSize = RxBuffer[0] - 21;
                            rxFrame.PayLoad = &(RxBuffer[18]);
                        }
                    #endif
                }
                break;
            
            case 0xCC: // long dest, long source
                #ifndef TARGET_SMALL
                    if( RxBuffer[1] & 0x40 ) // check if it is intraPAN
                #endif
                {
                    rxFrame.flags.bits.intraPAN = 1;
                    #ifndef TARGET_SMALL
                        rxFrame.SourcePANID.v[0] = RxBuffer[4];
                        rxFrame.SourcePANID.v[1] = RxBuffer[5];
                    #endif
                    for(i = 0; i < 8; i++)
                    {
                        rxFrame.SourceLongAddress[i] = RxBuffer[14 + i];
                    }

                    rxFrame.PayLoadSize = RxBuffer[0] - 25;
                    rxFrame.PayLoad = &(RxBuffer[22]);    
                } 
                #ifndef TARGET_SMALL
                    else
                    {
                        rxFrame.SourcePANID.v[0] = RxBuffer[14];
                        rxFrame.SourcePANID.v[1] = RxBuffer[15];
                        for(i = 0; i < 8; i++)
                        {
                            rxFrame.SourceLongAddress[i] = RxBuffer[16 + i];
                        }
                        rxFrame.PayLoadSize = RxBuffer[0] - 27;
                        rxFrame.PayLoad = &(RxBuffer[24]);
                    }
                #endif
                break;
            case 0x88: // short dest, short source
            case 0x80: //dest-none , source-short
            case 0x08: //dest-short, source-none
            case 0x8C: //long dest, short source
                // all other addressing mode will not be supported in P2P
            default:
                // not valid addressing mode or no addressing info
                DiscardPacket();
                return;             
        }  
        
        #ifdef ENABLE_SECURITY
            if( RxBuffer[1] & 0x08 )
            {
                // if security is used, remove the security header and MIC from
                // the payload size
                switch(mySecurityLevel)
                {
                    case 0x02:
                    case 0x05:
                        rxFrame.PayLoadSize -= 21;
                        break;
                    case 0x03:
                    case 0x06:
                        rxFrame.PayLoadSize -= 13;
                        break;
                    case 0x04:
                    case 0x07:
                        rxFrame.PayLoadSize -= 9;
                        break;
                    case 0x01:
                        rxFrame.PayLoadSize -= 5;
                        break;
                    default:
                        break;    
                }
                // remove the security header from the payload 
                rxFrame.PayLoad = &(rxFrame.PayLoad[5]);
                rxFrame.flags.bits.security = 1;
            }
        #endif
        P2PStatus.bits.RxHasUserData = 0;
        
        // check the frame type. Only the data and command frame type
        // are supported. Acknowledgement frame type is handled in 
        // MRF24J40 transceiver hardware.
        switch( RxBuffer[1] & 0x07 ) // check frame type
        {
            case 0x01:  // data frame
                P2PStatus.bits.RxHasUserData = 1;
                break;
            case 0x03:  // command frame
                rxFrame.flags.bits.commandFrame = 1;
                break;
            default:    // not support frame type
                DiscardPacket();
                return;
        }
        
        #ifndef TARGET_SMALL
            rxFrame.PacketLQI = RxBuffer[RxBuffer[0]-1];
            rxFrame.PacketRSSI = RxBuffer[RxBuffer[0]];
        #endif

        if( P2PStatus.bits.RxHasUserData == 0 )
        {
            // if comes here, we know it is a command frame
            switch( rxFrame.PayLoad[0] )
            {
                case CMD_P2P_CONNECTION_REQUEST:
                    {
                        BYTE status = STATUS_SUCCESS;
                        
                        // if a device goes to sleep, it can only have one
                        // connection, as the result, it cannot accept new
                        // connection request
                        #ifdef ENABLE_SLEEP
                            break;
                        #endif
                        
                        // if channel does not math, it may be a 
                        // sub-harmonics signal, ignore the request
                        if( currentChannel != rxFrame.PayLoad[1] )
                        {
                            break;
                        }
                        
                        #ifndef TARGET_SMALL
                            // if PANID does not match, ignore the request
                            if( rxFrame.SourcePANID.Val != 0xFFFF &&
                                rxFrame.SourcePANID.Val != myPANID.Val )
                            {
                                status = STATUS_NOT_SAME_PAN;
                            }
                            else
                        #endif
                        {
                            // request accepted, try to add the requesting
                            // device into P2P Connection Entry
                            status = AddConnection();
                        }
                        
                        // if new connection is not allowed, ignore 
                        // the request
                        if( P2PStatus.bits.EnableNewConnection == 0 &&
                            status != STATUS_EXISTS &&
                            status != STATUS_ACTIVE_SCAN)
                        {
                            break;
                        }
                        
                        
                        // prepare the P2P_CONNECTION_RESPONSE command
                        FlushTx();
                        WriteData(CMD_P2P_CONNECTION_RESPONSE);
                        WriteData(status);
                        if( status == STATUS_SUCCESS ||
                            status == STATUS_EXISTS )
                        {
                            WriteData(P2P_CAPACITY_INFO);
                            #if ADDITIONAL_CONNECTION_PAYLOAD > 0
                                for(i = 0; i < ADDITIONAL_CONNECTION_PAYLOAD; i++)
                                {
                                    WriteData(AdditionalConnectionPayload[i]);
                                }
                            #endif
                        }
                        
                        // unicast the response to the requesting device
                        #ifdef TARGET_SMALL
                            SendPacket(FALSE, myPANID, rxFrame.SourceLongAddress, TRUE, rxFrame.flags.bits.security);       
                        #else
                            SendPacket(FALSE, rxFrame.SourcePANID, rxFrame.SourceLongAddress, TRUE, rxFrame.flags.bits.security);
                        #endif
                    }
                    break;
                
                #ifndef TARGET_SMALL    
                    case CMD_P2P_CONNECTION_REMOVAL_REQUEST:
                        {
                            FlushTx();
                            WriteData(CMD_P2P_CONNECTION_REMOVAL_RESPONSE);
                                
                            for(i = 0; i < P2P_CONNECTION_SIZE; i++)
                            {
                                // if the record is valid
                                if( P2PConnections[i].PeerInfo[0] & 0x80 )
                                {
                                    // if the record is the same as the requesting device
                                    if( isSameAddress(rxFrame.SourceLongAddress, P2PConnections[i].PeerLongAddress) )
                                    {
                                        // find the record. disable the record and
                                        // set status to be SUCCESS
                                        P2PConnections[i].PeerInfo[0] = 0;
                                        WriteData(STATUS_SUCCESS);
                                        break;
                                    }
                                } 
                            }
                            
                            if( i == P2P_CONNECTION_SIZE ) 
                            {
                                // not found, the requesting device is not my peer
                                WriteData(STATUS_ENTRY_NOT_EXIST);
                            }
                            #ifdef TARGET_SMALL
                                SendPacket(FALSE, myPANID, rxFrame.SourceLongAddress, TRUE, rxFrame.flags.bits.security);
                            #else
                                SendPacket(FALSE, rxFrame.SourcePANID, rxFrame.SourceLongAddress, TRUE, rxFrame.flags.bits.security);
                            #endif
                        }
                        break;
                #endif
                
                case CMD_P2P_CONNECTION_RESPONSE:
                    {
                        switch( rxFrame.PayLoad[1] )
                        {
                            case STATUS_SUCCESS:
                            case STATUS_EXISTS:
                                AddConnection();
                                break;
                            
                            case STATUS_ACTIVE_SCAN:
                                if( P2PStatus.bits.Resync )
                                {
                                    P2PStatus.bits.Resync = 0;   
                                }
                                #ifdef ENABLE_ACTIVE_SCAN 
                                    else   
                                    {
                                        for(i = 0; i < ActiveScanResultIndex; i++)
                                        {
                                            if( ActiveScanResults[ActiveScanResultIndex].Channel == (currentChannel + 11) &&
                                                ActiveScanResults[ActiveScanResultIndex].PANID.Val == rxFrame.SourcePANID.Val )
                                            {
                                                if( ActiveScanResults[ActiveScanResultIndex].RSSIValue < rxFrame.PacketRSSI )
                                                {
                                                    ActiveScanResults[ActiveScanResultIndex].RSSIValue = rxFrame.PacketRSSI;
                                                }
                                                break;
                                            }
                                        }
                                        if( i == ActiveScanResultIndex && (i < ACTIVE_SCAN_RESULT_SIZE))
                                        {
                                            ActiveScanResults[ActiveScanResultIndex].Channel = currentChannel;
                                            ActiveScanResults[ActiveScanResultIndex].RSSIValue = rxFrame.PacketRSSI;
                                            ActiveScanResults[ActiveScanResultIndex].PANID.Val = rxFrame.SourcePANID.Val;
                                            ActiveScanResultIndex++;
                                        }
                                    }
                                #endif
                                break;
                                
                            default:
                                break;
                        }
                                                  
                    }
                    break; 
                    
                #ifndef TARGET_SMALL
                    case CMD_P2P_CONNECTION_REMOVAL_RESPONSE:
                    {
                        if( rxFrame.PayLoad[1] == STATUS_SUCCESS )
                        {
                            for(i = 0; i < P2P_CONNECTION_SIZE; i++)
                            {
                                // if the record is valid
                                if( P2PConnections[i].PeerInfo[0] & 0x80 )
                                {
                                    // if the record address is the same as the requesting device
                                    if( isSameAddress(rxFrame.SourceLongAddress, P2PConnections[i].PeerLongAddress) )
                                    {
                                        // invalidate the record
                                        P2PConnections[i].PeerInfo[0] = 0;
                                        break;
                                    }
                                } 
                            }
                        }
                    }
                    
                    break;
                #endif
                
                #ifdef ENABLE_INDIRECT_MESSAGE
                    case CMD_DATA_REQUEST:
                    case CMD_MAC_DATA_REQUEST: 
                        FlushTx();
                        for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
                        {
                            if( indirectMessages[i].flags.bits.isValid )
                            {    
                                BYTE j;
                                
                                #ifdef ENABLE_BROADCAST
                                    if( indirectMessages[i].flags.bits.isBroadcast )
                                    {
                                        for(j = 0; j < P2P_CONNECTION_SIZE; j++)
                                        {
                                            if( indirectMessages[i].DestAddress.DestIndex[j] != 0xFF &&
                                                isSameAddress(P2PConnections[indirectMessages[i].DestAddress.DestIndex[j]].PeerLongAddress, rxFrame.SourceLongAddress) )
                                            {
                                                indirectMessages[i].DestAddress.DestIndex[j] = 0xFF;
                                                for(j = 0; j < indirectMessages[i].PayLoadSize; j++)
                                                {
                                                    WriteData(indirectMessages[i].PayLoad[j]);
                                                }
                                                SendPacket(FALSE, indirectMessages[i].DestPANID, rxFrame.SourceLongAddress, indirectMessages[i].flags.bits.isCommand, indirectMessages[i].flags.bits.isSecured);
                                                goto DiscardPacketHere;
                                            }
                                        }         
                                    }
                                    else 
                                #endif
                                if( isSameAddress(indirectMessages[i].DestAddress.DestLongAddress, rxFrame.SourceLongAddress) )
                                {                          
                                    for(j = 0; j < indirectMessages[i].PayLoadSize; j++)
                                    {
                                        WriteData(indirectMessages[i].PayLoad[j]);
                                    }
                                    SendPacket(FALSE, indirectMessages[i].DestPANID, indirectMessages[i].DestAddress.DestLongAddress, (BOOL)indirectMessages[i].flags.bits.isCommand, (BOOL)indirectMessages[i].flags.bits.isSecured);
                                    indirectMessages[i].flags.Val = 0;   
                                    break;    
                                }    
                            }
                        }
                       
                        if( i == INDIRECT_MESSAGE_SIZE )
                        {
                            #ifdef TARGET_SMALL
                                SendPacket(FALSE, myPANID, rxFrame.SourceLongAddress, FALSE, FALSE);
                            #else
                                SendPacket(FALSE, rxFrame.SourcePANID, rxFrame.SourceLongAddress, FALSE, FALSE);
                            #endif
                        }
                        break;
                #endif
                     
                #if defined(ENABLE_FREQUENCY_AGILITY) 
                    case CMD_CHANNEL_HOPPING:
                        if( rxFrame.PayLoad[1] != currentChannel )
                        {
                            break;
                        }
                        StartChannelHopping(rxFrame.PayLoad[2]);
                        printf("\r\nHopping Channel to ");
                        PrintDec((currentChannel>>4) + 11);
                        break;
                    
                #endif
                                      
                default:
                    // let upper application layer to handle undefined command frame
                    P2PStatus.bits.RxHasUserData = 1;
                    break;
            }
        }
        
        #ifdef ENABLE_SLEEP
            if( P2PStatus.bits.DataRequesting && P2PStatus.bits.RxHasUserData )
            {
                P2PStatus.bits.DataRequesting = 0;
            }
        #endif
        
        if( rxFrame.PayLoadSize == 0 )
        {
            P2PStatus.bits.RxHasUserData = 0;
        }

DiscardPacketHere:        
        if(P2PStatus.bits.RxHasUserData == 0)
        {
            DiscardPacket();
        }
    }   

}



/*********************************************************************
 * void DiscardPacket(void)
 *
 * Overview:        This function needs to be called to discard, clear
 *                  and reset the Rx module of the MRF24J40.  This 
 *                  function needs to be called after the user is done
 *                  processing a received packet
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    The RxBuffer gets flushed and the MRF24J40 is allowed
 *                  to receive new packet again.
 *
 ********************************************************************/
void DiscardPacket(void)
{
    P2PStatus.bits.RxHasUserData = 0;
    
    //re-enable the ACKS
    RFIE = 0;
    PHYSetShortRAMAddr(WRITE_RXMCR,0x00);
    P2PStatus.bits.RX_BUFFERED = 0;
    P2PStatus.bits.RX_ENABLED = 1;
    RFIE = 1;
}



/*********************************************************************
 * void PHYSetLongRAMAddr(INPUT WORD address, INPUT BYTE value)
 *
 * Overview:        This function writes a value to a LONG RAM address
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           address - the address of the LONG RAM address
 *                      that you want to write to
 *                  value   - the value that you want to write to
 *                      that register
 *
 * Output:          None
 *
 * Side Effects:    The register value is changed
 *                  Interrupt from radio is turned off before accessing
 *                  the SPI and turned back on after accessing the SPI
 *
 ********************************************************************/
void PHYSetLongRAMAddr(INPUT WORD address, INPUT BYTE value)
{
    volatile BYTE tmpRFIE = RFIE;
    
    RFIE = 0;
    PHY_CS = 0;
    SPIPut((((BYTE)(address>>3))&0x7F)|0x80);
    SPIPut((((BYTE)(address<<5))&0xE0)|0x10);
    SPIPut(value);
    PHY_CS = 1;
    RFIE = tmpRFIE;
}

/*********************************************************************
 * void PHYSetShortRAMAddr(INPUT BYTE address, INPUT BYTE value)
 *
 * Overview:        This function writes a value to a short RAM address
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           address - the address of the short RAM address
 *                      that you want to write to.  Should use the
 *                      WRITE_ThisAddress definition in the MRF24J40 
 *                      include file.
 *                  value   - the value that you want to write to
 *                      that register
 *
 * Output:          None
 *
 * Side Effects:    The register value is changed
 *                  Interrupt from radio is turned off before accessing
 *                  the SPI and turned back on after accessing the SPI
 *
 ********************************************************************/
void PHYSetShortRAMAddr(INPUT BYTE address, INPUT BYTE value)
{
    volatile BYTE tmpRFIE = RFIE;
    
    RFIE = 0;
    PHY_CS = 0;     
    SPIPut(address);
    SPIPut(value);
    PHY_CS = 1;
    RFIE = tmpRFIE;
}

/*********************************************************************
 * BYTE PHYGetShortRAMAddr(INPUT BYTE address)
 *
 * Overview:        This function reads a value from a short RAM address
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           address - the address of the short RAM address
 *                      that you want to read from.  Should use the
 *                      READ_ThisAddress definition in the MRF24J40 
 *                      include file.
 *
 * Output:          None
 *
 * Side Effects:    Interrupt from radio is turned off before accessing
 *                  the SPI and turned back on after accessing the SPI
 *
 ********************************************************************/
BYTE PHYGetShortRAMAddr(INPUT BYTE address)
{
    BYTE toReturn;
    volatile BYTE tmpRFIE = RFIE;
    
    RFIE = 0;
    PHY_CS = 0;
    SPIPut(address);
    toReturn = SPIGet();
    PHY_CS = 1;
    RFIE = tmpRFIE;
    
    return toReturn;
}

/*********************************************************************
 * BYTE PHYGetLongRAMAddr(INPUT WORD address)
 *
 * Overview:        This function reads a value from a long RAM address
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           address - the address of the long RAM address
 *                      that you want to read from.  
 *
 * Output:          the value read from the specified Long register
 *
 * Side Effects:    Interrupt from radio is turned off before accessing
 *                  the SPI and turned back on after accessing the SPI
 *
 ********************************************************************/
BYTE PHYGetLongRAMAddr(INPUT WORD address)
{
    BYTE toReturn;
    volatile BYTE tmpRFIE = RFIE;
    
    RFIE = 0;
    PHY_CS = 0;
    SPIPut(((address>>3)&0x7F)|0x80);
    SPIPut(((address<<5)&0xE0));
    toReturn = SPIGet();
    PHY_CS = 1;
    RFIE = tmpRFIE;
    
    return toReturn;
}




/*********************************************************************
 * void SetChannel(INPUT BYTE channel)
 *
 * Overview:        This function sets the current operating channel
 *                  of the MRF24J40
 *
 * PreCondition:    MRF24J40 is initialized
 *
 * Input:           channel - this is the channel that you wish
 *                  to operate on.  This should be CHANNEL_11, CHANNEL_12,
 *                  ..., CHANNEL_26. 
 *
 * Output:          None
 *
 * Side Effects:    the MRF24J40 now operates on the input channel
 *
 ********************************************************************/
void SetChannel(INPUT BYTE channel)
{   
    currentChannel = channel;
    PHYSetLongRAMAddr(RFCTRL0,channel|0x02);
    PHYSetShortRAMAddr(WRITE_RFCTL,0x04);
    PHYSetShortRAMAddr(WRITE_RFCTL,0x00);  
}


/*********************************************************************
 * void initMRF24J40(void)
 *
 * Overview:        This function initializes the MRF24J40 and is required
 *                  before stack operation is available
 *
 * PreCondition:    BoardInit (or other initialzation code is required)
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    MRF24J40 is initialized
 *
 ********************************************************************/
void initMRF24J40(void)
{
    BYTE i;
    WORD j;
    
    PHY_RESETn = 0;
    for(j=0;j<(WORD)300;j++){}

    PHY_RESETn = 1;
    for(j=0;j<(WORD)300;j++){}
  
    /* do a soft reset */
    PHYSetShortRAMAddr(WRITE_SOFTRST,0x07);
    do
    {
        i = PHYGetShortRAMAddr(READ_SOFTRST);
    }
    while((i&0x07) != (BYTE)0x00);   

    for(j=0;j<(WORD)1000;j++){}
 
    /* flush the RX fifo */
    PHYSetShortRAMAddr(WRITE_RXFLUSH,0x01);
    
    /* Program the short MAC Address, 0xffff */
    PHYSetShortRAMAddr(WRITE_SADRL,0xFF);
    PHYSetShortRAMAddr(WRITE_SADRH,0xFF);
    PHYSetShortRAMAddr(WRITE_PANIDL,myPANID.v[0]);
    PHYSetShortRAMAddr(WRITE_PANIDH,myPANID.v[1]);
    
    /* Program Long MAC Address*/
    for(i=0;i<(BYTE)8;i++)
    {
        PHYSetShortRAMAddr(WRITE_EADR0+i*2,myLongAddress[i]);
    }
    
    /* select the correct channel */
    PHYSetLongRAMAddr(RFCTRL0,0x02);    
    
    /* setup */
    PHYSetLongRAMAddr(RFCTRL2,0x80);
    
    // power level to be 0dBms
    PHYSetLongRAMAddr(RFCTRL3,0x00);
    /* program RSSI ADC with 2.5 MHz clock */
    PHYSetLongRAMAddr(RFCTRL6,0x90);
    
    PHYSetLongRAMAddr(RFCTRL7,0x80);
    
    PHYSetLongRAMAddr(RFCTRL8,0x10); 
    
    PHYSetLongRAMAddr(SCLKDIV, 0x01);

    /* Program CCA mode using RSSI */
    PHYSetShortRAMAddr(WRITE_BBREG2,0x80);
    /* Enable the packet RSSI */
    PHYSetShortRAMAddr(WRITE_BBREG6,0x40);
    /* Program CCA, RSSI threshold values */
    PHYSetShortRAMAddr(WRITE_RSSITHCCA,0x60);

    PHYSetShortRAMAddr(WRITE_FFOEN, 0x98);
    PHYSetShortRAMAddr(WRITE_TXPEMISP, 0x95);

    do
    {
        i = PHYGetLongRAMAddr(RFSTATE);
    }
    while((i&0xA0) != 0xA0);
    
    PHYSetShortRAMAddr(WRITE_INTMSK,0xE6);

    #ifdef ENABLE_INDIRECT_MESSAGE
        PHYSetShortRAMAddr(WRITE_ACKTMOUT, 0xB9);
    #endif
    
    // bypass the errata to make RF communication stable
    PHYSetLongRAMAddr(RFCTRL1, 0x01);
  
    // Define TURBO_MODE if more bandwidth if required
    // to enable radio to operate to TX/RX maximum 
    // 625Kbps
    #ifdef TURBO_MODE
    
        PHYSetShortRAMAddr(WRITE_BBREG0, 0x01);
        PHYSetShortRAMAddr(WRITE_BBREG3, 0x38);
        PHYSetShortRAMAddr(WRITE_BBREG4, 0x5C);
        
        PHYSetShortRAMAddr(WRITE_RFCTL,0x04);
        PHYSetShortRAMAddr(WRITE_RFCTL,0x00);

    #endif
}

/*********************************************************************
 * void P2PInit(void)
 *
 * Overview:        This function initializes the P2P stack and is required
 *                  before stack operation is available
 *
 * PreCondition:    BoardInit (or other initialzation code is required)
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    the P2P stack is initialized
 *
 ********************************************************************/
 void P2PInit(void)
{
    BYTE i;
    
    myPANID.Val = MY_PAN_ID;
    
    initMRF24J40();
    
    //clear all status bits
    P2PStatus.Val = 0;
    //enable reception
    P2PStatus.bits.RX_ENABLED = 1;
    //P2PStatus.bits.RX_BUFFERED = 0;
    P2PStatus.bits.AckRequired = 1;

    for(i = 0; i < P2P_CONNECTION_SIZE; i++)
    {
        P2PConnections[i].PeerInfo[0] = 0;
    }
 
    InitSymbolTimer();
    
    TxData = 0;

    IEEESeqNum = TMRL;

    RFIF = 0;
    RFIE = 1;

    if(RF_INT_PIN == 0)
    {
        RFIF = 1;
    }
    
    #ifdef ENABLE_INDIRECT_MESSAGE
        for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
        {
            indirectMessages[i].flags.Val = 0;
        }
    #endif
    
    #ifdef ENABLE_SECURITY
    	OutgoingFrameCounter.Val = 1;
    #endif    
}

#ifdef ENABLE_SLEEP
    /*********************************************************************
    * void MRF24J40Sleep(void)
    *
    * Overview:        Put the MRF24J40 radio into sleep
    *
    * PreCondition:    BoardInit (or other initialzation code is required)
    *
    * Input:           None
    *
    * Output:          None
    *
    * Side Effects:    Radio enter sleep mode, supply current is aprox 2uA
    *       
    ********************************************************************/
    
    void MRF24J40Sleep(void)
    {
        P2PStatus.bits.PHY_SLEEPING = 1;
        
        //;clear the WAKE pin in order to allow the device to go to sleep
        PHY_WAKE = 0;
        
        //;write the registers required to place the device in sleep
        PHYSetShortRAMAddr(WRITE_TXBCNINTL,0x80);
        PHYSetShortRAMAddr(WRITE_RXFLUSH,0x60);
        PHYSetShortRAMAddr(WRITE_SLPACK,0x80);
    }
    
    /*********************************************************************
     * void MRF24J40Wake(void)
     *
     * Overview:        This function wakes up the MRF24J40 radio   
     *
     * PreCondition:    MRF24J40 radio is in sleeping mode
     *
     * Input:           None
     *
     * Output:          None
     *
     * Side Effects:    Wake radio from sleeping mode
     *
     ********************************************************************/
    void MRF24J40Wake(void)
    {
        BYTE results;
        TICK failureTimer;
        TICK currentDifference;
        
        //wake up the device
        PHY_WAKE = 1;
    
        failureTimer = TickGet();
    
        while(1)
        {
            currentDifference = TickGet();
                
            currentDifference.Val = TickGetDiff(currentDifference,failureTimer);
            
            // if timeout, assume the device has waken up
            if(currentDifference.Val > 6250)
            {
                break;
            }
        
            results = PHYGetShortRAMAddr(READ_ISRSTS);
            if((results & 0x40) != 0x00)
            {
                break;
            }
        }

        while(1)
        {
            currentDifference = TickGet();
                
            currentDifference.Val = TickGetDiff(currentDifference,failureTimer);
            
            // if timeout, assume the device has waken up
            if(currentDifference.Val > 6250)
            {
                break;
            }
            
            results = PHYGetLongRAMAddr(RFSTATE);
            if( (results & 0xE0) == 0xA0 )
            {
                break;
            }
            
        }
                   
        P2PStatus.bits.PHY_SLEEPING = 0;
     }
     
     
     /*********************************************************************
     * BOOL CheckForData(void)
     *
     * Overview:        This function sends out a Data Request to the peer
     *                  device of the first P2P connection. 
     *
     * PreCondition:    MRF24J40 is initialized and fully waken up
     *
     * Input:           None
     *
     * Output:          None
     *
     * Side Effects:    The P2P stack is waiting for the response from
     *                  the peer device. A data request timer has been
     *                  started. In case there is no response from the
     *                  peer device, the data request will time-out itself
     *
     ********************************************************************/
     BOOL CheckForData(void)
     {
        FlushTx();
        WriteData(CMD_MAC_DATA_REQUEST);
        if( UnicastConnection(0, TRUE, FALSE) )
        {
            P2PStatus.bits.DataRequesting = 1;  
            DataRequestTimer = TickGet();  
            return TRUE;
        }
        return FALSE;
     }
 #endif
 
/*********************************************************************
 * void HighISR(void)
 *
 * Overview:        This is the interrupt handler for the MRF24J40 and
 *                  P2P stack. 
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Various flags and registers set.
 *
 ********************************************************************/
#if defined(__18CXX)
    #if defined(HITECH_C18)
        #pragma interrupt_level 0
        void interrupt HighISR(void)
    #else
        #pragma interruptlow HighISR
        void HighISR(void)
    #endif
#elif defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24H__)
    void _ISRFAST __attribute__((interrupt, auto_psv)) _INT2Interrupt(void)
#elif defined(__PIC32MX__)
    void __ISR(_EXTERNAL_2_VECTOR, ipl4) _INT2Interrupt(void)
#elif defined(__PICC__)
    #pragma interrupt_level 0
    void interrupt HighISR(void)
#else
    void _ISRFAST _INT1Interrupt(void)
#endif
{
    if(RFIE && RFIF)
    {    
        //clear the interrupt flag as soon as possible such that another interrupt can
        //occur quickly.
TryAgain:
        RFIF = 0;
        
        //create a new scope for the MRF24J40 interrupts so that we can clear the interrupt
        //flag quickly and then handle the interrupt that we have already received
        {
            MRF24J40_IFREG flags;        

            //read the interrupt status register to see what caused the interrupt        
            flags.Val=PHYGetShortRAMAddr(READ_ISRSTS);
           
            if(flags.bits.RF_TXIF)
            {
                //if the TX interrupt was triggered
                //clear the busy flag indicating the transmission was complete
                P2PStatus.bits.TX_BUSY = 0;
                
                failureCounter = 0;
                
                #ifndef TARGET_SMALL
                    //if we were waiting for an ACK
                    if(P2PStatus.bits.TX_PENDING_ACK)
                    {
                        BYTE_VAL results;
                        
                        //read out the results of the transmission
                        results.Val = PHYGetShortRAMAddr(READ_TXSR);
                        
                        if(results.bits.b0 == 1)
                        {
                            //the transmission wasn't successful and the number
                            //of retries is located in bits 7-6 of TXSR
                            //TODO: failed to Transmit
                            P2PStatus.bits.TX_FAIL = 1;
                            #ifdef ENABLE_FREQUENCY_AGILITY
                                if( results.bits.b5 )
                                {
                                    CCAFailureTimes++;
                                }
                                else
                                {
                                    AckFailureTimes++;
                                }
                            #endif
                            //ConsolePutROMString((ROM char *)"\r\nfailed to transmit");
                            //PrintChar(results.Val);
                        }
                        else
                        {
                            //transmission successful
                            //clear that I am pending an ACK, already got it
                            P2PStatus.bits.TX_PENDING_ACK = 0;
                            #ifdef ENABLE_FREQUENCY_AGILITY
                                CCAFailureTimes = 0;
                                AckFailureTimes = 0;
                            #endif
                            //TODO: need to track somewhere that the packet was successful
                        }
                    }
                #endif
                
            }
            
            else if(flags.bits.RF_RXIF)
            {      
                //if the RX interrupt was triggered
                if(P2PStatus.bits.RX_ENABLED)
                {
                    #ifdef ENABLE_SECURITY
                        if( P2PStatus.bits.RX_SECURITY )
                        {
                            BYTE DecryptionStatus = PHYGetShortRAMAddr(READ_SECISR);
                            
                            P2PStatus.bits.RX_SECURITY = 0;
                            if( (DecryptionStatus & 0x02) == 0 )
                            {
                                // there is no decryption error
                                P2PStatus.bits.RX_PENDING = 1;
                            }
                            else
                            {
                                PHYSetShortRAMAddr(WRITE_RXFLUSH,0x01);
                            }
                        }
                        else
                    #endif
                    #if defined(ENABLE_SECURITY) && !defined(TARGET_SMALL)
                        if( P2PStatus.bits.RX_IGNORE_SECURITY )
                        {
                            P2PStatus.bits.RX_IGNORE_SECURITY = 0;
                            PHYSetShortRAMAddr(WRITE_RXFLUSH,0x01); 
                        }
                        else
                    #endif
                    //If the part is enabled for receiving packets right now
                    //(not pending an ACK)
                    //indicate that we have a packet in the buffer pending to 
                    //be read into the buffer from the FIFO
                    P2PStatus.bits.RX_PENDING = 1;
                }
                else
                {
                    //else if the RX is not enabled then we need to flush this packet
                    //flush the buffer
                    PHYSetShortRAMAddr(WRITE_RXFLUSH,0x01);
                }//end of RX_ENABLED check
                    
            } //end of RXIF check
            #ifdef ENABLE_SECURITY
                else if(flags.bits.SECIF)
                {
                    BYTE i;
                    
                    #ifndef TARGET_SMALL
                        BYTE SourceLongAddress[8];
                        DWORD_VAL FrameCounter;
                        BYTE FrameControl;
                        BYTE j;
                        
                        // all the code below is to check the frame counter
                        // and compare it with the frame counter for the 
                        // source device in the memory. We do this to avoid
                        // repeat attack.
                         
                        // bypass MRF24J40 errata 5
                        //PHYSetShortRAMAddr(WRITE_FFOEN, 0x08);
                        
                        i = 6;
                        FrameControl = PHYGetLongRAMAddr(0x301);
                        if( (FrameControl & 0x40) == 0 ) // intra PAN?
                        {
                            i += 2;
                        }
                        
                        FrameControl = PHYGetLongRAMAddr(0x302);
                        if( (FrameControl & 0x0C) == 0x0C )
                        {
                            i += 8;
                        }
                        else
                        {
                            i += 2;
                        }

                        // get the source address
                        for(j = 0; j < 8; j++)
                        {
                            SourceLongAddress[j] = PHYGetLongRAMAddr(0x300 + i + j);
                        }
                      
                        for(j = 0; j < 4; j++)
                        {
                            FrameCounter.v[j] = PHYGetLongRAMAddr(0x308 + i + j);
                        }
                        
                        // get key sequence number
                        j = PHYGetLongRAMAddr(0x30C + i);

                        if( j != myKeySequenceNumber )
                        {
                            PHYSetShortRAMAddr(WRITE_SECCR0, 0x80); // ignore the packet
                            P2PStatus.bits.RX_IGNORE_SECURITY = 1;
                        }
                        else
                        {
                            for(i = 0; i < P2P_CONNECTION_SIZE; i++)
                            {
                                if( (P2PConnections[i].PeerInfo[0] & 0x80) && 
                                    isSameAddress(P2PConnections[i].PeerLongAddress, SourceLongAddress) )
                                {
                                    break;
                                }
                            }
                            if( i < P2P_CONNECTION_SIZE )
                            {
                                if( P2PConnections[i].IncomingFrameCounter.Val > FrameCounter.Val )
                                {
                                    PHYSetShortRAMAddr(WRITE_SECCR0, 0x80); // ignore the packet
                                    P2PStatus.bits.RX_IGNORE_SECURITY = 1;  
                                }
                                else
                                {
                                    P2PConnections[i].IncomingFrameCounter.Val = FrameCounter.Val;
                                }
                            }
                        }
                        
                        // bypass MRF24J40 errata 5
                        //PHYSetShortRAMAddr(WRITE_FFOEN, 0x88);
                        
                        if( P2PStatus.bits.RX_IGNORE_SECURITY == 0 )
                    #endif
                    {
                        // supply the key
                        for(i = 0; i < 16; i++)
                        {
                            PHYSetLongRAMAddr(0x2B0 + i, mySecurityKey[i]);
                        }
                        P2PStatus.bits.RX_SECURITY = 1;

                        // set security level and trigger the decryption
                        PHYSetShortRAMAddr(WRITE_SECCR0, mySecurityLevel << 3 | 0x40);
                    }
                }
            #endif  
            else if( flags.Val )
            {
                PHYSetShortRAMAddr(WRITE_RXFLUSH,0x01);   
            }
            
            if( flags.Val )
            {
                goto TryAgain;
            }
            
        } //end of scope of RF interrupt handler
    } //end of if(RFIE && RFIF)
    
    #if defined(__18CXX)
        //check to see if the symbol timer overflowed
        if(INTCONbits.TMR0IF)
        {
            if(INTCONbits.TMR0IE)
            {
                /* there was a timer overflow */
                INTCONbits.TMR0IF = 0;
                timerExtension1++;
                if(timerExtension1 == 0)
                {
                    timerExtension2++;
                }
            }
        }
        
        if(INTCONbits.RBIE && INTCONbits.RBIF)
        {
            INTCONbits.RBIF = 0;
        }
    #endif
} //end of interrupt handler


#ifdef ENABLE_INDIRECT_MESSAGE
    
    /*********************************************************************
     * BOOL IndirectPacket(INPUT BOOL Broadcast, 
     *                     INPUT WORD_VAL DestinationPANID, 
     *                     INPUT BYTE *DestinationAddress, 
     *                     INPUT BOOL isCommand, 
     *                     INPUT BOOL SecurityEnabled)
     *
     * Overview:        This function store the indirect message for node
     *                  that turns off radio when idle     
     *
     * PreCondition:    None
     *
     * Input:           Broadcast           - Boolean to indicate if the indirect
     *                                        message a broadcast message
     *                  DestinationPANID    - The PAN Identifier of the 
     *                                        destination node
     *                  DestinationAddress  - The pointer to the destination
     *                                        long address
     *                  isCommand           - The boolean to indicate if the packet
     *                                        is command
     *                  SecurityEnabled     - The boolean to indicate if the 
     *                                        packet needs encryption
     *
     * Output:          boolean to indicate if operation successful
     *
     * Side Effects:    An indirect message stored and waiting to deliever
     *                  to sleeping device. An indirect message timer
     *                  has started to expire the indirect message in case
     *                  RFD does not acquire data in predefined interval
     *
     ********************************************************************/
    BOOL IndirectPacket(INPUT BOOL Broadcast, 
                        INPUT WORD_VAL DestinationPANID, 
                        INPUT BYTE *DestinationAddress, 
                        INPUT BOOL isCommand, 
                        INPUT BOOL SecurityEnabled)
    { 
        BYTE i;
        
        #ifndef ENABLE_BROADCAST
            if( Broadcast )
            {
                return FALSE;
            }
        #endif

        // loop through the available indirect message buffer and locate
        // the empty message slot
        for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
        {
            if( indirectMessages[i].flags.bits.isValid == 0 )
            {
                BYTE j;
                
                // store the message
                indirectMessages[i].flags.bits.isValid          = TRUE;
                indirectMessages[i].flags.bits.isBroadcast      = Broadcast;
                indirectMessages[i].flags.bits.isCommand        = isCommand;
                indirectMessages[i].flags.bits.isSecured        = SecurityEnabled;
                indirectMessages[i].DestPANID.Val               = DestinationPANID.Val;
                if( DestinationAddress != NULL )
                {
                    for(j = 0; j < 8; j++)
                    {
                        indirectMessages[i].DestAddress.DestLongAddress[j] = DestinationAddress[j];
                    }
                }
                #ifdef ENABLE_BROADCAST
                    else
                    {
                        BYTE k = 0;
    
                        for(j = 0; j < P2P_CONNECTION_SIZE; j++)
                        {
                            if( (P2PConnections[j].PeerInfo[0] & 0x83) == 0x82 )
                            {
                                indirectMessages[i].DestAddress.DestIndex[k++] = j;
                            }
                        }
                        for(; k < P2P_CONNECTION_SIZE; k++)
                        {
                            indirectMessages[i].DestAddress.DestIndex[k] = 0xFF;
                        }
                    }
                #endif
                indirectMessages[i].PayLoadSize = TxData;
                for(j = 0; j < TxData; j++)
                {
                    indirectMessages[i].PayLoad[j] = TxBuffer[j];
                }
                indirectMessages[i].TickStart = TickGet();
                return TRUE;
            }
        }
        return FALSE;
    }
#endif


/*********************************************************************
 * BOOL SendPacket(INPUT BOOL Broadcast, 
 *                 INPUT WORD_VAL DestinationPANID, 
 *                 INPUT BYTE *DestinationAddress, 
 *                 INPUT BOOL isCommand, 
 *                 INPUT BOOL SecurityEnabled)
 *
 * Overview:        This function sends the packet  
 *
 * PreCondition:    MRF24J40 is initialized
 *
 * Input:     
 *          BOOL        Broadcast           If packet to send needs to be broadcast
 *          WORD_VAL    DestinationPANID    Destination PAN Identifier
 *          BYTE *      DestinationAddress  Pointer to destination long address 
 *          BOOL        isCommand           If packet to send is a command packet
 *          BOOL        SecurityEnabled     If packet to send needs encryption
 *                  
 * Output: 
 *          BOOL                            If operation successful
 *
 * Side Effects:    MRF24J40 is triggered to transmit a packet
 *
 ********************************************************************/
BOOL SendPacket(INPUT BOOL Broadcast, 
                INPUT WORD_VAL DestinationPANID, 
                INPUT BYTE *DestinationAddress, 
                INPUT BOOL isCommand, 
                INPUT BOOL SecurityEnabled)
{ 
    BYTE headerLength;
    BYTE loc = 0;
    BYTE i;
    #ifndef TARGET_SMALL
        BOOL IntraPAN;
    #endif
    #ifdef VERIFY_TRANSMIT
        WORD w;
    #endif

    // wait for the previous transmission finish
    while( P2PStatus.bits.TX_BUSY ) 
    {
        if(RF_INT_PIN == 0)
        {
            RFIF = 1;
        }
    }
    
    // set the frame control in variable i    
    if( isCommand )
    {
        i = 0x03;
    }
    else
    {
        i = 0x01;
    }
    
    // decide the header length for different addressing mode
    #ifndef TARGET_SMALL
        if( DestinationPANID.Val == myPANID.Val ) // this is intraPAN
    #endif
    {
        if( Broadcast )
        {
            headerLength = 15;
        }
        else
        {
            headerLength = 21;
        }
        i |= 0x40;
        #ifndef TARGET_SMALL
            IntraPAN = TRUE;
        #endif
    }
    
    #ifndef TARGET_SMALL
    else
    {
        if( Broadcast )
        {
            headerLength = 17;
        }
        else
        {
            headerLength = 23;
        }
        IntraPAN = FALSE;
    }
    #endif
    
    #ifdef ENABLE_SECURITY
        if( SecurityEnabled )
        {
            i |= 0x08;   
        }
    #endif
    
    if( P2PStatus.bits.AckRequired && Broadcast == FALSE ) 
    {
        i |= 0x20;   
    }

    // set header length
    PHYSetLongRAMAddr(loc++, headerLength);
    // set packet length
    #ifdef ENABLE_SECURITY
        if( SecurityEnabled )
        {
            PHYSetLongRAMAddr(loc++, headerLength + TxData + 5);
        }
        else
    #endif
    {
        PHYSetLongRAMAddr(loc++, headerLength + TxData);
    }
    
    // set frame control LSB
    PHYSetLongRAMAddr(loc++, i);
    
    // set frame control MSB
    if( Broadcast )
    {
        PHYSetLongRAMAddr(loc++, 0xC8);       // long source, short destination
    }
    else
    {
        PHYSetLongRAMAddr(loc++, 0xCC);       // long source, long destination
    } 
    
    // sequence number
    PHYSetLongRAMAddr(loc++, IEEESeqNum++);
    
    // destination PANID     
    PHYSetLongRAMAddr(loc++, DestinationPANID.v[0]);
    PHYSetLongRAMAddr(loc++, DestinationPANID.v[1]);
    
    // destination address
    if( Broadcast )
    {
        PHYSetLongRAMAddr(loc++, 0xFF);
        PHYSetLongRAMAddr(loc++, 0xFF);
    }
    else
    {
        for(i = 0; i < 8; i++)
        {
            PHYSetLongRAMAddr(loc++, DestinationAddress[i]);
        }
    }
    
    #ifndef TARGET_SMALL
        // source PANID if necessary
        if( IntraPAN == FALSE )
        {
            PHYSetLongRAMAddr(loc++, myPANID.v[0]);
            PHYSetLongRAMAddr(loc++, myPANID.v[1]);
        }
    #endif
    
    // source address
    for(i = 0; i < 8; i++)
    {
        PHYSetLongRAMAddr(loc++, myLongAddress[i]);
    }
    
    #ifdef ENABLE_SECURITY
        if( SecurityEnabled )
        {
            WORD keyloc = 0x280;
            // fill the additional security aux header
            for(i = 0; i < 4; i++)
            {
                PHYSetLongRAMAddr(loc++, OutgoingFrameCounter.v[i]);
            }
            OutgoingFrameCounter.Val++;
            PHYSetLongRAMAddr(loc++, myKeySequenceNumber);
            
            // fill the security key
            for(i = 0; i < 16; i++)
            {
                PHYSetLongRAMAddr(0x280 + i, mySecurityKey[i]);
            }
            
            // set the cipher mode
            PHYSetShortRAMAddr(WRITE_SECCR0, mySecurityLevel);
        }
    #endif
    
    
    // write the payload
    for(i = 0; i < TxData; i++)
    {
        PHYSetLongRAMAddr(loc++, TxBuffer[i]);
    }
    TxData = 0;
    
    P2PStatus.bits.TX_BUSY = 1;

    // set the trigger value
    if( P2PStatus.bits.AckRequired && Broadcast == FALSE )
    {
        i = 0x05;
        #ifndef TARGET_SMALL
            P2PStatus.bits.TX_PENDING_ACK = 1;
        #endif
    }
    else
    {
        i = 0x01;
        #ifndef TARGET_SMALL
            P2PStatus.bits.TX_PENDING_ACK = 0;
        #endif
    }
    #ifdef ENABLE_SECURITY
        if( SecurityEnabled )
        {
            i |= 0x02;
        }
    #endif
    
    // now trigger the transmission
    PHYSetShortRAMAddr(WRITE_TXNMTRIG, i); 
    
    #ifdef VERIFY_TRANSMIT
        w = 0;
        while(1)
        {   
            if( RF_INT_PIN == 0 )
            {
                RFIF = 1;
            }
            if( P2PStatus.bits.TX_BUSY == 0 )
            {
                if( P2PStatus.bits.TX_FAIL )
                {
                    P2PStatus.bits.TX_FAIL = 0;
                    return FALSE;
                }
                break;   
            }
            if( ++w > 0xFFFE )
            {
                return FALSE;
            }
        }
        CCAFailureTimes = 0;
        AckFailureTimes = 0;
    #endif
   
    return TRUE;
}

/*********************************************************************
 * BOOL BroadcastPacket(    INPUT WORD_VAL DestinationPANID, 
 *                          INPUT BOOL isCommand, 
 *                          INPUT BOOL SecurityEnabled )
 *
 * Overview:        This function broadcast a packet
 *
 * PreCondition:    MRF24J40 is initialized
 *
 * Input:  
 *          DestinationPANID    - Destination PAN Identifier
 *          isCommand           - If packet to send is a command packet
 *          SecurityEnabled     - If packet to send needs encryption
 *                  
 * Output: 
 *          If operation successful
 *
 * Side Effects:    MRF24J40 is triggered to broadcast a packet
 *  
 ********************************************************************/
BOOL BroadcastPacket(   INPUT WORD_VAL DestinationPANID, 
                        INPUT BOOL isCommand, 
                        INPUT BOOL SecurityEnabled )
{   
    #ifdef ENABLE_INDIRECT_MESSAGE
        BYTE i;
        for(i = 0; i < P2P_CONNECTION_SIZE; i++)
        {
            if( P2PConnections[i].PeerInfo[0]>= 0x80 ) PrintChar(P2PConnections[i].PeerInfo[0]);
            
            if( (P2PConnections[i].PeerInfo[0] & 0x83) == 0x82 )
            {
                IndirectPacket(TRUE, DestinationPANID, NULL, isCommand, SecurityEnabled);
            }     
        }
    #endif
    return SendPacket(TRUE, DestinationPANID, NULL, isCommand, SecurityEnabled);
}

/*********************************************************************
 * BOOL UnicastConnection( INPUT BYTE ConnectionIndex, 
 *                         INPUT BOOL isCommand, 
 *                         INPUT BOOL SecurityEnabled)
 *
 * Overview:        This is one of ways to unicast a packet to the peer device
 *
 * PreCondition:    MRF24J40 is initialized
 *
 * Input:  
 *          ConnectionIndex - The index of P2P Connection Entry for the destination device
 *          isCommand       - If packet to send is a command packet
 *          SecurityEnabled - If packet to send needs encryption
 *                  
 * Output: 
 *          If operation successful
 *
 * Side Effects:    MRF24J40 is triggered to unicast a packet
 ********************************************************************/
BOOL UnicastConnection( INPUT BYTE ConnectionIndex, 
                        INPUT BOOL isCommand, 
                        INPUT BOOL SecurityEnabled)
{
    if( P2PConnections[ConnectionIndex].PeerInfo[0] & 0x80 )
    {
        #ifdef ENABLE_INDIRECT_MESSAGE
            // check if RX on when idle
            if( (P2PConnections[ConnectionIndex].PeerInfo[0] & 0x03) == 0x02 )
            {
                return IndirectPacket(FALSE, myPANID, P2PConnections[ConnectionIndex].PeerLongAddress, isCommand, SecurityEnabled);
            }
        #endif
        return UnicastLongAddress(myPANID, P2PConnections[ConnectionIndex].PeerLongAddress, isCommand, SecurityEnabled);
    }
    return FALSE;
}

/*********************************************************************
 * BOOL UnicastLongAddress(INPUT WORD_VAL DestinationPANID, 
 *                         INPUT BYTE *DestinationAddress, 
 *                         INPUT BOOL isCommand, 
 *                         INPUT BOOL SecurityEnabled)
 *
 * Overview:        Unicast a packet to the peer device represented by 
 *                  the destination long address
 *
 * PreCondition:    MRF24J40 is initialized
 *
 * Input:  
 *          DestinationPANID    - Destination PAN Identifier
 *          DestinationAddress  - Pointer to the destination long address
 *          isCommand           - If packet to send is a command packet
 *          SecurityEnabled     - If packet to send needs encryption
 *                  
 * Output: 
 *          If operation successful
 *
 * Side Effects:    MRF24J40 is triggered to unicast a packet
 *
 ********************************************************************/
BOOL UnicastLongAddress(INPUT WORD_VAL DestinationPANID, 
                        INPUT BYTE *DestinationAddress, 
                        INPUT BOOL isCommand, 
                        INPUT BOOL SecurityEnabled)
{
    #ifdef ENABLE_INDIRECT_MESSAGE
        BYTE i;
        
        for(i = 0; i < P2P_CONNECTION_SIZE; i++)
        {
            // check if RX on when idle
            if( (P2PConnections[i].PeerInfo[0] & 0x83) == 0x82 &&
                isSameAddress(DestinationAddress, P2PConnections[i].PeerLongAddress) )
            {
                return IndirectPacket(FALSE, DestinationPANID, DestinationAddress, isCommand, SecurityEnabled);
            }
        }
    #endif
    return SendPacket(FALSE, DestinationPANID, DestinationAddress, isCommand, SecurityEnabled);    
}


/*********************************************************************
 * BOOL    isSameAddress(INPUT BYTE *Address1, INPUT BYTE *Address2)
 *
 * Overview:        This function compares two long addresses and returns
 *                  the boolean to indicate if they are the same
 *
 * PreCondition:    
 *
 * Input:  
 *          Address1    - Pointer to the first long address to be compared
 *          Address2    - Pointer to the second long address to be compared
 *                  
 * Output: 
 *          If the two address are the same
 *
 * Side Effects:    
 *
 ********************************************************************/
BOOL    isSameAddress(INPUT BYTE *Address1, INPUT BYTE *Address2)
{
    BYTE i;
    
    for(i = 0; i < 8; i++)
    {
        if( Address1[i] != Address2[i] )
        {
            return FALSE;
        }
    }
    return TRUE;
}

#ifdef ENABLE_ED_SCAN
    /*********************************************************************
     * BYTE CreateNewConnection(INPUT BYTE RetryInterval, INPUT DWORD ChannelMap)
     *
     * Overview:        This function create a new P2P connection between two devices
     *
     * Side Effects:    A new P2P Connection Entry 
     *
     * PreCondition:    
     *
     * Input:  
     *          RetryInterval   - The interval between two tries of sending P2P Connection 
     *                            request in seconds
     *          ChannelMap      - Bitmap of available channels. Bit 11 means channel 11 and 
     *                            bit 26 means channel 26
     *                  
     * Output: 
     *          The index of the P2P Connection Entry of the peer device
     *          that established the new connection
     *
     ********************************************************************/
    BYTE CreateNewConnection(INPUT BYTE RetryInterval, INPUT DWORD ChannelMap)
#else
    BYTE CreateNewConnection(INPUT BYTE RetryInterval)
#endif
{
    TICK    startTick = TickGet();
    BYTE    EnableConnection = P2PStatus.bits.EnableNewConnection;
    BYTE    Interval = 0;
    #ifdef ENABLE_ED_SCAN
        DWORD ChannelMask = 0x00000800;
        BYTE i = 0;
    #endif
    
    EnableNewConnection();
    P2PStatus.bits.SearchConnection = 1;
    while( P2PStatus.bits.SearchConnection )
    {
        
        TICK currentTick = TickGet();
        
        if( TickGetDiff(currentTick, startTick) > (ONE_SECOND) )
        {
            startTick = currentTick;
            
            // avoid multiply of the DWORD, save about 150 byes 
            // of programming space for PIC18
            if( ++Interval < RetryInterval )
            {
                continue;
            }
            Interval = 0;
            
            #ifdef ENABLE_ED_SCAN
                // if enable energy scan, we need to scan all channels
                // here loop through all available channels
                while( (ChannelMap & (ChannelMask << i)) == 0 )
                {
                    i++;
                    if( i > 15 )
                    {
                        i = 0;
                    }
                }
                SetChannel(i << 4);
                printf("\r\nScan channel ");
                PrintDec(i+11);
                i++;
            #endif
            
            FlushTx();
            WriteData(CMD_P2P_CONNECTION_REQUEST);
            WriteData(currentChannel);
            WriteData(P2P_CAPACITY_INFO);
            #if ADDITIONAL_CONNECTION_PAYLOAD > 0
                for(i = 0; i < ADDITIONAL_CONNECTION_PAYLOAD; i++)
                {
                    WriteData(AdditionalConnectionPayload[i]);
                }
            #endif

            SendPacket(TRUE, myPANID, NULL, TRUE, FALSE);
        }

        P2PTasks();
    }
  
    P2PStatus.bits.EnableNewConnection = EnableConnection;
    
    return LatestConnection;
}


/*********************************************************************
 * BOOL ReceivedPacket(void)
 *
 * Overview:        This function returns the boolean to indicate if a new packet
 *                  has been received by the stack
 *
 * PreCondition:    None
 *
 * Input:           None
 *          
 * Output: 
 *          Boolean to indicate if a new packet has been received by the stack
 *
 * Side Effects:    Stack state machine has been called 
 *
 ********************************************************************/
BOOL ReceivedPacket(void)
{ 
    P2PTasks(); 
    
    return P2PStatus.bits.RxHasUserData;
}

#ifdef ENABLE_DUMP
    /*********************************************************************
     * void DumpConnection(INPUT BYTE index)
     *
     * Overview:        This function prints out the content of the connection 
     *                  with the input index of the P2P Connection Entry
     *
     * PreCondition:    
     *
     * Input:  
     *          index   - The index of the P2P Connection Entry to be printed out
     *                  
     * Output:  None
     *
     * Side Effects:    The content of the connection pointed by the index 
     *                  of the P2P Connection Entry will be printed out
     *
     ********************************************************************/
    void DumpConnection(INPUT BYTE index)
    {
        BYTE i, j;
        
        if( index < P2P_CONNECTION_SIZE )
        {
            printf("\r\n\r\nConnection     PeerLongAddress     PeerInfo\r\n");  
            if( P2PConnections[index].PeerInfo[0] & 0x80 )
            {
                PrintChar(index);
                printf("             ");
                for(i = 0; i < 8; i++)
                {
                    PrintChar( P2PConnections[index].PeerLongAddress[7-i] );
                }
                printf("    ");
                #if ADDITIONAL_CONNECTION_PAYLOAD > 0
                    for(i = 0; i < ADDITIONAL_CONNECTION_PAYLOAD + 1; i++)
                    {
                        PrintChar( P2PConnections[index].PeerInfo[i] );
                    }
                #endif
                printf("\r\n");
            }
        }
        else
        {
            printf("\r\n\r\nConnection     PeerLongAddress     PeerInfo\r\n");  
            for(i = 0; i < P2P_CONNECTION_SIZE; i++)
            {
                
                if( P2PConnections[i].PeerInfo[0] & 0x80 )
                {
                    PrintChar(i);
                    printf("             ");
                    for(j = 0; j < 8; j++)
                    {
                        PrintChar( P2PConnections[i].PeerLongAddress[7-j] );
                    }
                    printf("    ");
                    #if ADDITIONAL_CONNECTION_PAYLOAD > 0
                        for(j = 0; j < ADDITIONAL_CONNECTION_PAYLOAD + 1; j++)
                        {
                            PrintChar( P2PConnections[i].PeerInfo[j] );
                        }
                    #endif
                    printf("\r\n");
                }  
            }
        }
    }
#endif


/*********************************************************************
 * BYTE AddConnection(void)
 *
 * Overview:        This function create a new P2P connection entry
 *
 * PreCondition:    A P2P Connection Request or Response has been 
 *                  received and stored in rxFrame structure
 *
 * Input:  None
 *                  
 * Output: 
 *          The index of the P2P Connection Entry for the newly added
 *          connection
 *
 * Side Effects:    A new P2P Connection Entry is created. The search 
 *                  connection operation ends if an entry is added 
 *                  successfully
 *
 ********************************************************************/
BYTE AddConnection(void)
{
    BYTE i;
    BYTE status = STATUS_SUCCESS;
    BYTE connectionSlot = 0xFF;

    // if no peerinfo attached, this is only an active scan request,
    // so do not save the source device's info
    #ifdef ENABLE_ACTIVE_SCAN
        if( rxFrame.PayLoadSize < 3 )
        {
            return STATUS_ACTIVE_SCAN;
        }
    #endif
    
    // loop through all entry and locate an proper slot
    for(i = 0; i < P2P_CONNECTION_SIZE; i++)
    {
        // check if the entry is valid
        if( P2PConnections[i].PeerInfo[0] & 0x80 )
        {
            // check if the entry address matches source address of current received packet
            if( isSameAddress(rxFrame.SourceLongAddress, P2PConnections[i].PeerLongAddress) )
            {
                connectionSlot = i;
                status = STATUS_EXISTS;
                break;
            }
        }
        else if( connectionSlot == 0xFF )
        {
            // store the first empty slot
            connectionSlot = i;
        }  
    }
        
    if( connectionSlot == 0xFF )
    {
        return STATUS_NOT_ENOUGH_SPACE;
    }
    else 
    {
        if( P2PStatus.bits.EnableNewConnection == 0 )
        {
            return status;
        }

        // store the source address
        for(i = 0; i < 8; i++)
        {
            P2PConnections[connectionSlot].PeerLongAddress[i] = rxFrame.SourceLongAddress[i];
        }
        
        // store the capacity info and validate the entry
        P2PConnections[connectionSlot].PeerInfo[0] = rxFrame.PayLoad[2] | 0x80;
        // store possible additional connection payload
        #if ADDITIONAL_CONNECTION_PAYLOAD > 0
            for(i = 0; i < ADDITIONAL_CONNECTION_PAYLOAD; i++)
            {
                P2PConnections[connectionSlot].PeerInfo[1+i] = rxFrame.PayLoad[3+i];
            }
        #endif

        #ifdef ENABLE_SECURITY
            // if security is enabled, clear the incoming frame control
            P2PConnections[connectionSlot].IncomingFrameCounter.Val = 0;
        #endif
        LatestConnection = connectionSlot;
        P2PStatus.bits.SearchConnection = 0;   
    } 
    return status;
}


#ifdef ENABLE_ACTIVE_SCAN
     /*********************************************************************
     * BYTE ActiveScan(INPUT BYTE ScanDuration, INPUT DWORD ChannelMap)
     *
     * Overview:        This function do an active scan and returns the 
     *                  number of PANs catched during the scan. The scan 
     *                  results are stored in global variable 
     *                  ActiveScanResults     
     *
     * PreCondition:    P2P stack has been initialized
     *
     * Input:   ScanDuration    - The time period to do the active scan 
     *                            on each channel
     *          ChannelMap      - The bit map of the channels to do active 
     *                            scan
     *                  
     * Output:  The totoal number of PANs that have been acquired during 
     *          active scan
     *
     * Side Effects:    None
     *
     ********************************************************************/
    BYTE ActiveScan(INPUT BYTE ScanDuration, INPUT DWORD ChannelMap)
    {
        BYTE i;
        DWORD channelMask = 0x00000800;    
        BYTE backupChannel = currentChannel;
        
        for(i = 0; i < ACTIVE_SCAN_RESULT_SIZE; i++)
        {
            ActiveScanResults[i].Channel = 0xFF;
        }
        
        ActiveScanResultIndex = 0;
        i = 0;
        while( i < 16 )
        {
            if( ChannelMap & (channelMask << i) )
            {
                TICK ScanStart;
                TICK tempTick;
                WORD_VAL tmpPANID;

                /* choose appropriate channel */
                SetChannel( (i << 4) );
     
                FlushTx();
                WriteData(CMD_P2P_CONNECTION_REQUEST);
                WriteData(currentChannel);
                tmpPANID.Val = 0xFFFF;
                SendPacket(TRUE, tmpPANID, NULL, TRUE, FALSE);
                
                ScanStart = TickGet();                
                while(1)
                {
                    P2PTasks();
                    
                    tempTick = TickGet();
                    if( TickGetDiff(tempTick, ScanStart) > ((DWORD)(ScanTime[ScanDuration])) )
                    {
                        // if scan time exceed scan duration, prepare to scan the next channel
                        break;
                    }
                }          
            }  
            i++;
        }
        
        SetChannel(backupChannel);
        
        return ActiveScanResultIndex;
    }   

#endif

#ifdef ENABLE_ED_SCAN

    /*********************************************************************
     * BYTE OptimalChannel(INPUT BYTE ScanDuration, 
     *                     INPUT DWORD ChannelMap, 
     *                     OUTPUT BYTE *RSSIValue)
     *
     * Overview:        This function finds out the optimal channel with 
     *                  least noise after doing energy scan on all available
     *                  channels supplied.
     *     
     * PreCondition:    MRF24J40 has been initialized
     *
     * Input:   ScanDuration    - The scan duration of the energy scan. 
     *                            The scan duration is defined in IEEE902.15.4. 
     *                            The actual time is 60 * (2^(ScanDuration) + 1) 
     *                            symbols
     *          ChannelMap      - The bitmap of the available channels. Bit 11 
     *                            represents channel 11 and Bit 26 represents 
     *                            channel 26 etc.
     *          RSSIValue       - (OUTPUT) The maximum RSSI value for the optimal 
     *                            channel.
     *                  
     * Output: 
     *          The optimal channel with least noise
     *      
     * Side Effects:    Current operating channel has been changed
     *
     ********************************************************************/
    BYTE OptimalChannel(INPUT BYTE ScanDuration, INPUT DWORD ChannelMap, OUTPUT BYTE *RSSIValue)
    {
        BYTE i;
        BYTE OptimalChannel;
        BYTE minRSSI = 0xFF;
        DWORD channelMask = 0x00000800;
        
        ConsolePutROMString((ROM char*)"\r\nEnergy Scan Results:");
        i = 0;
        while( i < 16 )
        {
            if( ChannelMap & (channelMask << i) )
            {
                BYTE RSSIcheck;
                BYTE maxRSSI = 0;
                TICK ScanStart;
                TICK tempTick;

                /* choose appropriate channel */
                SetChannel( (i << 4) );
                
                ScanStart = TickGet();
                
                while(1)
                {
                    // calculate RSSI for firmware request
                    PHYSetShortRAMAddr(WRITE_BBREG6, 0x80);                                                 
                    
                    // Firmware Request the RSSI
                    RSSIcheck = PHYGetShortRAMAddr (READ_BBREG6);
                    while ((RSSIcheck & 0x01) != 0x01)
                    {
                        RSSIcheck = PHYGetShortRAMAddr (READ_BBREG6);
                    }
                    
                    // read the RSSI
                    RSSIcheck = PHYGetLongRAMAddr(0x210);
                    if( RSSIcheck > maxRSSI )
                    {
                        maxRSSI = RSSIcheck;
                    }
                    
                    tempTick = TickGet();
                    if( TickGetDiff(tempTick, ScanStart) > ((DWORD)(ScanTime[ScanDuration])) )
                    {
                        // if scan time exceed scan duration, prepare to scan the next channel
                        break;
                    }
                } 
                
                printf("\r\nChannel ");
                PrintDec(i+11);
                printf(": ");
                PrintChar(maxRSSI);
                
                if( maxRSSI < minRSSI )
                {
                    minRSSI = maxRSSI;
                    OptimalChannel = (i << 4);
                    if( RSSIValue )
                    {
                        *RSSIValue = minRSSI;
                    }   
                }              
            }  
            i++;
        }        
       
        // enable RSSI attached to received packet again after
        // the energy scan is finished
        PHYSetShortRAMAddr(WRITE_BBREG6, 0x40);

        return OptimalChannel;
    }
   
#endif


#ifdef ENABLE_FREQUENCY_AGILITY

    /*********************************************************************
     * void StartChannelHopping(INPUT BYTE OptimalChannel)
     *
     * Overview:        This function broadcast the channel hopping command
     *                  and after that, change operating channel to the 
     *                  input optimal channel     
     *
     * PreCondition:    MRF24J40 has been initialized
     *
     * Input:           OptimalChannel  - The channel to hop to
     *                  
     * Output: 
     *          None
     *
     * Side Effects:    The operating channel for current device will change
     *                  to the specified channel
     *
     ********************************************************************/
    void StartChannelHopping(INPUT BYTE OptimalChannel)
    {
        BYTE i;

        for( i = 0; i < FA_BROADCAST_TIME; i++)
        {
            TICK startTick = TickGet();
            while(1)
            {
                TICK currentTick = TickGet();
                if( TickGetDiff(currentTick, startTick) > SCAN_DURATION_8 )
                {
                    FlushTx();
                    WriteData(CMD_CHANNEL_HOPPING);
                    WriteData(currentChannel);
                    WriteData(OptimalChannel);
                    SendPacket(TRUE, myPANID, NULL, TRUE, FALSE);
                    break;
                }
            }
        }
        SetChannel(OptimalChannel);  
    }
    
    
    /*********************************************************************
     * BOOL ResyncConnection(  INPUT BYTE *DestinationAddress, 
     *                         INPUT DWORD ChannelMap)
     *
     * Overview:        This function try to resynchronize the connection 
     *                  with its peer with the input destination address
     *     
     * PreCondition:    MRF24J40 has been initialized
     *
     * Input:           DestinationAddress  - The pointer to the long 
     *                                        address of the destination 
     *                                        address
     *                  ChannelMap:         - The bit map of the candicate 
     *                                        channel to resynchronize 
     *                                        connection
     *                  
     * Output: 
     *                  boolean to indicate of connection has been 
     *                  resynchronized
     *
     * Side Effects:    The operating channel will change to the 
     *                  operating channel of the destination node, 
     *                  if successful
     *
     ********************************************************************/
    BOOL ResyncConnection(INPUT BYTE *DestinationAddress, INPUT DWORD ChannelMap)
    {
        BYTE i;
        BYTE j;
        BYTE backupChannel = currentChannel;
        TICK startTick = TickGet();
        
        P2PStatus.bits.Resync = 1;
        for(i = 0; i < RESYNC_TIMES; i++)
        {
            DWORD ChannelMask = 0x00000800;
            
            j = 0;
            while(P2PStatus.bits.Resync)
            {
                TICK currentTick = TickGet();
                
                if( TickGetDiff(currentTick, startTick) > SCAN_DURATION_8 )
                {
                    startTick = currentTick;
                
                    while( (ChannelMap & (ChannelMask << j)) == 0 )
                    {
                        if( ++j > 15 )
                        {
                            goto GetOutOfLoop;
                        }
                    }
                    SetChannel(j << 4);
                    j++;

                
                    FlushTx();
                    WriteData(CMD_P2P_CONNECTION_REQUEST);
                    WriteData(currentChannel);
        
                    SendPacket(FALSE, myPANID, DestinationAddress, TRUE, FALSE);
                }
                P2PTasks();
            }
            printf("\r\nResynchronized Connection to Channel ");
            PrintDec((currentChannel>>4)+11);
            return TRUE;
GetOutOfLoop:
            Nop();         
        }
        SetChannel(backupChannel);
        P2PStatus.bits.Resync = 0;
        return FALSE;
    }        

    #ifdef FREQUENCY_AGILITY_STARTER
        /*********************************************************************
         * BOOL InitChannelHopping( INPUT DWORD ChannelMap)
         *
         * Overview:        This function try to do energy scan to locate the
         *                  channel with least noise. If the channel is not
         *                  current operating channel, process of channel 
         *                  hopping will be started.
         *
         * PreCondition:    MRF24J40 has been initialized
         *
         * Input:           ChannelMap  - The bit map of the candicate channels
         *                                to hop to
         *                  
         * Output: 
         *                  boolean to indicate if channel hopping is initiated
         *
         * Side Effects:    The operating channel will change to the optimal 
         *                  channel with least noise
         *
         ********************************************************************/
        BOOL InitChannelHopping( INPUT DWORD ChannelMap)
        {
            BYTE RSSIValue;
            BYTE backupChannel = currentChannel;
            BYTE optimalChannel = OptimalChannel(10, ChannelMap, &RSSIValue);
            
            SetChannel(backupChannel);
            if( optimalChannel == backupChannel )
            {
                return FALSE;
            }
            
            printf("\r\nHopping to Channel ");
            PrintDec((optimalChannel>>4)+11);
            StartChannelHopping(optimalChannel);
            return TRUE;
        }
    #endif

#endif



#if defined(__18CXX) & !defined(HI_TECH_C)
#pragma code highVector=0x08
void HighVector (void)
{
    _asm goto HighISR _endasm
}
#pragma code /* return to default code section */
#endif

#if defined(__18CXX) & !defined(HI_TECH_C)
#pragma code lowhVector=0x18
void LowVector (void)
{
    _asm goto HighISR _endasm
}
#pragma code /* return to default code section */
#endif
