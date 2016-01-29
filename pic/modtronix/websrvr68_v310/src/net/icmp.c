/*********************************************************************
 *
 *                  ICMP Module for Modtronix TCP/IP Stack
 *
 *********************************************************************
 * FileName:        ICMP.C
 * Dependencies:    ICMP.h
 *                  string.h
 *                  stacktsk.h
 *                  Helpers.h
 *                  IP.h
 *                  MAC.h
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
 *                  -FAKELOCAL -G -O -Zg -E -C
 *
 *
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     4/30/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 * 2006-09-08, David Hosken (DH): Implemented changes from Microchip TCP/IP stack 3.75 
 ********************************************************************/

#include <string.h>

#include "projdefs.h"
#include "net\checkcfg.h"
#include "net\icmp.h"
#include "net\helpers.h"
#include "net\ip.h"
#include "net\mac.h"

#if !defined(STACK_USE_ICMP)
#error You have selected to not include ICMP.  Remove this file from \
       your project to reduce code size.
#endif

// ICMP packet definition
typedef struct _ICMP_PACKET
{
    BYTE    Type;
    BYTE    Code;
    WORD    Checksum;
    WORD    Identifier;
    WORD    SequenceNumber;
    BYTE    Data[MAX_ICMP_DATA];
} ICMP_PACKET;
#define ICMP_HEADER_SIZE    (sizeof(ICMP_PACKET) - MAX_ICMP_DATA)

static void SwapICMPPacket(ICMP_PACKET* p);


/**
 * Gets the next ICMP packet
 *
 * @preCondition    MAC buffer contains ICMP type packet.
 *
 * @param code      Buffer to hold ICMP code value
 * @param data      Buffer to hold ICMP data
 * @param len       Buffer to hold ICMP data length
 * @param id        Buffer to hold ICMP id
 * @param seq       Buffer to hold ICMP seq
 *
 * @return          TRUE if valid ICMP packet was received <br>
 *                  FALSE otherwise
 */
BOOL ICMPGet(ICMP_CODE *code,
             BYTE *data,
             BYTE *len,
             WORD *id,
             WORD *seq)
{
    ICMP_PACKET packet;
    WORD CalcChecksum;
    WORD ReceivedChecksum;
#if defined(NON_MCHP_MAC)
    WORD checksums[2];
#endif

    //Read ICMP Header. The data is read from the current MAC RX Buffer
    MACGetArray((BYTE*)&packet, ICMP_HEADER_SIZE);


#if !defined(NON_MCHP_MAC)
    // Calculate the checksum using the Microchip MAC's DMA module
    // The checksum data includes the precomputed checksum in the 
    // header, so a valid packet will always have a checksum of 
    // 0x0000 if the packet is not disturbed.
    ReceivedChecksum = 0x0000;
    CalcChecksum = MACCalcRxChecksum(0+sizeof(IP_HEADER), *len);
#endif

    // Obtain the ICMP data payload
    *len -= ICMP_HEADER_SIZE;
    MACGetArray(data, *len);    //Read ICMP Data. The data is read from the current MAC RX Buffer

#if defined(NON_MCHP_MAC)
    // Calculte the checksum in local memory without hardware help
    ReceivedChecksum = packet.Checksum;
    packet.Checksum = 0;

    checksums[0] = ~CalcIPChecksum((BYTE*)&packet, ICMP_HEADER_SIZE);
    checksums[1] = ~CalcIPChecksum(data, *len);
    
    CalcChecksum = CalcIPChecksum((BYTE*)checksums, 2 * sizeof(WORD));
#endif


    SwapICMPPacket(&packet);

    *code = packet.Type;
    *id = packet.Identifier;
    *seq = packet.SequenceNumber;

    return ( CalcChecksum == ReceivedChecksum );
}

/**
 * A ICMP packet is created and put on MAC.
 *
 * @preCondition ICMPIsTxReady() == TRUE
 *
 * @param remote    Remote node info
 * @param code      ICMP_ECHO_REPLY or ICMP_ECHO_REQUEST
 * @param data      Data bytes
 * @param len       Number of bytes to send
 * @param id        ICMP identifier
 * @param seq       ICMP sequence number
 */
void ICMPPut(NODE_INFO *remote,
             ICMP_CODE code,
             BYTE *data,
             BYTE len,
             WORD id,
             WORD seq)
{
    ICMP_PACKET    packet;
    WORD ICMPLen;
    BUFFER MyTxBuffer;
    MyTxBuffer = MACGetTxBuffer(TRUE);

    // Abort if there is no where in the Ethernet controller to 
    // store this packet.
    if(MyTxBuffer == INVALID_BUFFER)
        return;

    IPSetTxBuffer(MyTxBuffer, 0);


    ICMPLen = ICMP_HEADER_SIZE + (WORD)len;

    packet.Code             = 0;
    packet.Type             = code;
    packet.Checksum         = 0;
    packet.Identifier       = id;
    packet.SequenceNumber   = seq;

    memcpy((void*)packet.Data, (void*)data, len);

    SwapICMPPacket(&packet);

#if defined(NON_MCHP_MAC)   //This is NOT a Microchip MAC
    packet.Checksum         = CalcIPChecksum((BYTE*)&packet,
                                    ICMPLen);
#endif

    IPPutHeader(remote,
                IP_PROT_ICMP,
                (WORD)(ICMP_HEADER_SIZE + len));

    IPPutArray((BYTE*)&packet, ICMPLen);

#if !defined(NON_MCHP_MAC)  //This is a Microchip MAC
    // Calculate and write the ICMP checksum using the Microchip MAC's DMA
    packet.Checksum = MACCalcTxChecksum(sizeof(IP_HEADER), ICMPLen);
    IPSetTxBuffer(MyTxBuffer, 2);
    MACPutArray((BYTE*)&packet.Checksum, 2);
#endif


    MACFlush();
}

/**
 * Swaps the contents of the words contained in the given ICMP packet
 *
 * @param p     ICMP packet header
 *
 * @return      ICMP packet is swapped
 */
static void SwapICMPPacket(ICMP_PACKET* p)
{
    p->Identifier           = swaps(p->Identifier);
    p->SequenceNumber       = swaps(p->SequenceNumber);
    p->Checksum             = swaps(p->Checksum);
}
