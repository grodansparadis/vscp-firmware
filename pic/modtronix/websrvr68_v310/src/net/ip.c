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
 * 2006-09-08, David Hosken (DH): Implemented changes from Microchip TCP/IP stack 3.75
 ********************************************************************/
#define THIS_IS_IP

#include <string.h>

#include "projdefs.h"
#include "net\checkcfg.h"
#include "net\ip.h"
#include "net\helpers.h"

#include "debug.h"

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xE6, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xE6, msgCode, msgStr)


// This is left shifted by 4.  Actual value is 0x04.
#define IPv4                (0x40ul)
#define IP_VERSION          IPv4

// IHL (Internet Header Length) is # of DWORDs in a header.
// Since, we do not support options, our IP header length will be
// minimum i.e. 20 bytes : IHL = 20 / 4 = 5.
#define IP_IHL              (0x05ul)

#define IP_SERVICE_NW_CTRL  (0x07ul)
#define IP_SERVICE_IN_CTRL  (0x06ul)
#define IP_SERVICE_ECP      (0x05ul)
#define IP_SERVICE_OVR      (0x04ul)
#define IP_SERVICE_FLASH    (0x03ul)
#define IP_SERVICE_IMM      (0x02ul)
#define IP_SERVICE_PRIOR    (0x01ul)
#define IP_SERVICE_ROUTINE  (0x00ul)

#define IP_SERVICE_N_DELAY  (0x00ul)
#define IP_SERCICE_L_DELAY  (0x08ul)
#define IP_SERVICE_N_THRPT  (0x00ul)
#define IP_SERVICE_H_THRPT  (0x10ul)
#define IP_SERVICE_N_RELIB  (0x00ul)
#define IP_SERVICE_H_RELIB  (0x20ul)

#define IP_SERVICE          (IP_SERVICE_ROUTINE | IP_SERVICE_N_DELAY)

/**
 * Mask for IP_HEADER.FragmentInfo.DF flag. Don't Fragment
 */ 
#define IP_FLAG_MASK_DF         (0x4000ul)

/**
 * Mask for IP_HEADER.FragmentInfo.MF flag.
 */ 
#define IP_FLAG_MASK_MF         (0x2000ul)

/**
 * Mask for IP_HEADER.FragmentInfo.FragmentOffset field.
 */ 
#define IP_FLAG_MASK_FRAGOFFSET (0x1ffful)


static WORD _Identifier = 0;
static BYTE IPHeaderLen;


static void SwapIPHeader(IP_HEADER* h);


/**
 * Only one IP message can be received. Caller may not transmit and receive
 * a message at the same time. If this function returns false, the caller must
 * call MACDiscardRx() to discard and release the current receive buffer!
 *
 * @preCondition    MACGetHeader() == TRUE
 *
 * @param localIP   Local node IP Address (Destination IP Address) as received in current IP header.
 *                  If this information is not required caller may pass NULL value.
 * @param remote    Remote node info
 * @param protocol  Current packet protocol
 * @param len       Length of IP data. For example, if TCP is contained in this IP
 *                  packet, this will be = TCP Header length + TCP Data Length
 *
 * @return          TRUE, if valid packet was received <br>
 *                  FALSE otherwise - call MACDiscardRx() to discard message if FALSE is returned !!
 */
BOOL IPGetHeader(IP_ADDR *localIP,
                 NODE_INFO *remote,
                 BYTE *protocol,
                 WORD *len)
{
    WORD_VAL    CalcChecksum;
    IP_HEADER   header;

#if defined(NON_MCHP_MAC)
    WORD_VAL    ReceivedChecksum;
    WORD        checksums[2];
    BYTE        optionsLen;
    #define MAX_OPTIONS_LEN     (40ul)  // As per RFC 791.
    BYTE        options[MAX_OPTIONS_LEN];
#endif

    //Read IP header. The data is read from the current MAC RX Buffer
    MACGetArray((BYTE*)&header, sizeof(header));

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
        #if (DEBUG_IP >= LOG_WARN)
            debugPutMsg(2);     //@mxd:2:IP Header contained too many options, discarding message!
        #endif

        return FALSE;
    }

    //Calculate the length of the header.
    //!!! Important !!! There is a bug in the Microchip TCP/IP stack!!! This must be calculated for Microchip and NON Microchip MACs!
    IPHeaderLen = (header.VersionIHL & 0x0f) << 2;
    
#if !defined(NON_MCHP_MAC)  //This is a Microchip MAC
    // Validate the IP header.  If it is correct, the checksum 
    // will come out to 0x0000 (because the header contains a 
    // precomputed checksum).  A corrupt header will have a 
    // nonzero checksum.
    CalcChecksum.Val = MACCalcRxChecksum(0, IPHeaderLen);

    // Seek to the end of the IP header
    MACSetRxBuffer(IPHeaderLen);

    if(CalcChecksum.Val)
#else   //This is NOT a Microchip MAC
    // Calculate options length in this header, if there are any.
    // IHL is in terms of numbers of 32-bit DWORDs; i.e. actual
    // length is 4 times IHL.
    optionsLen = IPHeaderLen - sizeof(header);

    // If there is any option(s), read it so that we can include them
    // in checksum calculation.
    if ( optionsLen > MAX_OPTIONS_LEN )
        return FALSE;

    if ( optionsLen > 0 )
        MACGetArray(options, optionsLen);   //Read options data. The data is read from the current MAC RX Buffer

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

    // Make sure that checksum is correct
    if ( ReceivedChecksum.Val != CalcChecksum.Val )
#endif
    {
        // Bad packet. The function caller will be notified by means of the FALSE 
        // return value and it should discard the packet.
        return FALSE;
    }

    // Network to host conversion.
    SwapIPHeader(&header);

    // If caller is intrested, return destination IP address
    // as seen in this IP header.
    if ( localIP )
        localIP->Val    = header.DestAddress.Val;

    remote->IPAddr.Val  = header.SourceAddress.Val;
    *protocol           = header.Protocol;

    //Optimized Microchip stack, this is the same as "header.TotalLength.Val - optionsLen - sizeof(header)"
    *len                 = header.TotalLength.Val - IPHeaderLen;

    return TRUE;
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

    IPHeaderLen = sizeof(IP_HEADER);

    header.VersionIHL           = IP_VERSION | IP_IHL;
    header.TypeOfService        = IP_SERVICE;
    header.TotalLength.Val      = sizeof(header) + len;
    header.Identification.Val   = ++_Identifier;
    header.FragmentInfo.Val     = IP_FLAG_MASK_DF;  //Set the Don't fragment flag for all IP message we sent. We do NOT want to get fragmented data!
    header.TimeToLive           = MY_IP_TTL;
    header.Protocol             = protocol;
    header.HeaderChecksum.Val   = 0;
    header.SourceAddress.v[0]   = MY_IP_BYTE1;
    header.SourceAddress.v[1]   = MY_IP_BYTE2;
    header.SourceAddress.v[2]   = MY_IP_BYTE3;
    header.SourceAddress.v[3]   = MY_IP_BYTE4;

    header.DestAddress.Val = remote->IPAddr.Val;

    SwapIPHeader(&header);

#if defined(NON_MCHP_MAC)   //This is NOT a Microchip MAC
    header.HeaderChecksum.Val   = CalcIPChecksum((BYTE*)&header,
                                                sizeof(header));
#endif

    //Write the Ethernet Header to the current TX buffer. The last parameter (dataLen) is the length
    //of the data to follow. This function will do the following:
    // - Reset the NIC Remote DMA write pointer to the first byte of the current TX Buffer
    // - Write the given header
    // - Set the NIC Remote DMA byte count to the given len. This configures the Remote DMA to
    //   receive the given number of bytes
    MACPutHeader(&remote->MACAddr, MAC_IP, (sizeof(header)+len));

    //Write the IP header to the MAC's TX buffer.
    MACPutArray((BYTE*)&header, sizeof(header));

#if !defined(NON_MCHP_MAC)      //This is a Microchip MAC
    header.HeaderChecksum = MACCalcTxChecksum(0, sizeof(header));
    MACSetTxBuffer(CurrentTxBuffer, 10);    // 10 is the offset in header to the HeaderChecksum member
    MACPutArray((BYTE*)&header.HeaderChecksum, 2);
    MACSetTxBuffer(CurrentTxBuffer, sizeof(header));    // Seek back to the end of the packet
#endif    

    return 0x0;

}


/**
 * Sets the receive buffer access pointer to given offset in IP Data.
 * For example, if IP data is a TCP packet, an offset of 0 will set
 * access to first byte of TCP header.
 * Layers that use IP services (TCP, UDP...) should call this macro
 * to set the access pointer for the current buffer.
 *
 * @param offset     Offset, An offset with respect to IP Data
 *
 */
void IPSetRxBuffer(WORD Offset) 
{
    //The IP header can be a variable lengh, seeing that it can have options. We can NOT use
    //sizeof(IP_HEADER), because this will not take into account any options!
    MACSetRxBuffer(Offset+IPHeaderLen);
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
