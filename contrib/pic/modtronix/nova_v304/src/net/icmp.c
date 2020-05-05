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
 ********************************************************************/

#include <string.h>

#include "projdefs.h"
#include "net\icmp.h"
#include "net\helpers.h"
#include "net\ip.h"
#include "net\mac.h"

#if !defined(STACK_USE_ICMP)
#error You have selected to not include ICMP.  Remove this file from \
       your project to reduce code size.
#endif

/*
 * ICMP packet definition
 */
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
    WORD checksums[2];
    WORD CalcChecksum;
    WORD ReceivedChecksum;

    //Read ICMP Header. The data is read from the current MAC RX Buffer
    MACRxbufGetArray((BYTE*)&packet, ICMP_HEADER_SIZE);

    ReceivedChecksum = packet.Checksum;
    packet.Checksum = 0;

    checksums[0] = ~CalcIPChecksum((BYTE*)&packet, ICMP_HEADER_SIZE);

    *len -= ICMP_HEADER_SIZE;

    //Read ICMP Data. The data is read from the current MAC RX Buffer
    MACRxbufGetArray(data, *len);
    checksums[1] = ~CalcIPChecksum(data, *len);

    CalcChecksum = CalcIPChecksum((BYTE*)checksums, 2 * sizeof(WORD));

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
    ICMP_PACKET packet;

    packet.Code             = 0;
    packet.Type             = code;
    packet.Checksum         = 0;
    packet.Identifier       = id;
    packet.SequenceNumber   = seq;

    memcpy((void*)packet.Data, (void*)data, len);

    SwapICMPPacket(&packet);

    packet.Checksum         = CalcIPChecksum((BYTE*)&packet,
                                    (WORD)(ICMP_HEADER_SIZE + len));

    IPPutHeader(remote,
                IP_PROT_ICMP,
                (WORD)(ICMP_HEADER_SIZE + len));

    IPPutArray((BYTE*)&packet, (WORD)(ICMP_HEADER_SIZE + len));

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
