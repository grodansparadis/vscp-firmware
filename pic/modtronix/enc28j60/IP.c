/*********************************************************************
 *
 *                  PIC IP Module for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        IP.C
 * Dependencies:    string.h
 *                  StackTsk.h
 *                  Helpers.h
 *                  IP.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
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
 * Howard Schlunder		8/31/04	Beta Rev 0.9 (See version.log for detail)
 ********************************************************************/

#include <string.h>

#include "StackTsk.h"
#include "Helpers.h"
#include "MAC.h"
#include "IP.h"

// This is left shifted by 4.  Actual value is 0x04.
#define IPv4                (0x40u)
#define IP_VERSION          IPv4

/*
 * IHL (Internet Header Length) is # of DWORDs in a header.
 * Since, we do not support options, our IP header length will be
 * minimum i.e. 20 bytes : IHL = 20 / 4 = 5.
 */
#define IP_IHL              (0x05)

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

#define MY_IP_TTL           (100)   /* Time-To-Live in Seconds */




static WORD _Identifier = 0;
static BYTE IPHeaderLen;


static void SwapIPHeader(IP_HEADER* h);




/*********************************************************************
 * Function:        BOOL IPGetHeader( IP_ADDR    *localIP,
 *                                    NODE_INFO  *remote,
 *                                    BYTE        *Protocol,
 *                                    WORD        *len)
 *
 * PreCondition:    MACGetHeader() == TRUE
 *
 * Input:           localIP     - Local node IP Address as received
 *                                in current IP header.
 *                                If this information is not required
 *                                caller may pass NULL value.
 *                  remote      - Remote node info
 *                  Protocol    - Current packet protocol
 *                  len         - Current packet data length
 *
 * Output:          TRUE, if valid packet was received
 *                  FALSE otherwise
 *
 * Side Effects:    None
 *
 * Note:            Only one IP message can be received.
 *                  Caller may not transmit and receive a message
 *                  at the same time.
 *
 ********************************************************************/
BOOL IPGetHeader(IP_ADDR *localIP,
                 NODE_INFO *remote,
                 BYTE *protocol,
                 WORD *len)
{
    WORD_VAL    CalcChecksum;
    IP_HEADER   header;

#if defined(MCHP_MAC)
    BYTE		headerLen;	// ENC28J60
#else
    WORD_VAL    ReceivedChecksum;
    WORD        checksums[2];
    BYTE        optionsLen;
#define MAX_OPTIONS_LEN     (40)            // As per RFC 791.
    BYTE        options[MAX_OPTIONS_LEN];
#endif

    // Read IP header.
    MACGetArray((BYTE*)&header, sizeof(header));

    // Make sure that this is an IPv4 packet.
    if ( (header.VersionIHL & 0xf0) != IP_VERSION )
    	return FALSE;

#if defined(MCHP_MAC)
	headerLen = (header.VersionIHL & 0x0f) << 2;
	
// TODO: Optimize checksum procedure.  
// You don't have to zero out the checksum stored in the buffer.
// If you leave it in place, the newly calculated checksum always 
// come out to be 0x0000.
	MACSetRxBuffer(10);	// 10 is the offset in header to the HeaderChecksum member
	MACPut(0x00);
	MACPut(0x00);
	CalcChecksum.Val = MACCalcRxChecksum(0, headerLen);
	MACSetRxBuffer(headerLen);	// Seek back to the end of the IP header

    if ( header.HeaderChecksum != CalcChecksum.Val )
#else
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
        return FALSE;

    if ( optionsLen > 0 )
        MACGetArray(options, optionsLen);

    // Save header checksum; clear it and recalculate it ourselves.
    ReceivedChecksum.Val = header.HeaderChecksum;
    header.HeaderChecksum = 0;

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

    /*
     * If caller is intrested, return destination IP address
     * as seen in this IP header.
     */
    if ( localIP )
        localIP->Val    = header.DestAddress.Val;

    remote->IPAddr.Val  = header.SourceAddress.Val;
    *protocol           = header.Protocol;
#if defined(MCHP_MAC)
    *len 				= header.TotalLength - headerLen;
#else
    *len                = header.TotalLength - optionsLen -
                            sizeof(header);
#endif                           

    return TRUE;
}




/*********************************************************************
 * Function:        WORD IPPutHeader(   IP_ADDR *Dest,
 *                                      BYTE    Protocol,
 *                                      WORD    Identifier,
 *                                      WORD    DataLen)
 *
 * PreCondition:    IPIsTxReady() == TRUE
 *
 * Input:           Src         - Destination node address
 *                  Protocol    - Current packet protocol
 *                  Identifier  - Current packet identifier
 *                  DataLen     - Current packet data length
 *
 * Output:          Handle to current packet - For use by
 *                  IPSendByte() function.
 *
 * Side Effects:    None
 *
 * Note:            Only one IP message can be transmitted at any
 *                  time.
 *                  Caller may not transmit and receive a message
 *                  at the same time.
 *
 ********************************************************************/
WORD IPPutHeader(NODE_INFO *remote,
                BYTE protocol,
                WORD    len)
{
    IP_HEADER   header;
    IPHeaderLen = sizeof(IP_HEADER);

    header.VersionIHL       = IP_VERSION | IP_IHL;
    header.TypeOfService    = IP_SERVICE;
    header.TotalLength      = sizeof(header) + len;
    header.Identification   = ++_Identifier;
    header.FragmentInfo     = 0;
    header.TimeToLive       = MY_IP_TTL;
    header.Protocol         = protocol;
    header.HeaderChecksum   = 0;
    header.SourceAddress.v[0] = MY_IP_BYTE1;
    header.SourceAddress.v[1] = MY_IP_BYTE2;
    header.SourceAddress.v[2] = MY_IP_BYTE3;
    header.SourceAddress.v[3] = MY_IP_BYTE4;

    header.DestAddress.Val = remote->IPAddr.Val;

    SwapIPHeader(&header);

#if !defined(MCHP_MAC)
    header.HeaderChecksum   = CalcIPChecksum((BYTE*)&header,
                                                sizeof(header));
#endif

    MACPutHeader(&remote->MACAddr, MAC_IP, (sizeof(header)+len));
    MACPutArray((BYTE*)&header, sizeof(header));

#if defined(MCHP_MAC)
	header.HeaderChecksum = MACCalcTxChecksum(0, sizeof(header));
	MACSetTxBuffer(0, 10);	// 10 is the offset in header to the HeaderChecksum member
	MACPutArray((BYTE*)&header.HeaderChecksum, 2);
	MACSetTxBuffer(0, sizeof(header));	// Seek back to the end of the packet
#endif	
	
    return 0x0;

}

/*********************************************************************
 * Function:        IPSetRxBuffer(WORD Offset)
 *
 * PreCondition:    IPHeaderLen must have been intialized by 
 *					IPGetHeader() or IPPutHeader()
 *
 * Input:           Offset from beginning of IP data field
 *
 * Output:          Next Read/Write access to receive buffer is
 *                  set to Offset 
 *
 * Side Effects:    None
 *
 * Note:            None
 *
 ********************************************************************/
void IPSetRxBuffer(WORD Offset) 
{
	MACSetRxBuffer(Offset+IPHeaderLen);
}

static void SwapIPHeader(IP_HEADER* h)
{
    h->TotalLength      = swaps(h->TotalLength);
    h->Identification   = swaps(h->Identification);
    h->HeaderChecksum   = swaps(h->HeaderChecksum);
}
