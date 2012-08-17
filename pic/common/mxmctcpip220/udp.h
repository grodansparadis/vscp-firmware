/*********************************************************************
 *
 *                  UDP Module Defs for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        UDP.h
 * Dependencies:    stacktsk.h
 *                  MAC.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
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
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     3/19/01  Original        (Rev 1.0)
 ********************************************************************/

#ifndef UDP_H
#define UDP_H

#include "projdefs.h"

#include "ip.h"
#include "mac.h"


typedef WORD UDP_PORT;
typedef BYTE UDP_SOCKET;

typedef struct _UDP_SOCKET_INFO
{
    NODE_INFO   remoteNode;	//10 bytes
    UDP_PORT    remotePort;	//2 bytes
    UDP_PORT    localPort;	//2 bytes
    WORD        TxCount;	//2 bytes
    WORD        RxCount;	//2 bytes
    BUFFER      TxBuffer;	
    WORD        TxOffset;

    struct
    {
        unsigned int bFirstRead     : 1;
    } Flags;

} UDP_SOCKET_INFO;


#define INVALID_UDP_SOCKET      (0xff)
#define INVALID_UDP_PORT        (0L)

/*
 * All module utilizing UDP module will get extern definition of
 * activeUDPSocket.  While UDP module itself will define activeUDPSocket.
 */
#if !defined(THIS_IS_UDP_MODULE)
    extern UDP_SOCKET activeUDPSocket;
    extern UDP_SOCKET_INFO  UDPSocketInfo[MAX_UDP_SOCKETS];
#endif


typedef struct _UDP_HEADER
{
    UDP_PORT    SourcePort;
    UDP_PORT    DestinationPort;
    WORD        Length;
    WORD        Checksum;
} UDP_HEADER;


/*********************************************************************
 * Function:        void UDPInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initializes internal variables.
 *
 * Note:
 ********************************************************************/
void        UDPInit(void);


/*********************************************************************
 * Function:        UDP_SOCKET UDPOpen(UDP_PORT localPort,
 *                                     NODE_INFO *remoteNode,
 *                                     UDP_PORT remotePort)
 *
 * PreCondition:    UDPInit() is already called
 *
 * Input:           remoteNode - Remote Node info such as MAC and IP
 *                               address
 *                               If NULL, localPort is opened for
 *                               Listen.
 *                  remotePort - Remote Port to which to talk to
 *                               If INVALID_UDP_SOCKET, localPort is
 *                               opened for Listen.
 *                  localPort  - A non-zero port number.
 *
 * Output:          A valid UDP socket that is to be used for
 *                  subsequent UDP communications.
 *
 * Side Effects:    None
 *
 * Overview:        A UDP packet header is assembled and loaded into
 *                  UDP transmit buffer.
 *
 * Note:            A localPort value of '0' is considered nonexistent
 *                  port.  This call must always have nonzero localPort
 *                  value.
 *                  This function sets returned socket as an active
 *                  UDP socket.
  ********************************************************************/
UDP_SOCKET UDPOpen(UDP_PORT localPort,
                   NODE_INFO *remoteNode,
                   UDP_PORT remotePort);


/*********************************************************************
 * Function:        void UDPClose(UDP_SOCKET s)
 *
 * PreCondition:    UDPOpen() is already called
 *
 * Input:           s       - Socket that is to be closed.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Given socket is marked as available for future
 *                  new communcations.
 *
 * Note:            This function does not affect previous
 *                  active UDP socket designation.
  ********************************************************************/
void UDPClose(UDP_SOCKET s);


/*********************************************************************
 * Macro:           BOOL UDPIsPutReady(UDP_SOCKET s)
 *
 * PreCondition:
 *
 * Input:           s       - Socket that is to be loaded and made
 *                            an active UDP socket.
 *
 * Output:          TRUE if at least one UDP buffer is ready to transmit
 *                  FALSE if no UDP buffer is ready
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            This call sets given socket as an active UDP socket.
 ********************************************************************/
#define UDPIsPutReady(s)        (activeUDPSocket = s, MACIsTxReady())


/*********************************************************************
 * Function:        BOOL UDPPut(BYTE v)
 *
 * PreCondition:    UDPIsPutReady() == TRUE with desired UDP socket
 *                  that is to be loaded.
 *
 * Input:           v       - Data byte to loaded into transmit buffer
 *
 * Output:          TRUE if transmit buffer is still ready to accept
 *                  more data bytes
 *
 *                  FALSE if transmit buffer can no longer accept
 *                  any more data byte.
 *
 * Side Effects:    None
 *
 * Overview:        Given data byte is put into UDP transmit buffer
 *                  and active UDP socket buffer length is incremented
 *                  by one.
 *                  If buffer has become full, FALSE is returned.
 *                  Or else TRUE is returned.
 *
 * Note:            This function loads data into an active UDP socket
 *                  as determined by previous call to UDPIsPutReady()
 ********************************************************************/
BOOL UDPPut(BYTE v);


/*********************************************************************
 * Function:        BOOL UDPFlush(void)
 *
 * PreCondition:    UDPPut() is already called and desired UDP socket
 *                  is set as an active socket by calling
 *                  UDPIsPutReady().
 *
 * Input:           None
 *
 * Output:          All and any data associated with active UDP socket
 *                  buffer is marked as ready for transmission.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            This function transmit all data from
 *                  an active UDP socket.
 ********************************************************************/
void UDPFlush(void);



/*********************************************************************
 * Function:        BOOL UDPIsGetReady(UDP_SOCKET s)
 *
 * PreCondition:    UDPInit() is already called.
 *
 * Input:           A valid UDP socket that is already "Listen"ed on
 *                  or opened.
 *
 * Output:          TRUE if given port contains any data.
 *                  FALSE if given port does not contain any data.
 *
 * Side Effects:    Given socket is set as an active UDP Socket.
 *
 * Overview:        None
 *
 * Note:            This function automatically sets supplied socket
 *                  as an active socket.  Caller need not call
 *                  explicit function UDPSetActiveSocket().  All
 *                  subsequent calls will us this socket as an
 *                  active socket.
 ********************************************************************/
BOOL UDPIsGetReady(UDP_SOCKET s);



/*********************************************************************
 * Function:        BOOL UDPGet(BYTE *v)
 *
 * PreCondition:    UDPInit() is already called     AND
 *                  UDPIsGetReady(s) == TRUE
 *
 * Input:           v       - Buffer to receive UDP data byte
 *
 * Output:          TRUE    if a data byte was read
 *                  FALSE   if no data byte was read or available
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            This function fetches data from an active UDP
 *                  socket as set by UDPIsGetReady() call.
 ********************************************************************/
BOOL UDPGet(BYTE *v);


/*********************************************************************
 * Function:        void UDPDiscard(void)
 *
 * PreCondition:    UDPInit() is already called    AND
 *                  UDPIsGetReady() == TRUE with desired UDP socket.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            This function discards an active UDP socket content.
 ********************************************************************/
void UDPDiscard(void);



/*********************************************************************
 * Function:        BOOL UDPProcess(NODE_INFO* remoteNode,
 *                                  IP_ADDR *localIP,
 *                                  WORD len)
 *
 * PreCondition:    UDPInit() is already called     AND
 *                  UDP segment is ready in MAC buffer
 *
 * Input:           remoteNode      - Remote node info
 *                  len             - Total length of UDP semgent.
 *                  localIP         - The IP Address of the currently received packet.
 *
 * Output:          TRUE if this function has completed its task
 *                  FALSE otherwise
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
BOOL        UDPProcess(NODE_INFO *remoteNode, IP_ADDR *localIP, WORD len);


/*********************************************************************
 * Macro:           UDPSetTxBuffer(a, b)
 *
 * PreCondition:    None
 *
 * Input:           a       - Buffer identifier
 *                  b       - Offset
 *
 * Output:          Next Read/Write access to transmit buffer 'a'
 *                  set to offset 'b'
 *
 * Side Effects:    None
 *
 * Note:            None
 *
 ********************************************************************/
#define UDPSetTxBuffer(a, b) (UDPSocketInfo[activeUDPSocket].TxOffset = b,  IPSetTxBuffer(a, b+sizeof(UDP_HEADER)))


/*********************************************************************
 * Macro:           UDPSetRxBuffer(a)
 *
 * PreCondition:    None
 *
 * Input:           a       - Offset
 *
 * Output:          Next Read/Write access to receive buffer is
 *                  set to offset 'b'
 *
 * Side Effects:    None
 *
 * Note:            None
 *
 ********************************************************************/
#define UDPSetRxBuffer(a) IPSetRxBuffer(a+sizeof(UDP_HEADER))


#endif
