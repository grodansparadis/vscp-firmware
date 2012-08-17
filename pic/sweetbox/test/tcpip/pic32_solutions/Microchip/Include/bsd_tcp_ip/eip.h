/*********************************************************************
 *
 *                  IP module for Microchip eTCP/IP Stack
 *
 *********************************************************************
 * FileName:        ip.h
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
 * $Id: eIP.h,v 1.2 2006/10/24 16:39:07 C12923 Exp $
 *
 ********************************************************************/

#ifndef _IP_HDR___
#define _IP_HDR___

#include "bsd_tcp_ip\netpkt.h"

#define IP_PROT_ICMP    1
#define IP_PROT_TCP     6
#define IP_PROT_UDP     17

typedef struct _IP_HEADER
{
    BYTE    IHL_Version;                // 4 bit nibble
    BYTE    TypeOfService;
    WORD    TotalLength;
    WORD    Identification;
    WORD    FragmentInfo;
    BYTE    TimeToLive;
    BYTE    Protocol;
    WORD    HeaderChecksum;
    BYTE    SourceAddr[4];
    BYTE	DestAddr[4];
} IP_HEADER;

BOOL 	IP_FormatPacket( NET_PKT* sbfr, DWORD DestIP, BYTE protocol, WORD len );

int 	IP_TransmitPkt( NET_PKT* sbfr );
#define IP_TX_SUCCESS		0
#define IP_TX_QUEUED		1
#define IP_TX_ARPQUEUED		2

void IPProcess(void);
void InitIPLayer();
BOOL IP_IsARPqueued( NET_PKT* sbfr );

#endif
