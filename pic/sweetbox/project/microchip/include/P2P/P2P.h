/********************************************************************
* FileName:		P2P.h
* Dependencies: SymbolTime.h
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
*  This is the header file for P2P stack.  
*
* Change History:
*  Rev   Date         Author    Description
*  0.1   1/03/2008    yfy       Initial revision
********************************************************************/

#ifndef __P2P_H_
#define __P2P_H_

#define INPUT
#define OUTPUT
#define IOPUT

/************************ HEADERS **********************************/
#include "P2PDefs.h"
#include ".\P2P\SymbolTime.h"


/************************ DEFINITIONS ******************************/

#define STATUS_SUCCESS              0x00
#define STATUS_EXISTS               0x01
#define STATUS_ACTIVE_SCAN          0x02
#define STATUS_ENTRY_NOT_EXIST      0xF0
#define STATUS_NOT_ENOUGH_SPACE     0xF1
#define STATUS_NOT_SAME_PAN         0xF2

#define CMD_P2P_CONNECTION_REQUEST              0x81
#define CMD_P2P_CONNECTION_REMOVAL_REQUEST      0x82
#define CMD_DATA_REQUEST                        0x83
#define CMD_CHANNEL_HOPPING                     0x84
#define CMD_TIME_SYNCHRONIZATION_REQUEST        0x85
#define CMD_TIME_SYNCHRONIZATION_NOTIFICATION   0x86

#define CMD_P2P_CONNECTION_RESPONSE             0x91
#define CMD_P2P_CONNECTION_REMOVAL_RESPONSE     0x92

#define CMD_MAC_DATA_REQUEST                    0x04

#define RFD_DATA_WAIT 0x00003FFF

#ifdef ENABLE_FREQUENCY_AGILITY
    #define FA_BROADCAST_TIME           0x03
    #define CCA_FAILURE_TIMES           0x03
    #define ACK_FAILURE_TIMES           0x03
    #define RESYNC_TIMES                0x03
#endif

#if defined(ENABLE_FREQUENCY_AGILITY) || defined(ENABLE_SLEEP)    
    #define VERIFY_TRANSMIT
#endif

/************************ DATA TYPE *******************************/
 

/*******************************************************************
 * Overview: The interpolation of MRF24J40 radio interrupts. Details of 
 * interrupts can be found in MRF24J40 data sheet
 ******************************************************************/
typedef union 
{
    BYTE Val;               // value of interrupts
    struct 
    {
        BYTE RF_TXIF :1;    // transmission finish interrupt
        BYTE :2;
        BYTE RF_RXIF :1;    // receiving a packet interrupt
        BYTE SECIF :1;      // receiving a secured packet interrupt
        BYTE :4;
    }bits;                  // bit map of interrupts
} MRF24J40_IFREG;


/******************************************************************
 * Overview: The record to store the information of peer device in the
 * MiWi(TM) P2P Connection
 *****************************************************************/
typedef struct 
{
    BYTE        PeerLongAddress[8];         // the long address of the peer device
    #if defined(ENABLE_SECURITY) && !defined(TARGET_SMALL)
        DWORD_VAL   IncomingFrameCounter;   // the incoming frame counter. Used to check
                                            // frame freshness to avoid repeat attack
                                            // only valid if security is enabled
    #endif
    BYTE        PeerInfo[1+ADDITIONAL_CONNECTION_PAYLOAD];  // The peer information. The first byte
                                                            // is the standard peer information. The
                                                            // detailed definition of the first byte
                                                            // can be found in P2P_CAPACITY definition.
                                                            // Additional info can be added according
                                                            // to the specific application
} P2P_CONNECTION_ENTRY;     


/******************************************************************
 * Overview: The capacity information for a MiWi(TM) P2P device. It is the 
 * definition of the first byte of PeerInfo defined in 
 * P2P_CONNECTION_ENTRY. The highest bit also be used to indicate 
 * if the P2P connection entry is a valid entry
 *****************************************************************/
typedef union 
{
    BYTE    Val;                            // the value of the P2P capacitity 
    struct _P2P_CAPACITY_BITS
    {
        BYTE    RXOnWhileIdel       : 1;    // if device turns on radio when idle
        BYTE    DataRequestNeeded   : 1;    // if data request is required when device turns off radio when 
                                            // idle. It is used to decide if an indirect message is necessary
                                            // to be stored.
        BYTE    TimeSynchronization : 1;    // reserved bit for future development
        BYTE    SecurityCapacity    : 1;    // if the device is capable of handling encrypted information
        BYTE    filler              : 3;
        BYTE    isValid             : 1;    // use this bit to indicate that this entry is a valid entry
    } bits;
} P2P_CAPACITY;         

/******************************************************************
 * Overview: The structure that stores all information about the received 
 * frame, provided to the upper layer of the MiWi(TM) P2P stack
 *****************************************************************/
typedef struct 
{
    union 
    {
        BYTE    Val;                        // value of the flags for RECEIVED_FRAME
        struct 
        {
            BYTE    commandFrame    : 1;    // if the received frame a command frame. data: 0; command 1
            BYTE    security        : 1;    // if the received frame is encrypted during transmission.
                                            // The payload in this structure has already been decrypted
            BYTE    framePending    : 1;    // if the frame pending bit has been set in the frame control
                                            // this bit is hardly used, just a legancy inherited from
                                            // IEEE 802.15.4
            BYTE    intraPAN        : 1;    // if the received frame a intra-PAN frame. Meaning if the 
                                            // the source PAN ID the same as the destination PAN ID
            BYTE    broadcast       : 1;    // if the received frame a broadcast message
        } bits;                             // bit map of the flags for received frame
    } flags;                                // received frame flags
    
    #ifndef TARGET_SMALL
        BYTE        PacketLQI;              // the link quality indication for the received packet
                                            // that indicats the quality of the received frame
        BYTE        PacketRSSI;             // the RSSI of for the received packet that indicates 
                                            // the signal strength of the received frame
        WORD_VAL    SourcePANID;            // The PAN identifier of the source device
    #endif
    BYTE        SourceLongAddress[8];       // The long address of the source device
    BYTE        PayLoadSize;                // The size of the payload
    BYTE        *PayLoad;                   // The pointer to the pay load of the received packet. 
                                            // The pay load is the MAC payload without the MAC header
} RECEIVED_FRAME;                            


/******************************************************************
 * Overview: The structure to store indirect messages for devices turn off 
 * radio when idle
 *****************************************************************/
typedef struct 
{
    TICK        TickStart;      // start time of the indirect message. Used for checking 
                                // indirect message time out
    WORD_VAL    DestPANID;      // the PAN identifier for the destination node
    union 
    {
        BYTE        DestLongAddress[8];             // unicast destination long address
        BYTE        DestIndex[P2P_CONNECTION_SIZE]; // broadcast index of the P2P Connection Entries 
                                                    // for destination RFD devices
    } DestAddress;  // destination address for the indirect message. Can either for unicast or broadcast
    union 
    {
        BYTE    Val;                        // value for the flags
        struct 
        {
            BYTE    isValid         : 1;    // if this indirect message is valid
            BYTE    isBroadcast     : 1;    // if this indirect message is for broadcasting
            BYTE    isCommand       : 1;    // if this indirect message a command
            BYTE    isSecured       : 1;    // if this indirect message requires encryption
        } bits;                             // bit map of the flags
    } flags;                                // flags for indirect message
    BYTE        PayLoadSize;                // the indirect message pay load size
    BYTE        PayLoad[TX_BUFFER_SIZE-21]; // the indirect message pay load
} INDIRECT_MESSAGE; 


/******************************************************************
 * Overview: structure to indicate the status of P2P stack
 *****************************************************************/
typedef union
{
    WORD Val;                               // The value of the P2P status flags
    struct 
    {
        BYTE RX_PENDING             :1;     // indicate if a frame is received and pending to read
        BYTE RX_BUFFERED            :1;     // indicate if a frame has been read into the buffer
                                            // of MCU and waiting to be processed
        BYTE RX_ENABLED             :1;     // indicate if the radio is enabled to receive the 
                                            // next packet
        BYTE RX_IGNORE_SECURITY     :1;     // indicate if we need to ignore the current packet
                                            // with encryption. Usually, the reason is frame counter
                                            // freshness checking fails
        BYTE RX_SECURITY            :1;     // indicate if current received packet is encrypted
        BYTE TX_BUSY                :1;     // indicate if the radio is currently busy transmitting
                                            // a packet
        BYTE TX_PENDING_ACK         :1;     // indicate if the current transmitting packet waiting
                                            // for an acknowledgement from the peer device
        BYTE PHY_SLEEPING           :1;     // indicate if the device in sleeping state
        BYTE TimeToSleep            :1;     // used by the application layer to indicate that the RFD
                                            // device is idle and ready to go to sleep
        BYTE DataRequesting         :1;     // indicate that device is in the process of data request
                                            // from its parent. Only effective if device enables sleeping
        BYTE RxHasUserData          :1;     // indicate if the received frame needs processing from
                                            // the application layer
        BYTE AckRequired            :1;     // indicate if the transmitting packet require acknowledgement
                                            // from the peer device
        BYTE EnableNewConnection    :1;     // indicate if the current device allow new P2P connection 
                                            // established.
        BYTE TX_FAIL                :1;     // indicate if the current transmission fails
        
        BYTE SearchConnection       :1;     // indicate if the stack is currently in the process of 
                                            // looking for new connection
        BYTE Resync                 :1;     // indicate if the stack is currently in the process of 
                                            // resynchronizing connection with the peer device
    }bits;                                  // bit map of the P2P status
} P2P_STATUS;                               

#ifdef ENABLE_ACTIVE_SCAN
    /******************************************************************
     * Overview: The structure that store the active scan result
     *****************************************************************/
    typedef struct 
    {
        BYTE        Channel;        // The channel of the PAN that is operating on
        BYTE        RSSIValue;      // The signal strength of the PAN. If there are more than one
                                    // devices that belongs to the same PAN and respond to the active
                                    // scan, the one with the strongest signal will be recorded
        WORD_VAL    PANID;          // The PAN identifier of the PAN
    } ACTIVE_SCAN_RESULT;
#endif

/************************ EXTERNAL VARIABLES **********************/
extern BYTE TxBuffer[];
extern BYTE TxData;
extern WORD_VAL myPANID;
extern P2P_CONNECTION_ENTRY    P2PConnections[P2P_CONNECTION_SIZE];
extern volatile P2P_STATUS P2PStatus;
extern RECEIVED_FRAME rxFrame;

#ifdef ENABLE_ACTIVE_SCAN
    #define ACTIVE_SCAN_RESULT_SIZE 16
    extern ACTIVE_SCAN_RESULT ActiveScanResults[ACTIVE_SCAN_RESULT_SIZE];
#endif

#ifdef ENABLE_FREQUENCY_AGILITY
    extern BYTE AckFailureTimes;
    extern BYTE CCAFailureTimes;
    extern BYTE currentChannel;
#endif

/************************ MACROS **********************************/

/*****************************************************************
 * Write one byte into transmission buffer
 ****************************************************************/
#define WriteData(a) TxBuffer[TxData++] = a  

/*****************************************************************
 * Reset the transmission buffer before writing any data
 ****************************************************************/ 
#define FlushTx() TxData = 0; 

/*****************************************************************
 * Enable the MiWi(TM) P2P stack to accept new P2P connection 
 * request
 ****************************************************************/
#define EnableNewConnection() P2PStatus.bits.EnableNewConnection = 1;

/*****************************************************************
 * Disable the MiWi(TM) P2P stack to accept new P2P connection
 * request
 ****************************************************************/
#define DisableNewConnection() P2PStatus.bits.EnableNewConnection = 0;

/*****************************************************************
 * Enable the request of acknowledgment for all unicast message
 ****************************************************************/
#define EnableAcknowledgement() P2PStatus.bits.AckRequired = 1;

/*****************************************************************
 * Disable the request of acknowledgement for all unicast message
 ****************************************************************/
#define DisableAcknowledgement() P2PStatus.bits.AckRequired = 0;

/*****************************************************************
 * Notify the P2P stack that a data request procedure is going on
 ****************************************************************/
#define DataRequesting()    P2PStatus.bits.DataRequesting = 1;

/*****************************************************************
 * Check if the stack is in the data request process
 ****************************************************************/
#define isDataRequesting()  P2PStatus.bits.DataRequesting

/*****************************************************************
 * Notify the stack that the device is idle and ready to go to
 * sleep now
 ****************************************************************/
#define ReadyToSleep()      P2PStatus.bits.TimeToSleep = 1;

/*****************************************************************
 * Check if the stack is idle and ready to go to sleep
 ****************************************************************/
#define isReadyToSleep()    P2PStatus.bits.TimeToSleep

/*****************************************************************
 * Clear the flag to notify the stack that the device is idle
 * and ready to go to sleep
 ****************************************************************/
#define ClearReadyToSleep() P2PStatus.bits.TimeToSleep = 0;

/************************ FUNCTION PROTOTYPES **********************/
void P2PInit(void);
void P2PTasks(void);
void DiscardPacket(void);
void SetChannel(INPUT BYTE channel);  
void initMRF24J40(void);

void PHYSetLongRAMAddr(INPUT WORD address, INPUT BYTE value);
void PHYSetShortRAMAddr(INPUT BYTE address, INPUT BYTE value);
BYTE PHYGetShortRAMAddr(INPUT BYTE address);
BYTE PHYGetLongRAMAddr(INPUT WORD address);

BOOL BroadcastPacket( INPUT WORD_VAL DestinationPANID, INPUT BOOL isCommand, INPUT BOOL SecurityEnabled );
BOOL UnicastConnection( INPUT BYTE ConnectionIndex, INPUT BOOL isCommand, INPUT BOOL SecurityEnabled);
BOOL UnicastLongAddress( INPUT WORD_VAL DestinationPANID, INPUT BYTE *DestinationAddress, INPUT BOOL isCommand, INPUT BOOL SecurityEnabled);
void DumpConnection(INPUT BYTE index);
BOOL ReceivedPacket(void);

#ifdef ENABLE_ACTIVE_SCAN
    BYTE ActiveScan(INPUT BYTE ScanDuration, INPUT DWORD ChannelMap);
#endif

#ifdef ENABLE_ED_SCAN
    BYTE CreateNewConnection(INPUT BYTE RetryInterval, INPUT DWORD ChannelMap);
    BYTE OptimalChannel(INPUT BYTE ScanDuration, INPUT DWORD ChannelMap, OUTPUT BYTE *RSSIValue);
#else
    BYTE CreateNewConnection(INPUT BYTE RetryInterval);
#endif

#ifdef ENABLE_FREQUENCY_AGILITY
    BOOL InitChannelHopping( INPUT DWORD ChannelMap);
    BOOL ResyncConnection(INPUT BYTE *DestinationAddress, INPUT DWORD ChannelMap);
#endif

#ifdef ENABLE_SLEEP
    void MRF24J40Sleep(void);
    void MRF24J40Wake(void);
    BOOL CheckForData(void);
#endif    
#endif

    
    
