/*********************************************************************
 *
 *            MAC API Header File
 *
 *********************************************************************
 * FileName:        mac.h
 * Description: 	MAC Layer API header
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
 * $Id: eMAC.h,v 1.1 2006/09/28 22:10:28 C12770 Exp $
 *
*/


#ifndef __MAC__HDR__
#define __MAC__HDR__

#include "bsd_tcp_ip\netpkt.h"

void MACInit(void);

BOOL MACIsLinked(void);

NET_PKT* MACGetRxPacket(void);

BOOL MACTransmit(NET_PKT* sbfr);

WORD MACGetTxQueueCount(void);

void MACTxProcess(void);

#endif
