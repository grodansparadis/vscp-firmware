/**
 * @brief           ARP Server Module for Modtronix TCP/IP Stack
 * @file            arptsk.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    stacktsk.h, mac.h
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_tcpip_base_arp
 *
 * @section description Description
 **********************************
 * This module contains the ARP Server code.
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
 * Nilesh Rajbharti     8/20/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 ********************************************************************/

#ifndef ARP_TSK_H
#define ARP_TSK_H

#include "net\ip.h"


/**
 * ARP Cache is initialized.
 */
void ARPInit(void);


/**
 * Performes ARP processing.
 * @preCondition ARP packet is ready in MAC buffer.
 * @return ARP FSM is executed.
 */
BOOL ARPProcess(void);


/**
 * An ARP request is sent.
 * This function is available only when STACK_CLIENT_MODE is defined.
 *
 * @param IPAddr  - IP Address to be resolved. For example, if IP address is "192.168.1.0", then:
 * - IPAddr.v[0] = 192
 * - IPAddr.v[1] = 168
 * - IPAddr.v[2] = 1
 * - IPAddr.v[3] = 10
 */
void ARPResolve(IP_ADDR *IPAddr);


/**
 * This function is used to retrieve the MAC Address for a given IP address.
 * This information will be received via ARP after sending an ARP request over
 * the network. If we have a destination IP address, but not it's MAC address,
 * we will send an ARP request, and wait for an ARP reply.
 * As soon as we receive the reply, the requested MAC address can be retrieve
 * via this function.<br>
 * <b>Note:</b> This function is available only when STACK_CLIENT_MODE is defined.
 * @param IPAddr IPAddress to be resolved.
 * @param MACAddr Buffer to hold corresponding MAC Address.
 * @return TRUE if given IP Address has been resolved.<br>
 *         FALSE otherwise.
 */
BOOL ARPIsResolved(IP_ADDR *IPAddr, MAC_ADDR *MACAddr);

#endif
