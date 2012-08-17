/********************************************************************
 * FileName:		SNMP_Private.h		
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

#ifndef _SNMP_PRIVATE_HEADER_FILE
#define _SNMP_PRIVATE_HEADER_FILE

#define SNMP_V1                 (0)
#define STRUCTURE               (0x30)
#define ASN_INT                 (0x02)
#define OCTET_STRING            (0x04)
#define ASN_NULL                (0x05)
#define ASN_OID                 (0x06)

#define GET_REQUEST             (0xa0)
#define GET_NEXT_REQUEST        (0xa1)
#define GET_RESPONSE            (0xa2)
#define SET_REQUEST             (0xa3)
#define TRAP                    (0xa4)

// SNMP specific variables
#define SNMP_IP_ADDR            (0x40)
#define SNMP_COUNTER32          (0x41)
#define SNMP_GAUGE32            (0x42)
#define SNMP_TIME_TICKS         (0x43)
#define SNMP_OPAQUE             (0x44)
#define SNMP_NSAP_ADDR          (0x45)


enum 
{
    INT8_VAL        = 0x00,
    INT16_VAL       = 0x01,
    INT32_VAL       = 0x02,
    BYTE_ARRAY      = 0x03,
    ASCII_STRING    = 0x04,
    IP_ADDRESS      = 0x05,
    COUNTER32       = 0x06,
    TIME_TICKS_VAL  = 0x07,
    GAUGE32         = 0x08,
    OID_VAL         = 0x09,

    DATA_TYPE_UNKNOWN
};

typedef BYTE    DATA_TYPE;

typedef enum _SNMP_ERR_STATUS
{
    SNMP_NO_ERR = 0,
    SNMP_TOO_BIG,
    SNMP_NO_SUCH_NAME,
    SNMP_BAD_VALUE,
    SNMP_READ_ONLY,
    SNMP_GEN_ERR
} SNMP_ERR_STATUS;

/*******************************************************************
 * unions
 *******************************************************************/

/*******************************************************************
 * type definitions
 *******************************************************************/
typedef union _INDEX_INFO
{
    struct
    {
        unsigned int bIsOID:1;
    } Flags;
    BYTE Val;
} __attribute__((packed)) INDEX_INFO;


typedef struct
{
	BYTE	length;
	BYTE	data[OID_MAX_LEN];
} __attribute__((packed)) OID_DATA;

typedef union _MIB_INFO
{
    struct
    {
        BYTE bIsDistantSibling:		1;
        BYTE bIsConstant:			1;
        BYTE bIsSequence:			1;
        BYTE bIsSibling:			1;
        BYTE bIsParent:				1;
        BYTE bIsEditable:			1;
        BYTE bIsAgentID:			1;
        BYTE bIsIDPresent:			1;
    } flags;

    BYTE val;

}  __attribute__((packed)) MIB_INFO;

typedef struct _OID_INFO
{
    DWORD			node_pos;

    BYTE            oid;
    MIB_INFO        mib_info;
    DATA_TYPE       data_type;
    SNMP_ID         id;

    WORD_VAL        data_len;
    WORD            data_pos;
    WORD            sibling_pos;
    WORD            child_pos;
    BYTE            index;
    BYTE            index_len;
}  __attribute__((packed)) OID_INFO;

typedef struct _DATA_TYPE_INFO
{
    BYTE asnType;
    BYTE asnLen;
}  __attribute__((packed)) DATA_TYPE_INFO;

typedef struct _SNMP_NOTIFY_INFO
{
    SOCKET		socket;
    char		community[SNMP_NOTIFY_COMM_LEN];
    BYTE		communityLen;
    SNMP_ID		agentIDVar;
    BYTE		notificationCode;
    DWORD_VAL	timestamp;
	IP_ADDR		remoteHost;
}  __attribute__((packed)) SNMP_NOTIFY_INFO;

/*******************************************************************
 * Private functions
 *******************************************************************/
PRIVATE int _PacketReady(void);
PRIVATE BYTE _GetLength(BYTE *pkt, WORD *length);
PRIVATE BOOL	_GetIntData(BYTE *pkt, DWORD *data);
PRIVATE BYTE _CheckForStructure(BYTE *pkt, WORD *data_len);
PRIVATE BOOL _GetCommunity(BYTE *pkt, BYTE *community, BYTE *len);
PRIVATE BYTE _GetPDU(BYTE *pkt, SNMP_ACTION *pdu);
PRIVATE BYTE _ProcessGetSetHeader(BYTE *pkt, DWORD *request);
PRIVATE SNMP_ACTION _ProcessHeader(BYTE *pkt, BYTE *community, BYTE *commumnitylen);
PRIVATE BOOL _OIDLookUp(FSFILE *file, BYTE *oid, BYTE oid_len, OID_INFO *oid_info);
PRIVATE BOOL _ProcessMsgVaribles(BYTE *pkt, SOCKADDR_IN *sock_addr, int sock_len, FSFILE *file, BYTE *community, BYTE comm_len, DWORD_VAL *request, BYTE pdu_type);
PRIVATE BOOL _ChkASNNull(BYTE *pkt);
PRIVATE BYTE _GetByte(BYTE *packet);
PRIVATE BOOL _PutByte(BYTE *packet, BYTE data);
PRIVATE BOOL _PutBuffer(BYTE *packet, const BYTE *buffer, WORD size);
PRIVATE void _SetTxPosition(WORD pos);
PRIVATE WORD _GetTxPosition(void);
PRIVATE BYTE _ProcessGetVariable(BYTE *pkt, FSFILE *file, OID_INFO *oid_info, BOOL as_oid);
PRIVATE void _SetErrorStatus(BYTE *pkt, WORD status_offset, WORD idx_offset, SNMP_ERR_STATUS err_status, BYTE err_idx);
PRIVATE DWORD _ReadMIBRecord(FSFILE *file, DWORD position, OID_INFO *node);
PRIVATE BYTE _ProcessSetVariable(BYTE *tx_pkt, BYTE *rx_pkt, FSFILE *file, OID_INFO *oid_info, SNMP_ERR_STATUS *err_status);
PRIVATE BYTE _ProcessGetNextVariable(BYTE *tx_pkt, FSFILE *file, OID_INFO *oid_info);
PRIVATE BOOL _GetOID(BYTE *pkt, BYTE *oid, BYTE *len);
PRIVATE BOOL _GetDataTypeInfo(DATA_TYPE type, DATA_TYPE_INFO *info);
PRIVATE BOOL _GetNextLeaf(FSFILE *file, OID_INFO *node);
PRIVATE BOOL _GetOIDStringByAddress(FSFILE *file, OID_INFO *node, BYTE *string, BYTE *len);
PRIVATE BOOL _GetOIDStringByID(FSFILE *file, SNMP_ID id, OID_INFO *node, BYTE *string, BYTE *len);

#endif
