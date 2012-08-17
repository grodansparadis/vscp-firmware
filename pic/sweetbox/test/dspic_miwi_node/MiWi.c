/********************************************************************
* GUID=E25BE8D4-3281-4d98-867A-9160A7FD7A31
* GUIInterfaceVersion=1.00
* LibraryVersion=1.00
*
* FileName:		MiWi.c
* Dependencies:    
* Processor:	PIC18, PIC24F, PIC24H, dsPIC30, dsPIC33
*               tested with 18F4620, dsPIC33FJ256GP710	
* Hardware:		PICDEM Z, Explorer 16
* Complier:     Microchip C18 v3.11 or higher
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
*
*
* Change History:
*  Rev   Date         Author	Description
*  0.1   11/09/2006   df        Initial revision
*  1.0   01/09/2007   yfy       Initial release
********************************************************************/

/************************ HEADERS **********************************/
#include ".\MiWi\MSPI.h"
#include ".\MiWi\MiWi.h"
#include ".\MiWi\MRF24J40.h"
#include "Compiler.h"
#include "GenericTypeDefs.h"
#include ".\Common\Console.h"
#include ".\MiWi\SymbolTime.h"
#include "MiWiDefs.h"

/************************ VARIABLES ********************************/
extern ROM unsigned char myLongAddress[8];
extern ROM BYTE availableChannels[AVAILABLE_CHANNELS_SIZE];
#ifdef SUPPORT_SECURITY
	#define INPUT
	#define OUTPUT
	#define IOPUT
	
	extern ROM unsigned char mySecurityLevel;
	extern ROM unsigned char mySecurityKey[16];
	extern ROM unsigned char myKeySequenceNumber;
	DWORD_VAL OutgoingFrameCounter;
	
	BOOL DataEncrypt(IOPUT BYTE *Input, IOPUT BYTE *DataLen, INPUT BYTE *Header, INPUT BYTE HeaderLen);
	BOOL DataDecrypt(IOPUT BYTE *Input, IOPUT BYTE *DataLen, INPUT BYTE *Header, INPUT BYTE HeaderLen);
	
    DWORD_VAL	OutgoingFrameCount;
    DWORD_VAL IncomingFrameCount[NETWORK_TABLE_SIZE];
#endif	

DEST_INFO destInfo;
BYTE tempLongAddress[8];
NETWORK_STATUS_INFO tempNodeStatus;
WORD_VAL tempShortAddress;
WORD_VAL tempPANID;

#if defined(__18CXX)
#pragma udata TRX_BUFFER=0x100
#endif
BYTE RxBuffer[RX_BUFFER_SIZE];
BYTE TxBuffer[TX_BUFFER_SIZE];
#ifdef SUPPORT_SECURITY
    BYTE tmpBuf[TX_BUFFER_SIZE];
#endif
#if defined(__18CXX)
#pragma udata
#endif

BYTE TxHeader;
BYTE TxData;
BYTE *pRxData;
BYTE RxSize;
TICK startTick,tickDifference,tickCurrentTime;
BYTE currentSearchChannel;
BYTE IEEESeqNum;
BYTE MiWiSeqNum;

#if defined(I_AM_COORDINATOR_CAPABLE)
BYTE indirectBuffer[INDIRECT_BUFFER_SIZE];
BYTE RoutingTable[8];
BYTE knownCoordinators;
BYTE role;
#endif

OPEN_SOCKET openSocketInfo;

SEND_REPORT_STATUS sendReportStatus;
volatile MIWI_STATUS MiWiStatus;

#if defined(__18CXX)
#pragma udata BIGvariables1 = 0x200
#endif
NETWORK_TABLE_ENTRY networkTable[NETWORK_TABLE_SIZE];
#if defined(__18CXX)
#pragma udata
#endif
NETWORK_STATUS_INFO networkStatus[NETWORK_TABLE_SIZE];

BYTE RxData;
WORD_VAL myShortAddress;
WORD_VAL myPANID;
volatile BYTE failureCounter = 0;
MIWI_STATE_MACHINE MiWiStateMachine;
BYTE currentChannel;
BYTE myParent;

/************************ FUNCTIONS ********************************/

/*********************************************************************
 * Function:        void MiWiTasks( void )
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
 * Overview:        This function maintains the operation of the stack
 *                  It should be called as often as possible.  
 ********************************************************************/
void MiWiTasks(void)
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
    if(MiWiStatus.bits.TX_BUSY)
    {
        if(failureCounter == 0x20)
        {
            failureCounter = 0;
            MiWiStatus.bits.TX_BUSY = 0;
        }
        else
        {
            failureCounter++;
        }
    }

    //if there is a message pending and there isn't one already buffered
    if((MiWiStatus.bits.RX_PENDING == 1) && (MiWiStatus.bits.RX_BUFFERED == 0))
    {         
        while( MiWiStatus.bits.TX_BUSY ) 
        {
            if(RF_INT_PIN == 0)
            {
                RFIF = 1;
            }
        }
  
        //disable the RF interrupt       
        RFIE = 0;     
        // enter loopback mode
        PHYSetShortRAMAddr(WRITE_BBREG1, 0x04);
               
        //get the size of the packet 
        RxBuffer[0] = PHYGetLongRAMAddr(0x300) + 2;

        if(RxBuffer[0]<RX_BUFFER_SIZE)
        {
            //disable the ACKs until the buffer is cleared
            PHYSetShortRAMAddr(WRITE_RXMCR,0x20);
            
            //indicate that data is now stored in the buffer
            MiWiStatus.bits.RX_BUFFERED = 1;            
            
            //Disable RX of packets until this one is cleared
            MiWiStatus.bits.RX_ENABLED = 0;
            //Clear the pending flag indicating that we removed the packet from the buffer
            MiWiStatus.bits.RX_PENDING = 0;
            
            //copy all of the data from the FIFO into the TxBuffer
            for(i=1;i<=RxBuffer[0];i++)
            {
                RxBuffer[i] = PHYGetLongRAMAddr(0x300+i);
            }
            RxData = 1;
        }
        else
        {
            //else it was a larger packet than we can receive
            //flush it
            PHYSetShortRAMAddr(WRITE_RXFLUSH,0x01);
            MiWiStatus.bits.RX_PENDING = 0;
        }
        
        RFIE = 1;
        
        // get out of loopback mode
        PHYSetShortRAMAddr(WRITE_BBREG1, 0x00);

    }
    
    if(MiWiStatus.bits.RX_BUFFERED == 1)
    {
        BYTE addrMode;
        MAC_FRAME_CONTROL macFrameControl;
        
        //now that we have the packet in the buffer we can discard the packet 
        //to get ready to accept the next packet
        
        //Determine the start of the MAC payload
        addrMode = RxBuffer[2] & 0xCC;
        switch(addrMode)
        {
            case 0x80: //dest-none , source-short
            case 0x08: //dest-short, source-none
                i = 8; //the MAC command is located at byte 7 (the 8th byte in the packet)
                break;
            case 0xC8: //short dest, long source
                i = 18;
                break;
            case 0x8C: //long dest, short source
                i = 18;
            case 0xCC: // long dest, long source
                i = 24;
                {
                    BYTE j;
                    BOOL forMe;
                    
                    //check to see if it is for me
                    forMe = TRUE;
                    
                    for(j=0;j<8;j++)
                    {
                        if(RxBuffer[6+j] != myLongAddress[j])
                        {
                            //if a single byte of the long address doesn't match mine
                            //then it wasn't for me
                            forMe = FALSE;
                        }
                    }
                    
                    if(!forMe)
                    {
                        //if the packet wasn't for me then throw it away
                        goto DiscardPacketAndFinish;
                    }
                }
                break;
            case 0x88: // short dest, short source
                i = 12;
                break;
            default:
                //no addressing info
                break;                
        }  
        
        //if the intra-pan bit was set and it wasn't 0x00, 0x08, 0x80, 0xC0, or 0x0C
        if((RxBuffer[1]&0x40) && ((addrMode & 0x0C) != 0x00) && ((addrMode & 0xC0) != 0x00))
        {
            //IntraPAN bit set so there are not two PANs, thus remove 2 bytes
            i -= 2;
        }
        
        //record the first byte of the frame control
        macFrameControl.Val = RxBuffer[1];

        MiWiStateMachine.RxHasUserData = 0;

        //determine what type of packet it is.
        switch(macFrameControl.bits.frameType)
        {
            //if it is a data packet
            case FRAME_TYPE_DATA:
                //The guts of MiWi ...
                {
                    WORD_VAL destPANID;
                    WORD_VAL destShortAddress;
                    WORD_VAL sourcePANID;
                    WORD_VAL sourceShortAddress;
                                        
                    destPANID.v[0] = RxBuffer[i+2];
                    destPANID.v[1] = RxBuffer[i+3];
                    destShortAddress.v[0] = RxBuffer[i+4];
                    destShortAddress.v[1] = RxBuffer[i+5];
                    
                    sourcePANID.v[0] = RxBuffer[i+6];
                    sourcePANID.v[1] = RxBuffer[i+7];
                    sourceShortAddress.v[0] = RxBuffer[i+8];
                    sourceShortAddress.v[1] = RxBuffer[i+9];                 
                     
                    #if !defined(P2P_ONLY)
                    #if defined(I_AM_COORDINATOR_CAPABLE) && !defined(SUPPORT_P2P_SOCKETS)     
                    if((addrMode & 0x0C) == 0x08)
                    #endif
                    #endif
                    {                        
                        if((destPANID.Val == 0xFFFF) && (destShortAddress.Val == 0xFFFF))
                        {
                            #if defined(I_AM_COORDINATOR_CAPABLE)
                                //This is a broadcast packet
                                if(RxBuffer[i]>0x00)
                                {
                                    BYTE j;
                                    
                                    if((sourcePANID.Val == myPANID.Val) && (sourceShortAddress.Val == myShortAddress.Val))
                                    {
                                        //this is from me... do not rebroadcast it
                                    }
                                    else
                                    {
                                        //create a new variable to store the current read byte at so we can
                                        //also process this packet for us later
                                        j=i;
                                    
                                        RxBuffer[i]--;
                                    
                                        //if the number of hops left is more than 0 then rebroadcast it
                                        while(j<(RxBuffer[0]-3))
                                        {
                                            TxBuffer[TxData++] = RxBuffer[j++];
                                        }   
                                        TxBuffer[TxHeader--] = myShortAddress.v[1]; //source Addr MSB
                                        TxBuffer[TxHeader--] = myShortAddress.v[0]; //source Addr LSB
                                        TxBuffer[TxHeader--] = myPANID.v[1];    //source PANID MSB broadcast
                                        TxBuffer[TxHeader--] = myPANID.v[0];    //source PANID LSB broadcast
                                        TxBuffer[TxHeader--] = 0xFF;    //dest address MSB broadcast
                                        TxBuffer[TxHeader--] = 0xFF;    //dest address LSB broadcast
                                        TxBuffer[TxHeader--] = 0xFF;    //dest PANID MSB broadcast
                                        TxBuffer[TxHeader--] = 0xFF;    //dest PANID LSB broadcast
                                        TxBuffer[TxHeader--] = IEEESeqNum++;    //seq number
                                        TxBuffer[TxHeader--] = 0x88;    //short address for both source and dest
                                        TxBuffer[TxHeader--] = 0x01;    //data, no ACK, not intraPAN
                                        #ifdef SUPPORT_SECURITY
                                        	SendTxBuffer(FALSE, FALSE);
                                        #else
                                            SendTxBuffer(FALSE);
                                        #endif
                                    }
                                }
                            #endif
                            //since this is a broadcast we need to parse the packet as well.
                            goto ThisPacketIsForMe;
                        }
    
    
                        if((destPANID.Val == myPANID.Val) && (destShortAddress.Val == myShortAddress.Val))
                        {                           
                            //need to check to see if the MiWi Ack is set
                            if(RxBuffer[i+1] & MIWI_ACK_REQ)
                            {                                
                                TxBuffer[TxData++] = MAX_HOPS;				//number of hops
                                TxBuffer[TxData++] = 0x02;					//Frame Control
                                TxBuffer[TxData++] = RxBuffer[i+6];
                                TxBuffer[TxData++] = RxBuffer[i+7];
                                TxBuffer[TxData++] = RxBuffer[i+8];
                                TxBuffer[TxData++] = RxBuffer[i+9];
                                TxBuffer[TxData++] = myPANID.v[0];			//source PANID LSB
                                TxBuffer[TxData++] = myPANID.v[1];			//source PANID MSB
                                TxBuffer[TxData++] = myShortAddress.v[0];	//source address LSB
                                TxBuffer[TxData++] = myShortAddress.v[1];	//source address MSB
                                TxBuffer[TxData++] = RxBuffer[i+10];		//sequence number copied from received packet
                                TxBuffer[TxData++] = MIWI_STACK_REPORT_TYPE;
                                TxBuffer[TxData++] = ACK_REPORT_TYPE;
                                
                                switch(RxBuffer[2] & 0xCC)
                                {
                                    #if !defined(P2P_ONLY)
                                    case 0x80: //dest-none , source-short
                                    case 0x08: //dest-short, source-none
                                        i = 8; //the MAC command is located at byte 7 (the 8th byte in the packet)
                                        break;
                                    case 0xC8: //short dest, long source
                                    case 0x8C: //long dest, short source
                                        i = 18;
                                        break;
                                    case 0x88: // short dest, short source
                                        sourceShortAddress.v[1] = RxBuffer[i-1];    //dest address MSB 
                                        sourceShortAddress.v[0] = RxBuffer[i-2];    //dest address LSB 
                                        sourcePANID.v[1] = RxBuffer[5];    //dest PANID MSB broadcast
                                        sourcePANID.v[0] = RxBuffer[4];    //dest PANID LSB broadcast
                                        // forwardPacket should be FALSE here by definition. Use TRUE to
                                        // bypass the requirement of the Sequence number. As the result,
                                        // acknowledgement packet won't be encrypted
                                        SendReportByShortAddress(sourcePANID,sourceShortAddress,TRUE);
                                        break;
                                    #endif
                                    case 0xCC: // long dest, long source
                                        i = 24;
                                        break;
                                    default:
                                        //no addressing info
                                        break;                
                                } 
                            }
ThisPacketIsForMe:
						    #ifdef SUPPORT_SECURITY
							    if( RxBuffer[i+1] & 0x01 ) // security on
    							{
	    							BYTE DataLen = RxBuffer[0] - i - 14;
		    						if( !DataDecrypt(&(RxBuffer[i+11]), &DataLen, &(RxBuffer[i+1]), 10) )
			    					{
					    				goto DiscardPacketAndFinish;
						    		}
							    }
						    #endif
							
							//if the packet is a MiWi stack packet (but not an ACK report type
                            if((RxBuffer[i+11] == MIWI_STACK_REPORT_TYPE) && (RxBuffer[i+12] != ACK_REPORT_TYPE))
                            {                      
                                //record who the packet is from          
                                sourcePANID.v[0] = RxBuffer[i+6];
                                sourcePANID.v[1] = RxBuffer[i+7];
                                sourceShortAddress.v[0] = RxBuffer[i+8];
                                sourceShortAddress.v[1] = RxBuffer[i+9];
                                
                                //determine which stack report it was
                                switch(RxBuffer[i+12])
                                {                                
                                    #if defined(SUPPORT_EUI_ADDRESS_SEARCH)
                                    #if defined(I_AM_COORDINATOR_CAPABLE) 
                                    
                                    case EUI_ADDRESS_SEARCH_REQUEST:
                                    {
                                        BYTE handle,j;
                                        BOOL ItsMe;

                                        if(sourceShortAddress.Val == myShortAddress.Val)
                                        {
                                            if(sourcePANID.Val == myPANID.Val)
                                            {
                                                //I started this message.  I don't need to re-broadcast it.
                                                break;
                                            }
                                        }
                                        
                                        //copy the address into tempLongAddress
                                        ConsolePutROMString((ROM char*)"\r\nEUI address search request\r\n");
                                        tempLongAddress[0] = RxBuffer[i+13];
                                        tempLongAddress[1] = RxBuffer[i+14];
                                        tempLongAddress[2] = RxBuffer[i+15];
                                        tempLongAddress[3] = RxBuffer[i+16];
                                        tempLongAddress[4] = RxBuffer[i+17];
                                        tempLongAddress[5] = RxBuffer[i+18];
                                        tempLongAddress[6] = RxBuffer[i+19];
                                        tempLongAddress[7] = RxBuffer[i+20];
                                        
                                        ItsMe = TRUE;
                                        for(j=0;j<8;j++)
                                        {
                                            //see if the address in question is my own
                                            if(tempLongAddress[j] != myLongAddress[j])
                                            {
                                                ItsMe = FALSE;
                                                break;
                                            }
                                        }
                                        
                                        //load up the TX Buffer with the results
                                        TxPayLoad();
                                        TxBuffer[TxData++] = MIWI_STACK_REPORT_TYPE;      //Report Type
                                        TxBuffer[TxData++] = EUI_ADDRESS_SEARCH_RESPONSE;      //Report ID
                                        TxBuffer[TxData++] = tempLongAddress[0];      //Data byte 0
                                        TxBuffer[TxData++] = tempLongAddress[1];
                                        TxBuffer[TxData++] = tempLongAddress[2];
                                        TxBuffer[TxData++] = tempLongAddress[3];
                                        TxBuffer[TxData++] = tempLongAddress[4];
                                        TxBuffer[TxData++] = tempLongAddress[5];
                                        TxBuffer[TxData++] = tempLongAddress[6];
                                        TxBuffer[TxData++] = tempLongAddress[7];
                                        
                                        //if it wasn't me
                                        if(ItsMe == FALSE)
                                        {
                                            //search the network table for the device
                                            handle = SearchForLongAddress();
                                            
                                            //if we found the device
                                            if(handle!=0xFF)
                                            {
                                                //check to see if the short address is valid for that device
                                                if(networkStatus[handle].bits.shortAddressValid)
                                                {
                                                    //I know the device in question so I will send back a response
                                                    TxBuffer[TxData++] = networkTable[handle].PANID.v[0];
                                                    TxBuffer[TxData++] = networkTable[handle].PANID.v[1];
                                                    TxBuffer[TxData++] = networkTable[handle].ShortAddress.v[0];
                                                    TxBuffer[TxData++] = networkTable[handle].ShortAddress.v[1];
                                                     
                                                    //if everything is cool then send the response back    
                                                    SendReportByShortAddress(sourcePANID,sourceShortAddress,FALSE);
                                                }
                                                else
                                                {
                                                    //if we didn't have the short address then we can't
                                                    //respond to this packet even though we know the long address
                                                    ConsolePutROMString((ROM char*)"found long address but short address not valid\r\n");
                                                    //clear out the data I already wrote
                                                    TxData = 0;     
                                                }
        
                                            }
                                            else
                                            {
                                                //if we couldn't find the long address of the device then forget it
                                                ConsolePutROMString((ROM char*)"couldn't find the device");
                                                TxData = 0;     
                                            }
                                        }
                                        else
                                        {
                                            //The node in question is me
                                            TxBuffer[TxData++] = myPANID.v[0];
                                            TxBuffer[TxData++] = myPANID.v[1];
                                            TxBuffer[TxData++] = myShortAddress.v[0];
                                            TxBuffer[TxData++] = myShortAddress.v[1];
                                               
                                            //send a response back with my own information
                                            SendReportByShortAddress(sourcePANID,sourceShortAddress,FALSE);
                                        }
                                        
                                        break;
                                    }
                                    #endif //COORDINATOR_CAPABLE
                                    #endif //SUPPORT_EUI_ADDRESS_SEARCH
                                    #if defined(SUPPORT_EUI_ADDRESS_SEARCH)
                                    case EUI_ADDRESS_SEARCH_RESPONSE:
                                        //got a response back from the IEEE address search
                                        //record all of the information in the packet
                                        //long address
                                        tempLongAddress[0] = RxBuffer[i+13];
                                        tempLongAddress[1] = RxBuffer[i+14];
                                        tempLongAddress[2] = RxBuffer[i+15];
                                        tempLongAddress[3] = RxBuffer[i+16];
                                        tempLongAddress[4] = RxBuffer[i+17];
                                        tempLongAddress[5] = RxBuffer[i+18];
                                        tempLongAddress[6] = RxBuffer[i+19];
                                        tempLongAddress[7] = RxBuffer[i+20];
                                        //PANID and short address
                                        tempPANID.v[0] = RxBuffer[i+21];
                                        tempPANID.v[1] = RxBuffer[i+22];
                                        tempShortAddress.v[0] = RxBuffer[i+23];
                                        tempShortAddress.v[1] = RxBuffer[i+24];
                                        
                                        //create a new Status entry for the network table
                                        tempNodeStatus.Val = 0x2D;
                                        if((tempShortAddress.Val == sourceShortAddress.Val) && (tempPANID.Val == sourcePANID.Val))
                                        {
                                            tempNodeStatus.bits.directConnection = 1;
                                        }
                                        if((tempShortAddress.v[0] & 0x80) == 0)
                                        {
                                            tempNodeStatus.bits.RXOnWhenIdle = 1;
                                        }
                                        //add the node to the network table
                                        AddNodeToNetworkTable();
                                        break;
                                    #endif
                                    
                                    #if defined(SUPPORT_P2P_SOCKETS)
                                    #if defined(I_AM_FFD)
                                    case OPEN_P2P_SOCKET_REQUEST:
                                        ConsolePutROMString((ROM char*)"Got a P2P request from another device.\r\n");
                                        if(openSocketInfo.status.bits.requestIsOpen)
                                        {
                                            //if we have made a similar request recently
                                            //then we need to send a response
                                            if(openSocketInfo.status.bits.type == P2P_SOCKET)
                                            {
                                                {
                                                    //record the long address in the request
                                                    BYTE j;
                                                    for(j=0;j<8;j++)
                                                    {
                                                        openSocketInfo.LongAddress1[j] = RxBuffer[i+j-8];
                                                    }
                                                }
                                                {
                                                    BYTE handle,i;
                                                    BOOL previousEntry;
                                                    
                                                    previousEntry = FALSE;
                                                    
                                                    //look to see if that address already exists in our table
                                                    
                                                    for(i=0;i<8;i++)
                                                    {
                                                        tempLongAddress[i] = openSocketInfo.LongAddress1[i];
                                                    }
                                                    
                                                    handle = SearchForLongAddress();
                                                    
                                                    //see if we already knew the device
                                                    if(handle != 0xFF)
                                                    {
                                                        previousEntry = TRUE;
                                                    }
                                                    else
                                                    { 
                                                        //if we didn't know the device then create
                                                        //a new entry for the device                
                                                        handle = findNextNetworkEntry();
                                                    }
                                                      
                                                    //if we have an entry for the device  
                                                    if(handle != 0xFF)
                                                    {
                                                        //send a repsonse to the device
                                                        openSocketInfo.socketHandle = handle;
                                                        
                                                        //if there is room in the table to add them
                                                        //send out an open P2P socket request               
                                                        TxBuffer[TxData++] = 0; //no hops required because it will be a direct communication
                                                        TxBuffer[TxData++] = 0x00;  //frame control - no ack, intra cluster, no encrypt (for now)
                                                        TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
                                                        TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
                                                        TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
                                                        TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
                                                        TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
                                                        TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
                                                        TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
                                                        TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
                                                        TxBuffer[TxData++] = MiWiSeqNum++;      //sequence number
                                                        TxBuffer[TxData++] = MIWI_STACK_REPORT_TYPE;      //Report Type
                                                        TxBuffer[TxData++] = OPEN_P2P_SOCKET_RESPONSE;      //Report ID
                                                        
                                                        TxBuffer[TxHeader--] = myLongAddress[7];  //MSB source long address 
                                                        TxBuffer[TxHeader--] = myLongAddress[6];  //source long address 
                                                        TxBuffer[TxHeader--] = myLongAddress[5];  //source long address 
                                                        TxBuffer[TxHeader--] = myLongAddress[4];  //source long address 
                                                        TxBuffer[TxHeader--] = myLongAddress[3];  //source long address 
                                                        TxBuffer[TxHeader--] = myLongAddress[2];  //source long address 
                                                        TxBuffer[TxHeader--] = myLongAddress[1];  //source long address 
                                                        TxBuffer[TxHeader--] = myLongAddress[0];  //source long address 
                                                        
                                                        TxBuffer[TxHeader--] = 0xFF;  //MSB source PANID
                                                        TxBuffer[TxHeader--] = 0xFF;  //source PANID
                                                        
                                                        TxBuffer[TxHeader--] = openSocketInfo.LongAddress1[7];
                                                        TxBuffer[TxHeader--] = openSocketInfo.LongAddress1[6];
                                                        TxBuffer[TxHeader--] = openSocketInfo.LongAddress1[5];
                                                        TxBuffer[TxHeader--] = openSocketInfo.LongAddress1[4];
                                                        TxBuffer[TxHeader--] = openSocketInfo.LongAddress1[3];
                                                        TxBuffer[TxHeader--] = openSocketInfo.LongAddress1[2];
                                                        TxBuffer[TxHeader--] = openSocketInfo.LongAddress1[1];
                                                        TxBuffer[TxHeader--] = openSocketInfo.LongAddress1[0];
                                                
                                                        TxBuffer[TxHeader--] = 0xFF;  //MSB dest PANID
                                                        TxBuffer[TxHeader--] = 0xFF;  //dest PANID
                                                        
                                                        TxBuffer[TxHeader--] = IEEESeqNum++;  //seq num
                                                        
                                                        TxBuffer[TxHeader--] = 0xCC;  //frame control -  both long addresses
                                                        
                                                        TxBuffer[TxHeader--] = 0x21;  //frame control -  command packet
                                                
                                                    #ifdef SUPPORT_SECURITY
                                                    	SendTxBuffer(FALSE, TRUE);
                                                    #else
                                                        SendTxBuffer(FALSE);
                                                    #endif
                                                        
                                                        //mark as having found a match
                                                        openSocketInfo.status.bits.matchFound = 1;
                                                        openSocketInfo.status.bits.requestIsOpen = 0;
                                                        
                                                        if(previousEntry == FALSE)
                                                        {
                                                            //if I didn't know this node before then add him to the table.  otherwise leave the old data alone
                                                            networkStatus[handle].Val = 0x37;
                                                            for(i=0;i<8;i++)
                                                            {
                                                                networkTable[handle].info.LongAddress[i] = openSocketInfo.LongAddress1[i];
                                                            }
                                                        }
                                                    }      
                                                }                                    
                                            }
                                        }
                                        else
                                        {
                                            //if a P2P socket request wasn't already open
                                            BYTE j;
                                            
                                            //open a new request and save the important information 
                                            //about the request
                                            openSocketInfo.status.bits.requestIsOpen = 1;
                                            openSocketInfo.status.bits.itIsMe = 0;
                                            openSocketInfo.status.bits.type = P2P_SOCKET;
                                            
                                            ConsolePutROMString((ROM char*)"Socket wasn't already open\r\n");
    
                                            openSocketInfo.socketStart = TickGet();
                                            openSocketInfo.ShortAddress1.v[0] = RxBuffer[i+8];
                                            openSocketInfo.ShortAddress1.v[1] = RxBuffer[i+9];
    
                                            for(j=0;j<8;j++)
                                            {
                                                openSocketInfo.LongAddress1[j] = RxBuffer[i+j-8];
                                            }
                                            openSocketInfo.status.bits.requestIsOpen = 1;
                                            openSocketInfo.status.bits.type = CLUSTER_SOCKET;
                                            openSocketInfo.status.bits.matchFound = 0; 
                                        }
                                        break;
                                    #endif  //I_AM_FFD
                                    #endif  //SUPPORT_P2P_SOCKETS
                                    
                                    #if defined(SUPPORT_P2P_SOCKETS)
                                    case OPEN_P2P_SOCKET_RESPONSE:
                                        {
                                            BYTE j;
                                            
                                            //we found a socket so save the important information
                                            openSocketInfo.status.bits.requestIsOpen = 0;
                                            openSocketInfo.status.bits.matchFound = 1;
                                            for(j=0;j<8;j++)
                                            {
                                                openSocketInfo.LongAddress1[j] = RxBuffer[i+j-8];
                                            }    
                                        }                                
                                        break;
                                    #endif 
                                    #if defined(SUPPORT_CLUSTER_SOCKETS)
                                    #if defined(I_AM_COORDINATOR_CAPABLE)
                                    case OPEN_CLUSTER_SOCKET_REQUEST:
                                        {
                                            BYTE j;
                                            
                                            //if we have a socket request open
                                            if(openSocketInfo.status.bits.requestIsOpen == 0)
                                            {
                                                //if there isn't a request already open
                                                //get the current time and the source address
                                                openSocketInfo.socketStart = TickGet();
                                                openSocketInfo.ShortAddress1.v[0] = RxBuffer[i+8];
                                                openSocketInfo.ShortAddress1.v[1] = RxBuffer[i+9];
    
                                                //copy the long address
                                                for(j=0;j<8;j++)
                                                {
                                                    openSocketInfo.LongAddress1[j] = RxBuffer[i+j+13];
                                                }
                                                //mark a request as open
                                                openSocketInfo.status.bits.requestIsOpen = 1;
                                                openSocketInfo.status.bits.type = CLUSTER_SOCKET;
                                                openSocketInfo.status.bits.matchFound = 0;
                                            }
                                            else
                                            {
                                                //if there was already a request open
                                                if(openSocketInfo.status.bits.type == CLUSTER_SOCKET)
                                                {
                                                    if((RxBuffer[i+8] == openSocketInfo.ShortAddress1.v[0]) && (RxBuffer[i+9] == openSocketInfo.ShortAddress1.v[1]))
                                                    {
                                                        //if we get a request from the same guy twice,
                                                        //just throw away the second request
                                                        //ConsolePutROMString((ROM char*)"got request from same guy twice\r\n");
                                                    }
                                                    else if(openSocketInfo.ShortAddress1.Val == 0x0000)
                                                    {                                                
                                                        //I am the PAN coordinator and I initiated the request so don't send back a response to myself
                                                        //copy the long and short address from the Rx Buffer
                                                        for(j=0;j<8;j++)
                                                        {
                                                            openSocketInfo.LongAddress2[j] = RxBuffer[i+13+j];      //Data byte
                                                            tempLongAddress[j] = openSocketInfo.LongAddress2[j];
                                                        }
                                                        
                                                        //save the important information about the node sending 
                                                        //the request
                                                        openSocketInfo.ShortAddress2.v[0] = RxBuffer[i+8];
                                                        openSocketInfo.ShortAddress2.v[1] = RxBuffer[i+9];
                                                        tempShortAddress.v[0] = openSocketInfo.ShortAddress2.v[0];
                                                        tempShortAddress.v[1] = openSocketInfo.ShortAddress2.v[1];
                                                        tempPANID.Val = myPANID.Val;
                                                        
                                                        //look to see if that device exists in our network table
                                                        openSocketInfo.socketHandle = SearchForShortAddress(NEIGHBOR);
                                                        
                                                        if(openSocketInfo.socketHandle != 0xFF)
                                                        {
                                                            //if they do exists then copy their status information
                                                            tempNodeStatus.Val = networkStatus[openSocketInfo.socketHandle].Val;
                                                        }
                                                        else
                                                        {
                                                            //otherwise create it
                                                            //Family, RxOnWHenIdle, Neighbor/Network, P2P, ShortVal, LongVal, Direct, Valid
                                                            tempNodeStatus.Val = 0x2D;
                                                            if((tempShortAddress.v[0] & 0x80) == 0)
                                                            {
                                                                tempNodeStatus.Val |= 0x40;
                                                            }
                                                        }
                                                                                          
                                                        openSocketInfo.status.bits.requestIsOpen = 1;
                                                        openSocketInfo.status.bits.matchFound = 1; 
                                                    }
                                                    else
                                                    {                                            
                                                        ConsolePutROMString((ROM char*)"sending out response\r\n");
                                                        
                                                        //we got a match so let's send out the response
                                                        
                                                        TxPayLoad();
                                                        TxBuffer[TxData++] = MIWI_STACK_REPORT_TYPE;      //Report Type
                                                        TxBuffer[TxData++] = OPEN_CLUSTER_SOCKET_RESPONSE;      //Report ID
        
                                                        //copy the long and short address from the Rx Buffer
                                                        for(j=0;j<8;j++)
                                                        {
                                                            openSocketInfo.LongAddress2[j] = RxBuffer[i+13+j];      //Data byte
                                                            TxBuffer[TxData++] = openSocketInfo.LongAddress2[j]; 
                                                        }
        
                                                        openSocketInfo.ShortAddress2.v[0] = RxBuffer[i+8];
                                                        openSocketInfo.ShortAddress2.v[1] = RxBuffer[i+9];
                                                        TxBuffer[TxData++] = openSocketInfo.ShortAddress2.v[0];
                                                        TxBuffer[TxData++] = openSocketInfo.ShortAddress2.v[1];
                                                        
                                                        SendReportByShortAddress(myPANID,openSocketInfo.ShortAddress1,FALSE);
                                                        openSocketInfo.status.bits.requestIsOpen = 1;
                                                        openSocketInfo.status.bits.matchFound = 1;    
                                                    }
                                                }
                                            }
                                        }
                                        break;
                                        #endif
                                    case OPEN_CLUSTER_SOCKET_RESPONSE:
                                        //ConsolePutROMString((ROM char*)"found a response to my request\r\n");
                                        {
                                            BYTE j;
                                            
                                            //save the long address of the device
                                            for(j=0;j<8;j++)
                                            {
                                                tempLongAddress[j] = RxBuffer[j+i+13];
                                            }
                                            
                                            //create a new status for the device
                                            
                                            tempNodeStatus.Val = 0x2D;
                                            
                                            //check the RxOnWhenIdle bit
                                            if((RxBuffer[i+21] & 0x80) == 0)
                                            {
                                                tempNodeStatus.Val |= 0x40;   //set the RxOnWhenIdle bit
                                            }
                                            
                                            //copy the short address
                                            tempShortAddress.v[0] = RxBuffer[i+21];
                                            tempShortAddress.v[1] = RxBuffer[i+22];
                                            
                                            //use my PANID since this version of MiWi only supports single PANIDs
                                            tempPANID.Val = myPANID.Val;
                                            
                                            //see if this node already exists in my table
                                            openSocketInfo.socketHandle = SearchForShortAddress(NEIGHBOR);
                                            
                                            if(openSocketInfo.socketHandle != 0xFF)
                                            {
                                                //if it does then get the status of the node already in the table
                                                tempNodeStatus.Val = networkStatus[openSocketInfo.socketHandle].Val;
                                            }
                                            else
                                            {
                                                //Family, RxOnWHenIdle, Neighbor/Network, P2P, ShortVal, LongVal, Direct, Valid
                                                tempNodeStatus.Val = 0x2D;
    
                                                if((tempShortAddress.v[0] & 0x80) == 0)
                                                {
                                                    tempNodeStatus.Val |= 0x40;
                                                }
                                            }
                                            //update the network information
                                            openSocketInfo.socketHandle = AddNodeToNetworkTable();
                                            
                                            //clear out out request
                                            openSocketInfo.status.bits.requestIsOpen = 0;
                                            openSocketInfo.status.bits.matchFound = 1;
                                        }
                                        break;
                                    #endif
                                    default:
                                        //unknown stack request.  don't do anything
                                        //just let this packet die
                                        break;
                                }
                            }
                            else
                            {
                                //This data is for the user, pass it up to them
                                #if defined(I_AM_RFD)
                                    MiWiStateMachine.dataRequestComplete = 1;
                                #endif
                                pRxData = &RxBuffer[i+10];
                                RxSize = RxBuffer[0] - (i + 13);
                                MiWiStateMachine.RxHasUserData = 1;
                            }
                        }
                        else
                        {
                            //if this packet wasn't for me
                            //ConsolePutROMString((ROM char*)"This packet isn't for me\r\n"); 
                            #if defined(I_AM_COORDINATOR_CAPABLE)
                                //then if I am a coordinator pass it along to the
                                //next hop, decrementing the number of hops available
                                RxBuffer[i]--;      //decrement the hops counter
                                while(i<(RxBuffer[0]-3))
                                {
                                    TxBuffer[TxData++] = RxBuffer[i++];
                                } 
                                SendReportByShortAddress(destPANID, destShortAddress,TRUE); 
                            #endif    
                        }
                    }
                }
                break;
            #if !defined(P2P_ONLY)
            //if it is a beacon packet
            case FRAME_TYPE_BEACON:
                {
                    BEACON_SUPERFRAME beaconSuperframe;
                    BYTE ProtocolId;
                    BYTE VersionNumber;
                    
                    #if defined(I_AM_COORDINATOR_CAPABLE)
                    {
                        //if I am a coordinator then get 
                        //the coordinators number
                        BYTE coordinatorNumber = RxBuffer[7] & 0x03;
                        BYTE mask = 1<<(coordinatorNumber);
                        
                        //make sure it is a MiWi beacon
                        if(RxBuffer[12] == MIWI_PROTOCOL_ID)
                        {
                            //Make sure its a MiWi coordinator
                            if(RxBuffer[6] == 0x00)
                            {
                                //if it is then mark this device as known
                                knownCoordinators |= mask;
                            }
                        }
                        
                        //take this new coordinator and mark this coordinator as 
                        //a possible route for any of the devices
                        {
                            BYTE_VAL data;
                            BYTE j;
                            
                            data.Val = RxBuffer[14];
                            
                            for(j=0;j<8;j++)
                            {
                                if(data.bits.b0)
                                {
                                    RoutingTable[j] |= mask;
                                }
                                else
                                {
                                    RoutingTable[j] &= !mask;
                                }
                                //shift right one
                                data.Val >>=1;
                            }
                        }
                    }
                    #endif
                                       
                    //if we are looking for a network
                    if(MiWiStateMachine.searchingForNetwork)
                    {
                        BYTE entry;
                        
                        tempPANID.v[0] = RxBuffer[4];
                        tempPANID.v[1] = RxBuffer[5];
                        tempShortAddress.v[0] = RxBuffer[6];
                        tempShortAddress.v[1] = RxBuffer[7];                    
                                   
                        //let's see if this device already exists
                        entry = SearchForShortAddress(NETWORK); 
                        
                        if(entry == 0xFF)
                        {
                            //this device doesn't already exist in our network table
                            //let's create a new entry for it.           
                            entry = findNextNetworkEntry();                      
                        }
                                               
                        //ignore all beacon networks
                        if(RxBuffer[i++] == 0xFF)
                        {
                            //save the channel number where it was found
                            networkTable[entry].info.networkInfo.Channel = currentChannel;
                                           
                            networkTable[entry].PANID.Val = tempPANID.Val;
                            networkTable[entry].ShortAddress.Val = tempShortAddress.Val;
                            
                            //save off superframe flags
                            networkTable[entry].info.networkInfo.flags.Val = RxBuffer[i] & 0xC0;
                        
                            //don't care about GTS or Pending so skip them
                            i+=3;
                        
                            //save protocol information
                            networkTable[entry].info.networkInfo.Protocol = RxBuffer[i++];
                            //save version number
                            networkTable[entry].info.networkInfo.Version = RxBuffer[i++];
                            //save the RSSI information
                            networkTable[entry].info.networkInfo.sampleRSSI = RxBuffer[i+4];
                            //mark this as network and not as a neighbor
                            networkStatus[entry].bits.NeighborOrNetwork = 0;
                            //mark this as a valid entry 
                            networkStatus[entry].bits.isValid = 1;
                        }
                    }
                }
                break;
            //if it is a command packet
            case FRAME_TYPE_COMMAND:
                //figure out which command packet it is
                switch(RxBuffer[i])
                {
                    #if defined(I_AM_COORDINATOR_CAPABLE)
                    case MAC_COMMAND_ASSOCIATION_REQUEST:
                        {
                            BYTE handle;
                            
                            //get the long address of the device
                            tempLongAddress[0] = RxBuffer[i-8];
                            tempLongAddress[1] = RxBuffer[i-7];
                            tempLongAddress[2] = RxBuffer[i-6];
                            tempLongAddress[3] = RxBuffer[i-5];
                            tempLongAddress[4] = RxBuffer[i-4];
                            tempLongAddress[5] = RxBuffer[i-3];
                            tempLongAddress[6] = RxBuffer[i-2];
                            tempLongAddress[7] = RxBuffer[i-1];

                            tempNodeStatus.Val = 0xA7;

                            //add this node to the network table
                            handle = AddNodeToNetworkTable();
                                                       
                            //if entry was successful then assign the device a short address
                            if(handle != 0xFF)
                            {
                                //set the short address as an end device address
                                tempShortAddress.v[0] = handle + 1;
                                tempShortAddress.v[1] = myShortAddress.v[1];
                                tempPANID.Val = myPANID.Val;
                                
                                //if I am the PAN coordinator
                                if(role == ROLE_PAN_COORDINATOR)
                                {
                                    if(RxBuffer[i+1] & 0x01)
                                    {
                                        //If this device is a potential coordinator
                                        BYTE j;
                                        WORD_VAL CoordAddress;
                                        BYTE entry;
                                                                                
                                        CoordAddress.Val = tempShortAddress.Val;
                                        tempShortAddress.v[0] = 0;
                                        
                                        //search to see if there is a coordinator address available
                                        for(j=1;j<8;j++)
                                        {
                                            tempShortAddress.v[1] = j;                                        
                                            entry = SearchForShortAddress(NEIGHBOR);
                                            
                                            if(entry == 0xFF)
                                            {
                                                tempShortAddress.v[0] = 0x00;
                                                tempShortAddress.v[1] = j;
                                                knownCoordinators |= (1<<j);
                                                break;
                                            }
                                        }
                                        
                                        if(j==8)
                                        {
                                            tempShortAddress.Val= CoordAddress.Val;
                                        }
                                        
                                    }
                                }
                                //create a new status entry for the device
                                tempNodeStatus.Val = 0xAF;
                                
                                //mark if the Rx is on when idle
                                if((RxBuffer[i+1] & 0x08) == 0x08)
                                {
                                    tempNodeStatus.bits.RXOnWhenIdle = 1;
                                }
                                else
                                {
                                    tempShortAddress.v[0] |= 0x80;                                   
                                }
                                
                                //update the information in the network table
                                handle = AddNodeToNetworkTable();
                                
                                //send back the asociation response
                                TxBuffer[0] = MAC_COMMAND_ASSOCIATION_RESPONSE;
                                TxBuffer[1] = tempShortAddress.v[0];
                                TxBuffer[2] = tempShortAddress.v[1];
                                TxBuffer[3] = ASSOCIATION_SUCCESSFUL;
                                TxData = 4;
                                
                            #if defined(SUPPORT_SECURITY)
						        networkTable[handle].IncomingFrameCounter.Val = 0;
					        #endif
                                
                                SendIndirectMessage(handle);
                            }
                        }
                        break;
                    #endif
                    case MAC_COMMAND_ASSOCIATION_RESPONSE:
                        //ConsolePutROMString((ROM char*)"Association Response\r\n");
                        if(RxBuffer[i+3] == ASSOCIATION_SUCCESSFUL)
                        {
                            //The join was successful
                            //assign myself the correct short address
                            myShortAddress.v[0] = RxBuffer[i+1];
                            myShortAddress.v[1] = RxBuffer[i+2];
                                                        
                            #if defined(I_AM_COORDINATOR_CAPABLE)
                            if(myShortAddress.v[0] == 0x00)
                            {
                                ConsolePutROMString((ROM char*)"I am a coordinator\r\n");
                                role = ROLE_COORDINATOR;
                                knownCoordinators = (1<<(myShortAddress.v[1] & 0x03)) + 1;
                                //I know the PAN coordinator and myself
                            }
                            else
                            {
                                ConsolePutROMString((ROM char*)"I am an end device\r\n");
                                role = ROLE_FFD_END_DEVICE;
                            }
                            #endif
                            
                            //set the short address of the device
                            PHYSetShortRAMAddr(WRITE_SADRL,myShortAddress.v[0]);
                            PHYSetShortRAMAddr(WRITE_SADRH,myShortAddress.v[1]);
                            //mark as having joined a network
                            MiWiStateMachine.memberOfNetwork = 1;
                            //and clear that we are no longer searching for a netork
                            MiWiStateMachine.attemptingToJoin = 0;
                            
                            //record the parents information
                            tempNodeStatus.Val = 0xEF;
                            tempLongAddress[0] = RxBuffer[i-8];
                            tempLongAddress[1] = RxBuffer[i-7];
                            tempLongAddress[2] = RxBuffer[i-6];
                            tempLongAddress[3] = RxBuffer[i-5];
                            tempLongAddress[4] = RxBuffer[i-4];
                            tempLongAddress[5] = RxBuffer[i-3];
                            tempLongAddress[6] = RxBuffer[i-2];
                            tempLongAddress[7] = RxBuffer[i-1];

                            if(myShortAddress.v[0] == 0x00)
                            {
                                //if I am a coordinator then my parent is the PAN coordinator
                                //and I am a router
                                tempShortAddress.Val = 0x0000;
                            }
                            else
                            {
                                tempShortAddress.v[1] = myShortAddress.v[1];
                                tempShortAddress.v[0] = 0x00;
                            }
                            
                            tempPANID.Val = myPANID.Val;
                            
                            //and add the parents information to the network table
                            myParent = AddNodeToNetworkTable();
                            #if defined(SUPPORT_SECURITY)
						        networkTable[myParent].IncomingFrameCounter.Val = 0;
					        #endif                            
					        DumpNetworkTable();
					        
                            ConsolePutROMString((ROM char *)"Join the network successfully\r\n");                            
                        }
                        else
                        {
                            //Access denied, remove this as my parent
                            myParent = 0xFF;
                            myPANID.Val = 0xFFFF;
                            PHYSetShortRAMAddr(WRITE_PANIDL,0xFF);
                            PHYSetShortRAMAddr(WRITE_PANIDH,0xFF);
                        }
                        
                        {
                            unsigned int p;
                            
                            for(p=0;p<10000;p++)
                            {
                            }
                        }
                        break;
                    #if defined(I_AM_COORDINATOR_CAPABLE)
                    case MAC_COMMAND_DISASSOCIATION_NOTIFICATION:
                        ConsolePutROMString((ROM char*)"Disassociation notification\r\n");
                        break;
                    #endif
                    #if defined(I_AM_COORDINATOR_CAPABLE)
                    case MAC_COMMAND_DATA_REQUEST:
                    {
                        BYTE handle;
                        
                        switch(RxBuffer[2] & 0xCC)
                        {
                            case 0x80: //dest-none , source-short
                            case 0x08: //dest-short, source-none
                                break;
                            case 0xC8: //short dest, long source                                    
                                //ConsolePutROMString((ROM char*)"association response data request\r\n");
                                                               
                                tempLongAddress[0] = RxBuffer[i-8];
                                tempLongAddress[1] = RxBuffer[i-7];
                                tempLongAddress[2] = RxBuffer[i-6];
                                tempLongAddress[3] = RxBuffer[i-5];
                                tempLongAddress[4] = RxBuffer[i-4];
                                tempLongAddress[5] = RxBuffer[i-3];
                                tempLongAddress[6] = RxBuffer[i-2];
                                tempLongAddress[7] = RxBuffer[i-1];

                                //see if we know this device
                                handle = SearchForLongAddress();
                                                            
                                //if we do then see if there is a packet waiting for this device    
                                if(handle!=0xFF)
                                {
                                    SendIndirectPacket(handle,DATA_REQUEST_ASSOCIATION_RESPONSE);
                                }
                                break;
                            case 0x8C: //long dest, short source
                                break;
                            case 0xCC: // long dest, long source
                                break;
                            case 0x88: // short dest, short source
                                    tempPANID.v[0] = RxBuffer[4];
                                    tempPANID.v[1] = RxBuffer[5];

                                    tempShortAddress.v[0] = RxBuffer[i-2];
                                    tempShortAddress.v[1] = RxBuffer[i-1];

                                    //see if we know this device
                                    handle = SearchForShortAddress(NEIGHBOR);
                                    
                                    //if we do then see if there is a packet waiting
                                    //for this device
                                    if(handle!=0xFF)
                                    {
                                        SendIndirectPacket(handle,DATA_REQUEST_SHORT_ADDRESSES);
                                    }
                                
                                
                                break;
                            default:
                                //no addressing info
                                break;                
                        }
                        break;
                    }
                    #endif
                    #if defined(I_AM_COORDINATOR_CAPABLE)
                    case MAC_COMMAND_PAN_ID_CONFLICT_NOTIFICATION:
                        //ConsolePutROMString((ROM char*)"PAN id conflict notification\r\n");
                        break;
                    #endif
                    #if defined(I_AM_COORDINATOR_CAPABLE)
                    case MAC_COMMAND_ORPHAN_NOTIFICATION:
                        {
                            BYTE handle;
                            
                            tempLongAddress[0] = RxBuffer[i-8];
                            tempLongAddress[1] = RxBuffer[i-7];
                            tempLongAddress[2] = RxBuffer[i-6];
                            tempLongAddress[3] = RxBuffer[i-5];
                            tempLongAddress[4] = RxBuffer[i-4];
                            tempLongAddress[5] = RxBuffer[i-3];
                            tempLongAddress[6] = RxBuffer[i-2];
                            tempLongAddress[7] = RxBuffer[i-1];
                            
                            //see if we knew this device
                            handle = SearchForLongAddress();
                            
                            if(handle!=0xFF)
                            {
                                //if this device was family before
                                if(networkStatus[handle].bits.isFamily == 1)
                                {
                                    //send a coordiator realignment 
                                    TxBuffer[0]=0x23;   //frame control - beacon
                                    TxBuffer[1]=0xCC;   //addressing mode - source only
                                    TxBuffer[2]=IEEESeqNum++;   
                                    TxBuffer[3]=0xFF;   //PANID LSB (broadcast)
                                    TxBuffer[4]=0xFF;   //PANID MSB (broadcast)
                                    TxBuffer[5]=tempLongAddress[0];   //dest long address
                                    TxBuffer[6]=tempLongAddress[1];   //dest long address
                                    TxBuffer[7]=tempLongAddress[2];   //dest long address
                                    TxBuffer[8]=tempLongAddress[3];   //dest long address
                                    TxBuffer[9]=tempLongAddress[4];   //dest long address
                                    TxBuffer[10]=tempLongAddress[5];   //dest long address
                                    TxBuffer[11]=tempLongAddress[6];   //dest long address
                                    TxBuffer[12]=tempLongAddress[7];   //dest long address
                                    
                                    TxBuffer[13]=myPANID.v[0];   //PANID LSB
                                    TxBuffer[14]=myPANID.v[1];   //PANID address MSB
                                    
                                    TxBuffer[15]=myLongAddress[0];  //source long address
                                    TxBuffer[16]=myLongAddress[1];  //source long address
                                    TxBuffer[17]=myLongAddress[2];  //source long address
                                    TxBuffer[18]=myLongAddress[3];  //source long address
                                    TxBuffer[19]=myLongAddress[4];  //source long address
                                    TxBuffer[20]=myLongAddress[5];  //source long address
                                    TxBuffer[21]=myLongAddress[6];  //source long address
                                    TxBuffer[22]=myLongAddress[7];  //source long address
                                    
                                    TxBuffer[23]=0x08;   //coordinator realignment
                                    
                                    TxBuffer[24]=networkTable[handle].PANID.v[0];   //devices PANID
                                    TxBuffer[25]=networkTable[handle].PANID.v[1];   //devices PANID
                                    
                                    //this device was the PAN coordinator
                                    if(networkTable[handle].ShortAddress.Val == 0x0000)
                                    {
                                        TxBuffer[26]=0x00;   //parent address (itself)
                                        TxBuffer[27]=0x00;   //parent address (itself)
                                    }
                                    else
                                    {
                                        TxBuffer[26]=myShortAddress.v[0];   //my short address
                                        TxBuffer[27]=myShortAddress.v[1];   //my short address
                                    }
                                            
                                    TxBuffer[28]=(currentChannel>>4)+11;  //channel
                                    TxBuffer[29]=networkTable[handle].ShortAddress.v[0];  //devices short address
                                    TxBuffer[30]=networkTable[handle].ShortAddress.v[1];  //devices short address
                                    TxData = 31;
                                    #ifdef SUPPORT_SECURITY
                                    	networkTable[handle].IncomingFrameCounter.Val = 0;
                                    	SendTxBuffer(TRUE, FALSE);
                                    #else
                                        SendTxBuffer(TRUE);
                                    #endif
                                }
                            }
                        }
                        break;
                    #endif
                    #if defined(I_AM_COORDINATOR_CAPABLE)
                    case MAC_COMMAND_BEACON_REQUEST:
                        //if we are a member of a network
                        if(MiWiStateMachine.memberOfNetwork)
                        {
                            //send out a beacon as long as we are not
                            //currently acting as an FFD end device
                            if(role != ROLE_FFD_END_DEVICE)
                            {
                                TxBuffer[0]=0x00;   //frame control - beacon
                                TxBuffer[1]=0x80;   //addressing mode - source only
                                TxBuffer[2]=IEEESeqNum++;   //seq number 
                                TxBuffer[3]=myPANID.v[0];   //PANID LSB
                                TxBuffer[4]=myPANID.v[1];   //PANID MSB
                                TxBuffer[5]=myShortAddress.v[0];   //short address LSB
                                TxBuffer[6]=myShortAddress.v[1];   //short address MSB
                                TxBuffer[7]=0xFF;   //superframe specification (BO = 0xF, SO = 0xF)
                                TxBuffer[8]=0x0F;   //superframe capabilities
                                if(role == ROLE_PAN_COORDINATOR)
                                {
                                    TxBuffer[8] |= 0x40;
                                }
                                if(findNextNetworkEntry() != 0xFF)
                                {
                                    TxBuffer[8] |= 0x80;
                                }
                                TxBuffer[9]=0x00;   //GTS
                                TxBuffer[10]=0x00;  //Pending addresses
                                TxBuffer[11]=0x4D;  //Protocol ID
                                TxBuffer[12]=0x10;  //Version Number
                                TxBuffer[13]=knownCoordinators;
    
                                TxData = 14;
                                #ifdef SUPPORT_SECURITY
                                	SendTxBuffer(FALSE, FALSE);
                                #else
                                    SendTxBuffer(FALSE);
                                #endif
                            }
                        }
                        break;
                    #endif
                    case MAC_COMMAND_COORDINATOR_REALIGNMENT:
                        //if we get a coorinator realignment
                        if(MiWiStateMachine.attemptingToRejoin)
                        {
                            //ConsolePutROMString((ROM char*)"Coordinator realignment\r\n");
                            tempLongAddress[0] = RxBuffer[i-8];
                            tempLongAddress[1] = RxBuffer[i-7];
                            tempLongAddress[2] = RxBuffer[i-6];
                            tempLongAddress[3] = RxBuffer[i-5];
                            tempLongAddress[4] = RxBuffer[i-4];
                            tempLongAddress[5] = RxBuffer[i-3];
                            tempLongAddress[6] = RxBuffer[i-2];
                            tempLongAddress[7] = RxBuffer[i-1];
                            
                            i++;
                            tempPANID.v[0] = RxBuffer[i++];
                            tempPANID.v[1] = RxBuffer[i++];
                            tempShortAddress.v[0] = RxBuffer[i++];
                            tempShortAddress.v[1] = RxBuffer[i++];
                            
                            tempNodeStatus.Val = 0xEF;
                            
                            //save that devices information in the network table
                            myParent = AddNodeToNetworkTable();
                                                           
                            if(myParent != 0xFF)
                            {
                                myPANID.Val = tempPANID.Val;
                                MiWiStateMachine.attemptingToRejoin = 0;
                                MiWiStateMachine.memberOfNetwork = 1;
                                                            
                                i++;        //skip channel -> obviously this one
                                myShortAddress.v[0] = RxBuffer[i++];
                                myShortAddress.v[1] = RxBuffer[i++];
                                
                                PHYSetShortRAMAddr(WRITE_SADRL,myShortAddress.v[0]);
                                PHYSetShortRAMAddr(WRITE_SADRH,myShortAddress.v[1]);
                                PHYSetShortRAMAddr(WRITE_PANIDL,myPANID.v[0]);
                                PHYSetShortRAMAddr(WRITE_PANIDH,myPANID.v[1]);
                                
                                #if defined(SUPPORT_SECURITY)
    						        networkTable[myParent].IncomingFrameCounter.Val = 0;
    					        #endif
                            }   
                        }                     
                        
                        break;
                    //case GTS_REQUEST not serviced beacause non-beacon network only
                    default:
                        break;
                  }
                break;
            //case FRAME_TYPE_ACK handled by the MRF24J40
//            default:
//                break;
            #endif //P2P_ONLY
        }
        if(MiWiStateMachine.RxHasUserData == 0)
        {
DiscardPacketAndFinish:
            DiscardPacket();
        }
    }   

    //get the time difference between the start of the last function and now
    //used in the network discovery and join functions
    tickCurrentTime = TickGet();
    tickDifference.Val = TickGetDiff(tickCurrentTime,startTick);
    
    #if !defined(P2P_ONLY)
    //if we are looking to join or rejoin a network
    if(MiWiStateMachine.searchingForNetwork || MiWiStateMachine.attemptingToRejoin)
    {       
        //if we haven't searched all of the channels yet
        if(currentSearchChannel<AVAILABLE_CHANNELS_SIZE)    
        {
            //if this channel has timed out
            if(tickDifference.Val > NETWORK_DISCOVERY_TIMEOUT)
            {
                //start the next channel
                startTick = TickGet();
                //set to the next channel
                SetChannel(availableChannels[currentSearchChannel++]);
                //mark as not having sent the beacon request yet
                MiWiStateMachine.beaconRequestSent = 0;
            }
        }
        else
        {
            //if the network discovery has timed out
            if(tickDifference.Val > NETWORK_DISCOVERY_TIMEOUT)
            {
                //mark the search as complete
                MiWiStateMachine.searchingForNetwork = 0;
                MiWiStateMachine.attemptingToRejoin = 0;
                MiWiStateMachine.beaconRequestSent = 1;
            }
        }
        
        //if we are searching for a network
        if(MiWiStateMachine.searchingForNetwork)
        {
            //and a beacon request has not been sent yet
            if(MiWiStateMachine.beaconRequestSent == 0)
            {
                //send a beacon request
                TxBuffer[0] = 0x03;
                TxBuffer[1] = 0x08;
                TxBuffer[2] = IEEESeqNum++;     //sequence number
                TxBuffer[3] = 0xFF;
                TxBuffer[4] = 0xFF;
                TxBuffer[5] = 0xFF;
                TxBuffer[6] = 0xFF;
                TxBuffer[7] = 0x07;
                TxData = 8;
                #ifdef SUPPORT_SECURITY
                	SendTxBuffer(FALSE, FALSE);
                #else
                    SendTxBuffer(FALSE);
                #endif
                
                MiWiStateMachine.beaconRequestSent = 1;
            }
        }
        
        //if we are attempting to rejoin a network
        if(MiWiStateMachine.attemptingToRejoin)
        {
            //and we haven't sent out a orphan notification yet
            if(MiWiStateMachine.beaconRequestSent == 0)
            {
                TxBuffer[0]=0x03;   //frame control - beacon
                TxBuffer[1]=0xC8;   //addressing mode - source only
                TxBuffer[2]=IEEESeqNum++;   //seq number 
                TxBuffer[3]=0xFF;   //PANID LSB
                TxBuffer[4]=0xFF;   //PANID MSB
                TxBuffer[5]=0xFF;   //short address LSB
                TxBuffer[6]=0xFF;   //short address MSB
                TxBuffer[7]=0xFF;   //
                TxBuffer[8]=0xFF;   //
                TxBuffer[9]=myLongAddress[0];   //
                TxBuffer[10]=myLongAddress[1];  //
                TxBuffer[11]=myLongAddress[2];  //
                TxBuffer[12]=myLongAddress[3];  //
                TxBuffer[13]=myLongAddress[4];  //
                TxBuffer[14]=myLongAddress[5];  //
                TxBuffer[15]=myLongAddress[6];  //
                TxBuffer[16]=myLongAddress[7];  //
                TxBuffer[17]=0x06;  //
                TxData = 18;
                #ifdef SUPPORT_SECURITY
                	SendTxBuffer(FALSE, FALSE);
                #else
                    SendTxBuffer(FALSE);
                #endif
                
                MiWiStateMachine.beaconRequestSent = 1;
            }
        }
    }   
    else if(MiWiStateMachine.attemptingToJoin == 1)
    {
        //otherwise if we are trying to join a network
        //and we have waited long enough for a response
        if(tickDifference.Val > SYMBOLS_TO_TICKS(aResponseWaitTime))
        {
            //Waited long enough for a response
            if(MiWiStateMachine.dataRequestSent == 0)
            { 
                //send a data request to get the association response
                MiWiStateMachine.dataRequestSent = 1;
                TxBuffer[0] = 0x63;
                TxBuffer[1] = 0xC8;
                TxBuffer[2] = IEEESeqNum++;     //sequence number
                TxBuffer[3] = networkTable[myParent].PANID.v[0];    //dest PANID
                TxBuffer[4] = networkTable[myParent].PANID.v[1];    //dest PANID
                TxBuffer[5] = networkTable[myParent].ShortAddress.v[0]; //dest short address
                TxBuffer[6] = networkTable[myParent].ShortAddress.v[1]; //dest short address
                TxBuffer[7] = myLongAddress[0];                         //source long address
                TxBuffer[8] = myLongAddress[1];                         //source long address
                TxBuffer[9] = myLongAddress[2];                         //source long address
                TxBuffer[10] = myLongAddress[3];                         //source long address
                TxBuffer[11] = myLongAddress[4];                         //source long address
                TxBuffer[12] = myLongAddress[5];                         //source long address
                TxBuffer[13] = myLongAddress[6];                         //source long address
                TxBuffer[14] = myLongAddress[7];                         //source long address
                TxBuffer[15] = 0x04;                //data request
                TxData = 16;
                #ifdef SUPPORT_SECURITY
                	SendTxBuffer(FALSE, FALSE);
                #else
                    SendTxBuffer(FALSE);
                #endif
            }
        }
    }  
    else
    {
        //if there really isn't anything going on 
        BYTE i,j,k;
        
        i=0;

    #if defined(I_AM_COORDINATOR_CAPABLE)
        //clean up old indirect buffered messages        
        while(1)
        {
            j=indirectBuffer[i]+i;
            if(indirectBuffer[i] == 0)
            {
                break;
            }
            
            //if the indirect message timed out already
            tickDifference.byte.b3 = indirectBuffer[i+2];
            tickDifference.byte.b2 = indirectBuffer[i+3];
            tickDifference.byte.b1 = indirectBuffer[i+4];
            tickDifference.byte.b0 = indirectBuffer[i+5];
            
            tickDifference.Val = TickGetDiff(tickCurrentTime, tickDifference);
            
            if(tickDifference.Val < INDIRECT_BUFFER_TIMEOUT)
            {
                break;
            }    
            
            ConsolePutROMString((ROM char*)"Indirect Message Timedout\r\n"); 
            
            k=i;
            while(j<INDIRECT_BUFFER_SIZE)
            {
                indirectBuffer[k++]=indirectBuffer[j++];        //move next buffer down
            } 
        }
        
    #endif //COORDINATOR_CAPABLE
    #else //P2P_ONLY
    {
    #endif
    #if defined(I_AM_RFD)
        //if we are an RFD and a member of the network
        if(MiWiStateMachine.memberOfNetwork)
        {
            if(MiWiStateMachine.dataRequestNeeded)
            {
                //and we need to send a data request
                if(MiWiStateMachine.RxHasUserData == 0)
                {                                                
                    //need to send a data request to my parent
                    TxBuffer[TxData++] = 0x63;
                    TxBuffer[TxData++] = 0x88;
                    TxBuffer[TxData++] = IEEESeqNum++;      //sequence number
                    TxBuffer[TxData++] = myPANID.v[0];
                    TxBuffer[TxData++] = myPANID.v[1];
                    TxBuffer[TxData++] = networkTable[myParent].ShortAddress.v[0];
                    TxBuffer[TxData++] = networkTable[myParent].ShortAddress.v[1];
                    TxBuffer[TxData++] = myShortAddress.v[0];
                    TxBuffer[TxData++] = myShortAddress.v[1];
                    TxBuffer[TxData++] = 0x04;
                    #ifdef SUPPORT_SECURITY
                    	SendTxBuffer(TRUE, FALSE);
                    #else
                        SendTxBuffer(TRUE);
                    #endif
                    
                    MiWiStateMachine.dataRequestNeeded = 0;
                    MiWiStateMachine.dataRequestComplete = 0;
        
                    startTick = TickGet();
                }
            } 
            else if(MiWiStateMachine.dataRequestComplete == 0)
            {
                if(tickDifference.Val > RFD_DATA_WAIT)
                {
                    MiWiStateMachine.dataRequestComplete = 1;
                }
            }
        }
    #endif
        
        //clean up an old socket if one still exists
        if(openSocketInfo.status.bits.requestIsOpen)
        {
            if(openSocketInfo.status.bits.matchFound)
            {  
                if(openSocketInfo.status.bits.type == CLUSTER_SOCKET)
                {              
                    #if defined(I_AM_COORDINATOR_CAPABLE)
                    BYTE j;

                    if(ClearToSend())
                    {             
                        ConsolePutROMString((ROM char*)"sending out second response\r\n");

                        FlushTx();
                    
                        TxPayLoad();
                        TxBuffer[TxData++] = MIWI_STACK_REPORT_TYPE;      //Report Type
                        TxBuffer[TxData++] = OPEN_CLUSTER_SOCKET_RESPONSE;      //Report ID

                    
                        //copy the long and short address from the Rx Buffer
                        for(j=0;j<8;j++)
                        {
                            TxBuffer[TxData++] = openSocketInfo.LongAddress1[j]; 
                        }
                    
                        TxBuffer[TxData++] = openSocketInfo.ShortAddress1.v[0];
                        TxBuffer[TxData++] = openSocketInfo.ShortAddress1.v[1];
    
                        SendReportByShortAddress(myPANID,openSocketInfo.ShortAddress2, FALSE);
                        openSocketInfo.status.bits.requestIsOpen = 0;
                    }
                    #endif
                }
    
                //openSocketInfo.status.bits.requestIsOpen = 0;
            }
            else
            {
                tickDifference.Val = TickGetDiff(tickCurrentTime, openSocketInfo.socketStart);
                
                if(tickDifference.Val > OPEN_CLUSTER_SOCKET_TIMEOUT)
                {
                    //invalidate the expired socket request
                    openSocketInfo.status.bits.requestIsOpen = 0;
    
                    //ConsolePutROMString((ROM char*)"Socket request timeout\r\n");
                }
            }
        }
    }
}

/*********************************************************************
 * Function:        BYTE SendReportByShortAddress(WORD_VAL PANID, WORD_VAL ShortAddress)
 *
 * PreCondition:    TxBuffer is loaded with the packet that needs to be
 *                  sent
 *
 * Input:           WORD_VAL PANID - the PAN of the device that you 
 *                      want to send the packet to
 *                  WORD_VAL ShortAddress - the short address of the device
 *                      that you want to send the packet to.
 * Output:          BYTE the MiWi handle for this packet
 *
 * Side Effects:    The TxBuffer gets transmited and the Tx pointer 
 *                  gets reset
 *
 * Overview:        Load the TxBuffer with the data that needs to be 
 *                  sent.  Call this function with the address of the
 *                  final destination.  This function will take care
 *                  of the routing as long as a valid path is known.
 *                  If a valid path isn't known then the user will need
 *                  to initiate a EUI discovery to find a route to the 
 *                  destination device.
 ********************************************************************/
#if !defined(P2P_ONLY)
BYTE SendReportByShortAddress(WORD_VAL PANID, WORD_VAL ShortAddress, BOOL forwardPacket)
{
    BYTE handle;
    
    tempPANID.Val = PANID.Val;
    tempShortAddress.Val = ShortAddress.Val;
    
    handle = SearchForShortAddress(NEIGHBOR);
    
    if(handle != 0xFF)
    {
        return SendReportByHandle(handle,forwardPacket);
    }
    else
    {
        #if !defined(I_AM_COORDINATOR_CAPABLE)
            if(MiWiSeqNum == 0xFF)
            {
                MiWiSeqNum++;
            }
            //If I am an RFD, then send it to my parent
            //I don't have direct comm with this person so send it to my parent      
            TxBuffer[0] = MAX_HOPS;				//number of hops
            TxBuffer[1] = 0x02;					//Frame Control
            TxBuffer[2] = PANID.v[0];			
            TxBuffer[3] = PANID.v[1];
            TxBuffer[4] = ShortAddress.v[0];
            TxBuffer[5] = ShortAddress.v[1];
            TxBuffer[6] = myPANID.v[0];			//source PANID LSB
            TxBuffer[7] = myPANID.v[1];			//source PANID MSB
            TxBuffer[8] = myShortAddress.v[0];	//source address LSB
            TxBuffer[9] = myShortAddress.v[1];	//source address MSB
            TxBuffer[10] = MiWiSeqNum;			//sequence number
            
            TxBuffer[TxHeader--] = myShortAddress.v[1];      //source address MSB
            TxBuffer[TxHeader--] = myShortAddress.v[0];      //source address LSB
            TxBuffer[TxHeader--] = networkTable[myParent].ShortAddress.v[1];
            TxBuffer[TxHeader--] = networkTable[myParent].ShortAddress.v[0];
            TxBuffer[TxHeader--] = networkTable[myParent].PANID.v[1];
            TxBuffer[TxHeader--] = networkTable[myParent].PANID.v[0];
            TxBuffer[TxHeader--] = IEEESeqNum++;    //seq number 
            TxBuffer[TxHeader--] = 0x88;    //short dest address, short source address
            TxBuffer[TxHeader--] = 0x61;    //intra-pan bit set, ack requested
            #ifdef SUPPORT_SECURITY
        	    SendTxBuffer(TRUE, !forwardPacket);
            #else
                SendTxBuffer(TRUE);
            #endif
        #else
            //If I am a coordinator then determine the next hop for the packet
            tempPANID.Val = PANID.Val;
            tempShortAddress.v[0] = 0x00;       //see if I know this devices parent
            
            //search for devices parent
            handle = SearchForShortAddress(NEIGHBOR);
            
            //if we know the guys parent
            if(handle != 0xFF)
            {
                //if this packet was originated by this device
                if(forwardPacket == FALSE)
                {
                    //if there is room in the buffer
                    if((TxData + 11) < TX_BUFFER_SIZE)
                    {
                        if(MiWiSeqNum == 0xFF)
                        {
                            MiWiSeqNum++;
                        }                        
                        //I don't have direct comm with this person so send it to their parent
                        TxBuffer[0] = MAX_HOPS;				//number of hops
                        TxBuffer[1] = 0x02;					//Frame Control
                        TxBuffer[2] = PANID.v[0];
                        TxBuffer[3] = PANID.v[1];
                        TxBuffer[4] = ShortAddress.v[0];
                        TxBuffer[5] = ShortAddress.v[1];
                        TxBuffer[6] = myPANID.v[0];			//source PANID LSB
                        TxBuffer[7] = myPANID.v[1];			//source PANID MSB
                        TxBuffer[8] = myShortAddress.v[0];	//source address LSB
                        TxBuffer[9] = myShortAddress.v[1];	//source address MSB
                        TxBuffer[10] = MiWiSeqNum;			//sequence number
                        forwardPacket = TRUE;
                    }
                    else
                    {
                       //not enough room in the buffer, erase the packet
                       TxData = 0;
                       TxHeader = TX_BUFFER_SIZE - 1;
                       
                       return 0xFF;
                    }
                }
    
                return SendReportByHandle(handle,forwardPacket);
            }  
            else
            {
                BYTE parentNode = tempShortAddress.v[1] & 0x03;
                if(RoutingTable[parentNode] != 0)
                {
                    if(RoutingTable[parentNode] &= (1<<parentNode))
                    {
                        //If the guys parent is in radio range but not in my network table
                        ConsolePutROMString((ROM char*)"routing the packet -- directly to parent\r\n");       
                        //if this packet was originated by this device
                        if(forwardPacket == FALSE)
                        {
                            //if there is room in the buffer
                            if((TxData + 11) < TX_BUFFER_SIZE)
                            {
                                if(MiWiSeqNum == 0xFF)
                                {
                                    MiWiSeqNum++;
                                }
                                //I don't have direct comm with this person so send it to their parent
                                TxBuffer[0] = MAX_HOPS;				//number of hops
                                TxBuffer[1] = 0x02;					//Frame Control
                                TxBuffer[2] = PANID.v[0];			
                                TxBuffer[3] = PANID.v[1];
                                TxBuffer[4] = ShortAddress.v[0];
                                TxBuffer[5] = ShortAddress.v[1];
                                TxBuffer[6] = myPANID.v[0];			//source PANID LSB
                                TxBuffer[7] = myPANID.v[1];			//source PANID MSB
                                TxBuffer[8] = myShortAddress.v[0];	//source address LSB
                                TxBuffer[9] = myShortAddress.v[1];	//source address MSB
                                TxBuffer[10] = MiWiSeqNum;			//sequence number
                            }
                            else
                            {
                               //not enough room in the buffer, erase the packet
                               TxData = 0;
                               TxHeader = TX_BUFFER_SIZE - 1;
                               
                               return 0xFF;
                            }
                        }
                        if((TxData + 11) < TX_BUFFER_SIZE)
                        {
                            TxBuffer[TxHeader--] = myShortAddress.v[1];
                            TxBuffer[TxHeader--] = myShortAddress.v[0]; 
                            TxBuffer[TxHeader--] = tempShortAddress.v[1];
                            TxBuffer[TxHeader--] = 0x00;
                            TxBuffer[TxHeader--] = myPANID.v[1];
                            TxBuffer[TxHeader--] = myPANID.v[0];
                            TxBuffer[TxHeader--] = IEEESeqNum++;    //seq number 
                            TxBuffer[TxHeader--] = 0x88;    //short dest address, short source address
                            TxBuffer[TxHeader--] = 0x61;    //intra-pan bit set, ack requested
                            #ifdef SUPPORT_SECURITY
                            	SendTxBuffer(TRUE, !forwardPacket);
                            #else
                                SendTxBuffer(TRUE);
                            #endif
                        }
                        else
                        {
                            TxData = 0;
                            TxHeader = TX_BUFFER_SIZE - 1;
                            return 0xFF;
                        }
                    }
                    else
                    {
                        BYTE randomNumber = 1<<(TMRL & 0x03); //not really random but good enough
                        BYTE j;
                                                
                        for(j=0;j<8;j++)
                        {
                            if(randomNumber == 0)
                            {
                                randomNumber = 1;
                            }                  
                            
                            if(randomNumber & RoutingTable[parentNode])
                            {
                                ConsolePutROMString((ROM char*)"sending it through a sibling\r\n");       
                                break;
                            }
                                  
                            randomNumber <<= 1;
                        }
                    }
                }
                else
                {                
                    ConsolePutROMString((ROM char*)"no route\r\n");
                    DumpNetworkTable();
                    
                    //clear the buffer cause we couldn't send the packet
                    TxData = 0;
                    TxHeader = TX_BUFFER_SIZE - 1;
                
                    return 0xFF;          
                }
            }
        #endif
    }
    return MiWiSeqNum++;
}
#endif

/*********************************************************************
 * Function:        BYTE SendReportByLongAddress(BYTE *pLongAddress)
 *
 * PreCondition:    TxBuffer is loaded with the packet that needs to be
 *                  sent
 *
 * Input:           BYTE *pLongAddress
 * Output:          BYTE the MiWi application handle for this packet
 *
 * Side Effects:    The TxBuffer gets transmited and the Tx pointer 
 *                  gets reset
 *
 * Overview:        Load the TxBuffer with the data that needs to be 
 *                  sent.  Call this function with the address of the
 *                  final destination.  This function will take care
 *                  of the routing as long as a valid path is known.
 *                  If a valid path isn't known then the user will need
 *                  to initiate a EUI discovery to find a route to the 
 *                  destination device.
 ********************************************************************/
BYTE SendReportByLongAddress(BYTE *pLongAddress)
{
    BYTE handle;
    BYTE toReturn;
    
    toReturn = 0xFF;
    
    //copy the long address
    tempLongAddress[0] = pLongAddress[0];
    tempLongAddress[1] = pLongAddress[1];
    tempLongAddress[2] = pLongAddress[2];
    tempLongAddress[3] = pLongAddress[3];
    tempLongAddress[4] = pLongAddress[4];
    tempLongAddress[5] = pLongAddress[5];
    tempLongAddress[6] = pLongAddress[6];
    tempLongAddress[7] = pLongAddress[7];
    
    #if !defined(P2P_ONLY)
    
    //look for it in the network table
    handle = SearchForLongAddress();
    
    if(handle != 0xFF)
    {
        //if we know the person then send the packet
        //via the network handle that you know
        //this gives us the most routing options
        return SendReportByHandle(handle,FALSE);
    }
    else
    #endif
    {
        if(MiWiSeqNum == 0xFF)
        {
            MiWiSeqNum++;
        }   

        TxBuffer[0] = 0x00;
        TxBuffer[1] = 0x02;
        TxBuffer[2] = 0xFF;
        TxBuffer[3] = 0xFF;
        TxBuffer[4] = 0xFF;
        TxBuffer[5] = 0xFF;
        TxBuffer[6] = 0xFF;
        TxBuffer[7] = 0xFF;
        TxBuffer[8] = 0xFF;
        TxBuffer[9] = 0xFF;
        TxBuffer[10] = MiWiSeqNum;
        
        toReturn = MiWiSeqNum++;
         
        TxBuffer[TxHeader--] = myLongAddress[7];  //MSB source long address 
        TxBuffer[TxHeader--] = myLongAddress[6];  //source long address 
        TxBuffer[TxHeader--] = myLongAddress[5];  //source long address 
        TxBuffer[TxHeader--] = myLongAddress[4];  //source long address 
        TxBuffer[TxHeader--] = myLongAddress[3];  //source long address 
        TxBuffer[TxHeader--] = myLongAddress[2];  //source long address 
        TxBuffer[TxHeader--] = myLongAddress[1];  //source long address 
        TxBuffer[TxHeader--] = myLongAddress[0];  //LSB source long address 
        
        TxBuffer[TxHeader--] = 0xFF;  //MSB source PANID
        TxBuffer[TxHeader--] = 0xFF;  //source PANID
        
        TxBuffer[TxHeader--] = tempLongAddress[7];  //MSB dest long address 
        TxBuffer[TxHeader--] = tempLongAddress[6];  //dest long address 
        TxBuffer[TxHeader--] = tempLongAddress[5];  //dest long address 
        TxBuffer[TxHeader--] = tempLongAddress[4];  //dest long address 
        TxBuffer[TxHeader--] = tempLongAddress[3];  //dest long address 
        TxBuffer[TxHeader--] = tempLongAddress[2];  //dest long address 
        TxBuffer[TxHeader--] = tempLongAddress[1];  //dest long address 
        TxBuffer[TxHeader--] = tempLongAddress[0];  //LSB dest long address 
        
        TxBuffer[TxHeader--] = 0xFF;  //MSB dest PANID
        TxBuffer[TxHeader--] = 0xFF;  //dest PANID
        
        TxBuffer[TxHeader--] = IEEESeqNum++;  //seq num
        
        TxBuffer[TxHeader--] = 0xCC;  //frame control -  both long addresses
        
        TxBuffer[TxHeader--] = 0x21;  //frame control -  command packet

    	#ifdef SUPPORT_SECURITY
	    	SendTxBuffer(FALSE, TRUE);
    	#else
            SendTxBuffer(FALSE);
        #endif
    }    
    return toReturn;
}

/*********************************************************************
 * Function:        BYTE SendReportByHandle(BYTE handle)
 *
 * PreCondition:    TxBuffer is loaded with the packet that needs to be
 *                  sent
 *
 * Input:           BYTE handle - the handle of the device that you are
 *                      sending the packet to
 *                  BOOL forwardPacket - TRUE if this packet is being
 *                      forwarded from a different source.  FALSE if
 *                      this device is the originator of the packet
 *
 * Output:          BYTE the MiWi handle for this packet
 *
 * Side Effects:    The TxBuffer gets transmited and the Tx pointer 
 *                  gets reset
 *
 * Overview:        Load the TxBuffer with the data that needs to be 
 *                  sent.  Call this function with the address of the
 *                  final destination.  This function will take care
 *                  of the routing as long as a valid path is known.
 *                  If a valid path isn't known then the user will need
 *                  to initiate a EUI discovery to find a route to the 
 *                  destination device.
 ********************************************************************/
#if !defined(P2P_ONLY)
BYTE SendReportByHandle(BYTE handle, BOOL forwardPacket)
{
    BYTE i;
        
    //if this packet originated from this device and isn't being
    //forwarded on behalf of another device
    if(forwardPacket == FALSE)
    {
        //if there is room in the buffer for the packet
        if((TxData + 11) < TX_BUFFER_SIZE)
        {
            if(MiWiSeqNum == 0xFF)
            {
                MiWiSeqNum++;
            }
            
            if(networkStatus[handle].bits.P2PConnection && (networkStatus[handle].bits.shortAddressValid == 0))
            {
                //I don't have direct comm with this person so send it to my parent
                TxBuffer[0] = 0;			//number of hops - don't need any more hops than one, P2P
                TxBuffer[1] = 0x02;			//Frame Control
                TxBuffer[2] = 0xFF;
                TxBuffer[3] = 0xFF;
                TxBuffer[4] = 0xFF;
                TxBuffer[5] = 0xFF;
                TxBuffer[6] = 0xFF;
                TxBuffer[7] = 0xFF;			//source PANID MSB
                TxBuffer[8] = 0xFF;			//source address LSB
                TxBuffer[9] = 0xFF;			//source address MSB
                TxBuffer[10] = MiWiSeqNum;	//sequence number
            }
            else
            {
                //I don't have direct comm with this person so send it to my parent
                TxBuffer[0] = MAX_HOPS;									//number of hops
                TxBuffer[1] = 0x02;										//Frame Control
                TxBuffer[2] = networkTable[handle].PANID.v[0];
                TxBuffer[3] = networkTable[handle].PANID.v[1];
                TxBuffer[4] = networkTable[handle].ShortAddress.v[0];
                TxBuffer[5] = networkTable[handle].ShortAddress.v[1];
                TxBuffer[6] = myPANID.v[0];								//source PANID LSB
                TxBuffer[7] = myPANID.v[1];								//source PANID MSB
                TxBuffer[8] = myShortAddress.v[0];						//source address LSB
                TxBuffer[9] = myShortAddress.v[1];						//source address MSB
                TxBuffer[10] = MiWiSeqNum;								//sequence number
            }
        }
        else
        {
           //not enough room in the buffer, erase the packet
           TxData = 0;
           TxHeader = TX_BUFFER_SIZE - 1;
           return 0xFF;
        }
    }
    
    if(networkStatus[handle].bits.isValid)
    {
        #if defined(I_AM_COORDINATOR_CAPABLE)
            if(networkStatus[handle].bits.shortAddressValid)
            {
                //if the short address is valid and the destination is a coordinator
                if(networkTable[handle].ShortAddress.v[0] == 0x00)
                {
                    //if I learned of this coordinator through a beacon
                    if(knownCoordinators & (1<<(networkTable[handle].ShortAddress.v[1]&0x03)))
                    {
                        //indicate that I can talk to this guy directly
                        networkStatus[handle].bits.directConnection = 1;
                    }
                }
            }
        #endif
        
        if(networkStatus[handle].bits.directConnection)
        {
            if(networkStatus[handle].bits.P2PConnection)
            {               
                TxBuffer[TxHeader--] = myLongAddress[7];  //MSB source long address 
                TxBuffer[TxHeader--] = myLongAddress[6];  //source long address 
                TxBuffer[TxHeader--] = myLongAddress[5];  //source long address 
                TxBuffer[TxHeader--] = myLongAddress[4];  //source long address 
                TxBuffer[TxHeader--] = myLongAddress[3];  //source long address 
                TxBuffer[TxHeader--] = myLongAddress[2];  //source long address 
                TxBuffer[TxHeader--] = myLongAddress[1];  //source long address 
                TxBuffer[TxHeader--] = myLongAddress[0];  //LSB source long address 
                
                TxBuffer[TxHeader--] = 0xFF;  //MSB source PANID
                TxBuffer[TxHeader--] = 0xFF;  //source PANID
                
                TxBuffer[TxHeader--] = networkTable[handle].info.LongAddress[7];  //MSB dest long address 
                TxBuffer[TxHeader--] = networkTable[handle].info.LongAddress[6];  //dest long address 
                TxBuffer[TxHeader--] = networkTable[handle].info.LongAddress[5];  //dest long address 
                TxBuffer[TxHeader--] = networkTable[handle].info.LongAddress[4];  //dest long address 
                TxBuffer[TxHeader--] = networkTable[handle].info.LongAddress[3];  //dest long address 
                TxBuffer[TxHeader--] = networkTable[handle].info.LongAddress[2];  //dest long address 
                TxBuffer[TxHeader--] = networkTable[handle].info.LongAddress[1];  //dest long address 
                TxBuffer[TxHeader--] = networkTable[handle].info.LongAddress[0];  //LSB dest long address 
                
                TxBuffer[TxHeader--] = 0xFF;  //MSB dest PANID
                TxBuffer[TxHeader--] = 0xFF;  //dest PANID
                
                TxBuffer[TxHeader--] = IEEESeqNum++;  //seq num
                
                TxBuffer[TxHeader--] = 0xCC;  //frame control -  both long addresses

                TxBuffer[TxHeader--] = 0x21;  //frame control -  command packet
            	#ifdef SUPPORT_SECURITY
            		SendTxBuffer(FALSE, !forwardPacket);
            	#else
                    SendTxBuffer(FALSE);
                #endif
            }
            else
            {
                if(networkStatus[handle].bits.RXOnWhenIdle)
                {
                   if(networkStatus[handle].bits.shortAddressValid)
                   {                 
                        //if there is enough room in the buffer to transmit the packet
                        if((TxData + 9) < TX_BUFFER_SIZE)
                        {                    
                            TxBuffer[TxHeader--] = myShortAddress.v[1];
                            TxBuffer[TxHeader--] = myShortAddress.v[0]; 
                            TxBuffer[TxHeader--] = networkTable[handle].ShortAddress.v[1];
                            TxBuffer[TxHeader--] = networkTable[handle].ShortAddress.v[0];
                            TxBuffer[TxHeader--] = networkTable[handle].PANID.v[1];
                            TxBuffer[TxHeader--] = networkTable[handle].PANID.v[0];
                            TxBuffer[TxHeader--] = IEEESeqNum++;    //seq number 
                            TxBuffer[TxHeader--] = 0x88;    //short dest address, short source address
                            TxBuffer[TxHeader--] = 0x61;    //intra-pan bit set, ack requested
                           	#ifdef SUPPORT_SECURITY
                           		SendTxBuffer(TRUE, !forwardPacket);
                           	#else
                                SendTxBuffer(TRUE);
                            #endif
                       }
                       else
                       {
                           //not enough room in the buffer, erase the packet
                           TxData = 0;
                           TxHeader = TX_BUFFER_SIZE - 1;
                           return 0xFF;
                       }
                   }
                   else
                   {
                       return 0xFF;
                   }
                }
                else
                {
                    if(networkStatus[handle].bits.shortAddressValid)
                    {                    
                        #if defined(I_AM_COORDINATOR_CAPABLE)
                            SendIndirectMessage(handle);
                        #endif
                    }
                    else
                    {
                       return 0xFF;
                    }
                }
            }
        }
        else
        {
            #if defined(I_AM_COORDINATOR_CAPABLE)
            if(role != ROLE_PAN_COORDINATOR)
            {
                TxBuffer[TxHeader--] = myShortAddress.v[1];
                TxBuffer[TxHeader--] = myShortAddress.v[0]; 
                TxBuffer[TxHeader--] = networkTable[myParent].ShortAddress.v[1];
                TxBuffer[TxHeader--] = networkTable[myParent].ShortAddress.v[0];
                TxBuffer[TxHeader--] = networkTable[myParent].PANID.v[1];
                TxBuffer[TxHeader--] = networkTable[myParent].PANID.v[0];
                TxBuffer[TxHeader--] = IEEESeqNum++;    //seq number 
                TxBuffer[TxHeader--] = 0x88;    //short dest address, short source address
                TxBuffer[TxHeader--] = 0x61;    //intra-pan bit set, ack requested
                #ifdef SUPPORT_SECURITY
                	SendTxBuffer(TRUE, !forwardPacket);
                #else
                    SendTxBuffer(TRUE);
                #endif
            }
            else
            {
                //otherwise I don't know have a parent because I am the PAN coordinator.
                //in this case I know that I know his parent so send it to them
                TxBuffer[TxHeader--] = myShortAddress.v[1];
                TxBuffer[TxHeader--] = myShortAddress.v[0]; 
                TxBuffer[TxHeader--] = networkTable[handle].ShortAddress.v[1];  //keep the upper byte the same
                TxBuffer[TxHeader--] = 0x00;    //send to the parent of the device (aka zero out the lower byte)
                TxBuffer[TxHeader--] = myPANID.v[1];
                TxBuffer[TxHeader--] = myPANID.v[0];
                TxBuffer[TxHeader--] = IEEESeqNum++;    //seq number 
                TxBuffer[TxHeader--] = 0x88;    //short dest address, short source address
                TxBuffer[TxHeader--] = 0x61;    //intra-pan bit set, ack requested
                #ifdef SUPPORT_SECURITY
                	SendTxBuffer(TRUE, !forwardPacket);
                #else
                    SendTxBuffer(TRUE);
                #endif
            }
            #else
                //if I am not a coordinator and I don't have a direct connection then try send it to my parent
                TxBuffer[TxHeader--] = myShortAddress.v[1];
                TxBuffer[TxHeader--] = myShortAddress.v[0]; 
                TxBuffer[TxHeader--] = networkTable[myParent].ShortAddress.v[1];
                TxBuffer[TxHeader--] = networkTable[myParent].ShortAddress.v[0];
                TxBuffer[TxHeader--] = networkTable[myParent].PANID.v[1];
                TxBuffer[TxHeader--] = networkTable[myParent].PANID.v[0];
                TxBuffer[TxHeader--] = IEEESeqNum++;    //seq number 
                TxBuffer[TxHeader--] = 0x88;    //short dest address, short source address
                TxBuffer[TxHeader--] = 0x61;    //intra-pan bit set, ack requested
                #ifdef SUPPORT_SECURITY
                	SendTxBuffer(TRUE, !forwardPacket);
                #else
                    SendTxBuffer(TRUE);
                #endif
            #endif
        }
    }
    else
    {
        return 0xFF;
    }

    return MiWiSeqNum++;
}
#endif

/*********************************************************************
 * Function:        void SendIndirectPacket(BYTE handle,BYTE dataRequestType)
 *
 * PreCondition:    TxBuffer is loaded with the packet that needs to be
 *                  sent
 *
 * Input:           BYTE handle - the entry in the neighbor table that
 *                      applies to this device
 *                  BYTE dataRequestType - this value needs to be either
 *                      DATA_REQUEST_ASSOCIATION_RESPONSE, OR
 *                      DATA_REQUEST_SHORT_ADDRESS.
 * Output:          None
 *
 * Side Effects:    The TxBuffer gets transmited and the Tx pointer 
 *                  gets reset
 *
 * Overview:        This function sends the packet in the TxBuffer 
 *                  indirectly.  This function is only called by the
 *                  stack and shouldn't be called by the user.  This 
 *                  function is only available to coordinators
 ********************************************************************/
#if defined(I_AM_COORDINATOR_CAPABLE)
void SendIndirectPacket(BYTE handle,BYTE dataRequestType)
{
    BYTE i,j,k;
    
    i=0;
    
    //if the handle isn't valid then we can't send it
    if(handle == 0xFF)
    {
        return;
    }
    
    while(1)
    {
        //if we have reached the end of the buffer
        if(i>=INDIRECT_BUFFER_SIZE)
        {
            break;
        }
        
        //point to the next buffer
        j=indirectBuffer[i]+i;
        
        //if this buffer is empty
        if(indirectBuffer[i] == 0)
        {
            break;
        }
        
        //if this buffer matches the device I wish to send it to
        if(indirectBuffer[i+1] == handle)
        {
            //load up the packet and send it
            for(k=i+6;k<j;k++)
            {
                TxBuffer[TxData++]=indirectBuffer[k];
            }
            if((dataRequestType == DATA_REQUEST_ASSOCIATION_RESPONSE) || (networkStatus[handle].bits.P2PConnection == 1))
            {
                TxBuffer[TxHeader--] = myLongAddress[7];  //MSB source long address 
                TxBuffer[TxHeader--] = myLongAddress[6];  //source long address 
                TxBuffer[TxHeader--] = myLongAddress[5];  //source long address 
                TxBuffer[TxHeader--] = myLongAddress[4];  //source long address 
                TxBuffer[TxHeader--] = myLongAddress[3];  //source long address 
                TxBuffer[TxHeader--] = myLongAddress[2];  //source long address 
                TxBuffer[TxHeader--] = myLongAddress[1];  //source long address 
                TxBuffer[TxHeader--] = myLongAddress[0];  //source long address 
                
                TxBuffer[TxHeader--] = myPANID.v[1];  //MSB source PANID
                TxBuffer[TxHeader--] = myPANID.v[0];  //source PANID
                
                TxBuffer[TxHeader--] = networkTable[handle].info.LongAddress[7];  //MSB dest long address 
                TxBuffer[TxHeader--] = networkTable[handle].info.LongAddress[6];  //dest long address 
                TxBuffer[TxHeader--] = networkTable[handle].info.LongAddress[5];  //dest long address 
                TxBuffer[TxHeader--] = networkTable[handle].info.LongAddress[4];  //dest long address 
                TxBuffer[TxHeader--] = networkTable[handle].info.LongAddress[3];  //dest long address 
                TxBuffer[TxHeader--] = networkTable[handle].info.LongAddress[2];  //dest long address 
                TxBuffer[TxHeader--] = networkTable[handle].info.LongAddress[1];  //dest long address 
                TxBuffer[TxHeader--] = networkTable[handle].info.LongAddress[0];  //dest long address 

                TxBuffer[TxHeader--] = networkTable[handle].PANID.v[1];  //MSB dest PANID
                TxBuffer[TxHeader--] = networkTable[handle].PANID.v[0];  //dest PANID
                
                TxBuffer[TxHeader--] = IEEESeqNum++;  //seq num
                
                TxBuffer[TxHeader--] = 0xCC;  //frame control -  both long addresses
            
                TxBuffer[TxHeader--] = 0x23;  //frame control -  command packet
            }
            else
            {
                TxBuffer[TxHeader--] = myShortAddress.v[1];  //MSB source short address 
                TxBuffer[TxHeader--] = myShortAddress.v[0];  //source short address 
                
                TxBuffer[TxHeader--] = networkTable[handle].ShortAddress.v[1];  //MSB dest short address 
                TxBuffer[TxHeader--] = networkTable[handle].ShortAddress.v[0];  //dest short address 
                
                TxBuffer[TxHeader--] = myPANID.v[1];  //MSB source PANID
                TxBuffer[TxHeader--] = myPANID.v[0];  //source PANID
                                
                TxBuffer[TxHeader--] = IEEESeqNum++;  //seq num
                
                TxBuffer[TxHeader--] = 0x88;  //frame control -  both short addresses

                TxBuffer[TxHeader--] = 0x61;  //frame control -  data packet

            }
    		#ifdef SUPPORT_SECURITY
	    		if( (TxBuffer[TxHeader+1] & 0x07) == 0x01 )
		    		SendTxBuffer(TRUE, TRUE);
			    else
				    SendTxBuffer(TRUE, FALSE);
		    #else
                SendTxBuffer(TRUE);   
            #endif 
        }
        else
        {
            i=j;
            continue;
        }

        while(j<INDIRECT_BUFFER_SIZE)
        {
            indirectBuffer[i++]=indirectBuffer[j++];        //move next buffer down
        } 
        return;
    }
}
#endif

/*********************************************************************
 * Function:        void DiscardPacket(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    The RxBuffer gets flushed and the MRF24J40 is allowed
 *                  to receive again.
 *
 * Overview:        This function needs to be called to discard, clear
 *                  and reset the Rx module of the MRF24J40.  This 
 *                  function needs to be called after the user is done
 *                  processing a received packet
 ********************************************************************/
void DiscardPacket(void)
{
    MiWiStateMachine.RxHasUserData = 0;
    
    //re-enable the ACKS
    RFIE = 0;
    PHYSetShortRAMAddr(WRITE_RXMCR,0x00);
    MiWiStatus.bits.RX_BUFFERED = 0;
    MiWiStatus.bits.RX_ENABLED = 1;
    RxData = 0;
    RFIE = 1;
}


/*********************************************************************
 * Function:        BYTE findNextNetworkEntry(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          BYTE 0xFF if there is no room in the network table
 *                  otherwise this function returns the index of the 
 *                  first blank entry in the table
 *
 * Side Effects:    None
 *
 * Overview:        This function is used to determine the next available
 *                  slot in the network table.  This function can be used
 *                  to manually create entries in the networkTable
 ********************************************************************/
BYTE findNextNetworkEntry(void)
{
    BYTE i;
    
    for(i=0;i<sizeof(networkTable)/sizeof(NETWORK_TABLE_ENTRY);i++)
    {
        if(networkStatus[i].bits.isValid == 0)
        {
            return i;
        }
    }
    return 0xFF;
}

/*********************************************************************
 * Function:        BOOL SendTxBuffer(BOOL AckRequested)
 *
 * PreCondition:    TxBuffer is loaded with the data to send
 *
 * Input:           BOOL AckRequested - if called with TRUE then the ACK
 *                  requested bit will be set.  If FALSE then the ACK
 *                  requested bit will be clear.
 *
 * Output:          BOOL this will indicate if the transmission was 
 *                  successful (returns TRUE) or not (returns FALSE)
 *
 * Side Effects:    The TxBuffer pointers are reset
 *
 * Overview:        This function is used to transmit a packet in the
 *                  Tx buffer.  This function is a helper function for 
 *                  the stack and is not intended for user usage.
 ********************************************************************/
#ifdef SUPPORT_SECURITY
	BOOL SendTxBuffer(BOOL AckRequested, BOOL SecurityOn)
#else	 
	BOOL SendTxBuffer(BOOL AckRequested)
#endif	
{
    while( MiWiStatus.bits.TX_BUSY )
    {
        if( RF_INT_PIN == 0 )
        {
            RFIF = 1;
        }    
    }
    
    //if(MiWiStatus.bits.TX_BUSY == 0)
    {
        BYTE i,j;
        
        #ifdef SUPPORT_SECURITY
		    if( SecurityOn )
    		{
	    		BYTE extraLen = 0;
		    	switch(mySecurityLevel)
			    {
				    case 1:
					    extraLen = 13;
    					break;
	    			case 2:
		    		case 5:
			    		extraLen = 29;
				    	break;
    				case 3:
	    			case 6:
		    			extraLen = 21;
			    		break;
				    case 4:
    				case 7:
	    				extraLen = 17;
		    			break;
				
			    }
    			if( TxData + extraLen > TxHeader )
	    		{
		    		ConsolePutROMString((ROM char *)"Packet too big to send");
			    	return FALSE;
    			}

	    		TxBuffer[1] |= 0x01;
		    	TxData -= 11;
			    if( !DataEncrypt(&(TxBuffer[11]), &TxData, &(TxBuffer[1]), 10) )
    			{
	    			//ConsolePutROMString((ROM char *)"Encryption error\r\n");
	            	return FALSE;
	            }
    			TxData += 11;
	    	}
        #endif        
        
        //write the packet length
        PHYSetLongRAMAddr(0,TX_BUFFER_SIZE-1-TxHeader);
        //write the header length
        PHYSetLongRAMAddr(1,TxData+TX_BUFFER_SIZE-1-TxHeader);

        
        i=2;
        
        //write the packet header
        while(TxHeader < (TX_BUFFER_SIZE - 1))
        {
            PHYSetLongRAMAddr(i++,TxBuffer[++TxHeader]);
        }
        TxHeader = TX_BUFFER_SIZE-1;
                
        //write the packet data
        for(j=0;j<TxData;j++)
        {
            PHYSetLongRAMAddr(i++,TxBuffer[j]);
        }
        TxData = 0;
    
        //mark the device as busy sending
        MiWiStatus.bits.TX_BUSY = 1;
        
        //if an ACK is required then indicated that
        if(AckRequested == TRUE)
        {    
            MiWiStatus.bits.TX_PENDING_ACK = 1;       
            PHYSetShortRAMAddr(WRITE_TXNMTRIG,0x05); 
        }
        else
        {
            MiWiStatus.bits.TX_PENDING_ACK = 0;
            PHYSetShortRAMAddr(WRITE_TXNMTRIG,0x01); 
        }
        return TRUE;
    }
    TxData = 0;
    TxHeader = TX_BUFFER_SIZE - 1;
    return FALSE;
}


/*********************************************************************
 * Function:        void PHYSetLongRAMAddr(WORD address, BYTE value)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           WORD address is the address of the LONG RAM address
 *                      that you want to write to
 *                  BYTE value is the value that you want to write to
 *                      that register
 *
 * Output:          None
 *
 * Side Effects:    The register value is changed
 *
 * Overview:        This function writes a value to a LONG RAM address
 ********************************************************************/
void PHYSetLongRAMAddr(WORD address, BYTE value)
{
    volatile BYTE tmpRFIE = RFIE;
    RFIE = 0;
    PHY_CS = 0;
    Nop();
    SPIPut((((BYTE)(address>>3))&0x7F)|0x80);
    SPIPut((((BYTE)(address<<5))&0xE0)|0x10);
    SPIPut(value);
    Nop();
    PHY_CS = 1;
    RFIE = tmpRFIE;
}

/*********************************************************************
 * Function:        void PHYSetShortRAMAddr(BYTE address, BYTE value)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           BYTE address is the address of the short RAM address
 *                      that you want to write to.  Should use the
 *                      WRITE_ThisAddress definition in the MRF24J40 
 *                      include file.
 *                  BYTE value is the value that you want to write to
 *                      that register
 *
 * Output:          None
 *
 * Side Effects:    The register value is changed
 *
 * Overview:        This function writes a value to a short RAM address
 ********************************************************************/
void PHYSetShortRAMAddr(BYTE address, BYTE value)
{
    volatile BYTE tmpRFIE = RFIE;
    RFIE = 0;
    PHY_CS = 0;       
    Nop();
    SPIPut(address);
    SPIPut(value);
    Nop();
    PHY_CS = 1;
    RFIE = tmpRFIE;
}


/*********************************************************************
 * Function:        void PHYGetShortRAMAddr(BYTE address, BYTE value)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           BYTE address is the address of the short RAM address
 *                      that you want to read from.  Should use the
 *                      READ_ThisAddress definition in the MRF24J40 
 *                      include file.
 *
 * Output:          BYTE the value read from the specified short register
 *
 * Side Effects:    None
 *
 * Overview:        This function reads a value from a short RAM address
 ********************************************************************/
BYTE PHYGetShortRAMAddr(BYTE address)
{
    BYTE toReturn;
    volatile BYTE tmpRFIE = RFIE;
    RFIE = 0;
    PHY_CS = 0;
    Nop();
    SPIPut(address);
    toReturn = SPIGet();
    Nop();
    PHY_CS = 1;
    RFIE = tmpRFIE;
    return toReturn;
}

/*********************************************************************
 * Function:        BYTE PHYGetLongRAMAddr(WORD address)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           WORD address is the address of the long RAM address
 *                      that you want to read from.  
 *
 * Output:          BYTE the value read from the specified Long register
 *
 * Side Effects:    None
 *
 * Overview:        This function reads a value from a long RAM address
 ********************************************************************/
BYTE PHYGetLongRAMAddr(WORD address)
{
    BYTE toReturn;
    volatile BYTE tmpRFIE = RFIE;
    RFIE = 0;
    PHY_CS = 0;
    Nop();
    SPIPut(((address>>3)&0x7F)|0x80);
    SPIPut(((address<<5)&0xE0));
    toReturn = SPIGet();
    Nop();
    PHY_CS = 1;
    RFIE = tmpRFIE;
    return toReturn;
}


/*********************************************************************
 * Function:        void DumpNetworkTable(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function dumps the contents of many tables to
 *                  the console.  This is a debugging\helper function only
 *                  and is not used by the stack
 ********************************************************************/
#if defined(ENABLE_TABLE_DUMP)
void DumpNetworkTable(void)
{
    BYTE i;

    ConsolePutROMString((ROM char*)"Available networks\r\nHandle PANID ADDR CHAN RSSI PROT REV PCOORD ASSOC\r\n");
    for(i=0;i<sizeof(networkTable)/sizeof(NETWORK_TABLE_ENTRY);i++)
    {
        if(networkStatus[i].bits.isValid)
        {
            if(networkStatus[i].bits.NeighborOrNetwork == NETWORK)
            {
                //this is a network
                PrintChar(i);
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                PrintChar(networkTable[i].PANID.v[1]);
                PrintChar(networkTable[i].PANID.v[0]);
                ConsolePut(' ');
                ConsolePut(' ');
                PrintChar(networkTable[i].ShortAddress.v[1]);
                PrintChar(networkTable[i].ShortAddress.v[0]);
                ConsolePut(' ');
                PrintChar(networkTable[i].info.networkInfo.Channel);
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                PrintChar(networkTable[i].info.networkInfo.sampleRSSI);
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                PrintChar(networkTable[i].info.networkInfo.Protocol);
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                PrintChar(networkTable[i].info.networkInfo.Version);
                ConsolePut(' ');
                ConsolePut(' ');
                if(networkTable[i].info.networkInfo.flags.bits.PANcoordinator)
                {
                    ConsolePut('Y');
                }
                else
                {
                    ConsolePut('N');
                }
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                if(networkTable[i].info.networkInfo.flags.bits.associationPermit)
                {
                    ConsolePut('Y');
                }
                else
                {
                    ConsolePut('N');
                }
                ConsolePut('\r');
                ConsolePut('\n');
            }
        }
    }
    
    ConsolePut('\r');
    ConsolePut('\n');
    
    ConsolePutROMString((ROM char*)"Available nodes\r\nHandle RX DC P2P PANID ADDR LONG_ADDR\r\n");
    for(i=0;i<NETWORK_TABLE_SIZE;i++)
    {
        if(networkStatus[i].bits.isValid)
        {
            if(networkStatus[i].bits.NeighborOrNetwork == NEIGHBOR)
            {
                PrintChar(i);
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                
                if(networkStatus[i].bits.RXOnWhenIdle == 1)
                {
                    ConsolePut('Y');
                }
                else
                {
                    ConsolePut('N');
                }
                ConsolePut(' ');
                ConsolePut(' ');
                
                if(networkStatus[i].bits.directConnection == 1)
                {
                    ConsolePut('Y');
                }
                else
                {
                    ConsolePut('N');
                }
                ConsolePut(' ');
                ConsolePut(' ');
                
                if(networkStatus[i].bits.P2PConnection == 1)
                {
                    ConsolePut('Y');
                }
                else
                {
                    ConsolePut('N');
                }
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                
                if(networkStatus[i].bits.shortAddressValid)
                {
                    //this is a neighbor
                    PrintChar(networkTable[i].PANID.v[1]);
                    PrintChar(networkTable[i].PANID.v[0]);
                    ConsolePut(' ');
                    ConsolePut(' ');
                    PrintChar(networkTable[i].ShortAddress.v[1]);
                    PrintChar(networkTable[i].ShortAddress.v[0]);
                    ConsolePut(' ');
                }
                else
                {
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut(' ');
                    ConsolePut(' ');
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut(' ');
                }
                
                if(networkStatus[i].bits.longAddressValid)
                {
                    PrintChar(networkTable[i].info.LongAddress[7]);
                    PrintChar(networkTable[i].info.LongAddress[6]);
                    PrintChar(networkTable[i].info.LongAddress[5]);
                    PrintChar(networkTable[i].info.LongAddress[4]);
                    PrintChar(networkTable[i].info.LongAddress[3]);
                    PrintChar(networkTable[i].info.LongAddress[2]);
                    PrintChar(networkTable[i].info.LongAddress[1]);
                    PrintChar(networkTable[i].info.LongAddress[0]);
                    ConsolePut('\r');
                    ConsolePut('\n');
                }
                else
                {
                    ConsolePut('\r');
                    ConsolePut('\n');
                }
            }
        }
    }
    
#if defined(I_AM_COORDINATOR_CAPABLE)    
    ConsolePutROMString((ROM char*)"\r\nIndirect Buffer\r\nSize Handle TimeStamp Message\r\n");
    for(i=0;i<INDIRECT_BUFFER_SIZE;i++)
    {
        if(indirectBuffer[i]==0)
        {
            break;
        }
        else
        {
            BYTE j;
            
            j = indirectBuffer[i] - 6;
            
            PrintChar(indirectBuffer[i++]);
            ConsolePutROMString((ROM char*)"   ");
            PrintChar(indirectBuffer[i++]);
            ConsolePutROMString((ROM char*)"     ");
            PrintChar(indirectBuffer[i++]);
            PrintChar(indirectBuffer[i++]);
            PrintChar(indirectBuffer[i++]);
            PrintChar(indirectBuffer[i++]);
            ConsolePutROMString((ROM char*)"  ");
            while(j--)
            {
                PrintChar(indirectBuffer[i++]);
            }
            i--;
            ConsolePutROMString((ROM char*)"\r\n");
        }
    }
#endif

#if defined(I_AM_COORDINATOR_CAPABLE)    
    ConsolePutROMString((ROM char*)"\r\nRouting Table\r\ncoord nextHop\r\n");
    for(i=0;i<8;i++)
    {
        PrintChar(i);
        ConsolePut(' ');
        ConsolePut(' ');
        ConsolePut(' ');
        ConsolePut(' ');
        PrintChar(RoutingTable[i]);
        ConsolePut('\r');
        ConsolePut('\n');
    }
#endif
}
#else
void DumpNetworkTable(void){}
#endif


/*********************************************************************
 * Function:        void DiscoverNodeByEUI(void)
 *
 * PreCondition:    tempLongAddress[0-7] need to be set to the address
 *                  that needs to be discovered
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function constructs and sends out the from to
 *                  discover a device by their EUI address
 ********************************************************************/
#if !defined(P2P_ONLY)
#if defined(SUPPORT_EUI_ADDRESS_SEARCH)
void DiscoverNodeByEUI(void)
{
    #if !defined(I_AM_COORDINATOR_CAPABLE)
        //if we a member of the network
        if(MiWiStateMachine.memberOfNetwork == 1)
        {
            //load up and send out the request
	        #ifdef SUPPORT_SECURITY
    	    	TxBuffer[TxHeader--] = myShortAddress.v[1];
	        	TxBuffer[TxHeader--] = myShortAddress.v[0];
	        	TxBuffer[TxHeader--] = networkTable[myParent].ShortAddress.v[1];
	    	    TxBuffer[TxHeader--] = networkTable[myParent].ShortAddress.v[0];
    	    	TxBuffer[TxHeader--] = myPANID.v[1];
	        	TxBuffer[TxHeader--] = myPANID.v[0];
	        	TxBuffer[TxHeader--] = IEEESeqNum++;
	    	    TxBuffer[TxHeader--] = 0x88;
    	    	TxBuffer[TxHeader--] = 0x61;
	        #else
                TxBuffer[TxData++] = 0x61;      //Data command, intrapan
                TxBuffer[TxData++] = 0x88;      //both short addresses
                TxBuffer[TxData++] = IEEESeqNum++;     
                TxBuffer[TxData++] = myPANID.v[0];      //destination PANID LSB
                TxBuffer[TxData++] = myPANID.v[1];      //dest PANID MSB
                TxBuffer[TxData++] = networkTable[myParent].ShortAddress.v[0];      //dest address LSB
                TxBuffer[TxData++] = networkTable[myParent].ShortAddress.v[1];      //dest address MSB
                TxBuffer[TxData++] = myShortAddress.v[0];      //source address LSB
                TxBuffer[TxData++] = myShortAddress.v[1];      //source address MSB
            #endif
            TxBuffer[TxData++] = MAX_HOPS;      
            TxBuffer[TxData++] = 0x06;      //Frame Control
            TxBuffer[TxData++] = 0xFF;      //dest PANID LSB
            TxBuffer[TxData++] = 0xFF;      //dest PANID MSB
            TxBuffer[TxData++] = 0xFF;      //dest address LSB
            TxBuffer[TxData++] = 0xFF;      //dest address MSB
            TxBuffer[TxData++] = myPANID.v[0];      //source PANID LSB
            TxBuffer[TxData++] = myPANID.v[1];      //source PANID MSB
            TxBuffer[TxData++] = myShortAddress.v[0];      //source address LSB
            TxBuffer[TxData++] = myShortAddress.v[1];      //source address MSB
            TxBuffer[TxData++] = MiWiSeqNum++;      //seq num
            TxBuffer[TxData++] = MIWI_STACK_REPORT_TYPE;      //Report Type
            TxBuffer[TxData++] = EUI_ADDRESS_SEARCH_REQUEST;      //Report ID
            TxBuffer[TxData++] = tempLongAddress[0];      //Data byte 0
            TxBuffer[TxData++] = tempLongAddress[1];
            TxBuffer[TxData++] = tempLongAddress[2];
            TxBuffer[TxData++] = tempLongAddress[3];
            TxBuffer[TxData++] = tempLongAddress[4];
            TxBuffer[TxData++] = tempLongAddress[5];
            TxBuffer[TxData++] = tempLongAddress[6];
            TxBuffer[TxData++] = tempLongAddress[7];
            #ifdef SUPPORT_SECURITY
            	SendTxBuffer(TRUE, TRUE);
            #else
                SendTxBuffer(TRUE);
            #endif
        }
    #else
        //if I am a coordinator capable device
        if(MiWiStateMachine.memberOfNetwork == 1)
        {
    	    #ifdef SUPPORT_SECURITY
	        	TxBuffer[TxHeader--] = myShortAddress.v[1];
	        	TxBuffer[TxHeader--] = myShortAddress.v[0];
	    	    TxBuffer[TxHeader--] = myPANID.v[1]; //Source PANID
    	    	TxBuffer[TxHeader--] = myPANID.v[0]; //
	        	TxBuffer[TxHeader--] = 0xFF; //broadcast Dest Addr
	        	TxBuffer[TxHeader--] = 0xFF; //broadcast
	    	    TxBuffer[TxHeader--] = 0xFF; //broadcast Dest PANID
    	    	TxBuffer[TxHeader--] = 0xFF; //broadcast
	        	TxBuffer[TxHeader--] = IEEESeqNum++;
	        	TxBuffer[TxHeader--] = 0x88;
	    	    TxBuffer[TxHeader--] = 0x01;
	        #else
                TxBuffer[TxData++] = 0x01;      //Data command, intrapan
                TxBuffer[TxData++] = 0x88;      //both short addresses
                TxBuffer[TxData++] = IEEESeqNum++;     
                TxBuffer[TxData++] = 0xFF;      //destination PANID LSB
                TxBuffer[TxData++] = 0xFF;      //dest PANID MSB
                TxBuffer[TxData++] = 0xFF;      //dest address LSB
                TxBuffer[TxData++] = 0xFF;      //dest address MSB
                TxBuffer[TxData++] = myPANID.v[0];      //source PANID LSB
                TxBuffer[TxData++] = myPANID.v[1];      //source PANID MSB
                TxBuffer[TxData++] = myShortAddress.v[0];      //source address LSB
                TxBuffer[TxData++] = myShortAddress.v[1];      //source address MSB
            #endif
            TxBuffer[TxData++] = MAX_HOPS;      
            TxBuffer[TxData++] = 0x06;      //Frame Control
            TxBuffer[TxData++] = 0xFF;      //dest PANID LSB
            TxBuffer[TxData++] = 0xFF;      //dest PANID MSB
            TxBuffer[TxData++] = 0xFF;      //dest address LSB
            TxBuffer[TxData++] = 0xFF;      //dest address MSB
            TxBuffer[TxData++] = myPANID.v[0];      //source PANID LSB
            TxBuffer[TxData++] = myPANID.v[1];      //source PANID MSB
            TxBuffer[TxData++] = myShortAddress.v[0];      //source address LSB
            TxBuffer[TxData++] = myShortAddress.v[1];      //source address MSB
            TxBuffer[TxData++] = MiWiSeqNum++;      //seq num
            TxBuffer[TxData++] = MIWI_STACK_REPORT_TYPE;      //Report Type
            TxBuffer[TxData++] = EUI_ADDRESS_SEARCH_REQUEST;      //Report ID
            TxBuffer[TxData++] = tempLongAddress[0];      //Data byte 0
            TxBuffer[TxData++] = tempLongAddress[1];
            TxBuffer[TxData++] = tempLongAddress[2];
            TxBuffer[TxData++] = tempLongAddress[3];
            TxBuffer[TxData++] = tempLongAddress[4];
            TxBuffer[TxData++] = tempLongAddress[5];
            TxBuffer[TxData++] = tempLongAddress[6];
            TxBuffer[TxData++] = tempLongAddress[7];
            #ifdef SUPPORT_SECURITY
            	SendTxBuffer(FALSE, FALSE);
            #else
                SendTxBuffer(FALSE);
            #endif
        }
    #endif
}
#endif
#endif

/*********************************************************************
 * Function:        void ClearNetworkTable(BYTE options)
 *
 * PreCondition:    None
 *
 * Input:           BYTE options - if the following option bits are set
 *                  then the following entries are erased:
 *                      b0 = 1 => clear anyone without direct connections
 *                      b1 = 1 => clear anyone without a long address
 *                      b2 = 1 => clear anyone without a short address (and is not P2P)
 *                      b3 = 1 => clear any P2P connection nodes
 *                      b4 = 1 => clear all network entries
 *                      b5 = 1 => clear all neighbor entries
 *                      b6 = not defined
 *                      b7 = not defined
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This erases the specified nodes from the network table
 ********************************************************************/
#if !defined(P2P_ONLY)
void ClearNetworkTable(BYTE options)
{
    BYTE i;
    BYTE_VAL clearOptions;
    
    clearOptions.Val = options;
    
    for(i=0;i<NETWORK_TABLE_SIZE;i++)
    {
        if(options == CLEAR_ALL_ENTRIES)
        {
            networkStatus[i].bits.isValid = 0;
            continue;
        }
        if(clearOptions.bits.b0)
        {
            if(networkStatus[i].bits.directConnection == 0)
            {
                networkStatus[i].bits.isValid = 0;
            }
        }
        if(clearOptions.bits.b1)
        {
            if(networkStatus[i].bits.longAddressValid == 0)
            {
                networkStatus[i].bits.isValid = 0;
            }
        }
        if(clearOptions.bits.b2)
        {
            if(networkStatus[i].bits.shortAddressValid == 0)
            {
                if(networkStatus[i].bits.P2PConnection == 0)
                {
                    networkStatus[i].bits.isValid = 0;
                }
            }
        }
        if(clearOptions.bits.b3)
        {
            if(networkStatus[i].bits.P2PConnection == 1)
            {
                networkStatus[i].bits.isValid = 0;
            }
        }
        if(clearOptions.bits.b4)
        {
            if(networkStatus[i].bits.NeighborOrNetwork == NETWORK)
            {
                networkStatus[i].bits.isValid = 0;
            }
        }
        if(clearOptions.bits.b5)
        {
            if(networkStatus[i].bits.NeighborOrNetwork == NEIGHBOR)
            {
                networkStatus[i].bits.isValid = 0;
            }
        }
        if(clearOptions.bits.b6)
        {
        }
        if(clearOptions.bits.b7)
        {
        }
    }    
}
#endif


/*********************************************************************
 * Function:        void FormNetwork(WORD PANID)
 *
 * PreCondition:    None
 *
 * Input:           WORD PANID - the # of the PANID that needs to be created
 *                      if 0xFFFF is inputed then the low byte of the 
 *                      symbol timer will be used to create a PANID.  This
 *                      will not be a very random PANID but can be replaced
 *                      with a users own random number generation
 *
 * Output:          None
 *
 * Side Effects:    the short address and PANID of the MRF24J40 are set
 *                  to the new network address information.  
 *
 * Overview:        This function will cause this device to become a 
 *                  PAN coordinator of the specified network
 ********************************************************************/
#if defined(I_AM_COORDINATOR_CAPABLE)
void FormNetwork(WORD PANID)
{
    BYTE i;
    BYTE j;
    BYTE k;
    BYTE minRSSI;
    BYTE worstRSSI;
    BYTE sampleRSSI;
    BYTE bestChannel;
    
    if(PANID == 0xFFFF)
    {
        //I want a random PANID
        #if defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24H__) || defined(__PIC32MX__)
            PANID = TMR2;
        #elif defined(__18CXX)        
            PANID=TMR0L;
            PANID+=(TMR0H<<6);
        #endif
    }
        
    //do a search of your network table and see if the PANID that
    //we selected already exists in my radio sphere
    for(i=0;i<NETWORK_TABLE_SIZE;i++)
    {
        if(networkStatus[i].bits.isValid)
        {
            if(networkStatus[i].bits.NeighborOrNetwork == NETWORK)
            {
                if(networkTable[i].PANID.Val == PANID)
                {
                    //there was a PANID conflict with a network that I can hear
                    ConsolePutROMString((ROM char*)"PANID conflict\r\n");
                    return;
                }
            }
        }
    }
    
    minRSSI = 0xFF;
    
    //for all of the allowable channels
    for(i=0;i<AVAILABLE_CHANNELS_SIZE;i++)
    {
        //clear the worst RSSI for that channel
        worstRSSI = 0;
        
        //traverse through the list of available channels
        SetChannel(availableChannels[i]);
        
        //get the RSSI
        PHYSetShortRAMAddr(WRITE_RXFLUSH,0x03);
        PHYSetShortRAMAddr(WRITE_RXMCR,0x0D);

        //sample the RSSI for the requested number of times
        for(j=0;j<RSSI_SAMPLES_PER_CHANNEL;j++)
        {
            //sample the RSSI
            PHYSetShortRAMAddr(WRITE_BBREG6,0x80);
            
            k = 0;
            do
            {
                PHYSetShortRAMAddr(WRITE_BBREG6,0x80);
                k++;
                sampleRSSI = PHYGetShortRAMAddr(READ_BBREG6);
            }
            while(((sampleRSSI & 0x01) == 0x01) && (k<200));
              
            //bail if the MRF24J40 gets locked up
            //trying to get the RSSI value
            if(k>=200)
            {
                //flush the RX buffer, disable filters, set WAKE active high
                PHYSetShortRAMAddr(WRITE_RXFLUSH,0x41);
                
                //set as a pan coordinator, coordinator, disable promi mode
                PHYSetShortRAMAddr(WRITE_RXMCR,0x0C);
                return;
            }
                     
            //read the RSSI value   
            sampleRSSI = PHYGetLongRAMAddr(RSSI);
            
            //if the RSSI value is higher than the worst
            if(sampleRSSI > worstRSSI)
            {
                //then this is the new worst channel (highest RSSI - most noise)
                worstRSSI = sampleRSSI;
            }
        }
        //if the minRSSI is worse than the worst sample of the other channel
        if(worstRSSI < minRSSI)
        {
            //save this as the best channel thus far
            bestChannel = availableChannels[i];
            minRSSI = worstRSSI;
        }
    }
    
    //change to the best channel
    SetChannel(bestChannel);
    
    ConsolePutROMString((ROM char*)"Starting my own network\r\n");

    //flush the RX buffer, disable filters, set WAKE active high
    PHYSetShortRAMAddr(WRITE_RXFLUSH,0x41);
    
    //set as a pan coordinator, coordinator, disable promi mode
    PHYSetShortRAMAddr(WRITE_RXMCR,0x0C);
    
    //Configure to calculate RSSI on packet reception
    PHYSetShortRAMAddr(WRITE_BBREG6,0x40);
    
    myPANID.Val = PANID;
    myShortAddress.Val = 0x0000;
    PHYSetShortRAMAddr(WRITE_SADRL,myShortAddress.v[0]);
    PHYSetShortRAMAddr(WRITE_SADRH,myShortAddress.v[1]);
    PHYSetShortRAMAddr(WRITE_PANIDL,myPANID.v[0]);
    PHYSetShortRAMAddr(WRITE_PANIDH,myPANID.v[1]);
    PHYSetShortRAMAddr(WRITE_ACKTMOUT,0xB9);    //enable the pending bit in the ACK for data requests
    MiWiStateMachine.memberOfNetwork = 1;
    role = ROLE_PAN_COORDINATOR;
    knownCoordinators = 0x01;   //I know myself
    return;    
}
#endif


/*********************************************************************
 * Function:        void OpenSocket(BYTE socketType)
 *
 * PreCondition:    None
 *
 * Input:           BYTE socketType - either CLUSTER_SOCKET for a cluster
 *                  socket or P2P_SOCKET for a P2P socket.
 *
 * Output:          None
 *
 * Side Effects:    This will send out a packet trying to create a socket
 *
 * Overview:        This will send out a packet to the PAN coordinator 
 *                  that is trying to link this device to another device.
 *                  the socket operation is complete when OpenSocketComplete()
 *                  returns TRUE.  The status of the operation is retreived through
 *                  the OpenSocketSuccessful() function.  If it returns TRUE
 *                  then OpenSocketHandle() returns the handle of the created
 *                  socket.  This value is valid until openSocket() is called
 *                  again.  If OpenSocketComplete() returns FALSE then the 
 *                  scan in still in progress.
 ********************************************************************/
void OpenSocket(BYTE socketType)
{
    BYTE i;
    
    #if defined(SUPPORT_CLUSTER_SOCKETS)
    if(socketType == CLUSTER_SOCKET)
    {        
        WORD_VAL PANcoordinator;
        
        openSocketInfo.status.bits.matchFound = 0;
        
        #if defined(I_AM_COORDINATOR_CAPABLE)
        
        //If I am the PAN coordinator
        if(role == ROLE_PAN_COORDINATOR)
        {
            BYTE j;
            //if there isn't a request currently open
            if(openSocketInfo.status.bits.requestIsOpen == 0)
            {
                //I am the PAN coordinator, there is no reason to send a request out
                openSocketInfo.socketStart = TickGet();
                openSocketInfo.ShortAddress1.Val = 0x0000;
                for(j=0;j<8;j++)
                {
                    openSocketInfo.LongAddress1[j] = myLongAddress[j];
                }
                openSocketInfo.status.bits.requestIsOpen = 1;
                openSocketInfo.status.bits.matchFound = 0;
                openSocketInfo.status.bits.type = CLUSTER_SOCKET;
            }
            else
            {
                if(openSocketInfo.ShortAddress1.Val == 0x0000)
                {
                    //I am the PAN coordinator and I don't want to talk to myself so lets forget we even tried
                    return;
                }

                if(openSocketInfo.ShortAddress1.Val != myShortAddress.Val)
                {
	                TxPayLoad();
                    TxBuffer[TxData++] = MIWI_STACK_REPORT_TYPE;      //Report Type
                    TxBuffer[TxData++] = OPEN_CLUSTER_SOCKET_RESPONSE;      //Report ID
                    TxBuffer[TxData++] = myLongAddress[0];      //Data byte 0
                    TxBuffer[TxData++] = myLongAddress[1];      //Data byte 1
                    TxBuffer[TxData++] = myLongAddress[2];      //Data byte 2
                    TxBuffer[TxData++] = myLongAddress[3];      //Data byte 3
                    TxBuffer[TxData++] = myLongAddress[4];      //Data byte 4
                    TxBuffer[TxData++] = myLongAddress[5];      //Data byte 5
                    TxBuffer[TxData++] = myLongAddress[6];      //Data byte 6
                    TxBuffer[TxData++] = myLongAddress[7];      //Data byte 7
                    TxBuffer[TxData++] = myShortAddress.v[0];
                    TxBuffer[TxData++] = myShortAddress.v[1];
                
                    for(j=0;j<8;j++)
                    {
                        tempLongAddress[j] = openSocketInfo.LongAddress1[j];
                    }
                    tempShortAddress.Val = openSocketInfo.ShortAddress1.Val;
                    tempPANID.Val = myPANID.Val;

                    openSocketInfo.socketHandle = SearchForShortAddress(NEIGHBOR);
                
                    if(openSocketInfo.socketHandle != 0xFF)
                    {
                        tempNodeStatus.Val = networkStatus[openSocketInfo.socketHandle].Val;
                    }
                    else
                    {
                        //Family, RxOnWHenIdle, Neighbor/Network, P2P, ShortVal, LongVal, Direct, Valid
                        tempNodeStatus.Val = 0x2D;
                        if((tempShortAddress.v[0] & 0x80) == 0)
                        {
                            tempNodeStatus.Val |= 0x40;
                        }
                    }
                                                    
                    openSocketInfo.socketHandle = AddNodeToNetworkTable();
                    
                    SendReportByShortAddress(myPANID,openSocketInfo.ShortAddress1,FALSE);
                    openSocketInfo.status.bits.requestIsOpen = 0;
                    openSocketInfo.status.bits.matchFound = 1;    
                }            
            }
            return;
        }
        else
        {
            //take a record of when you started to send the socket request
            //and send it to the PAN coordinator
            openSocketInfo.socketStart = TickGet();
        }
        
        #else
        openSocketInfo.socketStart = TickGet();
        #endif
           
        PANcoordinator.Val = 0x0000;

		TxPayLoad();
        TxBuffer[TxData++] = MIWI_STACK_REPORT_TYPE;      //Report Type
        TxBuffer[TxData++] = OPEN_CLUSTER_SOCKET_REQUEST;      //Report ID
        TxBuffer[TxData++] = myLongAddress[0];      //Data byte 0
        TxBuffer[TxData++] = myLongAddress[1];      //Data byte 1
        TxBuffer[TxData++] = myLongAddress[2];      //Data byte 2
        TxBuffer[TxData++] = myLongAddress[3];      //Data byte 3
        TxBuffer[TxData++] = myLongAddress[4];      //Data byte 4
        TxBuffer[TxData++] = myLongAddress[5];      //Data byte 5
        TxBuffer[TxData++] = myLongAddress[6];      //Data byte 6
        TxBuffer[TxData++] = myLongAddress[7];      //Data byte 7
        SendReportByShortAddress(myPANID,PANcoordinator,FALSE);
        
        openSocketInfo.status.bits.requestIsOpen = 1;

    }
    else 
    #endif
    #if defined(SUPPORT_P2P_SOCKETS)
    if(socketType == P2P_SOCKET)
    {       
        if(openSocketInfo.status.bits.requestIsOpen == 0)
        {
            BYTE j;
            
            openSocketInfo.socketStart = TickGet();
            
            //send out an open P2P socket request               
            TxBuffer[TxData++] = 0; //no hops required because it will be a direct communication
            TxBuffer[TxData++] = 0x00;  //frame control - no ack, intra cluster, no encrypt (for now)
            TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
            TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
            TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
            TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
            TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
            TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
            TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
            TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
            TxBuffer[TxData++] = MiWiSeqNum++;      //sequence number
            TxBuffer[TxData++] = MIWI_STACK_REPORT_TYPE;      //Report Type
            TxBuffer[TxData++] = OPEN_P2P_SOCKET_REQUEST;      //Report ID
            
            TxBuffer[TxHeader--] = myLongAddress[7];  //MSB source long address 
            TxBuffer[TxHeader--] = myLongAddress[6];  //source long address 
            TxBuffer[TxHeader--] = myLongAddress[5];  //source long address 
            TxBuffer[TxHeader--] = myLongAddress[4];  //source long address 
            TxBuffer[TxHeader--] = myLongAddress[3];  //source long address 
            TxBuffer[TxHeader--] = myLongAddress[2];  //source long address 
            TxBuffer[TxHeader--] = myLongAddress[1];  //source long address 
            TxBuffer[TxHeader--] = myLongAddress[0];  //source long address 
            
            TxBuffer[TxHeader--] = 0xFF;  //MSB source PANID
            TxBuffer[TxHeader--] = 0xFF;  //source PANID
            
            TxBuffer[TxHeader--] = 0xFF;  //dest address broadcast 
            TxBuffer[TxHeader--] = 0xFF;  //dest address broadcast
    
            TxBuffer[TxHeader--] = 0xFF;  //MSB dest PANID
            TxBuffer[TxHeader--] = 0xFF;  //dest PANID
            
            TxBuffer[TxHeader--] = IEEESeqNum++;  //seq num
            
            TxBuffer[TxHeader--] = 0xC8;  //frame control -  both long addresses

            TxBuffer[TxHeader--] = 0x01;  //frame control -  command packet
    
    		#ifdef SUPPORT_SECURITY
	    		SendTxBuffer(FALSE, TRUE);
		    #else
                SendTxBuffer(FALSE);
            #endif
            
            openSocketInfo.status.bits.requestIsOpen = 1;
            openSocketInfo.status.bits.itIsMe = 1;

            openSocketInfo.socketStart = TickGet();
            #if defined(I_AM_COORDINATOR_CAPABLE)
            for(j=0;j<8;j++)
            {
                openSocketInfo.LongAddress1[j] = myLongAddress[j];
            }
            #endif
            openSocketInfo.status.bits.matchFound = 0;
            openSocketInfo.status.bits.type = P2P_SOCKET;
        } 
        #if defined(I_AM_COORDINATOR_CAPABLE)  
        else
        {
            if(openSocketInfo.status.bits.itIsMe == 0)
            {
                BYTE handle,i;
                BOOL previousEntry;
                
                previousEntry = FALSE;
                
                for(i=0;i<8;i++)
                {
                    tempLongAddress[i] = openSocketInfo.LongAddress1[i];
                }
                
                handle = SearchForLongAddress();
                
                if(handle != 0xFF)
                {
                    previousEntry = TRUE;
                }
                else
                {                 
                    handle = findNextNetworkEntry();
                }
                    
                if(handle != 0xFF)
                {
                    openSocketInfo.socketHandle = handle;
                    
                    //if there is room in the table to add them
                    //send out an open P2P socket request               
                    TxBuffer[TxData++] = 0; //no hops required because it will be a direct communication
                    TxBuffer[TxData++] = 0x00;  //frame control - no ack, intra cluster, no encrypt (for now)
                    TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
                    TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
                    TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
                    TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
                    TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
                    TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
                    TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
                    TxBuffer[TxData++] = 0xFF;  //addressing info is all 0xFF
                    TxBuffer[TxData++] = MiWiSeqNum++;      //sequence number
                    TxBuffer[TxData++] = MIWI_STACK_REPORT_TYPE;      //Report Type
                    TxBuffer[TxData++] = OPEN_P2P_SOCKET_RESPONSE;      //Report ID
                    
                    TxBuffer[TxHeader--] = myLongAddress[7];  //MSB source long address 
                    TxBuffer[TxHeader--] = myLongAddress[6];  //source long address 
                    TxBuffer[TxHeader--] = myLongAddress[5];  //source long address 
                    TxBuffer[TxHeader--] = myLongAddress[4];  //source long address 
                    TxBuffer[TxHeader--] = myLongAddress[3];  //source long address 
                    TxBuffer[TxHeader--] = myLongAddress[2];  //source long address 
                    TxBuffer[TxHeader--] = myLongAddress[1];  //source long address 
                    TxBuffer[TxHeader--] = myLongAddress[0];  //source long address 
                    
                    TxBuffer[TxHeader--] = 0xFF;  //MSB source PANID
                    TxBuffer[TxHeader--] = 0xFF;  //source PANID
                    
                    TxBuffer[TxHeader--] = openSocketInfo.LongAddress1[7];
                    TxBuffer[TxHeader--] = openSocketInfo.LongAddress1[6];
                    TxBuffer[TxHeader--] = openSocketInfo.LongAddress1[5];
                    TxBuffer[TxHeader--] = openSocketInfo.LongAddress1[4];
                    TxBuffer[TxHeader--] = openSocketInfo.LongAddress1[3];
                    TxBuffer[TxHeader--] = openSocketInfo.LongAddress1[2];
                    TxBuffer[TxHeader--] = openSocketInfo.LongAddress1[1];
                    TxBuffer[TxHeader--] = openSocketInfo.LongAddress1[0];
            
                    TxBuffer[TxHeader--] = 0xFF;  //MSB dest PANID
                    TxBuffer[TxHeader--] = 0xFF;  //dest PANID
                    
                    TxBuffer[TxHeader--] = IEEESeqNum++;  //seq num
                    
                    TxBuffer[TxHeader--] = 0xCC;  //frame control -  both long addresses

                    TxBuffer[TxHeader--] = 0x21;  //frame control -  command packet
            
                	#ifdef SUPPORT_SECURITY
                		SendTxBuffer(FALSE, TRUE);
                	#else
                        SendTxBuffer(FALSE);
                    #endif
                    
                    openSocketInfo.status.bits.matchFound = 1;
                    
                    if(previousEntry == FALSE)
                    {
                        //if I didn't know this node before then add him to the table.  otherwise leave the old data alone
                        networkStatus[handle].Val = 0x37;
                        for(i=0;i<8;i++)
                        {
                            networkTable[handle].info.LongAddress[i] = openSocketInfo.LongAddress1[i];
                        }
                    }
                }
            }
        }  
        #endif  
    }
    else
    #endif
    {
        //if the input to the function is incorrect
        //then just bail
        return;
    }
}

/*********************************************************************
 * Function:        void JoinNetwork(BYTE handle)
 *
 * PreCondition:    None
 *
 * Input:           BYTE handle - the index into the network table of 
 *                  network that you wish to join to.  
 *
 * Output:          None
 *
 * Side Effects:    the device attempts to join the specified network
 *
 * Overview:        This function is used to try to join to a network.
 *                  a user needs to select a network that it wants to join.
 *                  DiscoverNetworks() function can be used to find 
 *                  possible networks to join.  This will search all of the
 *                  available channels specified in the availableChannels 
 *                  array.  While the device is trying to join 
 *                  AttemptingToJoinNetwork() will return TRUE.  Once
 *                  the device successfully joins MemberOfNetwork() will
 *                  return TRUE.
 ********************************************************************/
#if !defined(P2P_ONLY)
void JoinNetwork(BYTE handle)
{
    //verify that the handle that we are trying to join to is valid
    if(networkStatus[handle].bits.isValid == 1)
    {
        SetChannel(networkTable[handle].info.networkInfo.Channel);
        
        //mark that we are trying to join to a network
        MiWiStateMachine.attemptingToJoin = 1;
        MiWiStateMachine.dataRequestSent = 0;
        myParent = handle;
        
        /* Program the PANID to the attempted network */
        PHYSetShortRAMAddr(WRITE_PANIDL,networkTable[handle].PANID.v[0]);
        PHYSetShortRAMAddr(WRITE_PANIDH,networkTable[handle].PANID.v[1]);
        myPANID.Val = networkTable[handle].PANID.Val;
    
        //send out the association request
        TxBuffer[0]=0x23;   //frame control - command
        TxBuffer[1]=0xC8;   //addressing mode - source only
        TxBuffer[2]=IEEESeqNum++;   //seq number 
        TxBuffer[3]=networkTable[handle].PANID.v[0];   //PANID LSB
        TxBuffer[4]=networkTable[handle].PANID.v[1];   //PANID MSB
        TxBuffer[5]=networkTable[handle].ShortAddress.v[0];   //short address LSB
        TxBuffer[6]=networkTable[handle].ShortAddress.v[1];   //short address MSB
        TxBuffer[7]=0xFF;   //source PANID LSB
        TxBuffer[8]=0xFF;   //source PANID MSB
        TxBuffer[9]=myLongAddress[0];   //source address EUI LSB
        TxBuffer[10]=myLongAddress[1];  //source address EUI B1
        TxBuffer[11]=myLongAddress[2];  //source address EUI B2
        TxBuffer[12]=myLongAddress[3];  //source address EUI B3
        TxBuffer[13]=myLongAddress[4];  //source address EUI B4
        TxBuffer[14]=myLongAddress[5];  //source address EUI B5
        TxBuffer[15]=myLongAddress[6];  //source address EUI B6
        TxBuffer[16]=myLongAddress[7];  //source address EUI MSB
        TxBuffer[17]=0x01;  //association request
        TxBuffer[18]=CAP_INFO;  //cap info of requesting device 
                    
        TxData = 19;
        #ifdef SUPPORT_SECURITY
        	SendTxBuffer(TRUE, FALSE);
        #else
            SendTxBuffer(TRUE);  
        #endif
        
        startTick = TickGet();  
    }
}
#endif

/*********************************************************************
 * Function:        BOOL SendIndirectMessage(BYTE handle)
 *
 * PreCondition:    TxBuffer loaded with the data that you wish to send
 *
 * Input:           BYTE handle - the index into the network table of 
 *                  device you want to send the message to.  
 *
 * Output:          None
 *
 * Side Effects:    the indirect buffer is loaded with the packet
 *
 * Overview:        This function is used to transmit a packet to a device
 *                  that polls for data (RFD).  This function buffers
 *                  the provided packet in the indirect transmission buffer
 *                  for up to INDIRECT_BUFFER_TIMEOUT time.  If this 
 *                  time is exceeded then the packet is discarded from
 *                  the indirect buffer
 ********************************************************************/
#if defined(I_AM_COORDINATOR_CAPABLE)
BOOL SendIndirectMessage(BYTE handle)
{
    BYTE i,j;
    TICK t;
    
    i = 0;
    
    while(indirectBuffer[i]!=0)
    {
        i+=indirectBuffer[i];
    }
    if((TxData + 6 + (TX_BUFFER_SIZE - TxHeader - 1)) < (INDIRECT_BUFFER_SIZE - i))
    {
        t = TickGet();
        
        indirectBuffer[i++] = TxData + (TX_BUFFER_SIZE - TxHeader - 1) + 6;
        indirectBuffer[i++] = handle;
        indirectBuffer[i++] = t.byte.b3;
        indirectBuffer[i++] = t.byte.b2;
        indirectBuffer[i++] = t.byte.b1;
        indirectBuffer[i++] = t.byte.b0;
       
        while(TxHeader < (TX_BUFFER_SIZE - 1))
        {
            indirectBuffer[i++]=TxBuffer[++TxHeader];
        }  
        
        for(j=0;j<TxData;j++)
        {
            indirectBuffer[i++]=TxBuffer[j];
        }
       
        indirectBuffer[i]=0;
        
        FlushTx();
        
        return TRUE;
    }
    
    TxData = 0;
    return FALSE;
}
#endif


/*********************************************************************
 * Function:        void SetChannel(BYTE channel)
 *
 * PreCondition:    MRF24J40 is initialized
 *
 * Input:           BYTE channel - this is the channel that you wish
 *                  to operate on.  This should be CHANNEL_11, CHANNEL_12,
 *                  ..., CHANNEL_26. 
 *
 * Output:          None
 *
 * Side Effects:    the MRF24J40 now operates on that channel
 *
 * Overview:        This function sets the current operating channel
 *                  of the MRF24J40
 ********************************************************************/
void SetChannel(BYTE channel)
{
    currentChannel = channel;
    PHYSetLongRAMAddr(RFCTRL0,channel|0x02);
    PHYSetShortRAMAddr(WRITE_RFCTL,0x04);
    PHYSetShortRAMAddr(WRITE_RFCTL,0x00);
}


/*********************************************************************
 * Function:        BYTE SearchForShortAddress(BYTE entryType)
 *
 * PreCondition:    tempShortAddress and tempPANID are set to the device
 *                  that you are looking for
 *
 * Input:           BYTE entryType - this is set to NEIGHBOR if you
 *                  want to find a Node on the network.  This value
 *                  is set to NETWORK if you are looking for a specific network
 *                  and not a node on the current operating network
 *
 * Output:          BYTE - the index of the network table entry of the
 *                  requested device.  0xFF indicates that the requested
 *                  device doesn't exist in the network table
 *
 * Side Effects:    None
 *
 * Overview:        This function looks up the index of a node or network
 *                  in the network table by short address.
 ********************************************************************/
#if !defined(P2P_ONLY)
BYTE SearchForShortAddress(BYTE entryType)
{
    BYTE i;   
    
    for(i=0;i<NETWORK_TABLE_SIZE;i++)
    {
        if(networkStatus[i].bits.isValid)
        {
            if(networkStatus[i].bits.NeighborOrNetwork == entryType)
            {
                if((networkTable[i].PANID.Val == tempPANID.Val) && (networkTable[i].ShortAddress.Val == tempShortAddress.Val))
                {
                    return i;
                }
            }
        }
    }
    return 0xFF;
}
#endif

/*********************************************************************
 * Function:        BYTE SearchForLongAddress(void)
 *
 * PreCondition:    tempLongAddress is set to the device
 *                  that you are looking for
 *
 * Input:           None
 *
 * Output:          BYTE - the index of the network table entry of the
 *                  requested device.  0xFF indicates that the requested
 *                  device doesn't exist in the network table
 *
 * Side Effects:    None
 *
 * Overview:        This function looks up the index of a node or network
 *                  in the network table by long address.
 ********************************************************************/
//#if !defined(P2P_ONLY)
BYTE SearchForLongAddress(void)
{
    BYTE i,j;   
    
    for(i=0;i<NETWORK_TABLE_SIZE;i++)
    {
        if(networkStatus[i].bits.isValid)
        {
            for(j=0;j<8;j++)
            {
                if(networkTable[i].info.LongAddress[j] != tempLongAddress[j])
                {
                    goto EndOfSearchLoop;
                }
            }
            return i;
        }
EndOfSearchLoop:
        ;
    }
    return 0xFF;
}
//#endif


/*********************************************************************
 * Function:        BYTE AddNodeToNetworkTable(void)
 *
 * PreCondition:    tempLongAddress, tempShortAddress, tempPANID, and 
 *                  tempNodeStatus are set to the correct values.  If 
 *                  tempNodeStatus.bits.longAddressValid is set then
 *                  tempLongAddress needs to be set.  If 
 *                  tempNodeStatus.bits.shortAddressValid is set then
 *                  tempShortAddress and tempPANID need to be set.
 *
 * Input:           None
 *
 * Output:          BYTE - the index of the network table entry 
 *                  where the device was inserted.  0xFF indicates that
 *                  the requested device couldn't be inserted into the table
 *
 * Side Effects:    Network table is updated with the devices info
 *
 * Overview:        This function is used to insert new device into the 
 *                  network table (or update already existing entries)
 ********************************************************************/
#if !defined(P2P_ONLY)
BYTE AddNodeToNetworkTable(void)
{
    BYTE handle;
    
    if(tempNodeStatus.bits.longAddressValid)
    {
        handle = SearchForLongAddress();
    }
    else
    {
        if(tempNodeStatus.bits.shortAddressValid)
        {
            handle = SearchForShortAddress(NEIGHBOR);
        }
    }
                                   
    if(handle==0xFF)
    {
        handle = findNextNetworkEntry();
    }
    
    if(handle != 0xFF)
    { 
        //isFamily = 1, RxOnWhenIdle = 1, Is a neighbor and not a network,
        //not a P2P connection, short and long addresses valid as well 
        //as the entire entry
        
        //If it is not family then update the node information
        networkStatus[handle].Val = tempNodeStatus.Val;
        if(tempNodeStatus.bits.longAddressValid)
        {
            networkTable[handle].info.LongAddress[0] = tempLongAddress[0];
            networkTable[handle].info.LongAddress[1] = tempLongAddress[1];
            networkTable[handle].info.LongAddress[2] = tempLongAddress[2];
            networkTable[handle].info.LongAddress[3] = tempLongAddress[3];
            networkTable[handle].info.LongAddress[4] = tempLongAddress[4];
            networkTable[handle].info.LongAddress[5] = tempLongAddress[5];
            networkTable[handle].info.LongAddress[6] = tempLongAddress[6];
            networkTable[handle].info.LongAddress[7] = tempLongAddress[7];
        }
               
        if(tempNodeStatus.bits.shortAddressValid)
        {        
            networkTable[handle].ShortAddress.Val = tempShortAddress.Val;
        }
        
        networkTable[handle].PANID.Val = tempPANID.Val;
        #if defined(SUPPORT_SECURITY)
	        networkTable[handle].IncomingFrameCounter.Val = 0;
        #endif
    }
    
    return handle;
}
#endif

/*********************************************************************
 * Function:        void initMRF24J40(void)
 *
 * PreCondition:    BoardInit (or other initialzation code is required)
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    MRF24J40 is initialized
 *
 * Overview:        This function initializes the MRF24J40 and is required
 *                  before stack operation is available
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
    
    #if defined(P2P_ONLY)
        myShortAddress.Val = 0xFFFF;
        myPANID.Val = 0xFFFF;
    #endif
    
    /* Program the short MAC Address, 0xffff */
    PHYSetShortRAMAddr(WRITE_SADRL,myShortAddress.v[0]);
    PHYSetShortRAMAddr(WRITE_SADRH,myShortAddress.v[1]);
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
    PHYSetLongRAMAddr(RFCTRL1,0x01);
    PHYSetLongRAMAddr(RFCTRL2,0x80);
    
    PHYSetLongRAMAddr(RFCTRL3,PA_LEVEL);
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
    while((i&0xE0) != 0xA0);
    
    PHYSetShortRAMAddr(WRITE_INTMSK,0xE6);
    
    SetChannel(availableChannels[0]);

    #ifdef TURBO_MODE
    
        PHYSetShortRAMAddr(WRITE_BBREG0, 0x01);
        PHYSetShortRAMAddr(WRITE_BBREG3, 0x38);
        PHYSetShortRAMAddr(WRITE_BBREG4, 0x5C);
        
        PHYSetShortRAMAddr(WRITE_RFCTL,0x04);
        PHYSetShortRAMAddr(WRITE_RFCTL,0x00);

    #endif
}

/*********************************************************************
 * Function:        void MiWiInit(void)
 *
 * PreCondition:    BoardInit (or other initialzation code is required)
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    the MiWi stack is initialized
 *
 * Overview:        This function initializes the MiWi stack and is required
 *                  before stack operation is available
 ********************************************************************/
 void MiWiInit(void)
{
    BYTE i;
    
    myShortAddress.Val = 0xFFFF;
    myPANID.Val = 0xFFFF;
    
    initMRF24J40();
    
    //clear all status bits
    MiWiStatus.Val = 0;
    //enable reception
    MiWiStatus.bits.RX_ENABLED = 1;
    MiWiStatus.bits.RX_BUFFERED = 0;

    //clear the network table
    #if !defined(P2P_ONLY)
        ClearNetworkTable(CLEAR_ALL_ENTRIES);
    #endif
    
    #if defined(I_AM_COORDINATOR_CAPABLE)
    for(i=0;i<8;i++)
    {
        RoutingTable[i] = 0;
    }
    knownCoordinators = 0;
    #endif
    
    MiWiStateMachine.memberOfNetwork = 0;
    MiWiStateMachine.attemptingToJoin = 0;
    MiWiStateMachine.dataRequestNeeded = 0;
    
    openSocketInfo.status.Val = 0;
 
    InitSymbolTimer();
    
    TxHeader = TX_BUFFER_SIZE - 1;
    TxData = 0;
#if defined(I_AM_COORDINATOR_CAPABLE)
    //if you sent the first byte of the indirect table to 0 then the whole table is erased
    indirectBuffer[0]=0;
#endif

    RFIF = 0;
    RFIE = 1;

    if(RF_INT_PIN == 0)
    {
        RFIF = 1;
    }
    
#ifdef SUPPORT_SECURITY
	OutgoingFrameCounter.Val = 0;
#endif    
}

/*********************************************************************
* Function:        void MRF24J40Sleep(void)
*
* PreCondition:    BoardInit (or other initialzation code is required)
*
* Input:           None
*
* Output:          None
*
* Side Effects:    
*
* Overview:        
********************************************************************/

void MRF24J40Sleep(void)
{
    MiWiStatus.bits.PHY_SLEEPING = 1;
    
    //;clear the WAKE pin in order to allow the device to go to sleep
    PHY_WAKE = 0;
    
    // make a power management reset to ensure device goes to sleep
    PHYSetShortRAMAddr(WRITE_SOFTRST, 0x04);
    
    //;write the registers required to place the device in sleep
    PHYSetShortRAMAddr(WRITE_TXBCNINTL,0x80);
    PHYSetShortRAMAddr(WRITE_RXFLUSH,0x60);
    PHYSetShortRAMAddr(WRITE_SLPACK,0x80);
}

/*********************************************************************
 * Function:        void MRF24J40Wake(void)
 *
 * PreCondition:    BoardInit (or other initialzation code is required)
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    the MiWi stack is initialized
 *
 * Overview:        This function initializes the MiWi stack and is required
 *                  before stack operation is available
 ********************************************************************/
void MRF24J40Wake(void)
{
    BYTE results;
    TICK failureTimer;
    TICK currentDifference;
     
    //;wake up the device
    PHY_WAKE = 1;

    failureTimer = TickGet();

    while(1)
    {
        currentDifference = TickGet();
            
        currentDifference.Val = TickGetDiff(currentDifference,failureTimer);
        
        if(currentDifference.Val > 6250)
        {
            break;
        }
    
        results = PHYGetShortRAMAddr(READ_ISRSTS);
        if((results & 0x40))
        {
            break;
        }
    }
    
    while(1)
    {
        currentDifference = TickGet();
        
        currentDifference.Val = TickGetDiff(currentDifference, failureTimer);
        
        if( currentDifference.Val > 6250 )
        {
            break;
        }
        
        results = PHYGetLongRAMAddr(RFSTATE);
        if( (results & 0xE0) == 0xA0 )
        {
            break;
        }
    }
           
    MiWiStatus.bits.PHY_SLEEPING = 0;
 }
 
/*********************************************************************
 * Function:        void HighISR(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Various flags and registers set.
 *
 * Overview:        This is the interrupt handler for the MRF24J40 and
 *                  MiWi stack.
 ********************************************************************/
#if defined(__18CXX)
    #pragma interruptlow HighISR
    void HighISR(void)
#elif defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24H__)
    void _ISRFAST __attribute__((interrupt, auto_psv)) _INT1Interrupt(void)
#elif defined(__PIC32MX__)
    void __ISR(_EXTERNAL_1_VECTOR, ipl4) _INT1Interrupt(void)
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
                MiWiStatus.bits.TX_BUSY = 0;
                
                failureCounter = 0;
                
                //if we were waiting for an ACK
                if(MiWiStatus.bits.TX_SECURITY || MiWiStatus.bits.TX_PENDING_ACK)
                {
                    BYTE_VAL results;
                    
                    //read out the results of the transmission
                    results.Val = PHYGetShortRAMAddr(READ_TXSR);
                    
                    if(results.bits.b0 == 1)
                    {
                        //the transmission wasn't successful and the number
                        //of retries is located in bits 7-6 of TXSR
                    }
                    else
                    {
                        //transmission successful
                        //clear that I am pending an ACK, already got it
                        MiWiStatus.bits.TX_PENDING_ACK = 0;
                        
                    }
                }
                MiWiStatus.bits.TX_SECURITY = 0;
                
                PHYSetShortRAMAddr(WRITE_RXFLUSH,0x01);
            }
            
            else if(flags.bits.RF_RXIF)
            {      
                //if the RX interrupt was triggered
                if(MiWiStatus.bits.RX_ENABLED)
                {
                    //If the part is enabled for receiving packets right now
                    //(not pending an ACK)
                    //indicate that we have a packet in the buffer pending to 
                    //be read into the buffer from the FIFO
                    MiWiStatus.bits.RX_PENDING = 1;
                }
                else
                {
                    //else if the RX is not enabled then we need to flush this packet
                    //flush the buffer
                    PHYSetShortRAMAddr(WRITE_RXFLUSH,0x01);
                }//end of RX_ENABLED check
            } //end of RXIF check
            
            else 
            {
	            PHYSetShortRAMAddr(WRITE_RXFLUSH,0x01);
            } //end of RXIF check

            	
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

#ifdef SUPPORT_SECURITY

/*********************************************************************
 * Function:        CIPHER_STATUS PHYCipher(INPUT CIPHER_MODE CipherMode, INPUT SECURITY_INPUT SecurityInput, OUTPUT BYTE *OutData, OUTPUT BYTE *OutDataLen)
 *
 * PreCondition:    Called by DataEncrypt or DataDecrypt
 *
 * Input:           CIPHER_MODE CipherMode       - Either MODE_ENCRYPTION or MODE_DECRYPTION
 *                  SECURITY_INPUT SecurityInput - Cipher operation input. Filled by DataEncryption or DataDecryption
 *
 * Output:          BYTE *OutData                - Encrypted or decrypted data, including MIC
 *                  BYTE *OutDataLen             - Data length after cipher operation, including MIC bytes
 *                  CIPHER_STATUS                - Cipher operation result
 *
 * Side Effects:    Input data get encrypted or decrypted and put into output buffer
 *
 * Overview:        This is the function that invoke the hardware cipher to do encryption and decryption
 ********************************************************************/
CIPHER_STATUS PHYCipher(INPUT CIPHER_MODE CipherMode, INPUT SECURITY_INPUT SecurityInput, OUTPUT BYTE *OutData, OUTPUT BYTE *OutDataLen)
{
	BYTE CipherRetry = CIPHER_RETRY;
	BYTE i;
	WORD loc;

	// make sure that we are not in the process of sending out a packet
	while( MiWiStatus.bits.TX_BUSY )
	{
	   if(RF_INT_PIN == 0)
	   {
	       RFIF = 1;
	   }
		Nop();
	}
	
CipherOperationStart:

	// step 1, set the normal FIFO
	// step 1a. fill the length of the header
	if( mySecurityLevel > 0x04 )
	{
		PHYSetLongRAMAddr(0x000, SecurityInput.HeaderLen+SecurityInput.TextLen+13);
	} else {
		PHYSetLongRAMAddr(0x000, SecurityInput.HeaderLen+13);
	}

	// step 1b, fill the length of the packet
	if( CipherMode == MODE_ENCRYPTION )
	{
		PHYSetLongRAMAddr(0x001, SecurityInput.TextLen+SecurityInput.HeaderLen+13); 
	} else {
		PHYSetLongRAMAddr(0x001, SecurityInput.TextLen+SecurityInput.HeaderLen+15);// two additional bytes FCS
	}	

	//step 1c, fill the MiWi header
	loc = 0x002;
	for(i = 0; i < SecurityInput.HeaderLen; i++)
	{
		PHYSetLongRAMAddr(loc++, SecurityInput.Header[i]);
	}
	// step 1d, fill the frame counter
	for(i = 0; i < 4; i++)
	{
		PHYSetLongRAMAddr(loc++, SecurityInput.FrameCounter.v[i]);
	}
	// step 1e, fill the source extended address
	for(i = 0; i < 8; i++)
	{
		PHYSetLongRAMAddr(loc++, SecurityInput.SourceAddress[i]);
	}
	// step 1f, fill the security key sequence number. It will be useful
	// if application layer decided to be able to request and transport key
	PHYSetLongRAMAddr(loc++, myKeySequenceNumber);
	
	// step 1g, fill the data to be encrypted or decrypted
	for(i = 0; i < SecurityInput.TextLen; i++) 
	{
		PHYSetLongRAMAddr(loc++, SecurityInput.InputText[i]);
	}

	// step 2, set nounce
	loc = 0x24C;
	for(i = 0; i < 8; i++) {
		PHYSetLongRAMAddr(loc--, SecurityInput.SourceAddress[i]);
	}
	for(i = 0; i < 4; i++) 
	{
		PHYSetLongRAMAddr(loc--, SecurityInput.FrameCounter.v[i]);
	}
	PHYSetLongRAMAddr(loc--, mySecurityLevel);
	

	// step 3, set TXNFIFO security key
	loc = 0x280;
	for(i = 0; i < 16; i++) 
	{
		PHYSetLongRAMAddr(loc++, mySecurityKey[i]);
	}

	// step 4, set the mode either to encrypt or decrypt
	if( CipherMode == MODE_ENCRYPTION )
	{
		PHYSetShortRAMAddr(WRITE_SECCR2, 0x40);
	} else {
		PHYSetShortRAMAddr(WRITE_SECCR2, 0x80);
	}

	// step 5, fill the security mode
	PHYSetShortRAMAddr(WRITE_SECCR0, mySecurityLevel);

	// trigger the hardware cipher to operate
	MiWiStatus.bits.TX_SECURITY = 1;
	PHYSetShortRAMAddr(WRITE_TXNMTRIG, 0x03);

	// wait until hardware cipher report job finished
	// in interrupt handler
	i = 0;
	while( MiWiStatus.bits.TX_SECURITY ) 
	{
	   if(RF_INT_PIN == 0)
	   {
	       RFIF = 1;
	   }
	   
		i++;
		#if 1
		// in rare condition, the hardware cipher will stall. Handle such condition
		// here
		if(i > 0xef)
		{
			ConsolePutROMString((ROM char*)"X ");
			MiWiStatus.bits.TX_SECURITY = 0;
			PHY_RESETn = 0;
			initMRF24J40();
			return CIPHER_ERROR;
		}
		#endif
	}
	//PHYSetShortRAMAddr(WRITE_RXFLUSH, 0x01);


	// if MIC is generated, check MIC here
	if( (CipherMode == MODE_DECRYPTION) && (mySecurityLevel != 0x01))
	{
		BYTE MIC_check = PHYGetShortRAMAddr(READ_RXSR);
		if( MIC_check & 0x40 )
		{
			PHYSetShortRAMAddr(WRITE_RXSR, 0x40);
			// there is a small chance that the hardware cipher will not 
			// decrypt for the first time, retry to solve this problem.
			// details documented in errata
			if( CipherRetry )
			{
				//ConsolePutROMString((ROM char*)"Cipher Retry\r\n");
				CipherRetry--;
				for(loc = 0; loc < 0x255; loc++);
				goto CipherOperationStart;
			}
			PHY_RESETn = 0;
			initMRF24J40();
			return CIPHER_MIC_ERROR;
		}
	} 

	// working with interrupt handler to solve the problem in case
	// there is an error while doing hardware cipher operation. The error
	// can be detected while reading TXSR register. 
	{	
		// get output data length
		*OutDataLen = PHYGetLongRAMAddr(0x001)- SecurityInput.HeaderLen - 13; 
		// get the index of data encrypted or decrypted
		loc = 0x002 + SecurityInput.HeaderLen + 13;
		
		// if this is a decryption operation, get rid of the useless MIC and two bytes FCS
		if( CipherMode == MODE_DECRYPTION )
		{
			switch( mySecurityLevel )
			{
				case 0x02:
				case 0x05:
					*OutDataLen -= 18; 
					break;
				case 0x03:
				case 0x06:
					*OutDataLen -= 10; 
					break;
				case 0x04:
				case 0x07:
					*OutDataLen -= 6; 
					break;
				case 0x01:
					*OutDataLen -= 2;
					break;
			}
		}
		
		// copy the output data
		for(i = 0; i < *OutDataLen; i++) 
		{
			OutData[i] = PHYGetLongRAMAddr(loc++);
		}
		return CIPHER_SUCCESS;	
	}

	return CIPHER_ERROR;
}

/*********************************************************************
 * Function:        BOOL DataEncrypt(IOPUT BYTE *Input, IOPUT BYTE *DataLen, INPUT BYTE *Header, INPUT BYTE HeaderLen)
 *
 * PreCondition:    Input and Header has been filled 
 *
 * Input:           BYTE *Header   - Point to MiWi header
 *                  BYTE HeaderLen - MiWi header length
 *
 * Output:          BOOL           - If data encryption successful
 *
 * Input/Output:    BYTE *Input    - Pointer to the data to be encrypted. The encrypted data will be write back to the pointer
 *                  BYTE *DataLen  - Input as the length of the data to be encrypted. The encrypted data length (including MICs) will be written back
 *
 * Side Effects:    Input data get encrypted and written back to the input pointer
 *
 * Overview:        This is the function that call the hardware cipher to encrypt input data
 ********************************************************************/
BOOL DataEncrypt(IOPUT BYTE *Input, IOPUT BYTE *DataLen, INPUT BYTE *Header, INPUT BYTE HeaderLen)
{
	SECURITY_INPUT SecurityInput;
	BYTE EncryptedLen;
	BYTE i;
	BYTE Counter;

	// operate the frame counter 
	SecurityInput.FrameCounter = OutgoingFrameCounter;
	OutgoingFrameCounter.Val++;

	// fill the source extended address
	SecurityInput.SourceAddress[0] = myLongAddress[0];
	SecurityInput.SourceAddress[1] = myLongAddress[1];
	SecurityInput.SourceAddress[2] = myLongAddress[2];
	SecurityInput.SourceAddress[3] = myLongAddress[3];
	SecurityInput.SourceAddress[4] = myLongAddress[4];
	SecurityInput.SourceAddress[5] = myLongAddress[5];
	SecurityInput.SourceAddress[6] = myLongAddress[6];
	SecurityInput.SourceAddress[7] = myLongAddress[7];
	
	// fill the pointer to header and header length
	SecurityInput.Header = Header;
	SecurityInput.HeaderLen = HeaderLen;
 
 	// in rare cases, the hardware encryption engine may not suceed for the 
 	// first time. Retry a few times will solve the problem
	Counter = CIPHER_RETRY;
	while(Counter)
	{
		// fill the input data and data length
		SecurityInput.InputText = Input;
		SecurityInput.TextLen = *DataLen;
		// call hardware cipher and store the output to temporary buffer
		PHYCipher(MODE_ENCRYPTION, SecurityInput, tmpBuf, &EncryptedLen);
		
		// try to decrypt the buffer to make sure that encryption is correct
		SecurityInput.InputText = tmpBuf;
		SecurityInput.TextLen = EncryptedLen;
		if( PHYCipher(MODE_DECRYPTION, SecurityInput, Input, &i) == CIPHER_SUCCESS )
		{
			break;
		}
		Counter--;
	} 

	// fill the frame counter
	for(i = 0; i < 4; i++)
	{
		Input[i] = SecurityInput.FrameCounter.v[i];
	}
	// fill the source extended address
	Counter = i;
	for(i = 0; i < 8; i++)
	{
		Input[Counter++] = SecurityInput.SourceAddress[i];
	}
	// fill the security key sequence number
	Input[Counter++] = myKeySequenceNumber;	
		
	// fill the encrypted data
	for(i = 0; i < EncryptedLen; i++)
	{
		Input[Counter++] = tmpBuf[i];
	}

	*DataLen = Counter;

	return TRUE;
}

/*********************************************************************
 * Function:        BOOL DataDecrypt(IOPUT BYTE *Input, IOPUT BYTE *DataLen, INPUT BYTE *Header, INPUT BYTE HeaderLen)
 *
 * PreCondition:    Input and Header has been filled 
 *
 * Input:           BYTE *Header   - Point to MiWi header
 *                  BYTE HeaderLen - MiWi header length
 *
 * Output:          BOOL           - If data encryption successful
 *
 * Input/Output:    BYTE *Input    - Pointer to the data to be decrypted. The decrypted data will be write back to the pointer
 *                  BYTE *DataLen  - Input as the length of the data to be decrypted. The encrypted data length (excluding MICs) will be written back
 *
 * Side Effects:    Input data get decrypted and written back to the input pointer
 *
 * Overview:        This is the function that call the hardware cipher to decrypt input data
 ********************************************************************/
BOOL DataDecrypt(IOPUT BYTE *Input, IOPUT BYTE *DataLen, INPUT BYTE *Header, INPUT BYTE HeaderLen)
{
	SECURITY_INPUT SecurityInput;
	BYTE i;
	BYTE Counter;
	DWORD_VAL	FrameCounter;
	BYTE KeySeq;

	// fill the frame counter
	Counter = 0;
	for(i = 0; i < 4; i++)
	{
		FrameCounter.v[i] = Input[Counter++];
	}
	// fill the source extended address
	for(i = 0; i < 8; i++)
	{
		tempLongAddress[i] = Input[Counter++];
	}
	// get security key sequence number
	KeySeq = Input[Counter++];
	
	// check if the security key sequence number match
	if( KeySeq != myKeySequenceNumber )
	{
		// if keys can be requested/transferred in application layer
		// request the security key here
		ConsolePutROMString((ROM char *)"Key Sequence Number fail.");
		return FALSE;
	}
	
	// fill the security input
	SecurityInput.FrameCounter = FrameCounter;
	SecurityInput.InputText = &(Input[Counter]);
	SecurityInput.TextLen = *DataLen - Counter;
	SecurityInput.Header = Header;
	SecurityInput.HeaderLen = HeaderLen;
	SecurityInput.SourceAddress[0] = tempLongAddress[0];
	SecurityInput.SourceAddress[1] = tempLongAddress[1];
	SecurityInput.SourceAddress[2] = tempLongAddress[2];
	SecurityInput.SourceAddress[3] = tempLongAddress[3];
	SecurityInput.SourceAddress[4] = tempLongAddress[4];
	SecurityInput.SourceAddress[5] = tempLongAddress[5];
	SecurityInput.SourceAddress[6] = tempLongAddress[6];
	SecurityInput.SourceAddress[7] = tempLongAddress[7];

	// call hardware cipher
	if( PHYCipher(MODE_DECRYPTION, SecurityInput, Input, DataLen) != CIPHER_SUCCESS )
	{
		return FALSE;
	}
	
	// check the frame counter, make sure that the frame counter increase always
	// we only check family members, because only family members know if a node 
	// join or leave the network to reset the frame counter
	i = SearchForLongAddress();
	if( (i != 0xFF) && networkStatus[i].bits.isFamily )
	{
		if( FrameCounter.Val < networkTable[i].IncomingFrameCounter.Val )
		{
			/*
			ConsolePutROMString((ROM char *)"Frame counter test fail\r\n");
			PrintChar(FrameCounter.v[3]);
			PrintChar(FrameCounter.v[2]);
			PrintChar(FrameCounter.v[1]);
			PrintChar(FrameCounter.v[0]);
			ConsolePutROMString((ROM char *)" vs ");
			PrintChar(networkTable[i].IncomingFrameCounter.v[3]);
			PrintChar(networkTable[i].IncomingFrameCounter.v[2]);
			PrintChar(networkTable[i].IncomingFrameCounter.v[1]);
			PrintChar(networkTable[i].IncomingFrameCounter.v[0]);
			*/
			return FALSE;
		}
		networkTable[i].IncomingFrameCounter.Val = FrameCounter.Val + 1;
	}
	
	
	return TRUE;
}

#endif

//******************************************************************************
// Interrupt Vectors
//******************************************************************************

#if defined(__18CXX)
#pragma code highVector=0x08
void HighVector (void)
{
    _asm goto HighISR _endasm
}
#pragma code /* return to default code section */
#endif

#if defined(__18CXX)
#pragma code lowhVector=0x18
void LowVector (void)
{
    _asm goto HighISR _endasm
}
#pragma code /* return to default code section */
#endif
