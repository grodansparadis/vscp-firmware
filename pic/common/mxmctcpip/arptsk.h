/*********************************************************************
 *
 *                  ARP Server Defs for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        ARPTsk.h
 * Dependencies:    stacktsk.h
 *                  MAC.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Microchip Technology, Inc.
 *
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
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     8/20/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 ********************************************************************/

#ifndef ARP_TSK_H
#define ARP_TSK_H

#include <projdefs.h>
#include "ip.h"


/*********************************************************************
 * Function:        void ARPInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          ARP Cache is initialized.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
void ARPInit(void);


/*********************************************************************
 * Function:        BOOL ARPProcess(void)
 *
 * PreCondition:    ARP packet is ready in MAC buffer.
 *
 * Input:           None
 *
 * Output:          ARP FSM is executed.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
BOOL ARPProcess(void);


/*********************************************************************
 * Function:        void ARPResolve(IP_ADDR* IPAddr)
 *
 * PreCondition:    None
 *
 * Input:           IPAddr  - IP Address to be resolved.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        An ARP request is sent.
 *
 * Note:
 ********************************************************************/
void ARPResolve(IP_ADDR *IPAddr);


/*********************************************************************
 * Function:        BOOL ARPIsResolved(IP_ADDR* IPAddr,
 *                                      MAC_ADDR *MACAddr)
 *
 * PreCondition:    None
 *
 * Input:           IPAddr      - IPAddress to be resolved.
 *                  MACAddr     - Buffer to hold corresponding
 *                                MAC Address.
 *
 * Output:          TRUE if given IP Address has been resolved.
 *                  FALSE otherwise.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            This function is available only when
 *                  STACK_CLIENT_MODE is defined.
 ********************************************************************/
BOOL ARPIsResolved(IP_ADDR *IPAddr, MAC_ADDR *MACAddr);



#endif
