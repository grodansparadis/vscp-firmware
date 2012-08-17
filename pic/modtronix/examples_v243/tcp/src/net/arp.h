/**
 * @brief           ARP Module for Modtronix TCP/IP Stack
 * @file            arp.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    stacktsk.h, mac.h
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_tcpip_base_arp
 * @ingroup         mod_tcpip_user_ipgleaning
 *
 *
 * @section description Description
 **********************************
 * This module contains the ARP code.
 * <br>For a detailed description, see the @ref mod_tcpip_base_arp section of this document - in 
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
 * Nilesh Rajbharti     5/1/01  Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 ********************************************************************/


/////////////////////////////////////////////////
// ARP Module - documentation module
/**
@defgroup mod_tcpip_base_arp ARP
@ingroup mod_tcpip_base
The ARP layer of the Modtronix TCP/IP Stack is actually implemented by two modules: ARP and ARPTask.
ARP, implemented by the file "arp.c", creates the ARP primitives. ARPTask, implemented by the file
"arptsk.c", utilizes the primitives and provides complete ARP services.

ARPTask is implemented as a cooperative state machine, responding to ARP requests from the remote host.
It also maintains a one-level cache to store the ARP reply and returns to a higher level when the
appropriate calls are made. ARPTask does not implement a retry mechanism, so the upper level modules
or applications must detect time-out conditions and respond accordingly.

arptask operates in two modes: Server mode and Server/Client mode.
@li In Server/Client mode, a portion of code is enabled and compiled to generate ARP requests from the
local host itself.
@li In Server mode, the ARP request code is not compiled.

Usually, if the stack is used with server applications (such as HTTP Server or FTP serve)r, arptask should
be compiled in Server mode to reduce code size.

The compiler define STACK_CLIENT_MODE includes the client portion of code. In Server/Client mode, ARPTask
maintains a one-level cache to store the ARP reply from the remote host. When Server/Client mode is not
enabled, the cache is not defined and the corresponding RAM and program memory is not used.
*/ 

#ifndef ARP_H
#define ARP_H

#include "net\ip.h"

/*
 * Following codes are must be used with ARPGet/Put functions.
 */
#define ARP_REPLY       (0x00)
#define ARP_REQUEST     (0x01)
#define ARP_UNKNOWN     (0x02)


/**
 * If a valid ARP packet addressed to us was received, true is returned and
 * opcode (ARP_REPLY or ARP_REQUEST) is writen to given opCode buffer.
 *
 * @preCondition ARP packet is ready in MAC buffer.
 * @param remote Remote node info
 * @param opCode Buffer to hold ARP op code.
 * @return TRUE if a valid ARP packet was received, and addressed to us.<br>
 *         FALSE otherwise.
 */
BOOL    ARPGet(NODE_INFO *remote, BYTE *opCode);


/**
 * @return TRUE if ARP receive buffer is full.<br>
 *         FALSE otherwise.
 */
#define ARPIsTxReady()      MACIsTxReady()


/**
 * Send an ARP message.
 *
 * @param remote  - Remote node info
 * @param opCode  - ARP op code to send
 */
void    ARPPut(NODE_INFO *remote, BYTE opCode);

#endif

/**
@}
*/


