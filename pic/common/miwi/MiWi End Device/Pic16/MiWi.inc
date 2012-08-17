;*********************************************************************
;* FileName:		MiWi.inc
;* Dependencies:    MiWiDefs.inc
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
;*   Definitions for all MiWi and MRF24J40 functions, variables, bit,
;*     and register mappings.
;* 
;* Change History:
;*  Rev   Date         Description
;*  0.1   11/15/2006   Initial revision
;*  1.0   01/15/2007   Initial release
;********************************************************************/
#ifndef MIWI_H
#define MIWI_H
	#ifdef MIWI_ASM
		#include "MiWiDefs.inc"
		;Functions
		GLOBAL TransmitPacket
		GLOBAL LoadBeaconRequest
		GLOBAL UpdateStatus
		GLOBAL MiWiInit
		GLOBAL MiWiTasks
		GLOBAL StartNetworkSearch
		GLOBAL JoinNetwork
		GLOBAL LoadDataRequest
		GLOBAL RejoinNetwork
		#ifdef ENABLE_SEARCH_NWK_TBL_BY_SHORT_ADDRESS
		    GLOBAL SearchNetworkTableByShortAddress
		#endif
		GLOBAL MiWiSourcePANID
		GLOBAL MiWiSourceShortAddress
		GLOBAL TxBuffer
		GLOBAL MACSeqNum
		GLOBAL myShortAddress
		GLOBAL MiWiSourceSeqNum
		#ifdef ENABLE_CLUSTER_SOCKETS
		    GLOBAL OpenSocket
		#endif
        GLOBAL PHYShortRAMAddr
        GLOBAL PHYLongRAMAddr
        GLOBAL MRF24J40Init
        GLOBAL SetChannel
    	GLOBAL MRF24J40Sleep
    	GLOBAL MRF24J40Wake
        GLOBAL FillInLongAddress
        #ifdef DEBUG_MRF
            #ifdef ENABLE_CONSOLE
               GLOBAL DumpTxBuffer
               GLOBAL DumpAddressInfo
            #endif
        #endif
        #ifdef DEBUG_DUMP_NETWORK_TABLE
            GLOBAL DumpNetworkTable
        #endif
        
        #ifdef ENABLE_EUI_ADDRESS_SEARCH
            GLOBAL SendEUIAddressSearchRequest
        #endif
		
		;Variables
		GLOBAL MiWiStatus
		GLOBAL MiWiTasksVar
        GLOBAL parentShortAddress 
        GLOBAL parentPANID        
        GLOBAL parentChannel      
        GLOBAL parentRSSI  
        GLOBAL RxPacketAddress   
        #ifdef ENABLE_CLUSTER_SOCKETS
            GLOBAL MiWiSocketStatus  
            GLOBAL MiWiSocketHandle  
        #endif
        GLOBAL NetworkTable 
    	GLOBAL MRF_ADDRL
    	GLOBAL MRF_ADDRH
    	GLOBAL MRF_DATA
    	GLOBAL sourceLongAddress
    	#ifdef ENABLE_EUI_ADDRESS_SEARCH
    	    GLOBAL MiWiSearchStatus
    	#endif
	#else
	    ;Functions
		EXTERN TransmitPacket
		EXTERN LoadBeaconRequest
		EXTERN UpdateStatus
		EXTERN MiWiInit
		EXTERN MiWiTasks
		EXTERN StartNetworkSearch
		EXTERN MiWiTasksVar
		EXTERN JoinNetwork
		EXTERN LoadDataRequest
		EXTERN RejoinNetwork
		#ifdef ENABLE_SEARCH_NWK_TBL_BY_SHORT_ADDRESS
		    EXTERN SearchNetworkTableByShortAddress
		#endif
		EXTERN MiWiSourcePANID
		EXTERN MiWiSourceShortAddress
		EXTERN TxBuffer
		EXTERN MACSeqNum
		EXTERN myShortAddress
		EXTERN MiWiSourceSeqNum
		#ifdef ENABLE_CLUSTER_SOCKETS
		    EXTERN OpenSocket
		#endif
    	EXTERN PHYShortRAMAddr
    	EXTERN PHYLongRAMAddr
    	EXTERN MRF24J40Init
    	EXTERN SetChannel
    	EXTERN MRF24J40Sleep
    	EXTERN MRF24J40Wake
    	#ifdef DEBUG_MRF
    	    #ifdef ENABLE_CONSOLE
    	        EXTERN DumpTxBuffer
    	        EXTERN DumpAddressInfo
    	    #endif
    	#endif
        #ifdef DEBUG_DUMP_NETWORK_TABLE
            EXTERN DumpNetworkTable
        #endif
        #ifdef ENABLE_EUI_ADDRESS_SEARCH
            EXTERN SendEUIAddressSearchRequest
        #endif
		
		;Variables
		EXTERN MiWiStatus
		EXTERN parentShortAddress 
        EXTERN parentPANID        
        EXTERN parentChannel      
        EXTERN parentRSSI       
        EXTERN RxPacketAddress
		#ifdef ENABLE_CLUSTER_SOCKETS
            EXTERN MiWiSocketStatus
            EXTERN MiWiSocketHandle
        #endif
        EXTERN NetworkTable
    	EXTERN MRF_ADDRL
    	EXTERN MRF_ADDRH
    	EXTERN MRF_DATA
    	EXTERN sourceLongAddress
    	#ifdef ENABLE_EUI_ADDRESS_SEARCH
    	    EXTERN MiWiSearchStatus
    	#endif
	#endif
	
	;Definitions
	;  MiWiStatus = | 0 | 0 | DataReqInProgress | FoundNetwork | NetworkMember | TxSuccessful | PacketReceived | TxBusy
    #define TX_BUSY 0
    #define PACKET_RECEIVED 1
    #define TX_SUCCESSFUL 2
    #define NETWORK_MEMBER 3
    #define FOUND_NETWORK 4
    #define DATA_REQ_IN_PROGRESS 5
    #define MRF_NOT_READY 6
    #define MIWI_STATUS_DONT_SLEEP_MASK ((1<<TX_BUSY) | (1<<PACKET_RECEIVED) | (1<<DATA_REQ_IN_PROGRESS) | (1<<MRF_NOT_READY))
    ;  MiWiTasksVar = | 0 | 0 | REJOIN_NETWORK | JOIN_SUCCESSFUL | JOIN_DATA_REQ_SENT | ASSOC_REQ_SENT | JOIN_NETWORK | SEARCH_FOR_NETWORK
    #define SEARCH_FOR_NETWORK 0
    #define JOIN_NETWORK 1
    #define ASSOC_REQ_SENT 2
    #define JOIN_DATA_REQ_SENT 3
    #define JOIN_SUCCESSFUL 4
    #define REJOIN_NETWORK 5
    ;  searchStatus = | CurrentChannel (4 bits) | 0 | 0 | reserved | BeaconRequestSent |
    #define BEACON_REQUEST_SENT 0
    ;  MiWiSocketStatus = | 0 | 0 | 0 | 0 | NetworkTableFull | SocketCreated | SocketResponseReceived | SocketRequestSent |
    #define SOCKET_REQUEST_SENT 0
    #define SOCKET_RESPONSE_RECEIVED 1
    #define SOCKET_CREATED 2
    #define SOCKET_NETWORK_TABLE_FULL 3
    ;  NetworkTableStatus = isFamily (bit 7) | RXOnWhenIdle | 0 | P2PConnection | shortAddressValid | longAddressValid | directConnection | isValid
    #define STATUS_IS_VALID 0
    #define STATUS_DIRECT_CONNECTION 1
    #define STATUS_LONG_ADDRESS_VALID 2
    #define STATUS_SHORT_ADDRESS_VALID 3
    #define STATUS_P2P_CONNECTION 4
    #define STATUS_RX_ON_WHEN_IDLE 6
    #define STATUS_IS_FAMILY 7
    ;  MiWiSearchStatus
    #define STATUS_EUI_SEARCH_IN_PROGRESS 0
    
    
    #define DATA_REQ_LONG_ADDR h'00'
    #define DATA_REQ_SHORT_ADDR h'FF'
    
    
    ;MRF24J40 definitions
    
    ;; Short RAM Addresses
    ;;   Write
    #define WRITE_RXMCR (0x01)
    #define WRITE_PANIDL (0x03)
    #define WRITE_PANIDH (0x05)
    #define WRITE_SADRL (0x07)
    #define WRITE_SADRH (0x09)
    #define WRITE_EADR0 (0x0B)
    #define WRITE_EADR1 (0x0D)
    #define WRITE_EADR2 (0x0F)
    #define WRITE_EADR3 (0x11)
    #define WRITE_EADR4 (0x13)
    #define WRITE_EADR5 (0x15)
    #define WRITE_EADR6 (0x17)
    #define WRITE_EADR7 (0x19)
    #define WRITE_RXFLUSH (0x1B)
    #define WRITE_TXSTATE0 (0x1D)
    #define WRITE_TXSTATE1 (0x1F)
    #define WRITE_ORDER (0x21)
    #define WRITE_TXMCR (0x23)
    #define WRITE_ACKTMOUT (0x25)
    #define WRITE_SLALLOC (0x27)
    #define WRITE_SYMTICKL (0x29)
    #define WRITE_SYMTICKH (0x2B)
    #define WRITE_PAONTIME (0x2D)
    #define WRITE_PAONSETUP (0x2F)
    #define WRITE_FFOEN (0x31)
    #define WRITE_CSMACR (0x33)
    #define WRITE_TXBCNTRIG (0x35)
    #define WRITE_TXNMTRIG (0x37)
    #define WRITE_TXG1TRIG (0x39)
    #define WRITE_TXG2TRIG (0x3B)
    #define WRITE_ESLOTG23 (0x3D)
    #define WRITE_ESLOTG45 (0x3F)
    #define WRITE_ESLOTG67 (0x41)
    #define WRITE_TXPEND (0x43)
    #define WRITE_TXBCNINTL (0x45)
    #define WRITE_FRMOFFSET (0x47)
    #define WRITE_TXSR (0x49)
    #define WRITE_TXLERR (0x4B)
    #define WRITE_GATE_CLK (0x4D)
    #define WRITE_TXOFFSET (0x4F)
    #define WRITE_HSYMTMR0 (0x51)
    #define WRITE_HSYMTMR1 (0x53)
    #define WRITE_SOFTRST (0x55)
    #define WRITE_BISTCR (0x57)
    #define WRITE_SECCR0 (0x59)
    #define WRITE_SECCR1 (0x5B)
    #define WRITE_TXPEMISP (0x5D)
    #define WRITE_SECISR (0x5F)
    #define WRITE_RXSR (0x61)
    #define WRITE_ISRSTS (0x63)
    #define WRITE_INTMSK (0x65)
    #define WRITE_GPIO (0x67)
    #define WRITE_GPIODIR (0x69)
    #define WRITE_SLPACK (0x6B)
    #define WRITE_RFCTL (0x6D)
    #define WRITE_SECCR2 (0x6F)
    ;//#define  (0x71)
    #define WRITE_BBREG1 (0x73)
    #define WRITE_BBREG2 (0x75)
    #define WRITE_BBREG3 (0x77)
    #define WRITE_BBREG4 (0x79)
    #define WRITE_BBREG5 (0x7B)
    #define WRITE_BBREG6 (0x7D)
    #define WRITE_RSSITHCCA (0x7F)
    
    ;;   Read
    #define READ_RXMCR (0x00)
    #define READ_PANIDL (0x02)
    #define READ_PANIDH (0x04)
    #define READ_SADRL (0x06)
    #define READ_SADRH (0x08)
    #define READ_EADR0 (0x0A)
    #define READ_EADR1 (0x0C)
    #define READ_EADR2 (0x0E)
    #define READ_EADR3 (0x10)
    #define READ_EADR4 (0x12)
    #define READ_EADR5 (0x14)
    #define READ_EADR6 (0x16)
    #define READ_EADR7 (0x18)
    #define READ_RXFLUSH (0x1a)
    #define READ_TXSTATE0 (0x1c)
    #define READ_TXSTATE1 (0x1e)
    #define READ_ORDER (0x20)
    #define READ_TXMCR (0x22)
    #define READ_ACKTMOUT (0x24)
    #define READ_SLALLOC (0x26)
    #define READ_SYMTICKL (0x28)
    #define READ_SYMTICKH (0x2A)
    #define READ_PAONTIME (0x2C)
    #define READ_PAONSETUP (0x2E)
    #define READ_FFOEN (0x30)
    #define READ_CSMACR (0x32)
    #define READ_TXBCNTRIG (0x34)
    #define READ_TXNMTRIG (0x36)
    #define READ_TXG1TRIG (0x38)
    #define READ_TXG2TRIG (0x3A)
    #define READ_ESLOTG23 (0x3C)
    #define READ_ESLOTG45 (0x3E)
    #define READ_ESLOTG67 (0x40)
    #define READ_TXPEND (0x42)
    #define READ_TXBCNINTL (0x44)
    #define READ_FRMOFFSET (0x46)
    #define READ_TXSR (0x48)
    #define READ_TXLERR (0x4A)
    #define READ_GATE_CLK (0x4C)
    #define READ_TXOFFSET (0x4E)
    #define READ_HSYMTMR0 (0x50)
    #define READ_HSYMTMR1 (0x52)
    #define READ_SOFTRST (0x54)
    #define READ_BISTCR (0x56)
    #define READ_SECCR0 (0x58)
    #define READ_SECCR1 (0x5A)
    #define READ_TXPEMISP (0x5C)
    #define READ_SECISR (0x5E)
    #define READ_RXSR (0x60)
    #define READ_ISRSTS (0x62)
    #define READ_INTMSK (0x64)
    #define READ_GPIO (0x66)
    #define READ_GPIODIR (0x68)
    #define READ_SLPACK (0x6A)
    #define READ_RFCTL (0x6C)
    #define READ_SECCR2 (0x6E)
    ;//#define  (0x70)
    #define READ_BBREG1 (0x72)
    #define READ_BBREG2 (0x74)
    #define READ_BBREG3 (0x76)
    #define READ_BBREG4 (0x78)
    #define READ_BBREG5 (0x7A)
    #define READ_BBREG6 (0x7C)
    #define READ_RSSITHCCA (0x7E)	
    
    ;; Long RAM Addresses
    ;;   Write
    #define WRITE_RFCTRL0_H b'11000000';(0x200)
    #define WRITE_RFCTRL0_L b'00010000'
    #define WRITE_RFCTRL1_H b'11000000';(0x201)
    #define WRITE_RFCTRL1_L b'00110000'
    #define WRITE_RFCTRL2_H b'11000000';(0x202)
    #define WRITE_RFCTRL2_L b'01010000'
    #define WRITE_RFCTRL3_H b'11000000';(0x203)
    #define WRITE_RFCTRL3_L b'01110000'
    #define WRITE_RFCTRL4_H b'11000000';(0x204)
    #define WRITE_RFCTRL4_L b'10010000'
    #define WRITE_RFCTRL5_H b'11000000';(0x205)
    #define WRITE_RFCTRL5_L b'10110000'
    #define WRITE_RFCTRL6_H b'11000000';(0x206)
    #define WRITE_RFCTRL6_L b'11010000'
    #define WRITE_RFCTRL7_H b'11000000';(0x207)
    #define WRITE_RFCTRL7_L b'11110000'
    #define WRITE_RFCTRL8_H b'11000001';(0x208)
    #define WRITE_RFCTRL8_L b'00010000'
    #define WRITE_CAL1_H b'11000001';(0x209)
    #define WRITE_CAL1_L b'00110000'
    #define WRITE_CAL2_H b'11000001';(0x20a)
    #define WRITE_CAL2_L b'01010000'
    #define WRITE_CAL3_H b'11000001';(0x20b)
    #define WRITE_CAL3_L b'01110000'
    #define WRITE_SFCNTRH_H b'11000001';(0x20c)
    #define WRITE_SFCNTRH_L b'10010000'
    #define WRITE_SFCNTRM_H b'11000001';(0x20d)
    #define WRITE_SFCNTRM_L b'10110000'
    #define WRITE_SFCNTRL_H b'11000001';(0x20e)
    #define WRITE_SFCNTRL_L b'11010000'
    #define WRITE_RFSTATE_H b'11000001';(0x20f)
    #define WRITE_RFSTATE_L b'11110000'
    #define WRITE_RSSI_H b'11000010';(0x210)
    #define WRITE_RSSI_L b'00010000'
    #define WRITE_CLKIRQCR_H b'11000010';(0x211)
    #define WRITE_CLKIRQCR_L b'00110000'
    #define WRITE_SRCADRMODE_H b'11000010';(0x212)
    #define WRITE_SRCADRMODE_L b'01010000'
    #define WRITE_SRCADDR0_H b'11000010';(0x213)
    #define WRITE_SRCADDR0_L b'01110000'
    #define WRITE_SRCADDR1_H b'11000010';(0x214)
    #define WRITE_SRCADDR1_L b'10010000'
    #define WRITE_SRCADDR2_H b'11000010';(0x215)
    #define WRITE_SRCADDR2_L b'10110000'
    #define WRITE_SRCADDR3_H b'11000010';(0x216)
    #define WRITE_SRCADDR3_L b'11010000'
    #define WRITE_SRCADDR4_H b'11000010';(0x217)
    #define WRITE_SRCADDR4_L b'11110000'
    #define WRITE_SRCADDR5_H b'11000011';(0x218)
    #define WRITE_SRCADDR5_L b'00010000'
    #define WRITE_SRCADDR6_H b'11000011';(0x219)
    #define WRITE_SRCADDR6_L b'00110000'
    #define WRITE_SRCADDR7_H b'11000011';(0x21a)
    #define WRITE_SRCADDR7_L b'01010000'
    #define WRITE_RXFRAMESTATE_H b'11000011';(0x21b)
    #define WRITE_RXFRAMESTATE_L b'01110000'
    #define WRITE_SECSTATUS_H b'11000011';(0x21c)
    #define WRITE_SECSTATUS_L b'10010000'
    #define WRITE_STCCMP_H b'11000011';(0x21d)
    #define WRITE_STCCMP_L b'10110000'
    #define WRITE_HLEN_H b'11000011';(0x21e)
    #define WRITE_HLEN_L b'11010000'
    #define WRITE_FLEN_H b'11000011';(0x21f)
    #define WRITE_FLEN_L b'11110000'
    #define WRITE_SCLKDIV_H b'11000100';(0x220)
    #define WRITE_SCLKDIV_L b'00010000'
    ;//#define reserved (0x221)
    #define WRITE_WAKETIMEL_H b'11000100';(0x222)
    #define WRITE_WAKETIMEL_L b'01010000'
    #define WRITE_WAKETIMEH_H b'11000100';(0x223)
    #define WRITE_WAKETIMEH_L b'01110000'
    #define WRITE_TXREMCNTL_H b'11000100';(0x224)
    #define WRITE_TXREMCNTL_L b'10010000'
    #define WRITE_TXREMCNTH_H b'11000100';(0x225)
    #define WRITE_TXREMCNTH_L b'10110000'
    #define WRITE_TXMAINCNTL_H b'11000100';(0x226)
    #define WRITE_TXMAINCNTL_L b'11010000'
    #define WRITE_TXMAINCNTM_H b'11000100';(0x227)
    #define WRITE_TXMAINCNTM_L b'11110000'
    #define WRITE_TXMAINCNTH0_H b'11000101';(0x228)
    #define WRITE_TXMAINCNTH0_L b'00010000'
    #define WRITE_TXMAINCNTH1_H b'11000101';(0x229)
    #define WRITE_TXMAINCNTH1_L b'00110000'
    #define WRITE_RFMANUALCTRLEN_H b'11000101';(0x22a)
    #define WRITE_RFMANUALCTRLEN_L b'01010000'
    #define WRITE_RFMANUALCTRL_H b'11000101';(0x22b)
    #define WRITE_RFMANUALCTRL_L b'01110000'
    #define WRITE_RFRXCTRL_H WRITE_RFMANUALCTRL_H
    #define WRITE_RFRXCTRL_L WRITE_RFMANUALCTRL_L
    #define WRITE_TxDACMANUALCTRL_H b'11000101';(0x22c)
    #define WRITE_TxDACMANUALCTRL_L b'10010000'
    #define WRITE_RFMANUALCTRL2_H b'11000101';(0x22d)
    #define WRITE_RFMANUALCTRL2_L b'10110000'
    #define WRITE_TESTRSSI_H b'11000101';(0x22e)
    #define WRITE_TESTRSSI_L b'11010000'
    #define WRITE_TESTMODE_H b'11000101';(0x22f)		
    #define WRITE_TESTMODE_L b'11110000'
    		
    ;;   Read
    #define READ_RFCTRL0_H b'11000000';(0x200)
    #define READ_RFCTRL0_L b'00000000'
    #define READ_RFCTRL1_H b'11000000';(0x201)
    #define READ_RFCTRL1_L b'00100000'
    #define READ_RFCTRL2_H b'11000000';(0x202)
    #define READ_RFCTRL2_L b'01000000'
    #define READ_RFCTRL3_H b'11000000';(0x203)
    #define READ_RFCTRL3_L b'01100000'
    #define READ_RFCTRL4_H b'11000000';(0x204)
    #define READ_RFCTRL4_L b'10000000'
    #define READ_RFCTRL5_H b'11000000';(0x205)
    #define READ_RFCTRL5_L b'10100000'
    #define READ_RFCTRL6_H b'11000000';(0x206)
    #define READ_RFCTRL6_L b'11000000'
    #define READ_RFCTRL7_H b'11000000';(0x207)
    #define READ_RFCTRL7_L b'11100000'
    #define READ_RFCTRL8_H b'11000001';(0x208)
    #define READ_RFCTRL8_L b'00000000'
    #define READ_CAL1_H b'11000001';(0x209)
    #define READ_CAL1_L b'00100000'
    #define READ_CAL2_H b'11000001';(0x20a)
    #define READ_CAL2_L b'01000000'
    #define READ_CAL3_H b'11000001';(0x20b)
    #define READ_CAL3_L b'01100000'
    #define READ_SFCNTRH_H b'11000001';(0x20c)
    #define READ_SFCNTRH_L b'10000000'
    #define READ_SFCNTRM_H b'11000001';(0x20d)
    #define READ_SFCNTRM_L b'10100000'
    #define READ_SFCNTRL_H b'11000001';(0x20e)
    #define READ_SFCNTRL_L b'11000000'
    #define READ_RFSTATE_H b'11000001';(0x20f)
    #define READ_RFSTATE_L b'11100000'
    #define READ_RSSI_H b'11000010';(0x210)
    #define READ_RSSI_L b'00000000'
    #define READ_CLKIRQCR_H b'11000010';(0x211)
    #define READ_CLKIRQCR_L b'00100000'
    #define READ_SRCADRMODE_H b'11000010';(0x212)
    #define READ_SRCADRMODE_L b'01000000'
    #define READ_SRCADDR0_H b'11000010';(0x213)
    #define READ_SRCADDR0_L b'01100000'
    #define READ_SRCADDR1_H b'11000010';(0x214)
    #define READ_SRCADDR1_L b'10000000'
    #define READ_SRCADDR2_H b'11000010';(0x215)
    #define READ_SRCADDR2_L b'10100000'
    #define READ_SRCADDR3_H b'11000010';(0x216)
    #define READ_SRCADDR3_L b'11000000'
    #define READ_SRCADDR4_H b'11000010';(0x217)
    #define READ_SRCADDR4_L b'11100000'
    #define READ_SRCADDR5_H b'11000011';(0x218)
    #define READ_SRCADDR5_L b'00000000'
    #define READ_SRCADDR6_H b'11000011';(0x219)
    #define READ_SRCADDR6_L b'00100000'
    #define READ_SRCADDR7_H b'11000011';(0x21a)
    #define READ_SRCADDR7_L b'01000000'
    #define READ_RXFRAMESTATE_H b'11000011';(0x21b)
    #define READ_RXFRAMESTATE_L b'01100000'
    #define READ_SECSTATUS_H b'11000011';(0x21c)
    #define READ_SECSTATUS_L b'10000000'
    #define READ_STCCMP_H b'11000011';(0x21d)
    #define READ_STCCMP_L b'10100000'
    #define READ_HLEN_H b'11000011';(0x21e)
    #define READ_HLEN_L b'11000000'
    #define READ_FLEN_H b'11000011';(0x21f)
    #define READ_FLEN_L b'11100000'
    #define READ_SCLKDIV_H b'11000100';(0x220)
    #define READ_SCLKDIV_L b'00000000'
    ;//#define reserved (0x221)
    #define READ_WAKETIMEL_H b'11000100';(0x222)
    #define READ_WAKETIMEL_L b'01000000'
    #define READ_WAKETIMEH_H b'11000100';(0x223)
    #define READ_WAKETIMEH_L b'01100000'
    #define READ_TXREMCNTL_H b'11000100';(0x224)
    #define READ_TXREMCNTL_L b'10000000'
    #define READ_TXREMCNTH_H b'11000100';(0x225)
    #define READ_TXREMCNTH_L b'10100000'
    #define READ_TXMAINCNTL_H b'11000100';(0x226)
    #define READ_TXMAINCNTL_L b'11000000'
    #define READ_TXMAINCNTM_H b'11000100';(0x227)
    #define READ_TXMAINCNTM_L b'11100000'
    #define READ_TXMAINCNTH0_H b'11000101';(0x228)
    #define READ_TXMAINCNTH0_L b'00000000'
    #define READ_TXMAINCNTH1_H b'11000101';(0x229)
    #define READ_TXMAINCNTH1_L b'00100000'
    #define READ_RFMANUALCTRLEN_H b'11000101';(0x22a)
    #define READ_RFMANUALCTRLEN_L b'01000000'
    #define READ_RFMANUALCTRL_H b'11000101';(0x22b)
    #define READ_RFMANUALCTRL_L b'01100000'
    #define READ_RFRXCTRL_H READ_RFMANUALCTRL_H
    #define READ_RFRXCTRL_L READ_RFMANUALCTRL_L
    #define READ_TxDACMANUALCTRL_H b'11000101';(0x22c)
    #define READ_TxDACMANUALCTRL_L b'10000000'
    #define READ_RFMANUALCTRL2_H b'11000101';(0x22d)
    #define READ_RFMANUALCTRL2_L b'10100000'
    #define READ_TESTRSSI_H b'11000101';(0x22e)
    #define READ_TESTRSSI_L b'11000000'
    #define READ_TESTMODE_H b'11000101';(0x22f)		
    #define READ_TESTMODE_L b'11100000'
    
    
    ;; Channel Definitions
    #define CHANNEL_11 0x02
    #define CHANNEL_12 0x12
    #define CHANNEL_13 0x22
    #define CHANNEL_14 0x32
    #define CHANNEL_15 0x42
    #define CHANNEL_16 0x52
    #define CHANNEL_17 0x62
    #define CHANNEL_18 0x72
    #define CHANNEL_19 0x82
    #define CHANNEL_20 0x92
    #define CHANNEL_21 0xa2
    #define CHANNEL_22 0xb2
    #define CHANNEL_23 0xc2
    #define CHANNEL_24 0xd2
    #define CHANNEL_25 0xe2
    #define CHANNEL_26 0xf2
    
#endif
