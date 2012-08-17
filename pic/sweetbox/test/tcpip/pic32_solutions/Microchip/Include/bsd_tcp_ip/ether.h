/*********************************************************************
 *
 *            ether layer Header File
 *
 *********************************************************************
 * FileName:        ether.h
 * Description: 	ether layer header
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
 * $Id: ether.h,v 1.4 2006/10/24 16:39:08 C12923 Exp $
 *
*/

#ifndef __ETHER_HD__
#define __ETHER_HD__

#include "bsd_tcp_ip\netpkt.h"

#define MAX_ETHERNET_FRAME_SZ 1518

#define ETH_ADRLEN			6

#define PKT_LOCAL			10
#define PKT_BROADCAST		11
#define PKT_MULTICAST		12
#define PKT_NON_LOCAL		13
#define PKT_LOOPBACK		14

#define ETH_PROTO_IP		0x0800		// Internet Protocol packet
#define ETH_PROTO_ARP		0x0806		// Address Resolution packet

WORD GetEtherFrameInfo( NET_PKT* sbfr );
void FormatEtherFrame( NET_PKT* sbfr, WORD proto, BYTE* pSrcMAC );
void SetEtherDestMAC( NET_PKT* sbfr, BYTE* pDestMAC );

#define ETH_ADRLEN	6
#define ETH_HDRLEN	14

#pragma pack(push, 1)
typedef struct _ETHER_HEADER
{
    BYTE		destMAC[ETH_ADRLEN];
    BYTE		srcMAC[ETH_ADRLEN];
    WORD		Type;
} ETHER_HEADER;
#pragma pack(pop)

#endif
