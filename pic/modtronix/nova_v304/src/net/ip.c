/*********************************************************************
 *
 *                  PIC IP Module for Modtronix TCP/IP Stack
 *
 *********************************************************************
 * FileName:        IP.C
 * Dependencies:    string.h
 *                  stacktsk.h
 *                  Helpers.h
 *                  IP.h
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
 * Nilesh Rajbharti     4/27/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 ********************************************************************/
#define THIS_IS_IP

#include <string.h>

#include "projdefs.h"
#include "net\ip.h"
#include "net\helpers.h"

#include "debug.h"

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xD8, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xD8, msgCode, msgStr)


// This is left shifted by 4.  Actual value is 0x04.
#define IPv4                (0x40)
#define IP_VERSION          IPv4

/**
 * IHL (Internet Header Length) is # of DWORDs in a header.
 * Since, we do not support options, our IP header length will be
 * minimum i.e. 20 bytes : IHL = 20 / 4 = 5.
 */
#define IP_IHL              (0x05)

/**
 * Type of service defines. Is a value assigned to the IP_HEADER.TypeOfService field
 */ 
#define IP_SERVICE_NW_CTRL  (0x07)
#define IP_SERVICE_IN_CTRL  (0x06)
#define IP_SERVICE_ECP      (0x05)
#define IP_SERVICE_OVR      (0x04)
#define IP_SERVICE_FLASH    (0x03)
#define IP_SERVICE_IMM      (0x02)
#define IP_SERVICE_PRIOR    (0x01)
#define IP_SERVICE_ROUTINE  (0x00)

#define IP_SERVICE_N_DELAY  (0x00)
#define IP_SERCICE_L_DELAY  (0x08)
#define IP_SERVICE_N_THRPT  (0x00)
#define IP_SERVICE_H_THRPT  (0x10)
#define IP_SERVICE_N_RELIB  (0x00)
#define IP_SERVICE_H_RELIB  (0x20)

#define IP_SERVICE          (IP_SERVICE_ROUTINE | IP_SERVICE_N_DELAY)

/**
 * Mask for IP_HEADER.FragmentInfo.DF flag.
 */ 
#define IP_FLAG_MASK_DF         (0x4000)

/**
 * Mask for IP_HEADER.FragmentInfo.MF flag.
 */ 
#define IP_FLAG_MASK_MF         (0x2000)

/**
 * Mask for IP_HEADER.FragmentInfo.FragmentOffset field.
 */ 
#define IP_FLAG_MASK_FRAGOFFSET (0x1fff)


static WORD _Identifier = 0;

static void SwapIPHeader(IP_HEADER* h);


/**
 * Only one IP message can be received. Caller may not transmit and receive
 * a message at the same time.
 *
 * @preCondition    MACRxbufGetHdr() == TRUE
 *
 * @param localIP   Local node IP Address (Destination IP Address) as received in current IP header.
 *                  If this information is not required caller may pass NULL value.
 * @param remote    Remote node info
 * @param protocol  Current packet protocol
 * @param len       Length of IP data. For example, if TCP is contained in this IP
 *                  packet, this will be = TCP Header length + TCP Data Length
 *
 * @return          TRUE, if valid packet was received <br>
 *                  FALSE otherwise
 */
BOOL IPGetHeader(IP_ADDR *localIP,
                 NODE_INFO *remote,
                 BYTE *protocol,
                 WORD *len)
{
    WORD_VAL    ReceivedChecksum;
    WORD_VAL    CalcChecksum;
    WORD        checksums[2];
    IP_HEADER   header;
    BYTE        optionsLen;
    #define MAX_OPTIONS_LEN     (20)            // As per RFC 791.
    BYTE        options[MAX_OPTIONS_LEN];

    //Read IP header. The data is read from the current MAC RX Buffer
    MACRxbufGetArray((BYTE*)&header, sizeof(header));

    //Write out ID of received IP header
    #if (DEBUG_IP >= LOG_DEBUG)
        debugPutMsg(1);     //@mxd:1:Received IP header with ID=0x%x%x
        //Write HEX WORD value of tmp
        debugPutByteHex(header.Identification.v[0]);
        debugPutByteHex(header.Identification.v[1]);
    #endif

    // Make sure that this IPv4 packet.
    if ( (header.VersionIHL & 0xf0) != IP_VERSION )
    {
        goto IPGetHeader_Discard;
    }

    /*
     * Calculate options length in this header, if there is any.
     * IHL is in terms of numbers of 32-bit DWORDs; i.e. actual
     * length is 4 times IHL.
     */
    optionsLen = ((header.VersionIHL & 0x0f) << 2) - sizeof(header);

    /*
     * If there is any option(s), read it so that we can include them
     * in checksum calculation.
     */
    if ( optionsLen > MAX_OPTIONS_LEN )
    {
        goto IPGetHeader_Discard;
    }

    if ( optionsLen > 0 ) {
        //Read options data. The data is read from the current MAC RX Buffer
        MACRxbufGetArray(options, optionsLen);
    }

    // Save header checksum; clear it and recalculate it ourselves.
    ReceivedChecksum.Val = header.HeaderChecksum.Val;
    header.HeaderChecksum.Val = 0;

    // Calculate checksum of header including options bytes.
    checksums[0] = ~CalcIPChecksum((BYTE*)&header, sizeof(header));

    // Calculate Options checksum too, if they are present.
    if ( optionsLen > 0 )
        checksums[1] = ~CalcIPChecksum((BYTE*)options, optionsLen);
    else
        checksums[1] = 0;

    CalcChecksum.Val  = CalcIPChecksum((BYTE*)checksums,
                                            2 * sizeof(WORD));

    // Network to host conversion.
    SwapIPHeader(&header);

    // Make sure that checksum is correct and IP version is supported.
    if ( ReceivedChecksum.Val != CalcChecksum.Val   ||
         (header.VersionIHL & 0xf0) != IP_VERSION )

    {
        // Bad/Unknown packet. Discard it.
        goto IPGetHeader_Discard;
    }

    /*
     * If caller is intrested, return destination IP address
     * as seen in this IP header.
     */
    if ( localIP )
        localIP->Val    = header.DestAddress.Val;

    remote->IPAddr.Val  = header.SourceAddress.Val;
    *protocol           = header.Protocol;
    *len                = header.TotalLength.Val - optionsLen -  sizeof(header);

    return TRUE;

IPGetHeader_Discard:
    MACRxbufDiscard();
    return FALSE;

}


/**
 * Write the Ethernet Header (MAC Header) and IP Header to the current TX buffer.
 * The last parameter (len) is the length of the data to follow.
 * This function will do the following: <ul>
 *  <li> Reset the NIC Remote DMA write pointer to the first byte of the current TX Buffer </li>
 *  <li> Write the given header </li>
 *  <li> Set the NIC Remote DMA byte count to the given len. This configures the Remote DMA
 *  to send the given number of bytes. Only one IP message can be transmitted at any time.
 *  Caller may not transmit and receive a message at the same time.</li></ul>
 *
 * @preCondition    IPIsTxReady() == TRUE
 *
 * @param remote    Destination node address
 * @param protocol  Protocol of data to follow, for example IP_PROT_ICMP, IP_PROT_TCP....
 * @param len       Total length of IP data bytes to follow, excluding IP header. This
 *                  is the length of the bytes to follow.
 *
 * @return          Handle to current packet - For use by IPSendByte() function.
 *
 */
WORD IPPutHeader(NODE_INFO *remote,
                BYTE protocol,
                WORD    len)
{
    IP_HEADER   header;

    header.VersionIHL           = IP_VERSION | IP_IHL;
    header.TypeOfService        = IP_SERVICE;
    header.TotalLength.Val      = sizeof(header) + len;
    header.Identification.Val   = ++_Identifier;
    /** Set the Don't fragment flag for all IP message we sent. We do NOT want to get fragmented data! */
    header.FragmentInfo.Val     = IP_FLAG_MASK_DF;
    header.TimeToLive           = MY_IP_TTL;
    header.Protocol             = protocol;
    header.HeaderChecksum.Val   = 0;
    header.SourceAddress.v[0]   = MY_IP_BYTE1;
    header.SourceAddress.v[1]   = MY_IP_BYTE2;
    header.SourceAddress.v[2]   = MY_IP_BYTE3;
    header.SourceAddress.v[3]   = MY_IP_BYTE4;

    header.DestAddress.Val = remote->IPAddr.Val;

    SwapIPHeader(&header);

    header.HeaderChecksum.Val   = CalcIPChecksum((BYTE*)&header,
                                                sizeof(header));

    //Write the Ethernet Header to the current TX buffer. The last parameter (dataLen) is the length
    //of the data to follow. This function will do the following:
    // - Reset the NIC Remote DMA write pointer to the first byte of the current TX Buffer
    // - Write the given header
    // - Set the NIC Remote DMA byte count to the given len. This configures the Remote DMA to
    //   receive the given number of bytes
    MACPutHeader(&remote->MACAddr, MAC_IP, (sizeof(header)+len));

    //Write the IP header to the MAC's TX buffer.
    MACPutArray((BYTE*)&header, sizeof(header));

    return 0x0;

}


static void SwapIPHeader(IP_HEADER* h)
{
#if defined(IP_SPEED_OPTIMIZE)
    BYTE b0;
    b0 = h->TotalLength.v[0];
    h->TotalLength.v[0]     = h->TotalLength.v[1];
    h->TotalLength.v[1]     = b0;

    b0 = h->Identification.v[0];
    h->Identification.v[0]  = h->Identification.v[1];
    h->Identification.v[1]  = b0;

    b0 = h->FragmentInfo.v[0];
    h->FragmentInfo.v[0]    = h->FragmentInfo.v[1];
    h->FragmentInfo.v[1]    = b0;

    b0 = h->HeaderChecksum.v[0];
    h->HeaderChecksum.v[0]  = h->HeaderChecksum.v[1];
    h->HeaderChecksum.v[1]  = b0;
#else
    h->TotalLength.Val      = swaps(h->TotalLength.Val);
    h->Identification.Val   = swaps(h->Identification.Val);
    h->FragmentInfo.Val     = swaps(h->FragmentInfo.Val);
    h->HeaderChecksum.Val   = swaps(h->HeaderChecksum.Val);
#endif
}
