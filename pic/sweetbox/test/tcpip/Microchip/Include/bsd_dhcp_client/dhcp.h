/********************************************************************
 * $Id:  $
 * FileName:		DHCP.h
 * Dependencies:
 * Processor:
 * Hardware:
 * Assembler:		MPLAB C30
 * Linker:
 * Company:			Microchip Technology, Inc.
 *
 * Software License Agreement:
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
 *********************************************************************
 * File Description:
 *
 * DHCP Client Appilcation
 *
 * Change History:
 * Name				Date            Changes
 * Chad Mercer		06/19/06		Initial Version
 *
 ********************************************************************/

#ifndef _DHCP_HEADER_FILE
#define _DHCP_HEADER_FILE

#include "bsd_tcp_ip\tcpip.h"

typedef enum _SM_DHCP
{
    SM_DHCP_INIT,
    SM_DHCP_RESET_WAIT,
    SM_DHCP_DISCOVER,
    SM_DHCP_SELECTING,
    SM_DHCP_REQUESTING,
    SM_DHCP_PREBOUND,
    SM_DHCP_BOUND,
    SM_DHCP_RENEW,
    SM_DHCP_REBIND,
    SM_DHCP_DISABLED,
    SM_DHCP_ABORTED
} SM_DHCP;

#if !defined(THIS_IS_DHCP)
    extern SM_DHCP smDHCPState;
#endif
/********************************************************************
 * function prototypes
 ********************************************************************/
/*********************************************************************
 * Function:	    void DHCPInit(void)
 *
 * PreCondition:    The program needs to have already initialized
 *		    		TCP/IP stack.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    A socket will be created on the DHCP Port
 *
 * Overview:        The function creates a UDP socket and binds it
 *		    		to the DHCP Port to allow the application to
 *		    		act as a DHCP client. The DHCP client will also
 *                  be placed in SM_DHCP_INIT state.
 ********************************************************************/
void DHCPInit(void);

/*********************************************************************
 * Function:        void DHCPTask(void)
 *
 * PreCondition:    DHCPInit() needs to be called before SNMPTask can
 *		    		effectivity run
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function will serve as the state machine for the
 *                  DHCP client. As DHCP packets are exchanged between the
 *                  client and the server this task will advance through the
 *                  various state conditions. This task is also responsible
 *                  for monitoring lease experation conditions and handle 
 *					retransmissions.
 *
 * Note:	        None
 ********************************************************************/
 
void DHCPTask(void);

#endif
