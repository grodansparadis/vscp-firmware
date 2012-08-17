/*********************************************************************
 *
 *                  DHCP Defs for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        DHCP.h
 * Dependencies:    stacktsk.h
 *                  UDP.h
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
 * Nilesh Rajbharti     3/21/01  Original        (Rev 1.0)
 ********************************************************************/

/*******************************************************
----------------- DHCP Configuration --------------------
********************************************************
//The following should be defined in the projdefs.h file OR on the command line

//Defines DHCP ports
#define DHCP_CLIENT_PORT    (68)
#define DHCP_SERVER_PORT    (67)

//Timeouts
#define DHCP_TIMEOUT        (TICK)(2L * TICK_SECOND)
/********************************************************/

#ifndef DHCP_H
#define DHCP_H

#include "projdefs.h"


/////////////////////////////////////////////////
//Default defines - they can be overridden in projdefs.h file

//Defines DHCP ports
#if !defined(DHCP_CLIENT_PORT)
#define DHCP_CLIENT_PORT    (68)
#endif

#if !defined(DHCP_SERVER_PORT)
#define DHCP_SERVER_PORT    (67)
#endif

//Timeouts
#if !defined(DHCP_TIMEOUT)
#define DHCP_TIMEOUT        (TICK)(2L * TICK_SECOND)
#endif


typedef enum _SM_DHCP
{
    SM_DHCP_INIT,
    SM_DHCP_RESET_WAIT,
    SM_DHCP_BROADCAST,
    SM_DHCP_DISCOVER,
    SM_DHCP_REQUEST,
    SM_DHCP_BIND,
    SM_DHCP_BOUND,
    SM_DHCP_DISABLED,
    SM_DHCP_ABORTED
} SM_DHCP;

#if !defined(THIS_IS_DHCP)
    extern SM_DHCP smDHCPState;
#endif

/*********************************************************************
 * Macro:           void DHCPDisable(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Puts DHCPTask into unhandled state "SM_DHCP_DISABLED"
 *                  and hence DHCP is effictively disabled.
 *
 * Note:            This macro should be called before DHCPTask is called
 *                  or else a UDP port will be kept open and there will
 *                  be no task to process it.
 ********************************************************************/
#define DHCPDisable()       (smDHCPState = SM_DHCP_DISABLED)


/*********************************************************************
 * Function:        void DHCPTask(void)
 *
 * PreCondition:    DHCPInit() is already called AND
 *                  IPGetHeader() is called with
 *                  IPFrameType == IP_PROT_UDP
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Fetches pending UDP packet from MAC receive buffer
 *                  and dispatches it appropriate UDP socket.
 *                  If not UDP socket is matched, UDP packet is
 *                  silently discarded.
 *
 * Note:            Caller must make sure that MAC receive buffer
 *                  access pointer is set to begining of UDP packet.
 *                  Required steps before calling this function is:
 *
 *                  If ( MACIsRxReady() )
 *                  {
 *                      MACRxbufGetHdr()
 *                      If MACFrameType == IP
 *                          IPGetHeader()
 *                          if ( IPFrameType == IP_PROT_UDP )
 *                              Call DHCPTask()
 *                  ...
 ********************************************************************/
void DHCPTask(void);


/*********************************************************************
 * Function:        void DHCPAbort(void)
 *
 * PreCondition:    DHCPTask() must have been called at least once.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Forgets about any previous DHCP attempts and
 *                  closes DHCPSocket.
 *
 * Note:
 ********************************************************************/
void DHCPAbort(void);



/*********************************************************************
 * Macro:           BOOL DHCPIsBound(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE if DHCP is bound to given configuration
 *                  FALSE if DHCP has yet to be bound.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:
 ********************************************************************/
#define DHCPIsBound()       (DHCPState.bits.bIsBound)

typedef union _DHCP_STATE
{
    struct
    {
        unsigned int bIsBound : 1;
    } bits;
    BYTE Val;
} DHCP_STATE;

#if !defined(THIS_IS_DHCP)
    extern DHCP_STATE DHCPState;
#endif

/*********************************************************************
 * Macro:           void DHCPReset(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Closes any previously opened DHCP socket
 *                  and resets DHCP state machine so that on next
 *                  call to DHCPTask will result in new DHCP request.
 *
 * Note:            None
 ********************************************************************/
void DHCPReset(void);




#endif
