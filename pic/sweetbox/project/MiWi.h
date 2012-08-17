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
#include "GenericTypedefs.h"
#include "sweetbox.h"
#include "miwi.def"

/************************ FUNCTION PROTOTYPES **********************/
void MiWiInit(void);
void MiWiTasks(void);
void DiscardPacket(void);
#ifdef SUPPORT_SECURITY
	BOOL SendTxBuffer( BOOL AckRequested, BOOL SecurityOn );
#else
	BOOL SendTxBuffer( BOOL AckRequested );
#endif	
uint8_t findNextNetworkEntry(void);
void DumpNetworkTable(void);
void SetChannel(uint8_t channel);
void ClearNetworkTable(uint8_t options);
void FormNetwork(uint16_t PANID);
void JoinNetwork(uint8_t handle);
BOOL SendIndirectMessage(uint8_t handle);
uint8_t SearchForLongAddress(void);
uint8_t SearchForShortAddress(uint8_t entryType);   
void SendIndirectPacket(uint8_t handle, uint8_t dataRequestType);
uint8_t AddNodeToNetworkTable(void);
uint8_t SendReportByShortAddress(WORD_VAL PANID, WORD_VAL ShortAddress, BOOL forwardPacket);
uint8_t SendReportByLongAddress(uint8_t *pLongAddress);
uint8_t SendReportByHandle(uint8_t handle, BOOL forwardPacket);
void DiscoverNodeByEUI(void);
void OpenSocket(uint8_t socketType);
void initMRF24J40(void);
void MRF24J40Sleep(void);
void MRF24J40Wake(void);

void PHYSetLongRAMAddr(uint16_t address, uint8_t value);
void PHYSetShortRAMAddr(uint8_t address, uint8_t value);
uint8_t PHYGetShortRAMAddr(uint8_t address);
uint8_t PHYGetLongRAMAddr(uint16_t address);

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
    #define ASSOCIATION_SUCCESSFUL                      0x00
    #define ASSOCIATION_PAN_FULL                        0x01
    #define ASSOCIATION_ACCESS_DENIED                   0x02
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
    uint8_t Val;
    struct _MAC_FRAME_CONTROL_bits
    {
        uint8_t frameType :3;
        uint8_t securityEnabled :1;
        uint8_t framePending :1;
        uint8_t ACKRequest :1;
        uint8_t intraPAN :1;
        uint8_t :1;
    } bits;
} MAC_FRAME_CONTROL;

typedef union _BEACON_SUPERFRAME
{
    WORD_VAL word;
    
    struct _BEACON_SUPERFRAME_bits
    {
        uint8_t beaconOrder        :4;
        uint8_t superframeOrder    :4;
        uint8_t finalCAPslot       :4;
        uint8_t batteryLifeExt     :1;
        uint8_t :1;
        uint8_t PANCoordinator     :1;
        uint8_t associationPermit  :1;
    } bits;
} BEACON_SUPERFRAME;
    

typedef union _SEND_REPORT_STATUS
{
    struct _SEND_REPORT_STATUS_bits
    {
        uint8_t inUse :1;
        uint8_t macAckValid :1;
        uint8_t macAck :1;
        uint8_t appAckValid :1;
        uint8_t appAck :1;
    } bits;
    uint8_t Val;
} SEND_REPORT_STATUS;

typedef union _MRF24J40_IFS
{
    uint8_t Val;
    struct _MRF24J40_IFS_bits
    {
        uint8_t RF_TXIF :1;
        uint8_t :2;
        uint8_t RF_RXIF :1;
//        uint8_t SECIF :1;
        uint8_t :4;
    }bits;
} MRF24J40_IFREG;

typedef union _NETWORK_TABLE_ENTRY_status
{
    uint8_t Val;
    struct _NETWORK_TABLE_ENTRY_bits
    {
        uint8_t isValid            :1;     //1 = this entry is valid, 0 = this entry is not valid
        uint8_t directConnection   :1;     //1 = can talk to this device directly, 0 = must route to this device
        uint8_t longAddressValid   :1;     //1 = long address valid, 0 = long address unknown
        uint8_t shortAddressValid  :1;     //1 = short address valid, 0 = short address unknown
        uint8_t P2PConnection      :1;     //1 = P2P connection, 0 = network device
        uint8_t NeighborOrNetwork  :1;     //1 = it is a neighbor, 0 = it is a network
        uint8_t RXOnWhenIdle       :1;     //1 = RxOnWhenIdle , 0 = RxOffWhenIdle
        uint8_t isFamily           :1;     //1 = either my parent or a child, 0 = someone else
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
    uint8_t filler[4];
#endif    
    union _NETWORK_TABLE_ENTRY_INFO
    {
        uint8_t LongAddress[8];
        struct _NETWORK_TABLE_ENTRY_INFO_NETWORK
        {
            uint8_t Channel;
            uint8_t sampleRSSI;
            uint8_t Protocol;
            uint8_t Version;
            union _NETWORK_TABLE_ENTRY_INFO_NETWORK_STRUCT
            {
                uint8_t Val;
                struct _NETWORK_TABLE_ENTRY_INFO_NETWORK_STRUCT_bits
                {
                    uint8_t :6;
                    uint8_t PANcoordinator     :1;
                    uint8_t associationPermit  :1;
                } bits;
            } flags;
        } networkInfo;
    } info;
} NETWORK_TABLE_ENTRY;

typedef struct _MIWI_STATE_MACHINE
{
    uint8_t searchingForNetwork    :1;
    uint8_t attemptingToJoin       :1;
    uint8_t memberOfNetwork        :1;
    uint8_t beaconRequestSent      :1;
    uint8_t dataRequestSent        :1;
    uint8_t attemptingToRejoin     :1;
    uint8_t RxHasUserData          :1;
    uint8_t dataRequestNeeded      :1;
    uint8_t dataRequestComplete    :1;
} MIWI_STATE_MACHINE;

typedef union _MIWI_STATUS
{
    uint16_t Val;
    struct _MIWI_STATUS_bits
    {
        uint8_t RX_PENDING         :1;
        uint8_t RX_BUFFERED        :1;
        uint8_t RX_ENABLED         :1;
        uint8_t TX_BUSY            :1;
        uint8_t TX_PENDING_ACK     :1;
        uint8_t TX_SECURITY		   :1;
        uint8_t PHY_SLEEPING       :1;
    }bits;
} MIWI_STATUS;

typedef union _DEST_INFO
{
    uint8_t *pLongAddr;
    uint8_t handle;
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
            uint8_t matchFound :1;
            uint8_t requestIsOpen :1;
            uint8_t itIsMe         :1;
            uint8_t type :1;           //CLUSTER_SOCKET = cluster socket, P2P_SOCKET = P2P socket
        } bits;
        uint8_t Val;
    } status;
    uint8_t socketHandle;
    WORD_VAL ShortAddress1;
    #if defined(I_AM_COORDINATOR_CAPABLE) 
        WORD_VAL ShortAddress2;
        uint8_t LongAddress2[8];
    #endif
    uint8_t LongAddress1[8];
    uint32_t socketStart;
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
	uint8_t	*InputText;
	uint8_t	TextLen;
	uint8_t	*Header;
	uint8_t	HeaderLen;
	uint8_t	SourceAddress[8];
	DWORD_VAL	FrameCounter;
} SECURITY_INPUT;
#endif //#if defined(SUPPORT_SECURITY)
/************************ EXTERNAL VARIABLES **********************/

extern MIWI_STATE_MACHINE MiWiStateMachine;
extern uint8_t currentSearchChannel;
extern TICK startTick;
extern NETWORK_TABLE_ENTRY networkTable[NETWORK_TABLE_SIZE];
extern NETWORK_STATUS_INFO networkStatus[NETWORK_TABLE_SIZE];
extern uint8_t TxBuffer[];
extern uint8_t TxData;
extern WORD_VAL myPANID;
extern WORD_VAL myShortAddress;
extern uint8_t *pRxData;
extern uint8_t RxSize;
extern uint8_t myParent;
extern uint8_t tempLongAddress[8];
extern WORD_VAL tempShortAddress;
extern OPEN_SOCKET openSocketInfo;
extern uint8_t currentRxByte;
extern uint8_t currentTxByte;
extern uint8_t phyIsIdle;

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

    
    
