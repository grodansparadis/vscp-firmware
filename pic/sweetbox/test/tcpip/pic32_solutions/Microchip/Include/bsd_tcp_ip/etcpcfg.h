/*********************************************************************
 *
 *            Stack Config Header File
 *
 *********************************************************************
 * FileName:        eTCPcfg.h
 * Description: 	Include basic configuration for TCP Stack
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
 * $Id: eTCPcfg.h,v 1.6 2006/09/12 18:38:36 C12770 Exp $
 *
*/


#ifndef __eTCP_CFG___
#define __eTCP_CFG___

#include <Generic.h>
#include <p32xxxx.h>
#include "eTCP.def"

typedef struct _MAC_ADDR
{
    BYTE v[6];
} MAC_ADDR;

typedef union _IP_ADDR
{
    BYTE        v[4];
	WORD		w[2];
    DWORD       Val;
} IP_ADDR;

extern IP_ADDR  SysIPAddr;		// big endian
extern IP_ADDR  SysIPMask;		// big-endian
extern IP_ADDR 	SysIPGateway;	// big-endian
extern MAC_ADDR SysMACAddr;	// big-endian

#endif

