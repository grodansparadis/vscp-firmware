/*********************************************************************
 *
 *                  TCP Module for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        TCP.C
 * Dependencies:    string.h
 *                  stacktsk.h
 *                  Helpers.h
 *                  IP.h
 *                  MAC.h
 *                  ARP.h
 *                  Tick.h
 *                  TCP.h
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
 * HiTech PICC18 Compiler Options excluding device selection:
 *                  -FAKELOCAL -G -O -Zg -E -C
 *
 *
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     5/8/01  Original        (Rev 1.0)
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 * Nilesh Rajbharti     11/1/02 Fixed TCPTick() SYN Retry bug.
 * Nilesh Rajbharti     12/5/02 Modified TCPProcess() to include localIP as third param.
 *                      This was done to allow this function to calculate checksum correctly.
********************************************************************/
#define THIS_IS_TCP

#include <string.h>

#include "tcp.h"
#include "debug.h"
#include "appcfg.h"
#include "helpers.h"
#include "ip.h"
#include "mac.h"
#include "tick.h"
#include "delay.h"

#if (DEBUG_TCP > LOG_OFF)
static ROM char DebugMsgCksmOK[] = "\rT:CksmOK";
static ROM char DebugMsgCksmErr[] = "\rT:CksmErr";
static ROM char DebugMsgCksm[] = "\rT:Cksm ";
static ROM char DebugMsgSktMatch[] = "\rT:SktMtch";
static ROM char DebugMsgSktUnknown[] = "\rT:SktUkn";
static ROM char DebugMsgEndline[] = "\r";
#endif

/*
 * Max TCP data length is MAC_TX_BUFFER_SIZE - sizeof(TCP_HEADER) -
 * sizeof(IP_HEADER) - sizeof(ETHER_HEADER)
 */
#define MAX_TCP_DATA_LEN    (MAC_TX_BUFFER_SIZE - 54)

/*
 * TCP Flags as defined by rfc793
 */
#define FIN     (0x01)
#define SYN     (0x02)
#define RST     (0x04)
#define PSH     (0x08)
#define ACK     (0x10)
#define URG     (0x20)


/*
 * TCP Header def. as per rfc 793.
 */

typedef struct _TCP_HEADER
{
    WORD    SourcePort;
    WORD    DestPort;
    DWORD   SeqNumber;
    DWORD   AckNumber;

    struct
    {
        unsigned int Reserved3      : 4;
        unsigned int Val            : 4;
    } DataOffset;


    union
    {
        struct
        {
            unsigned int flagFIN    : 1;
            unsigned int flagSYN    : 1;
            unsigned int flagRST    : 1;
            unsigned int flagPSH    : 1;
            unsigned int flagACK    : 1;
            unsigned int flagURG    : 1;
            unsigned int Reserved2  : 2;
        } bits;
        BYTE byte;
    } Flags;

    WORD    Window;
    WORD    Checksum;
    WORD    UrgentPointer;

} TCP_HEADER;


/*
 * TCP Options as defined by rfc 793
 */
#define TCP_OPTIONS_END_OF_LIST     (0x00)
#define TCP_OPTIONS_NO_OP           (0x01)
#define TCP_OPTIONS_MAX_SEG_SIZE    (0x02)
typedef struct _TCP_OPTIONS
{
    BYTE        Kind;
    BYTE        Length;
    WORD_VAL    MaxSegSize;
} TCP_OPTIONS;

//Swap bytes of TCPLength field
#define SwapPseudoTCPHeader(h)  (h.TCPLength = swaps(h.TCPLength))

/*
 * Pseudo header as defined by rfc 793.
 */
typedef struct _PSEUDO_HEADER
{
    IP_ADDR SourceAddress;
    IP_ADDR DestAddress;
    BYTE Zero;
    BYTE Protocol;
    WORD TCPLength;
} PSEUDO_HEADER;

/*
 * These are all sockets supported by this TCP.
 * Seeing that this segment can be larger than a bank (256 bytes) it has to be defined it's own
 * segment.
 * - For MPLAB C18 compiler we have to add an entry in the linker 
 * - For the Hi-Tech compiler nothing has to be done, compiler does it all
 */
#if defined(MCHP_C18)
    #pragma udata TCP_SOCKET_INFO_SEG
    SOCKET_INFO TCB[MAX_SOCKETS];
    #pragma udata   //Return to default section
#endif

#if defined(HITECH_C18)
    SOCKET_INFO TCB[MAX_SOCKETS];
#endif


/*
 * Local temp port numbers.
 */
static WORD _NextPort;

/*
 * Starting segment sequence number for each new connection.
 */
static DWORD ISS;


static  void    HandleTCPSeg(TCP_SOCKET s,
                               NODE_INFO *remote,
                               TCP_HEADER *h,
                               WORD len);

static void TransmitTCP(NODE_INFO *remote,
                        TCP_PORT localPort,
                        TCP_PORT remotePort,
                        DWORD seq,
                        DWORD ack,
                        BYTE flags,
                        BUFFER buffer,
                        WORD len);

static  TCP_SOCKET FindMatchingSocket(TCP_HEADER *h,
                                    NODE_INFO *remote);
static  void    SwapTCPHeader(TCP_HEADER* header);
static void CloseSocket(SOCKET_INFO* ps);


#define SendTCP(remote, localPort, remotePort, seq, ack, flags)     \
        TransmitTCP(remote, localPort, remotePort, seq, ack, flags, \
                    INVALID_BUFFER, 0)




/*********************************************************************
 * Function:        void TCPInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TCP is initialized.
 *
 * Side Effects:    None
 *
 * Overview:        Initialize all socket info.
 *
 * Note:            This function is called only one during lifetime
 *                  of the application.
 ********************************************************************/
void TCPInit(void)
{
    TCP_SOCKET s;
    SOCKET_INFO* ps;


    // Initialize all sockets.
    for ( s = 0; s < MAX_SOCKETS; s++ )
    {
        ps = &TCB[s];

        ps->smState             = TCP_CLOSED;
        ps->Flags.bServer       = FALSE;
        ps->Flags.bIsPutReady   = TRUE;
        ps->Flags.bFirstRead    = TRUE;
        ps->Flags.bIsTxInProgress = FALSE;
        ps->Flags.bIsGetReady   = FALSE;
        ps->TxBuffer            = INVALID_BUFFER;
        ps->TimeOut             = TCP_START_TIMEOUT_VAL;
    }

    _NextPort = TCP_LOCAL_PORT_START_NUMBER;
    ISS = 0;
}



/*********************************************************************
 * Function:        TCP_SOCKET TCPListen(TCP_PORT port)
 *
 * PreCondition:    TCPInit() is already called.
 *
 * Input:           port    - A TCP port to be opened.
 *
 * Output:          Given port is opened and returned on success
 *                  INVALID_SOCKET if no more sockets left.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
TCP_SOCKET TCPListen(TCP_PORT port)
{
    TCP_SOCKET s;
    SOCKET_INFO* ps;

    for ( s = 0; s < MAX_SOCKETS; s++ )
    {
        ps = &TCB[s];

        if ( ps->smState == TCP_CLOSED )
        {
            /*
             * We have a CLOSED socket.
             * Initialize it with LISTENing state info.
             */
            ps->smState             = TCP_LISTEN;
            ps->localPort           = port;
            ps->remotePort          = 0;

            /*
             * There is no remote node IP address info yet.
             */
            ps->remote.IPAddr.Val   = 0x00;

            /*
             * If a socket is listened on, it is a SERVER.
             */
            ps->Flags.bServer       = TRUE;

            ps->Flags.bIsGetReady   = FALSE;
            ps->TxBuffer            = INVALID_BUFFER;
            ps->Flags.bIsPutReady   = TRUE;

            return s;
        }
    }
    return INVALID_SOCKET;
}



/*********************************************************************
 * Function:        TCP_SOCKET TCPConnect(NODE_INFO* remote,
 *                                      TCP_PORT remotePort)
 *
 * PreCondition:    TCPInit() is already called.
 *
 * Input:           remote      - Remote node address info
 *                  remotePort  - remote port to be connected.
 *
 * Output:          A new socket is created, connection request is
 *                  sent and socket handle is returned.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            By default this function is not included in
 *                  source.  You must define STACK_CLIENT_MODE to
 *                  be able to use this function.
 ********************************************************************/
#ifdef STACK_CLIENT_MODE
TCP_SOCKET TCPConnect(NODE_INFO *remote, TCP_PORT remotePort)
{
    TCP_SOCKET s;
    SOCKET_INFO* ps;
    BOOL lbFound;


    lbFound = FALSE;
    /*
     * Find an available socket
     */
    for ( s = 0; s < MAX_SOCKETS; s++ )
    {
        ps = &TCB[s];
        if ( ps->smState == TCP_CLOSED )
        {
            lbFound = TRUE;
            break;
        }
    }

    /*
     * If there is no socket available, return error.
     */
    if ( lbFound == FALSE )
        return INVALID_SOCKET;

    /*
     * Each new socket that is opened by this node, gets
     * next sequential port number.
     */
    ps->localPort = ++_NextPort;
    if ( _NextPort > TCP_LOCAL_PORT_END_NUMBER )
        _NextPort = TCP_LOCAL_PORT_START_NUMBER;

    /*
     * This is a client socket.
     */
    ps->Flags.bServer = FALSE;

    /*
     * This is the port, we are trying to connect to.
     */
    ps->remotePort = remotePort;

    /*
     * Each new socket that is opened by this node, will
     * start with next segment seqeuence number.
     */
    ps->SND_SEQ = ++ISS;
    ps->SND_ACK = 0;

    memcpy((BYTE*)&ps->remote, (const void*)remote, sizeof(ps->remote));

    /*
     * Send SYN message.
     */
    SendTCP(&ps->remote,
            ps->localPort,
            ps->remotePort,
            ps->SND_SEQ,
            ps->SND_ACK,
            SYN);

    ps->smState = TCP_SYN_SENT;
    ps->SND_SEQ++;

    return s;
}
#endif



/*********************************************************************
 * Function:        BOOL TCPIsConnected(TCP_SOCKET s)
 *
 * PreCondition:    TCPInit() is already called.
 *
 * Input:           s       - Socket to be checked for connection.
 *
 * Output:          TRUE    if given socket is connected
 *                  FALSE   if given socket is not connected.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            A socket is said to be connected if it is not
 *                  in LISTEN and CLOSED mode.  Socket may be in
 *                  SYN_RCVD or FIN_WAIT_1 and may contain socket
 *                  data.
 ********************************************************************/
BOOL TCPIsConnected(TCP_SOCKET s)
{
    return ( TCB[s].smState == TCP_EST );
}



/*********************************************************************
 * Function:        void TCPDisconnect(TCP_SOCKET s)
 *
 * PreCondition:    TCPInit() is already called     AND
 *                  TCPIsPutReady(s) == TRUE
 *
 * Input:           s       - Socket to be disconnected.
 *
 * Output:          A disconnect request is sent for given socket.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
void TCPDisconnect(TCP_SOCKET s)
{
    SOCKET_INFO *ps;

    ps = &TCB[s];

    /*
     * If socket is not connected, may be it is already closed
     * or in process of closing.  Since user has called this
     * explicitly, close it forcefully.
     */
    if ( ps->smState != TCP_EST )
    {
        CloseSocket(ps);
        return;
    }


    /*
     * Discard any outstand data that is to be read.
     */
    TCPDiscard(s);

    /*
     * Send FIN message.
     */
    SendTCP(&ps->remote,
            ps->localPort,
            ps->remotePort,
            ps->SND_SEQ,
            ps->SND_ACK,
            FIN | ACK);

        ps->SND_SEQ++;

    ps->smState = TCP_FIN_WAIT_1;

    return;
}

/*********************************************************************
 * Function:        BOOL TCPFlush(TCP_SOCKET s)
 *
 * PreCondition:    TCPInit() is already called.
 *
 * Input:           s       - Socket whose data is to be transmitted.
 *
 * Output:          All and any data associated with this socket
 *                  is marked as ready for transmission.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
BOOL TCPFlush(TCP_SOCKET s)
{
    SOCKET_INFO *ps;

    ps = &TCB[s];

    // Make sure that this already a TxBuffer assigned to this
    // socket.
    if ( ps->TxBuffer == INVALID_BUFFER )
        return FALSE;

    if ( ps->Flags.bIsPutReady == FALSE )
        return FALSE;

    TransmitTCP(&ps->remote,
                ps->localPort,
                ps->remotePort,
                ps->SND_SEQ,
                ps->SND_ACK,
                ACK,
                ps->TxBuffer,
                ps->TxCount);
    ps->SND_SEQ += (DWORD)ps->TxCount;
    ps->Flags.bIsPutReady       = FALSE;
    ps->Flags.bIsTxInProgress   = FALSE;

#ifdef TCP_NO_WAIT_FOR_ACK
    MACDiscardTx(ps->TxBuffer);
    ps->TxBuffer                = INVALID_BUFFER;
    ps->Flags.bIsPutReady       = TRUE;
#endif

    return TRUE;
}



/*********************************************************************
 * Function:        BOOL TCPIsPutReady(TCP_SOCKET s)
 *
 * PreCondition:    TCPInit() is already called.
 *
 * Input:           s       - socket to test
 *
 * Output:          TRUE if socket 's' is free to transmit
 *                  FALSE if socket 's' is not free to transmit.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            Each socket maintains only transmit buffer.
 *                  Hence until a data packet is acknowledeged by
 *                  remote node, socket will not be ready for
 *                  next transmission.
 *                  All control transmission such as Connect,
 *                  Disconnect do not consume/reserve any transmit
 *                  buffer.
 ********************************************************************/
BOOL TCPIsPutReady(TCP_SOCKET s)
{
    if ( TCB[s].TxBuffer == INVALID_BUFFER )
        return IPIsTxReady();
    else
        return TCB[s].Flags.bIsPutReady;
}




/*********************************************************************
 * Function:        BOOL TCPPut(TCP_SOCKET s, BYTE byte)
 *
 * PreCondition:    TCPIsPutReady() == TRUE
 *
 * Input:           s       - socket to use
 *                  byte    - a data byte to send
 *
 * Output:          TRUE if given byte was put in transmit buffer
 *                  FALSE if transmit buffer is full.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
BOOL TCPPut(TCP_SOCKET s, BYTE byte)
{
    WORD tempTxCount;       // This is a fix for HITECH bug
    SOCKET_INFO* ps;

    ps = &TCB[s];

    if ( ps->TxBuffer == INVALID_BUFFER )
    {
        ps->TxBuffer = MACGetTxBuffer();

        // This function is used to transmit data only.  And every data packet
        // must be ack'ed by remote node.  Until this packet is ack'ed by
        // remote node, we must preserve its content so that we can retransmit
        // if we need to.
        MACReserveTxBuffer(ps->TxBuffer);

        ps->TxCount = 0;

        IPSetTxBuffer(ps->TxBuffer, sizeof(TCP_HEADER));
    }

    /*
     * HITECH does not seem to compare ps->TxCount as it is.
     * Copying it to local variable and then comparing seems to work.
     */
    tempTxCount = ps->TxCount;
    if ( tempTxCount >= MAX_TCP_DATA_LEN )
        return FALSE;

    ps->Flags.bIsTxInProgress = TRUE;

    MACPut(byte);

    // REMOVE
    //tempTxCount = ps->TxCount;
    tempTxCount++;
    ps->TxCount = tempTxCount;

    //ps->TxCount++;
    //tempTxCount = ps->TxCount;
    if ( tempTxCount >= MAX_TCP_DATA_LEN )
        TCPFlush(s);
    //if ( TCB[s].TxCount >= MAX_TCP_DATA_LEN )
    //    TCPFlush(s);

    return TRUE;
}



/*********************************************************************
 * Function:        BOOL TCPDiscard(TCP_SOCKET s)
 *
 * PreCondition:    TCPInit() is already called.
 *
 * Input:           s       - socket
 *
 * Output:          TRUE if socket received data was discarded
 *                  FALSE if socket received data was already
 *                          discarded.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
BOOL TCPDiscard(TCP_SOCKET s)
{
    SOCKET_INFO* ps;

    ps = &TCB[s];

    //This socket must contain data for it to be discarded.
    //The bIsGetReady flag indicates if the MAC RX Buffer pointer is currently set to the
    //packet containing this socket?
    if ( !ps->Flags.bIsGetReady )
        return FALSE;

    //Remove the currently read packet (packet containing this socket) from the MAC RX Buffer
    MACRxbufDiscard();
    ps->Flags.bIsGetReady = FALSE;

    return TRUE;
}




/*********************************************************************
 * Function:        WORD TCPGetArray(TCP_SOCKET s, BYTE *buffer,
 *                                      WORD count)
 *
 * PreCondition:    TCPInit() is already called     AND
 *                  TCPIsGetReady(s) == TRUE
 *
 * Input:           s       - socket
 *                  buffer  - Buffer to hold received data.
 *                  count   - Buffer length
 *
 * Output:          Number of bytes loaded into buffer.
 *
 * Side Effects:    None
 *
 * Overview:        Get the given number of bytes from the TCP data. The MAC RX Ring Buffer pointer
 *                  has to currently point to the TCP data area of the current received packet.
 *
 * Note:            None
 ********************************************************************/
WORD TCPGetArray(TCP_SOCKET s, BYTE *buffer, WORD count)
{
    SOCKET_INFO *ps;

    ps = &TCB[s];

    //The bIsGetReady flag indicates if the MAC RX Buffer pointer is currently set to the
    //packet containing this socket?
    if ( ps->Flags.bIsGetReady )
    {
        if ( ps->Flags.bFirstRead )
        {
            // Position read pointer to first byte of TCP data. Skips the TCP header.
            IPSetRxBuffer(sizeof(TCP_HEADER));

            ps->Flags.bFirstRead = FALSE;
        }

        ps->Flags.bIsTxInProgress = TRUE;

        //Read the requested amount of data from the current MAC RX Buffer
        return MACRxbufGetArray(buffer, count);
    }
    else
        return 0;
}



/*********************************************************************
 * Function:        BOOL TCPGet(TCP_SOCKET s, BYTE *byte)
 *
 * PreCondition:    TCPInit() is already called     AND
 *                  TCPIsGetReady(s) == TRUE
 *
 * Input:           s       - socket
 *                  byte    - Pointer to a byte.
 *
 * Output:          TRUE if a byte was read.
 *                  FALSE if byte was not read.
 *
 * Side Effects:    None
 *
 * Overview:        This function reads the next byte from the current TCP packet.
 *
 * Note:            None
 ********************************************************************/
BOOL TCPGet(TCP_SOCKET s, BYTE *byte)
{
    SOCKET_INFO* ps;

    ps = &TCB[s];

    if ( ps->Flags.bIsGetReady )
    {
        if ( ps->Flags.bFirstRead )
        {
            // Position read pointer to first byte of TCP data. Skips the TCP header.
            IPSetRxBuffer(sizeof(TCP_HEADER));

            ps->Flags.bFirstRead = FALSE;
        }

        //Has all TCP Data been read?
        if ( ps->RxCount == 0 )
        {
            MACRxbufDiscard();
            ps->Flags.bIsGetReady = FALSE;
            return FALSE;
        }

        ps->RxCount--;
        //Read a single byte from the current MAC Receive buffer
        *byte = MACRxbufGet();
        return TRUE;
    }
    return FALSE;
}



/*********************************************************************
 * Function:        BOOL TCPIsGetReady(TCP_SOCKET s)
 *
 * PreCondition:    TCPInit() is already called.
 *
 * Input:           s       - socket to test
 *
 * Output:          TRUE if socket 's' contains any data.
 *                  FALSE if socket 's' does not contain any data.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
BOOL TCPIsGetReady(TCP_SOCKET s)
{
    /*
     * A socket is said to be "Get" ready when it has already
     * received some data.  Sometime, a socket may be closed,
     * but it still may contain data.  Thus in order to ensure
     * reuse of a socket, caller must make sure that it reads
     * a socket, if is ready.
     */
    return (TCB[s].Flags.bIsGetReady );
}



/*********************************************************************
 * Function:        void TCPTick(void)
 *
 * PreCondition:    TCPInit() is already called.
 *
 * Input:           None
 *
 * Output:          Each socket FSM is executed for any timeout
 *                  situation.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
void TCPTick(void)
{
#if 1       //!defined(TCP_NO_WAIT_FOR_ACK)
    TCP_SOCKET s;
    TICK diffTicks;
    TICK tick;
    SOCKET_INFO* ps;
    DWORD seq;
    BYTE flags;

    flags = 0x00;
    /*
     * Periodically all "not closed" sockets must perform timed operation.
     */
    for ( s = 0; s < MAX_SOCKETS; s++ )
    {
        ps = &TCB[s];

        if ( ps->Flags.bIsGetReady || ps->Flags.bIsTxInProgress )
            continue;


        /*
         * Closed or Passively Listening socket do not care
         * about timeout conditions.
         */
        if ( (ps->smState == TCP_CLOSED) ||
             (ps->smState == TCP_LISTEN &&
              ps->Flags.bServer == TRUE) )
            continue;

        tick = TickGet();

        // Calculate timeout value for this socket.
        diffTicks = TickGetDiff(tick, ps->startTick);

        // If timeout has not occured, do not do anything.
        if ( diffTicks <= ps->TimeOut )
            continue;

        /*
         * All states require retransmission, so check for transmitter
         * availability right here - common for all.
         */
        if ( !IPIsTxReady() )
            return;

        // Restart timeout reference.
        ps->startTick = TickGet();

        // Update timeout value if there is need to wait longer.
        ps->TimeOut++;

        // This will be one more attempt.
        ps->RetryCount++;

        /*
         * A timeout has occured.  Respond to this timeout condition
         * depending on what state this socket is in.
         */
        switch(ps->smState)
        {
        case TCP_SYN_SENT:
            /*
             * Keep sending SYN until we hear from remote node.
             * This may be for infinite time, in that case
             * caller must detect it and do something.
             * Bug Fix: 11/1/02
             */
            flags = SYN;
            break;

        case TCP_SYN_RCVD:
            /*
             * We must receive ACK before timeout expires.
             * If not, resend SYN+ACK.
             * Abort, if maximum attempts counts are reached.
             */
            if ( ps->RetryCount < TCP_MAX_RETRY_COUNTS )
            {
                flags = SYN | ACK;
            }
            else
                CloseSocket(ps);
            break;

        case TCP_EST:
#if !defined(TCP_NO_WAIT_FOR_ACK)
            /*
             * Don't let this connection idle for very long time.
             * If we did not receive or send any message before timeout
             * expires, close this connection.
             */
            if ( ps->RetryCount <= TCP_MAX_RETRY_COUNTS )
            {
                if ( ps->TxBuffer != INVALID_BUFFER )
                {
                    MACSetTxBuffer(ps->TxBuffer, 0);
                    MACFlush();
                }
                else
                    flags = ACK;
            }
            else
            {
                // Forget about previous transmission.
                if ( ps->TxBuffer != INVALID_BUFFER )
                    MACDiscardTx(ps->TxBuffer);
                ps->TxBuffer = INVALID_BUFFER;

#endif
                // Request closure.
                flags = FIN | ACK;

                ps->smState = TCP_FIN_WAIT_1;
#if !defined(TCP_NO_WAIT_FOR_ACK)
            }
#endif
            break;

        case TCP_FIN_WAIT_1:
        case TCP_LAST_ACK:
            /*
             * If we do not receive any response to out close request,
             * close it outselves.
             */
            if ( ps->RetryCount > TCP_MAX_RETRY_COUNTS )
                CloseSocket(ps);
            else
                flags = FIN;
            break;

        case TCP_CLOSING:
        case TCP_TIMED_WAIT:
            /*
             * If we do not receive any response to out close request,
             * close it outselves.
             */
            if ( ps->RetryCount > TCP_MAX_RETRY_COUNTS )
                CloseSocket(ps);
            else
                flags = ACK;
            break;
        }

        if ( flags > 0x00 )
        {
            if ( flags != ACK )
                seq = ps->SND_SEQ++;
            else
                seq = ps->SND_SEQ;

            SendTCP(&ps->remote,
                    ps->localPort,
                    ps->remotePort,
                    seq,
                    ps->SND_ACK,
                    flags);
        }
    }
#else
    return;
#endif
}

/*
WORD CalcIPBufferChecksumTest(WORD len)
{
    BOOL lbMSB;
    WORD_VAL checkSum;
    BYTE c;
    WORD_VAL adrDMA;
    WORD_VAL adrDMAOld;

    lbMSB = TRUE;
    checkSum.Val = 0;


    #if (DEBUG_TCP >= LOG_INFO)
    debugPutRomString(DebugMsgEndline);
    debugPutRomString(DebugMsgEndline);
    #endif

    IPSetRxBuffer(0);   //Set to first byte of TCP header
    //Set current address of Remote DMA
    adrDMA.Val = MACGetNICAddr();

    while( len-- )
    {
        adrDMA.Val = MACGetNICAddr();

        c = MACRxbufGet();  //Read a single byte from the current MAC Receive buffer

        #if (DEBUG_TCP >= LOG_INFO)
        if ((adrDMA.v[0] & 0x0f) == 0) {
            debugPutByte('\r');
            debugPutByte('(');
            debugPutByteHex(adrDMA.v[1]);
            debugPutByteHex(adrDMA.v[0]);
            debugPutByte(')');
        }
        else {
            debugPutByte(' ');
        }
        debugPutByteHex(c);
        #endif

        if ( !lbMSB )
        {
            if ( (checkSum.v[0] = c + checkSum.v[0]) < c)
            {
                if ( ++checkSum.v[1] == 0 )
                    checkSum.v[0]++;
            }
        }
        else
        {
            if ( (checkSum.v[1] = c+checkSum.v[1]) < c)
            {
                if ( ++checkSum.v[0] == 0 )
                    checkSum.v[1]++;
            }
        }

        lbMSB = !lbMSB;
    }

    checkSum.v[1] = ~checkSum.v[1];
    checkSum.v[0] = ~checkSum.v[0];

    #if (DEBUG_TCP >= LOG_INFO)
    debugPutRomString(DebugMsgEndline);
    debugPutRomString(DebugMsgEndline);
    #endif

    return checkSum.Val;
}
*/



/*********************************************************************
 * Function:        BOOL TCPProcess(NODE_INFO* remote,
 *                                  IP_ADDR *localIP,
 *                                  WORD len)
 *
 * PreCondition:    TCPInit() is already called     AND
 *                  TCP segment is ready in MAC buffer
 *
 * Input:           remote      - Remote node info
 *                  len         - Total length of TCP semgent = (TCP Header + TCP Data)
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
BOOL TCPProcess(NODE_INFO *remote, IP_ADDR *localIP, WORD len)
{
    TCP_HEADER      TCPHeader;
    PSEUDO_HEADER   pseudoHeader;
    TCP_SOCKET      socket;
    WORD_VAL        checksum;
    BYTE            optionsSize;
    #if (DEBUG_TCP > LOG_OFF)
    BYTE tmp;
    #endif
    WORD_VAL        checksumBak;


    /////////////////////////////////////////////////
    //Read header and check checksum

    // Retrieve TCP header. The data is read from the current MAC RX Buffer
    MACRxbufGetArray((BYTE*)&TCPHeader, sizeof(TCPHeader));

    //Swap all WORDs and DWORDs in header   
    SwapTCPHeader(&TCPHeader);

    // Calculate TCP pseudoheader checksum. To calculate the TCP checksum, not only all bytes in
    //the TCP header have to be taken into account, but also bytes of the pseudoheader. This is a
    //a 12 byte segment created with data from TCP and IP
    pseudoHeader.SourceAddress      = remote->IPAddr;
    pseudoHeader.DestAddress.v[0]   = localIP->v[0];
    pseudoHeader.DestAddress.v[1]   = localIP->v[1];
    pseudoHeader.DestAddress.v[2]   = localIP->v[2];
    pseudoHeader.DestAddress.v[3]   = localIP->v[3];
    pseudoHeader.Zero               = 0x0;
    pseudoHeader.Protocol           = IP_PROT_TCP;
    pseudoHeader.TCPLength          = len;  //The length of the TCP segment, including both header
                                            //and data. Note that this is not a specific field in
                                            //the TCP header; it is computed.

    //Swap bytes of TCPLength field - all other bytes are already correct
    SwapPseudoTCPHeader(pseudoHeader);

    checksum.Val = ~CalcIPChecksum((BYTE*)&pseudoHeader,
                                    sizeof(pseudoHeader));

    checksumBak.Val = checksum.Val;    // TEST TEST

    //Set TCP packet checksum = pseudo header checksum in MAC RAM.
    //Position RX buffer access p pointer to 16 bytes into IP data. In this case the IP
    //data is a TCP packet, and offset 16 = Checksum
    IPSetRxBuffer(16);
    MACPut(checksum.v[0]);
    MACPut(checksum.v[1]);
    IPSetRxBuffer(0);   //Set to first byte of TCP header

    // Now calculate TCP packet checksum in NIC RAM - including
    // pesudo header.
    checksum.Val = CalcIPBufferChecksum(len);

    if ( checksum.Val != TCPHeader.Checksum )
    {
        #if (DEBUG_TCP >= LOG_WARN)
        //Write HEX value of calculated and actual checksums
        debugPutRomString(DebugMsgCksmErr);
        debugPutRomString(DebugMsgCksm);
        debugPutByteHex(checksum.v[1]);
        debugPutByteHex(checksum.v[0]);
        debugPutByte(' ');
        tmp = (BYTE)(TCPHeader.Checksum >> 8);
        debugPutByteHex(tmp);
        tmp = (BYTE)TCPHeader.Checksum;
        debugPutByteHex(tmp);
        debugPutByte(' ');
        tmp = (BYTE)(len >> 8);
        debugPutByteHex(tmp);
        tmp = (BYTE)len;
        debugPutByteHex(tmp);
        #endif

        MACRxbufDiscard();
        return TRUE;
    }
    else {
        #if (DEBUG_TCP >= LOG_DEBUG)
        debugPutRomString(DebugMsgCksmOK);
        #endif
    }

    //Set len = length of TCP data. 
    optionsSize = (BYTE)((TCPHeader.DataOffset.Val << 2)-
                            sizeof(TCPHeader));
    len = len - optionsSize - sizeof(TCPHeader);

    // Position packet read pointer to start of data area.
    IPSetRxBuffer((TCPHeader.DataOffset.Val << 2));


    /////////////////////////////////////////////////
    //Find a matching socket, if any
    socket = FindMatchingSocket(&TCPHeader, remote);
    if ( socket < INVALID_SOCKET )
    {
        #if (DEBUG_TCP >= LOG_DEBUG)
        debugPutRomString(DebugMsgSktMatch);
        #endif

        //This TCP socket is handled by us (addressed to one our of sockets) - handle it
        HandleTCPSeg(socket, remote, &TCPHeader, len);
    }
    else
    {
        #if (DEBUG_TCP >= LOG_WARN)
        debugPutRomString(DebugMsgSktUnknown);
        #endif

        /*
         * If this is an unknown socket, discard it and
         * send RESET to remote node.
         */
        MACRxbufDiscard();

        if ( socket == UNKNOWN_SOCKET )
        {

            TCPHeader.AckNumber += len;
            if ( TCPHeader.Flags.bits.flagSYN ||
                 TCPHeader.Flags.bits.flagFIN )
                TCPHeader.AckNumber++;

            SendTCP(remote,
                    TCPHeader.DestPort,
                    TCPHeader.SourcePort,
                    TCPHeader.AckNumber,
                    TCPHeader.SeqNumber,
                    RST);
        }

    }

    return TRUE;
}


/*********************************************************************
 * Function:        static void TransmitTCP(NODE_INFO* remote
 *                                          TCP_PORT localPort,
 *                                          TCP_PORT remotePort,
 *                                          DWORD seq,
 *                                          DWORD ack,
 *                                          BYTE flags,
 *                                          BUFFER buffer,
 *                                          WORD len)
 *
 * PreCondition:    TCPInit() is already called     AND
 *                  TCPIsPutReady() == TRUE
 *
 * Input:           remote      - Remote node info
 *                  localPort   - Source port number
 *                  remotePort  - Destination port number
 *                  seq         - Segment sequence number
 *                  ack         - Segment acknowledge number
 *                  flags       - Segment flags
 *                  buffer      - Buffer to which this segment
 *                                is to be transmitted
 *                  len         - Total data length for this segment.
 *
 * Output:          A TCP segment is assembled and put to transmit.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
static void TransmitTCP(NODE_INFO *remote,
                        TCP_PORT localPort,
                        TCP_PORT remotePort,
                        DWORD tseq,
                        DWORD tack,
                        BYTE flags,
                        BUFFER buffer,
                        WORD len)
{
    WORD_VAL        checkSum;
    TCP_HEADER      header;
    TCP_OPTIONS     options;
    PSEUDO_HEADER   pseudoHeader;

    /*
     * When using SLIP (USART), packet transmission takes some time
     * and hence before sending another packet, we must make sure
     * that, last packet is transmitted.
     * When using ethernet controller, transmission occurs very fast
     * and by the time next packet is transmitted, previous is
     * transmitted and we do not need to check for last packet.
     */
    while( !IPIsTxReady() );

    header.SourcePort           = localPort;
    header.DestPort             = remotePort;
    header.SeqNumber            = tseq;
    header.AckNumber            = tack;
    header.Flags.bits.Reserved2 = 0;
    header.DataOffset.Reserved3 = 0;
    header.Flags.byte           = flags;
    // Receive window = MAC Free buffer size - TCP header (20) - IP header (20)
    //                  - ETHERNET header (14 if using NIC) .
    header.Window               = MACGetFreeRxSize();
#if !defined(STACK_USE_SLIP)
    /*
     * Limit one segment at a time from remote host.
     * This limit increases overall throughput as remote host does not
     * flood us with packets and later retry with significant delay.
     */
    if ( header.Window >= MAC_RX_BUFFER_SIZE )
        header.Window = MAC_RX_BUFFER_SIZE;

    else if ( header.Window > 54 )
    {
        header.Window -= 54;
    }
    else
        header.Window = 0;
#else
    if ( header.Window > 40 )
    {
        header.Window -= 40;
    }
    else
        header.Window = 0;
#endif

    header.Checksum             = 0;
    header.UrgentPointer        = 0;

    SwapTCPHeader(&header);

    len += sizeof(header);

    if ( flags & SYN )
    {
        len += sizeof(options);
        options.Kind = TCP_OPTIONS_MAX_SEG_SIZE;
        options.Length = 0x04;

        // Load MSS in already swapped order.
        options.MaxSegSize.v[0]  = (MAC_RX_BUFFER_SIZE >> 8); // 0x05;
        options.MaxSegSize.v[1]  = (MAC_RX_BUFFER_SIZE & 0xff); // 0xb4;

        header.DataOffset.Val   = (sizeof(header) + sizeof(options)) >> 2;
    }
    else
        header.DataOffset.Val   = sizeof(header) >> 2;


    // Calculate IP pseudoheader checksum.
    pseudoHeader.SourceAddress.v[0] = MY_IP_BYTE1;
    pseudoHeader.SourceAddress.v[1] = MY_IP_BYTE2;
    pseudoHeader.SourceAddress.v[2] = MY_IP_BYTE3;
    pseudoHeader.SourceAddress.v[3] = MY_IP_BYTE4;
    pseudoHeader.DestAddress    = remote->IPAddr;
    pseudoHeader.Zero           = 0x0;
    pseudoHeader.Protocol       = IP_PROT_TCP;
    pseudoHeader.TCPLength      = len;

    SwapPseudoTCPHeader(pseudoHeader);

    header.Checksum = ~CalcIPChecksum((BYTE*)&pseudoHeader,
                        sizeof(pseudoHeader));
    checkSum.Val = header.Checksum;

    if ( buffer == INVALID_BUFFER )
        buffer = MACGetTxBuffer();

    IPSetTxBuffer(buffer, 0);

    // Write IP header.
    IPPutHeader(remote, IP_PROT_TCP, len);
    IPPutArray((BYTE*)&header, sizeof(header));

    if ( flags & SYN )
        IPPutArray((BYTE*)&options, sizeof(options));

    IPSetTxBuffer(buffer, 0);

    checkSum.Val = CalcIPBufferChecksum(len);

    // Update the checksum.
    IPSetTxBuffer(buffer, 16);
    MACPut(checkSum.v[1]);
    MACPut(checkSum.v[0]);
    MACSetTxBuffer(buffer, 0);

    MACFlush();
}


/*********************************************************************
 * Function:        static TCP_SOCKET FindMatchingSocket(TCP_HEADER *h,
 *                                      NODE_INFO* remote)
 *
 * PreCondition:    TCPInit() is already called
 *
 * Input:           h           - TCP Header to be matched against.
 *                  remote      - Node who sent this header.
 *
 * Output:          A socket that matches with given header and remote
 *                  node is searched.
 *                  If such socket is found, its index is returned
 *                  else INVALID_SOCKET is returned.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
static TCP_SOCKET FindMatchingSocket(TCP_HEADER *h, NODE_INFO *remote)
{
    SOCKET_INFO *ps;
    TCP_SOCKET s;
    TCP_SOCKET partialMatch;

    partialMatch = INVALID_SOCKET;

    for ( s = 0; s < MAX_SOCKETS; s++ )
    {
        ps = &TCB[s];

        if ( ps->smState != TCP_CLOSED )
        {
            if ( ps->localPort == h->DestPort )
            {
                if ( ps->smState == TCP_LISTEN )
                    partialMatch = s;

                if ( ps->remotePort == h->SourcePort &&
                     ps->remote.IPAddr.Val == remote->IPAddr.Val )
                {
                        return s;
                }
            }
        }
    }

    ps = &TCB[partialMatch];

    if ( partialMatch != INVALID_SOCKET &&
         ps->smState == TCP_LISTEN )
    {
        memcpy((void*)&ps->remote, (void*)remote, sizeof(*remote));
        //ps->remote              = *remote;
        ps->localPort           = h->DestPort;
        ps->remotePort          = h->SourcePort;
        ps->Flags.bIsGetReady   = FALSE;
        ps->TxBuffer            = INVALID_BUFFER;
        ps->Flags.bIsPutReady   = TRUE;

        return partialMatch;
    }

    if ( partialMatch == INVALID_SOCKET )
        return UNKNOWN_SOCKET;
    else
        return INVALID_SOCKET;
}






/*********************************************************************
 * Function:        static void SwapTCPHeader(TCP_HEADER* header)
 *
 * PreCondition:    None
 *
 * Input:           header      - TCP Header to be swapped.
 *
 * Output:          Given header is swapped.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
static void SwapTCPHeader(TCP_HEADER* header)
{
    header->SourcePort      = swaps(header->SourcePort);
    header->DestPort        = swaps(header->DestPort);
    header->SeqNumber       = swapl(header->SeqNumber);
    header->AckNumber       = swapl(header->AckNumber);
    header->Window          = swaps(header->Window);
    header->Checksum        = swaps(header->Checksum);
    header->UrgentPointer   = swaps(header->UrgentPointer);
}



/*********************************************************************
 * Function:        static void CloseSocket(SOCKET_INFO* ps)
 *
 * PreCondition:    TCPInit() is already called
 *
 * Input:           ps  - Pointer to a socket info that is to be
 *                          closed.
 *
 * Output:          Given socket information is reset and any
 *                  buffer held by this socket is discarded.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
static void CloseSocket(SOCKET_INFO* ps)
{
    if ( ps->TxBuffer != INVALID_BUFFER )
    {
        MACDiscardTx(ps->TxBuffer);
        ps->TxBuffer            = INVALID_BUFFER;
        ps->Flags.bIsPutReady   = TRUE;
    }

    ps->remote.IPAddr.Val = 0x00;
    ps->remotePort = 0x00;
    if ( ps->Flags.bIsGetReady )
    {
        MACRxbufDiscard();
    }
    ps->Flags.bIsGetReady       = FALSE;
    ps->TimeOut                 = TCP_START_TIMEOUT_VAL;

    ps->Flags.bIsTxInProgress   = FALSE;

    if ( ps->Flags.bServer )
        ps->smState = TCP_LISTEN;
    else
        ps->smState = TCP_CLOSED;
    return;
}



/*********************************************************************
 * Function:        static void HandleTCPSeg(TCP_SOCKET s,
 *                                      NODE_INFO *remote,
 *                                      TCP_HEADER* h,
 *                                      WORD len)
 *
 * PreCondition:    TCPInit() is already called     AND
 *                  TCPProcess() is the caller.
 *
 * Input:           s           - Socket that owns this segment
 *                  remote      - Remote node info
 *                  h           - TCP Header
 *                  len         - Length of TCP data. This does NOT include the header length!
 *
 * Output:          TCP FSM is executed on given socket with
 *                  given TCP segment.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
static void HandleTCPSeg(TCP_SOCKET s,
                        NODE_INFO *remote,
                        TCP_HEADER *h,
                        WORD len)
{
    DWORD ack;
    DWORD seq;
    DWORD prevAck, prevSeq;
    SOCKET_INFO *ps;
    BYTE flags;


    flags = 0x00;
    ps = &TCB[s];

    /*
     * Remember current seq and ack for our connection so that if
     * we have to silently discard this packet, we can go back to
     * previous ack and seq numbers.
     */
    prevAck = ps->SND_ACK;
    prevSeq = ps->SND_SEQ;

    ack = h->SeqNumber ;
    ack += (DWORD)len;
    seq = ps->SND_SEQ;

    /*
     * Clear retry counts and timeout tick counter.
     */
    ps->RetryCount  = 0;
    ps->startTick   = TickGet();
    ps->TimeOut = TCP_START_TIMEOUT_VAL;

    if ( ps->smState == TCP_LISTEN )
    {
        MACRxbufDiscard();

        ps->SND_SEQ     = ++ISS;
        ps->SND_ACK     = ++ack;
        seq             = ps->SND_SEQ;
        ++ps->SND_SEQ;
        if ( h->Flags.bits.flagSYN )
        {
            /*
             * This socket has received connection request.
             * Remember calling node, assign next segment seq. number
             * for this potential connection.
             */
            memcpy((void*)&ps->remote, (const void*)remote, sizeof(*remote));
            ps->remotePort  = h->SourcePort;

            /*
             * Grant connection request.
             */
            flags           = SYN | ACK;
            ps->smState     = TCP_SYN_RCVD;

        }
        else
        {
            /*
             * Anything other than connection request is ignored in
             * LISTEN state.
             */
            flags               = RST;
            seq                 = ack;
            ack                 = h->SeqNumber;
            ps->remote.IPAddr.Val = 0x00;
        }

    }
    else
    {
        /*
         * Reset FSM, if RST is received.
         */
        if ( h->Flags.bits.flagRST )
        {
            MACRxbufDiscard();

            CloseSocket(ps);

            return;

        }

        else if ( seq == h->AckNumber )
        {
            if ( ps->smState == TCP_SYN_RCVD )
            {
                if ( h->Flags.bits.flagACK )
                {
                    ps->SND_ACK = ack;
                    ps->RetryCount = 0;
                    ps->startTick = TickGet();
                    ps->smState = TCP_EST;

                    if ( len > 0 )
                    {
                        ps->Flags.bIsGetReady   = TRUE;
                        ps->RxCount             = len;  //Set RxCount = Length of TCP Data (NOT including header, only data)
                        ps->Flags.bFirstRead    = TRUE;
                    }
                    else
                        MACRxbufDiscard();
                }
                else
                {
                    MACRxbufDiscard();
                }
            }
            else if ( ps->smState == TCP_SYN_SENT )
            {
                if ( h->Flags.bits.flagSYN )
                {
                    ps->SND_ACK = ++ack;
                    if ( h->Flags.bits.flagACK )
                    {
                        flags = ACK;
                        ps->smState = TCP_EST;
                    }
                    else
                    {
                        flags = SYN | ACK;
                        ps->smState = TCP_SYN_RCVD;
                        ps->SND_SEQ = ++seq;
                    }

                    if ( len > 0 )
                    {
                        ps->Flags.bIsGetReady   = TRUE;
                        ps->RxCount             = len;  //Set RxCount = Length of TCP Data (NOT including header, only data)
                        ps->Flags.bFirstRead    = TRUE;
                    }
                    else
                        MACRxbufDiscard();
                }
                else
                {
                    MACRxbufDiscard();
                }
            }
            else
            {
                if ( h->SeqNumber != ps->SND_ACK )
                {
                    // Discard buffer.
                    MACRxbufDiscard();
                    return;
                }

                ps->SND_ACK = ack;

                if ( ps->smState == TCP_EST )
                {
                    if ( h->Flags.bits.flagACK )
                    {
                        if ( ps->TxBuffer != INVALID_BUFFER )
                        {
                            MACDiscardTx(ps->TxBuffer);
                            ps->TxBuffer            = INVALID_BUFFER;
                            ps->Flags.bIsPutReady   = TRUE;
                        }
                    }

                    if ( h->Flags.bits.flagFIN )
                    {
                        flags = FIN | ACK;
                        seq = ps->SND_SEQ++;
                        ack = ++ps->SND_ACK;
                        ps->smState = TCP_LAST_ACK;
                    }


                    if ( len > 0 )
                    {
                        if ( !ps->Flags.bIsGetReady )
                        {
                            ps->Flags.bIsGetReady   = TRUE;
                            ps->RxCount             = len;  //Set RxCount = Length of TCP Data (NOT including header, only data)
                            ps->Flags.bFirstRead    = TRUE;

                             // 4/1/02
                            flags = ACK;
                       }
                        else
                        {
                            /*
                             * Since we cannot accept this packet,
                             * restore to previous seq and ack.
                             * and do not send anything back.
                             * Host has to resend this packet when
                             * we are ready.
                             */
                            flags = 0x00;
                            ps->SND_SEQ = prevSeq;
                            ps->SND_ACK = prevAck;

                            MACRxbufDiscard();
                        }
                    }
                    else
                    {
                        MACRxbufDiscard();
                    }


                }
                else if ( ps->smState == TCP_LAST_ACK )
                {
                    MACRxbufDiscard();

                    if ( h->Flags.bits.flagACK )
                    {
                        CloseSocket(ps);
                    }
                }
                else if ( ps->smState == TCP_FIN_WAIT_1 )
                {
                    MACRxbufDiscard();

                    if ( h->Flags.bits.flagFIN )
                    {
                        flags = ACK;
                        ack = ++ps->SND_ACK;
                        if ( h->Flags.bits.flagACK )
                        {
                            CloseSocket(ps);
                        }
                        else
                        {
                            ps->smState = TCP_CLOSING;
                        }
                    }
                }
                else if ( ps->smState == TCP_CLOSING )
                {
                    MACRxbufDiscard();

                    if ( h->Flags.bits.flagACK )
                    {
                        CloseSocket(ps);
                    }
                }
            }
        }
        else
        {
            MACRxbufDiscard();
        }
    }

    if ( flags > 0x00 )
    {
        SendTCP(remote,
                h->DestPort,
                h->SourcePort,
                seq,
                ack,
                flags);
    }
}

/*********************************************************************
 * Function:        SOCKET_INFO TCPGetSocketInfo(TCP_SOCKET socket)
 *
 * PreCondition:    TCPInit() is already called.
 *
 * Input:           s       - socket
 *
 * Output:          Returns a pointer to the SOCKET_INFO structure for the given socket.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
SOCKET_INFO* TCPGetSocketInfo(TCP_SOCKET s)
{
    return &TCB[s];
}
