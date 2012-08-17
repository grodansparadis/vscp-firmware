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
 * 2006-09-08, David Hosken (DH): Implemented changes from Microchip TCP/IP stack 3.75
 *********************************************************************/

#define THIS_IS_STACK_TASK

#include "projdefs.h"
#include "net\checkcfg.h"
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
#define debugPutMsg(msgCode) debugPut2Bytes(0xE5, msgCode)


/////////////////////////////////////////////////
//Modtronix TCP/IP Stack Version number
// - n = major part, and can be 1 or 2 digets long
// - mm is minor part, and must be 2 digets long!
ROM char STACK_VER_STR[] = "V2.51";

typedef union _STACK_FLAGS
{
    struct
    {
        //Indicates if DHCP is bound yet. This flag is set on reset. It is cleared when DHCP is bound.
        //It is reset again if DHCP is enabled, and the link is lost
        unsigned char bInConfigMode : 1;
        
        //Indicate if DHCP has already been reset
        unsigned char bDHCPReset : 1;
    } bits;
    BYTE Val;
} STACK_FLAGS;

STACK_FLAGS stackFlags;

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
    SM_STACK_UDP,
    SM_STACK_VSCP
} SM_STACK;
static SM_STACK smStack;

NODE_INFO remoteNode;


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
    
    stackFlags.bits.bDHCPReset = FALSE;
#endif

    MACInit();

    ARPInit();

#if defined(STACK_USE_UDP)
    UDPInit();
#endif

#if defined(STACK_USE_TCP)
    TCPInit();
#endif

#if defined(STACK_USE_DHCP)
    if(!STACK_IS_DHCP_ENABLED)
    {
        DHCPDisable();
    }
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

    #if defined(STACK_USE_DHCP)
    // Normally, an application would not include  DHCP module
    // if it is not enabled. But in case some one wants to disable
    // DHCP module at run-time, remember to not clear our IP
    // address if link is removed.
    if(STACK_IS_DHCP_ENABLED)
    {
        //If MAC not linked and DHCP not been reset yet
        if(!MACIsLinked() && !stackFlags.bits.bDHCPReset)
        {
            //AppConfig.MyIPAddr.v[0] = MY_DEFAULT_IP_ADDR_BYTE1;
            //AppConfig.MyIPAddr.v[1] = MY_DEFAULT_IP_ADDR_BYTE2;
            //AppConfig.MyIPAddr.v[2] = MY_DEFAULT_IP_ADDR_BYTE3;
            //AppConfig.MyIPAddr.v[3] = MY_DEFAULT_IP_ADDR_BYTE4;
            AppConfig.MyIPAddr.v[0] = MY_STATIC_IP_BYTE1;
            AppConfig.MyIPAddr.v[1] = MY_STATIC_IP_BYTE2;
            AppConfig.MyIPAddr.v[2] = MY_STATIC_IP_BYTE3;
            AppConfig.MyIPAddr.v[3] = MY_STATIC_IP_BYTE4;

            AppConfig.MyMask.v[0] = MY_DEFAULT_MASK_BYTE1;
            AppConfig.MyMask.v[1] = MY_DEFAULT_MASK_BYTE2;
            AppConfig.MyMask.v[2] = MY_DEFAULT_MASK_BYTE3;
            AppConfig.MyMask.v[3] = MY_DEFAULT_MASK_BYTE4;

            DHCPFlags.bits.bDHCPServerDetected = FALSE;

            stackFlags.bits.bInConfigMode = TRUE;

            stackFlags.bits.bDHCPReset = TRUE;
            DHCPReset();
        }

        // DHCP must be called all the time even after IP configuration is
        // discovered.
        // DHCP has to account lease expiration time and renew the configuration
        // time.
        DHCPTask();

        if(DHCPIsBound()) {
            stackFlags.bits.bInConfigMode = FALSE;
            stackFlags.bits.bDHCPReset = FALSE;
        }
    }
    #endif

    do
    {
        lbContinue = FALSE;

        switch(smStack)
        {
        case SM_STACK_IDLE:
        case SM_STACK_MAC:

            //debugPutGenRomStr(2, (ROM char*)"1");     //@mxd:2:%s

            //Check if the MAC RX Buffer has any data, and if it does, read the header.
            //Get the next header from the NIC. The node who sent it's address will be copied to 
            //'remoteNode.MACAddr'.
            //Header was NOT read if MACGetHeader returned FALSE
            if ( !MACGetHeader(&remoteNode.MACAddr, &type.MACFrameType) )
            {
                break;      //case SM_STACK_IDLE:  AND   case SM_STACK_MAC:
            }

            //debugPutGenRomStr(2, (ROM char*)"4");     //@mxd:2:%s

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
            else if ( type.MACFrameType == MAC_VSCP ) {
	        	smStack = SM_STACK_VSCP;
                #if (DEBUG_STACKTSK >= LOG_DEBUG)
                debugPutMsg(4); //@mxd:4:Reading MAC VSCP header
                #endif
	        } 
            else {
                MACDiscardRx(); //Discard the contents of the current RX buffer
                #if (DEBUG_STACKTSK >= LOG_WARN)
                debugPutMsg(5); //@mxd:5:Unknown MAC header read, MAC Frame Type = 0x%x
                debugPutByteHex(type.MACFrameType);
                #endif
            }
            break;      //case SM_STACK_IDLE:  AND   case SM_STACK_MAC:

        case SM_STACK_ARP:
            if ( ARPProcess() )
                smStack = SM_STACK_IDLE;
            //lbContinue = FALSE;   //Removed in latest Microchip TCP/IP stack
            break;
            
        case SM_STACK_VSCP:
        	smStack = SM_STACK_IDLE;
        	MACDiscardRx();
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
                    if(stackFlags.bits.bInConfigMode && STACK_IS_DHCP_ENABLED)
                    {
                        // Accoriding to "IP Gleaning" procedure,
                        // when we receive an ICMP packet with a valid
                        // IP address while we are still in configuration
                        // mode, accept that address as ours and conclude
                        // configuration mode.
                        if ( destIP.Val != 0xffffffff )
                        {
                            stackFlags.bits.bInConfigMode    = FALSE;
                            MY_IP_BYTE1                 = destIP.v[0];
                            MY_IP_BYTE2                 = destIP.v[1];
                            MY_IP_BYTE3                 = destIP.v[2];
                            MY_IP_BYTE4                 = destIP.v[3];
                            myDHCPBindCount--;
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

                else    // Unknown/unsupported higher level protocol
                {
                    lbContinue = FALSE;
                    MACDiscardRx(); //Discard the contents of the current RX buffer

                    smStack = SM_STACK_IDLE;
                }
            }
            else    // Improper IP header version or checksum
            {
                MACDiscardRx(); //Discard the contents of the current RX buffer
                smStack = SM_STACK_IDLE;
            }
            break;      //case SM_STACK_IP:

#if defined(STACK_USE_UDP)
        case SM_STACK_UDP:
            if ( UDPProcess(&remoteNode, &destIP, dataCount) )
                smStack = SM_STACK_IDLE;
            //lbContinue = FALSE;   //Removed in latest Microchip TCP/IP stack
            break;      //case SM_STACK_UDP:
#endif

#if defined(STACK_USE_TCP)
        case SM_STACK_TCP:
            if ( TCPProcess(&remoteNode, &destIP, dataCount) )  //Will return TRUE if TCPProcess finished it's task, else FALSE
                smStack = SM_STACK_IDLE;
            //lbContinue = FALSE;   //Removed in latest Microchip TCP/IP stack
            break;      //case SM_STACK_TCP:
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
                }
            }
#endif
            MACDiscardRx(); //Discard the contents of the current RX buffer
            break;          //case SM_STACK_ICMP:

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
            break;          //case SM_STACK_ICMP_REPLY:
#endif

        }       //switch(smStack)

        FAST_USER_PROCESS();
    } while(lbContinue);

#if defined(STACK_USE_TCP)
    // Perform timed TCP FSM.
    TCPTick();
#endif

    //debugPutGenRomStr(2, (ROM char*)"MACTask");     //@mxd:2:%s

    //Perform routine MAC tasks
    MACTask();
}

