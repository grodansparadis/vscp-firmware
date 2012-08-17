/*********************************************************************
 *
 *                  Test module for Microchip HTTP Server
 *
 *********************************************************************
 * FileName:        main.c
 * Processor:       PIC24
 * Complier:        MPLAB C30
 *
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement:
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its dsPIC30F and PICmicro® Microcontroller is intended
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip's dsPIC30F and PICmicro Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
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
 * Author               Date      Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * AR			9/27/07	  Initial Rlease
 *
 ********************************************************************/
#include <plib.h>
#include "bsd_dhcp_client\dhcp.h"
#include "ex16lcd.h"

//Config Fuses for 72Mhz Core and 36Mhz Peripheral
#pragma config FPLLMUL = MUL_18, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF, WDTPS = PS512
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_2, DEBUG = OFF, IESO = OFF, FCKSM = CSDCMD, FSOSCEN = OFF

///////////////////////////////////////////////////////
void SetupDefault_IP_MAC()
{
	// Initialize default IP and MAC address
	IP_StrToDWord( DEFAULT_IP_ADDR, 	&SysIPAddr.Val );
	IP_StrToDWord( DEFAULT_IP_MASK, 	&SysIPMask.Val );
	IP_StrToDWord( DEFAULT_IP_GATEWAY, 	&SysIPGateway.Val );
	MAC_StrToBin ( DEFAULT_MAC_ADDR,	SysMACAddr.v );
}

#define PC_SERVER_IP_ADDR "10.10.33.79"

///////////////////////////////////////////////////////
int main()
{
	DWORD TestServerIP;
	int len, ClTx = 0, EchoCntr = 0;
	SOCKET s, srvr, StreamSock, clSock;
	BOOL ClientConnected = FALSE;

	IP_ADDR	curr_ip;
	static BYTE bfr[50];
	struct sockaddr_in addr;
	int addrlen = sizeof(struct sockaddr_in);


    SYSTEMConfigWaitStatesAndPB(72000000);
	// Turn on the interrupts
	INTEnableSystemMultiVectoredInt();

    Ex16LCDInit(36000000);
    Ex16LCDWriteLine(1, "PIC32 BSD SOCK");

    // Initialize the TCP/IP
    Ex16LCDWriteLine(2, "Init TCP/IP");
	SetupDefault_IP_MAC();
	MSTimerInit(36000000);	
	InitStackMgr();
	TickInit();

	DHCPInit();

	//create tcp server socket /////////////////
	if( (srvr = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP )) == SOCKET_ERROR )
		return -1;

	//bind to a local port
	addr.sin_port = 6653;
	addr.sin_addr.S_un.S_addr = IP_ADDR_ANY;
	if( bind( srvr, (struct sockaddr*)&addr, addrlen ) == SOCKET_ERROR )
		return -1;

	listen( srvr, 5 );

	//create datagram socket //////////////////
	if( (s = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP )) == SOCKET_ERROR )
		return -1;

	//bind to a local port
	addr.sin_port = 6652;
	addr.sin_addr.S_un.S_addr = IP_ADDR_ANY;
	if( bind( s, (struct sockaddr*)&addr, addrlen ) == SOCKET_ERROR )
		return -1;

	StreamSock = INVALID_SOCK;
	clSock = INVALID_SOCK;
	IP_StrToDWord( PC_SERVER_IP_ADDR, 	&TestServerIP );

	curr_ip.Val = 0;

	while(1)
	{
		StackMgrProcess();
		DHCPTask();

		//if dhcp server changed our ip addr, then display the new value
		if(curr_ip.Val != SysIPAddr.Val)
		{
	        BYTE  ip_addr[17];
	        BYTE  number[4];

	        strcpy(ip_addr, "  ");
	        itoa(SysIPAddr.v[0],number);
	        strcat(ip_addr, number);
	        strcat(ip_addr, ".");
	        itoa(SysIPAddr.v[1],number);
	        strcat(ip_addr, number);
	        strcat(ip_addr, ".");
	        itoa(SysIPAddr.v[2],number);
	        strcat(ip_addr, number);
	        strcat(ip_addr, ".");
	        itoa(SysIPAddr.v[3],number);
	        strcat(ip_addr, number);

    		Ex16LCDWriteLine(2, ip_addr);

    		curr_ip.Val = SysIPAddr.Val;
		}

		///////////// TCP Client Test Code

		if( clSock == INVALID_SOCK )
		{
			//create tcp client socket /////////////////
			if( (clSock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP )) == SOCKET_ERROR )
				return -1;

			//bind to a unique local port
			addr.sin_port = 0;
			addr.sin_addr.S_un.S_addr = IP_ADDR_ANY;
			if( bind( clSock, (struct sockaddr*)&addr, addrlen ) == SOCKET_ERROR )
				return -1;

			addr.sin_port = 7000;
			addr.sin_addr.S_un.S_addr = TestServerIP;
			addrlen = sizeof(struct sockaddr);

			//len = 11;
			//setsockopt( clSock, SOL_SOCKET, SO_SNDBUF, (char*)&len, sizeof(int) );

			len = 1;
			setsockopt( clSock, SOL_SOCKET, TCP_NODELAY, (char*)&len, sizeof(int) );

			connect( clSock, (struct sockaddr*)&addr, addrlen );
			ClientConnected = FALSE;
		}

		if( !ClientConnected )
		{
			addr.sin_port = 7000;
			addr.sin_addr.S_un.S_addr = TestServerIP;
			addrlen = sizeof(struct sockaddr);

			if( connect( clSock, (struct sockaddr*)&addr, addrlen ) == 0 )
				ClientConnected = TRUE;
		}

		if( ClientConnected )
		{
			if( EchoCntr == 10 )
			{
				closesocket( clSock );
				clSock = INVALID_SOCK;
				ClTx = FALSE;
				ClientConnected = FALSE;
				EchoCntr = 0;
			}
			else if( !ClTx )
			{
				strcpy( bfr, "1234567890" );
				send( clSock, bfr, 11, 0 );
				ClTx = TRUE;
			}
			else
			{
				len = recvfrom( clSock, bfr, sizeof(bfr), 0, NULL, NULL );
				if( len > 0 )
				{
					ClTx = FALSE;
					EchoCntr++;
				}
				else if( len < 0 )
				{
					closesocket( clSock );
					clSock = INVALID_SOCK;
					ClTx = FALSE;
					ClientConnected = FALSE;
					EchoCntr = 0;
				}
			}
		}

		///////////// TCP Server Test Code
		if( StreamSock == INVALID_SOCK )
		{
			StreamSock = accept( srvr, (struct sockaddr*)&addr, &addrlen );
			if( StreamSock != INVALID_SOCK )
			{
				len = 17;
				setsockopt( StreamSock, SOL_SOCKET, SO_SNDBUF, (char*)&len, sizeof(int) );
			}
		}
		else
		{
			//receive TCP data
			len = recvfrom( StreamSock, bfr, sizeof(bfr), 0, NULL, NULL );
			if( len > 0 )
			{
				send( StreamSock, bfr, len, 0 );
			}
			else if( len < 0 )
			{
				closesocket( StreamSock );
				StreamSock = SOCKET_ERROR;
			}
		}

		//////////////// UDP rx/tx datagrams Test
		addrlen = sizeof(struct sockaddr_in);
		if( (len = recvfrom( s, bfr, sizeof(bfr), 0, (struct sockaddr*)&addr, &addrlen )) > 0 )
		{
			len++;
			len--;

			sendto( s, bfr, strlen(bfr)+1, 0, (struct sockaddr*)&addr, addrlen ); //echo back
		}
	}

}
