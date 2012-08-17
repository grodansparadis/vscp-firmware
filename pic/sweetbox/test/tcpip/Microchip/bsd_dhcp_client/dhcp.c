/********************************************************************
 * $Id: dhcp.c,v 1.3 2006/07/08 00:11:08 C13217 Exp $
 * $Name:  $
 * FileName:		DHCP.c
 * Dependencies:
 * Processor:
 * Hardware:
 * Assembler:		MPLAB C30
 * Linker:
 * Company:			Microchip Technology, Inc.
 *
 * Software License Agreement:
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *********************************************************************
 * File Description:
 *
 * DHCP Client Appilcation
 *
 * Change History:
 * Name				Date            Changes
 * Chad Mercer 		06/19/06 	Initial Version
 ********************************************************************/
#define THIS_IS_DHCP

#include "generic.h"
#include <bsd_dhcp_client\dhcp.h>
#include "bsd_dhcp_client\dhcp_private.h"

#include <stdio.h>
#include <string.h>

PUBLIC BOOL gDHCPServerDetected = FALSE;

/*
 * Uncomment following line if DHCP transactions are to be displayed on
 * RS-232 - for debug purpose only.
 */
//#define DHCP_DEBUG_MODE

#if defined(DHCP_DEBUG_MODE)
static USARTPut(BYTE c)
{
    while( !TXSTA_TRMT);
    TXREG = c;
}
#else

#define USARTPut(a)

#endif


/*********************************************************************
 * Function:		PUBLIC void DHCPInit(void)
 *
 * PreCondition:    The program needs to have already initialized
 *		    		TCP/IP stack.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    A socket will be created on the DHCP Port
 *
 * Overview:        The function creates a UDP socket and binds it
 *		    		to the DHCP Port to allow the application to
 *		   			act as a DHCP Client Agent
 ********************************************************************/
PUBLIC void DHCPInit(void)
{
	SOCKADDR_IN		dhcpClient;
	int             result;

	/*
	 * Use tick count to seed srand function
	 */
	srand(TickGet());
	
	_dhcpSocket = socket(	AF_INET,		// internetwork: UDP, TCP, etc
			      			SOCK_DGRAM,		// connectionless
			      			IPPROTO_UDP);	// user datagram protocol


	if(_dhcpSocket == INVALID_SOCKET)
		return;

	dhcpClient.sin_port			= DHCP_CLIENT_PORT;
	dhcpClient.sin_family		= AF_INET;
	dhcpClient.sin_addr.s_addr	= INADDR_BROADCAST;

	result = bind(	_dhcpSocket,
					(LPSOCKADDR)&dhcpClient,
					sizeof(struct sockaddr));

	if(result == SOCKET_ERROR)
	{
		closesocket(_dhcpSocket);
		_dhcpSocket = INVALID_SOCKET;
		smDHCPState == SM_DHCP_ABORTED;
	}

	/*
	 * DHCP Client statemachine starts in init state
	 */
    smDHCPState = SM_DHCP_INIT;				      
	
	_numOfAttempts = 0;
	_retryDelay	= 0;

    USARTPut('I');

    _DebugCntr = 0;
    

}
/*********************************************************************
 * Function:		PUBLIC BOOL DHCPTask(void)
 *
 * PreCondition:    DHCPInit() needs to be called before DHCPTask can
 *					effectivity run
 *
 * Input:           None
 *
 * Output:          true if a DHCP UDP packet was successfully processed,
 *					otherwise false
 *
 * Side Effects:    None
 *
 * Overview:        This function serveS as the state machine for the
 *                  DHCP client. As DHCP packets are exchanged between the
 *                  client and the server this task will advance through the
 *                  various state conditions. This task is also responsible
 *                  for monitoring lease experation conditions and handling
 *					retransmissions.
 *
 * Note:			None
 ********************************************************************/
PUBLIC void DHCPTask(void)
{
    static TICK 	lastTryTick;
    BYTE 			DHCPRecvReturnValue;

    switch(smDHCPState)
    {
    case SM_DHCP_INIT:
		if(_numOfAttempts >= MAX_NUM_RETRY)
		{
			/*
			 * Max number of retries have been met
			 * Abort the DHCP client.
			 */
			_DHCPAbort();	
			_DebugCntr = 255;
			break;
		}
  
	    tempIPAddress.Val = 0x0;
		lastTryTick = TickGet();		
		
		/*
		 * Assign a unique client ID for this transaction
		 */
		_Xid[0] = rand();
		_Xid[1] = rand();

        smDHCPState = SM_DHCP_RESET_WAIT;
 
    	_DebugCntr = 1;

       /* No break */


    case SM_DHCP_RESET_WAIT:

		/* 
		 * Waitloop is used to create a randomized exponentiation
		 * backoff delay used for retransmissions 
         */
        if ( _retryDelay != 0x0 )			
        {
        	if ( TickGetDiff(TickGet(), lastTryTick) >= 
				(TICKS_PER_SECOND * ((rand() & 0xf) + _retryDelay)) )
			{
            	smDHCPState = SM_DHCP_DISCOVER;
			}
		}
		else
		{
			_retryDelay = 1;
			smDHCPState = SM_DHCP_DISCOVER;
		}

        break;

    case SM_DHCP_DISCOVER:
             
        _DHCPSend(DHCP_DISCOVER_MESSAGE);

        // DEBUG
        USARTPut('D');

        lastTryTick = TickGet();
        smDHCPState = SM_DHCP_SELECTING;

        _DebugCntr = 2;

        break;


    case SM_DHCP_SELECTING:

        if ( TickGetDiff(TickGet(), lastTryTick) >= DHCP_TIMEOUT )
        {
			/*
			 * DHCP timeout occured. Need to resend 
			 * discovery message.
			 */
            smDHCPState = SM_DHCP_RESET_WAIT;
			lastTryTick = TickGet();
			if(_retryDelay < MAX_TRY_TIME)
			{
				_retryDelay = _retryDelay * 2;
				_numOfAttempts++;	
				break;
			}	
			else
			{
            	_DHCPAbort();
				_DebugCntr = 254;

				//DEBUG
	            USARTPut('A');
				return;
			}
        }
		/* 
		 * Wait for offer message reply from server
		 */
        if ( _DHCPReceive() == DHCP_OFFER_MESSAGE )
        {
            // DEBUG
            USARTPut('O');
            smDHCPState = SM_DHCP_REQUESTING;
			gDHCPServerDetected = TRUE;
        }
        else
            break;

    case SM_DHCP_REQUESTING:
		/* 
		 * Send Request message to offering server for 
		 * IP lease
		 */
        _DHCPSend(DHCP_REQUEST_MESSAGE);

        lastTryTick = TickGet();
        smDHCPState = SM_DHCP_PREBOUND;

        // DEBUG
        USARTPut('R');
       _DebugCntr = 3;

        break;

    case SM_DHCP_PREBOUND:

        DHCPRecvReturnValue = _DHCPReceive();
        if ( DHCPRecvReturnValue == DHCP_NAK_MESSAGE )
        {
            /* NAK recieved from server.  DHCP server 
			 * didn't like our DHCP Request format.
			 * Need to retry
			 */
            USARTPut('n');
			_numOfAttempts++;
			if(_retryDelay < MAX_TRY_TIME)
				_retryDelay = _retryDelay * 2;
            smDHCPState = SM_DHCP_INIT;			 
        }
        else if ( DHCPRecvReturnValue == DHCP_ACK_MESSAGE )
        {
            // DEBUG
            USARTPut('B');

            lastTryTick = TickGet();
			_numOfAttempts = 0;
			_retryDelay = 0;
            smDHCPState = SM_DHCP_BOUND;
			
			/* DHCP client is bound to an IP address.
			 * Store off the IP address and other
			 * paramaters
			 */
			SysIPAddr.Val = tempIPAddress.Val;


        }
        else
		{ 
			if ( TickGetDiff(TickGet(), lastTryTick) >= DHCP_TIMEOUT )
        	{
            USARTPut('t');
            smDHCPState = SM_DHCP_INIT;
			}
        }  

		//DEBUG
        _DebugCntr = 4;
        break;

    case SM_DHCP_BOUND:
        /*
         * While in bound state keep track of how long 
		 * we use this IP configuration. When lease 
		 * period expires, renew the configuration.
         */
        if ( TickGetDiff(TickGet(), lastTryTick) >= TICKS_PER_SECOND )
        {
            DHCPLeaseTime_T1.Val -= 1;
			if ( DHCPLeaseTime_T1.Val == 0 )
			{
				/*
         		 * Attempt to renew the same IP address before 
		 		 * lease expires using unicast message.
         		 */
				smDHCPState = SM_DHCP_RENEW; 
				_DHCPSend(DHCP_REQUEST_MESSAGE);
				DHCPLeaseTime_T1.Val = DHCPLeaseTime_T2.Val - (DHCPLeaseTime_T2.Val * 7)/8;
				DHCPLeaseTime_T2.Val = DHCPLeaseTime_T2.Val/2;
				break;
			}

            lastTryTick = TickGet();
        }
		/* 
		 * Ignore any spurious DHCP messages that is 
	     * recieved while in BOUND state.
		 */
		_DHCPReceive();
        _DebugCntr = 5;
		break;


	case SM_DHCP_RENEW:
        
        if ( TickGetDiff(TickGet(), lastTryTick) >= TICKS_PER_SECOND )
        {
            DHCPLeaseTime_T1.Val -= 1;
			DHCPLeaseTime_T2.Val -= 1;
            if ( DHCPLeaseTime_T1.Val == 0 )
			{
				/* 
				 * Attempt to renew the same IP address before 
		 		 * lease expires using broadcast message.
				 */
                smDHCPState = SM_DHCP_REBIND;
				_DHCPSend(DHCP_REQUEST_MESSAGE);
				break;
			}

			lastTryTick = TickGet();

		}

        if ( _DHCPReceive() == DHCP_ACK_MESSAGE )
		{
			smDHCPState = SM_DHCP_BOUND;
			lastTryTick = TickGet();
		}

		//DEBUG
        USARTPut('R');
        _DebugCntr = 6;
		
		break;

	case SM_DHCP_REBIND:
        if ( TickGetDiff(TickGet(), lastTryTick) >= TICKS_PER_SECOND )
        {
            DHCPLeaseTime_T2.Val -= 1;
            if ( DHCPLeaseTime_T2.Val == 0 )
			{
				/* 
				 * Lease has expired. Restart DHCP
				 */
                smDHCPState = SM_DHCP_INIT;
				_numOfAttempts = 0;
				_retryDelay = 1;
				break;
			}

			lastTryTick = TickGet();

		}

        if ( _DHCPReceive() == DHCP_ACK_MESSAGE )
		{
			smDHCPState = SM_DHCP_BOUND;
			lastTryTick = TickGet();
		}

		//DEBUG
        USARTPut('B');
        _DebugCntr = 7;

		break;

    }

}


/*********************************************************************
 * Function:        PRIVATE void _DHCPAbort(void)
 *
 * PreCondition:    DHCPTask() must have been called at least once.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Forgets about any previous DHCP attempts and
 *                  closes DHCPSocket.
 *
 * Note:
 ********************************************************************/
PRIVATE void _DHCPAbort(void)
{
    smDHCPState = SM_DHCP_ABORTED;
	closesocket(_dhcpSocket);

}

/*********************************************************************
 * Function:        PRIVATE int _DHCPReceive()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          type - What type of message was recieved
 *
 * Side Effects:    None
 *
 * Overview:        This function processes any recieved DHCP message 
 *                  and records the appropriate fields.
 *
 * Note:			None
 ********************************************************************/
PRIVATE int _DHCPReceive()
{
    BYTE 				v;
    int 				i;
	int					result;
    BYTE 				type;
	BYTE 				packet[DHCP_MAX_PKT_LEN];
	SOCKADDR_IN			saServer;
	DHCP_MSG_HEADER*	pdhcpMsgHeader;
    BOOL 				lbDone;
	int					len;
    DWORD_VAL 			tempServerID;
	DWORD_VAL 			tempLeaseTime;

    /*
	 * Assume unknown message until proven otherwise.
	 */
    type = DHCP_UNKNOWN_MESSAGE;

	len = sizeof(SOCKADDR_IN);

	result = recvfrom(	_dhcpSocket,
						packet,
						DHCP_MAX_PKT_LEN,
						0,
						(LPSOCKADDR)&saServer,
						&len);

	/*
	 * Exit if no packet is ready to be recieved
	 */
	if (result < 1)
		return type;

	/*
	 * Ignore any incoming packets while in 
	 * BOUND state
	 */
	if(smDHCPState == SM_DHCP_BOUND)
		return type;


	pdhcpMsgHeader = (DHCP_MSG_HEADER *)packet;

    /*
     * Make sure this is BOOT_REPLY
     */
    if ( pdhcpMsgHeader->Op == BOOT_REPLY )
    {
		/*
		 * Check that this DHCP reply matches the client's
		 * request id
		 */
		if( pdhcpMsgHeader->Xid[0] != _Xid[0] ||
			pdhcpMsgHeader->Xid[1] != _Xid[1])
			return type; 
		
        /*
         * Save offered IP address until we know for sure that
         * we have it (ie. after ACK).
         */
		tempIPAddress.Val = pdhcpMsgHeader->Yiaddr.Val;

        /*
         * Check to see if chaddr (Client Hardware Address) belongs to us.
         */
		for(i =0 ; i < 6; i++)
		{
		    if( pdhcpMsgHeader->Chaddr[i] != SysMACAddr.v[i])
				return type;
 		}

        /*
         * Skip past DHCP header and magic cookie.
         */
		i = sizeof(DHCP_MSG_HEADER) + 4;
		
        lbDone = FALSE;
		/* 
		 * This loop processes the DHCP option headers
		 * and stores off the appropriate data fields.
		 */
        do
        {
            switch(packet[i])
            {
            case DHCP_MESSAGE_TYPE:
                i++;							
                // Len must be 1.
                if ( packet[i++] == 1 )
                {
					/* 
					 * Get message type
					 */
                    type = packet[i++];                  
                }
                else
                    return type;
                break;

            case DHCP_SUBNET_MASK:
				i++;
                // Len must be 4.
                if ( packet[i++] == 4 )
                {
					UPPER_MSB(tempMask) = packet[i++];
 					UPPER_LSB(tempMask) = packet[i++];
					LOWER_MSB(tempMask) = packet[i++];
					LOWER_LSB(tempMask) = packet[i++];
                }
                else
                    return DHCP_UNKNOWN_MESSAGE;
                break;

            case DHCP_ROUTER:
				i++;
				len = packet[i++];
                // Len must be >= 4.
                if ( len >= 4 )
                {
					SysIPGateway.v[0] = packet[i++];
 					SysIPGateway.v[1] = packet[i++];
					SysIPGateway.v[2] = packet[i++];
					SysIPGateway.v[3] = packet[i++];

                }
                else
                    return DHCP_UNKNOWN_MESSAGE;
  
                /*
                 * Discard any other router addresses.
                 */
                i += len - 4;
                break;

            case DHCP_SERVER_IDENTIFIER:
				i++;
                // Len must be 4.
                if ( packet[i++] == 4 )
                {
					UPPER_MSB(tempServerID) = packet[i++];
 					UPPER_LSB(tempServerID) = packet[i++];
					LOWER_MSB(tempServerID) = packet[i++];
					LOWER_LSB(tempServerID) = packet[i++];
                }
                else
                    return DHCP_UNKNOWN_MESSAGE;
                break;

            case DHCP_PARAM_IP_LEASE_TIME:
				i++;
                // Len must be 4.
                if ( packet[i++] == 4 )
                {
					UPPER_MSB(tempLeaseTime) = packet[i++];
 					UPPER_LSB(tempLeaseTime) = packet[i++];
					LOWER_MSB(tempLeaseTime) = packet[i++];
					LOWER_LSB(tempLeaseTime) = packet[i++];
					
					if ( (smDHCPState == SM_DHCP_PREBOUND) ||
						 (smDHCPState == SM_DHCP_RENEW) || 
					     (smDHCPState == SM_DHCP_REBIND) )
					{
                    	/*
                     	 * Configure T1 to be 1/2 of Lease time 
                     	 */
				  		DHCPLeaseTime_T1.Val = (tempLeaseTime.Val / 2);
                       	DHCPLeaseTime_T2.Val = tempLeaseTime.Val;
					}
                }
                else
                    return DHCP_UNKNOWN_MESSAGE;
                break;
  
            case DHCP_END_OPTION:
                lbDone = TRUE;
                break;

            default:
                // Ignore all unsupport tags.
				i++;
				len = packet[i++];
                i += len;
            }
        } while( !lbDone );
    }

    /*
     * If this is an OFFER message, remember current server id.
     */
    if ( type == DHCP_OFFER_MESSAGE )
    {
        DHCPServerID.Val = tempServerID.Val;        
    }
    else
    {
        /*
         * For other types of messages, make sure that received
         * server id matches with our previous one.
         */
        if ( DHCPServerID.Val != tempServerID.Val )
            type = DHCP_UNKNOWN_MESSAGE;
    }


    return type;

}

/*********************************************************************
 * Function:        PRIVATE void _DHCPSend(BYTE messageTyp)
 *
 * PreCondition:    None
 *
 * Input:           messageType - The type of DHCP message to be sent
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function generates the proper DHCP message
 *					and then transmits it.
 *
 * Note:			None
 ********************************************************************/

PRIVATE void _DHCPSend(BYTE messageType)
{
    BYTE i = 0;
	SOCKADDR_IN	dhcpServer;

	/*
	 * The dchpPacket size is based on the maximum option field size
     * required by the dhcp client. 
     *     [Header + Cookie+  Msg Type +  Req List + Req IP + Srv ID + End]
     * 
	 * If more options are added the dhcpPacket.dhcpOptions size must increase 
     * accordingly.
	 */
	DHCP_PACKET dhcpPacket;					

	/*
	 * Set fixed values in the DHCP message header
	 */
	dhcpPacket.dhcpHeader.Op = BOOT_REQUEST; 
	dhcpPacket.dhcpHeader.Htype = HW_TYPE;
	dhcpPacket.dhcpHeader.Hlen = LEN_OF_HW_TYPE;
	dhcpPacket.dhcpHeader.Hops = 0;
	dhcpPacket.dhcpHeader.Xid[0] = _Xid[0];
	dhcpPacket.dhcpHeader.Xid[1] = _Xid[1];
	dhcpPacket.dhcpHeader.Secs = 0;

	/*
	 * Broadcast flag is set
	 */
	dhcpPacket.dhcpHeader.Flags.Val = 0x0080;

	dhcpPacket.dhcpHeader.Ciaddr.Val = 0x0;
	dhcpPacket.dhcpHeader.Yiaddr.Val = 0x0;
	dhcpPacket.dhcpHeader.Siaddr.Val = 0x0;
	dhcpPacket.dhcpHeader.Giaddr.Val = 0x0;

    /*
	 * Load MAC address to chaddr field
	 */
	for(i =0 ; i < 6; i++)
	{
	    dhcpPacket.dhcpHeader.Chaddr[i] = SysMACAddr.v[i]; 
	}
	for(i = 6 ; i < 16; i++)
	{
		//Zero the rest of the chaddr field
   		dhcpPacket.dhcpHeader.Chaddr[i] = 0x0; 			 
	}
	
	memset(dhcpPacket.dhcpHeader.Sname, 0x0, 64);
	memset(dhcpPacket.dhcpHeader.file, 0x0, 128);


    /*
     * Load magic cookie as per RFC 1533.
     */
	i = 0;
	dhcpPacket.dhcpOptions[i++] = 0x63;
	dhcpPacket.dhcpOptions[i++] = 0x82;
	dhcpPacket.dhcpOptions[i++] = 0x53;
	dhcpPacket.dhcpOptions[i++] = 0x63;

    /*
     * Load message type.
     */
	dhcpPacket.dhcpOptions[i++] = DHCP_MESSAGE_TYPE;
	dhcpPacket.dhcpOptions[i++] = DHCP_MESSAGE_TYPE_LEN;	
	dhcpPacket.dhcpOptions[i++] = messageType;

    /*
     * Load our interested request parameters
     * This is hardcoded list.  If any new parameters are desired,
     * new lines must be added here and dhcpOptions size must 
     * be increased.
     */
	dhcpPacket.dhcpOptions[i++] = DHCP_PARAM_REQUEST_LIST;
	dhcpPacket.dhcpOptions[i++] = DHCP_PARAM_REQUEST_LIST_LEN;
	dhcpPacket.dhcpOptions[i++] = DHCP_SUBNET_MASK;
	dhcpPacket.dhcpOptions[i++] = DHCP_ROUTER;

	switch(messageType)
	{
	case DHCP_DISCOVER_MESSAGE:

		dhcpPacket.dhcpHeader.Ciaddr.Val = 0x0;

		//Use broadcast IP address	
		dhcpServer.sin_addr.s_addr	= INADDR_BROADCAST;

		break;

	case DHCP_REQUEST_MESSAGE:
		if (smDHCPState == SM_DHCP_REQUESTING)
		{
			/*
			 * _DHCPReceive() will populate "serverID" and 
			 * "tempIPAddress" when it receives DHCP OFFER
			 * message. This is reused when we are replying 
			 * to server.
	    	 */
         	dhcpPacket.dhcpOptions[i++] = DHCP_SERVER_IDENTIFIER;
	        dhcpPacket.dhcpOptions[i++] = DHCP_PARAM_IPv4_ADDRESS_LEN;
			dhcpPacket.dhcpOptions[i++] = UPPER_MSB(DHCPServerID);
			dhcpPacket.dhcpOptions[i++] = UPPER_LSB(DHCPServerID);
			dhcpPacket.dhcpOptions[i++] = LOWER_MSB(DHCPServerID);
			dhcpPacket.dhcpOptions[i++] = LOWER_LSB(DHCPServerID);

         	dhcpPacket.dhcpOptions[i++] = DHCP_PARAM_REQUEST_IP_ADDRESS;
	        dhcpPacket.dhcpOptions[i++] = DHCP_PARAM_IPv4_ADDRESS_LEN;
			dhcpPacket.dhcpOptions[i++] = LOWER_LSB(tempIPAddress);
			dhcpPacket.dhcpOptions[i++] = LOWER_MSB(tempIPAddress);
			dhcpPacket.dhcpOptions[i++] = UPPER_LSB(tempIPAddress);
			dhcpPacket.dhcpOptions[i++] = UPPER_MSB(tempIPAddress);


			//Use broadcast IP address	
			dhcpServer.sin_addr.s_addr	= INADDR_BROADCAST;

		}
		else if (smDHCPState == SM_DHCP_RENEW)
		{
			/*
			 * During renew cycle the client must send 
    		 * the current client's IP address in 
	    	 * ciaddr.
	    	 */
			dhcpPacket.dhcpHeader.Ciaddr.Val = tempIPAddress.Val;

			//Use unicast DST address
			dhcpServer.sin_addr.s_net	= UPPER_MSB(DHCPServerID);
			dhcpServer.sin_addr.s_host	= UPPER_LSB(DHCPServerID);
			dhcpServer.sin_addr.s_lh    = LOWER_MSB(DHCPServerID);
			dhcpServer.sin_addr.s_impno = LOWER_LSB(DHCPServerID);
			
		}
		else if (smDHCPState == SM_DHCP_REBIND)
		{
			dhcpPacket.dhcpHeader.Ciaddr.Val = tempIPAddress.Val;
			//Use broadcast IP address	
			dhcpServer.sin_addr.s_addr = INADDR_BROADCAST;

		}
		break;
	case DHCP_RELEASE_MESSAGE:
		break;
	}		
  
    /*
     * Add any new paramter request here.
     */

    /*
     * End of Options.
     */
	dhcpPacket.dhcpOptions[i++] = DHCP_END_OPTION;

//	memcpy((BYTE *) &dhcpPacket.dhcpHeader, (BYTE*) &_dhcpMsgHeader, 
//		   sizeof(DHCP_MSG_HEADER));


	dhcpServer.sin_port	= DHCP_SERVER_PORT;
	dhcpServer.sin_family = AF_INET;

	sendto(	_dhcpSocket,
			(BYTE *) &dhcpPacket,
			(i + sizeof(DHCP_MSG_HEADER)),
			0,
			(LPSOCKADDR)&dhcpServer,
			sizeof(struct sockaddr));
	
}
