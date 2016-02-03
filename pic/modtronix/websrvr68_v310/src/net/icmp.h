/**
 * @brief           ICMP Module for Modtronix TCP/IP Stack
 * @file            icmp.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    stacktsk.h, ip.h, mac.h
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_tcpip_base_icmp
 * @ingroup         mod_tcpip_user_ipgleaning
 *
 * @section description Description
 **********************************
 * This module contains the ICMP code.
 * <br>For a detailed description, see the @ref mod_tcpip_base_icmp section of this document - in 
 * [Modules] [TCP/IP Stack] [TCP/IP Base Protocols].
 *
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
 * Nilesh Rajbharti     4/27/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 ********************************************************************/


/////////////////////////////////////////////////
// ICMP - documentation module
/**
@defgroup mod_tcpip_base_icmp ICMP
@ingroup mod_tcpip_base
The ICMP layer is implemented by the file 'ICMP.c'. The header file "icmp.h" defines the services
provided by the layer. In this architecture, the ICMP layer is a passive layer; it does not respond
to the ICMP data packet. Instead, higher level layers use ICMP primitives and fetch the ICMP packet,
interpret it and take appropriate action.

Normally, ICMP is used to send and receive IP error or diagnostic messages. In the Modtronix TCP/IP
Stack, ICMP implements ICMP primitives that can be used to generate any of the ICMP messages. In embedded
applications, ICMP is useful for diagnostic purpose. When enabled, ICMP can respond to 'ping' packets,
thus allowing a remote host to determine local host presence.

The Microchip ICMP layer only responds to ping data packets of up to 32 bytes; larger packets will be
ignored. If it is necessary to handle larger packets, modify the compiler define MAX_ICMP_DATA_LEN (in the
header file "stacktsk.h").
*/


/////////////////////////////////////////////////
// IP Gleaning - documentation module
/**
@defgroup mod_tcpip_user_ipgleaning IP Gleaning
@ingroup mod_tcpip_user
As a lean alternative to DCHP, the Modtronix TCP/IP Stack also implements a
simple method, known as IP Gleaning, to remotely set the IP address of Modtronix
Stack node. This method is not an Internet Protocol standard, and there is no
corresponding RFC document. IP Gleaning allows only the IP address to be set. For
complete IP configuration, DCHP must be used.

IP Gleaning does not require any additional software modules. Instead, it uses the
ARP and ICMP modules. To use it, the file "icmp.c" must be included in the project, and the
compiler define STACK_USE_IP_GLEANING must be uncommented in the StackTsh.h header file.

With IP Gleaning enabled, the Microchip Stack enters into a special "IP Configuration" mode
on RESET. During this mode, it accepts any ICMP (ping) message that is destined to this node,
and sets the node’s IP address to that of the ping message’s destination. Once a valid ping
packet is received, the stack exits from "Configuration" and enters into normal mode.

During configuration, the user application must not attempt to communicate; it may call the
function StackIsInConfigMode() to determine whether or not the stack is in Configuration mode.
StackIsInConfigMode() == TRUE indicates that Stack is in Configuration mode.

To remotely set the IP address of stack node, it is necessary to issue a sequence of commands
from a remote machine. For this example, assume that a node running the Microchip Stack needs
to be assigned an IP address of 10.10.5.106. The MAC address of this node (hexadecimal) is
0a-0a-0a-0a-0a-0a. After resetting the node, another system on the network (we will assume
a computer running Microsoft Windows) issues the commands:

@verbatim
> arp -s 10.10.5.106 0a-0a-0a-0a-0a-0a

> ping 10.10.5.106
@endverbatim

This should generate a standard series of ping responses from 10.10.5.106, indicating the node has
been successfully assigned the IP address.
*/



#ifndef ICMP_H
#define ICMP_H

#include "net\ip.h"
#include "net\mac.h"

// Windows ping uses 32 bytes, while MAC uses 56 bytes.
#define MAX_ICMP_DATA       (32ul)

#define MAX_ICMP_DATA_LEN   (MAX_ICMP_DATA)


typedef enum _ICMP_CODE
{
    ICMP_ECHO_REPLY = 0,
    ICMP_ECHO_REQUEST = 8
} ICMP_CODE, ICMP_REQUESTS;


/**
 * @return  TRUE if transmit buffer is ready <br>
 *          FALSE if transmit buffer is not ready
 */
#define ICMPIsTxReady()     MACIsTxReady(TRUE)


/**
 * A ICMP packet is created and put on MAC.
 *
 * @preCondition ICMPIsTxReady() == TRUE
 *
 * @param remote    Remote node info
 * @param code      ICMP_ECHO_REPLY or ICMP_ECHO_REQUEST
 * @param data      Data bytes
 * @param len       Number of bytes to send
 * @param id        ICMP identifier
 * @param seq       ICMP sequence number
 */
void ICMPPut(NODE_INFO *remote,
             ICMP_CODE code,
             BYTE *data,
             BYTE len,
             WORD id,
             WORD seq);


/**
 * Gets the next ICMP packet
 *
 * @preCondition    MAC buffer contains ICMP type packet.
 *
 * @param code      Buffer to hold ICMP code value
 * @param data      Buffer to hold ICMP data
 * @param len       Buffer to hold ICMP data length
 * @param id        Buffer to hold ICMP id
 * @param seq       Buffer to hold ICMP seq
 *
 * @return          TRUE if valid ICMP packet was received <br>
 *                  FALSE otherwise
 */
BOOL ICMPGet(ICMP_CODE *code,
             BYTE *data,
             BYTE *len,
             WORD *id,
             WORD *seq);



#endif


