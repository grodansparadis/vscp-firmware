/*********************************************************************
 *
 *                  ARP Server Module for Modtronix TCP/IP Stack
 *
 *********************************************************************
 * FileName:        ARPTsk.c
 * Dependencies:    compiler.h
 *                  string.h
 *                  ARP.h
 *                  ARPTsk.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.35PL3 or higher
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
 * HiTech PICC18 Compiler Options excluding device selection:
 *                  -FAKELOCAL -G -Zg -E -C
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     8/20/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 * 2006-09-08, David Hosken (DH): Implemented changes from Microchip TCP/IP stack 3.75
 ********************************************************************/

#include <string.h>

#include "projdefs.h"
#include "net\checkcfg.h"
#include "net\arptsk.h"
#include "net\arp.h"

/*
 * ARP Task FSM States
 */
typedef enum _ARP_STATE
{
    SM_ARP_IDLE,
    SM_ARP_REPLY
} ARP_STATE;


/*
 * This ARP task caches one ARP response.
 */
static ARP_STATE smARP;

#ifdef STACK_CLIENT_MODE
static NODE_INFO Cache; //This Cache is used 
#endif

/**
 * ARP Cache is initialized.
 * @return
 */
void ARPInit(void)
{
    smARP = SM_ARP_IDLE;

#ifdef STACK_CLIENT_MODE
    Cache.MACAddr.v[0] = 0xff;
    Cache.MACAddr.v[1] = 0xff;
    Cache.MACAddr.v[2] = 0xff;
    Cache.MACAddr.v[3] = 0xff;
    Cache.MACAddr.v[4] = 0xff;
    Cache.MACAddr.v[5] = 0xff;

    Cache.IPAddr.Val = 0x0;
#endif
}

/**
 * ARP FSM is executed.
 * @preCondition ARP packet is ready in MAC buffer.
 * @return
 */
BOOL ARPProcess(void)
{
    NODE_INFO remoteNode;
    BYTE opCode;

    //If a valid ARP packet addressed to us was received, true is returned and
    //opcode (ARP_REPLY or ARP_REQUEST) is writen to given opCode buffer.
    switch(smARP)
    {
    case SM_ARP_IDLE:
        if ( !ARPGet(&remoteNode, &opCode) )
            break;

        if ( opCode == ARP_REPLY )  //We have received an ARP reply resolving a requested IP address's MAC address
        {
#ifdef STACK_CLIENT_MODE
            Cache.MACAddr = remoteNode.MACAddr;
            Cache.IPAddr.Val = remoteNode.IPAddr.Val;
#endif
            break;
        }
        else
            smARP = SM_ARP_REPLY;

    default:
        if(ARPPut(&remoteNode, ARP_REPLY))
        {
            smARP = SM_ARP_IDLE;
        }
        else
            return FALSE;
        break;
    }
    return TRUE;
}

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
#ifdef STACK_CLIENT_MODE
void ARPResolve(IP_ADDR *IPAddr)
{
    NODE_INFO remoteNode;

    remoteNode.IPAddr = *IPAddr;

    ARPPut(&remoteNode, ARP_REQUEST);
}
#endif

/**
 * This function is used to retrieve the MAC Address for a given IP
 * address. This information will be received via ARP after sending an ARP
 * request over the network. If we have a destination IP address, but not
 * it's MAC address, we will send an ARP request, and wait for an ARP reply.
 * As soon as we receive the reply, the requested MAC address can be retrieve
 * via this function.
 * Note:            This function is available only when STACK_CLIENT_MODE is defined.
 *
 * @param IPAddr    IPAddress to be resolved
 * @param MACAddr   Buffer to hold corresponding MAC address
 * @return          TRUE if given IP address has been resolved<br>
 *                  FALSE otherwise
 */
#ifdef STACK_CLIENT_MODE
BOOL ARPIsResolved(IP_ADDR *IPAddr, MAC_ADDR *MACAddr)
{
    if ( (Cache.IPAddr.Val == IPAddr->Val) ||
         (Cache.IPAddr.v[0] == MY_GATE_BYTE1 &&
          Cache.IPAddr.v[1] == MY_GATE_BYTE2 &&
          Cache.IPAddr.v[2] == MY_GATE_BYTE3 &&
          Cache.IPAddr.v[3] == MY_GATE_BYTE4) )
    {
        *MACAddr = Cache.MACAddr;
        return TRUE;
    }
    return FALSE;
}
#endif

