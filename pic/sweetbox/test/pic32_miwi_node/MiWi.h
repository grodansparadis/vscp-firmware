/********************************************************************
* FileName:		MiWi.h
* Dependencies: SymbolTime.h
* Processor:	PIC18, PIC24F, PIC24H, dsPIC30, dsPIC33
*               tested with 18F4620, dsPIC33FJ256GP710	
* Hardware:		PICDEM Z, Explorer 16
* Complier:     Microchip C18 v3.04 or higher
*				Microchip C30 v2.03 or higher	
* Company:		Microchip Technology, Inc.
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
*  This file provides the interface into the MiWi stack
*
* Change History:
*  Rev   Date         Description
*  0.1   11/09/2006   Initial revision
*  1.0   01/09/2007   Initial release
********************************************************************/

#ifndef __MIWI_H_
#define __MIWI_H_

/************************ HEADERS **********************************/
#include "MiWiDefs.h"
#include "SymbolTime.h"

/************************ FUNCTION PROTOTYPES **********************/
void MiWiInit(void);
void MiWiTasks(void);
void DiscardPacket(void);
#ifdef SUPPORT_SECURITY
	BOOL SendTxBuffer(BOOL AckRequested, BOOL SecurityOn);
#else
	BOOL SendTxBuffer(BOOL AckRequested);
#endif	
BYTE findNextNetworkEntry(void);
void DumpNetworkTable(void);
void SetChannel(BYTE channel);
void ClearNetworkTable(BYTE options);
void FormNetwork(WORD PANID);
void JoinNetwork(BYTE handle);
BOOL SendIndirectMessage(BYTE handle);
BYTE SearchForLongAddress(void);
BYTE SearchForShortAddress(BYTE entryType);   
void SendIndirectPacket(BYTE handle, BYTE dataRequestType);
BYTE AddNodeToNetworkTable(void);
BYTE SendReportByShortAddress(WORD_VAL PANID, WORD_VAL ShortAddress, BOOL forwardPacket);
BYTE SendReportByLongAddress(BYTE *pLongAddress);
BYTE SendReportByHandle(BYTE handle, BOOL forwardPacket);
void DiscoverNodeByEUI(void);
void OpenSocket(BYTE socketType);
void initMRF24J40(void);
void MRF24J40Sleep(void);
void MRF24J40Wake(void);

void PHYSetLongRAMAddr(WORD address, BYTE value);
void PHYSetShortRAMAddr(BYTE address, BYTE value);
BYTE PHYGetShortRAMAddr(BYTE address);
BYTE PHYGetLongRAMAddr(WORD address);

void PHYDumpRegisters(void);
void PHYDumpTxFIFO(void);

/************************ DEFINITIONS ******************************/
#ifdef __PIC32MX
    #define FRAME_TYPE_COMMAND  0x03
    #define FRAME_TYPE_DATA     0x01
    #define FRAME_TYPE_BEACON   0x00
    #define FRAME_TYPE_ACK      0x02
#else
    #define FRAME_TYPE_COMMAND  0b011
    #define FRAME_TYPE_DATA     0b001
    #define FRAME_TYPE_BEACON   0b000
    #define FRAME_TYPE_ACK      0b010
#endif

#define MAC_COMMAND_ASSOCIATION_REQUEST             0x01
#define MAC_COMMAND_ASSOCIATION_RESPONSE            0x02
    #define ASSOCIATION_SUCCESSFUL                  0x00
    #define ASSOCIATION_PAN_FULL                    0x01
    #define ASSOCIATION_ACCESS_DENIED               0x02
#define MAC_COMMAND_DISASSOCIATION_NOTIFICATION     0x03
#define MAC_COMMAND_DATA_REQUEST                    0x04
#define MAC_COMMAND_PAN_ID_CONFLICT_NOTIFICATION    0x05
#define MAC_COMMAND_ORPHAN_NOTIFICATION             0x06
#define MAC_COMMAND_BEACON_REQUEST                  0x07
#define MAC_COMMAND_COORDINATOR_REALIGNMENT         0x08

#define MIWI_PROTOCOL_ID 0x4D
#define MIWI_VERSION_NUM 0x10   //v1.0
#define MIWI_ACK_REQ 0x04

#ifdef __PIC32MX
    #define CLEAR_ALL_ENTRIES 0xFF
    #define CLEAR_NON_DIRECT_CONNECTIONS 0x01
    #define CLEAR_NO_LONG_ADDRESS 0x02
    #define CLEAR_NO_SHORT_ADDRESS 0x04  //this does not effect P2P nodes
    #define CLEAR_P2P 0x08
    #define CLEAR_NETWORKS 0x10
    #define CLEAR_NEIGHBORS 0x20
#else
    #define CLEAR_ALL_ENTRIES 0b11111111
    #define CLEAR_NON_DIRECT_CONNECTIONS 0b00000001
    #define CLEAR_NO_LONG_ADDRESS 0b00000010
    #define CLEAR_NO_SHORT_ADDRESS 0b00000100  //this does not effect P2P nodes
    #define CLEAR_P2P 0b00001000
    #define CLEAR_NETWORKS 0b00010000
    #define CLEAR_NEIGHBORS 0b00100000
#endif

#define NETWORK 0
#define NEIGHBOR 1

#define aResponseWaitTime 30720   //(16*32*60)

#define CLUSTER_SOCKET 0x00
#define P2P_SOCKET 0x01

#define ROLE_FFD_END_DEVICE 0x00
#define ROLE_COORDINATOR 0x01
#define ROLE_PAN_COORDINATOR 0x02

#define DATA_REQUEST_ASSOCIATION_RESPONSE 0x00
#define DATA_REQUEST_SHORT_ADDRESSES 0x01

#define NETWORKED_TRANSMISSION 0x00
#define P2P_TRANSMISSION 0x01
#define BY_HANDLE_TRANSMISSION 0x02
#define BY_LONG_ADDRESS_TRANSMISSION 0x04

/* Report type and ID definitions */
/* as a user you are able to use Report types 0x10 - 0xFF */

#define MIWI_STACK_REPORT_TYPE 0x00
    #define OPEN_CLUSTER_SOCKET_REQUEST 0x10
    #define OPEN_CLUSTER_SOCKET_RESPONSE 0x11
    #define OPEN_P2P_SOCKET_REQUEST 0x12
    #define OPEN_P2P_SOCKET_RESPONSE 0x13
    #define EUI_ADDRESS_SEARCH_REQUEST 0x20
    #define EUI_ADDRESS_SEARCH_RESPONSE 0x21
    #define ACK_REPORT_TYPE 0x30
/* Report Type 0x06-0x0F are reserved */
#define USER_REPORT_TYPE 0x12
    #define LIGHT_REPORT 0x34
        #define LIGHT_TOGGLE 0x55
        #define LIGHT_OFF 0x00
        #define LIGHT_ON 0xFF

#define CIPHER_RETRY 3

/************************ DATA TYPE *******************************/

typedef union _MAC_FRAME_CONTROL
{
    BYTE Val;
    struct _MAC_FRAME_CONTROL_bits
    {
        BYTE frameType :3;
        BYTE securityEnabled :1;
        BYTE framePending :1;
        BYTE ACKRequest :1;
        BYTE intraPAN :1;
        BYTE :1;
    } bits;
} MAC_FRAME_CONTROL;

typedef union _BEACON_SUPERFRAME
{
    WORD_VAL word;
    
    struct _BEACON_SUPERFRAME_bits
    {
        BYTE beaconOrder        :4;
        BYTE superframeOrder    :4;
        BYTE finalCAPslot       :4;
        BYTE batteryLifeExt     :1;
        BYTE :1;
        BYTE PANCoordinator     :1;
        BYTE associationPermit  :1;
    } bits;
} BEACON_SUPERFRAME;
    

typedef union _SEND_REPORT_STATUS
{
    struct _SEND_REPORT_STATUS_bits
    {
        BYTE inUse :1;
        BYTE macAckValid :1;
        BYTE macAck :1;
        BYTE appAckValid :1;
        BYTE appAck :1;
    } bits;
    BYTE Val;
} SEND_REPORT_STATUS;

typedef union _MRF24J40_IFS
{
    BYTE Val;
    struct _MRF24J40_IFS_bits
    {
        BYTE RF_TXIF :1;
        BYTE :2;
        BYTE RF_RXIF :1;
//        BYTE SECIF :1;
        BYTE :4;
    }bits;
} MRF24J40_IFREG;

typedef union _NETWORK_TABLE_ENTRY_status
{
    BYTE Val;
    struct _NETWORK_TABLE_ENTRY_bits
    {
        BYTE isValid            :1;     //1 = this entry is valid, 0 = this entry is not valid
        BYTE directConnection   :1;     //1 = can talk to this device directly, 0 = must route to this device
        BYTE longAddressValid   :1;     //1 = long address valid, 0 = long address unknown
        BYTE shortAddressValid  :1;     //1 = short address valid, 0 = short address unknown
        BYTE P2PConnection      :1;     //1 = P2P connection, 0 = network device
        BYTE NeighborOrNetwork  :1;     //1 = it is a neighbor, 0 = it is a network
        BYTE RXOnWhenIdle       :1;     //1 = RxOnWhenIdle , 0 = RxOffWhenIdle
        BYTE isFamily           :1;     //1 = either my parent or a child, 0 = someone else
    } bits;

} NETWORK_STATUS_INFO;


typedef struct _NETWORK_TABLE_ENTRY
{
    //NETWORK_STATUS_INFO status;
    WORD_VAL PANID;
    WORD_VAL ShortAddress;
#ifdef SUPPORT_SECURITY    
    DWORD_VAL IncomingFrameCounter;
#elif !defined(__C30__)
    BYTE filler[4];
#endif    
    union _NETWORK_TABLE_ENTRY_INFO
    {
        BYTE LongAddress[8];
        struct _NETWORK_TABLE_ENTRY_INFO_NETWORK
        {
            BYTE Channel;
            BYTE sampleRSSI;
            BYTE Protocol;
            BYTE Version;
            union _NETWORK_TABLE_ENTRY_INFO_NETWORK_STRUCT
            {
                BYTE Val;
                struct _NETWORK_TABLE_ENTRY_INFO_NETWORK_STRUCT_bits
                {
                    BYTE :6;
                    BYTE PANcoordinator     :1;
                    BYTE associationPermit  :1;
                } bits;
            } flags;
        } networkInfo;
    } info;
} NETWORK_TABLE_ENTRY;

typedef struct _MIWI_STATE_MACHINE
{
    BYTE searchingForNetwork    :1;
    BYTE attemptingToJoin       :1;
    BYTE memberOfNetwork        :1;
    BYTE beaconRequestSent      :1;
    BYTE dataRequestSent        :1;
    BYTE attemptingToRejoin     :1;
    BYTE RxHasUserData          :1;
    BYTE dataRequestNeeded      :1;
    BYTE dataRequestComplete    :1;
} MIWI_STATE_MACHINE;

typedef union _MIWI_STATUS
{
    WORD Val;
    struct _MIWI_STATUS_bits
    {
        BYTE RX_PENDING         :1;
        BYTE RX_BUFFERED        :1;
        BYTE RX_ENABLED         :1;
        BYTE TX_BUSY            :1;
        BYTE TX_PENDING_ACK     :1;
        BYTE TX_SECURITY		:1;
        BYTE PHY_SLEEPING       :1;
    }bits;
} MIWI_STATUS;

typedef union _DEST_INFO
{
    BYTE *pLongAddr;
    BYTE handle;
    struct _DEST_INFO_NETWORKED
    {
        WORD_VAL PANID;
        WORD_VAL ShortAddress;
    } network;
} DEST_INFO;

typedef struct _OPEN_SOCKET
{
    union _OPEN_SOCKET_STATUS
    {
        struct _OPEN_SOCKET_STATUS_bits
        {
            BYTE matchFound :1;
            BYTE requestIsOpen :1;
            BYTE itIsMe         :1;
            BYTE type :1;           //CLUSTER_SOCKET = cluster socket, P2P_SOCKET = P2P socket
        } bits;
        BYTE Val;
    } status;
    BYTE socketHandle;
    WORD_VAL ShortAddress1;
    #if defined(I_AM_COORDINATOR_CAPABLE) 
        WORD_VAL ShortAddress2;
        BYTE LongAddress2[8];
    #endif
    BYTE LongAddress1[8];
    TICK socketStart;
} OPEN_SOCKET;

#if defined(SUPPORT_SECURITY)
typedef enum _CIPHER_MODE
{
	MODE_ENCRYPTION,
	MODE_DECRYPTION
} CIPHER_MODE;

typedef enum _CIPHER_STATUS
{
	CIPHER_SUCCESS = 0,
	CIPHER_ERROR,
	CIPHER_MIC_ERROR
} CIPHER_STATUS;

typedef struct _SECURITY_INPUT
{
	BYTE	*InputText;
	BYTE	TextLen;
	BYTE	*Header;
	BYTE	HeaderLen;
	BYTE	SourceAddress[8];
	DWORD_VAL	FrameCounter;
} SECURITY_INPUT;
#endif //#if defined(SUPPORT_SECURITY)
/************************ EXTERNAL VARIABLES **********************/

extern MIWI_STATE_MACHINE MiWiStateMachine;
extern BYTE currentSearchChannel;
extern TICK startTick;
extern NETWORK_TABLE_ENTRY networkTable[NETWORK_TABLE_SIZE];
extern NETWORK_STATUS_INFO networkStatus[NETWORK_TABLE_SIZE];
extern BYTE TxBuffer[];
extern BYTE TxData;
extern WORD_VAL myPANID;
extern WORD_VAL myShortAddress;
extern BYTE *pRxData;
extern BYTE RxSize;
extern BYTE myParent;
extern BYTE tempLongAddress[8];
extern WORD_VAL tempShortAddress;
extern OPEN_SOCKET openSocketInfo;
extern BYTE currentRxByte;
extern BYTE currentTxByte;
extern BYTE phyIsIdle;

/************************ MACROS **********************************/
#define DiscoverNetworks()  MiWiStateMachine.searchingForNetwork = 1;\
                            MiWiStateMachine.beaconRequestSent = 1;\
                            currentSearchChannel = 0;\
                            startTick = TickGet();
#define RejoinNetwork()     MiWiStateMachine.attemptingToRejoin = 1;\
                            MiWiStateMachine.beaconRequestSent = 1;\
                            currentSearchChannel = 0;\
                            startTick = TickGet();
#define SearchingForNetworks() MiWiStateMachine.searchingForNetwork
#define MemberOfNetwork() MiWiStateMachine.memberOfNetwork
#define AttemptingToJoinNetwork() MiWiStateMachine.attemptingToJoin
#define AttemptingToRejoinNetwork() MiWiStateMachine.attemptingToRejoin
#define RxPacket() MiWiStateMachine.RxHasUserData
#define ClearToSend() !MiWiStatus.bits.TX_BUSY
#define OpenSocketComplete() (!openSocketInfo.status.bits.requestIsOpen)
#define OpenSocketSuccessful() openSocketInfo.status.bits.matchFound
#define OpenSocketHandle() openSocketInfo.socketHandle
#define CheckForData() MiWiStateMachine.dataRequestNeeded = 1;
#define CheckForDataComplete() MiWiStateMachine.dataRequestComplete
#define WriteData(a) TxBuffer[TxData++] = a
#define FlushTx() {TxData = 0; TxHeader = TX_BUFFER_SIZE - 1;}
#define TxPayLoad() TxData = 11    
    
#endif

    
    
