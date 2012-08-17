/*********************************************************************
 *
 *                  DHCP Module for Modtronix TCP/IP Stack
 *
 *********************************************************************
 * FileName:        DHCP.c
 * Dependencies:    stacktsk.h
 *                  UDP.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.35PL3 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on v2.20.04.01
 * of the Microchip TCP/IP stack. The original code base is owned by Microchip
 * Technology Incorporated (the Company), and is protected under applicable
 * copyright laws. The modifications are owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller based products manufactured
 * by Modtronix Engineering. The code may be modified and can be used free of charge
 * for non commercial and commercial applications. All rights are reserved. Any use
 * in violation of the foregoing restrictions may subject the user to criminal
 * sanctions under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * HiTech PICC18 Compiler Options excluding device selection:
 *                  -FAKELOCAL -G -Zg -E -C
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     3/21/01  Original        (Rev 1.0)
 * Nilesh Rajbharti     7/10/02  Explicitly initialized tempIPAddress
 *                                               (Rev 2.11)
 * Nilesh Rajbharti     5/16/03 Increased DHCP_TIMEOUT to 2 seconds.
 * Nilesh Rajbharti     5/16/03 Fixed SM_DHCP_BROADCAST logic
 *                              where UDPPut was called before setting
 *                              active socket.
 * Robert Sloan         5/29/03 Improved DHCP State machine to handle
 *                              NAK and renew existing IP address.
 * Nilesh Rajbharti     8/15/03 Modified _DHCPRecieve() to check for
 *                              chaddr field before accpting the packet.
 *                              Fixed DHCPTask() where it would not
 *                              reply to first OFFER.
 * 2005-12-21, David Hosken (DH):
 *    - Increased DHCP timeout to 4 seconds
 *    - Modified ticks to use 16 bit variable
 * 2006-09-08, David Hosken (DH): Implemented changes from Microchip TCP/IP stack 3.75
 ********************************************************************/
#define THIS_IS_DHCP

#include "projdefs.h"
#include "net\checkcfg.h"
#include "net\dhcp.h"
#include "net\udp.h"
#include "net\tick.h"

#include "debug.h"

#if defined(STACK_USE_DHCP)

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xE1, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xE1, msgCode, msgStr)

/////////////////////////////////////////////////
//Do some checks
#if defined(STACK_USE_SLIP)
    #error DHCP module is not available when SLIP is used.
#endif

#define BOOT_REQUEST                    (1u)
#define BOOT_REPLY                      (2u)
#define HW_TYPE                         (1u)
#define LEN_OF_HW_TYPE                  (6u)

#define DHCP_MESSAGE_TYPE               (53u)
#define DHCP_MESSAGE_TYPE_LEN           (1u)

#define DHCP_UNKNOWN_MESSAGE            (0u)

#define DHCP_DISCOVER_MESSAGE           (1u)
#define DHCP_OFFER_MESSAGE              (2u)
#define DHCP_REQUEST_MESSAGE            (3u)
#define DHCP_DECLINE_MESSAGE            (4u)
#define DHCP_ACK_MESSAGE                (5u)
#define DHCP_NAK_MESSAGE                (6u)
#define DHCP_RELEASE_MESSAGE            (7u)

#define DHCP_SERVER_IDENTIFIER          (54u)
#define DHCP_SERVER_IDENTIFIER_LEN      (4u)

#define DHCP_PARAM_REQUEST_LIST         (55u)
#define DHCP_PARAM_REQUEST_LIST_LEN     (4u)
#define DHCP_PARAM_REQUEST_IP_ADDRESS       (50u)
#define DHCP_PARAM_REQUEST_IP_ADDRESS_LEN   (4u)
#define DHCP_SUBNET_MASK                (1u)
#define DHCP_ROUTER                     (3u)
#define DHCP_DNS                        (6u)
#define DHCP_HOST_NAME                    (12u)
#define DHCP_IP_LEASE_TIME              (51u)
#define DHCP_END_OPTION                 (255u)

#define HALF_HOUR                       (WORD)((WORD)60 * (WORD)30)

SM_DHCP  smDHCPState = SM_DHCP_INIT_FIRST_TIME;
static UDP_SOCKET DHCPSocket = INVALID_UDP_SOCKET;


DHCP_STATE DHCPState = { 0x00 };

static DWORD_VAL DHCPServerID;
static DWORD_VAL DHCPLeaseTime;

static IP_ADDR tempIPAddress;
static IP_ADDR tempGateway;
static IP_ADDR tempMask;
#if defined(STACK_USE_DNS)
static IP_ADDR tempDNS;
#endif
//static BYTE tempHostName[16];

static union
{
    struct
    {
        char IPAddress:1;
        char Gateway:1;
        char Mask:1;
        char DNS:1;
        char HostName:1;
    } bits;
    BYTE Val;
} ValidValues;

static BYTE _DHCPReceive(void);
static void _DHCPSend(BYTE messageType);

BYTE DHCPBindCount = 0;


void DHCPReset(void)
{
    // Do not reset DHCP if it was previously disabled.
    if ( smDHCPState == SM_DHCP_DISABLED )
        return;

    if ( DHCPSocket != INVALID_UDP_SOCKET )
        UDPClose(DHCPSocket);
    DHCPSocket = INVALID_UDP_SOCKET;

    smDHCPState = SM_DHCP_INIT_FIRST_TIME;
    DHCPBindCount = 0;

    DHCPState.bits.bIsBound = FALSE;
    DHCPState.bits.bStaticIP = FALSE;
}

/**
 * Fetches pending UDP packet from MAC receive buffer and dispatches it appropriate UDP socket.
 * If not UDP socket is matched, UDP packet is silently discarded.
 * Note:    Caller must make sure that MAC receive buffer
 *          access pointer is set to begining of UDP packet.
 *          Required steps before calling this function is:
 *          
 *          If ( MACIsRxReady() )
 *          {
 *              MACGetHeader()
 *              If MACFrameType == IP
 *                  IPGetHeader()
 *                  if ( IPFrameType == IP_PROT_UDP )
 *                      Call DHCPTask()
 *              ...
 *          }
 *
 * @@preCondition    DHCPInit() is already called AND IPGetHeader() is called with
 *                  IPFrameType == IP_PROT_UDP
 */
void DHCPTask(void)
{
    NODE_INFO DHCPServerNode;
    static TICK16 lastTryTick;
    BYTE DHCPRecvReturnValue;
    static BYTE broadcastCount;

    switch(smDHCPState)
    {
    case SM_DHCP_INIT_FIRST_TIME:
        tempIPAddress.Val = 0x0;
//        smDHCPState = SM_DHCP_INIT;            // State automatically changes
        /* No break */

    case SM_DHCP_INIT:
        broadcastCount = 3;
        DHCPServerNode.MACAddr.v[0] = 0xff;
        DHCPServerNode.MACAddr.v[1] = 0xff;
        DHCPServerNode.MACAddr.v[2] = 0xff;
        DHCPServerNode.MACAddr.v[3] = 0xff;
        DHCPServerNode.MACAddr.v[4] = 0xff;
        DHCPServerNode.MACAddr.v[5] = 0xff;
        DHCPServerNode.IPAddr.Val = 0xffffffff;
        DHCPSocket = UDPOpen(DHCP_CLIENT_PORT,
                             &DHCPServerNode,
                             DHCP_SERVER_PORT);

        if( DHCPSocket == INVALID_UDP_SOCKET ) {
            #if (DEBUG_DHCP >= LOG_ERROR)
            debugPutMsg(7); //@mxd:7:Could not open UDP socket
            #endif
            break;
        }
                             
        lastTryTick = TickGet16bit();
        smDHCPState = SM_DHCP_RESET_WAIT;
        /* No break */

    case SM_DHCP_RESET_WAIT:
        //More then 200ms has passed
        if ( TickGetDiff16bit(lastTryTick) >= ((TICK16)TICKS_PER_SECOND / (TICK16)5) )
            smDHCPState = SM_DHCP_BROADCAST;
        break;

    case SM_DHCP_BROADCAST:
        // Assume default IP Lease time of 60 seconds.
        // This should be minimum possible to make sure that if
        // server did not specify lease time, we try again after this minimum time.
        DHCPLeaseTime.Val = 60;

        //After a certian number of tries, give up, and assign a static IP address
        if (broadcastCount-- == 0) {
            //Once DCHP is successful, release the UDP socket
            //This will ensure that UDP layer discards any further DHCP related packets.
            UDPClose(DHCPSocket);
            DHCPSocket = INVALID_UDP_SOCKET;

            smDHCPState = SM_DHCP_BOUND;

            DHCPState.bits.bIsBound = TRUE;
            DHCPBindCount++;
            
            //Disable DHCP. No more attempts will be made to obtain a IP from the DHCP server, except
            //if the cable is unplugged.
            DHCPState.bits.bStaticIP = TRUE;
            
            MY_IP_BYTE1 = MY_STATIC_IP_BYTE1;
            MY_IP_BYTE2 = MY_STATIC_IP_BYTE2;
            MY_IP_BYTE3 = MY_STATIC_IP_BYTE3;
            MY_IP_BYTE4 = MY_STATIC_IP_BYTE4;

            #if (DEBUG_DHCP >= LOG_INFO)
            debugPutMsg(10); //@mxd:10:Maxumum Broadcast tries, assigned static IP address
            #endif
            
            return;
        }

        // If we have already obtained some IP address, renew it.
        if(DHCPState.bits.bIsBound)
        {
            smDHCPState = SM_DHCP_REQUEST;
        }
        else if ( UDPIsPutReady(DHCPSocket) )
        {
            // To minimize code requirement, user must make sure that
            // above call will be successful by making at least one
            // UDP socket available.
            // Usually this will be the case, given that DHCP will be
            // the first one to use UDP socket.
            // Also, we will not check for transmitter readiness,
            // we assume it to be ready.
            _DHCPSend(DHCP_DISCOVER_MESSAGE);
            ValidValues.Val = 0x00;

            // DEBUG
             #if (DEBUG_DHCP >= LOG_INFO)
            debugPutMsg(1); //@mxd:1:Broadcast
            #endif

            lastTryTick = TickGet16bit();
            smDHCPState = SM_DHCP_DISCOVER;
        }
        else {
            // DEBUG
             #if (DEBUG_DHCP >= LOG_INFO)
            debugPutMsg(8); //@mxd:8:Can not Broadcase, UDP Not ready!
            #endif
        }

        break;


    case SM_DHCP_DISCOVER:
        if ( TickGetDiff16bit(lastTryTick) >= DHCP_TIMEOUT)    //Timeout has expired
        {
            smDHCPState = SM_DHCP_BROADCAST;

            // DEBUG
            #if (DEBUG_DHCP >= LOG_INFO)
            debugPutMsg(9); //@mxd:9:Discover time out
            #endif

            break;
        }

        if ( UDPIsGetReady(DHCPSocket) )
        {
            // DEBUG
            #if (DEBUG_DHCP >= LOG_INFO)
            debugPutMsg(2); //@mxd:2:Discover
            #endif

            if ( _DHCPReceive() == DHCP_OFFER_MESSAGE )
            {
                // DEBUG
                #if (DEBUG_DHCP >= LOG_INFO)
                debugPutMsg(3); //@mxd:3:Offer Message
                #endif

                smDHCPState = SM_DHCP_REQUEST;
            }
            else
                break;
        }
        else
            break;



    case SM_DHCP_REQUEST:
        if ( UDPIsPutReady(DHCPSocket) )
        {
            _DHCPSend(DHCP_REQUEST_MESSAGE);

            lastTryTick = TickGet16bit();
            smDHCPState = SM_DHCP_BIND;

            // DEBUG
            #if (DEBUG_DHCP >= LOG_INFO)
            debugPutMsg(12); //@mxd:12:Sending Request
            #endif
        }
        break;

    case SM_DHCP_BIND:
        if ( UDPIsGetReady(DHCPSocket) )
        {
            DHCPRecvReturnValue = _DHCPReceive();
            if ( DHCPRecvReturnValue == DHCP_NAK_MESSAGE )
            {
                   // (RSS) NAK recieved.  DHCP server didn't like our DHCP Request (format wrong/IP address allocated to someone else/outside IP pool)
                #if (DEBUG_DHCP >= LOG_WARN)
                debugPutMsg(4);     //@mxd:4:NAK recieved (RSS). DHCP server didn't like our DHCP Request format
                #endif
                DHCPReset();        // Start all over again
                return;
            }
            else if ( DHCPRecvReturnValue == DHCP_ACK_MESSAGE )
            {
                // DEBUG
                #if (DEBUG_DHCP >= LOG_INFO)
                debugPutMsg(5);     //@mxd:5:ACK Received
                #endif

                // Once DCHP is successful, release the UDP socket
                // This will ensure that UDP layer discards any further DHCP related packets.
                UDPClose(DHCPSocket);
                DHCPSocket = INVALID_UDP_SOCKET;

                lastTryTick = TickGet16bit();
                smDHCPState = SM_DHCP_BOUND;

                if(ValidValues.bits.IPAddress) {
                    MY_IP_BYTE1     = tempIPAddress.v[0];
                    MY_IP_BYTE2     = tempIPAddress.v[1];
                    MY_IP_BYTE3     = tempIPAddress.v[2];
                    MY_IP_BYTE4     = tempIPAddress.v[3];
                }

                if(ValidValues.bits.Mask) {
                    MY_MASK_BYTE1   = tempMask.v[0];
                    MY_MASK_BYTE2   = tempMask.v[1];
                    MY_MASK_BYTE3   = tempMask.v[2];
                    MY_MASK_BYTE4   = tempMask.v[3];
                }

                if(ValidValues.bits.Gateway) {
                    MY_GATE_BYTE1   = tempGateway.v[0];
                    MY_GATE_BYTE2   = tempGateway.v[1];
                    MY_GATE_BYTE3   = tempGateway.v[2];
                    MY_GATE_BYTE4   = tempGateway.v[3];
                }

#if defined(STACK_USE_DNS)
                if(ValidValues.bits.DNS) {
                    MY_DNS_BYTE1_SET(tempDNS.v[0]);
                    MY_DNS_BYTE2_SET(tempDNS.v[1]);
                    MY_DNS_BYTE3_SET(tempDNS.v[2]);
                    MY_DNS_BYTE4_SET(tempDNS.v[3]);
                }
#endif
//                if(ValidValues.bits.HostName)
//                    memcpy(AppConfig.NetBIOSName, (void*)tempHostName, sizeof(AppConfig.NetBIOSName));

                DHCPState.bits.bIsBound = TRUE;

                DHCPBindCount++;

                return;
            }
        }
        else if ( TickGetDiff16bit(lastTryTick) >= DHCP_TIMEOUT )   //Timeout has expired
        {
            #if (DEBUG_DHCP >= LOG_WARN)
            debugPutMsg(6);     //@mxd:6:Timeout
            #endif
            smDHCPState = SM_DHCP_BROADCAST;
        }
        break;

    case SM_DHCP_BOUND:
        //If a static IP was assigned, it never times out. Only when the cable is unplugged, will
        //the DHCP server be reset, and start looking for an IP again.
        if (DHCPState.bits.bStaticIP) return;
        
        // Keep track of how long we use this IP configuration.
        // When lease period expires, renew the configuration.
        while ( TickGetDiff16bit(lastTryTick) >= ((TICK16)TICKS_PER_SECOND) )
        {
            DHCPLeaseTime.Val--;        //Decrement lease time
            if ( DHCPLeaseTime.Val == 0 ) {
                smDHCPState = SM_DHCP_INIT;

                // DEBUG
                #if (DEBUG_DHCP >= LOG_INFO)
                debugPutMsg(11);     //@mxd:11:Lease time expired
                #endif
            }

            //Add 1 seconds to lastTryTick - will cause this statement to execute in 1 second again
            lastTryTick += ((TICK16)TICKS_PER_SECOND);  //Add 1 second to lastTryTick
        }
    }

}

/**
 *  DHCP PACKET FORMAT AS PER RFC 1541
 *
 *  0                   1                   2                   3
 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |     op (1)    |   htype (1)   |   hlen (1)    |   hops (1)    |
 *   +---------------+---------------+---------------+---------------+
 *   |                            xid (4)                            |
 *   +-------------------------------+-------------------------------+
 *   |           secs (2)            |           flags (2)           |
 *   +-------------------------------+-------------------------------+
 *   |                          ciaddr  (4)                          |
 *   +---------------------------------------------------------------+
 *   |                          yiaddr  (4)                          |
 *   +---------------------------------------------------------------+
 *   |                          siaddr  (4)                          |
 *   +---------------------------------------------------------------+
 *   |                          giaddr  (4)                          |
 *   +---------------------------------------------------------------+
 *   |                                                               |
 *   |                          chaddr  (16)                         |
 *   |                                                               |
 *   |                                                               |
 *   +---------------------------------------------------------------+
 *   |                                                               |
 *   |                          sname   (64)                         |
 *   +---------------------------------------------------------------+
 *   |                                                               |
 *   |                          file    (128)                        |
 *   +---------------------------------------------------------------+
 *   |                                                               |
 *   |                          options (312)                        |
 *   +---------------------------------------------------------------+
 *
 */
static BYTE _DHCPReceive(void)
{
    BYTE v;
    BYTE i, j;
    BYTE type;
    BOOL lbDone;
    DWORD_VAL tempServerID;
    MAC_ADDR tempMacAddr;


    // Assume unknown message until proven otherwise.
    type = DHCP_UNKNOWN_MESSAGE;

    UDPGet(&v);                             // op

    // Make sure this is BOOT_REPLY.
    if ( v == BOOT_REPLY )
    {
        // Discard htype, hlen, hops, xid, secs, flags, ciaddr.
        for ( i = 0; i < 15u; i++ )
            UDPGet(&v);

        // Check to see if this is the first offer
        if(DHCPState.bits.bOfferReceived)
        {
            // Discard offered IP address, we already have an offer
            for ( i = 0; i < 4u; i++ )
                UDPGet(&v);
        }
        else
        {
            // Save offered IP address until we know for sure that we have it.
            UDPGet(&tempIPAddress.v[0]);
            UDPGet(&tempIPAddress.v[1]);
            UDPGet(&tempIPAddress.v[2]);
            UDPGet(&tempIPAddress.v[3]);
            ValidValues.bits.IPAddress = 1;
        }

        // Ignore siaddr, giaddr
        for ( i = 0; i < 8u; i++ )
            UDPGet(&v);

        // Check to see if chaddr (Client Hardware Address) belongs to us.
        tempMacAddr.v[0] = MY_MAC_BYTE1;
        tempMacAddr.v[1] = MY_MAC_BYTE2;
        tempMacAddr.v[2] = MY_MAC_BYTE3;
        tempMacAddr.v[3] = MY_MAC_BYTE4;
        tempMacAddr.v[4] = MY_MAC_BYTE5;
        tempMacAddr.v[5] = MY_MAC_BYTE6;
        for ( i = 0; i < 6u; i++ )
        {
            UDPGet(&v);
            if ( v != tempMacAddr.v[i])
                goto UDPInvalid;
        }


        // Ignore part of chaddr, sname, file, magic cookie.
        for ( i = 0; i < 206u; i++ )
            UDPGet(&v);

        lbDone = FALSE;
        do
        {
            // Get the Option number
            // Break out eventually in case if this is a malformed 
            // DHCP message, ie: missing DHCP_END_OPTION marker
            if(!UDPGet(&v))
            {
                lbDone = TRUE;
                break;
            }

            switch(v)
            {
            case DHCP_MESSAGE_TYPE:
                UDPGet(&v);                         // Skip len
                // Len must be 1.
                if ( v == 1u )
                {
                    UDPGet(&type);                  // Get type

                    // Throw away the packet if we know we don't need it (ie: another offer when we already have one)
                    if(DHCPState.bits.bOfferReceived && (type == DHCP_OFFER_MESSAGE))
                    {
                        goto UDPInvalid;
                    }
                }
                else
                    goto UDPInvalid;
                break;

            case DHCP_SUBNET_MASK:
                UDPGet(&v);                     // Skip len
                // Len must be 4.
                if ( v == 4u )
                {
                    // Check to see if this is the first offer
                    if(DHCPState.bits.bOfferReceived)
                    {
                        // Discard offered IP mask, we already have an offer
                        for ( i = 0; i < 4u; i++ )
                            UDPGet(&v);
                    }
                    else
                    {
                        UDPGet(&tempMask.v[0]);
                        UDPGet(&tempMask.v[1]);
                        UDPGet(&tempMask.v[2]);
                        UDPGet(&tempMask.v[3]);
                        ValidValues.bits.Mask = 1;
                    }
                }
                else
                    goto UDPInvalid;
                break;

            case DHCP_ROUTER:
                UDPGet(&j);
                // Len must be >= 4.
                if ( j >= 4u )
                {
                    // Check to see if this is the first offer
                    if(DHCPState.bits.bOfferReceived)
                    {
                        // Discard offered Gateway address, we already have an offer
                        for ( i = 0; i < 4u; i++ )
                            UDPGet(&v);
                    }
                    else
                    {
                        UDPGet(&tempGateway.v[0]);
                        UDPGet(&tempGateway.v[1]);
                        UDPGet(&tempGateway.v[2]);
                        UDPGet(&tempGateway.v[3]);
                        ValidValues.bits.Gateway = 1;
                    }
                }
                else
                    goto UDPInvalid;

                // Discard any other router addresses.
                j -= 4;
                while(j--)
                    UDPGet(&v);
                break;

#if defined(STACK_USE_DNS)
            case DHCP_DNS:
                UDPGet(&j);
                // Len must be >= 4.
                if ( j >= 4u )
                {
                    // Check to see if this is the first offer
                    if(DHCPState.bits.bOfferReceived)
                    {
                        // Discard offered DNS server address, we already have an offer
                        for ( i = 0; i < 4u; i++ )
                            UDPGet(&v);
                    }
                    else
                    {
                        UDPGet(&tempDNS.v[0]);
                        UDPGet(&tempDNS.v[1]);
                        UDPGet(&tempDNS.v[2]);
                        UDPGet(&tempDNS.v[3]);
                        ValidValues.bits.DNS = 1;
                    }
                }
                else
                    goto UDPInvalid;

                // Discard any other DNS server addresses
                j -= 4;
                while(j--)
                    UDPGet(&v);
                break;
#endif

//            case DHCP_HOST_NAME:
//                UDPGet(&j);
//                // Len must be >= 4.
//                if(j < 1u)
//                    goto UDPInvalid;
//
//                // Check to see if this is the first offer
//                if(DHCPState.bits.bOfferReceived)
//                {
//                    // Discard offered host name, we already have an offer
//                    while(j--)
//                        UDPGet(&v);
//                }
//                else
//                {
//                    for(i = 0; j, i < sizeof(tempHostName); i++, j--)
//                    {
//                        UDPGet(&tempHostName[i]);
//                    }
//                    while(j--)
//                    {
//                        UDPGet(&v);
//                    }
//                    ValidValues.bits.HostName = 1;
//                }
//
//                break;

            case DHCP_SERVER_IDENTIFIER:
                UDPGet(&v);                         // Get len
                // Len must be 4.
                if ( v == 4u )
                {
                    UDPGet(&tempServerID.v[3]);   // Get the id
                    UDPGet(&tempServerID.v[2]);
                    UDPGet(&tempServerID.v[1]);
                    UDPGet(&tempServerID.v[0]);
                }
                else
                    goto UDPInvalid;
                break;

            case DHCP_END_OPTION:
                lbDone = TRUE;
                break;

            case DHCP_IP_LEASE_TIME:
                UDPGet(&v);                         // Get len
                // Len must be 4.
                if ( v == 4u )
                {
                    // Check to see if this is the first offer
                    if(DHCPState.bits.bOfferReceived)
                    {
                        // Discard offered lease time, we already have an offer
                        for ( i = 0; i < 4u; i++ )
                            UDPGet(&v);
                    }
                    else
                    {
                        UDPGet(&DHCPLeaseTime.v[3]);
                        UDPGet(&DHCPLeaseTime.v[2]);
                        UDPGet(&DHCPLeaseTime.v[1]);
                        UDPGet(&DHCPLeaseTime.v[0]);

                        // Due to possible timing delays, consider actual lease
                        // time less by half hour.
                        if ( DHCPLeaseTime.Val > HALF_HOUR )
                            DHCPLeaseTime.Val = DHCPLeaseTime.Val - HALF_HOUR;
                    }
                }
                else
                    goto UDPInvalid;
                break;

            default:
                // Ignore all unsupport tags.
                UDPGet(&j);                     // Get option len
                while( j-- )                    // Ignore option values
                    UDPGet(&v);
            }
            FAST_USER_PROCESS();
        } while( !lbDone );
    }

    // If this is an OFFER message, remember current server id.
    if ( type == DHCP_OFFER_MESSAGE )
    {
        DHCPServerID.Val = tempServerID.Val;
        DHCPState.bits.bOfferReceived = TRUE;
    }
    else
    {
        // For other types of messages, make sure that received
        // server id matches with our previous one.
        if ( DHCPServerID.Val != tempServerID.Val )
            type = DHCP_UNKNOWN_MESSAGE;
    }

    UDPDiscard();                             // We are done with this packet
    return type;

UDPInvalid:
    UDPDiscard();
    return DHCP_UNKNOWN_MESSAGE;

}





static void _DHCPSend(BYTE messageType)
{
    BYTE i;


    UDPPut(BOOT_REQUEST);                       // op
    UDPPut(HW_TYPE);                            // htype
    UDPPut(LEN_OF_HW_TYPE);                     // hlen
    UDPPut(0);                                  // hops
    UDPPut(0x12);                               // xid[0]
    UDPPut(0x23);                               // xid[1]
    UDPPut(0x34);                               // xid[2]
    UDPPut(0x56);                               // xid[3]
    UDPPut(0);                                  // secs[0]
    UDPPut(0);                                  // secs[1]
    UDPPut(0x80);                               // flags[0] with BF set
    UDPPut(0);                                  // flags[1]

    // If this is DHCP REQUEST message, use previously allocated IP address.
#if 0
    if ( messageType == DHCP_REQUEST_MESSAGE )
    {
        UDPPut(tempIPAddress.v[0]);
        UDPPut(tempIPAddress.v[1]);
        UDPPut(tempIPAddress.v[2]);
        UDPPut(tempIPAddress.v[3]);
    }
    else
#endif
    {
        UDPPut(0x00);
        UDPPut(0x00);
        UDPPut(0x00);
        UDPPut(0x00);
    }

    // Set yiaddr, siaddr, giaddr as zeros,
    for ( i = 0; i < 12u; i++ )
        UDPPut(0x00);

    // Load chaddr - Client hardware address.
    UDPPut(MY_MAC_BYTE1);
    UDPPut(MY_MAC_BYTE2);
    UDPPut(MY_MAC_BYTE3);
    UDPPut(MY_MAC_BYTE4);
    UDPPut(MY_MAC_BYTE5);
    UDPPut(MY_MAC_BYTE6);

    // Set chaddr[6..15], sname and file as zeros.
    for ( i = 0; i < 202u; i++ )
        UDPPut(0);

    // Load magic cookie as per RFC 1533.
    UDPPut(99);
    UDPPut(130);
    UDPPut(83);
    UDPPut(99);

    // Load message type.
    UDPPut(DHCP_MESSAGE_TYPE);
    UDPPut(DHCP_MESSAGE_TYPE_LEN);
    UDPPut(messageType);

    if(messageType == DHCP_DISCOVER_MESSAGE)
    {
        // Reset offered flag so we know to act upon the next valid offer
        DHCPState.bits.bOfferReceived = FALSE;
    }


    if ( messageType != DHCP_DISCOVER_MESSAGE && tempIPAddress.Val != 0x0000u )
    {
         // DHCP REQUEST message may include server identifier,
         // to identify the server we are talking to.
         // DHCP ACK may include it too.  To simplify logic,
         // we will include server identifier in DHCP ACK message too.
         // _DHCPReceive() would populate "serverID" when it
         // receives DHCP OFFER message. We will simply use that
         // when we are replying to server.
         // If this is a renwal request, do not include server id.
         UDPPut(DHCP_SERVER_IDENTIFIER);
         UDPPut(DHCP_SERVER_IDENTIFIER_LEN);
         UDPPut(DHCPServerID.v[3]);
         UDPPut(DHCPServerID.v[2]);
         UDPPut(DHCPServerID.v[1]);
         UDPPut(DHCPServerID.v[0]);
     }

    // Load our interested parameters
    // This is hardcoded list.  If any new parameters are desired,
    // new lines must be added here.
    UDPPut(DHCP_PARAM_REQUEST_LIST);
    UDPPut(DHCP_PARAM_REQUEST_LIST_LEN);
    UDPPut(DHCP_SUBNET_MASK);
    UDPPut(DHCP_ROUTER);
    UDPPut(DHCP_DNS);
    UDPPut(DHCP_HOST_NAME);

     // Add requested IP address to DHCP Request Message
    if ( messageType == DHCP_REQUEST_MESSAGE )
    {
        UDPPut(DHCP_PARAM_REQUEST_IP_ADDRESS);
        UDPPut(DHCP_PARAM_REQUEST_IP_ADDRESS_LEN);

        UDPPut(tempIPAddress.v[0]);
        UDPPut(tempIPAddress.v[1]);
        UDPPut(tempIPAddress.v[2]);
        UDPPut(tempIPAddress.v[3]);
    }

    // Add any new paramter request here.

    // End of Options.
    UDPPut(DHCP_END_OPTION);

    UDPFlush();
}

#endif    //#if defined(STACK_USE_DHCP)
