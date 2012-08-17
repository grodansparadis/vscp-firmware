/**
 * @brief           Stack Task Module for Modtronix TCP/IP Stack
 * @file            stacktsk.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 **********************************************************************
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
 **********************************************************************
 * File History
 *
 * 2004-10-02, David Hosken (DH):
 *    - Created from Microchip TCP/IP stack v2.20.04.01
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 *********************************************************************/

#define THIS_IS_STACK_TASK

#include "projdefs.h"
#include "stacktsk.h"
#include "debug.h"

#include "net\arptsk.h"
#include "net\mac.h"
#include "net\ip.h"

#if defined(STACK_USE_TCP)
#include "net\tcp.h"
#endif

#if defined(STACK_USE_HTTP_SERVER)
#include "net\http.h"
#endif

#if defined(STACK_USE_ICMP)
    #include "net\icmp.h"
#endif

#if defined(STACK_USE_UDP)
    #include "net\udp.h"
#endif

#if defined(STACK_USE_DHCP)
    #include "net\dhcp.h"
#endif

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xD7, msgCode)


/////////////////////////////////////////////////
//Modtronix TCP/IP Stack Version number
// - n = major part, and can be 1 or 2 digets long
// - mm is minor part, and must be 2 digets long!
ROM char STACK_VER_STR[] = "V2.43";


/*
 * Stack FSM states.
 */
typedef enum _SM_STACK
{
    SM_STACK_IDLE,
    SM_STACK_MAC,
    SM_STACK_IP,
    SM_STACK_ICMP,
    SM_STACK_ICMP_REPLY,
    SM_STACK_ARP,
    SM_STACK_TCP,
    SM_STACK_UDP
} SM_STACK;
static SM_STACK smStack;


#if defined(STACK_USE_IP_GLEANING) || defined(STACK_USE_DHCP)
STACK_FLAGS stackFlags;
#endif

/**
 * This function must be called before any of the
 * stack or its component routines be used.
 *
 * Side Affect: Stack and its componentns are initialized
 */
void StackInit(void)
{
    smStack                     = SM_STACK_IDLE;

#if defined(STACK_USE_IP_GLEANING) || defined(STACK_USE_DHCP)
    /*
     * If DHCP or IP Gleaning is enabled,
     * startup in Config Mode.
     */
    stackFlags.bits.bInConfigMode = TRUE;

#endif

    MACInit();

    ARPInit();

#if defined(STACK_USE_UDP)
    UDPInit();
#endif

#if defined(STACK_USE_TCP)
    TCPInit();
#endif

}

/**
 * This FSM checks for new incoming packets, and routes it to appropriate
 * stack components. It also performs timed operations.
 *
 * This function must be called periodically called
 * to make sure that timely response.
 *
 * @preCondition    StackInit() is already called.
 *
 * side affect:     Stack FSM is executed.
 */
void StackTask(void)
{
    static NODE_INFO remoteNode;
    static WORD dataCount;

#if defined(STACK_USE_ICMP)
    static BYTE data[MAX_ICMP_DATA_LEN];
    static WORD ICMPId;
    static WORD ICMPSeq;
#endif
    IP_ADDR destIP;     //Is filled with the Destination IP address contained in the IP header


    union
    {
        BYTE MACFrameType;
        BYTE IPFrameType;
        ICMP_CODE ICMPCode;
    } type;


    BOOL lbContinue;


    lbContinue = TRUE;
    while( lbContinue )
    {
        lbContinue = FALSE;

        switch(smStack)
        {
        case SM_STACK_IDLE:
        case SM_STACK_MAC:

            //Check if the MAC RX Buffer has any data, and if it does, read the header.
            //Get the next header from the NIC. The node who sent it's address will be copied to 
            //'remoteNode.MACAddr'.
            if ( !MACRxbufGetHdr(&remoteNode.MACAddr, &type.MACFrameType) )
            {
                //Header was NOT read if MACRxbufGetHdr returned FALSE

                #if defined(STACK_USE_DHCP)
                    //If DHCP is enabled AND MAC is not linked yet, set our IP to 0
                    if (STACK_IS_DHCP_ENABLED)
                    {
                        if ( !MACIsLinked() )
                        {
                            #if (DEBUG_STACKTSK >= LOG_INFO)
                            debugPutMsg(1); //@mxd:1:DHCP Enabled but MAC not linked yet - set IP to 0.0.0.0
                            #endif
                            
                            //IP address must be 0.0.0.0 before DHCP has obtained a valid IP address
                            MY_IP_BYTE1 = 0;
                            MY_IP_BYTE2 = 0;
                            MY_IP_BYTE3 = 0;
                            MY_IP_BYTE4 = 0;

                            stackFlags.bits.bInConfigMode = TRUE;
                            DHCPReset();
                        }
                    }
                #endif

                break;
            }

            lbContinue = TRUE;
            if ( type.MACFrameType == MAC_IP ) {
                smStack = SM_STACK_IP;
                #if (DEBUG_STACKTSK >= LOG_DEBUG)
                debugPutMsg(2); //@mxd:2:Reading MAC IP header
                #endif
            }
            else if ( type.MACFrameType == MAC_ARP ) {
                smStack = SM_STACK_ARP;
                #if (DEBUG_STACKTSK >= LOG_DEBUG)
                debugPutMsg(3); //@mxd:3:Reading MAC ARP header
                #endif
            }
            else {
                MACRxbufDiscard();
                #if (DEBUG_STACKTSK >= LOG_WARN)
                debugPutMsg(4); //@mxd:4:Unknown MAC header read, MAC Frame Type = 0x%x
                debugPutByteHex(type.MACFrameType);
                #endif
            }
            break;

        case SM_STACK_ARP:
            lbContinue = FALSE;
            if ( ARPProcess() )
                smStack = SM_STACK_IDLE;
            break;

        case SM_STACK_IP:
            if ( IPGetHeader(&destIP,  /* Get Destination IP Address as received in IP header */
                             &remoteNode,
                             &type.IPFrameType,
                             &dataCount) )
            {
                lbContinue = TRUE;
                if ( type.IPFrameType == IP_PROT_ICMP )
                {
                    smStack = SM_STACK_ICMP;

#if defined(STACK_USE_IP_GLEANING)
                    if ( stackFlags.bits.bInConfigMode )
                    {
                        /*
                         * Accoriding to "IP Gleaning" procedure, when we receive an ICMP packet
                         * with a valid IP address while we are still in configuration mode,
                         * accept that address as ours and conclude configuration mode.
                         */
                        if ( destIP.Val != 0xffffffff )
                        {
                            stackFlags.bits.bInConfigMode    = FALSE;
                            MY_IP_BYTE1                 = destIP.v[0];
                            MY_IP_BYTE2                 = destIP.v[1];
                            MY_IP_BYTE3                 = destIP.v[2];
                            MY_IP_BYTE4                 = destIP.v[3];

#if defined(STACK_USE_DHCP)
                            /*
                             * If DHCP and IP gleaning is enabled at the
                             * same time, we must ensuer that once we have
                             * IP address through IP gleaning, we abort
                             * any pending DHCP requests and do not renew
                             * any new DHCP configuration.
                             */
                            DHCPAbort();
#endif
                        }
                    }
#endif
                }

#if defined(STACK_USE_TCP)
                else if ( type.IPFrameType == IP_PROT_TCP )
                    smStack = SM_STACK_TCP;
#endif

#if defined(STACK_USE_UDP)
                else if ( type.IPFrameType == IP_PROT_UDP )
                    smStack = SM_STACK_UDP;
#endif

                else
                {
                    lbContinue = FALSE;
                    MACRxbufDiscard();

                    smStack = SM_STACK_IDLE;
                }
            }
            else
            {
                MACRxbufDiscard();
                smStack = SM_STACK_IDLE;
            }
            break;

#if defined(STACK_USE_UDP)
        case SM_STACK_UDP:
            //tempLocalIP.v[0] = MY_IP_BYTE1;
            //tempLocalIP.v[1] = MY_IP_BYTE2;
            //tempLocalIP.v[2] = MY_IP_BYTE3;
            //tempLocalIP.v[3] = MY_IP_BYTE4;
            if ( UDPProcess(&remoteNode, &destIP, dataCount) )
                smStack = SM_STACK_IDLE;
            lbContinue = FALSE;
            break;
#endif

#if defined(STACK_USE_TCP)
        case SM_STACK_TCP:
            //tempLocalIP.v[0] = MY_IP_BYTE1;
            //tempLocalIP.v[1] = MY_IP_BYTE2;
            //tempLocalIP.v[2] = MY_IP_BYTE3;
            //tempLocalIP.v[3] = MY_IP_BYTE4;
            //Will return TRUE if TCPProcess finished it's task, else FALSE
            if ( TCPProcess(&remoteNode, &destIP, dataCount) )
                smStack = SM_STACK_IDLE;
            lbContinue = FALSE;
            break;
#endif

        case SM_STACK_ICMP:
            smStack = SM_STACK_IDLE;

#if defined(STACK_USE_ICMP)
            if ( dataCount <= (MAX_ICMP_DATA_LEN+9) )
            {
                if ( ICMPGet(&type.ICMPCode,
                             data,
                             (BYTE*)&dataCount,
                             &ICMPId,
                             &ICMPSeq) )
                {
                    if ( type.ICMPCode == ICMP_ECHO_REQUEST )
                    {
                        lbContinue = TRUE;
                        smStack = SM_STACK_ICMP_REPLY;
                    }
                    else
                    {
                        smStack = SM_STACK_IDLE;
                    }
                }
                else
                {
                    smStack = SM_STACK_IDLE;
                }
            }
#endif
            MACRxbufDiscard();
            break;

#if defined(STACK_USE_ICMP)
        case SM_STACK_ICMP_REPLY:
            if ( ICMPIsTxReady() )
            {
                ICMPPut(&remoteNode,
                        ICMP_ECHO_REPLY,
                        data,
                        (BYTE)dataCount,
                        ICMPId,
                        ICMPSeq);

                smStack = SM_STACK_IDLE;
            }
            break;
#endif

        }

    }

#if defined(STACK_USE_TCP)
    // Perform timed TCP FSM.
    TCPTick();
#endif


#if defined(STACK_USE_DHCP)
    /*
     * DHCP must be called all the time even after IP configuration is discovered.
     * DHCP has to account lease expiration time and renew the configuration time.
     */
    DHCPTask();

    if ( DHCPIsBound() )
        stackFlags.bits.bInConfigMode = FALSE;
#endif

    //Perform routine MAC tasks
    MACTask();
}

