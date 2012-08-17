/********************************************************************
 * FileName:		SNMP.c		
 * Dependencies:
 * Processor:				
 * Hardware:		
 * Assembler:		
 * Linker:		
 * Company:		Microchip Technology, Inc.
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
 * HTTP Server Appilcation
 *
 * Change History:
 * Name				Date            Changes
 * Sean Justice		02/21/06		Initial Version
 ********************************************************************/

#ifndef _SNMP_HEADER_FILE
#define _SNMP_HEADER_FILE

#include "generic.h"
#include "bsd_tcp_ip\tcpip.h"
#include "fat16\fat.h"

#define SNMP_PKT_LEN			1500
#define SNMP_COMMUNITY_MAX_LEN  (8)
#define SNMP_NOTIFY_COMM_LEN	SNMP_COMMUNITY_MAX_LEN
#define SNMP_BIB_FILE_NAME      "SNMP.BIB"
#define OID_MAX_LEN             (15)


#define SNMP_AGENT_PORT			(161)
#define SNMP_NMS_PORT			(162)
#define AGENT_NOTIFY_PORT		(0xfffe)

#define SNMP_PKT_PLACE_HOLDER	(0)
#define SNMP_START_OF_VAR       (0)
#define SNMP_END_OF_VAR         (0xff)
#define SNMP_INDEX_INVALID      (0xff)

typedef BYTE SNMP_ID;
typedef BYTE SNMP_INDEX;

typedef enum
{
    SNMP_GET            = 0xa0,
    SNMP_GET_NEXT       = 0xa1,
    SNMP_GET_RESPONSE   = 0xa2,
    SNMP_SET            = 0xa3,
    SNMP_TRAP           = 0xa4,
    SNMP_ACTION_UNKNOWN = 0

}SNMP_ACTION;

typedef union _SNMP_VAL
{
    DWORD dword;
    WORD  word;
    BYTE  byte;
    BYTE  v[sizeof(DWORD)];
} SNMP_VAL;

#ifdef _WINDOWS
typedef union _IP_ADDR
{
    BYTE        v[4];
    DWORD       Val;
} IP_ADDR;
#endif

/********************************************************************
 * function prototypes
 ********************************************************************/
/*********************************************************************
 * Function:		PUBLIC void SNMPInit(void)
 *
 * PreCondition:    The program needs to have already initialized
 *					TCP/IP stack.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    A socket will be created on the SNMP Port 
 *
 * Overview:        The function creates a UDP socket and binds it
 *					to the SNMP Port to allow the application to 
 *					act as a SNMP Agent
 ********************************************************************/
PUBLIC void SNMPInit(void);
/*********************************************************************
 * Function:		PUBLIC BOOL SNMPTask(void)
 *
 * PreCondition:    SMNPInit() needs to be called before SNMPTask can
 *					effectivity run
 *
 * Input:           None
 *
 * Output:          true if a SNMP UDP packet was successfully processed,
 *					otherwise false
 *
 * Side Effects:    None
 *
 * Overview:        This function will process any incoming SNMP packets.
 *
 * Note:			The packet memory can either be placed on the stack
 *					(default) or allow dymanic memory be allocated to handle
 *					it (_SNMP_USE_DYMANIC_MEMORY must be defined)
 ********************************************************************/
PUBLIC BOOL SNMPTask(void);
/*********************************************************************
 * Function:		PUBLIC void SNMPNotifyPrepare(	IP_ADDR *remoteHost, 
 *													char *community, 
 *													BYTE communityLen, 
 *													SNMP_ID agentIDVar, 
 *													BYTE notificationCode, 
 *													DWORD timestamp )     
 *
 * PreCondition:    None.
 *
 * Input:			remoteHost			- pinter to the IP address of the host
 *					community			- pointer to the community
 *					communityLen		- size of the community 
 *					agentIDVar			- SNMP_ID that willl be use
 *					notificationCode	- 8 bit nofitication code
 *					timestamp			- timer ticks of the start of tx
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function sets up the notify process by storing 
 *					the values into a static structure
 ********************************************************************/
PUBLIC void SNMPNotifyPrepare(IP_ADDR *remoteHost, char *community, BYTE communityLen, SNMP_ID agentIDVar, BYTE notificationCode, DWORD timestamp );
/*********************************************************************
 * Function:		PUBLIC BOOL SNMPIsNotifyReady(IP_ADDR *remoteHost)       
 *
 * PreCondition:    None.
 *
 * Input:			remoteHost			- pinter to the IP address of the host
 *
 * Output:          true if the socket was sucessfully create, else false
 *
 * Side Effects:    None.
 *
 * Overview:        This function creates and binds the notifying SNMP
 *					Agent notify port.
 ********************************************************************/
PUBLIC BOOL SNMPIsNotifyReady(IP_ADDR *remoteHost);
/*********************************************************************
 * Function:		PUBLIC BOOL SNMPNotify(	SNMP_ID var, 
 *											SNMP_VAL val, 
 *											SNMP_INDEX index)
 *
 * PreCondition:    None.
 *
 * Input:			var		- enumeration of the SNMP id
 *					val		- union of possible SNMP values
 *					index	- enumeration of the SNMP index
 *
 * Output:          true if notfiy message was sent, else false
 *
 * Side Effects:    None
 *	
 * Overview:		This function sends out the SNMP nofity message.
 *					The tx packet which may be allocated on the stack or 
 *					in dymanic RAM (user needs to define _SNMP_USE_DYMANIC_MEMORY)
 *					The packet will be transmitted and socket will closed.
 ********************************************************************/
PUBLIC BOOL SNMPNotify(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index);

#endif
