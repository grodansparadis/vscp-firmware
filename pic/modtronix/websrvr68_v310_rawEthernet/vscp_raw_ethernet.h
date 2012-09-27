/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * Copyright (C) 2011-2012 Ake Hedman, Grodans Paradis AB
 *
 * 
 * This file is part of VSCP - Very Simple Control Protocol
 * http://www.vscp.org
 *
 * ******************************************************************************
*/

#ifndef _VSCP_RAW_ETHERNET_H_
#define _VSCP_RAW_ETHERNET_H_

#include "vscp_firmware_level2.h"

#define FIRMWARE_MAJOR_VERSION			0x00
#define FIRMWARE_MINOR_VERSION			0x00
#define FIRMWARE_SUB_MINOR_VERSION		0x02

/** Structure to store a nodes info */
typedef struct _NODE_INFO
{
    MAC_ADDR    MACAddr;    //6 bytes
} NODE_INFO;    //14 bytes long



BOOL SendTestVSCPPacket( void );


#endif
