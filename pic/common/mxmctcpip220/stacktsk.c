/*********************************************************************
 *
 *               Microchip TCP/IP Stack FSM Implementation on PIC18
 *
 *********************************************************************
 * FileName:        StackTsk.c
 * Dependencies:    StackTsk.H
 *                  ARPTsk.h
 *                  MAC.h
 *                  IP.h
 *                  ICMP.h
 *                  Tcp.h
 *                  http.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
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
 * HiTech PICC18 Compiler Options excluding device selection:
 *                  -FAKELOCAL -G -O -Zg -E -C
 *
 * Author               Date        Comment
 ********************************************************************
 * 2004/10/02 David Hosken - Created from Microchip TCP/IP stack v2.20.04.01
 ********************************************************************/

#define STACK_INCLUDE

#include "stacktsk.h"
#include "debug.h"
#include "appcfg.h"
#include "arptsk.h"
#include "mac.h"
#include "ip.h"

#if defined(STACK_USE_TCP)
#include "tcp.h"
#endif

#if defined(STACK_USE_HTTP_SERVER)
#include "http.h"
#endif

#if defined(STACK_USE_ICMP)
    #include "icmp.h"
#endif

#if defined(STACK_USE_UDP)
    #include "udp.h"
#endif

#if defined(STACK_USE_DHCP)
    #include "dhcp.h"
#endif


#if (DEBUG_STACKTSK > LOG_OFF)
static ROM char DebugMsgReadMacIpHdr[] = "\rS:MacIpHdr";
static ROM char DebugMsgReadMacArpHdr[] = "\rS:MacArpHdr";
static ROM char DebugMsgReadMacUnknownHdr[] = "\rS:MacUknHdr";
static ROM char DebugMsgEndline[] = "\r";
#endif


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

/*********************************************************************
 * Function:        void StackInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Stack and its componentns are initialized
 *
 * Side Effects:    None
 *
 * Note:            This function must be called before any of the
 *                  stack or its component routines be used.
 *
 ********************************************************************/
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

/*********************************************************************
 * Function:        void StackTask(void)
 *
 * PreCondition:    StackInit() is already called.
 *
 * Input:           None
 *
 * Output:          Stack FSM is executed.
 *
 * Side Effects:    None
 *
 * Note:            This FSM checks for new incoming packets,
 *                  and routes it to appropriate stack components.
 *                  It also performs timed operations.
 *
 *                  This function must be called periodically called
 *                  to make sure that timely response.
 *
 ********************************************************************/
void StackTask(void)
{
    static NODE_INFO remoteNode;
    static WORD dataCount;

#if defined(STACK_USE_ICMP)
    static BYTE data[MAX_ICMP_DATA_LEN];
    static WORD ICMPId;
    static WORD ICMPSeq;
#endif
    IP_ADDR tempLocalIP;


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
                    if ( !MACIsLinked() )
                    {
                        MY_IP_BYTE1 = 0;
                        MY_IP_BYTE2 = 0;
                        MY_IP_BYTE3 = 0;
                        MY_IP_BYTE4 = 0;

                        stackFlags.bits.bInConfigMode = TRUE;
                        DHCPReset();
                    }
                #endif

                break;
            }

            lbContinue = TRUE;
            if ( type.MACFrameType == MAC_IP ) {
                smStack = SM_STACK_IP;
                #if (DEBUG_STACKTSK >= LOG_INFO)
                debugPutRomString(DebugMsgReadMacIpHdr);
                #endif
            }
            else if ( type.MACFrameType == MAC_ARP ) {
                smStack = SM_STACK_ARP;
                #if (DEBUG_STACKTSK >= LOG_INFO)
                debugPutRomString(DebugMsgReadMacArpHdr);
                #endif
            }
            else {
                MACRxbufDiscard();
                #if (DEBUG_STACKTSK >= LOG_WARN)
                debugPutRomString(DebugMsgReadMacUnknownHdr);
                #endif
            }
            break;

        case SM_STACK_ARP:
            lbContinue = FALSE;
            if ( ARPProcess() )
                smStack = SM_STACK_IDLE;
            break;

        case SM_STACK_IP:
            if ( IPGetHeader(&tempLocalIP,
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
                         * Accoriding to "IP Gleaning" procedure,
                         * when we receive an ICMP packet with a valid
                         * IP address while we are still in configuration
                         * mode, accept that address as ours and conclude
                         * configuration mode.
                         */
                        if ( tempLocalIP.Val != 0xffffffff )
                        {
                            stackFlags.bits.bInConfigMode    = FALSE;
                            MY_IP_BYTE1                 = tempLocalIP.v[0];
                            MY_IP_BYTE2                 = tempLocalIP.v[1];
                            MY_IP_BYTE3                 = tempLocalIP.v[2];
                            MY_IP_BYTE4                 = tempLocalIP.v[3];

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
            if ( UDPProcess(&remoteNode, &tempLocalIP, dataCount) )
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
            if ( TCPProcess(&remoteNode, &tempLocalIP, dataCount) )
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
     * DHCP must be called all the time even after IP configuration is
     * discovered.
     * DHCP has to account lease expiration time and renew the configuration
     * time.
     */
    DHCPTask();

    if ( DHCPIsBound() )
        stackFlags.bits.bInConfigMode = FALSE;

#endif
}

