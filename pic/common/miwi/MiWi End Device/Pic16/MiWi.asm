;*********************************************************************
;* FileName:		MiWi.asm
;* Dependencies:    none
;* Processor:   	PIC18F877A
;* Hardware:		PICDEM Z
;* Assembler:	    MPASMWIN 5.01 or higher
;* Linker:		    MPLINK 4.05 or higher
;* Company:		    Microchip Technology, Inc.
;*
;* Copyright and Disclaimer Notice for MiWi Software:
;*
;* Microchip Technology Inc. (“Microchip”) licenses this software to you 
;* solely for use with Microchip products, as described in the license 
;* agreement accompanying this software (“License”). The software is owned 
;* by Microchip, and is protected under applicable copyright laws.  
;* All rights reserved.
;* 
;* Distribution of this software (in object code or source code) is not 
;* permitted, except as described in Section 2 of the License.
;* 
;* SOFTWARE IS PROVIDED “AS IS.”  MICROCHIP EXPRESSLY DISCLAIM ANY 
;* WARRANTY OF ANY KIND, WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT 
;* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
;* PARTICULAR PURPOSE, OR NON-INFRINGEMENT. IN NO EVENT SHALL MICROCHIP 
;* BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, 
;* LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, 
;* TECHNOLOGY OR SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT 
;* LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, 
;* OR OTHER SIMILAR COSTS.
;*
;********************************************************************/
;* File Description:
;*   This file contains the MRF24J40 interface and the MiWi stack.
;* 
;* Change History:
;*  Rev   Date         Description
;*  0.1   11/09/2006   Initial revision
;*  1.0   01/15/2007   Initial release
;********************************************************************/
#define MIWI_ASM

;************************* INCLUDE FILES ****************************
	#include "MiWiDefs.inc"
	#include "MiWi.inc"
	#include "Console.inc"
	#include "SymbolTime.inc"

;************************* VARIABLES ********************************
SECT_MiWiRxBuffer UDATA 0x110
RxBuffer res RX_BUFFER_SIZE

SECT_MiWiTxBuffer UDATA 0x190
TxBuffer res TX_BUFFER_SIZE

SECT_MiWiNetworkTable UDATA 0x0A0
NetworkTable res (d'13'*NETWORK_TABLE_SIZE)
; | LSB - Status (1 byte) | PANID (2 bytes) | Short Address (2 bytes) | Long Address (8 bytes)                                                                   |
; | LSB - Status (1 byte) | PANID (2 bytes) | Short Address (2 bytes) | Channel (1 byte) | RSSI (1 byte) | Protocol (1 byte) | Version (1 byte) | Flags (1 byte) |
; Status = 
;     isFamily (bit 7) | RXOnWhenIdle | 0 | P2PConnection | shortAddressValid | longAddressValid | directConnection | isValid


SECT_MiWiVars           UDATA
;------------           --------
headerSize              res 0x01
packetSize              res 0x01
MACSeqNum               res 0x01
MiWiStatus              res 0x01
i                       res 0x01
MRFStatus               res 0x01
MiWiTasksVar            res 0x01
searchStatus            res 0x01
timeStamp               res 0x04
parentShortAddress      res 0x02
parentPANID             res 0x02
parentChannel           res 0x01
parentRSSI              res 0x01
myShortAddress          res 0x02
sourcePANID             res 0x02
sourceShortAddress      res 0x02
sourceLongAddress       res 0x08
sourceAddressInfo       res 0x01
MiWiSourceSeqNum        res 0x01
MiWiSourcePANID         res 0x02
MiWiSourceShortAddress  res 0x02
MiWiSeqNum              res 0x01
RxPacketAddress         res 0x01
tempFSR                 res 0x01
MRF_ADDRL           	res 0x01
MRF_ADDRH           	res 0x01
MRF_DATA            	res 0x01
#ifdef ENABLE_EUI_ADDRESS_SEARCH
MiWiSearchStatus        res 0x01
#endif
#ifdef ENABLE_CLUSTER_SOCKETS
MiWiSocketStatus        res 0x01
MiWiSocketHandle        res 0x01
#endif


;************************* CODE *************************************
SECT_MiWiCode CODE

;********************************************************************
; Function: MiWiTasks
;
; PreCondition:  none
;
; Input:  none     
;
; Output:  none
;
; Side Effects: stack my set or clear status bits about the network
;   connectivity.  stack my cause a transmission(s) to occur. 
;
; Stack Requirements:
;
; Overview: this is the main task controller for the MiWi stack.  This
;   function is responsible for joining, leaving, and finding networks
;   This function also is responsible for interfacing to the MRF24J40.
;   All of the optional features are also supported here.
;*********************************************************************
MiWiTasks
    ;read out the status of the MRF24J40
    call UpdateStatus
    clrwdt

    ;Check to see if we still have networking searchign tasks
    btfsc MiWiTasksVar,SEARCH_FOR_NETWORK
    goto _MiWiTasksSearchForNetwork
    goto _MiWiTasksSearchForNetworkEnd
_MiWiTasksSearchForNetwork
    ;we are looking for a network, have we sent out a Beacon request?
    btfsc searchStatus,BEACON_REQUEST_SENT
    goto _MiWiTaskBeaconSent

    ;if not then send one
	movlw READ_RFCTRL0_L
	movwf MRF_ADDRL
	movlw READ_RFCTRL0_H
	movwf MRF_ADDRH
	call PHYLongRAMAddr

    ;determine which channel to search
    movf searchStatus,W
    andlw 0xF0
    btfsc STATUS,Z 
    goto _MiWiTasksSearchChannel_11
    addlw 0xF0
    btfsc STATUS,Z
    goto _MiWiTasksSearchChannel_12
    addlw 0xF0
    btfsc STATUS,Z
    goto _MiWiTasksSearchChannel_13
    addlw 0xF0
    btfsc STATUS,Z
    goto _MiWiTasksSearchChannel_14
    addlw 0xF0
    btfsc STATUS,Z
    goto _MiWiTasksSearchChannel_15
    addlw 0xF0
    btfsc STATUS,Z
    goto _MiWiTasksSearchChannel_16
    addlw 0xF0
    btfsc STATUS,Z
    goto _MiWiTasksSearchChannel_17
    addlw 0xF0
    btfsc STATUS,Z
    goto _MiWiTasksSearchChannel_18
    addlw 0xF0
    btfsc STATUS,Z
    goto _MiWiTasksSearchChannel_19
    addlw 0xF0
    btfsc STATUS,Z
    goto _MiWiTasksSearchChannel_20
    addlw 0xF0
    btfsc STATUS,Z
    goto _MiWiTasksSearchChannel_21
    addlw 0xF0
    btfsc STATUS,Z
    goto _MiWiTasksSearchChannel_22
    addlw 0xF0
    btfsc STATUS,Z
    goto _MiWiTasksSearchChannel_23
    addlw 0xF0
    btfsc STATUS,Z
    goto _MiWiTasksSearchChannel_24
    addlw 0xF0
    btfsc STATUS,Z
    goto _MiWiTasksSearchChannel_25
    addlw 0xF0
    btfsc STATUS,Z
    goto _MiWiTasksSearchChannel_26   
    goto _MiWiTasksChannelSearchComplete
    
_MiWiTasksSearchChannel_11
    #ifdef SCAN_CHANNEL_11
        goto _MiWiTasksSearchSendBeacon
    #else
        goto _MiWiTasksBeaconReqTimeout
    #endif
_MiWiTasksSearchChannel_12
    #ifdef SCAN_CHANNEL_12
        goto _MiWiTasksSearchSendBeacon
    #else
        goto _MiWiTasksBeaconReqTimeout
    #endif
_MiWiTasksSearchChannel_13
    #ifdef SCAN_CHANNEL_13
        goto _MiWiTasksSearchSendBeacon
    #else
        goto _MiWiTasksBeaconReqTimeout
    #endif
_MiWiTasksSearchChannel_14
    #ifdef SCAN_CHANNEL_14
        goto _MiWiTasksSearchSendBeacon
    #else
        goto _MiWiTasksBeaconReqTimeout
    #endif
_MiWiTasksSearchChannel_15
    #ifdef SCAN_CHANNEL_15
        goto _MiWiTasksSearchSendBeacon
    #else
        goto _MiWiTasksBeaconReqTimeout
    #endif
_MiWiTasksSearchChannel_16
    #ifdef SCAN_CHANNEL_16
        goto _MiWiTasksSearchSendBeacon
    #else
        goto _MiWiTasksBeaconReqTimeout
    #endif
_MiWiTasksSearchChannel_17
    #ifdef SCAN_CHANNEL_17
        goto _MiWiTasksSearchSendBeacon
    #else
        goto _MiWiTasksBeaconReqTimeout
    #endif
_MiWiTasksSearchChannel_18
    #ifdef SCAN_CHANNEL_18
        goto _MiWiTasksSearchSendBeacon
    #else
        goto _MiWiTasksBeaconReqTimeout
    #endif
_MiWiTasksSearchChannel_19
    #ifdef SCAN_CHANNEL_19
        goto _MiWiTasksSearchSendBeacon
    #else
        goto _MiWiTasksBeaconReqTimeout
    #endif
_MiWiTasksSearchChannel_20
    #ifdef SCAN_CHANNEL_20
        goto _MiWiTasksSearchSendBeacon
    #else
        goto _MiWiTasksBeaconReqTimeout
    #endif
_MiWiTasksSearchChannel_21
    #ifdef SCAN_CHANNEL_21
        goto _MiWiTasksSearchSendBeacon
    #else
        goto _MiWiTasksBeaconReqTimeout
    #endif
_MiWiTasksSearchChannel_22
    #ifdef SCAN_CHANNEL_22
        goto _MiWiTasksSearchSendBeacon
    #else
        goto _MiWiTasksBeaconReqTimeout
    #endif
_MiWiTasksSearchChannel_23
    #ifdef SCAN_CHANNEL_23
        goto _MiWiTasksSearchSendBeacon
    #else
        goto _MiWiTasksBeaconReqTimeout
    #endif
_MiWiTasksSearchChannel_24
    #ifdef SCAN_CHANNEL_24
        goto _MiWiTasksSearchSendBeacon
    #else
        goto _MiWiTasksBeaconReqTimeout
    #endif
_MiWiTasksSearchChannel_25
    #ifdef SCAN_CHANNEL_25
        goto _MiWiTasksSearchSendBeacon
    #else
        goto _MiWiTasksBeaconReqTimeout
    #endif
_MiWiTasksSearchChannel_26
    #ifdef SCAN_CHANNEL_26
        goto _MiWiTasksSearchSendBeacon
    #else
        goto _MiWiTasksBeaconReqTimeout
    #endif

_MiWiTasksSearchSendBeacon
    clrwdt
    ;send the beacon
	call LoadBeaconRequest
	call TransmitPacket
	
	;get the current time
    movlw high(TickGet)
    movwf PCLATH
    call TickGet
    movlw high($)
    movwf PCLATH
    
    ;add NETWORK_SEARCH to the current and save
    ;  it as the expiration time
    movf SymbolTime+0,W
    addlw NETWORK_SEARCH_0
    movwf timeStamp+0
    movf SymbolTime+1,W
    btfsc STATUS,C
    addlw 0x01
    addlw NETWORK_SEARCH_1
    movwf timeStamp+1
    movf SymbolTime+2,W
    btfsc STATUS,C
    addlw 0x01
    addlw NETWORK_SEARCH_2
    movwf timeStamp+2
    movf SymbolTime+3,W
    btfsc STATUS,C
    addlw 0x01
    addlw NETWORK_SEARCH_3
    movwf timeStamp+3
    bsf searchStatus,BEACON_REQUEST_SENT
    
_MiWiTaskBeaconSent
    ;if the beacon was already sent;
    ;get the current time
    clrwdt
    movlw high(TickGet)
    movwf PCLATH
    call TickGet
    movlw high($)
    movwf PCLATH
    
    ;compare the current time to the saved time
    movf SymbolTime+0,W
    subwf timeStamp+0,W
    movf SymbolTime+1,W
    btfss STATUS,C
    addlw 0x01
    subwf timeStamp+1,W
    movf SymbolTime+2,W
    btfss STATUS,C
    addlw 0x01
    subwf timeStamp+2,W
    movf SymbolTime+3,W
    btfss STATUS,C
    addlw 0x01
    subwf timeStamp+3,W
    btfss STATUS,C
    goto _MiWiTasksBeaconReqTimeout         ;time is up
    goto _MiWiTasksSearchForNetworkEnd   
    
_MiWiTasksBeaconReqTimeout 
    ;the search time is complete for this channel
    bcf searchStatus,BEACON_REQUEST_SENT
    ;see if we need searched all of the channels
    movf searchStatus,W
    andlw 0xF0
    xorlw 0xF0
    btfsc STATUS,Z
    goto _MiWiTasksChannelSearchComplete
    ;if we haven't searched all channels
    ;  then switch to the next channel
    movf searchStatus,W
    addlw 0x10
    movwf searchStatus
    iorlw 0x02
    movwf MRF_DATA
    call SetChannel
    goto _MiWiTasksSearchForNetworkEnd
    
_MiWiTasksChannelSearchComplete
    bcf MiWiTasksVar,SEARCH_FOR_NETWORK
   
_MiWiTasksSearchForNetworkEnd
    ;if we aren't searching for a network
    ;  then see if we have a packet waiting for us   
    clrwdt
    btfsc MiWiStatus,PACKET_RECEIVED
    ;if there is a packet then process it
    call ProcessPacket

    ;if we are in the middle of a join process
    btfss MiWiTasksVar,JOIN_NETWORK
    goto _MiWiTasksJoinEnd
    
    ;if we have already sent the association request
    btfsc MiWiTasksVar,ASSOC_REQ_SENT
    goto _MiWiTasksJoinDataRequest
    
    ;indicate that the association request was sent
    bsf MiWiTasksVar,ASSOC_REQ_SENT
    
    ;get the current time
    movlw high(TickGet)
    movwf PCLATH
    call TickGet
    movlw high($)
    movwf PCLATH
    
    ;add ASSOC_REQ_WAIT to the current time
    ;  and save this as the expiration time
    movf SymbolTime+0,W
    addlw ASSOC_REQ_WAIT_0
    movwf timeStamp+0
    movf SymbolTime+1,W
    btfsc STATUS,C
    addlw 0x01
    addlw ASSOC_REQ_WAIT_1
    movwf timeStamp+1
    movf SymbolTime+2,W
    btfsc STATUS,C
    addlw 0x01
    addlw ASSOC_REQ_WAIT_2
    movwf timeStamp+2
    movf SymbolTime+3,W
    btfsc STATUS,C
    addlw 0x01
    addlw ASSOC_REQ_WAIT_3
    movwf timeStamp+3
    
_MiWiTasksJoinDataRequest
    clrwdt
    ;wait before sending the data request
    movlw high(TickGet)
    movwf PCLATH
    call TickGet
    movlw high($)
    movwf PCLATH
    
    ;check to see if the time has expired
    movf SymbolTime+0,W
    subwf timeStamp+0,W
    movf SymbolTime+1,W
    btfss STATUS,C
    addlw 0x01
    subwf timeStamp+1,W
    movf SymbolTime+2,W
    btfss STATUS,C
    addlw 0x01
    subwf timeStamp+2,W
    movf SymbolTime+3,W
    btfss STATUS,C
    addlw 0x01
    subwf timeStamp+3,W
    btfss STATUS,C
    goto _MiWiTasksJoinDataRequestTimeout
    goto _MiWiTasksJoinEnd  
    
_MiWiTasksJoinDataRequestTimeout
    clrwdt
    ;if time has expired on the wait before senting
    ;  and we haven't sent it already
    btfsc MiWiTasksVar,JOIN_DATA_REQ_SENT
    goto _MiWiTasksJoinDataRequestCheck   
    
    ;set my PANID to my new potential parents PANID
    movlw WRITE_PANIDL
    movwf MRF_ADDRL
    movf parentPANID,w
    movwf MRF_DATA
    call PHYShortRAMAddr
    movlw WRITE_PANIDH
    movwf MRF_ADDRL
    movf parentPANID+1,w
    movwf MRF_DATA
    call PHYShortRAMAddr
       
    ;time to send the data request
    movlw DATA_REQ_LONG_ADDR
    movwf MRF_DATA
    call LoadDataRequest
    call TransmitPacket
    
    ;mark as having sent the data request
    bsf MiWiTasksVar,JOIN_DATA_REQ_SENT
    goto _MiWiTasksJoinEnd
    
_MiWiTasksJoinDataRequestCheck
    ;if the data request was already sent
    btfss MiWiStatus,DATA_REQ_IN_PROGRESS
    ;and has finished then make the join process complete
    ;  (it has either passed or failed)
    bcf MiWiTasksVar,JOIN_NETWORK
    
_MiWiTasksJoinEnd
    clrwdt
    ;if we are in the middle of a rejoin process
    btfss MiWiTasksVar,REJOIN_NETWORK
    goto _MiWiTasksRejoinEnd
    
    ;get the current time
    movlw high(TickGet)
    movwf PCLATH
    call TickGet
    movlw high($)
    movwf PCLATH
    
    ;compare that to the expiration time for the rejoin
    movf SymbolTime+0,W
    subwf timeStamp+0,W
    movf SymbolTime+1,W
    btfss STATUS,C
    addlw 0x01
    subwf timeStamp+1,W
    movf SymbolTime+2,W
    btfss STATUS,C
    addlw 0x01
    subwf timeStamp+2,W
    movf SymbolTime+3,W
    btfss STATUS,C
    addlw 0x01
    subwf timeStamp+3,W
    btfss STATUS,C
    goto _MiWiTasksRejoinTimeout
    goto _MiWiTasksRejoinEnd
    
_MiWiTasksRejoinTimeout
    ;if the rejoin has timed-out
    ;  then clear up the status bits
    bcf MiWiTasksVar,REJOIN_NETWORK
    bcf MiWiStatus,NETWORK_MEMBER
        
_MiWiTasksRejoinEnd
    clrwdt
    ;if the rejoin hasn't timed out yet
    ;  if we have already sent a data request
    btfss MiWiStatus,DATA_REQ_IN_PROGRESS
    goto _MiWiTasksDataReqComplete
    
    ;get the current time
    movlw high(TickGet)
    movwf PCLATH
    call TickGet
    movlw high($)
    movwf PCLATH
    
    ;see if the data request has timed-out yet or not
    movf SymbolTime+0,W
    subwf timeStamp+0,W
    movf SymbolTime+1,W
    btfss STATUS,C
    addlw 0x01
    subwf timeStamp+1,W
    movf SymbolTime+2,W
    btfss STATUS,C
    addlw 0x01
    subwf timeStamp+2,W
    movf SymbolTime+3,W
    btfss STATUS,C
    addlw 0x01
    subwf timeStamp+3,W
    btfss STATUS,C
    ;if it has timed out then clear the in progress bit
    bcf MiWiStatus,DATA_REQ_IN_PROGRESS         
_MiWiTasksDataReqComplete
_MiWiTasksComplete
    return


;********************************************************************
; Function: UpdateStatus
;
; PreCondition: none
;
; Input: none      
;
; Output: none
;
; Side Effects: The MiWiStatus status register has changed
;
; Stack Requirements:  level deep
;
; Overview: This function checks the status of the MRF24J40 to
;   determine if a transmission was successful or if a packet was
;   received.
;*********************************************************************
UpdateStatus
    banksel 0
       
    btfsc MiWiStatus,PACKET_RECEIVED
    ;if we already have a packet that hasn't been processed yet either 
    ;by the stack or by the customer then refuse to check the status
    ;again.  Checking again could result in having to buffer a new packet
    ;that was received when the old one hadn't been processed yet.
    return 
    
    ;Read the ISRSTS register
    movlw READ_ISRSTS
    movwf MRF_ADDRL
    call PHYShortRAMAddr
    
    ;Store the results in MRFStatus register
    movwf MRFStatus
    
    ;Need to handle all of the MRFStatus bits otherwise next time
    ;I read it this flag will be cleared
    
    ;If the TX flag is set then indicate that the
    ;transmission was successful.  
    btfss MRFStatus,0
    goto _TXIFNotSet
    
    ;The TX is no longer busy
    bcf MiWiStatus,TX_BUSY
       
    ;read the TX status
    movlw READ_TXSR
    movwf MRF_ADDRL
    call PHYShortRAMAddr
    
    ;indicate it was a failure
    bcf MiWiStatus,TX_SUCCESSFUL
    andlw 0x01
    ;If STATUS,Z is set (0x01 & TXSR == 0) then it was a success
    btfsc STATUS,Z
    ;If the status says it was successful then change the results to
    ;  sucessful
    bsf MiWiStatus,TX_SUCCESSFUL

_TXIFNotSet
    btfsc MRFStatus,3
    ;If we received a packet then handle it.
    goto _HandleRx
    ;otherwise we are done with the MRF for now
    goto _HandleRxDone
_HandleRx
    ;read the first byte of the RX buffer (packet size)
    movlw b'11100000'
    movwf MRF_ADDRH
    movlw b'00000000'
    movwf MRF_ADDRL
	call PHYLongRAMAddr
    movwf packetSize
    
    ;subtract 2 such that we don't read the FCS
    addlw 0xFE
    btfss STATUS,C
    goto _HandleRxFlush
    
    ;Point to the RX buffer
    movlw RxBuffer
    movwf FSR
    bsf STATUS,IRP

    ;read out the packet size
    movf packetSize,w
    movwf INDF
    incf FSR,f
    
_HandleRxReadLoop   
    ;read all of the bytes in the RX buffer in the MRF24J40
    ;  into the RAM buffer in the microcontroller
	movlw b'00100000'
	addwf MRF_ADDRL,f
	btfsc STATUS,C
	incf MRF_ADDRH,f
    call PHYLongRAMAddr 
    movwf INDF
    incf FSR,F
	decf packetSize,f
	btfss STATUS,Z
	goto _HandleRxReadLoop
		
	;read LQI
    movlw b'01000000'		
    addwf MRF_ADDRL,f
	btfsc STATUS,C
	incf MRF_ADDRH,f
    call PHYLongRAMAddr 
    movwf INDF
    incf FSR,F
    
    ;read RSSI
	movlw b'00100000'
	addwf MRF_ADDRL,f
	btfsc STATUS,C
	incf MRF_ADDRH,f
    call PHYLongRAMAddr 
    movwf INDF
    
    ;Mark to indicate we received a packet
	bsf MiWiStatus,PACKET_RECEIVED
	
	;We are done with the RX buffer so flush it
_HandleRxFlush
    movlw WRITE_RXFLUSH
    movwf MRF_ADDRL
    movlw b'00000001'
    movwf MRF_DATA
    call PHYShortRAMAddr
 
_HandleRxDone
_UpdateStatusComplete
    return
    
;********************************************************************
; Function: LoadBeaconRequest
;
; PreCondition: none
;
; Input: none      
;
; Output: none
;
; Side Effects: The TX buffer is loaded with a beacon packet and the
;   FSR pointer has changed
;
; Stack Requirements:  level deep
;
; Overview: The function loads the TX buffer with a beacon request
;*********************************************************************
LoadBeaconRequest
    ;load a pointer to the TxBuffer
    movlw TxBuffer
    movwf FSR
    bsf STATUS,IRP

    ;header size
    movlw 0x08
    movwf INDF
    incf FSR,F
    
    ;packet size
    movlw 0x08
    movwf INDF
    incf FSR,F
    
    ;frame control
    movlw 0x03
    movwf INDF
    incf FSR,F
    
    movlw 0x08
    movwf INDF
    incf FSR,F
    
    ;sequence number
    banksel MACSeqNum
    movf MACSeqNum,W
    movwf INDF
    incf FSR,F
    incf MACSeqNum,F
    
    ;destination PAN
    movlw 0xFF
    movwf INDF
    incf FSR,F
    
    movlw 0xFF
    movwf INDF
    incf FSR,F
    
    ;destination address
    movlw 0xFF
    movwf INDF
    incf FSR,F
    
    movlw 0xFF
    movwf INDF
    incf FSR,F
    
    ;beacon request
    movlw 0x07
    movwf INDF
    incf FSR,F
    
    return
    
    	
;********************************************************************
; Function: TransmitPacket
;
; PreCondition: TX buffer is full of the packet that needs to be sent
;
; Input: none                  
;
; Output: none
;
; Side Effects: MRF24J40 will because busy transmitting if not already
;   transmitting.
;
; Stack Requirements: level deep
;
; Overview: This will take the packet located in the TX buffer and
;   transmit it.  
;*********************************************************************
TransmitPacket
    clrwdt
    ;check to see if this packet needs an ACK or not
    movlw 0x01
    banksel TxBuffer
    ;access the LSB frame control byte
    btfsc TxBuffer+2,5
    ;if ack bit is set then mark that we need to send an ACK
    ;This corresponds to the TXNMTRIG register in the MRF24J40
    iorlw 0x04

	banksel 0
	;recort the transmission requirements into i
	movwf i
	
	;point to the TxBuffer
	movlw TxBuffer
	movwf FSR
	bsf STATUS,IRP
	
	;load the header size
	movlw b'10000000'
	movwf MRF_ADDRH
	movlw b'00010000'
	movwf MRF_ADDRL
	movf INDF,w
	movwf headerSize
	movwf MRF_DATA
	call PHYLongRAMAddr
	incf FSR,F
	
	;load  the packet size
	movlw b'10000000'
	movwf MRF_ADDRH
	movlw b'00110000'
	movwf MRF_ADDRL
	movf INDF,w
	movwf packetSize
	movwf MRF_DATA
	call PHYLongRAMAddr
	incf FSR,F
		
	movlw b'10000000'
	movwf MRF_ADDRH
	movlw b'01010000'
	movwf MRF_ADDRL
	
_MiWiTransmitPacketLoop
    ;load the rest of the packet
	movf INDF,w
	movwf MRF_DATA
	call PHYLongRAMAddr
	incf FSR,f
	movlw b'00100000'
	addwf MRF_ADDRL,f
	btfsc STATUS,C
	incf MRF_ADDRH,f
	decf packetSize,f
	btfss STATUS,Z
	goto _MiWiTransmitPacketLoop

    ;cause the transmission to occur
	movlw WRITE_TXNMTRIG	
	movwf MRF_ADDRL
	;take the variable "i" set earlier in this function
	;that holds the value of TXNMTRIG
	movf i,w
	movwf MRF_DATA
	call PHYShortRAMAddr
	    
	;mark the MRF24J40 as TX_BUSY
	bsf MiWiStatus,TX_BUSY	
	
	return
	


;********************************************************************
; Function: MiWiInit
;
; PreCondition: The MRF24J40 has been initialized
;
; Input: none
;
; Output: none
;
; Side Effects: 
;
; Stack Requirements: level deep
;
; Overview: This function initializes the stack variables required to
;   operate the stack.
;*********************************************************************
MiWiInit
    clrf MiWiStatus
    clrf MiWiTasksVar
    
    #ifdef ENABLE_CLUSTER_SOCKETS
        clrf MiWiSocketStatus
    #endif
    
    ;Clear the network table
    movlw NETWORK_TABLE_SIZE
    movwf i
    
    movlw NetworkTable
    movwf FSR
    bcf STATUS,IRP
    
    movlw d'13'
    
_ClearNetworkTableLoop
    clrf INDF
    addwf FSR,F
    decf i,F
    btfss STATUS,Z
    goto _ClearNetworkTableLoop
    ;Done clearing the network table
    
    movlw high(InitSymbolTimer)
    movwf PCLATH
    call InitSymbolTimer
    movlw high($)
    movwf PCLATH
    
    ;Reset myShortAddress to 0xFFFF
    movlw 0xFF
    movwf myShortAddress
    movwf myShortAddress+1
    return
	
;********************************************************************
; Function: StartNetworkSearch
;
; PreCondition: none
;
; Input: none
;
; Output: none
;
; Side Effects: the stack will start searching for a new parent.  
;   Association to old parent will be lost.
;
; Stack Requirements: level deep
;
; Overview: This fucntion will cause the stack to search for a new 
;   network.
;*********************************************************************
StartNetworkSearch
    banksel MiWiTasksVar
    ;force into search for network mode
    bsf MiWiTasksVar,SEARCH_FOR_NETWORK
    clrf parentRSSI    
    clrf searchStatus
    return
    
;********************************************************************
; Function: ProcessPacket
;
; PreCondition: This function should only be called by the MiWiStack
;
; Input: none
;
; Output: MiWiStatus,PACKET_RECEIVED is set if a packet arrives for
;   the user application to handle
;
; Side Effects: this may cause packets to be generated
;
; Stack Requirements: 1 level deep
;
; Overview: 
;*********************************************************************
ProcessPacket
    clrwdt
    ;point to the RX buffer
	banksel 0
	movlw RxBuffer+1
	movwf FSR
	bsf STATUS,IRP
	
	;check to see what type of packet it is
	movf INDF,W
	andlw b'00000111'
	
	;is it a beacon packet?
    btfsc STATUS,Z
    goto _ProcessPacketBeacon
    
    ;data packet?
    xorlw 0x01
    btfsc STATUS,Z
    goto _ProcessPacketDataPacket
    
    ;or MAC command packet?
    xorlw 0x02  ;only xor 0x02 because we already xored in 0x01
                ;thus Y ^ 0x01 ^ 0x03 = Y ^ (0x01 ^ 0x02) = Y ^ 0x03
    btfsc STATUS,Z
    goto _ProcessPacketMACCommand
    
    ;if it is none of the above then throw the packet away
    goto _ProcessPacketComplete
    
_ProcessPacketBeacon
    ;if MiWiTasksVar indicates that we are currently searching for a parent
    ;then lets process the packet
    btfss MiWiTasksVar,0
    ;otherwise throw away the packet because we aren't searching for a parent
    goto _ProcessPacketComplete
    
    ;start processing the beacon packet here
    ;Point to the RSSI value
    banksel 0x118
    ;Check the association permit flag in the superframe specification
    movf 0x119,W
    andlw 0x80
    ;if it is cleared (STATUS,Z is set) then forget this coordiantor,
    ;they aren't accepting joins
    btfsc STATUS,Z
    goto _ProcessPacketComplete
    
    ;Check the RSSI of this parent compared to other networks
    movf 0x120,W                    ;move RSSI to WREG
    banksel 0                       ;select bank 0 to access 
                                    ;  the parentRSSI register
    subwf parentRSSI,w              ;WREG = parentRSSI - WREG
    btfsc STATUS,C                  ;if the result borrowed then 
    goto _ProcessPacketComplete     ;  this parent is better
    
    bsf MiWiStatus,FOUND_NETWORK
    
    ;This is my new best parent so lets fill in their data
    movf 0x120,W                    ;move RSSI to WREG
    movwf parentRSSI
    movlw 0x03
    addwf FSR,F
    
    ;Save this coordinators PANID
    movf INDF,W
    movwf parentPANID+0
    incf FSR,F
    movf INDF,W
    movwf parentPANID+1
    ;Save this coordinators short address
    incf FSR,F
    movf INDF,W
    movwf parentShortAddress+0
    incf FSR,F
    movf INDF,W
    movwf parentShortAddress+1
    ;Save this coordinators channel
    movlw READ_RFCTRL0_L
    movwf MRF_ADDRL
    movlw READ_RFCTRL0_H
    movwf MRF_ADDRH
    call PHYLongRAMAddr
    movwf parentChannel
        
    ;Done recording this beacon
    goto _ProcessPacketComplete
    
_ProcessPacketMACCommand
    incf FSR,F          ;select the addressing part of frame control
    movlw 0x03          ;frame control + sequence number
                        ;  is the minimum header size
    movwf headerSize

    movf INDF,w         
    btfss STATUS,Z
    goto _ProcessPacketMACCommandNot0x00
    goto _ProcessPacketMACCommandSeqNum
_ProcessPacketMACCommandNot0x00
    ;check the dest address
    movlw 0x0C
    andwf INDF,w
    xorlw 0x0C
    btfss STATUS,Z
    goto _ProcessPacketMACComNotLADest
    ;this was a long dest address packet
    ;add to the header size
    ;8 bytes for the long address and 2 bytes for the PANID
    movlw d'10' 
    addwf headerSize,f
    ;don't need to save anything because we know it was for us.
    goto _ProcessPacketMACComDestDone
_ProcessPacketMACComNotLADest
    movlw 0x0C
    andwf INDF,w
    xorlw 0x08
    btfss STATUS,Z
    ;there are no MAC commands that allow no destination specified
    goto _ProcessPacketComplete     
    
    ;2 byte for the short address and 2 bytes for the PANID
    movlw d'4' 
    addwf headerSize,f
    
    movlw 0x05
    addwf FSR,f
    
    incf INDF,w
    btfsc STATUS,Z
    goto _ProcessPktMACComBroadcastCheck
    movlw -d'5'     ;reset the pointer to the frame control
    addwf FSR,f
    ;the lower byte is not 0xFF thus not a broadcast
    goto _ProcessPacketMACComDestDone   
_ProcessPktMACComBroadcastCheck
    ;lower byte is 0xFF thus the upper byte must 
    ;  be checked to see if it was a broadcast
    incf FSR,f
    incf INDF,w
    btfsc STATUS,Z
    goto _ProcessPktMACComBroadcastFound
    movlw -d'6'     ;reset the pointer to the frame control
    addwf FSR,f
    ;the lower byte is not 0xFF thus not a broadcast
    goto _ProcessPacketMACComDestDone   
_ProcessPktMACComBroadcastFound
    ;broadcast MAC commands don't exist for this implementation
    goto _ProcessPacketComplete
    
_ProcessPacketMACComDestDone
    ;check the source address
    movlw 0xC0
    andwf INDF,w
    xorlw 0xC0
    btfss STATUS,Z
    goto _ProcessPacketMACComNotLASource
    
    ;go to the first byte of the frame control
    decf FSR,f  
    btfsc INDF,6
    goto _ProcessPktMACComLANotIntraPAN
     
    movf headerSize,w
    ;add two for the PANID
    addlw 0x02  
    movwf headerSize
    
    ;fall through
_ProcessPktMACComLANotIntraPAN
    movf headerSize,w
    addwf FSR,f
    ;add 8 for the long address
    addlw 0x08  
    movwf headerSize
    
    ;store the long address in sourceLongAddress
    call ReadLongAddress
    goto _ProcessPacketMACComAddrModeDone
    
    ;if it wasn't a long address source mode 
_ProcessPacketMACComNotLASource
    movlw 0xC0
    andwf INDF,w
    xorlw 0x80
    btfss STATUS,Z
    goto _ProcessPacketMACComNoSource
       
    ;go to the first byte of the frame control
    decf FSR,f  
    btfsc INDF,6
    goto _ProcessPktMACComSANotIntraPAN
     
    movf headerSize,w
    ;add two for the PANID
    addlw 0x02  
    movwf headerSize
    
_ProcessPktMACComSANotIntraPAN
    movf headerSize,w
    addwf FSR,f
    ;add 2 for the short address
    addlw 0x02  
    movwf headerSize
    
    ;save the source short address
    movf INDF,w
    movwf sourceShortAddress+0
    incf FSR,f
    movf INDF,w
    movwf sourceShortAddress+1

    goto _ProcessPacketMACComAddrModeDone
    
_ProcessPacketMACComNoSource   
    movf headerSize,w
    addwf FSR,f
    ;fall through to _ProcessPacketMACComAddrModeDone
    ;goto _ProcessPacketMACComAddrModeDone 
_ProcessPacketMACComAddrModeDone
_ProcessPacketMACCommandSeqNum   
    incf FSR,f
    ;this will read out the MAC Command 
    movf INDF,W 
    
    ;find out what type of command it was
    addlw 0xFE
    btfsc STATUS,Z
    goto _ProcessPacketAssociationResp
    addlw 0xFF
    btfsc STATUS,Z
    goto _ProcessPacketDisassociation
    addlw 0xFB
    btfsc STATUS,Z
    goto _ProcessPacketCoordinatorRealign
    ;if it isn't one of the supported packet types then
    ;throw away the packet and exit the routine.
    goto _ProcessPacketComplete
    
_ProcessPacketAssociationResp  
    clrwdt
    ;record this short address as mine
    incf FSR,f
    movlw WRITE_SADRL
    movwf MRF_ADDRL
    movf INDF,w
    movwf myShortAddress
    movwf MRF_DATA
    call PHYShortRAMAddr
	
    incf FSR,f
    movlw WRITE_SADRH
    movwf MRF_ADDRL
    movf INDF,w
    movwf myShortAddress+1
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    ;Reset the PANID in the MRF
    movlw WRITE_PANIDL
    movwf MRF_ADDRL
    movf parentPANID,w
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    movlw WRITE_PANIDH
    movwf MRF_ADDRL
    movf parentPANID+1,w
    movwf MRF_DATA
    call PHYShortRAMAddr
        
    ;mark as having successfully joined the network
    incf FSR,f
    movf INDF,w
    bsf MiWiTasksVar,JOIN_SUCCESSFUL    ;indicate that the join was successful
    bsf MiWiStatus,NETWORK_MEMBER       ;indicate that we are a member of a network
    bcf MiWiTasksVar,JOIN_NETWORK       ;no longer searching for a network
    btfss STATUS,Z                      ;if the join was a failure 
    goto _ProcessPacketAssocRespFail
    
    ;store the long address in sourceLongAddress
    movlw -d'11'
    addwf FSR,F
    call ReadLongAddress
    
    ;point to the network table
    movlw NetworkTable
    movwf FSR
    bcf STATUS,IRP
    
    ;fill in the appropriate status bits into the network table
    movlw ((1<<STATUS_IS_FAMILY) | (1<<STATUS_RX_ON_WHEN_IDLE) | (1<<STATUS_SHORT_ADDRESS_VALID) | (1<<STATUS_LONG_ADDRESS_VALID) | (1<<STATUS_DIRECT_CONNECTION) | (1<<STATUS_IS_VALID))
    movwf INDF
    
    ;record the parents information
    incf FSR,f
    movf parentPANID,w
    movwf INDF
    incf FSR,f
    movf parentPANID+1,w
    movwf INDF
    incf FSR,f
    movf parentShortAddress+0,w
    movwf INDF
    incf FSR,f
    movf parentShortAddress+1,w
    movwf INDF
    
    call FillInLongAddress
    goto _ProcessPacketComplete
    
    ;if the association response was a failure
_ProcessPacketAssocRespFail
    clrf MiWiTasksVar
    
    ;indicate that we are NOT a member of a network
    bcf MiWiStatus,NETWORK_MEMBER       
    
    ;Reset the short address in the MRF
    movlw WRITE_SADRL
    movwf MRF_ADDRL
    movlw 0xFF
    movwf MRF_DATA
    ;Be careful here.  We do not return PCLATH for a while.  DO not insert 
    ; gotos, calls, or returns in this area
    call PHYShortRAMAddr
    
    movlw WRITE_SADRH
    movwf MRF_ADDRL
    movlw 0xFF
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    ;Reset the PANID in the MRF
    movlw WRITE_PANIDL
    movwf MRF_ADDRL
    movlw 0xFF
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    movlw WRITE_PANIDH
    movwf MRF_ADDRL
    movlw 0xFF
    movwf MRF_DATA
    call PHYShortRAMAddr
       
    ;Reset myShortAddress variable
    movlw 0xFF
    movwf myShortAddress
    movwf myShortAddress+1
    
    goto _ProcessPacketComplete
_ProcessPacketDisassociation
    movf sourceShortAddress+0,w
    xorwf parentShortAddress+0,w
    btfss STATUS,Z
    goto _ProcessPacketComplete ;first byte of address didn't match my parent so quit

    movf sourceShortAddress+1,w
    xorwf parentShortAddress+1,w
    btfss STATUS,Z
    goto _ProcessPacketComplete ;second byte of address didn't match my parent so quit
      
    ;this packet did come from my parent
    incf FSR,f
    movf INDF,w
    ;check to see if it is a coordinator asking us to leave
    xorlw 0x01      
    btfss STATUS,Z
    ;incorrect reason
    goto _ProcessPacketComplete     
    ;treat this the same was as we would if we failed to join
    goto _ProcessPacketAssocRespFail    
    
_ProcessPacketCoordinatorRealign
    btfss MiWiTasksVar,REJOIN_NETWORK
    ;we weren't looking for one so don't process it
    goto _ProcessPktRealignNotExpected
       
    ;we are expecting and got a coordinator realignment
    banksel RxBuffer
    movf RxBuffer+2,w
    andlw 0x0C
    xorlw 0x0C
    banksel 0
    btfss STATUS,Z
    ;incorrect destination, if this is in response to an orphan notification 
    ;then we would receive a 0xC as the destination addressing mode
    goto _ProcessPacketComplete 
    goto _ProcessPktCoordRealign
_ProcessPktRealignNotExpected
    ;we are not expecting a coordinator realignment but got one
    
    banksel RxBuffer
    movf RxBuffer+2,w
    andlw 0x0C
    xorlw 0x08
    banksel 0
    btfss STATUS,Z
    ;incorrect destination, for broadcast coordinator realignment we should
    ;get a 0x8 as the destination addressing mode
    goto _ProcessPacketComplete 

    movf sourceShortAddress+0,w
    xorwf parentShortAddress+0,w
    btfss STATUS,Z
    goto _ProcessPacketComplete ;first byte of address didn't match my parent so quit

    movf sourceShortAddress+1,w
    xorwf parentShortAddress+1,w
    btfss STATUS,Z
    goto _ProcessPacketComplete ;second byte of address didn't match my parent so quit
    
_ProcessPktCoordRealign
    clrwdt 
    incf FSR,f
    movf INDF,w
    movwf parentPANID
    incf FSR,f
    movf INDF,w
    movwf parentPANID+1
    incf FSR,f
    movf INDF,w
    movwf parentShortAddress
    incf FSR,f
    movf INDF,w
    movwf parentShortAddress+1    
    ;skip over the channel
    incf FSR,f  

    ;record this short address as mine
    incf FSR,f
    movlw WRITE_SADRL
    movwf MRF_ADDRL
    movf INDF,w
    movwf myShortAddress
    movwf MRF_DATA
    ;Be careful here.  We do not return PCLATH for several instructions.  Do not
    ;insert gotos, calls, or returns here.
    call PHYShortRAMAddr
    
    ;set the short address
    incf FSR,f
    movlw WRITE_SADRH
    movwf MRF_ADDRL
    movf INDF,w
    movwf myShortAddress+1
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    ;set the PANID
    movlw WRITE_PANIDL
    movwf MRF_ADDRL
    movf parentPANID,w
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    movlw WRITE_PANIDH
    movwf MRF_ADDRL
    movf parentPANID+1,w
    movwf MRF_DATA
    call PHYShortRAMAddr
        	
    ;point back to the long address
    movlw -d'15'
    addwf FSR,F
    ;read out the long address into sourceLongAddress
    call ReadLongAddress
    
    ;point to the network table
    movlw NetworkTable
    movwf FSR
    bcf STATUS,IRP
    
    ;mark the appropriate status bits
    movlw ((1<<STATUS_IS_FAMILY) | (1<<STATUS_RX_ON_WHEN_IDLE) | (1<<STATUS_SHORT_ADDRESS_VALID) | (1<<STATUS_LONG_ADDRESS_VALID) | (1<<STATUS_DIRECT_CONNECTION) | (1<<STATUS_IS_VALID))
    movwf INDF
    
    ;record the addressing information
    incf FSR,f
    movf parentPANID+0,w
    movwf INDF
    incf FSR,f
    movf parentPANID+1,w
    movwf INDF
    incf FSR,f
    movf parentShortAddress+0,w
    movwf INDF
    incf FSR,f
    movf parentShortAddress+1,w
    movwf INDF
    call FillInLongAddress
    
    ;mark the stack as part of the network    
    bsf MiWiStatus,NETWORK_MEMBER       ;indicate that we are a member of a network
    bcf MiWiTasksVar,REJOIN_NETWORK     ;no longer searching for a network
    goto _ProcessPacketComplete
    
    ;if it was a data packet
_ProcessPacketDataPacket
    clrwdt
    ;point to the MiWi portion of the packet
    movlw d'10'
    addwf headerSize,w
    addwf FSR,f

    ;record the MiWi information
    decf FSR,f
    movf INDF,w
    movwf MiWiSourceSeqNum

    decf FSR,f
    movf INDF,w
    movwf MiWiSourceShortAddress+1
    
    decf FSR,f
    movf INDF,w
    movwf MiWiSourceShortAddress+0
    
    decf FSR,f
    movf INDF,w
    movwf MiWiSourcePANID+1
    
    decf FSR,f
    movf INDF,w
    movwf MiWiSourcePANID+0
    
    movlw -d'5'
    addwf FSR,f
    btfss INDF,2
    goto _ProcessPacketDataPacketNoAck
    
    ;save the FSR value so we can continue parsing the packet
    ;after sending the ACK packet
    movf FSR,w
    movwf tempFSR   
       
    ;if an ACK was required then send a MiWi ACK
    call SendMiWiAck
    
    ;restore the FSR value
    movf tempFSR,w
    movwf FSR   
    
_ProcessPacketDataPacketNoAck
    movlw d'10'
    addwf FSR,f
    
    ;check to see what type of report it was
    movf INDF,w
    btfsc STATUS,Z
    goto _ProcessPacketStackReport
    ;if it is not a stack report then it is a user report 
    ;reset the pointer and return the packet to the user
    movlw -d'11'
    addwf FSR,w
    movwf RxPacketAddress
    goto _ProcessPacketUserReport
_ProcessPacketStackReport
    ;else if it was a zero then it was a stack report and we can process some data here
    
    incf FSR,f
    #ifdef ENABLE_CLUSTER_SOCKETS
        movlw 0x11
        xorwf INDF,w
        btfss STATUS,Z
        goto _ProcessPacketNotSocketResponse
        btfss MiWiSocketStatus,SOCKET_REQUEST_SENT
        ;if we didn't ask for a socket, don't accept one
        goto _ProcessPacketComplete     
        
        ;We asked for a socket, and got a response
        ;Mark as having successfully received a socket
        movlw (1<<SOCKET_RESPONSE_RECEIVED)
        movwf MiWiSocketStatus
        
        ;record the information for the device
        incf FSR,f
        call ReadLongAddress
        incf FSR,f        
        movf INDF,w
        movwf sourceShortAddress+0
        incf FSR,f
        movf INDF,w
        movwf sourceShortAddress+1
        movf parentPANID,w
        movwf sourcePANID
        movf parentPANID+1,w
        movwf sourcePANID+1
        
        ;look for the device in the network table
        call SearchNetworkTableByLongAddress
        movwf FSR
        movlw 0xFF
        xorwf FSR,w
        btfss STATUS,Z
        goto _ProcessPacketSocketUpdateNode
        ;an entry does not exist so we need to add a new entry
        ;search to see if there is an open slot
        call SearchNetworkTableOpenEntry
        movwf FSR
        movlw 0xFF
        xorwf FSR,w
        btfss STATUS,Z
        goto _ProcessPacketSocketSuccess
                
        ;if an empty entry does not exist then indicate the error
        movlw ((1<<SOCKET_NETWORK_TABLE_FULL) | (1<<SOCKET_RESPONSE_RECEIVED))
        movwf MiWiSocketStatus
        ;and throw away the rest of the packet.
        goto _ProcessPacketComplete
        
_ProcessPacketSocketSuccess         
        ;if we just created a new entry for the device
        ;point to that entry in the network table
        bcf STATUS,IRP
        movf FSR,w
        movwf MiWiSocketHandle
        
        ;SET the status information in the network table
        movlw ((1<<STATUS_SHORT_ADDRESS_VALID) | (1<<STATUS_LONG_ADDRESS_VALID) | (1<<STATUS_IS_VALID))
        movwf INDF
        
        ;mark the socket status
        movlw ((1<<SOCKET_CREATED) | (1<<SOCKET_RESPONSE_RECEIVED))
        movwf MiWiSocketStatus
        
_ProcessPktSocketAddressUpdatee 
        ;update the address information just in case the
        ;  address information is new or has changed since the 
        ;  last time the device talked to you.
        incf FSR,f
        movf parentPANID+0,w
        movwf INDF
        incf FSR,f
        movf parentPANID+1,w
        movwf INDF
        incf FSR,f
        movf sourceShortAddress+0,w
        movwf INDF
        incf FSR,f
        movf sourceShortAddress+1,w
        movwf INDF
        call FillInLongAddress
        
        ;we are done with the packet so let's go ahead and quit
        goto _ProcessPacketComplete
        
_ProcessPacketSocketUpdateNode
        ;if we already had the device and just need to update
        ;  some of the information
        bcf STATUS,IRP
        movf FSR,w
        movwf MiWiSocketHandle
        
        ;CHANGE the status bits as required
        movlw ((1<<STATUS_SHORT_ADDRESS_VALID) | (1<<STATUS_LONG_ADDRESS_VALID) | (1<<STATUS_IS_VALID))
        iorwf INDF,f
        
        ;set the socket information to reflect the new socket status
        movlw ((1<<SOCKET_CREATED) | (1<<SOCKET_RESPONSE_RECEIVED))
        movwf MiWiSocketStatus

        ;reuse the code from adding a new socket to set
        ;  the rest of the address information
        goto _ProcessPktSocketAddressUpdatee
        
_ProcessPacketNotSocketResponse
    #endif
    
    ;if the packet was an EUI address search response
    #ifdef ENABLE_EUI_ADDRESS_SEARCH
        movlw 0x21
        xorwf INDF,w
        btfss STATUS,Z
        goto _ProcessPktNotEUISearchResponse
        
        ;if we sent out a request
        btfss MiWiSearchStatus,STATUS_EUI_SEARCH_IN_PROGRESS
        ;  if not then don't process one.
        goto _ProcessPacketComplete
                
        ;read out the addressing information from the packet
        incf FSR,f
        call ReadLongAddress
        incf FSR,f
        movf INDF,w
        movwf sourcePANID+0       
        incf FSR,f
        movf INDF,w
        movwf sourcePANID+1
        incf FSR,f
        movf INDF,w
        movwf sourceShortAddress+0
        incf FSR,f
        movf INDF,w
        movwf sourceShortAddress+1
        
        ;look for the device in the network table
        call SearchNetworkTableByLongAddress
               
        ;store the results
        movwf i     
        incf i,w
        btfsc STATUS,Z
        goto _ProcessPacketEUIResultsNotFound
        
        ;Found this long address in our table.  Update the PANID and Short Address
        movf i,w
        movwf FSR
        bcf STATUS,IRP
        
        ;mark the status byte of the network table
        movlw ((1<<STATUS_SHORT_ADDRESS_VALID) | (1<<STATUS_LONG_ADDRESS_VALID) | (1<<STATUS_IS_VALID))
        iorwf INDF,f
        
        ;record the address information into the network table
        incf FSR,f
        movf sourcePANID+0,w
        movwf INDF      
        incf FSR,f
        movf sourcePANID+1,w
        movwf INDF
        incf FSR,f
        movf sourceShortAddress+0,w
        movwf INDF
        incf FSR,f
        movf sourceShortAddress+1,w
        movwf INDF
        goto _ProcessPacketComplete
        
        ;if we couldn't find an entry for that device
_ProcessPacketEUIResultsNotFound       
        ;search for an available location
        call SearchNetworkTableOpenEntry
        movwf FSR
        movlw 0xFF
        xorwf FSR,w
        btfsc STATUS,Z
        ;no room in the in the network table.  Throw away this packet.
        goto _ProcessPacketComplete
              
        ;if there was room then mark the status of that location
        movlw ((1<<STATUS_SHORT_ADDRESS_VALID) | (1<<STATUS_LONG_ADDRESS_VALID) | (1<<STATUS_IS_VALID))
        iorwf INDF,f
        
        ;record the addressing information
        incf FSR,f
        movf parentPANID+0,w
        movwf INDF
        incf FSR,f
        movf parentPANID+1,w
        movwf INDF
        incf FSR,f
        movf sourceShortAddress+0,w
        movwf INDF
        incf FSR,f
        movf sourceShortAddress+1,w
        movwf INDF
        call FillInLongAddress
        
        ;once we are done recording the info, throw away the packet
        goto _ProcessPacketComplete
    
_ProcessPktNotEUISearchResponse
        ;if it is none of the stack reports supported...
        goto _ProcessPacketComplete
    #endif
      
_ProcessPacketComplete
    banksel 0
    ;indicate that we have processed the packet already and it
    ;  is no longer valid
    bcf MiWiStatus,PACKET_RECEIVED
_ProcessPacketUserReport
    return
  
;********************************************************************
; Function: RejoinNetwork
;
; PreCondition: none
;
; Input: none
;
; Output: none
;
; Side Effects: the stack will entry orphan mode and will transmit 
;   packets as well as possibly switching channels
;
; Stack Requirements:  level deep
;
; Overview: This fuction will cause the device to perform an orphan
;   scan for a period of time.  
;*********************************************************************
RejoinNetwork
    banksel MiWiTasksVar
    ;mark that we are moving to an orphan mode
    bsf MiWiTasksVar,REJOIN_NETWORK
        
    ;get a timestamp of when the request was started
    movlw high(TickGet)
    movwf PCLATH
    call TickGet
    movlw high($)
    movwf PCLATH
    movf SymbolTime+0,W
    addlw ORPHAN_WAIT_0
    movwf timeStamp+0
    movf SymbolTime+1,W
    btfsc STATUS,C
    addlw 0x01
    addlw ORPHAN_WAIT_1
    movwf timeStamp+1
    movf SymbolTime+2,W
    btfsc STATUS,C
    addlw 0x01
    addlw ORPHAN_WAIT_2
    movwf timeStamp+2
    movf SymbolTime+3,W
    btfsc STATUS,C
    addlw 0x01
    addlw ORPHAN_WAIT_3
    movwf timeStamp+3
    
    ;switch to the correct channel
    movf parentChannel,w
    movwf MRF_DATA
    call SetChannel

    
    ;load a pointer to the TxBuffer
    movlw TxBuffer
    movwf FSR
    bsf STATUS,IRP

    ;header size
    movlw d'18'
    movwf INDF
    incf FSR,F
    
    ;packet size
    movlw d'18'
    movwf INDF
    incf FSR,F
    
    ;frame control
    movlw 0x03
    movwf INDF
    incf FSR,F
    
    movlw 0xC8
    movwf INDF
    incf FSR,F
    
    ;sequence number
    movf MACSeqNum,W
    movwf INDF
    incf FSR,F
    incf MACSeqNum,F
    
    ;destination PAN
    movlw 0xFF
    movwf INDF
    incf FSR,F
    
    movlw 0xFF
    movwf INDF
    incf FSR,F
    
    ;destination address
    movlw 0xFF
    movwf INDF
    incf FSR,F
    
    movlw 0xFF
    movwf INDF
    incf FSR,F
    
    ;source PANID - 0xFFFF
    movlw 0xFF
    movwf INDF
    incf FSR,F
    movwf INDF
    incf FSR,F
    
    ;source address - EUI
    movlw EUI_0
    movwf INDF
    incf FSR,F
    
    movlw EUI_1
    movwf INDF
    incf FSR,F
    
    movlw EUI_2
    movwf INDF
    incf FSR,F
    
    movlw EUI_3
    movwf INDF
    incf FSR,F
    
    movlw EUI_4
    movwf INDF
    incf FSR,F
    
    movlw EUI_5
    movwf INDF
    incf FSR,F
    
    movlw EUI_6
    movwf INDF
    incf FSR,F
    
    movlw EUI_7
    movwf INDF
    incf FSR,F
    
    ;orphan notification request
    movlw 0x06
    movwf INDF
    incf FSR,F
  
    ;send out the orphan notification
    call TransmitPacket
    return
      
;********************************************************************
; Function: JoinNetwork
;
; PreCondition: StartNetworkSearch has been called and run to
;   completion.
;
; Input: none
;
; Output: none
;
; Side Effects: the device will try to join the network.
;
; Stack Requirements:  level deep
;
; Overview: This function will cause the stack to try to join the 
;   network that was discovered in the StartNetworkSearch function.
;*********************************************************************
JoinNetwork
    banksel MiWiTasksVar  
    bsf MiWiTasksVar,JOIN_NETWORK
        
    ;switch to the correct channel
    movf parentChannel,w
    movwf MRF_DATA
    call SetChannel
    
    ;load a pointer to the TxBuffer
    movlw TxBuffer
    movwf FSR
    bsf STATUS,IRP

    ;header size
    movlw d'19'
    movwf INDF
    incf FSR,F
    
    ;packet size
    movlw d'19'
    movwf INDF
    incf FSR,F
    
    ;frame control
    movlw 0x23
    movwf INDF
    incf FSR,F
    
    movlw 0xC8
    movwf INDF
    incf FSR,F
    
    ;sequence number
    movf MACSeqNum,W
    movwf INDF
    incf FSR,F
    incf MACSeqNum,F
    
    ;destination PAN
    movf parentPANID,w
    movwf INDF
    incf FSR,F
    
    movf parentPANID+1,w
    movwf INDF
    incf FSR,F
    
    ;destination address
    movf parentShortAddress,w
    movwf INDF
    incf FSR,F
    
    movf parentShortAddress+1,w
    movwf INDF
    incf FSR,F
    
    ;source PANID - 0xFFFF
    movlw 0xFF
    movwf INDF
    incf FSR,F
    movwf INDF
    incf FSR,F
    
    ;source address - EUI
    movlw EUI_0
    movwf INDF
    incf FSR,F
    
    movlw EUI_1
    movwf INDF
    incf FSR,F
    
    movlw EUI_2
    movwf INDF
    incf FSR,F
    
    movlw EUI_3
    movwf INDF
    incf FSR,F
    
    movlw EUI_4
    movwf INDF
    incf FSR,F
    
    movlw EUI_5
    movwf INDF
    incf FSR,F
    
    movlw EUI_6
    movwf INDF
    incf FSR,F
    
    movlw EUI_7
    movwf INDF
    incf FSR,F
    
    ;association request
    movlw 0x01
    movwf INDF
    incf FSR,F
    
    ;association capabilities info
    movlw 0x80
    movwf INDF
    incf FSR,F
  
    ;transmit the packet that is loaded in the TX buffer
    call TransmitPacket
    return
    
;********************************************************************
; Function: LoadDataRequest
;
; PreCondition: none
;
; Input:   MRF_DATA = 0x00 for long address source, otherwise short
;
; Output: none
;
; Side Effects: this will cause the MRF24J40 to transmit a data 
;   request
;
; Stack Requirements: level deep
;
; Overview: This function causes the stack to send a data request
;   packet.  If MRF_DATA = 0x00 then it will be a long address source
;   data request packet used during association.  Otherwise it will
;   be a short address packet as used to poll a parent device for any
;   pending packets
;*********************************************************************    
LoadDataRequest
    ;load a pointer to the TxBuffer
    movlw TxBuffer
    movwf FSR
    bsf STATUS,IRP

    ;header size
    movlw d'10'  
    movf MRF_DATA,f 
    ;if long address mode then 16 bytes long, otherwise 10
    btfsc STATUS,Z  
    addlw d'6'   
    movwf INDF
    incf FSR,F
    ;packet size
    ;use the value already in WREG from the last calculation
    movwf INDF      
    incf FSR,F
    
    ;frame control
    movlw 0x63
    movwf INDF
    incf FSR,F
    
    movlw 0x88
    ;if long address mode then 0xC8, otherwise 0x88
    movf MRF_DATA,f 
    btfsc STATUS,Z
    iorlw 0xC8
    movwf INDF
    incf FSR,F
    
    ;sequence number
    banksel MACSeqNum
    movf MACSeqNum,W
    movwf INDF
    incf FSR,F
    incf MACSeqNum,F
      
    ;destination PANID
    movf parentPANID+0,w
    movwf INDF
    incf FSR,F
    
    movf parentPANID+1,w
    movwf INDF
    incf FSR,F
    
    ;destination Short Address
    movf parentShortAddress+0,w
    movwf INDF
    incf FSR,F
    
    movf parentShortAddress+1,w
    movwf INDF
    incf FSR,F
      
    movf MRF_DATA,w
    btfsc STATUS,Z
    goto _LoadDataRequestLong
    goto _LoadDataRequestShort
      
_LoadDataRequestLong
    ;source address (LONG)
    movlw EUI_0
    movwf INDF
    incf FSR,F

    movlw EUI_1
    movwf INDF
    incf FSR,F
    
    movlw EUI_2
    movwf INDF
    incf FSR,F    
    
    movlw EUI_3
    movwf INDF
    incf FSR,F
    
    movlw EUI_4
    movwf INDF
    incf FSR,F
    
    movlw EUI_5
    movwf INDF
    incf FSR,F
    
    movlw EUI_6
    movwf INDF
    incf FSR,F
    
    movlw EUI_7
    movwf INDF
    incf FSR,F
    
    goto _LoadDataRequestCommand
    
_LoadDataRequestShort
    ;source address (SHORT)
    movf myShortAddress,w
    movwf INDF
    incf FSR,F

    movf myShortAddress+1,w
    movwf INDF
    incf FSR,F
    
_LoadDataRequestCommand
    ;beacon request
    movlw 0x04
    movwf INDF
    incf FSR,F
    
    ;mark that the data request is in progress
    bsf MiWiStatus,DATA_REQ_IN_PROGRESS

    ;get the current time
    movlw high(TickGet)
    movwf PCLATH
    call TickGet
    movlw high($)
    movwf PCLATH
    
    ;add the data request delay to the current time
    ;  and save that as the time that this request expires
    movf SymbolTime+0,W
    addlw DATA_REQ_WAIT_0
    movwf timeStamp+0
    movf SymbolTime+1,W
    btfsc STATUS,C
    addlw 0x01
    addlw DATA_REQ_WAIT_1
    movwf timeStamp+1
    movf SymbolTime+2,W
    btfsc STATUS,C
    addlw 0x01
    addlw DATA_REQ_WAIT_2
    movwf timeStamp+2
    movf SymbolTime+3,W
    btfsc STATUS,C
    addlw 0x01
    addlw DATA_REQ_WAIT_3
    movwf timeStamp+3
    
    return
    	
;********************************************************************
; Function: OpenSocket
;
; PreCondition:    ENABLE_CLUSTER_SOCKETS defined
;
; Input: none
;
; Output: none
;
; Side Effects: MiWiSocketStatus and MiWiSocketHandle may change from
;   the effects of this function
;
; Stack Requirements:  level deep
;
; Overview: This function initiates the OpenSocket protocol.  This
;   will send a packet to the PAN coordinator in an attempt to find
;   a communication partner
;********************************************************************* 
#ifdef ENABLE_CLUSTER_SOCKETS
OpenSocket
    ;load a pointer to the TxBuffer
    movlw TxBuffer
    movwf FSR
    bsf STATUS,IRP

    ;header size
    movlw d'30'  
    movwf INDF
    incf FSR,F
    ;packet size
    movwf INDF
    incf FSR,F
    
    ;frame control
    movlw 0x61
    movwf INDF
    incf FSR,F
    
    movlw 0x88
    movf MRF_DATA,f
    movwf INDF
    incf FSR,F
    
    ;sequence number
    banksel MACSeqNum
    movf MACSeqNum,W
    movwf INDF
    incf FSR,F
    incf MACSeqNum,F
      
    ;destination PANID
    movf parentPANID+0,w
    movwf INDF
    incf FSR,F
    
    movf parentPANID+1,w
    movwf INDF
    incf FSR,F
    
    ;destination Short Address
    movf parentShortAddress+0,w
    movwf INDF
    incf FSR,F
    
    movf parentShortAddress+1,w
    movwf INDF
    incf FSR,F
      
    movf myShortAddress,w
    movwf INDF
    incf FSR,F

    movf myShortAddress+1,w
    movwf INDF
    incf FSR,F
    
    ;Start of the MiWi Header
    
    movlw 0x04      ;HOPS
    movwf INDF
    incf FSR,F
    
    movlw 0x06      ;MiWi FrameControl - ACK, Intra, no security
    movwf INDF
    incf FSR,F
    
    ;destination PANID
    movf parentPANID+0,w
    movwf INDF
    incf FSR,F
    
    movf parentPANID+1,w
    movwf INDF
    incf FSR,F
    
    ;destination address - PAN coordinator
    movlw 0x00
    movwf INDF
    incf FSR,F
    
    movlw 0x00
    movwf INDF
    incf FSR,F
    
    ;source PANID
    movf parentPANID+0,w
    movwf INDF
    incf FSR,F
    
    movf parentPANID+1,w
    movwf INDF
    incf FSR,F
    
    ;source address
    movf myShortAddress,w
    movwf INDF
    incf FSR,F

    movf myShortAddress+1,w
    movwf INDF
    incf FSR,F
    
    ;sequence number
    movf MiWiSeqNum,W
    movwf INDF
    incf FSR,F
    incf MiWiSeqNum,F
    
    ;Stack Report
    movlw 0x00
    movwf INDF
    incf FSR,F
    
    ;Cluster Socket Request
    movlw 0x10
    movwf INDF
    incf FSR,F

    ;source address (LONG)
    movlw EUI_0
    movwf INDF
    incf FSR,F

    movlw EUI_1
    movwf INDF
    incf FSR,F
    
    movlw EUI_2
    movwf INDF
    incf FSR,F    
    
    movlw EUI_3
    movwf INDF
    incf FSR,F
    
    movlw EUI_4
    movwf INDF
    incf FSR,F
    
    movlw EUI_5
    movwf INDF
    incf FSR,F
    
    movlw EUI_6
    movwf INDF
    incf FSR,F
    
    movlw EUI_7
    movwf INDF
    incf FSR,F
    
    ;indicate the socket request was sent, clear all other bits
    movlw (1<<SOCKET_REQUEST_SENT)
    movwf MiWiSocketStatus
    
    call TransmitPacket
    
    return
#endif    	

;********************************************************************
; Function: SearchNetworkTableByShortAddress
;
; PreCondition: ENABLE_SEARCH_NWK_TBL_BY_SHORT_ADDRESS
;
; Input: the short address to search for is located in sourcePANID
;   and sourceShortAddress
;
; Output: WREG has either 0xFF for no entry found or the index of 
;   the network table entry for this device
;
; Side Effects: none
;
; Stack Requirements:  level deep
;
; Overview: This function searches the network table for the specified
;   device.  It will return the index of the device in the network
;   table or 0xFF for failure to find the device.
;********************************************************************* 
#ifdef ENABLE_SEARCH_NWK_TBL_BY_SHORT_ADDRESS
SearchNetworkTableByShortAddress      
    ;Point to the network table
    movlw NetworkTable
    movwf FSR
    bcf STATUS,IRP
    
    ;use i as a loop counter
    movlw NETWORK_TABLE_SIZE
    movwf i
    
_SearchNWKTableBySALoop
    ;if the entry is valid
    btfss INDF,STATUS_IS_VALID
    goto _SearchNWKTableBySANotValid
    ;and it has a valid short address
    btfss INDF,STATUS_SHORT_ADDRESS_VALID
    goto _SearchNWKTableBySANotValid
    
    ;verify that the PANID matches
    incf FSR,f
    movf sourcePANID+0,w
    subwf INDF,w
    incf FSR,f
    movf sourcePANID+1,w
    btfss STATUS,C
    addlw 1
    subwf INDF,w
    btfss STATUS,Z
    goto _SearchNWKTableBySAFailPANID
    
    ;also verify that the short address matches
    incf FSR,f
    movf sourceShortAddress+0,w
    subwf INDF,w
    incf FSR,f
    movf sourceShortAddress+1,w
    btfss STATUS,C
    addlw 1
    subwf INDF,w
    btfss STATUS,Z
    goto _SearchNWKTableBySAFailSA
    
    ;if it does then put the handle to the entry in WREG
    movf FSR,W
    addlw -d'4'
    goto _SearchNWKTableBySAComplete
    
_SearchNWKTableBySAFailSA
    movlw d'9'
    goto _SearchNWKTableBySALoopFail
_SearchNWKTableBySAFailPANID
    movlw d'11'
    goto _SearchNWKTableBySALoopFail
_SearchNWKTableBySANotValid
    movlw d'13'
    ;goto _SearchNWKTableBySALoopFail   ;fall through to _SearchNWKTableBySALoopFail
_SearchNWKTableBySALoopFail
    addwf FSR,f    
_SearchNWKTableBySALoopCheck
    decfsz i,F
    goto _SearchNWKTableBySALoop
    
    movlw 0xFF
_SearchNWKTableBySAComplete
    return
#endif   ;#ifdef ENABLE_SEARCH_NWK_TBL_BY_SHORT_ADDRESS

;********************************************************************
; Function: SearchNetworkTableByLongAddress
;
; PreCondition: none
;
; Input: the long address that need to be found is located in 
;   sourceLongAddress
;
; Output: the handle of the resulting entry is in WREG (0xFF for a 
;   failure to find the device in the table)
;
; Side Effects: none
;
; Stack Requirements:  level deep
;
; Overview: 
;********************************************************************* 
SearchNetworkTableByLongAddress 
    ;Point to the network table
    movlw NetworkTable
    movwf FSR
    bcf STATUS,IRP
    
    ;use i as a loop counter
    movlw NETWORK_TABLE_SIZE
    movwf i
    
_SearchNWKTableByLALoop
    clrwdt
    ;is this entry valid
    btfss INDF,STATUS_IS_VALID
    goto _SearchNWKTableByLANotValid
    ;does this entry have a valid long address associated with it
    btfss INDF,STATUS_LONG_ADDRESS_VALID
    goto _SearchNWKTableByLANotValid
      
    ;does the long address match
    movlw d'5'
    addwf FSR,f
    movf sourceLongAddress+0,w
    subwf INDF,w
    incf FSR,f
    movf sourceLongAddress+1,w
    btfss STATUS,C
    addlw 1
    subwf INDF,w
    incf FSR,f
    movf sourceLongAddress+2,w
    btfss STATUS,C
    addlw 1
    subwf INDF,w
    incf FSR,f
    movf sourceLongAddress+3,w
    btfss STATUS,C
    addlw 1
    subwf INDF,w
    incf FSR,f
    movf sourceLongAddress+4,w
    btfss STATUS,C
    addlw 1
    subwf INDF,w
    incf FSR,f
    movf sourceLongAddress+5,w
    btfss STATUS,C
    addlw 1
    subwf INDF,w
    incf FSR,f
    movf sourceLongAddress+6,w
    btfss STATUS,C
    addlw 1
    subwf INDF,w
    incf FSR,f
    movf sourceLongAddress+7,w
    subwf INDF,w
    btfss STATUS,Z
    goto _SearchNWKTableByLAFailLA
    
    movf FSR,W
    addlw -d'12'
    goto _SearchNWKTableByLAComplete
    
_SearchNWKTableByLAFailLA
    movlw 1
    goto _SearchNWKTableByLALoopFail
_SearchNWKTableByLANotValid
    movlw d'13'
    ;goto _SearchNWKTableByLALoopFail   ;fall through to _SearchNWKTableByLALoopFail
_SearchNWKTableByLALoopFail
    addwf FSR,f 
_SearchNWKTableByLALoopCheck
    decfsz i,F
    goto _SearchNWKTableByLALoop
    
    movlw 0xFF
_SearchNWKTableByLAComplete
    return
    
;********************************************************************
; Function: SearchNetworkTableOpenEntry
;
; PreCondition: none
;
; Input: none
;
; Output: WREG contains either 0xFF for no entries available or the
;   handle the an open entry.  This entry is only valid until the next
;   time MiWiTask is called unless the Valid bit is marked to save it
;
; Side Effects: none
;
; Stack Requirements:  level deep
;
; Overview: This function will search the network table for the first
;   open entry and will return the handle to that entry.  If no entry
;   is found then it will return 0xFF
;********************************************************************* 
SearchNetworkTableOpenEntry
    ;Point to the network table
    movlw NetworkTable
    movwf FSR
    bcf STATUS,IRP
    
    ;use i as a loop counter
    movlw NETWORK_TABLE_SIZE
    movwf i
    
_SearchNWKTableOELoop
    clrwdt
    ;is the entry valid?
    btfss INDF,STATUS_IS_VALID
    goto _SearchNWKTableOEFound

    movlw d'13'
    addwf FSR,f
    decfsz i,f
    goto _SearchNWKTableOELoop
    movlw 0xFF
    goto _SearchNWKTableOEDone
    
_SearchNWKTableOEFound
    movf FSR,w
_SearchNWKTableOEDone    
    return
    
;********************************************************************
; Function: SendMiWiAck
;
; PreCondition: none
;
; Input: MiWiSourcePANID, MiWiSourceShortAddress, and 
;   MiWiSourceSeqNum have the values of the target device
;
; Output: none
;
; Side Effects: a MiWiAckPacket is transmitted
;
; Stack Requirements:  level deep
;
; Overview: This function causes the stack to transmit a MiWi Ack
;   packet.
;********************************************************************* 
SendMiWiAck
    ;load a pointer to the TxBuffer
    movlw TxBuffer
    movwf FSR
    bsf STATUS,IRP

    ;header size
    movlw d'22'  
    movwf INDF
    incf FSR,F
    ;packet size
    movwf INDF
    incf FSR,F
    
    ;frame control
    movlw 0x61
    movwf INDF
    incf FSR,F
    
    movlw 0x88
    movf MRF_DATA,f
    movwf INDF
    incf FSR,F
    
    ;sequence number
    banksel MACSeqNum
    movf MACSeqNum,W
    movwf INDF
    incf FSR,F
    incf MACSeqNum,F
      
    ;destination PANID
    movf parentPANID+0,w
    movwf INDF
    incf FSR,F
    
    movf parentPANID+1,w
    movwf INDF
    incf FSR,F
    
    ;destination Short Address
    movf parentShortAddress+0,w
    movwf INDF
    incf FSR,F
    
    movf parentShortAddress+1,w
    movwf INDF
    incf FSR,F
      
    movf myShortAddress,w
    movwf INDF
    incf FSR,F

    movf myShortAddress+1,w
    movwf INDF
    incf FSR,F
    
    ;Start of the MiWi Header
    
    movlw 0x04      ;HOPS
    movwf INDF
    incf FSR,F
    
    movlw 0x02      ;MiWi FrameControl - no ACK (this is it), Intra, no security
    movwf INDF
    incf FSR,F
    
    ;destination PANID - located in sourcePANID
    movf MiWiSourcePANID+0,w
    movwf INDF
    incf FSR,F
    
    movf MiWiSourcePANID+1,w
    movwf INDF
    incf FSR,F
    
    ;destination address - located in sourceShortAddress
    movf MiWiSourceShortAddress+0,w
    movwf INDF
    incf FSR,F
    
    movf MiWiSourceShortAddress+1,w
    movwf INDF
    incf FSR,F
    
    ;source PANID
    movf parentPANID+0,w
    movwf INDF
    incf FSR,F
    
    movf parentPANID+1,w
    movwf INDF
    incf FSR,F
    
    ;source address
    movf myShortAddress,w
    movwf INDF
    incf FSR,F

    movf myShortAddress+1,w
    movwf INDF
    incf FSR,F
    
    ;sequence number
    movf MiWiSourceSeqNum,W     ;send the same sequence number that we got from them
    movwf INDF
    incf FSR,F
    
    ;Stack Report
    movlw 0x00
    movwf INDF
    incf FSR,F
    
    ;MiWi Ack
    movlw 0x30
    movwf INDF
    incf FSR,F
  
    call TransmitPacket
    
    return
    
;********************************************************************
; Function: SendEUIAddressSearchRequest
;
; PreCondition: none
;
; Input: sourceLongAddress contains the long address of the device
;   that needs to be found
;
; Output: none
;
; Side Effects: and EUI address search request is sent, and
;   MiWiSearchStatus,STATUS_EUI_SEARCH_IN_PROGRESS is set.  This bit
;   must be cleared by the application in order to terminate the
;   search.  Failure to terminate the search will allow EUI address
;   search response packets to always change the network table
;
; Stack Requirements:  level deep
;
; Overview: This function will cause the device to send an EUI address
;   search packet to its parent in order to find an the short address
;   information of a device on the network by its EUI address
;********************************************************************* 
#ifdef ENABLE_EUI_ADDRESS_SEARCH
SendEUIAddressSearchRequest
    ;load a pointer to the TxBuffer
    movlw TxBuffer
    movwf FSR
    bsf STATUS,IRP

    ;header size
    movlw d'30'  
    movwf INDF
    incf FSR,F
    ;packet size
    movwf INDF
    incf FSR,F
    
    ;frame control
    movlw 0x61
    movwf INDF
    incf FSR,F
    
    movlw 0x88
    movf MRF_DATA,f
    movwf INDF
    incf FSR,F
    
    ;sequence number
    banksel MACSeqNum
    movf MACSeqNum,W
    movwf INDF
    incf FSR,F
    incf MACSeqNum,F
      
    ;destination PANID
    movf parentPANID+0,w
    movwf INDF
    incf FSR,F
    
    movf parentPANID+1,w
    movwf INDF
    incf FSR,F
    
    ;destination Short Address
    movf parentShortAddress+0,w
    movwf INDF
    incf FSR,F
    
    movf parentShortAddress+1,w
    movwf INDF
    incf FSR,F
      
    movf myShortAddress,w
    movwf INDF
    incf FSR,F

    movf myShortAddress+1,w
    movwf INDF
    incf FSR,F
    
    ;Start of the MiWi Header
    
    movlw 0x04      ;HOPS
    movwf INDF
    incf FSR,F
    
    movlw 0x06      ;MiWi FrameControl - ACK, Intra, no security
    movwf INDF
    incf FSR,F
    
    ;destination PANID - located in sourcePANID
    movlw 0xFF
    movwf INDF
    incf FSR,F

    ;movlw 0xFF ;still set from the last load    
    movwf INDF
    incf FSR,F
    
    ;destination address - located in sourceShortAddress
    ;movlw 0xFF ;still set from the last load
    movwf INDF
    incf FSR,F
    
    ;movlw 0xFF ;still set from the last load
    movwf INDF
    incf FSR,F
    
    ;source PANID
    movf parentPANID+0,w
    movwf INDF
    incf FSR,F
    
    movf parentPANID+1,w
    movwf INDF
    incf FSR,F
    
    ;source address
    movf myShortAddress,w
    movwf INDF
    incf FSR,F

    movf myShortAddress+1,w
    movwf INDF
    incf FSR,F
    
    ;sequence number
    movf MiWiSourceSeqNum,W     ;send the same sequence number that we got from them
    movwf INDF
    incf FSR,F
    
    ;Stack Report
    movlw 0x00
    movwf INDF
    incf FSR,F
    
    ;MiWi EUI address search request
    movlw 0x20
    movwf INDF

    ;EUI Address that we are looking for is located in sourceLongAddress
    call FillInLongAddress
    
    call TransmitPacket
    
    ;mark that the search is in progress.  Must be cleared by 
    ;  the application to determine when it is complete
    bsf MiWiSearchStatus,STATUS_EUI_SEARCH_IN_PROGRESS
    
    return
#endif
    
;********************************************************************
; Function: MRF24J40Sleep
;
; PreCondition: MRF24J40Init has been called and the interface pins
;   are set up correctly 
;
; Input: none
;
; Output: none
;
; Side Effects: the MRF24J40 is asleep and the WAKE pin is forced low
;   See MRF24J40Wake function for more possible side effects of using
;   MRF24J40Sleep and MRF24J40Wake
;
; Stack Requirements:  level deep
;
; Overview: This function causes the MRF24J40 to go into a low power
;   mode
;********************************************************************* 
MRF24J40Sleep   
    ;mark the MRF24J40 as not ready to use anymore
    bsf MiWiStatus,MRF_NOT_READY
    
    ;clear the WAKE pin in order to allow the device to go to sleep
    bcf PHY_WAKE
    
    ;write the registers required to place the device in sleep
    movlw WRITE_TXBCNINTL
    movwf MRF_ADDRL
    movlw 0x80
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    movlw WRITE_RXFLUSH
    movwf MRF_ADDRL
    movlw 0x60
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    movlw WRITE_SLPACK
    movwf MRF_ADDRL
    movlw 0x80
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    return
    
;********************************************************************
; Function: MRF24J40Wake
;
; PreCondition: none
;
; Input: none
;
; Output: none
;
; Side Effects: This may cause a WDT reset if the WDT is enabled and
;   the MRF24J40 is non-responsive.  
;
; Stack Requirements:  level deep
;
; Overview: This function will wake up the MRF24J40 and wait for the
;   the device to 
;********************************************************************* 
MRF24J40Wake
    ;wake up the device
    bsf PHY_WAKE
    
_MRF24J40WakeNotReady  
    ;wait until the MRF24J40 enters RX mode.  This means the device
    ;  is up, running, and calibrated.
    movlw READ_RFSTATE_L
    movwf MRF_ADDRL
    movlw READ_RFSTATE_H
    movwf MRF_ADDRH
    call PHYLongRAMAddr
    andlw b'11100000'
    xorlw b'10100000'
    btfss STATUS,Z
    goto _MRF24J40WakeNotReady
        
    ;mark the MRF24J40 as ready to use
    bcf MiWiStatus,MRF_NOT_READY
    
    return
    
;********************************************************************
; Function: PHYShortRAMAddr
;
; PreCondition: none
;
; Input: MRF_ADDRL has the address of the register that needs to 
;   be accessed.  use the WRITE_X to write to the register and READ_X
;   to read from the register.  MRF_DATA should contain the data
;   that needs to be written to the device.
;
; Output: On reads of registers the WREG will contain the value read
;   from the MRF24J40
;
; Side Effects: none
;
; Stack Requirements:  level deep
;
; Overview: This function will either read or write short register 
;   values in the MRF24J40
;********************************************************************* 
PHYShortRAMAddr
	banksel 0x00			;select bank 0
	bcf PHY_CS				;CS = 0
	bcf PIR1,SSPIF			;clear interrupt flag
_WCOL_SET_LOOP1				;
	bcf SSPCON,WCOL			;clear write collision flag
	movf MRF_ADDRL,w		;send address
	movwf SSPBUF			;
	btfsc SSPCON,WCOL		;check for write collision
	goto _WCOL_SET_LOOP1	;
_SPI_SET_LOOP1				;
	btfss PIR1,SSPIF		;check for transmission completion
	goto _SPI_SET_LOOP1		;
	bcf PIR1,SSPIF			;clear interrupt flag
	movlw 0x00				;clear WREG
	btfsc MRF_ADDRL,0		;if this is a write
	movf MRF_DATA,w			;then load data
_WCOL_SET_LOOP2				;
	bcf SSPCON,WCOL			;clear write collision flag
	movwf SSPBUF			;
	btfsc SSPCON,WCOL		;check for write collision
	goto _WCOL_SET_LOOP2	;
_SPI_SET_LOOP2				;
	btfss PIR1,SSPIF		;check for transmission completion
	goto _SPI_SET_LOOP2		;
	btfss MRF_ADDRL,0		;if this is a write
	movf SSPBUF,w			;
	bsf PHY_CS				;CS = 1
	return



;********************************************************************
; Function: PHYShortRAMAddr
;
; PreCondition: none
;
; Input: MRF_ADDRL and MRF_ADDRH have the address of the register
;   that needs to be accessed.  use the WRITE_X to write to the 
;   register and READ_X to read from the register.  MRF_DATA should
;   contain the data that needs to be written to the device.
;
; Output: On reads of registers the WREG will contain the value read
;   from the MRF24J40
;
; Side Effects: none
;
; Stack Requirements:  level deep
;
; Overview: This function will either read or write long register 
;   values in the MRF24J40
;********************************************************************* 
PHYLongRAMAddr
	banksel 0x00			;select bank 0
	bcf PHY_CS				;CS = 0
	bcf PIR1,SSPIF			;clear interrupt flag
_WCOL_SETL_LOOP1			;
	bcf SSPCON,WCOL			;clear write collision flag
	movf MRF_ADDRH,w		;send address
	movwf SSPBUF			;
	btfsc SSPCON,WCOL		;check for write collision
	goto _WCOL_SETL_LOOP1	;
_SPI_SETL_LOOP1				;
	btfss PIR1,SSPIF		;check for transmission completion
	goto _SPI_SETL_LOOP1	;
	bcf PIR1,SSPIF			;clear interrupt flag
_WCOL_SETL_LOOP2			;
	bcf SSPCON,WCOL			;clear write collision flag
	movf MRF_ADDRL,w		;send address
	movwf SSPBUF			;
	btfsc SSPCON,WCOL		;check for write collision
	goto _WCOL_SETL_LOOP2	;
_SPI_SETL_LOOP2				;
	btfss PIR1,SSPIF		;check for transmission completion
	goto _SPI_SETL_LOOP2	;
	bcf PIR1,SSPIF			;clear interrupt flag
	movlw 0x00				;clear WREG
	btfsc MRF_ADDRL,4		;if this is a write
	movf MRF_DATA,w			;then load data
_WCOL_SETL_LOOP3			;
	bcf SSPCON,WCOL			;clear write collision flag
	movwf SSPBUF			;
	btfsc SSPCON,WCOL		;check for write collision
	goto _WCOL_SETL_LOOP3	;
_SPI_SETL_LOOP3				;
	btfss PIR1,SSPIF		;check for transmission completion
	goto _SPI_SETL_LOOP3	;
	btfss MRF_ADDRL,4		;if this is a write
	movf SSPBUF,w			;
	bsf PHY_CS				;CS = 1
	return
	
;********************************************************************
; Function: MRF24J40Init
;
; PreCondition: the SPI and pins connected to the MRF24J40 have been
;   initialized to the proper direction, setting, and are enabled.
;
; Input: none
;
; Output: none
;
; Side Effects: the MRF24J40 is configured for operation.  This will
;   reset many of the operational values in the MRF24J40 so function
;   should not be called during stack operation.
;
; Stack Requirements:  level deep
;
; Overview: This function will configure the MRF24J40 for normal 
;   operation.
;********************************************************************* 
MRF24J40Init
	banksel PHY_RESETn_TRIS_REG
	bcf PHY_RESETn_TRIS
	banksel PHY_RESETn_REG
	bsf PHY_CS
	bcf PHY_RESETn			;place MRF in reset
	movlw 0xFF
	movwf MRF_DATA
_MRF24J40Init_ResetLoop1
    clrwdt
	decf MRF_DATA,f
	btfss STATUS,Z
	goto _MRF24J40Init_ResetLoop1
	bsf PHY_RESETn			;release MRF from reset
_MRF24J40Init_ResetLoop2
    clrwdt
	decf MRF_DATA,f
	btfss STATUS,Z
	goto _MRF24J40Init_ResetLoop2
	
	;Do a soft reset on the device
	movlw 0x07
	movwf MRF_DATA
	movlw WRITE_SOFTRST
	movwf MRF_ADDRL
	call PHYShortRAMAddr
_MRF24J40Init_SoftResetLoop
    clrwdt
	movlw READ_SOFTRST
	movwf MRF_ADDRL
	call PHYShortRAMAddr
	andlw 0x07
	btfss STATUS,Z
	goto _MRF24J40Init_SoftResetLoop
	
	;Flush the RX FIFO
	movlw 0x01
	movwf MRF_DATA
	movlw WRITE_RXFLUSH
	movwf MRF_ADDRL
	call PHYShortRAMAddr
	
	;set this devices PANID and short address to 0xFFFF
	movlw 0xFF
	movwf MRF_DATA
	movlw WRITE_PANIDL
	movwf MRF_ADDRL
	call PHYShortRAMAddr
	movlw WRITE_PANIDH
	movwf MRF_ADDRL
	call PHYShortRAMAddr
	movlw WRITE_SADRL
	movwf MRF_ADDRL
	call PHYShortRAMAddr
	movlw WRITE_SADRH
	movwf MRF_ADDRL
	call PHYShortRAMAddr
	
	;Set this devices long address
    movlw WRITE_EADR0
    movwf MRF_ADDRL
    movlw EUI_0
    movwf MRF_DATA
    call PHYShortRAMAddr

    movlw WRITE_EADR1
    movwf MRF_ADDRL
    movlw EUI_1
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    movlw WRITE_EADR2
    movwf MRF_ADDRL
    movlw EUI_2
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    movlw WRITE_EADR3
    movwf MRF_ADDRL
    movlw EUI_3
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    movlw WRITE_EADR4
    movwf MRF_ADDRL
    movlw EUI_4
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    movlw WRITE_EADR5
    movwf MRF_ADDRL
    movlw EUI_5
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    movlw WRITE_EADR6
    movwf MRF_ADDRL
    movlw EUI_6
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    movlw WRITE_EADR7
    movwf MRF_ADDRL
    movlw EUI_7
    movwf MRF_DATA
    call PHYShortRAMAddr
    
	;PHYSetLongRAMAddr(RFCTRL1,0x00);
	movlw WRITE_RFCTRL1_L
	movwf MRF_ADDRL
	movlw WRITE_RFCTRL1_H
	movwf MRF_ADDRH
	movlw 0x00
	movwf MRF_DATA
	call PHYLongRAMAddr
	
	;PHYSetLongRAMAddr(RFCTRL2,0x80);
	movlw WRITE_RFCTRL2_L
	movwf MRF_ADDRL
	movlw WRITE_RFCTRL2_H
	movwf MRF_ADDRH
	movlw 0x80
	movwf MRF_DATA
	call PHYLongRAMAddr
	
	;PHYSetLongRAMAddr(RFCTRL3,PA_LEVEL);
	movlw WRITE_RFCTRL3_L
	movwf MRF_ADDRL
	movlw WRITE_RFCTRL3_H
	movwf MRF_ADDRH
	movlw PA_LEVEL
	movwf MRF_DATA
	call PHYLongRAMAddr
	
	;PHYSetLongRAMAddr(RFCTRL6,0x90);
	movlw WRITE_RFCTRL6_L
	movwf MRF_ADDRL
	movlw WRITE_RFCTRL6_H
	movwf MRF_ADDRH
	movlw 0x90
	movwf MRF_DATA
	call PHYLongRAMAddr
	
	;PHYSetLongRAMAddr(RFCTRL7,0b10011000);
	movlw WRITE_RFCTRL7_L
	movwf MRF_ADDRL
	movlw WRITE_RFCTRL7_H
	movwf MRF_ADDRH
	movlw b'10011000'
	movwf MRF_DATA
	call PHYLongRAMAddr
	
	;PHYSetLongRAMAddr(RFCTRL8,0b00010000);
	movlw WRITE_RFCTRL8_L
	movwf MRF_ADDRL
	movlw WRITE_RFCTRL8_H
	movwf MRF_ADDRH
	movlw b'00010000'
	movwf MRF_DATA
	call PHYLongRAMAddr
	
    ;/* Program CCA mode using RSSI */
    ;PHYSetShortRAMAddr(WRITE_BBREG2,0x78);
    movlw WRITE_BBREG2
    movwf MRF_ADDRL
    movlw 0x78
    movwf MRF_DATA
    call PHYShortRAMAddr
	
    ;/* Enable the packet RSSI */
    ;PHYSetShortRAMAddr(WRITE_BBREG6,0x40);
    movlw WRITE_BBREG6
    movwf MRF_ADDRL
    movlw 0x40
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    ;/* Program CCA, RSSI threshold values */
    ;PHYSetShortRAMAddr(WRITE_RSSITHCCA,0x60);
    movlw WRITE_RSSITHCCA
    movwf MRF_ADDRL
    movlw 0x60
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    ;verify the RFSTATE is in the correct state
    ;do
    ;{
    ;    i = PHYGetLongRAMAddr(RFSTATE);
    ;}
    ;while((i&0xA0) != 0xA0);
	movlw READ_RFSTATE_L
	movwf MRF_ADDRL
	movlw READ_RFSTATE_H
	movwf MRF_ADDRH
_MRF24J40Init_RFSTATELoop
    clrwdt
	call PHYLongRAMAddr
	andlw 0xE0
	xorlw 0xA0
	btfss STATUS,Z
	goto _MRF24J40Init_RFSTATELoop
	
	;SetChannel(availableChannels[0]);
	movlw CHANNEL_11
	movwf MRF_DATA
	call SetChannel
	
	;PHYSetShortRAMAddr(WRITE_INTMSK,0xF6);
	movlw WRITE_INTMSK
	movwf MRF_ADDRL
	movlw 0xF6
	movwf MRF_DATA
	call PHYShortRAMAddr
	
	return
	
;********************************************************************
; Function: SetChannel
;
; PreCondition: the MRF24J40 has been initialized
;
; Input: the channel that needs to be selected is in MRF_DATA
;
; Output: none
;
; Side Effects: the MRF24J40 changes channels
;
; Stack Requirements:  level deep
;
; Overview: This function will change the channel of the MRF24J40
;********************************************************************* 
SetChannel
	;set the channel number
	movlw WRITE_RFCTRL0_L
	movwf MRF_ADDRL
	movlw WRITE_RFCTRL0_H
	movwf MRF_ADDRH
	call PHYLongRAMAddr
	
    ;PHYSetShortRAMAddr(WRITE_RFCTL,0x04);
    movlw WRITE_RFCTL
    movwf MRF_ADDRL
    movlw 0x04
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    ;PHYSetShortRAMAddr(WRITE_RFCTL,0x00);
    ;address already set by last call
    movlw 0x00
    movwf MRF_DATA
    call PHYShortRAMAddr
    
    return
    
;********************************************************************
; Function: ReadLongAddress
;
; PreCondition: FSR pointing to the first byte of the long address
;
; Input: none
;
; Output: sourceLongAddress contains the long address
;
; Side Effects: FSR pointer is moved by 8 bytes
;
; Stack Requirements:  level deep
;
; Overview: This function reads out a long address pointed to by FSR
;   and saves that long address in sourceLongAddress.  This is a
;   helper function and not intended for application use.
;********************************************************************* 
ReadLongAddress
    movf INDF,w
    movwf sourceLongAddress+0
    incf FSR,F
    movf INDF,w
    movwf sourceLongAddress+1
    incf FSR,F
    movf INDF,w
    movwf sourceLongAddress+2
    incf FSR,F
    movf INDF,w
    movwf sourceLongAddress+3
    incf FSR,F
    movf INDF,w
    movwf sourceLongAddress+4
    incf FSR,F
    movf INDF,w
    movwf sourceLongAddress+5
    incf FSR,F
    movf INDF,w
    movwf sourceLongAddress+6
    incf FSR,F
    movf INDF,w
    movwf sourceLongAddress+7
    return
    
;********************************************************************
; Function: FillInLongAddress
;
; PreCondition: FSR pointing to the first byte of the long address
;
; Input: sourceLongAddress contains the long address
;
; Output: none
;
; Side Effects: FSR pointer is moved by 8 bytes
;
; Stack Requirements:  level deep
;
; Overview: This function writes a long address from sourceLongAddress
;   into the RAM locations pointed to by FSR.  This is a
;   helper function and not intended for application use.
;*********************************************************************     
FillInLongAddress
    incf FSR,f
    movf sourceLongAddress+0,w
    movwf INDF
    incf FSR,f
    movf sourceLongAddress+1,w
    movwf INDF
    incf FSR,f
    movf sourceLongAddress+2,w
    movwf INDF
    incf FSR,f
    movf sourceLongAddress+3,w
    movwf INDF
    incf FSR,f
    movf sourceLongAddress+4,w
    movwf INDF
    incf FSR,f
    movf sourceLongAddress+5,w
    movwf INDF
    incf FSR,f
    movf sourceLongAddress+6,w
    movwf INDF
    incf FSR,f
    movf sourceLongAddress+7,w
    movwf INDF

    return

;************** DEBUGGING function **************************
; These functions are not intended for end application use
; but are made available in order to assist development
; debugging

#ifdef DEBUG_DUMP_NETWORK_TABLE
DumpNetworkTable
    movlw NetworkTable
    movwf FSR
    bcf STATUS,IRP
       
    movlw NETWORK_TABLE_SIZE
    movwf MRF_ADDRH
    
_DumpNetworkTableOL
    movlw d'13'
    movwf MRF_ADDRL
    
_DumpNetworkTableIL
    movlw high(PrintChar)
    movwf PCLATH
    movf INDF,w
    call PrintChar
    movlw high($)
    movwf PCLATH

    incf FSR,F
    decfsz MRF_ADDRL,f
    goto _DumpNetworkTableIL
    
    movlw high(ConsolePut)
    movwf PCLATH
    movlw a'\r'
    call ConsolePut

    movlw a'\n'
    call ConsolePut
    movlw high($)
    movwf PCLATH
    
    decfsz MRF_ADDRH,f
    goto _DumpNetworkTableOL

    movlw high(ConsolePut)
    movwf PCLATH
    movlw a'\r'
    call ConsolePut
    movlw a'\n'
    call ConsolePut
    
    movlw a'\r'
    call ConsolePut
    movlw a'\n'
    call ConsolePut
    movlw high($)
    movwf PCLATH
    banksel 0x00
    return
#endif
    
#ifdef DEBUG_MRF
#ifdef ENABLE_CONSOLE
DumpTxBuffer
    movlw 0x7F
    movwf MRF_DATA

	movlw b'10000000'
	movwf MRF_ADDRH
	movlw b'00000000'
	movwf MRF_ADDRL
	
_DumpTxBufferLoop
	call PHYLongRAMAddr
    call PrintChar
	movlw b'00100000'
	addwf MRF_ADDRL,f
	btfsc STATUS,C
	incf MRF_ADDRH,f
	decf MRF_DATA,f
	btfss STATUS,Z
	goto _DumpTxBufferLoop
    return
    
DumpAddressInfo
    movlw 0x7F
    movwf MRF_DATA

	movlw READ_PANIDH
	movwf MRF_ADDRL
	call PHYShortRAMAddr
    call PrintChar
	
	movlw READ_PANIDL
	movwf MRF_ADDRL
	call PHYShortRAMAddr
    call PrintChar
	
	movlw ' '
	call ConsolePut
	
	movlw READ_SADRH
	movwf MRF_ADDRL
	call PHYShortRAMAddr
    call PrintChar

	
	movlw READ_SADRL
	movwf MRF_ADDRL
	call PHYShortRAMAddr
    call PrintChar
	
	movlw ' '
	call ConsolePut
	
	movlw READ_EADR7
	movwf MRF_ADDRL
	call PHYShortRAMAddr
    call PrintChar
	
	movlw READ_EADR6
	movwf MRF_ADDRL
	call PHYShortRAMAddr
    call PrintChar
	
	movlw READ_EADR5
	movwf MRF_ADDRL
	call PHYShortRAMAddr
    call PrintChar
	
	movlw READ_EADR4
	movwf MRF_ADDRL
	call PHYShortRAMAddr
    call PrintChar
	
	movlw READ_EADR3
	movwf MRF_ADDRL
	call PHYShortRAMAddr
    call PrintChar
	
	movlw READ_EADR2
	movwf MRF_ADDRL
	call PHYShortRAMAddr
    call PrintChar

	movlw READ_EADR1
	movwf MRF_ADDRL
	call PHYShortRAMAddr
    call PrintChar

	
	movlw READ_EADR0
	movwf MRF_ADDRL
	call PHYShortRAMAddr
    call PrintChar

    return
    

    #endif
    #endif
    
	end
