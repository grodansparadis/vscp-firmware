/*********************************************************************
 *
 *            IP Route Implementation File
 *
 *********************************************************************
 * FileName:        Route.c
 * Description: 	IP Route Implementation File
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
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
 * $Id: route.c,v 1.5 2006/09/28 22:10:27 C12770 Exp $
 *
*/

#include "bsd_tcp_ip\ether.h"
#include "bsd_tcp_ip\route.h"
#include <string.h>
#include "bsd_tcp_ip\gpfunc.h"

#define MAX_HOST_ROUTES	16

#define RTE_UNDEFINED			0
#define RTE_WAIT_ARP_REPLY		1
#define RTE_CREATED_REMOTE		2
#define RTE_CREATED_LOCAL		3


///////////////////////////////////////////////////////
struct IPRoute
{
	DWORD IPadr;
	BYTE  MACadr[ETH_ADRLEN];
	BYTE  State;
};

struct IPRoute HostRoute[MAX_HOST_ROUTES];

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
void InitHostRouteTable()
{
int temp;

	for( temp = 0; temp < MAX_HOST_ROUTES; temp++ )
		HostRoute[temp].State = RTE_UNDEFINED;
}

BYTE gBroadcastMAC[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
BYTE* QueryHostRoute( DWORD IPadr )
{

int temp;

	if( IPadr == 0xFFFFFFFF )	//Global Broadcast IP Address
		return gBroadcastMAC;
	else if( IPadr == (SysIPAddr.Val | ~SysIPMask.Val) )
		return gBroadcastMAC;
	else
	{	
		if( ((SysIPAddr.Val ^ IPadr) & SysIPMask.Val) )
		{
			//the given ip is on different network. use gateway address
			
			IPadr = SysIPGateway.Val;
		}
	

		for( temp = 0; temp < MAX_HOST_ROUTES; temp++ )
		{
			if( HostRoute[temp].State > RTE_WAIT_ARP_REPLY )
			{ 
				if( HostRoute[temp].IPadr == IPadr )
					return HostRoute[temp].MACadr;
			}
		}
	}

	return NULL;
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
int Rte_AddReplyRecord( DWORD IPadr )
{
int temp;

	for( temp = 0; temp < MAX_HOST_ROUTES; temp++ )
	{
		if( HostRoute[temp].State > RTE_UNDEFINED && HostRoute[temp].IPadr == IPadr )
			return ROUTE_REC_EXISITS; //already present
	}
	
	//create a new entry
	for( temp = 0; temp < MAX_HOST_ROUTES; temp++ )
	{
		if( HostRoute[temp].State == RTE_UNDEFINED )
		{
			HostRoute[temp].State = RTE_WAIT_ARP_REPLY;
			HostRoute[temp].IPadr = IPadr;
			return ROUTE_REC_ADDED;
		}
	}
	
	return ROUTE_NO_MEM;
}


////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
void AddHostRoute( DWORD IPadr, BYTE* pMACadr, BOOL CreatedByRemoteHost )
{

int temp;
BOOL found  = FALSE;

	for( temp = 0; temp < MAX_HOST_ROUTES; temp++ )
	{
		if( HostRoute[temp].State > RTE_UNDEFINED )
		{
			if( HostRoute[temp].IPadr == IPadr )
			{
				memcpy( HostRoute[temp].MACadr, pMACadr, ETH_ADRLEN );
				found = TRUE;

				if( CreatedByRemoteHost )
					HostRoute[temp].State = RTE_CREATED_REMOTE;
				else
					HostRoute[temp].State = RTE_CREATED_LOCAL;

				break;
			}
		}
	}

	if( !found )
	{
		//create a new entry
		for( temp = 0; temp < MAX_HOST_ROUTES; temp++ )
		{
			if( HostRoute[temp].State == RTE_UNDEFINED )
			{
				if( CreatedByRemoteHost )
					HostRoute[temp].State = RTE_CREATED_REMOTE;
				else
					HostRoute[temp].State = RTE_CREATED_LOCAL;

				HostRoute[temp].IPadr = IPadr;
				memcpy( HostRoute[temp].MACadr, pMACadr, ETH_ADRLEN );
				found = TRUE;
				break;
			}
		}
	}

}

