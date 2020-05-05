/**
 * @brief           UDP Module for Modtronix TCP/IP Stack
 * @file            udp.c
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
 * 2003-02-26, Nilesh Rajbharti:
 *    - Fixed UDPGet and UDPProcess bugs
 *      as discovered and fixed by John Owen of Powerwave.
 *      1. UDPGet would return FALSE on last good byte
 *      2. UDPProcess was incorrectly calculating length.
 * 2001-03-19, Nilesh Rajbharti:
 *    - Original Rev 1.0
 * 2003-05-19, Nilesh Rajbharti:
 *    - Added bFirstRead flag similar to TCP to detect very first UDPGet and reset MAC Rx
 *      pointer to begining of UDP data area.  This would ensure that if UDP socket
 *      has pending Rx data and another module resets MAC Rx pointer, next UDP socket Get
 *      would get correct data.
 * 2003-05-29, Robert Sloan:
 *    - Improved FindMatchingSocket()
 * 2003-12-02, Nilesh Rajbharti:
 *    - Added UDPChecksum logic in UDPProcess()
 * 2003-12-05, Nilesh Rajbharti:
 *    - Modified UDPProcess() and FindMatchingSocket() to include destIp as new parameter.
 *      This corrects pseudo header checksum logic in UDPProcess(). It also corrects broadcast
 *      packet matching correct in FindMatchingSocket().
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 * 2005-12-28, David Hosken (DH):
 *    - Fixed problem with UDP lenght of received message
 * 2006-09-08, David Hosken (DH): Implemented changes from Microchip TCP/IP stack 3.75
 *********************************************************************/
 
#define THIS_IS_UDP_MODULE

#include <string.h>

#include "projdefs.h"
#include "net\checkcfg.h"
#include "net\udp.h"
#include "net\helpers.h"
#include "net\mac.h"
#include "net\ip.h"
#include "debug.h"

#if defined(STACK_USE_UDP)

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0x9F, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0x9F, msgCode, msgStr)

#if (MAX_UDP_SOCKETS <= 14)
    #define debugPutOffsetMsg(offset, msgCode) debugPut2Bytes(0x90 + offset, msgCode)
#else
    /* If there are more then 14 HTTP connections, then */
    #define debugPutOffsetMsg(offset, msgCode) \
        if (offset <= 14) {debugPut2Bytes(0x90 + offset, msgCode);} \
        else {debugPut2Bytes(0x90, offset);debugPutByte(msgCode);}
#endif


/*
 * Pseudo header as defined by rfc 793.
 */
typedef struct _PSEUDO_HEADER
{
    IP_ADDR SourceAddress;
    IP_ADDR DestAddress;
    BYTE Zero;
    BYTE Protocol;
    WORD Length;
} PSEUDO_HEADER;

#define SwapPseudoHeader(h)  (h.Length = swaps(h.Length))

UDP_SOCKET_INFO  UDPSocketInfo[MAX_UDP_SOCKETS];
UDP_SOCKET activeUDPSocket;

static UDP_SOCKET FindMatchingSocket(UDP_HEADER *h, NODE_INFO *remoteNode,
                                    IP_ADDR *destIP);


/**
 * Initializes internal variables.
 *
 * @preCondition     None
 */
void UDPInit(void)
{
    UDP_SOCKET s;

    for ( s = 0; s < MAX_UDP_SOCKETS; s++ )
    {
        UDPClose(s);
    }
}


/**
 * A UDP packet header is assembled and loaded into UDP transmit buffer.
 *
 * A localPort value of '0' is considered nonexistent port.  This call
 * must always have nonzero localPort value. This function sets returned
 * socket as an active UDP socket.
 *
 * @preCondition        UDPInit() is already called
 *
 * @param localPort     A non-zero port number.
 * @param remoteNode    Remote Node info such as MAC and IP address.
 *                      If NULL, broadcast node address is set.
 * @param remotePort    Remote Port to which to talk to.
 *                      If INVALID_UDP_PORT, localPort is opened for Listen.
 *
 * @return              A valid UDP socket that is to be used for subsequent UDP communications.
 *                      If the given port could not be opened, or an error occured, INVALID_UDP_SOCKET
 *                      is returned.
 */
UDP_SOCKET UDPOpen(UDP_PORT localPort,
                   NODE_INFO *remoteNode,
                   UDP_PORT remotePort)
{
    UDP_SOCKET s;
    UDP_SOCKET_INFO *p;


    p = UDPSocketInfo;
    for ( s = 0; s < MAX_UDP_SOCKETS; s++ )
    {
        if ( p->localPort == INVALID_UDP_PORT )
        {
            p->localPort    = localPort;

            // If remoteNode is supplied, remember it.
            if ( remoteNode )
            {
                memcpy((void*)&p->remoteNode,
                        (const void*)remoteNode,
                        sizeof(p->remoteNode));
            }
            // else Set broadcast address - TO BE DONE


            p->remotePort   = remotePort;
            p->TxCount      = 0;
            p->RxCount      = 0;

            // Mark this socket as active.
            // Once an active socket is set, subsequent operation can be
            // done without explicitely supply socket identifier.
            activeUDPSocket = s;
            return s;
        }
        p++;
    }

    return (UDP_SOCKET)INVALID_UDP_SOCKET;
}


/**
 * Given socket is marked as available for future new communcations.
 *
 * This function does not affect previous active UDP socket designation.
 *
 * @preCondition    UDPOpen() is already called
 *
 * @param s         Socket that is to be closed.
 */
void UDPClose(UDP_SOCKET s)
{
    UDPSocketInfo[s].localPort = INVALID_UDP_PORT;
    UDPSocketInfo[s].remoteNode.IPAddr.Val = 0x00000000;
    UDPSocketInfo[s].Flags.bFirstRead = FALSE;
}


/**
 * Checks if there is a transmit buffer ready for accepting data, and that the given socket
 * is valid (not equal to INVALID_UDP_SOCKET for example).
 *
 * Side Effects:    Given socket is set as the active UDP Socket. <br>
 *
 * Note: This function automatically sets supplied socket as the active
 * socket. All subsequent calls will us this socket as the active socket.
 *
 * @param s         Socket that is to be loaded and made an active UDP socket.
 *
 * @return          TRUE if at least one UDP buffer is ready to transmit, and given socket is valid<br>
 *                  FALSE if not
 */
BOOL UDPIsPutReady(UDP_SOCKET s)
{
    //Check if the given socket is valid
    if (s < MAX_UDP_SOCKETS) {
        activeUDPSocket = s;
    
        return MACIsTxReady(TRUE);
    }
    
    return FALSE;
}


/**
 * Given data byte is put into the UDP transmit buffer and active UDP socket buffer
 * length is incremented. The data is NOT sent yet, and the UDPFlush() function must
 * be called to send all data contained in the transmit buffer.
 *
 * If the transmit buffer filled up, the contents of the transmit buffer will be sent,
 * and this function will return FALSE. In this case, it is VERY IMPORTANT to call the
 * UDPIsPutReady() function again before calling the UDPPut() or UDPPutArray() functions!
 *
 * Note: This function loads data into an active UDP socket as determined by previous
 * call to UDPIsPutReady().
 *
 * @preCondition    UDPIsPutReady() == TRUE with desired UDP socket
 *                  that is to be loaded.
 *
 * @param v       - Data byte to loaded into transmit buffer
 *
 * @return          TRUE if transmit buffer is still ready to accept more data bytes <br>
 *                  FALSE if transmit buffer can no longer accept any more data byte.<br>
 *                  If FALSE is returned, then UDPIsPutReady() has to be called again before
 *                  calling the UDPPut() or UDPPutArray() functions!
 */
BOOL UDPPut(BYTE v)
{
    UDP_SOCKET_INFO *p;

    p = &UDPSocketInfo[activeUDPSocket];

    //This UDP Socket does not contain any unsent data, and currently does not own a TX Buffer!
    //Assign it the next available TX Buffer
    if ( p->TxCount == 0 )
    {
        // This is the very first byte that is loaded in UDP buffer.
        // Remember what transmit buffer we are loading, and
        // start loading this and next bytes in data area of UDP packet.
        p->TxBuffer = MACGetTxBuffer(TRUE);

        // Make sure that we received a TX buffer
        if(p->TxBuffer == INVALID_BUFFER)
            return FALSE;

        //This sets the current TX Buffer pointer to the given offset after the IP header.
        //We give the size of the UDP header here as a parameter. This causes the current
        //write pointer to be set to firt byte after the UDP header, which is the UDP data area.
        IPSetTxBuffer(p->TxBuffer, sizeof(UDP_HEADER));

        //p->TxOffset = 0;  /* TxOffset is not required! */
    }

    //Load it.
    MACPut(v);

    //Keep track of number of bytes loaded.
    //If total bytes fill up buffer, transmit it.
    p->TxCount++;

    if ( p->TxCount >= UDPGetMaxDataLength() )
    {
        UDPFlush();
    }
    
    return TRUE;
}


/**
 * Given number of data bytes from the given array are put into the UDP transmit buffer
 * and active UDP socket buffer length is incremented by number of bytes. The data is
 * NOT sent yet, and the UDPFlush() function must be called to send all data contained
 * in the transmit buffer.
 *
 * If there is not enough space in the transmit buffer for all the data, the contents of
 * the transmit buffer will be sent, and this function will return the actual amount of
 * bytes that were sent. In this case, it is VERY IMPORTANT to call the UDPIsPutReady()
 * function again before calling the UDPPut() or UDPPutArray() functions! This will however
 * only happen if the transmit buffer fills up. The transmit buffer for UDP data is
 * = (MAC_TX_BUFFER_SIZE - 42), which is usually 982 bytes. If writing less then this to
 * the transmit buffer before calling UDPFlush(), then this function will always return the
 * requested number of bytes!
 *
 * Note: This function loads data into an active UDP socket as determined by previous
 * call to UDPIsPutReady().
 *
 * @preCondition    UDPIsPutReady() == TRUE with desired UDP socket
 *                  that is to be loaded.
 *
 * @param[in] buffer Buffer containing data that has to be sent.
 * @param count     Number of bytes to send
 *
 * @return          Number of bytes added to the transmit buffer.<br>
 *                  !!!!! IMPORTANT !!!!!<br>
 *                  If this value is less then the number of bytes we requested to send, then 
 *                  UDPIsPutReady() must be called again before calling the UDPPut() or
 *                  UDPPutArray() functions!
 */
WORD UDPPutArray(BYTE *buffer, WORD count)
{
    UDP_SOCKET_INFO *p;
    WORD temp;

    p = &UDPSocketInfo[activeUDPSocket];

    //This UDP Socket does not contain any unsent data, and currently does not own a TX Buffer!
    //Assign it the next available TX Buffer
    if ( p->TxCount == 0 )
    {
        //Get handle to next available TX Buffer. The UDPIsPutReady() function that has to be called
        //prior to this function will determine if there is an available TX Buffer.
        p->TxBuffer = MACGetTxBuffer(TRUE);

        // Make sure that we received a TX buffer
        if(p->TxBuffer == INVALID_BUFFER)
            return FALSE;

        //This sets the current TX Buffer pointer to the given offset after the IP header.
        //We give the size of the UDP header here as a parameter. This causes the current
        //write pointer to be set to firt byte after the UDP header, which is the UDP data
        //area.
        IPSetTxBuffer(p->TxBuffer, sizeof(UDP_HEADER));
    }
    
    //This function request more bytes to be written to the TX Buffer then there is space 
    if ((p->TxCount + count) > UDPGetMaxDataLength()) {
        //Update count with maximum number of bytes that there is place for in the TX Buffer
        count = UDPGetMaxDataLength() - p->TxCount;
    }

    //Write buffer
    MACPutArray(buffer, count);

    //Keep track of number of bytes loaded.
    //If total bytes fill up buffer, transmit it.
    p->TxCount +=  count;

    if ( p->TxCount >= UDPGetMaxDataLength() )
    {
        UDPFlush();
    }
    
    return count;   //Return actual number of bytes sent
}


/**
 * This function transmit all data from the active UDP socket.
 *
 * @preCondition    UDPPut() is already called and desired UDP socket
 *                  is set as an active socket by calling
 *                  UDPIsPutReady().
 *
 * @return          All and any data associated with active UDP socket
 *                  buffer is marked as ready for transmission.
 *
 */
void UDPFlush(void)
{
    UDP_HEADER      h;
    UDP_SOCKET_INFO *p;

    // Wait for TX hardware to become available (finish transmitting 
    // any previous packet)
    while( !IPIsTxReady(TRUE) ) FAST_USER_PROCESS();

    p = &UDPSocketInfo[activeUDPSocket];

    h.SourcePort.Val        = swaps(p->localPort);
    h.DestinationPort.Val   = swaps(p->remotePort);
    h.Length.Val            = (WORD)((WORD)p->TxCount + (WORD)sizeof(UDP_HEADER));
    // Do not swap h.Length yet.  It is needed in IPPutHeader.
    h.Checksum.Val      = 0x00;

    //Makes the given TX Buffer active, and set's the pointer to the first byte after the IP
    //header. This is the first byte of the UDP header. The UDP header follows the IP header.
    IPSetTxBuffer(p->TxBuffer, 0);

    //Load IP header.
    IPPutHeader( &p->remoteNode,
                 IP_PROT_UDP,
                 h.Length.Val );


    //Now swap h.Length.Val
    h.Length.Val        = swaps(h.Length.Val);

    //Now load UDP header.
    IPPutArray((BYTE*)&h, sizeof(h));

    //Update checksum. !!!!!!!!!!!!!!! TO BE IMPLEMENTED !!!!!!!!!!!!!!!!!!!

    //Send data contained in TX Buffer via MAC
    MACFlush();

    // The buffer was reserved with AutoFree, so we can immediately 
    // discard it.  The MAC layer will free it after transmission.
    p->TxBuffer         = INVALID_BUFFER;
    p->TxCount          = 0;

}


/**
 * Check if given socket contains any data that is ready to be read, and that the given socket
 * is valid (not equal to INVALID_UDP_SOCKET for example). It also  sets the given socket as
 * the active UDP socket. <br>
 *
 * Side Effects:    Given socket is set as the active UDP Socket. <br>
 *
 * Note: This function automatically sets supplied socket as the active
 * socket. All subsequent calls will us this socket as the active socket.
 *
 * @preCondition     UDPInit() is already called.
 *
 * @param s         A valid UDP socket that is already "Listen"ed on
 *                  or opened.
 *
 * @return          TRUE if given socket contains any data, and is a valid socket.<br>
 *                  FALSE if not.
 */
BOOL UDPIsGetReady(UDP_SOCKET s)
{
    //Check if the given socket is valid
    if (s < MAX_UDP_SOCKETS) {
        activeUDPSocket = s;
        return ( UDPSocketInfo[activeUDPSocket].RxCount > 0 );
    }
    
    return FALSE;
}

/**
 * UDP function that returns number of UDP bytes that are available
 */
WORD UDPIsGetReady_(UDP_SOCKET s)
{
    //Check if the given socket is valid
    if (s < MAX_UDP_SOCKETS) {
        activeUDPSocket = s;
        return ( UDPSocketInfo[activeUDPSocket].RxCount);
    }
    
    return 0;
}




/**
 * Get a byte of data.
 *
 * Note: This function fetches data from an active UDP socket as set by
 * UDPIsGetReady() call.
 *
 * @preCondition    UDPInit() is already called     AND
 *                  UDPIsGetReady(s) == TRUE
 *
 * @param[out] v    Buffer to receive UDP data byte
 *
 * @return          TRUE if a data byte was read <br>
 *                  FALSE if no data byte was read or available
 */
BOOL UDPGet(BYTE *v)
{
    // CALLER MUST MAKE SURE THAT THERE IS ENOUGH DATA BYTE IN BUFFER
    // BEFORE CALLING THIS FUNCTION.
    // USE UDPIsGetReady() TO CONFIRM.
    if ( UDPSocketInfo[activeUDPSocket].RxCount == 0 )
        return FALSE;

    // If if this very first read to packet, set MAC Rx Pointer to
    // beginig of UDP data area.
    if ( UDPSocketInfo[activeUDPSocket].Flags.bFirstRead )
    {
        UDPSocketInfo[activeUDPSocket].Flags.bFirstRead = FALSE;
        UDPSetRxBuffer(0);
    }

    *v = MACGet(); //Read a single byte from the current MAC Receive buffer

    UDPSocketInfo[activeUDPSocket].RxCount--;

    if ( UDPSocketInfo[activeUDPSocket].RxCount == 0 )
    {
        MACDiscardRx(); //Discard the contents of the current RX buffer
    }

    return TRUE;
}


/**
 * Read the requested number of bytes from the active UDP socket into
 * the given buffer.
 *
 * Note: This function fetches data from an active UDP socket as set by
 * UDPIsGetReady() call.
 *
 * @preCondition    UDPInit() is already called     AND
 *                  UDPIsGetReady(s) == TRUE
 *
 * @param[in] buffer   Buffer to hold received data.
 * @param count         Buffer length
 *
 * @return          Number of bytes loaded into buffer.
 */
WORD UDPGetArray(BYTE *buffer, WORD count)
{
    WORD bytesRead;
    
    if ( UDPSocketInfo[activeUDPSocket].RxCount == 0 )
        return 0;

    // If if this very first read to packet, set MAC Rx Pointer to
    // beginig of UDP data area.
    if ( UDPSocketInfo[activeUDPSocket].Flags.bFirstRead )
    {
        UDPSocketInfo[activeUDPSocket].Flags.bFirstRead = FALSE;
        UDPSetRxBuffer(0);
    }

    //If more bytes requested then are left in the receive buffer, adjust count
    if (count > UDPSocketInfo[activeUDPSocket].RxCount) {
        count = UDPSocketInfo[activeUDPSocket].RxCount;
    }

    //Read the requested amount of data from the current MAC receive buffer
    bytesRead = MACGetArray(buffer, count);

    UDPSocketInfo[activeUDPSocket].RxCount -= bytesRead;

    if ( UDPSocketInfo[activeUDPSocket].RxCount == 0 )
    {
        MACDiscardRx(); //Discard the contents of the current RX buffer
    }

    return count;
}


/**
 * This function discards an active UDP socket content.
 *
 * @preCondition     UDPInit() is already called    AND
 *                  UDPIsGetReady() == TRUE with desired UDP socket.
 */
void UDPDiscard(void)
{
    if ( UDPSocketInfo[activeUDPSocket].RxCount )
        MACDiscardRx(); //Discard the contents of the current RX buffer

    UDPSocketInfo[activeUDPSocket].RxCount = 0;
}


/**
 * Performs UDP related tasks. Must continuesly be called.
 *
 * @preCondition    UDPInit() is already called AND <br>
 *                  UDP segment is ready in MAC buffer
 *
 * @param remoteNode    Remote node info
 * @param len           Total length of UDP semgent
 * @param destIP        The Destination IP Address of the currently received packet
 *
 * @return          TRUE if this function has completed its task <br>
 *                  FALSE otherwise
 */
BOOL UDPProcess(NODE_INFO *remoteNode, IP_ADDR *destIP, WORD len)
{
    UDP_HEADER h;
    UDP_SOCKET s;
    PSEUDO_HEADER   pseudoHeader;
    WORD_VAL        checksum;
    #if defined(UDP_SPEED_OPTIMIZE)
    BYTE temp;
    #endif

    //Retrieve UDP header. The data is read from the current MAC RX Buffer
    MACGetArray((BYTE*)&h, sizeof(h));

    #if defined(UDP_SPEED_OPTIMIZE)
        temp                    = h.SourcePort.v[0];
        h.SourcePort.v[0]       = h.SourcePort.v[1];
        h.SourcePort.v[1]       = temp;

        temp                    = h.DestinationPort.v[0];
        h.DestinationPort.v[0]  = h.DestinationPort.v[1];
        h.DestinationPort.v[1]  = temp;

        //Will an overflow occur after the subtraction?
        if ((BYTE)sizeof(UDP_HEADER) > h.Length.v[1]) {
            temp                = h.Length.v[0] - 1;
        }
        else {
            temp                = h.Length.v[0];
        }
        h.Length.v[0]           = h.Length.v[1] - (BYTE)sizeof(UDP_HEADER);
        h.Length.v[1]           = temp;
    #else
        h.SourcePort.Val        = swaps(h.SourcePort.Val);
        h.DestinationPort.Val   = swaps(h.DestinationPort.Val);
        h.Length.Val            = swaps(h.Length.Val) - sizeof(UDP_HEADER);
    #endif

    // See if we need to validate the checksum field (0x0000 is disabled)
    if(h.Checksum.Val)
    {
        #if defined(UDP_SPEED_OPTIMIZE)
            temp                    = h.Checksum.v[0];
            h.Checksum.v[0]         = h.Checksum.v[1];
            h.Checksum.v[1]         = temp;
        #else
            h.Checksum.Val          = swaps(h.Checksum.Val);
        #endif

        // Calculate IP pseudoheader checksum.
        pseudoHeader.SourceAddress      = remoteNode->IPAddr;
        pseudoHeader.DestAddress.v[0]   = destIP->v[0];
        pseudoHeader.DestAddress.v[1]   = destIP->v[1];
        pseudoHeader.DestAddress.v[2]   = destIP->v[2];
        pseudoHeader.DestAddress.v[3]   = destIP->v[3];
        pseudoHeader.Zero               = 0x0;
        pseudoHeader.Protocol           = IP_PROT_UDP;
        pseudoHeader.Length             = len;

        SwapPseudoHeader(pseudoHeader);

        checksum.Val = ~CalcIPChecksum((BYTE*)&pseudoHeader,
                                        sizeof(pseudoHeader));


        // Set UCP packet checksum = pseudo header checksum in MAC RAM.
        IPSetRxBuffer(6);
        MACPut(checksum.v[0]);
        // In case if the end of the RX buffer is reached and a wraparound is needed, set the next address to prevent writing to the wrong address.
        IPSetRxBuffer(7);
        MACPut(checksum.v[1]);
        IPSetRxBuffer(0);   //Set current receive buffer access pointer to first byte of IP data = first byte of TCP header

        // Now calculate UDP packet checksum in NIC RAM - including
        // pesudo header.
        checksum.Val = CalcIPBufferChecksum(len);

        if ( checksum.Val != h.Checksum.Val )
        {
            MACDiscardRx(); //Discard the contents of the current RX buffer
            return TRUE;
        }
    }

    s = FindMatchingSocket(&h, remoteNode, destIP);
    if ( s == INVALID_UDP_SOCKET )
    {
         // If there is no matching socket, There is no one to handle
         // this data.  Discard it.
        MACDiscardRx(); //Discard the contents of the current RX buffer
    }
    else
    {
        UDPSocketInfo[s].RxCount = h.Length.Val;
        UDPSocketInfo[s].Flags.bFirstRead = TRUE;
    }

    return TRUE;
}


/**
 * Get's the MAC and IP address of the currently active UDP socket. This data is assigned the MAC
 * and IP address of the remote node after a UDP message has been received. So, the MAC and IP address
 * returned by this function will only be valid if a UDP message has already been received on this
 * socket! The active socket is set by the UDPIsGetReady() call.
 *
 * @preCondition    UDPInit() is already called     AND
 *                  UDPIsGetReady(s) == TRUE
 *
 * @return          Pointer to a NODE_INFO structure containing the remote node's MAC and IP address
 */
NODE_INFO * UDPGetNodeInfo() {
    return &UDPSocketInfo[activeUDPSocket].remoteNode;
}


/**
 *
 * @preCondition        UDP Segment header has been retrieved from buffer
 *                      The IP header has also been retrieved
 *
 * @param remoteNode    Remote node info from IP header
 * @param h             header of UDP semgent.
 * @param destIP        The destination IP address
 *
 * @return          matching UDP socket or INVALID_UDP_SOCKET
 */
#define BROADCAST_ADDRESS   (0xfffffffful)
static UDP_SOCKET FindMatchingSocket(UDP_HEADER *h,
                                     NODE_INFO *remoteNode,
                                     IP_ADDR *destIP)
{
    UDP_SOCKET s;
    UDP_SOCKET partialMatch;
    UDP_SOCKET_INFO *p;

    partialMatch = INVALID_UDP_SOCKET;

    p = UDPSocketInfo;  //Get first UDP_SOCKET_INFO object in array
    for ( s = 0; s < MAX_UDP_SOCKETS; s++ )
    {
        // This packet is said to be matching with current socket
        // 1. If its destination port matches with our local port.
        // 2. This socket does not have any data pending.
        // 3. Packet source IP address matches with socket remote IP address.
        //    OR this socket had transmitted packet with destination address as broadcast.
        if ( p->localPort == h->DestinationPort.Val)
        {
            if ( (p->remotePort == h->SourcePort.Val) && (p->RxCount == 0L) )
            {
                if ( (p->remoteNode.IPAddr.Val == remoteNode->IPAddr.Val) ||
                     (destIP->Val == BROADCAST_ADDRESS) )
                {
                    return s;
                }
            }

            partialMatch = s;
        }
        p++;
    }

    //If any UDP packet was received that is addressed to the port of any of our UDPSocketInfo
    //objects, than we set the NODE_INFO of that USPSocketInfo object with the NODE_INFO
    //of the received packet. The NODE_INFO is the MAC address and IP address
    if ( partialMatch != INVALID_UDP_SOCKET )
    {
        p = &UDPSocketInfo[partialMatch];

        memcpy((void*)&p->remoteNode,
                (const void*)remoteNode, sizeof(p->remoteNode) );

        p->remotePort = h->SourcePort.Val;
    }
    return partialMatch;
}

#endif //#if defined(STACK_USE_UDP)
