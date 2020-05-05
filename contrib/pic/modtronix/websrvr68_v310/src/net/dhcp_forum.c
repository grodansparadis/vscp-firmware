
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
 * The software supplied herewith by Modtronix Engineering is based on
 * v2.20.04.01 of the Microchip TCP/IP stack. The original code base
 * is owned by Microchip Technology Incorporated (the Company), and is
 * protected under applicable copyright laws. The modifications are
 * owned by Modtronix Engineering, and is protected under applicable
 * copyright laws. The software supplied herewith is intended and
 * supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller based products
 * manufactured by Modtronix Engineering. The code may be modified and
 * can be used free of charge for non commercial and commercial
 * applications. All rights are reserved. Any use in violation of the
 * foregoing restrictions may subject the user to criminal sanctions
 * under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
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
 *
 * 2008-05-08, Andrew Eckhardt: Implemented changes from Microchip
 *              TCP/IP Stack Version v4.18 dated 28 November 2007
 ********************************************************************/
#define __DHCP_C
#define THIS_IS_DHCP

#include "projdefs.h"
#include "net\checkcfg.h"
#include "net\dhcp.h"
#include "net\udp.h"
#include "net\tick.h"
#include "debug.h"
#include <string.h>   // memset

#if defined(STACK_USE_DHCP)

//#define DHCP_TIMEOUT                    (TICK)(2*TICK_SECOND)

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xE1, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xE1, msgCode, msgStr)

/////////////////////////////////////////////////
//Do some checks
#if defined(STACK_USE_SLIP)
#error DHCP module is not available when SLIP is used.
#endif

SM_DHCP smDHCPState = SM_DHCP_GET_SOCKET;
DHCP_CLIENT_FLAGS DHCPFlags = {{0x00}};
BYTE DHCPBindCount = 0;

static UDP_SOCKET DHCPSocket = INVALID_UDP_SOCKET;

static DWORD_VAL DHCPServerID;
static DWORD_VAL DHCPLeaseTime;

static IP_ADDR tempIPAddress;
static IP_ADDR tempGateway;
static IP_ADDR tempMask;
#if defined(STACK_USE_DNS)
static IP_ADDR tempDNS;
static IP_ADDR tempDNS2;
#endif

//ToDo: Fix UDP code to be compatible to new UCHIP stack
//WORD UDPTxCount; 

typedef signed long LONG; // g:/hvac/uchip_TCP/Microchip
			  // Solutions/Microchip/Include/GenericTypeDefs.h

static union
{
	struct
	{
		char IPAddress:1;
		char Gateway:1;
		char Mask:1;
		char DNS:1;
		char DNS2:1;
		char HostName:1;
	} bits;
	BYTE Val;
} ValidValues;

static BYTE _DHCPReceive(void);
static void _DHCPSend(BYTE messageType, BOOL bRenewing);


/*********************************************************************
 * Function:        void DHCPReset(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:   Resets the DHCP client, giving up any lease, knowledge of
 *             DHCP servers, etc.
 *
 * Note:            None
 ********************************************************************/
void DHCPReset(void)
{
   // Do nothing if DHCP is disabled
   if ( smDHCPState == SM_DHCP_DISABLED )
      return;

   if(DHCPSocket != INVALID_UDP_SOCKET)
      smDHCPState = SM_DHCP_SEND_DISCOVERY;
   else
      smDHCPState = SM_DHCP_GET_SOCKET;

   DHCPBindCount = 0;
   DHCPFlags.bits.bIsBound = FALSE;
}

/*********************************************************************
 * Function:        void DHCPDisable(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Puts DHCPTask into unhandled state "SM_DHCP_DISABLED"
 *                  and hence DHCP is effectively disabled.
 *
 * Note: If the board was configured by DHCP before DHCPDisable() was
 * called, then the configuration will continue to be used, but the
 * DHCP module will no longer perform any lease renewals or otherwise.
 * In other words, if the lease expires the board will still be using
 * the old IP address and the DHCP server may decide to give the IP
 * address to someone else.  The application should change the current
 * IP address and other configuration with static information provided
 * by the user following a call to DHCPDisable().
 *
 ********************************************************************/
void DHCPDisable(void)
{
   if(DHCPSocket != INVALID_UDP_SOCKET)
      {
	 UDPClose(DHCPSocket);
	 DHCPSocket = INVALID_UDP_SOCKET;
      }
	
   smDHCPState = SM_DHCP_DISABLED;

    //Clear DHCP flag
    appcfgPutc(APPCFG_NETFLAGS, appcfgGetc(APPCFG_NETFLAGS) & ~APPCFG_NETFLAGS_DHCP);
}


/*********************************************************************
 * Function:        void DHCPEnable(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Puts DHCPTask into the original initialization 
 *		state, if currently disabled.  Does nothing if 
 *		already enabled.
 *
 * Note:            None
 ********************************************************************/
void DHCPEnable(void)
{
   if(smDHCPState == SM_DHCP_DISABLED)
      {
     //Set DHCP flag
     appcfgPutc(APPCFG_NETFLAGS, appcfgGetc(APPCFG_NETFLAGS) | APPCFG_NETFLAGS_DHCP);

	 smDHCPState = SM_DHCP_GET_SOCKET;
	 DHCPBindCount = 0;
	 DHCPFlags.bits.bIsBound = FALSE;
      }
}

/*********************************************************************
 * Function:        void DHCPTask(void)
 *
 * PreCondition:    DHCPInit() is already called
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Gets and generates DHCP messages to obtain a 
 *					lease and keep it from expiring
 *
 * Note:            None
 ********************************************************************/
void DHCPTask(void)
{
    static TICK eventTime;

    switch(smDHCPState)
    {
    case SM_DHCP_GET_SOCKET:
        // Open a socket to send and receive broadcast messages on
        DHCPSocket = UDPOpen(DHCP_CLIENT_PORT, NULL, DHCP_SERVER_PORT);
        if(DHCPSocket == INVALID_UDP_SOCKET) {
#if (DEBUG_DHCP >= LOG_ERROR)
                debugPutMsg(7); //@mxd:7:Could not open UDP socket
#endif
            break;
        }

        smDHCPState = SM_DHCP_SEND_DISCOVERY;
        // No break
    case SM_DHCP_SEND_DISCOVERY:
        if(!UDPIsPutReady(DHCPSocket)) {
#if (DEBUG_DHCP >= LOG_INFO)
                debugPutMsg(8); //@mxd:8:Can not Broadcast, UDP Not ready!
#endif
            break;
        }

        // Ensure that we transmit to the broadcast IP and MAC addresses
        // The UDP Socket remembers who it was last talking to
        memset((void*)&UDPSocketInfo[DHCPSocket].remoteNode, 0xFF, sizeof(UDPSocketInfo[DHCPSocket].remoteNode));

        // Assume default IP Lease time of 60 seconds.  This should
        // be minimum possible to make sure that if server did not
        // specify lease time, we try again after this minimum time.
        DHCPLeaseTime.Val = 60;
        ValidValues.Val = 0x00;
        DHCPBindCount = 0;
        DHCPFlags.bits.bIsBound = FALSE;	
        DHCPFlags.bits.bOfferReceived = FALSE;

        // Send the DHCP Discover broadcast
        _DHCPSend(DHCP_DISCOVER_MESSAGE, FALSE);

#if (DEBUG_DHCP >= LOG_INFO)
            debugPutMsg(1); //@mxd:1:Broadcast
#endif

        // Start a timer and begin looking for a response
        eventTime = TickGet() + DHCP_TIMEOUT;
        smDHCPState = SM_DHCP_GET_OFFER;
        break;
    case SM_DHCP_GET_OFFER:
        // Check to see if a packet has arrived
        if(!UDPIsGetReady(DHCPSocket))
        {
            // Go back and transmit a new discovery if we didn't get an offer after 2 seconds
            if((LONG)(eventTime - TickGet()) <= (LONG)0) {
#if (DEBUG_DHCP >= LOG_INFO)
                    debugPutMsg(9); //@mxd:9:Discover time out
#endif

                smDHCPState = SM_DHCP_SEND_DISCOVERY;
                break;
            }
        }

        // Let the DHCP server module know that there is a DHCP
        // server on this network
        DHCPFlags.bits.bDHCPServerDetected = TRUE;

        // Check to see if we received an offer
        if(_DHCPReceive() != DHCP_OFFER_MESSAGE)
            break;

#if (DEBUG_DHCP >= LOG_INFO)
        debugPutMsg(3); //@mxd:3:Offer Message
#endif

    smDHCPState = SM_DHCP_SEND_REQUEST;
    // No break
    case SM_DHCP_SEND_REQUEST:
        if(!UDPIsPutReady(DHCPSocket))
            break;

    // Send the DHCP request message
    _DHCPSend(DHCP_REQUEST_MESSAGE, FALSE);

#if (DEBUG_DHCP >= LOG_INFO)
        debugPutMsg(12); //@mxd:12:Sending Request
#endif
    // Start a timer and begin looking for a response
    eventTime = TickGet() + DHCP_TIMEOUT;
    smDHCPState = SM_DHCP_GET_REQUEST_ACK;
    break;
    case SM_DHCP_GET_REQUEST_ACK:
        // Check to see if a packet has arrived
        if(!UDPIsGetReady(DHCPSocket))
        {
            // Go back and transmit a new discovery if we didn't
            // get an ACK after 2 seconds
            if((LONG)(eventTime - TickGet()) <= (LONG)0)
                smDHCPState = SM_DHCP_SEND_DISCOVERY;
            break;
        }

        // Check to see if we received an offer
        switch(_DHCPReceive())
        {
        case DHCP_ACK_MESSAGE:
            eventTime = TickGet() + TICK_SECOND;
            smDHCPState = SM_DHCP_BOUND;

            DHCPFlags.bits.bIsBound = TRUE;	
            DHCPBindCount++;
            if(ValidValues.bits.IPAddress)
                AppConfig.MyIPAddr = tempIPAddress;
            if(ValidValues.bits.Mask)
                AppConfig.MyMask = tempMask;
            if(ValidValues.bits.Gateway)
                AppConfig.MyGateway = tempGateway;
#if defined(STACK_USE_DNS)
            if(ValidValues.bits.DNS)
                AppConfig.PrimaryDNSServer = tempDNS;
            if(ValidValues.bits.DNS2)
                AppConfig.SecondaryDNSServer = tempDNS2;
#endif
#if (DEBUG_DHCP >= LOG_INFO)
                debugPutMsg(5);     //@mxd:5:ACK Received
#endif
            break;
        case DHCP_NAK_MESSAGE:
            smDHCPState = SM_DHCP_SEND_DISCOVERY;
#if (DEBUG_DHCP >= LOG_WARN)
                debugPutMsg(4);     //@mxd:4:NAK recieved (RSS). DHCP server
#endif
            break;
        }
        break;
    case SM_DHCP_BOUND:
        if((LONG)(eventTime - TickGet()) >= (LONG)0)
            break;

    // Check to see if our lease has nearly expired and we must
    // renew
    eventTime += TICK_SECOND;
    DHCPLeaseTime.Val--;
    if(DHCPLeaseTime.Val) {
#if (DEBUG_DHCP >= LOG_INFO)
        debugPutMsg(11);     //@mxd:11:Lease time expired
#endif
    break;
    }

    smDHCPState = SM_DHCP_SEND_RENEW;
    // No break
    case SM_DHCP_SEND_RENEW:
    case SM_DHCP_SEND_RENEW2:
    case SM_DHCP_SEND_RENEW3:
        if(!UDPIsPutReady(DHCPSocket))
            break;

    // Send the DHCP request message
    _DHCPSend(DHCP_REQUEST_MESSAGE, TRUE);
    DHCPFlags.bits.bOfferReceived = FALSE;

    // Start a timer and begin looking for a response
    eventTime = TickGet() + DHCP_TIMEOUT;
    smDHCPState++;
    break;
    case SM_DHCP_GET_RENEW_ACK:
    case SM_DHCP_GET_RENEW_ACK2:
    case SM_DHCP_GET_RENEW_ACK3:
        // Check to see if a packet has arrived
        if(!UDPIsGetReady(DHCPSocket))
        {
            // Go back and transmit a new discovery if we didn't
            // get an ACK after 2 seconds
            if((LONG)(eventTime - TickGet()) <= (LONG)0)
            {
                if(++smDHCPState > SM_DHCP_GET_RENEW_ACK3)
                    smDHCPState = SM_DHCP_SEND_DISCOVERY;
            }
            break;
        }

        // Check to see if we received an offer
        switch(_DHCPReceive())
        {
        case DHCP_ACK_MESSAGE:
            eventTime = TickGet() + TICK_SECOND;
            DHCPBindCount++;
            smDHCPState = SM_DHCP_BOUND;
#if (DEBUG_DHCP >= LOG_INFO)
                debugPutMsg(5);     //@mxd:5:ACK Received
#endif
            break;
        case DHCP_NAK_MESSAGE:
            smDHCPState = SM_DHCP_SEND_DISCOVERY;
#if (DEBUG_DHCP >= LOG_WARN)
                debugPutMsg(4);     //@mxd:4:NAK recieved (RSS). DHCP server
#endif
            break;
        }
        break;
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
	 if(DHCPFlags.bits.bOfferReceived)
	    {
	       // Discard offered IP address, we already have an offer
	       for ( i = 0; i < 4u; i++ )
		  UDPGet(&v);
	    }
	 else
	    {
	       // Save offered IP address until we know for sure that
	       // we have it.
	       UDPGetArray((BYTE*)&tempIPAddress, sizeof(tempIPAddress));
	       ValidValues.bits.IPAddress = 1;
	    }

	 // Ignore siaddr, giaddr
	 for ( i = 0; i < 8u; i++ )
            UDPGet(&v);

	 // Check to see if chaddr (Client Hardware Address) belongs
	 // to us.
	 for ( i = 0; i < 6u; i++ )
	    {
	       UDPGet(&v);
	       if ( v != AppConfig.MyMACAddr.v[i])
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

	       switch(v)      // handle the various option numbers
		  {
		  case DHCP_MESSAGE_TYPE:
		     UDPGet(&v);                         // Skip len
		     // Len must be 1.
		     if ( v == 1u )
			{
			   UDPGet(&type);                  // Get type

			   // Throw away the packet if we know we
			   // don't need it (ie: another offer when we
			   // already have one)
			   if(DHCPFlags.bits.bOfferReceived && (type == DHCP_OFFER_MESSAGE))
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
			   if(DHCPFlags.bits.bOfferReceived)
			      {
				 // Discard offered IP mask, we
				 // already have an offer
				 for ( i = 0; i < 4u; i++ )
				    UDPGet(&v);
			      }
			   else
			      {
				 UDPGetArray((BYTE*)&tempMask, sizeof(tempMask));
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
			   if(DHCPFlags.bits.bOfferReceived)
			      {
				 // Discard offered Gateway address,
				 // we already have an offer
				 for ( i = 0; i < 4u; i++ )
				    UDPGet(&v);
			      }
			   else
			      {
				 UDPGetArray((BYTE*)&tempGateway, sizeof(tempGateway));
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
		     UDPGet(&j);
		     // Len must be >= 4.
		     if(j < 4u)
			goto UDPInvalid;
		     
		     // Check to see if this is the first offer
		     if(!DHCPFlags.bits.bOfferReceived)
			{
			   UDPGetArray((BYTE*)&tempDNS, sizeof(tempDNS));
			   ValidValues.bits.DNS = 1;
			   j -= 4;
			}
		     
		     // Len must be >= 4 for a secondary DNS server address
		     if(j >= 4u)
			{
			   // Check to see if this is the first offer
			   if(!DHCPFlags.bits.bOfferReceived)
			      {
				 UDPGetArray((BYTE*)&tempDNS2, sizeof(tempDNS2));
				 ValidValues.bits.DNS2 = 1;
				 j -= 4;
			      }
			}

		     // Discard any other DNS server addresses
		     while(j--)
			UDPGet(&v);
		     break;
#endif


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
			   if(DHCPFlags.bits.bOfferReceived)
			      {
				 // Discard offered lease time, we
				 // already have an offer
				 for ( i = 0; i < 4u; i++ )
				    UDPGet(&v);
			      }
			   else
			      {
				 UDPGet(&DHCPLeaseTime.v[3]);
				 UDPGet(&DHCPLeaseTime.v[2]);
				 UDPGet(&DHCPLeaseTime.v[1]);
				 UDPGet(&DHCPLeaseTime.v[0]);

				 // In case our clock is not as
				 // accurate as the remote DHCP
				 // server's clock, let's treat the
				 // lease time as only 96.875% of the
				 // value given
				 DHCPLeaseTime.Val -= DHCPLeaseTime.Val>>5;
			     
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
	 DHCPFlags.bits.bOfferReceived = TRUE;
      }
   else
      {
	 // For other types of messages, make sure that received
	 // server id matches with our previous one.
	 if ( DHCPServerID.Val != tempServerID.Val )
            type = DHCP_UNKNOWN_MESSAGE;
      }
   
   UDPDiscard();    // We are done with this packet
   return type;

 UDPInvalid:
   UDPDiscard();
   return DHCP_UNKNOWN_MESSAGE;

}




static void _DHCPSend(BYTE messageType, BOOL bRenewing)
{
   BYTE i;
   IP_ADDR	MyIP;


   UDPPut(BOOT_REQUEST);                       // op
   UDPPut(BOOT_HW_TYPE);                       // htype
   UDPPut(BOOT_LEN_OF_HW_TYPE);                // hlen
   UDPPut(0);                                  // hops
   UDPPut(0x12);                               // xid[0]
   UDPPut(0x23);                               // xid[1]
   UDPPut(0x34);                               // xid[2]
   UDPPut(0x56);                               // xid[3]
   UDPPut(0);                                  // secs[0]
   UDPPut(0);                                  // secs[1]
   UDPPut(0x80);                               // flags[0] with BF set
   UDPPut(0);                                  // flags[1]

   // If this is DHCP REQUEST message, use previously allocated IP
   // address.

   if((messageType == DHCP_REQUEST_MESSAGE) && bRenewing)
      {
	 UDPPutArray((BYTE*)&tempIPAddress, sizeof(tempIPAddress));
      }
   else
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
   UDPPutArray((BYTE*)&AppConfig.MyMACAddr, sizeof(AppConfig.MyMACAddr));

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
	 // Reset offered flag so we know to act upon the next valid
	 // offer
	 DHCPFlags.bits.bOfferReceived = FALSE;
      }

   if((messageType == DHCP_REQUEST_MESSAGE) && !bRenewing)
      {
         // DHCP REQUEST message must include server identifier the
         // first time to identify the server we are talking to.
         // _DHCPReceive() would populate "serverID" when it receives
         // DHCP OFFER message. We will simply use that when we are
         // replying to server.  If this is a renwal request, we must
         // not include server id.
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
   if( ((messageType == DHCP_REQUEST_MESSAGE) && !bRenewing) || 
       ((messageType == DHCP_DISCOVER_MESSAGE) && tempIPAddress.Val))
      {
	 UDPPut(DHCP_PARAM_REQUEST_IP_ADDRESS);
	 UDPPut(DHCP_PARAM_REQUEST_IP_ADDRESS_LEN);
	 UDPPutArray((BYTE*)&tempIPAddress, DHCP_PARAM_REQUEST_IP_ADDRESS_LEN);
      }

   // Add any new parameter request here.

   // End of Options.
   UDPPut(DHCP_END_OPTION);

   // Add zero padding to ensure compatibility with old BOOTP
   // relays that discard small packets (<300 UDP octets)
   /* ToDo:  
      while (UDPSocketInfo[DHCPSocket].TxCount < 300)  // while(UDPTxCount < 300)
      UDPPut(0); 
   */

   // Make sure we advertise a 0.0.0.0 IP address so all DHCP
   // servers will respond.  If we have a static IP outside the
   // DHCP server's scope, it may simply ignore discover
   // messages.
   MyIP.Val = AppConfig.MyIPAddr.Val;
   if(!bRenewing)
      AppConfig.MyIPAddr.Val = 0x00000000;
   UDPFlush();
   AppConfig.MyIPAddr.Val = MyIP.Val;
	
}

#endif    //#if defined(STACK_USE_DHCP)
