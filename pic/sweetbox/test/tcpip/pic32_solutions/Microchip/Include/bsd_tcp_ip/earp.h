/*********************************************************************
 *
 *                  ARP module Header for Microchip eTCP/IP Stack
 *
 *********************************************************************
 * FileName:        arp.h
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
 * $Id: eARP.h,v 1.1 2006/09/28 22:10:28 C12770 Exp $
 *
 ********************************************************************/



void InitiateARP( DWORD TargetIP );
void ARP_RxHandler( NET_PKT *sbfr );
void ARP_SendPkt( WORD OpCode, DWORD SenderIP, BYTE* pSenderMAC, 
						   	   DWORD TargetIP, BYTE* pTargetMAC );

/*
 * ARP Operation codes.
 */
#define ARP_OPERATION_REQ       0x01
#define ARP_OPERATION_RESP      0x02

