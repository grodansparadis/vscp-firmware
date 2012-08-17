/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * Copyright (C) 2011 Ake Hedman, eurosource
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 
 *	This file is part of VSCP - Very Simple Control Protocol 	
 *	http://www.vscp.org
 *
 * ******************************************************************************
*/

#ifndef _VSCP_RAW_ETHERNET_H_
#define _VSCP_RAW_ETHERNET_H_


#define FIRMWARE_MAJOR_VERSION			0x00
#define FIRMWARE_MINOR_VERSION			0x00
#define FIRMWARE_SUB_MINOR_VERSION		0x01

/** Structure to store a nodes info */
typedef struct _NODE_INFO
{
    MAC_ADDR    MACAddr;    //6 bytes
} NODE_INFO;    //14 bytes long



BOOL SendTestVSCPPacket( void );

#endif
