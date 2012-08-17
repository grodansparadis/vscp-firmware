/*********************************************************************
 *
 *                  Memory Block Manager for Microchip eTCP/IP Stack
 *
 *********************************************************************
 * FileName:        block_mgr.h
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
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
 * $Id: block_mgr.h,v 1.3 2006/09/12 18:38:36 C12770 Exp $
 *
 ********************************************************************/

#ifndef __BLOCK_MGR_H
#define __BLOCK_MGR_H

#include "bsd_tcp_ip\etcpcfg.h"
#include "bsd_tcp_ip\netpkt.h"

void InitBufferManager();
NET_PKT* AllocSocketBuffer( WORD DataLen ); //creates buffer with extra space for ip and mac headers
NET_PKT* AllocPacketBuffer( WORD BfrSize ); //creates buffer for BfrSize
void DeAllocateBuffer( NET_PKT* pBfr );

#endif
