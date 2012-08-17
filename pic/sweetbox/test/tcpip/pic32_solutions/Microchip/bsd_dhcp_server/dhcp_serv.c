/*********************************************************************
 *
 *  Dynamic Host Configuration Protocol (DHCP) Server
 *  Module for Microchip TCP/IP Stack
 *	 -Provides automatic IP address, subnet mask, gateway address, 
 *	  DNS server address, and other configuration parameters on DHCP 
 *	  enabled networks.
 *	 -Reference: RFC 2131, 2132
 *
 *********************************************************************
 * FileName:        DHCPs.c
 * Dependencies:    UDP, ARP, Tick
 * Processor:       PIC32
 * Complier:        MPLAB C32
 *
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright © 2002-2007 Microchip Technology Inc.  All rights 
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and 
 * distribute: 
 * (i)  the Software when embedded on a Microchip microcontroller or 
 *      digital signal controller product (“Device”) which is 
 *      integrated into Licensee’s product; or
 * (ii) ONLY the Software driver source files ENC28J60.c and 
 *      ENC28J60.h ported to a non-Microchip device used in 
 *      conjunction with a Microchip ethernet controller for the 
 *      sole purpose of interfacing with the ethernet controller. 
 *
 * You should refer to the license agreement accompanying this 
 * Software for additional information regarding your rights and 
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT 
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT 
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL 
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR 
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF 
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS 
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE 
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER 
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT 
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date    	Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder     02/28/07	Original
 * Abdul Rafiq			07/16/07	Port to HPMD BSD stack
 ********************************************************************/

#include "generic.h"
#include <bsd_dhcp_client\dhcp.h>
#include "bsd_dhcp_client\dhcp_private.h"
#include <stdio.h>
#include <string.h>

#define DHCP_LEASE_DURATION			(60)	// Seconds.  This is extremely short so the client(s) won't use our IP address for long if we inadvertently give out a DHCP lease while there is another DHCP server on this network that we didn't know about.

//#define DHCP_MAX_LEASES				2		// Not implemented
#define DHCP_IP_LEASE_TIME              (51u)

// ClientMAC multicast bit is used to determine if a lease is given out or not.
// Lease IP address is derived from index into DCB array
typedef struct _DHCP_CONTROL_BLOCK
{
	TICK 		LeaseExpires;
	MAC_ADDR	ClientMAC;
	enum 
	{
		LEASE_UNUSED = 0,
		LEASE_REQUESTED,
		LEASE_GRANTED
	} smLease;
} DHCP_CONTROL_BLOCK;

static SOCKET		SrvSock = INVALID_SOCKET;
static IP_ADDR		DHCPNextLease;
//static DHCP_CONTROL_BLOCK	DCB[DHCP_MAX_LEASES];	// Not Implmented

static void DHCPReply(DHCP_PACKET * pPkt, BYTE OpCode );
static void SelectNewLeaseIP();
int	DHCPBindCount = 0;
extern BOOL gDHCPServerDetected;

/*********************************************************************
 * Function:        void DHCPServerTask(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Processes DHCP requests and distributes IP 
 *					addresses
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
SOCKADDR_IN			dhcpClient;
DHCP_PACKET			DHCPMsg;
void DHCPServerTask(void)
{
	int			OpCode;
	DWORD		dw;
	BOOL		bAccept;
	int			rcvCount;

	static enum
	{
		DHCP_OPEN_SOCKET,
		DHCP_LISTEN
	} smDHCPServer = DHCP_OPEN_SOCKET;

	// Make sure we don't clobber anyone else's DHCP server
	if(gDHCPServerDetected)
		return;

	switch(smDHCPServer)
	{

		/////////////////////////////////////
		/////////////////////////////////////
		case DHCP_OPEN_SOCKET:

			// Obtain a UDP socket
			SrvSock = socket(	AF_INET,		// internetwork: UDP, TCP, etc
					      		SOCK_DGRAM,		// connectionless
					      		IPPROTO_UDP);	// user datagram protocol

			if( SrvSock == INVALID_SOCKET )
				return;
		
			dhcpClient.sin_port			= DHCP_SERVER_PORT;
			dhcpClient.sin_family		= AF_INET;
			dhcpClient.sin_addr.s_addr	= INADDR_ANY;
		
			if( bind(SrvSock, (LPSOCKADDR)&dhcpClient,	sizeof(struct sockaddr)) < 0 )
			{
				closesocket(SrvSock);
				SrvSock = INVALID_SOCKET;
				return;
			}
		
			SelectNewLeaseIP();

			smDHCPServer = DHCP_LISTEN;

			break;

		/////////////////////////////////////
		/////////////////////////////////////
		case DHCP_LISTEN:

			// Retrieve the BOOTP header
			dw = sizeof(SOCKADDR_IN);
			rcvCount = recvfrom( SrvSock, (char*)&DHCPMsg, sizeof(DHCPMsg), 0, (struct sockaddr *)&dhcpClient, (int*)&dw );

			if( rcvCount <= 0 )
				break;

			// Validate first three fields
			if(DHCPMsg.dhcpHeader.Op != 1u)
				break;
			if(DHCPMsg.dhcpHeader.Htype != 1u)
				break;
			if(DHCPMsg.dhcpHeader.Hlen != 6u)
				break;

			// Obtain Magic Cookie and verify
			dw = *(DWORD*)DHCPMsg.dhcpOptions;	//first four bytes after header = magic cookie
			if(dw != 0x63538263ul)
				break;

			switch(DHCPMsg.dhcpOptions[6])	//message type
			{
			case DHCP_DISCOVER_MESSAGE:
				SelectNewLeaseIP();
				DHCPReply( &DHCPMsg, DHCP_OFFER_MESSAGE );
				break;

			case DHCP_REQUEST_MESSAGE:

				// Someone is using our DHCP server, start using a static 
				// IP address and update the bind count.

				if( (DHCPMsg.dhcpHeader.Ciaddr.Val == DHCPNextLease.Val) || (DHCPMsg.dhcpHeader.Ciaddr.Val == 0x00000000) )
				{
					OpCode = DHCP_ACK_MESSAGE;
					DHCPBindCount++;
				}
				else
					OpCode = DHCP_NAK_MESSAGE;

				DHCPReply( &DHCPMsg, OpCode ); 
				break;

			// Need to handle these if supporting more than one DHCP lease
			case DHCP_RELEASE_MESSAGE:
			case DHCP_DECLINE_MESSAGE:
				break;
			}

	}
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
static void SelectNewLeaseIP()
{
	// Decide which address to lease out
	// Note that this needs to be changed if we are to 
	// support more than one lease
	DHCPNextLease.Val = (SysIPAddr.Val & SysIPMask.Val) + 0x02000000;

	if(DHCPNextLease.v[3] == 255)
		DHCPNextLease.v[3] += 0x03;

	if(DHCPNextLease.v[3] == 0)
		DHCPNextLease.v[3] += 0x02;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
static void DHCPReply(DHCP_PACKET * pPkt, BYTE OpCode )
{
	DHCP_MSG_HEADER* pHdr;
	pHdr = &pPkt->dhcpHeader;
			
	pHdr->Op = BOOT_REPLY;
//	pHdr->Htype;
//	pHdr->Hlen;
//	pHdr->Hops;
//	pHdr->Xid[2];
	pHdr->Secs = 0;	// Seconds Elapsed: 0 (Not used)
	pHdr->Flags.Val = 0; //unicast
	pHdr->Ciaddr.Val = 0;			// Your (client) IP Address: 0.0.0.0 (none yet assigned)
	pHdr->Yiaddr.Val = DHCPNextLease.Val; // Lease IP address to give out

	pHdr->Siaddr.Val = 0; // Next Server IP Address: 0.0.0.0 (not used)
	pHdr->Giaddr.Val = 0; // Relay Agent IP Address: 0.0.0.0 (not used)

//	pHdr->Chaddr[16];
//	pHdr->Sname[64];
//	pHdr->file[128];
//	for(i = 0; i < 64+128+(16-sizeof(MAC_ADDR)); i++)	// Remaining 10 bytes of client hardware address, server host name: Null string (not used)
//		UDPPut(0x00);									// Boot filename: Null string (not used)

	BYTE *OBfr = (BYTE*)pPkt->dhcpOptions;

	OBfr[0] = 0x63;				// Magic Cookie: 0x63538263
	OBfr[1] = 0x82;				// Magic Cookie: 0x63538263
	OBfr[2] = 0x53;				// Magic Cookie: 0x63538263
	OBfr[3] = 0x63;				// Magic Cookie: 0x63538263
	
	// Options: DHCP Offer
	OBfr[4] = DHCP_MESSAGE_TYPE;	
	OBfr[5] = 1;
	OBfr[6] = OpCode;

	// Option: Subnet Mask
	OBfr[7] = DHCP_SUBNET_MASK;
	OBfr[8] = sizeof(IP_ADDR);
	OBfr[9] = SysIPMask.v[0];
	OBfr[10] = SysIPMask.v[1];
	OBfr[11] = SysIPMask.v[2];
	OBfr[12] = SysIPMask.v[3];

	// Option: Server identifier
	OBfr[13] = DHCP_SERVER_IDENTIFIER;
	OBfr[14] = sizeof(IP_ADDR);

	OBfr[15] = SysIPAddr.v[0];
	OBfr[16] = SysIPAddr.v[1];
	OBfr[17] = SysIPAddr.v[2];
	OBfr[18] = SysIPAddr.v[3];

	// Option: Lease duration
	OBfr[19] = DHCP_IP_LEASE_TIME;
	OBfr[20] = 4;
	OBfr[21] = (DHCP_LEASE_DURATION>>24) & 0xFF;
	OBfr[22] = (DHCP_LEASE_DURATION>>16) & 0xFF;
	OBfr[23] = (DHCP_LEASE_DURATION>>8)  & 0xFF;
	OBfr[24] = (DHCP_LEASE_DURATION) 	 & 0xFF;

	// Option: Router/Gateway address
	OBfr[25] = DHCP_ROUTER;
	OBfr[26] = sizeof(IP_ADDR);

	OBfr[27] = SysIPAddr.v[0];
	OBfr[28] = SysIPAddr.v[1];
	OBfr[29] = SysIPAddr.v[2];
	OBfr[30] = SysIPAddr.v[3];

	// No more options, mark ending
	OBfr[31] = DHCP_END_OPTION;

	// Transmit the packet

	SOCKADDR_IN dhcpClient;
	int len = sizeof(SOCKADDR_IN);

	dhcpClient.sin_port			= DHCP_CLIENT_PORT;
	dhcpClient.sin_family		= AF_INET;
	dhcpClient.sin_addr.s_addr	= 0xFFFFFFFF;

	sendto( SrvSock, (char*)pPkt, sizeof(DHCP_MSG_HEADER)+32, 0, (LPSOCKADDR)&dhcpClient, len );
}

