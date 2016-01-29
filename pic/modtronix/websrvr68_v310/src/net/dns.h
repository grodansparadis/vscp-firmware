/**
 * @brief           DNS Client Module Header
 * @file            dns.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    stacktsk.h, tcp.h
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  Microchip C30 v2.01 or higher
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_tcpip_user_ftp
 *
 * @section description Description
 **********************************
 * DNS Client module.
 *
 * <br>For a detailed description, see the @ref mod_tcpip_user_dns section of this document - in 
 * [Modules] [TCP/IP Stack] [TCP/IP User Protocols].
 *
 *
 * @section dns_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //-------------------- DNS  Configuration --------------------
 //*********************************************************************
 // DNS Port. If not defined in "projdefs.h" file, defaults to 53
 #define DNS_PORT        53

 // DNS Timeout. If not defined in "projdefs.h" file, defaults to 500ms
 #define DNS_TIMEOUT        ((TICK)TICK_SECOND * (TICK)2)
 @endcode
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on v2.20.04.01
 * of the Microchip TCP/IP stack. The original code base is owned by Microchip
 * Technology Incorporated (the Company), and is protected under applicable
 * copyright laws. The modifications are owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller based products manufactured
 * by Modtronix Engineering. The code may be modified and can be used free of charge
 * for non commercial and commercial applications. All rights are reserved. Any use
 * in violation of the foregoing restrictions may subject the user to criminal
 * sanctions under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 ********************************************************************
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder     06/01/23 Original
 ********************************************************************/

/////////////////////////////////////////////////
// DNS - documentation module
/**
@defgroup mod_tcpip_user_dns DNS
@ingroup mod_tcpip_user



@section mod_tcpip_user_dns_desc Description
DNS Module.
@if DEMO_WEB_SERVER
The DNS address is initilized via the DHCP server. If DHCP is not used, the default
DNS address is used. The default DNS address can be configured via the Web Interface.
@endif



@section mod_tcpip_user_dns_conf Configuration
For details on configuring this module, see @ref dns_conf section of dns.h file.
*/





#ifndef DNS_H
#define DNS_H

#include "net\compiler.h"
#include "net\stacktsk.h"

/**
 * Call DNSIsResolved() until the host is resolved.
 * A UDP socket must be available before this function is called. It is freed
 * at the end of the resolution.  MAX_UDP_SOCKETS may need to be increased if
 * other modules use UDP sockets. You must not modify Hostname until
 * DNSIsResolved() returns TRUE.
 *
 * @preCondition        Stack is initialized()
 *
 * @param HostName      Null terminated string specifying the host address to
 *                      resolve to an IP address.
 */
void    DNSResolve(BYTE *HostName);


/**
 * Call DNSIsResolved() until the host is resolved.
 * You cannot start two DNS resolution proceedures concurrently.
 * You must not modify *Hostname until DNSIsResolved() returns TRUE.
 *
 * @preCondition        DNSResolve() was called.
 *
 * @param HostIP        4 byte IP address
 */
BOOL    DNSIsResolved(IP_ADDR *HostIP);


#endif
