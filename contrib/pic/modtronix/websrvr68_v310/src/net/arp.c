/*********************************************************************
 *
 *                  ARP Module for Modtronix TCP/IP Stack
 *
 *********************************************************************
 * FileName:        ARP.c
 * Dependencies:    string.h
 *                  stacktsk.h
 *                  helpers.h
 *                  arp.h
 *                  mac.h
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
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     5/1/01  Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 * 2006-09-08, David Hosken (DH): Implemented changes from Microchip TCP/IP stack 3.75
 ********************************************************************/
#include <string.h>

#include "projdefs.h"
#include "net\checkcfg.h"
#include "net\arp.h"
#include "net\helpers.h"

/*
 * ARP Operation codes.
 */
#define ARP_OPERATION_REQ       0x01ul
#define ARP_OPERATION_RESP      0x02ul

/*
 * ETHERNET packet type as defined by IEEE 802.3
 */
#define HW_ETHERNET             (0x0001ul)
#define ARP_IP                  (0x0800ul)


/*
 * ARP packet
 */
typedef struct _ARP_PACKET
{
    WORD        HardwareType;
    WORD        Protocol;
    BYTE        MACAddrLen;
    BYTE        ProtocolLen;
    WORD        Operation;
    MAC_ADDR    SenderMACAddr;
    IP_ADDR     SenderIPAddr;
    MAC_ADDR    TargetMACAddr;
    IP_ADDR     TargetIPAddr;
} ARP_PACKET;

// Helper function
static void SwapARPPacket(ARP_PACKET *p);


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
BOOL ARPGet(NODE_INFO *remote, BYTE *opCode)
{
    ARP_PACKET packet;

    MACGetArray((BYTE*)&packet, sizeof(packet));    //Read ARP packet into buffer. The data is read from the current MAC RX Buffer

    MACDiscardRx(); //Discard the contents of the current RX buffer

    SwapARPPacket(&packet);

    if ( packet.HardwareType != HW_ETHERNET     ||
         packet.MACAddrLen != sizeof(MAC_ADDR)  ||
         packet.ProtocolLen != sizeof(IP_ADDR) )
    {
         return FALSE;
    }

    if ( packet.Operation == ARP_OPERATION_RESP )
    {   
        *opCode = ARP_REPLY;
    }
    else if ( packet.Operation == ARP_OPERATION_REQ )
        *opCode = ARP_REQUEST;
    else
    {
        *opCode = ARP_UNKNOWN;
        return FALSE;
    }

    //Packet was addressed to us
    if ( (packet.TargetIPAddr.v[0] == MY_IP_BYTE1) &&
         (packet.TargetIPAddr.v[1] == MY_IP_BYTE2) &&
         (packet.TargetIPAddr.v[2] == MY_IP_BYTE3) &&
         (packet.TargetIPAddr.v[3] == MY_IP_BYTE4) )
    {
        remote->MACAddr     = packet.SenderMACAddr;
        remote->IPAddr      = packet.SenderIPAddr;
        return TRUE;
    }
    else
        return FALSE;
}


/**
 * Send an ARP message.
 *
 * @preCondition    MACIsTxReady() == TRUE
 *
 * @param remote    Remote node info
 * @param opCode    ARP op code to send
 *
 * @return          TRUE - The ARP packet was generated properly
 *                    FALSE - Unable to allocate a TX buffer
 */
BOOL ARPPut(NODE_INFO *remote, BYTE opCode)
{
    ARP_PACKET packet;
    BUFFER MyTxBuffer;
    MyTxBuffer = MACGetTxBuffer(TRUE);
    
    // Do not respond if there is no room to generate the ARP reply
    if(MyTxBuffer == INVALID_BUFFER)
        return FALSE;

    MACSetTxBuffer(MyTxBuffer, 0);


    packet.HardwareType             = HW_ETHERNET;
    packet.Protocol                 = ARP_IP;
    packet.MACAddrLen               = sizeof(MAC_ADDR);
    packet.ProtocolLen              = sizeof(IP_ADDR);

    if ( opCode == ARP_REQUEST )
    {
        packet.Operation            = ARP_OPERATION_REQ;
        packet.TargetMACAddr.v[0]   = 0xff;
        packet.TargetMACAddr.v[1]   = 0xff;
        packet.TargetMACAddr.v[2]   = 0xff;
        packet.TargetMACAddr.v[3]   = 0xff;
        packet.TargetMACAddr.v[4]   = 0xff;
        packet.TargetMACAddr.v[5]   = 0xff;
    }
    else
    {
        packet.Operation            = ARP_OPERATION_RESP;
        packet.TargetMACAddr        = remote->MACAddr;
    }

    packet.SenderMACAddr.v[0]       = MY_MAC_BYTE1;
    packet.SenderMACAddr.v[1]       = MY_MAC_BYTE2;
    packet.SenderMACAddr.v[2]       = MY_MAC_BYTE3;
    packet.SenderMACAddr.v[3]       = MY_MAC_BYTE4;
    packet.SenderMACAddr.v[4]       = MY_MAC_BYTE5;
    packet.SenderMACAddr.v[5]       = MY_MAC_BYTE6;

    packet.SenderIPAddr.v[0]        = MY_IP_BYTE1;
    packet.SenderIPAddr.v[1]        = MY_IP_BYTE2;
    packet.SenderIPAddr.v[2]        = MY_IP_BYTE3;
    packet.SenderIPAddr.v[3]        = MY_IP_BYTE4;


    // Check to see if target is on same subnet, if not, find Gateway MAC.
    // Once we get Gateway MAC, all access to remote host will go through Gateway.
    if (((packet.SenderIPAddr.v[0] ^ remote->IPAddr.v[0]) & MY_MASK_BYTE1) ||
        ((packet.SenderIPAddr.v[1] ^ remote->IPAddr.v[1]) & MY_MASK_BYTE2) ||
        ((packet.SenderIPAddr.v[2] ^ remote->IPAddr.v[2]) & MY_MASK_BYTE3) ||
        ((packet.SenderIPAddr.v[3] ^ remote->IPAddr.v[3]) & MY_MASK_BYTE4) )
    {
        packet.TargetIPAddr.v[0] = MY_GATE_BYTE1;
        packet.TargetIPAddr.v[1] = MY_GATE_BYTE2;
        packet.TargetIPAddr.v[2] = MY_GATE_BYTE3;
        packet.TargetIPAddr.v[3] = MY_GATE_BYTE4;
    }
    else
        packet.TargetIPAddr = remote->IPAddr;

    SwapARPPacket(&packet);

    //Write the Ethernet Header to the MAC's TX buffer. The last parameter (dataLen) is the length
    //of the data to follow.
    MACPutHeader(&packet.TargetMACAddr, MAC_ARP, sizeof(packet));
    MACPutArray((BYTE*)&packet, sizeof(packet));
    MACFlush();

    return TRUE;
}


/**
 * @param p     ARP packet to be swapped.
 */
static void SwapARPPacket(ARP_PACKET *p)
{
    p->HardwareType     = swaps(p->HardwareType);
    p->Protocol         = swaps(p->Protocol);
    p->Operation        = swaps(p->Operation);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// SendTestVSCPPacket
//

BOOL SendTestVSCPPacket( void ) 
{
    BUFFER MyTxBuffer;
    BYTE buf[ 46 ];
    MAC_ADDR TargetMACAddr;  
    MyTxBuffer = MACGetTxBuffer( TRUE );
    
    // Nill it
    memset( buf, 0, sizeof( buf ) );
    
    // Do not respond if there is no room to generate the ARP reply
    if ( MyTxBuffer == INVALID_BUFFER ) {
        return FALSE;
    }    

    MACSetTxBuffer( MyTxBuffer, 0 );
    
    // Destination address - Always MAC broadcast address
    TargetMACAddr.v[ 0 ] = 0xff; 
    TargetMACAddr.v[ 1 ] = 0xff;
    TargetMACAddr.v[ 2 ] = 0xff;
    TargetMACAddr.v[ 3 ] = 0xff;
    TargetMACAddr.v[ 4 ] = 0xff;
    TargetMACAddr.v[ 5 ] = 0xff;
    
    // Prepare the VSCP payload
    
    // VSCP head
    buf[ 0 ] = 0x00;
    buf[ 1 ] = 0x00;
    buf[ 2 ] = 0x00;
    buf[ 3 ] = 0x00;
    
    // VSCP sub source address
    buf[ 4 ] = 0x00;
    buf[ 5 ] = 0x00;
    
    // Timestamp
    buf[ 6 ] = 0x00;
    buf[ 7 ] = 0x00;
    buf[ 8 ] = 0x00;
    buf[ 9 ] = 0x00;
    
    // OBID
    buf[ 6 ] = 0x00;
    buf[ 7 ] = 0x00;
    buf[ 8 ] = 0x00;
    buf[ 9 ] = 0x00;
    
    // VSCP class
    buf[ 10 ] = 0x02;	// Class=512 (0x200) - Control
    buf[ 11 ] = 0x00; 
    
    // VSCP type 
    buf[ 12 ] = 0x00;	// Type = 2 (0x02) New node on line / Probe.
    buf[ 13 ] = 0x02;
    
    // Our GUID
    buf[ 13 ] = 0xff;			// Ethernet predefined  GUID
    buf[ 14 ] = 0xff;
    buf[ 15 ] = 0xff;
    buf[ 16 ] = 0xff;
    buf[ 17 ] = 0xff;
    buf[ 18 ] = 0xff;
    buf[ 19 ] = 0xff;
    buf[ 20 ] = 0xfe;
    buf[ 21 ] = MY_MAC_BYTE1; 	// Our MAC address
    buf[ 22 ] = MY_MAC_BYTE2;
    buf[ 23 ] = MY_MAC_BYTE3;
    buf[ 24 ] = MY_MAC_BYTE4;
    buf[ 25 ] = MY_MAC_BYTE5;
    buf[ 26 ] = MY_MAC_BYTE6;
    buf[ 27 ] = 0xaa;			// Sub address	
    buf[ 28 ] = 0x55;
    
    // Write the Ethernet Header to the MAC's TX buffer. The last parameter (dataLen) is the length
    // of the data to follow.
    MACPutHeader( &TargetMACAddr, MAC_VSCP, sizeof( buf ) );
    MACPutArray( buf, sizeof( buf ) );
    MACFlush();

    return TRUE;		
}
