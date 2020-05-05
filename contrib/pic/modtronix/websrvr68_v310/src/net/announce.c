/** !!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!
 *  This file has not been ported to the Modtronix TCP/IP stack yet!
 */
 
/*********************************************************************
 *
 *                  Announce Module for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        Announce.c
 * Dependencies:    UDP.h
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F
 * Complier:        Microchip C18 v3.02 or higher
 *                    Microchip C30 v2.01 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * This software is owned by Microchip Technology Inc. ("Microchip") 
 * and is supplied to you for use exclusively as described in the 
 * associated software agreement.  This software is protected by 
 * software and other intellectual property laws.  Any use in 
 * violation of the software license may subject the user to criminal 
 * sanctions as well as civil liability.  Copyright 2006 Microchip
 * Technology Inc.  All rights reserved.
 *
 * This software is provided "AS IS."  MICROCHIP DISCLAIMS ALL 
 * WARRANTIES, EXPRESS, IMPLIED, STATUTORY OR OTHERWISE, NOT LIMITED 
 * TO MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
 * INFRINGEMENT.  Microchip shall in no event be liable for special, 
 * incidental, or consequential damages.
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder     10/7/04    Original
 * Howard Schlunder        2/9/05    Simplified MAC address to text 
 *                                conversion logic
 * Howard Schlunder        2/14/05    Fixed subnet broadcast calculation
 ********************************************************************/
#define THIS_IS_ANNOUNCE

#include "projdefs.h"
#include "net\checkcfg.h"
#include "net\announce.h"
#include "net\nbns.h"
#include "net\stacktsk.h"
#include "debug.h"

#if defined(STACK_USE_ANNOUNCE)

#include "net\udp.h"
#include "net\helpers.h"

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xDD, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xDD, msgCode, msgStr)

//Defines
#define ANNOUNCE_PORT    30303

extern NODE_INFO remoteNode;

/*********************************************************************
 * Function:        void AnnounceIP(void)
 *
 * PreCondition:    Stack is initialized()
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        AnnounceIP opens a UDP socket and transmits a 
 *                    broadcast packet to port 30303.  If a computer is
 *                    on the same subnet and a utility is looking for 
 *                    packets on the UDP port, it will receive the 
 *                    broadcast.  For this application, it is used to 
 *                    announce the change of this board's IP address.
 *                    The messages can be viewed with the MCHPDetect.exe
 *                    program.
 *
 * Note:            A UDP socket must be available before this 
 *                    function is called.  It is freed at the end of 
 *                    the function.  MAX_UDP_SOCKETS may need to be 
 *                    increased if other modules use UDP sockets.
 ********************************************************************/
void AnnounceIP(void)
{
    UDP_SOCKET    MySocket;
    NODE_INFO    Remote;
    BYTE         i;
    
    // Set the socket's destination to be a broadcast over our IP 
    // subnet
    // Set the MAC destination to be a broadcast
    memset(&Remote, 0xFF, sizeof(Remote));
    
    // Open a UDP socket for outbound transmission
    MySocket = UDPOpen(2860, &Remote, ANNOUNCE_PORT);
    
    // Abort operation if no UDP sockets are available
    // If this ever happens, incrementing MAX_UDP_SOCKETS in 
    // StackTsk.h may help (at the expense of more global memory 
    // resources).
    if( MySocket == INVALID_UDP_SOCKET ) {
        #if (DEBUG_ANNOUNCE >= LOG_ERROR)
        debugPutMsg(1); //@mxd:1:Could not open UDP socket
        #endif

        return;
    }
    
    // Make certain the socket can be written to
    while( !UDPIsPutReady(MySocket) ) FAST_USER_PROCESS();
    
    // Begin sending our MAC address in human readable form.
    // The MAC address theoretically could be obtained from the 
    // packet header when the computer receives our UDP packet, 
    // however, in practice, the OS will abstract away the useful
    // information and it would be difficult to obtain.  It also 
    // would be lost if this broadcast packet were forwarded by a
    // router to a different portion of the network (note that 
    // broadcasts are normally not forwarded by routers).
    for(i=0; i < 15; i++)   //First 15 (0-14) characters are NetBIOS name, 16th character is 0x00
    {
        UDPPut(NETBIOS_NAME_GETCHAR(i));
    }
    UDPPut('\r');
    UDPPut('\n');

    // Convert the MAC address bytes to hex (text) and then send it
    i = 0;
    while(1)
    {
        UDPPut(btohexa_high(AppConfig.MyMACAddr.v[i]));
        UDPPut(btohexa_low(AppConfig.MyMACAddr.v[i]));
        if(++i == 6)
            break;
        UDPPut('-');
    }
    UDPPut('\r');
    UDPPut('\n');

    // Send some other human readable information.
    UDPPut('D');
    UDPPut('H');
    UDPPut('C');
    UDPPut('P');
    UDPPut('/');
    UDPPut('P');
    UDPPut('o');
    UDPPut('w');
    UDPPut('e');
    UDPPut('r');
    UDPPut(' ');
    UDPPut('e');
    UDPPut('v');
    UDPPut('e');
    UDPPut('n');
    UDPPut('t');
    UDPPut(' ');
    UDPPut('o');
    UDPPut('c');
    UDPPut('c');
    UDPPut('u');
    UDPPut('r');
    UDPPut('r');
    UDPPut('e');
    UDPPut('d');
    
    // Send the packet
    UDPFlush();
    
    // Close the socket so it can be used by other modules
    UDPClose(MySocket);
}

void DiscoveryTask(void)
{
    static enum {
        DISCOVERY_HOME = 0,
        DISCOVERY_LISTEN,
        DISCOVERY_REQUEST_RECEIVED,
        DISCOVERY_DISABLED
    } DiscoverySM = DISCOVERY_HOME;

    static UDP_SOCKET    MySocket;
    BYTE                 i;
    
    switch(DiscoverySM)
    {
        case DISCOVERY_HOME:
            // Open a UDP socket for inbound and outbound transmission
            // Since we expect to only receive broadcast packets and 
            // only send unicast packets directly to the node we last 
            // received from, the remote NodeInfo parameter can be anything
            MySocket = UDPOpen(ANNOUNCE_PORT, NULL, ANNOUNCE_PORT);

            if(MySocket == INVALID_UDP_SOCKET)
                return;
            else
                DiscoverySM++;
            break;

        case DISCOVERY_LISTEN:
            // Do nothing if no data is waiting
            if(!UDPIsGetReady(MySocket))
                return;
            
            // See if this is a discovery query or reply
            UDPGet(&i);
            UDPDiscard();
            if(i != 'D')
                return;
            
            // We received a discovery request, reply
            DiscoverySM++;
            break;

        case DISCOVERY_REQUEST_RECEIVED:
            if(!UDPIsPutReady(MySocket))
                return;

            // Begin sending our MAC address in human readable form.
            // The MAC address theoretically could be obtained from the 
            // packet header when the computer receives our UDP packet, 
            // however, in practice, the OS will abstract away the useful
            // information and it would be difficult to obtain.  It also 
            // would be lost if this broadcast packet were forwarded by a
            // router to a different portion of the network (note that 
            // broadcasts are normally not forwarded by routers).
            for(i=0; i < 15; i++)   //First 15 (0-14) characters are NetBIOS name, 16th character is 0x00
            {
                UDPPut(NETBIOS_NAME_GETCHAR(i));
            }
            UDPPut('\r');
            UDPPut('\n');
        
            // Convert the MAC address bytes to hex (text) and then send it
            i = 0;
            while(1)
            {
                UDPPut(btohexa_high(AppConfig.MyMACAddr.v[i]));
                UDPPut(btohexa_low(AppConfig.MyMACAddr.v[i]));
                if(++i == 6)
                    break;
                UDPPut('-');
            }
            UDPPut('\r');
            UDPPut('\n');

            // Change the destination to the unicast address of the last received packet
            memcpy((void*)&UDPSocketInfo[MySocket].remoteNode, (const void*)&remoteNode, sizeof(remoteNode));
        
            // Send the packet
            UDPFlush();

            // Listen for other discovery requests
            DiscoverySM = DISCOVERY_LISTEN;
            break;

        case DISCOVERY_DISABLED:
            break;
    }    

}


#endif //#if defined(STACK_USE_ANNOUNCE)
