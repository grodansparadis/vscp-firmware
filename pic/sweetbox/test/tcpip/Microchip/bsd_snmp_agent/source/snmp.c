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


#define SNMP_CHECK_BLOCKING
#include <stdlib.h>
#include "bsd_snmp_agent\snmp.h"
#include "snmp_private.h"
#include "snmpex.h"
/*******************************************************************
 * Constant data
 *******************************************************************/
const DATA_TYPE_INFO _dataTypeTable[] =
{
    /* INT8_VAL         */ { ASN_INT,           1       },
    /* INT16_VAL        */ { ASN_INT,           2       },
    /* INT32_VAL        */ { ASN_INT,           4       },
    /* BYTE_ARRAY       */ { OCTET_STRING,      0xff    },
    /* ASCII_ARRAY      */ { OCTET_STRING,      0xff    },
    /* IP_ADDRESS       */ { SNMP_IP_ADDR,      4       },
    /* COUNTER32        */ { SNMP_COUNTER32,    4       },
    /* TIME_TICKS_VAL   */ { SNMP_TIME_TICKS,   4       },
    /* GAUTE32          */ { SNMP_GAUGE32,      4       },
    /* OID_VAL          */ { ASN_OID,           0xff    }
};
#define DATA_TYPE_TABLE_SIZE    (sizeof(dataTypeTable)/sizeof(dataTypeTable[0]))

const BYTE _txPDUType[] = 
{
	GET_RESPONSE,
	0x82,
	0,
	0,
	ASN_INT,
	4
};
#define PDU_LEN_OFFSET			(8 + 1 + 2)		// tx header size + community length + structure length
const BYTE _txHeader[] = 
{
	STRUCTURE,
	0x82,
	0,
	0,
	ASN_INT,
	1,
	SNMP_V1,
	OCTET_STRING,
};
#define PACKET_STRUCT_LEN_OFFSET	2
const BYTE _errorStatus[] =
{
	ASN_INT,
	1,
	0,
	ASN_INT,
	1,
	0,
	STRUCTURE,
	0x82,
	0,
	0
};
#define ERROR_STATUS_OFFSET		(8 + 1 + 6 + 4 + 2)		// tx hdr + communtiy length + pdu type + resquest + 2 bytes
#define ERROR_INDEX_OFFSET		(ERROR_STATUS_OFFSET + 1 + 2)
#define VAR_STRUCT_LEN_OFFSET	(ERROR_INDEX_OFFSET + 1 + 2)
const BYTE _txStructHdr[] =
{
	STRUCTURE,
	0x82,
	0,
	0,
	ASN_OID
};

/********************************************************************
 * Private Variables
 ********************************************************************/
PRIVATE SOCKET				_snmpSocket;
PRIVATE WORD				_rxIndex;
PRIVATE WORD				_txIndex;
PRIVATE SNMP_NOTIFY_INFO	_snmpNotifyInfo;

WORD    _DebugCntr;
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
PUBLIC void SNMPInit(void)
{
	SOCKADDR_IN		saServer;
	int				result;

	_snmpNotifyInfo.socket = INVALID_SOCKET;

	_snmpSocket = socket(	AF_INET,		// internetwork: UDP, TCP, etc
							SOCK_DGRAM,		// connectionless
							IPPROTO_UDP);	// user datagram protocol


	if(_snmpSocket == INVALID_SOCKET)
		return;

#ifdef _WINDOWS	
	saServer.sin_port			= htons(SNMP_AGENT_PORT);
#else
	saServer.sin_port			= (SNMP_AGENT_PORT);
#endif
	saServer.sin_family			= AF_INET;
	saServer.sin_addr.s_addr	= INADDR_ANY;

	result = bind(	_snmpSocket,
					(LPSOCKADDR)&saServer,
					sizeof(struct sockaddr));

	if(result == SOCKET_ERROR)
	{
		closesocket(_snmpSocket);
		_snmpSocket = INVALID_SOCKET;
	}

    _DebugCntr = 0;

}
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
PUBLIC BOOL SNMPTask(void)
{
#ifndef _SNMP_USE_DYMANIC_MEMORY
	BYTE			packet[SNMP_PKT_LEN];
#else
	BYTE			*packet;
#endif
	int				result;
	int				packet_len;
	SOCKADDR_IN		saServer;
	int				len;
	SNMP_ACTION		pdu;
	BYTE			community[SNMP_COMMUNITY_MAX_LEN + 1];
	BYTE			comm_len;
    DWORD_VAL		request_id;
	FSFILE			*file;

	
	if(_snmpSocket == INVALID_SOCKET)
		return FALSE;

	SNMP_CHECK_BLOCKING;

#ifdef _SNMP_USE_DYMANIC_MEMORY
	packet = malloc(SNMP_PKT_LEN);

	if(!packet)
		return FALSE;
#endif

	len = sizeof(SOCKADDR_IN);

	result = recvfrom(	_snmpSocket,
						packet,
						SNMP_PKT_LEN,
						0,
						(LPSOCKADDR)&saServer,
						&len);



	if(result < 1)
	{
#ifdef _SNMP_USE_DYMANIC_MEMORY
		free(packet);
#endif
		return FALSE;
	}


    _DebugCntr++;


	_rxIndex	= 0;
	_txIndex	= 0;
	
	pdu = _ProcessHeader(packet, community, &comm_len);		// validate the header
	
	if(pdu == SNMP_ACTION_UNKNOWN)
	{
#ifdef _SNMP_USE_DYMANIC_MEMORY
		free(packet);
#endif
		return FALSE;
	}


	if(!_ProcessGetSetHeader(packet, &request_id.Val))
	{
#ifdef _SNMP_USE_DYMANIC_MEMORY
		free(packet);
#endif
		return FALSE;
	}


	file = FSfopen(SNMP_BIB_FILE_NAME, "rb");
	
	if(!file)
		return FALSE;

	_ProcessMsgVaribles(	packet,
							&saServer,
							len,
							file,
							community,
							comm_len,
							&request_id,
							pdu);


										
	if(file)
		FSfclose(file);

#ifdef _SNMP_USE_DYMANIC_MEMORY
	free(packet);
#endif
	return TRUE;		
}
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
PUBLIC void SNMPNotifyPrepare(IP_ADDR *remoteHost, char *community, BYTE communityLen, SNMP_ID agentIDVar, BYTE notificationCode, DWORD timestamp )
{
    strcpy(_snmpNotifyInfo.community, community);

    _snmpNotifyInfo.communityLen		= communityLen;
    _snmpNotifyInfo.agentIDVar			= agentIDVar;
    _snmpNotifyInfo.notificationCode	= notificationCode;
    _snmpNotifyInfo.timestamp.Val		= timestamp;
	_snmpNotifyInfo.remoteHost.Val		= remoteHost->Val;

}
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
PUBLIC BOOL SNMPIsNotifyReady(IP_ADDR *remoteHost)
{
	SOCKADDR_IN		saServer;
	int				result;

	_snmpNotifyInfo.socket = socket(	AF_INET,		// internetwork: UDP, TCP, etc
										SOCK_DGRAM,		// connectionless
										IPPROTO_UDP);	// user datagram protocol


	if(_snmpNotifyInfo.socket == INVALID_SOCKET)
		return FALSE;

#ifdef _WINDOWS	
	saServer.sin_port			= htons(AGENT_NOTIFY_PORT);
#else
	saServer.sin_port			= (AGENT_NOTIFY_PORT);
#endif
	saServer.sin_family			= AF_INET;
	saServer.sin_addr.s_addr	= remoteHost->Val;

	result = bind(	_snmpNotifyInfo.socket,
					(LPSOCKADDR)&saServer,
					sizeof(struct sockaddr));

	if(result == SOCKET_ERROR)
	{
		closesocket(_snmpNotifyInfo.socket);
		_snmpNotifyInfo.socket = INVALID_SOCKET;
		return FALSE;
	}

	return TRUE;
}
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
PUBLIC BOOL SNMPNotify(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index)
{
#ifndef _SNMP_USE_DYMANIC_MEMORY
	BYTE			tx_pkt[SNMP_PKT_LEN];
#else				
	BYTE			*tx_pkt;
#endif				
	FSFILE			*file;
	WORD			pkt_len_offset;
	WORD			pdu_len_offset;
    WORD			bind_struct_offset;
    WORD			pair_struct_offset;
	OID_INFO		node;
    BYTE			oid_string[OID_MAX_LEN];
    BYTE			oid_len;
    BYTE			agent_id_len;
	DATA_TYPE_INFO	data_type_info;

	if(_snmpNotifyInfo.socket == INVALID_SOCKET)
		return FALSE;

	file = FSfopen(SNMP_BIB_FILE_NAME, "rb");

	if(!file)
	{	closesocket(_snmpNotifyInfo.socket);
		_snmpNotifyInfo.socket = INVALID_SOCKET;
		return FALSE;
	}

#ifdef _SNMP_USE_DYMANIC_MEMORY
	tx_pkt = malloc(SMNP_PKT_LEN);

	if(!tx_pkt)
	{
		closesocket(_snmpNotifyInfo.socket);
		FSfclose(file);
		_snmpNotifyInfo.socket = INVALID_SOCKET;
		return FALSE;
	}
#endif

	_txIndex	= 0;

	_PutByte(tx_pkt, STRUCTURE);
	pkt_len_offset = _GetTxPosition();
	_PutByte(tx_pkt, 0);
	_PutByte(tx_pkt, ASN_INT);						// Int type.
    _PutByte(tx_pkt, 1);							// One byte long value.
    _PutByte(tx_pkt, SNMP_V1);						// v1.0.

    _PutByte(tx_pkt, OCTET_STRING);					// Octet string type.
    _PutByte(tx_pkt, _snmpNotifyInfo.communityLen);  // community string length

	_PutBuffer(tx_pkt, _snmpNotifyInfo.community,	_snmpNotifyInfo.communityLen);

    _PutByte(tx_pkt, TRAP);
	pdu_len_offset = _GetTxPosition();
    _PutByte(tx_pkt, 0);

    // Get complete OID string from MPFS.
    if (!_GetOIDStringByID(file, _snmpNotifyInfo.agentIDVar, &node, oid_string, &agent_id_len) )
    {
		closesocket(_snmpNotifyInfo.socket);
		FSfclose(file);
		_snmpNotifyInfo.socket = INVALID_SOCKET;
#ifdef _SNMP_USE_DYMANIC_MEMORY
		free(tx_pkt);
#endif
        return FALSE;
    }
	
    if (!node.mib_info.flags.bIsAgentID )
    {
		closesocket(_snmpNotifyInfo.socket);
		FSfclose(file);
		_snmpNotifyInfo.socket = INVALID_SOCKET;
#ifdef _SNMP_USE_DYMANIC_MEMORY
		free(tx_pkt);
#endif
        return FALSE;
    }

	if(FSfseek(file, node.data_pos, SEEK_SET))
    {
		closesocket(_snmpNotifyInfo.socket);
		FSfclose(file);
		_snmpNotifyInfo.socket = INVALID_SOCKET;
#ifdef _SNMP_USE_DYMANIC_MEMORY
		free(tx_pkt);
#endif
        return FALSE;
    }

    _PutByte(tx_pkt, ASN_OID);


	if(!FSfread(&agent_id_len, 1, 1, file))
	{
		closesocket(_snmpNotifyInfo.socket);
		FSfclose(file);
		_snmpNotifyInfo.socket = INVALID_SOCKET;
#ifdef _SNMP_USE_DYMANIC_MEMORY
		free(tx_pkt);
#endif
		return FALSE;
	}

    _PutByte(tx_pkt, agent_id_len);
    {
		BYTE	buf[256];
        int     result;

        result = FSfread(&buf, 1, agent_id_len, file);

        if((result <= 0) || ((BYTE)result < agent_id_len))
		{
			closesocket(_snmpNotifyInfo.socket);
			FSfclose(file);
			_snmpNotifyInfo.socket = INVALID_SOCKET;
#ifdef _SNMP_USE_DYMANIC_MEMORY
			free(tx_pkt);
#endif
			return FALSE;
		}

		_PutBuffer(tx_pkt, buf,	agent_id_len);
	}
	
    // This agent's IP address.
    _PutByte(tx_pkt, SNMP_IP_ADDR);
    _PutByte(tx_pkt, 4);
#ifndef _WINDOWS    
	_PutByte(tx_pkt, SysIPAddr.v[3]);
    _PutByte(tx_pkt, SysIPAddr.v[2]);
    _PutByte(tx_pkt, SysIPAddr.v[1]);
    _PutByte(tx_pkt, SysIPAddr.v[0]);
#else
	_PutByte(tx_pkt, MY_IP_BYTE1);
    _PutByte(tx_pkt, MY_IP_BYTE2);
    _PutByte(tx_pkt, MY_IP_BYTE3);
    _PutByte(tx_pkt, MY_IP_BYTE4);
#endif

    // Trap code
    _PutByte(tx_pkt, ASN_INT);
    _PutByte(tx_pkt, 1);
    _PutByte(tx_pkt, 6);					// Enterprisespecific trap code

    _PutByte(tx_pkt, ASN_INT);
    _PutByte(tx_pkt, 1);
    _PutByte(tx_pkt, _snmpNotifyInfo.notificationCode);

    // Time stamp
    _PutByte(tx_pkt, SNMP_TIME_TICKS);
    _PutByte(tx_pkt, 4);
    _PutByte(tx_pkt, _snmpNotifyInfo.timestamp.v[3]);
    _PutByte(tx_pkt, _snmpNotifyInfo.timestamp.v[2]);
    _PutByte(tx_pkt, _snmpNotifyInfo.timestamp.v[1]);
    _PutByte(tx_pkt, _snmpNotifyInfo.timestamp.v[0]);

    // Variable binding structure header
    _PutByte(tx_pkt, 0x30);
    bind_struct_offset = _GetTxPosition();
    _PutByte(tx_pkt, 0);

    // Create variable name-pair structure
    _PutByte(tx_pkt, 0x30);
    pair_struct_offset = _GetTxPosition();
    _PutByte(tx_pkt, 0);

    if (!_GetOIDStringByID(file, var, &node, oid_string, &oid_len) )
    {
		closesocket(_snmpNotifyInfo.socket);
		FSfclose(file);
		_snmpNotifyInfo.socket = INVALID_SOCKET;
#ifdef _SNMP_USE_DYMANIC_MEMORY
		free(tx_pkt);
#endif
        return FALSE;
    }

    // Copy OID string into packet.
    _PutByte(tx_pkt, ASN_OID);
    _PutByte(tx_pkt, (BYTE)(oid_len + 1));
	_PutBuffer(tx_pkt, oid_string,	oid_len);
    _PutByte(tx_pkt, index);

    // Encode and Copy actual data bytes
    if (!_GetDataTypeInfo(node.data_type, &data_type_info) )
    {
		closesocket(_snmpNotifyInfo.socket);
		FSfclose(file);
		_snmpNotifyInfo.socket = INVALID_SOCKET;
#ifdef _SNMP_USE_DYMANIC_MEMORY
		free(tx_pkt);
#endif
        return FALSE;
    }
	
    _PutByte(tx_pkt, data_type_info.asnType);
    // In this version, only data type of 4 bytes or less long can be
    // notification variable.
    if (data_type_info.asnLen == 0xff )
    {
		closesocket(_snmpNotifyInfo.socket);
		FSfclose(file);
		_snmpNotifyInfo.socket = INVALID_SOCKET;
#ifdef _SNMP_USE_DYMANIC_MEMORY
		free(tx_pkt);
#endif
        return FALSE;
    }

    {
		BYTE		len;
		WORD		prev_offset;
		SOCKADDR_IN	saServer;

		len = data_type_info.asnLen;
		_PutByte(tx_pkt, len);
		
		while( len-- )
			_PutByte(tx_pkt, val.v[len]);

		len = data_type_info.asnLen + oid_len + 5;

		prev_offset = _GetTxPosition();
		_SetTxPosition(pair_struct_offset);
		_PutByte(tx_pkt, len);

		len += 2;
		_SetTxPosition(bind_struct_offset);
		_PutByte(tx_pkt, len);

		len += (agent_id_len + 22);
		_SetTxPosition(pdu_len_offset);
		_PutByte(tx_pkt, len);

		len += (_snmpNotifyInfo.communityLen + 7);
		_SetTxPosition(pkt_len_offset);
		_PutByte(tx_pkt, len);
	
#ifdef _WINDOWS	
		saServer.sin_port			= htons(AGENT_NOTIFY_PORT);
#else
		saServer.sin_port			= (AGENT_NOTIFY_PORT);
#endif
		saServer.sin_family			= AF_INET;
		saServer.sin_addr.s_addr	= _snmpNotifyInfo.remoteHost.Val;
		
		sendto(	_snmpNotifyInfo.socket,
				tx_pkt,
				prev_offset,
				0,
				(LPSOCKADDR)&saServer,
				sizeof(struct sockaddr));
	}


	closesocket(_snmpNotifyInfo.socket);
	FSfclose(file);
	_snmpNotifyInfo.socket = INVALID_SOCKET;
#ifdef _SNMP_USE_DYMANIC_MEMORY
	free(tx_pkt);
#endif
	return TRUE;
}
/*********************************************************************
 * Function:		PRIVATE int _PacketReady(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          the result received from the recvfrom function
 *
 * Side Effects:    None
 *
 * Overview:        This function uses the peek flag to find out if there
 *					is a appending packet and if so, how big it is.
 ********************************************************************/
PRIVATE int _PacketReady(void)
{
	BYTE			temp;
	int				result;
	SOCKADDR_IN		saServer;
	int				len;

    len = sizeof(SOCKADDR_IN);

	result = recvfrom(	_snmpSocket,
						&temp,
						1,
						MSG_PEEK,
						(LPSOCKADDR)&saServer,
						&len);

	return result;
}
/*********************************************************************
 * Function:		PRIVATE SNMP_ACTION _ProcessHeader(	BYTE *pkt, 
 *														BYTE *community, 
 *														BYTE *commumnitylen)       
 *
 * PreCondition:    None.
 *
 * Input:           pkt				- pointer to the rx packet
 *					community		- pointer to put the community string
 *					communityLen	- pointer to size of the community
 *
 * Output:          The action that should be taken
 *
 * Side Effects:    None
 *
 * Overview:        This function start to decode the incoming SNMP 
 *					packet.  It checks the structure and gets the PDU.
 *					If there is a valid packet, the process continues.
 ********************************************************************/
PRIVATE SNMP_ACTION _ProcessHeader(BYTE *pkt, BYTE *community, BYTE *commumnitylen)
{
	SNMP_ACTION	pdu;
	DWORD_VAL	data;
	
	if(!_CheckForStructure(pkt, &data.w[0]))
		return SNMP_ACTION_UNKNOWN;

	if(!_GetIntData(pkt, &data.Val))
		return SNMP_ACTION_UNKNOWN;

    if (data.v[0] != SNMP_V1 )
        return SNMP_ACTION_UNKNOWN;

	if(!_GetCommunity(pkt, community, commumnitylen))
        return SNMP_ACTION_UNKNOWN;

	if(!_GetPDU(pkt, &pdu))
        return SNMP_ACTION_UNKNOWN;

    // callback to the main application
	if (!SNMPValidate(pdu, community))
        return SNMP_ACTION_UNKNOWN;

	return pdu;
}
/*********************************************************************
 * Function:		PRIVATE BYTE _ProcessGetSetHeader(	BYTE *pkt, 
 *														DWORD *request)       
 *
 * PreCondition:    None
 *
 * Input:           pkt			- Rx Packet 
 *					request		- pointer to the SNMP manger's request
 *
 * Output:          number of bytes, else 0
 *
 * Side Effects:    None
 *
 * Overview:        This function get the get/set header of the SNMP
 *					manager.
 ********************************************************************/
PRIVATE BYTE _ProcessGetSetHeader(BYTE *pkt, DWORD *request)
{
	DWORD_VAL	temp;

	if(!_GetIntData(pkt, request))
		return 0;

	if(!_GetIntData(pkt, &temp.Val))
		return 0;

	return _GetIntData(pkt, &temp.Val);
}
/*********************************************************************
 * Function:		PRIVATE WORD _ProcessMsgVaribles(	BYTE *pkt, 
 *														SOCKADDR_IN *sock_addr, 
 *														int sock_len, 
 *														FSFILE *file, 
 *														BYTE *community, 
 *														BYTE comm_len, 
 *														DWORD_VAL *request, 
 *														BYTE pdu_type)       
 *
 * PreCondition:    None
 *
 * Input:           pkt			- Rx packet
 *					sock_addr	- pointer to the socket address of the Tx packet (sento function)
 *					sock_len	- size of the data in sock_addr (sento function)
 *					file		- pointer to the FSFILE structure
 *					community	- pointer to the SNMP community
 *					comm_len	- size of the community
 *					request		- pointer to the SNMP request
 *					pdu_type	- the pdu type from SNMP manager
 *
 * Output:          true if the packet was successfully sent, else false 
 *
 * Side Effects:    None
 *
 * Overview:        This function processes and from the responce packet of the
 *					SNMP.
 ********************************************************************/
PRIVATE BOOL _ProcessMsgVaribles(BYTE *pkt, SOCKADDR_IN *sock_addr, int sock_len, FSFILE *file, BYTE *community, BYTE comm_len, DWORD_VAL *request, BYTE pdu_type)
{
#ifndef _SNMP_USE_DYMANIC_MEMORY
	BYTE			tx_pkt[SNMP_PKT_LEN];
#else
	BYTE			*tx_pkt;
#endif
	WORD_VAL		var_binding_len;
	WORD_VAL		var_bind_len;
	WORD_VAL		var_pair_len;
	BYTE			var_idx;
    SNMP_ERR_STATUS err_status;
	WORD			err_status_offset;
	WORD			err_index_offset;
	
	if(!_CheckForStructure(pkt, &var_binding_len.Val))
		return FALSE;

#ifdef _SNMP_USE_DYMANIC_MEMORY
	tx_pkt = malloc(SNMP_PKT_LEN);

	if(!tx_pkt)
		return FALSE;
#endif

    memset(tx_pkt, 0, SNMP_PKT_LEN);
    
	// prepare the header
	_PutBuffer(tx_pkt, _txHeader, sizeof(_txHeader));

	_PutByte(tx_pkt, comm_len);

	_PutBuffer(tx_pkt, community, (WORD)comm_len);


	_PutBuffer(tx_pkt, _txPDUType, sizeof(_txPDUType));
	
	{
		BYTE	temp_buf[4];

		temp_buf[0] = request->v[3];
		temp_buf[1] = request->v[2];
		temp_buf[2] = request->v[1];
		temp_buf[3] = request->v[0];

		_PutBuffer(tx_pkt, temp_buf, 4);
	}

	// put in the error status
	_PutBuffer(tx_pkt, _errorStatus, sizeof(_errorStatus));


	err_status_offset	= ERROR_STATUS_OFFSET + comm_len;
	err_index_offset	= ERROR_INDEX_OFFSET + comm_len;

	// init the index and error status
	var_idx		= 0;
	err_status	= SNMP_NO_ERR;

	var_bind_len.Val = 0;

	while(var_binding_len.Val)
	{
		BYTE		temp;
		WORD_VAL	len;
		WORD		var_struct_offset;
		WORD		oid_offset;
	    OID_INFO	oid_info;
		OID_DATA	oid;
		WORD		temp_offset;

		var_idx++;				// indicate what variable we are processing
		
		temp = _CheckForStructure(pkt, &len.Val);

		if(!temp)
		{
#ifdef _SNMP_USE_DYMANIC_MEMORY
			free(tx_pkt);
#endif		
			return FALSE;
		}

		var_binding_len.Val -= len.Val;
		var_binding_len.Val -= (WORD)temp;

		
		// decode the object
		if(!_GetOID(pkt, oid.data, &oid.length))
		{
#ifdef _SNMP_USE_DYMANIC_MEMORY
			free(tx_pkt);
#endif		
			return FALSE;
		}


		// if we are GET or GET-NEXT the value must be NULL
		if(pdu_type != (BYTE)SET_REQUEST)
		{
			if(!_ChkASNNull(pkt))
			{
#ifdef _SNMP_USE_DYMANIC_MEMORY
				free(tx_pkt);
#endif		
				return FALSE;
			}
		}

		// prepare the responce structure
		_PutBuffer(tx_pkt, _txStructHdr, sizeof(_txStructHdr));

		var_struct_offset = _GetTxPosition(); 
		var_struct_offset -= 3;
		oid_offset = _GetTxPosition();
		
		_PutByte(tx_pkt, oid.length);

		_PutBuffer(tx_pkt, oid.data, (WORD)oid.length);
		
		// look up the "oid" in other database
		if(!_OIDLookUp(file, oid.data, oid.length, &oid_info))
		{
			err_status = SNMP_NO_SUCH_NAME;

			_SetErrorStatus(tx_pkt,
							err_status_offset,
							err_index_offset,
							SNMP_NO_SUCH_NAME,
							var_idx);

			// oid name, header and value
			var_pair_len.Val = oid.length + 4;

			if(pdu_type != SNMP_SET)
			{
				_PutByte(tx_pkt, ASN_NULL);
				_PutByte(tx_pkt, 0);

			}else
			{
				_PutByte(tx_pkt, _GetByte(pkt));
				
				temp = _GetByte(pkt);
				_PutByte(tx_pkt, temp);

				while(temp)
				{
					temp--;
					var_pair_len.Val++;
					_PutByte(tx_pkt, _GetByte(pkt));
				}

			}
			
		}else
		{

			switch(pdu_type)
			{
			case SNMP_GET:
				var_pair_len.Val = oid.length + 2;

				temp_offset = _GetTxPosition();

				temp = _ProcessGetVariable(tx_pkt, file, &oid_info, FALSE);

				if(!temp)
				{
					_SetTxPosition(temp_offset);

					err_status = SNMP_NO_SUCH_NAME;

					_SetErrorStatus(tx_pkt,
									err_status_offset,
									err_index_offset,
									SNMP_NO_SUCH_NAME,
									var_idx);

					_PutByte(tx_pkt, ASN_NULL);
					_PutByte(tx_pkt, SNMP_PKT_PLACE_HOLDER);
					temp = 2;
				}

				var_pair_len.Val += temp;
				break;

			case SNMP_GET_NEXT:
				temp_offset = _GetTxPosition();
				_SetTxPosition(oid_offset);

				temp = _ProcessGetNextVariable(tx_pkt, file, &oid_info);

				if(!temp)
				{
					_SetTxPosition(temp_offset);

					_SetErrorStatus(tx_pkt,
									err_status_offset,
									err_index_offset,
									SNMP_NO_SUCH_NAME,
									var_idx);

					_PutByte(tx_pkt, ASN_NULL);
					_PutByte(tx_pkt, SNMP_PKT_PLACE_HOLDER);

					var_pair_len.Val = oid.length + 4;
				}else
				{
					var_pair_len.Val = temp + 2;
				}
				break;

			case SNMP_SET:
				temp = _ProcessSetVariable(tx_pkt, pkt, file, &oid_info, &err_status);

				if(err_status != SNMP_NO_ERR)
				{
					_SetErrorStatus(tx_pkt,
									err_status_offset,
									err_index_offset,
									SNMP_NO_SUCH_NAME,
									var_idx);

				}

				var_pair_len.Val	= oid.length + 2 + temp;
				break;
			}

		}

		temp_offset = _GetTxPosition();
		_SetTxPosition(var_struct_offset);
		_PutByte(tx_pkt, var_pair_len.v[1]);
		_PutByte(tx_pkt, var_pair_len.v[0]);
		_SetTxPosition(temp_offset);

		var_bind_len.Val	+= 4 + var_pair_len.Val;

	}

	err_status_offset = _GetTxPosition();

	err_index_offset = VAR_STRUCT_LEN_OFFSET + comm_len;
	_SetTxPosition(err_index_offset);
	_PutByte(tx_pkt, var_bind_len.v[1]);
	_PutByte(tx_pkt, var_bind_len.v[0]);

	err_index_offset = PDU_LEN_OFFSET + comm_len;
	var_bind_len.Val += 16;
	_SetTxPosition(err_index_offset);
	_PutByte(tx_pkt, var_bind_len.v[1]);
	_PutByte(tx_pkt, var_bind_len.v[0]);

	var_bind_len.Val += (9 + comm_len);
	_SetTxPosition(PACKET_STRUCT_LEN_OFFSET);
	_PutByte(tx_pkt, var_bind_len.v[1]);
	_PutByte(tx_pkt, var_bind_len.v[0]);


	sendto(	_snmpSocket,
			tx_pkt,
			err_status_offset,
			0,
			(LPSOCKADDR)sock_addr,
			sock_len);

	return TRUE;
	
}
/*********************************************************************
 * Function:       PRIVATE BOOL _OIDLookUp(	FSFILE *file, 
 *											BYTE *oid, 
 *											BYTE oid_len, 
 *											OID_INFO *oid_info)
 *
 * PreCondition:    None
 *
 * Input:           file		- pointer to the binary tree file
 *					oid			- pointer to the object id
 *					oid_len		- size of the object id
 *					oid_info	- object id tree informaiton
 *
 * Output:          true if object id was found, else false
 *
 * Side Effects:    None
 *
 * Overview:        This function looks through the binary file that
 *					contains the object ids that this SNMP agent supports.
 ********************************************************************/
PRIVATE BOOL _OIDLookUp(FSFILE *file, BYTE *oid, BYTE oid_len, OID_INFO *oid_info)
{
	DWORD	curr_pos;
	BYTE	match_cnt;
	BOOL	found = FALSE;
	BYTE	temp;

	if(!file)
		return FALSE;

	curr_pos	= 0;
	match_cnt	= oid_len;

	while(1)								// we got some work here
	{

		DWORD	new_pos;

		new_pos = _ReadMIBRecord(file, curr_pos, oid_info);

		// use the common function to read a record from the file
		if(!new_pos)
			return FALSE;

		temp = oid_info->oid;

		if(temp != *oid)
		{
			/*
			 * The oid might not match because it is
			 * 0 - 2.  In that case declare that there
			 * is a match.  The command processor would
			 * detect OID type and continue or reject
			 * this OID as a valid argument
			 */

			if(match_cnt == oid_len)
			{
				found = TRUE;
				break;
			}

			if(oid_info->mib_info.flags.bIsSibling)
			{
				curr_pos = oid_info->sibling_pos;
				continue;							// start over
			}else
			{
				break;							// didn't find it????
			}

		}else
		{
			// next
			match_cnt--;
			oid++;

            /*
			 * A node is said to be matched if last matched 
			 * node is a leaf node or all but last OID string 
			 * is matched and last byte of OID is '0'.
			 * i.e. single index.
			 */
			if(!oid_info->mib_info.flags.bIsParent)
			{
				found = TRUE;
				break;

			}else if((match_cnt == 1 && *oid == 0) || (!match_cnt))
			{
				found = TRUE;
				break;

			}else
			{
				curr_pos = new_pos;
			}

		}

	}

	if(found)
	{
		temp = *oid;

		oid_info->index = temp;

		if(!match_cnt)
		{
			oid_info->index		= SNMP_INDEX_INVALID;
			oid_info->index_len	= 0;

		}else if (match_cnt > 1 || temp & 0x80)
		{
			oid_info->index_len	= 0xFF;
			return FALSE;
		}else
		{
			oid_info->index_len	= 1;
		}
	}

	return found;

}
/*********************************************************************
 * Function:		PRIVATE BYTE _ProcessGetVariable(	BYTE *tx_pkt, 
 *														FSFILE *file, 
 *														OID_INFO *oid_info, 
 *														BOOL as_oid)       
 *
 * PreCondition:    None
 *
 * Input:           tx_pkt		- pointer to the tx packet
 *					file		- pointer to the file stream 
 *					oid_info	- pointer to the leaf data
 *					as_oid		- treat as a object id
 *
 * Output:          Number of bytes in the tx packet
 *
 * Side Effects:    None
 *
 * Overview:        This function processes the get request from the
 *					SNMP manager based on the object id.
 ********************************************************************/
PRIVATE BYTE _ProcessGetVariable(BYTE *tx_pkt, FSFILE *file, OID_INFO *oid_info, BOOL as_oid)
{
	DATA_TYPE_INFO	data_type_info;
	WORD			offset;
	BYTE			var_len;

	if(oid_info->mib_info.flags.bIsParent)
		return 0;

	if(!oid_info->mib_info.flags.bIsSequence)
		if(oid_info->index > 0)
			return 0;

	if(!_GetDataTypeInfo(oid_info->data_type, &data_type_info))
		return 0;

	if(!as_oid)
	{
		_PutByte(tx_pkt, data_type_info.asnType);
		offset = _GetTxPosition();
		_PutByte(tx_pkt, data_type_info.asnLen);
	}

	if(oid_info->mib_info.flags.bIsConstant)
	{
		BYTE	buf[256];
		BYTE	i;

		if(FSfseek(file, oid_info->data_pos, SEEK_SET))
			return 0;

		if(!FSfread(&var_len, 1, 1, file))
			return 0;


		if((BYTE)FSfread(buf, 1, var_len, file) != var_len)
			return 0;

		_PutBuffer(tx_pkt, buf, var_len);

	}else
	{
		BYTE		ref;
		SNMP_VAL	value;

		ref			= SNMP_START_OF_VAR;
		value.dword	= 0;
		var_len		= 0;

		while(ref != SNMP_END_OF_VAR)
		{
			if(SNMPGetVar(oid_info->id, oid_info->index, &ref, &value))
			{
				if(data_type_info.asnLen != 0xFF)
				{
					var_len = data_type_info.asnLen;

					while(data_type_info.asnLen)
					{
						data_type_info.asnLen--;
						_PutByte(tx_pkt, value.v[data_type_info.asnLen]);
					}

					break;
				}else
				{
					var_len++;
					_PutByte(tx_pkt, value.v[0]);
				}

			}else
			{
				return 0;
			}
		}
	}

	if(!as_oid)
	{
		WORD	prev_offset;

		prev_offset = _GetTxPosition();

		_SetTxPosition(offset);

		_PutByte(tx_pkt, var_len);

		_SetTxPosition(prev_offset);

		var_len += 2;

	}

	return var_len;
}
/*********************************************************************
 * Function:       PRIVATE BYTE _ProcessGetNextVariable(	BYTE *tx_pkt, 
 *															FSFILE *file, 
 *															OID_INFO *oid_info)
 *
 * Input:           tx_pkt		- pointer to the tx packet
 *					file		- pointer to the file stream 
 *					oid_info	- pointer to the leaf data
 *
 * Output:          Number of bytes in the tx packet
 *
 * Side Effects:    None
 *
 * Overview:        This function processes the get_next request from the
 *					SNMP manager based on the object id.
 ********************************************************************/
PRIVATE BYTE _ProcessGetNextVariable(BYTE *tx_pkt, FSFILE *file, OID_INFO *oid_info)
{
	BOOL		next_leaf;
	WORD_VAL	temp;
	BYTE		oid_str[51];
    BYTE		oid_str_len;
	WORD		holder_offset;
	WORD		curr_offset;
	BYTE		data_type;
	SNMP_ID		id;
	BYTE		ref;
	SNMP_VAL	value;
	MIB_INFO	mib_info;
	INDEX_INFO	idx_info;
	BYTE		idx_bytes;
	OID_INFO	rec_idx;

	next_leaf		= FALSE;
	temp.v[0]	= 0;

	if(oid_info->mib_info.flags.bIsParent || 
		(!oid_info->mib_info.flags.bIsParent && !oid_info->mib_info.flags.bIsSequence))
	{
_PGNV00:
		next_leaf = TRUE;

		if(!_GetNextLeaf(file, oid_info))
			return 0;
	}

	if(!_GetOIDStringByAddress(file, oid_info, oid_str, &oid_str_len))
		return 0;


	holder_offset	= _GetTxPosition();
	_SetTxPosition(holder_offset + 1);

    _PutBuffer(tx_pkt, oid_str, (WORD)oid_str_len);

	temp.v[0]	    = oid_str_len;
	data_type		= oid_info->data_type;
	id				= oid_info->id;

	if(!oid_info->mib_info.flags.bIsSequence)
	{
		BYTE	put_bytes;

		_PutByte(tx_pkt, 0);
		temp.v[0]++;

		curr_offset = _GetTxPosition();
		_SetTxPosition(holder_offset);
		_PutByte(tx_pkt, ++oid_str_len);
		_SetTxPosition(curr_offset);

		curr_offset = _GetTxPosition();
		put_bytes = _ProcessGetVariable(tx_pkt, file, oid_info, FALSE);

		if(!put_bytes)
		{
			_SetTxPosition(curr_offset);
			_PutByte(tx_pkt, ASN_NULL);
			_PutByte(tx_pkt, 0);
			put_bytes = 2;
		}

		temp.v[0] += put_bytes;

		return (temp.v[0]);
	}
	
	ref = SNMP_START_OF_VAR;

	if(next_leaf)
	{
		if(!SNMPGetVar(id, oid_info->index, &ref, &value))
		{

			_SetTxPosition(holder_offset);
			goto _PGNV00;
		}
	}

	mib_info.val = oid_info->mib_info.val;
	
	if(!FSfread(&idx_bytes, 1, 1, file))
		return 0;

	idx_bytes = 0;

	if(!FSfread(&idx_info.Val, 1, 1, file))
		return 0;

	
	if(!FSfread(&rec_idx.id, 1, 1, file))
		return 0;

	if(!FSfread(&rec_idx.data_type, 1, 1, file))
		return 0;

	rec_idx.index = oid_info->index;

	if(!next_leaf && !SNMPGetNextIndex(rec_idx.id, &rec_idx.index))
	{
		_SetTxPosition(holder_offset);
		goto _PGNV00;
	}

	rec_idx.mib_info.flags.bIsConstant	= 0;
	rec_idx.mib_info.flags.bIsParent	= 0;
	rec_idx.mib_info.flags.bIsSequence	= 1;
	idx_bytes							+= _ProcessGetVariable(tx_pkt, file, &rec_idx, TRUE);
	oid_info->index						= rec_idx.index;
	temp.v[0]						    += idx_bytes;
	oid_str_len							+= idx_bytes;
	
	curr_offset							= _GetTxPosition();
	_SetTxPosition(holder_offset);
	_PutByte(tx_pkt, oid_str_len);
	_SetTxPosition(curr_offset);

	oid_info->mib_info.val	= mib_info.val;
	oid_info->id			= id;
	oid_info->data_type		= data_type;

	temp.v[0]			    += _ProcessGetVariable(tx_pkt, file, oid_info, FALSE);

	return (temp.v[0]);

}
/*********************************************************************
 * Function:		PRIVATE BYTE _ProcessSetVariable(	BYTE *tx_pkt, 
 *														BYTE *rx_pkt, 
 *														FSFILE *file, 
 *														OID_INFO *oid_info, 
 *														SNMP_ERR_STATUS *err_status)
 *
 * PreCondition:    None
 *
 * Input:           tx_pkt		- pointer to the tx packet
 *					rx_pkt		- pointer to the rx packet
 *					file		- pointer to the file stream 
 *					oid_info	- pointer to the leaf data
 *					err_status	- pointer to the error
 *
 * Output:          Number of bytes in the tx packet
 *
 * Side Effects:    None
 *
 * Overview:        This function processes the set request from the
 *					SNMP manager based on the object id and information 
 *					passed.
 ********************************************************************/
PRIVATE BYTE _ProcessSetVariable(BYTE *tx_pkt, BYTE *rx_pkt, FSFILE *file, OID_INFO *oid_info, SNMP_ERR_STATUS *err_status)
{
	SNMP_ERR_STATUS	err_code;
	DATA_TYPE_INFO	actual_data_type;
	BYTE			copy_bytes;
	BYTE			data_type;
	BYTE			data_len;
	BYTE			ref;
	SNMP_VAL		value;
	BYTE			temp;

	err_code	= SNMP_NO_ERR;
	copy_bytes  = 0;

	if(oid_info->mib_info.flags.bIsParent ||
		oid_info->mib_info.flags.bIsConstant ||
		!oid_info->mib_info.flags.bIsEditable)
	{
		err_code = SNMP_NO_SUCH_NAME;
	}

	data_type		= _GetByte(rx_pkt);
	_PutByte(tx_pkt, data_type);
	copy_bytes++;

	if(!_GetDataTypeInfo(oid_info->data_type, &actual_data_type))
		err_code = SNMP_BAD_VALUE;

	if(data_type != actual_data_type.asnType)
		err_code = SNMP_BAD_VALUE;

	data_len = _GetByte(rx_pkt);
	_PutByte(tx_pkt, data_len);
	copy_bytes++;

	if(data_len & 0x80)
		err_code = SNMP_BAD_VALUE;

	if(!oid_info->mib_info.flags.bIsSequence && oid_info->index)
		err_code = SNMP_NO_SUCH_NAME;

	value.dword = 0;
	ref			= SNMP_START_OF_VAR;

	if(actual_data_type.asnLen != 0xFF)
	{
		if(data_len <= 4)
		{
			while(data_len--)
			{
				temp				= _GetByte(rx_pkt);
				value.v[data_len]	= temp;
				_PutByte(tx_pkt, temp);
				copy_bytes++;
			}

			if(err_code == SNMP_NO_ERR)
			{
				if(!SNMPSetVar(oid_info->id, oid_info->index, ref, value))
					err_code = SNMP_BAD_VALUE;
			}
		}else
		{
			err_code = SNMP_BAD_VALUE;
		}
	}else
	{
		if(!SNMPIsValidSetLen(oid_info->id, data_len))
			err_code = SNMP_BAD_VALUE;

		while(data_len--)
		{
			value.byte = _GetByte(rx_pkt);
			_PutByte(tx_pkt, value.byte);
			copy_bytes++;

			if(err_code == SNMP_NO_ERR)
			{
				if(!SNMPSetVar(oid_info->id, oid_info->index, ref++, value))
					err_code = SNMP_BAD_VALUE;

			}
		}
		if(err_code == SNMP_NO_ERR)
			SNMPSetVar(oid_info->id, oid_info->index, SNMP_END_OF_VAR, value);
	}

	*err_status = err_code;

	return copy_bytes;

}
/*********************************************************************
 * Function:		PRIVATE BOOL _GetNextLeaf(	FSFILE *file, 
 *												OID_INFO *node)       
 *
 * PreCondition:    None
 *
 * Input:           file		- pointer to the file stream 
 *					node		- pointer to the leaf data
 *
 * Output:          true if the next leaf was present, else
 *					false
 *
 * Side Effects:    None
 *
 * Overview:        This function gets the next leaf of the tree.
 ********************************************************************/
PRIVATE BOOL _GetNextLeaf(FSFILE *file, OID_INFO *node)
{
	DWORD	curr_pos;

	if(!file)
		return FALSE;

	if(!node->mib_info.flags.bIsParent)
	{
		if(node->mib_info.flags.bIsSibling || node->mib_info.flags.bIsDistantSibling)
		{
			if(FSfseek(file, node->sibling_pos, SEEK_SET))
				return FALSE;

		}else
		{
			return FALSE;
		}
	}

	curr_pos = FSftell(file);

	while(1)
	{
		curr_pos = _ReadMIBRecord(file, curr_pos, node);

		if(!curr_pos)
			break;
		
		if(node->mib_info.flags.bIsParent)
			continue;

		node->index_len = 1;
		node->index		= 0;

		return TRUE;

	}

	return FALSE;
}
/*********************************************************************
 * Function:       PRIVATE BOOL _GetOIDStringByID(	FSFILE *file, 
 *													SNMP_ID id, 
 *													OID_INFO *node, 
 *													BYTE *string, 
 *													BYTE *len)
 *
 * PreCondition:    None
 *
 * Input:           file		- pointer to the file stream
 *					id			- SNMP id
 *					node		- pointer to the leaf data
 *					string		- object id string
 *					len			- size of the object id string
 *
 * Output:          true if found, else false
 *
 * Side Effects:    none
 *
 * Overview:        This function get the object id string based on the
 *					SNMP id.
 ********************************************************************/
PRIVATE BOOL _GetOIDStringByID(FSFILE *file, SNMP_ID id, OID_INFO *node, BYTE *string, BYTE *len)
{
	DWORD	curr_pos;

	while(1)
	{
		curr_pos = _ReadMIBRecord(file, curr_pos, node);

		if(!curr_pos)
            break;
		
		if(!node->mib_info.flags.bIsParent)
		{
			if(node->mib_info.flags.bIsIDPresent)
			{
				if(node->id == id)
					return _GetOIDStringByAddress(file, node, string, len);
			}

			if(node->mib_info.flags.bIsSibling || node->mib_info.flags.bIsDistantSibling)
				curr_pos = node->sibling_pos;
			else
				break;

		}
	}

	return FALSE;
}
/*********************************************************************
 * Function:       PRIVATE BOOL _GetOIDStringByAddress(	FSFILE *file, 
 *														OID_INFO *node, 
 *														BYTE *string, 
 *														BYTE *len)
 *
 * PreCondition:    None
 *
 * Input:           file		- pointer to the file stream
 *					node		- pointer to the leaf data
 *					string		- object id string
 *					len			- size of the object id string
 *
 * Output:          true if found, else false
 *
 * Side Effects:    none
 *
 * Overview:        This function get the object id string based on the
 *					node location.
 ********************************************************************/
PRIVATE BOOL _GetOIDStringByAddress(FSFILE *file, OID_INFO *node, BYTE *string, BYTE *len)
{
	DWORD		curr_pos;
	DWORD		target_pos;
	OID_INFO	curr_node;
	BYTE		idx;
	enum
	{
		SM_PROBE_SIBLING,
		SM_PROBE_CHILD
	}state;

	curr_pos	= 0;
	target_pos	= node->node_pos;
	state		= SM_PROBE_SIBLING;
	idx			= 0;

	while(1)
	{
		if(!_ReadMIBRecord(file, curr_pos, &curr_node))
            break;
		
		string[idx] = curr_node.oid;

		if(target_pos == curr_pos)
		{
			idx++;

			*len = idx;
			return TRUE;
		}

		switch(state)
		{
		case SM_PROBE_SIBLING:
			if(!curr_node.mib_info.flags.bIsSibling)
			{
				state = SM_PROBE_CHILD;
			}else
			{
				DWORD next_pos;

				next_pos = curr_node.sibling_pos;
				
				if(FSfseek(file, next_pos, SEEK_SET))
					return FALSE;

				if(target_pos >= next_pos)
				{
					curr_pos = next_pos;
					break;
				}else
				{
					state = SM_PROBE_CHILD;
				}

			}
		case SM_PROBE_CHILD:
			if(!curr_node.mib_info.flags.bIsParent)
				return FALSE;

			idx++;

			curr_pos = curr_node.child_pos;
			state = SM_PROBE_SIBLING;
			break;
		}

	}
	
	return FALSE;
}
/*********************************************************************
 * Function:		PRIVATE DWORD _ReadMIBRecord(	FSFILE *file, 
 *													DWORD position, 
 *													OID_INFO *node)     
 *
 * PreCondition:    None
 *
 * Input:           file		- binary tree file system
 *					position	- file starting position
 *					node		- pointer to the node to fill
 *
 * Output:          the current position of the file pointer
 *
 * Side Effects:    None
 *
 * Overview:		This function fills the node structure with the 
 *					leaf information.
 *
 * Note:			MIB Binary Record Format 
 *					Always present					<>
 *					Optional						[]
 *					Optional, but occur together	{}
 *					
 *					ID								Size (bytes)
 *					-----------						---------------
 *					<oid>							1
 *					<node info>						1
 *					[id]							1
 *					[sibling offset]				2
 *					[distance sibling offset]		2
 *					{
 *					[data type]						1
 *					[data length]					1
 *					}
 *					[data]							data length
 *					{
 *					[index count]					1
 *					[index node info]				1
 *					[index data type]				1
 *					}
 *			
 *
 ********************************************************************/
PRIVATE DWORD _ReadMIBRecord(FSFILE *file, DWORD position, OID_INFO *node)
{
    MIB_INFO info;

	// goto the start of the BIB node
	if(FSfseek(file, position, SEEK_SET))
		return 0;

	memset(node, 0, sizeof(OID_INFO));

	node->node_pos = position;

	// <oid> and <node info>
	if(!FSfread(&node->oid, 1, 2, file))
		return 0;

	position += 2;

	info.val = node->mib_info.val;

	if(info.flags.bIsIDPresent)
	{
		//[id]
		if(!FSfread(&node->id, 1, 1, file))
			return 0;

		position++;
	}

	if(info.flags.bIsSibling)
	{
		// [sibling offset]
		if(FSfread(&node->sibling_pos, 1, 2, file) < 2)
			return 0;

		position += 2;
	}

	if(info.flags.bIsParent)
	{
		// next position
		node->child_pos = position;

	}else
	{
		if(info.flags.bIsDistantSibling)
		{
			// [distance sibling offset]
			if(FSfread(&node->sibling_pos, 1, 2, file) < 2)
				return 0;

			position += 2;
		}

		// [data type]
		if(!FSfread(&node->data_type, 1, 1, file))
			return 0;

		position++;

		// [data]
		node->data_pos = position;
	}

	return position;
}
/*********************************************************************
 * Function:		PRIVATE BYTE _CheckForStructure(	BYTE *pkt, 
 *														WORD *data_len)    
 *
 * PreCondition:    None
 *
 * Input:           pkt			- SNMP Rx Packet
 *					data_len	- pointer to the data length
 *
 * Output:          Number of header bytes
 *
 * Side Effects:    None
 *
 * Overview:        This function checks the incoming packet for a valid
 *					SNMP structure
 ********************************************************************/
PRIVATE BYTE _CheckForStructure(BYTE *pkt, WORD *data_len)
{
	BYTE	header_bytes;
	WORD	length;


	// make sure that we have a start of a structure
	if(_GetByte(pkt) != STRUCTURE)
		return 0;


	header_bytes = _GetLength(pkt, &length);

	if(!header_bytes)
		return 0;

	header_bytes++;

	*data_len = length;

	return header_bytes;
}
/*********************************************************************
 * Function:		PRIVATE BYTE _GetLength(	BYTE *pkt, 
 *												WORD *length)
 *
 * PreCondition:    None.
 *
 * Input:           pkt			- SNMP Rx Packet
 *					length		- pointer to the data length
 *
 * Output:			the amount of data read          
 *
 * Side Effects:    None
 *
 * Overview:        This function gets the length from the incoming
 *					SNMP packet.
 ********************************************************************/
PRIVATE BYTE _GetLength(BYTE *pkt, WORD *length)
{
	BYTE		result = 0;
	WORD_VAL	word_length;
	BYTE		temp;

	word_length.Val		= 0;
	word_length.v[0]    = _GetByte(pkt);
    temp                = word_length.v[0];

	if(temp & 0x80)
	{

		temp &= 0x7F;

		if(temp > 2)
		{
			return 0;
		}

		result = temp + 1;

		while(temp)
		{
			temp--;
			word_length.v[temp] = _GetByte(pkt);
		}
	}else
	{
		result = 1;
	}

	*length = word_length.Val;

	return result;
}
/*********************************************************************
 * Function:       PRIVATE BOOL _GetIntData(	BYTE *pkt, 
 *												DWORD *data)
 *
 * PreCondition:    None
 *
 * Input:           pkt			- SNMP Rx Packet
 *					data		- pointer to the data 
 *
 * Output:			true if data was valid, else false   
 *
 * Side Effects:    None
 *
 * Overview:        This function get the data from the incoming
 *					SNMP packet.
 ********************************************************************/
PRIVATE BOOL _GetIntData(BYTE *pkt, DWORD *data)
{
	DWORD_VAL	temp_data;
	WORD_VAL	temp_len;

	temp_len.Val = 0;

	if(_GetByte(pkt) != ASN_INT)
		return FALSE;


	if(!_GetLength(pkt, &temp_len.Val))
		return FALSE;

	if(temp_len.Val > 4)
		return FALSE;

	temp_data.Val	= 0;

	while(temp_len.Val)
	{
		temp_len.Val--;
		temp_data.v[temp_len.Val] = _GetByte(pkt);
	}

	*data = temp_data.Val;

	return TRUE;
}
/*********************************************************************
 * Function:		PRIVATE BOOL _GetCommunity(	BYTE *pkt, 
 *												BYTE *community, 
 *												BYTE *len)
 *
 * PreCondition:	None
 *
 * Input:           pkt			- SNMP Rx Packet
 *					community	- pointer to the community
 *					len			- pointer to the size of the community data
 *
 * Output:          true if a community was found, else false
 *
 * Side Effects:    None
 *
 * Overview:        This funciton gets the community from an incoming 
 *					SNMP packet.
 ********************************************************************/
PRIVATE BOOL _GetCommunity(BYTE *pkt, BYTE *community, BYTE *len)
{
	BYTE	temp;


	if(_GetByte(pkt) != OCTET_STRING)
		return FALSE;


	temp = _GetByte(pkt);
	*len = temp;


	if(temp > SNMP_COMMUNITY_MAX_LEN)
		return FALSE;


	while(temp)
	{
		temp--;
		*community = _GetByte(pkt);
		community++;
	}

	*community = 0;


	return TRUE;
}
/*********************************************************************
 * Function:       PRIVATE BYTE _GetPDU(	BYTE *pkt, 
 *											SNMP_ACTION *pdu)
 *
 * PreCondition:	   
 *
 * Input:           pkt			- SNMP Rx Packet
 *					pdu			- pointer to protocol data unit
 *
 * Output:          length of the data
 *
 * Side Effects:    None
 *
 * Overview:        This function gets the protocol data unit (pdu) from
 *					an incoming SNMP packet
 ********************************************************************/
PRIVATE BYTE _GetPDU(BYTE *pkt, SNMP_ACTION *pdu)
{
	WORD	len;
	
	*pdu = _GetByte(pkt);

	if(*pdu != GET_REQUEST && *pdu != GET_NEXT_REQUEST && *pdu != SET_REQUEST)
	{
		*pdu = SNMP_ACTION_UNKNOWN;
		return 0;
	}

	return _GetLength(pkt, &len);

}
/*********************************************************************
 * Function:       PRIVATE BOOL _GetOID(	BYTE *pkt, 
 *											BYTE *oid, 
 *											BYTE *len)
 *
 * PreCondition:	None
 *
 * Input:           pkt			- SNMP Rx Packet
 *					oid			- pointer to the obnject id
 *					len			- pointer to the size of the object id
 *
 * Output:          true if a valid object id was found, else false
 *
 * Side Effects:    None
 *
 * Overview:        This function get the object id from an incoming 
 *					SNMP packet
 ********************************************************************/
PRIVATE BOOL _GetOID(BYTE *pkt, BYTE *oid, BYTE *len)
{
	DWORD_VAL	temp;


	if(_GetByte(pkt) != ASN_OID)
		return FALSE;


	if(!_GetLength(pkt, &temp.w[0]))
		return FALSE;

	if(temp.w[0] > OID_MAX_LEN)
		return FALSE;

	*len = temp.v[0];

	while(temp.v[0])
	{
		*oid = _GetByte(pkt);
		oid++;
		temp.v[0]--;
	}


	return TRUE;
}
/*********************************************************************
 * Function:       PRIVATE void _SetErrorStatus(	BYTE *pkt, 
 *													WORD status_offset, 
 *													WORD idx_offset, 
 *													SNMP_ERR_STATUS err_status, 
 *													BYTE err_idx)
 *
 * PreCondition:    None
 *
 * Input:           pkt				- SNMP Rx Packet
 *					status_offset	- error status offset
 *					idx_offset		-  the error index offset
 *					err_status		- error status (SNMP_ERR_STATUS)
 *					err_idx			- error index
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function puts the error status and index in the 
 *					SNMP packet to be transmitted.
 ********************************************************************/
PRIVATE void _SetErrorStatus(BYTE *pkt, WORD status_offset, WORD idx_offset, SNMP_ERR_STATUS err_status, BYTE err_idx)
{
	WORD	temp;

	temp = _GetTxPosition();

	_SetTxPosition(status_offset);
	_PutByte(pkt, err_status);

	_SetTxPosition(idx_offset);
	_PutByte(pkt, err_idx);

	_SetTxPosition(temp);

}
/*********************************************************************
 * Function:       PRIVATE BOOL _GetDataTypeInfo(	DATA_TYPE type, 
 *													DATA_TYPE_INFO *info)
 *
 * PreCondition:    None
 *
 * Input:           type		- the data type 
 *					info		- pointer to the type and length of the data
 *
 * Output:          true if the data type was present, else false
 *
 * Side Effects:    None
 *
 * Overview:        This function fills the DATA_TYPE_INFO structure with the
 *					type and length of the requested data
 ********************************************************************/
PRIVATE BOOL _GetDataTypeInfo(DATA_TYPE type, DATA_TYPE_INFO *info)
{
	if(type >=  DATA_TYPE_UNKNOWN)
		return FALSE;

	info->asnType	= _dataTypeTable[type].asnType;	
	info->asnLen	= _dataTypeTable[type].asnLen;

	return TRUE;
}
/*********************************************************************
 * Function:       PRIVATE BOOL _ChkASNNull(BYTE *pkt)
 *
 * PreCondition:    None
 *
 * Input:           pkt				- SNMP Rx Packet
 *
 * Output:			true if it is ASN NULL, else false      
 *
 * Side Effects:    None
 *
 * Overview:        This function makes sure that the packet is ASN NULL valid.
 ********************************************************************/
PRIVATE BOOL _ChkASNNull(BYTE *pkt)
{

	if(_GetByte(pkt) != ASN_NULL)
		return FALSE;

	
	return (!_GetByte(pkt));

}
/*********************************************************************
 * Function:       PRIVATE BYTE _GetByte(BYTE *packet)
 *
 * PreCondition:    None
 *
 * Input:           packet - packet to read a byte from
 *
 * Output:          the byte from the packet
 *
 * Side Effects:    None
 *
 * Overview:        This function reads the data from an incoming packet
 *
 * Note:			Before starting to read, the user must first initialize
 *					the _rxIndex value.
 ********************************************************************/
PRIVATE BYTE _GetByte(BYTE *packet)
{
	if(_rxIndex >= SNMP_PKT_LEN)
		return 0xFF;

	return packet[_rxIndex++];
}
/*********************************************************************
 * Function:       PRIVATE BOOL _PutByte(	BYTE *packet, 
 *											BYTE data)
 *
 * PreCondition:    None
 *
 * Input:           packet	- packet to put a byte
 *					data	- byte to put in the packet
 *
 * Output:          true if allowed, else false
 *
 * Side Effects:    None
 *
 * Overview:        This function puts a byte into the packet.
 *
 * Note:			Before starting to place bytes into the packet, the user
 *					should initialize _txIndex.
 ********************************************************************/
PRIVATE BOOL _PutByte(BYTE *packet, BYTE data)
{
	if(_txIndex >= SNMP_PKT_LEN)
		return FALSE;

	packet[_txIndex++] = data;

	return TRUE;
}
/*********************************************************************
 * Function:       PRIVATE BOOL _PutBuffer(	BYTE *packet, 
 *											const BYTE *buffer, 
 *											WORD size)
 *
 * PreCondition:    None
 *
 * Input:           packet	- packet to put a byte
 *					buffer	- buffer of bytes to put in the packet
 *					size	- size of the buffer
 *
 * Output:          true of sucess, else false
 *
 * Side Effects:    None
 *
 * Overview:        This funciton puts a buffer of bytes into the 
 *					packet.
 *
 * Note:			Before starting to place bytes into the packet, the user
 *					should initialize _txIndex.
 ********************************************************************/
PRIVATE BOOL _PutBuffer(BYTE *packet, const BYTE *buffer, WORD size)
{
	if((size + _txIndex) >= SNMP_PKT_LEN)
		return FALSE;

	while(size)
	{
		size--;
		packet[_txIndex++] = *buffer;
		buffer++;
	}

	return TRUE;

}
/*********************************************************************
 * Function:       PRIVATE void _SetTxPosition(WORD pos)
 *
 * PreCondition:    None
 *
 * Input:           pos		- position that one wishes to set the _txIndex
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function set the current position of the _txIndex
 ********************************************************************/
PRIVATE void _SetTxPosition(WORD pos)
{
	if(pos >= SNMP_PKT_LEN)
		return;

	_txIndex = pos;
}
/*********************************************************************
 * Function:		PRIVATE WORD _GetTxPosition(void)     
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          The current position of the _txIndex
 *
 * Side Effects:    None
 *
 * Overview:        This function provides the _txIndex
 ********************************************************************/
PRIVATE WORD _GetTxPosition(void)
{
	return _txIndex;
}
