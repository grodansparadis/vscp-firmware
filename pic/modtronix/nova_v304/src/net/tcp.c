/**
 * @brief           TCP Module for Modtronix TCP/IP Stack
 * @file            tcp.c
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
 * 2001-05-08, Nilesh Rajbharti:
 *    - Original Rev 1.0
 * 2002-05-22, Nilesh Rajbharti:
 *    - Rev 2.0 (See version.log for detail)
 * 2002-11-01, Nilesh Rajbharti:
 *    - Fixed TCPTick() SYN Retry bug.
 * 2002-12-05, Nilesh Rajbharti:
 *    - Modified TCPProcess() to include destIP as third param.
 *      This was done to allow this function to calculate checksum correctly.
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 * 2005-11-23, David Hosken (DH):
 *    - Added "ps->startTick = TickGet();" line to end of TCPConnect() function.
 *********************************************************************/
 
/**
 Open Issues:

========== From forum.modtronix.com ========= 
Someone helps me figure out to comment the line
#define TCP_NO_WAIT_FOR_ACK (in projdef.h)
I commented that line and now, when the board send 1 package to PC and receive back package with ACK, it
stops to send. I debugged and found out that when the PIC receive the package, it interpreted the wrong
SEQ number and ACK number. The SEQ # seems to be random while the ACK # seems to be off (I expected
0x00000007 and got 0x0000000A or 0x00000003 or 0x00000008). I checked with ethereal and verified that the
computer sends correct information. Does anyone have any idea about it? 

========== From forum.modtronix.com =========
The timeout is not set up and this results in a second connect packet being issued
immediatly after the first one. This certainly caused problems with the server. Add following line
to end of TCPConnect() function.
ps->startTick   = TickGet16bit();

 */

#define THIS_IS_TCP

#include <string.h>

#include "projdefs.h"
#include "net\tcp.h"
#include "net\helpers.h"
#include "net\ip.h"
#include "net\mac.h"
#include "net\tick.h"
#include "net\delay.h"
#include "debug.h"

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0x8F, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0x8F, msgCode, msgStr)

#if (MAX_SOCKETS <= 14)
    #define debugPutOffsetMsg(offset, msgCode) debugPut2Bytes(0x80 + offset, msgCode)
#else
    /* If there are more then 14 Sockets, then */
    #define debugPutOffsetMsg(offset, msgCode) \
        if (offset <= 14) debugPut2Bytes(0x80 + offset, msgCode); \
        else { \
            debugPut2Bytes(0x80, offset); \
            debugPutByte(msgCode); \
        }
#endif


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
    WORD_VAL    SourcePort;
    WORD_VAL    DestPort;
    DWORD       SeqNumber;
    DWORD       AckNumber;

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

    WORD_VAL    Window;
    WORD_VAL    Checksum;
    WORD_VAL    UrgentPointer;

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
                               TCP_HEADER *hdr,
                               WORD len);

static void TransmitTCP(NODE_INFO *remote,
                        WORD localPort,
                        WORD remotePort,
                        DWORD seq,
                        DWORD ack,
                        BYTE flags,
                        BUFFER buffer,
                        WORD len);

static  TCP_SOCKET FindMatchingSocket(TCP_HEADER *hdr,
                                    NODE_INFO *remote);
static  void    SwapTCPHeader(TCP_HEADER* header);
static void CloseSocket(SOCKET_INFO* ps);

/**
 * Sends a TCP Message with the given data. The sent TCP Message will contain only a "TCP Header" with
 * possible "TCP Options". No "TCP Data" is transmitted. This function is normally used to sent TCP
 * handshaking data, like SYN, ACK, FIN...
 */
#define SendTCP(remote, localPort, remotePort, seq, ack, flags)     \
        TransmitTCP(remote, localPort, remotePort, seq, ack, flags, \
                    INVALID_BUFFER, 0)




/**
 * Initialize all socket info.
 * This function is called only one during lifetime of the application.
 */
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


/**
 * Do a TCP passive open for the given port.
 *
 * @preCondition    TCPInit() is already called.
 *
 * @param port      A TCP port to be opened.
 *
 * @return          Given port is opened and returned on success
 *                  INVALID_SOCKET if no more sockets left.
 */
TCP_SOCKET TCPListen(WORD port)
{
    TCP_SOCKET s;
    SOCKET_INFO* ps;

    //Search for a CLOSED socket, and if found, initialize it to LISTEN on given port
    for ( s = 0; s < MAX_SOCKETS; s++ )
    {
        ps = &TCB[s];

        if ( ps->smState == TCP_CLOSED )
        {
            // We have a CLOSED socket. Initialize it with LISTENing state info.
            ps->smState             = TCP_LISTEN;
            ps->localPort           = port;
            ps->remotePort          = 0;

            //There is no remote node IP address info yet.
            ps->remote.IPAddr.Val   = 0x00;

            //If a socket is listened on, it is a SERVER.
            ps->Flags.bServer       = TRUE;

            ps->Flags.bIsGetReady   = FALSE;
            
            //By assigning the TxBuffer with INVALID_BUFFER, the first available TX Buffer will be used
            //the first time the TCPPut() or TCPPutArray() function is called.
            ps->TxBuffer            = INVALID_BUFFER;
            
            ps->Flags.bIsPutReady   = TRUE;

            #if (DEBUG_TCP >= LOG_INFO)
            debugPutOffsetMsg(s, 9);    //@mxd:9:state LISTEN: Passive open done for port 0x%x%x
            debugPutByteHex((BYTE)(port >> 8));
            debugPutByteHex((BYTE)port);
            #endif
            
            return s;
        }
    }
    return INVALID_SOCKET;
}


/**
 * Do an TCP active open.
 * By default this function is not included in source. You must define
 * STACK_CLIENT_MODE to be able to use this function.
 *
 * @preCondition    TCPInit() is already called.
 *
 * @param remote    Remote node address info
 * @param remotePort remote port to be connected.
 *
 * @return          A new socket is created, connection request is
 *                  sent and socket handle is returned.
 */
#ifdef STACK_CLIENT_MODE
TCP_SOCKET TCPConnect(NODE_INFO *remote, WORD remotePort)
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
    if ( lbFound == FALSE ) {
        #if (DEBUG_TCP >= LOG_ERROR)
        debugPutOffsetMsg(s, 7);    //@mxd:7:Could not connect, no free sockets available
        #endif
        return INVALID_SOCKET;
    }

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

    //Send SYN message. Sends a TCP Message with the given data. The sent TCP Message will contain
    //only a "TCP Header" with possible "TCP Options". No "TCP Data" is transmitted.
    SendTCP(&ps->remote,
            ps->localPort,
            ps->remotePort,
            ps->SND_SEQ,
            ps->SND_ACK,
            SYN);


    #if (DEBUG_TCP >= LOG_INFO)
    debugPutOffsetMsg(s, 8);    //@mxd:8:state SYN-SENT: In the process of doing an active open
    #endif
    ps->smState = TCP_SYN_SENT;
    ps->SND_SEQ++;

    ps->startTick = TickGet16bit();  //Set timeout reference

    return s;
}
#endif


/**
 * A socket is said to be connected if it is not
 * in LISTEN and CLOSED mode.  Socket may be in
 * SYN_RCVD or FIN_WAIT_1 and may contain socket data.
 *
 * @preCondition    TCPInit() is already called.
 *
 * @param s         Socket to be checked for connection.
 *
 * @return          TRUE    if given socket is connected <br>
 *                  FALSE   if given socket is not connected.
 */
BOOL TCPIsConnected(TCP_SOCKET s)
{
    return ( TCB[s].smState == TCP_EST );
}


/**
 * A disconnect request is sent for given socket.
 *
 * @preCondition    TCPInit() is already called AND<br>
 *                  TCPIsPutReady(s) == TRUE
 *
 * @param s         Socket to be disconnected.
 */
void TCPDisconnect(TCP_SOCKET s)
{
    SOCKET_INFO *ps;

    ps = &TCB[s];

    //If socket is not connected, may be it is already closed or in process of closing.
    //Since user has called this explicitly, close it forcefully.
    if ( ps->smState != TCP_EST )
    {
        #if (DEBUG_TCP >= LOG_WARN)
        if ( ps->Flags.bServer )
            debugPutOffsetMsg(s, 25);    //@mxd:10:state LISTEN: TCPDisconnect clossed connection, no message was sent!"
        else
            debugPutOffsetMsg(s, 26);    //@mxd:11:state CLOSED: TCPDisconnect clossed connection, no message was sent!"
        #endif

        CloseSocket(ps);
        return;
    }


    /*
     * Discard any outstand data that is to be read.
     */
    TCPDiscard(s);

    //Send FIN message. Sends a TCP Message with the given data. The sent TCP Message will contain
    //only a "TCP Header" with possible "TCP Options". No "TCP Data" is transmitted.
    SendTCP(&ps->remote,
            ps->localPort,
            ps->remotePort,
            ps->SND_SEQ,
            ps->SND_ACK,
            FIN | ACK);

        ps->SND_SEQ++;

    #if (DEBUG_TCP >= LOG_INFO)
    debugPutOffsetMsg(s, 4);    //@mxd:4:state FIN-WAIT-1: Disconnect called, sent FIN|ACK message to target
    #endif

    ps->smState = TCP_FIN_WAIT_1;

    return;
}


/**
 * All and any data associated with this socket
 * is marked as ready for transmission.
 *
 * @preCondition    TCPInit() is already called.
 *
 * @param s         Socket whose data is to be transmitted.
 *
 * @return          TRUE is action was performed <br>
 *                  FALSE if not
 */
BOOL TCPFlush(TCP_SOCKET s)
{
    SOCKET_INFO *ps;

    ps = &TCB[s];

    // Make sure that there is a TxBuffer assigned to this socket.
    if ( ps->TxBuffer == INVALID_BUFFER ) {
        #if (DEBUG_TCP >= LOG_ERROR)
        debugPutOffsetMsg(s, 45);    //@mxd:45:Flush called with TxBuffer = INVALID_BUFFER!
        #endif
        return FALSE;
    }

    if ( ps->Flags.bIsPutReady == FALSE ) {
        #if (DEBUG_TCP >= LOG_ERROR)
        debugPutOffsetMsg(s, 46);    //@mxd:46:Flush called with bIsPutReady = FALSE!
        #endif
        return FALSE;
    }

    //A TCP segment is assembled with the given data, and transmitted. This function will send the
    //Ethernet Header (MAC Header), IP Header and IP Data (which is the TCP Header and TCP Data in this case).
    //The ps->TxBuffer must contain the TCP Data that has to be sent!
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
    
    //Is set as soon as the TCPPut() or TCPPutArray() functions is called for this socket, cleared when flushed
    ps->Flags.bIsTxInProgress   = FALSE;


    //If we do NOT wait for ACK, we can immediately discard the TxBuffer's data. If not, we have to keep it
    //in case we have to resend the data!
    #ifdef TCP_NO_WAIT_FOR_ACK
    MACDiscardTx(ps->TxBuffer); //Discard buffer, and mark it as available
    
    //By assigning the TxBuffer with INVALID_BUFFER, the first available TX Buffer will be used
    //the first time the TCPPut() or TCPPutArray() function is called.
    ps->TxBuffer                = INVALID_BUFFER;
    
    ps->Flags.bIsPutReady       = TRUE;
    #endif

    return TRUE;
}


/**
 * Each socket maintains only one transmit buffer. Hence until a data packet
 * is acknowledeged by remote node, socket will not be ready for next transmission.
 * All control transmission such as Connect, Disconnect do not
 * consume/reserve any transmit buffer. This function will check:
 * - If the given socket is valid, is not equal to INVALID_SOCKET for example
 * - If there is an available TX Buffer for writing data to via TCPPut() and TCPPutArray() functions.
 * - If the given socket is ready for transmission
 *
 * @preCondition    TCPInit() is already called.
 *
 * @param s         socket to test
 *
 * @return          TRUE if socket 's' is free to transmit, and it is a valid socket<br>
 *                  FALSE if socket 's' is not free to transmit, or an invalid socket was given.
 */
BOOL TCPIsPutReady(TCP_SOCKET s)
{
    //Check if the given socket is valid
    if (s < MAX_SOCKETS) {
        //If this socket's buffer = INVAlID_BUFFER, it means it will be assigned a free buffer when the
        //TCPPut() or TCPPutArray() function is called. Call the IPIsTxReady() function to see if there
        //are any free TX Buffers. This is the case when there is currently no unsent data in this socket's
        //TX Buffer. This socket will be assigned a free buffer during the TCPPut() and TCPPutArray() functions.
        if ( TCB[s].TxBuffer == INVALID_BUFFER )
            return IPIsTxReady();
        //This flag is valid when the socket's TX Buffer contains unsent data.
        else
            return TCB[s].Flags.bIsPutReady;
    }
    
    return FALSE;
}


/**
 * Write the given byte to the given socket's transmit buffer.
 * The data is NOT sent yet, and the TCPFlush() function must be called
 * to send all data contained in the transmit buffer.
 *
 * @preCondition    TCPIsPutReady() == TRUE
 *
 * @param s         socket to use
 * @param byte      a data byte to send
 *
 * @return          TRUE if transmit buffer is still ready to accept more data bytes <br>
 *                  FALSE if transmit buffer can no longer accept any more data byte.<br>
 *                  If FALSE is returned, then TCPIsPutReady() has to be called again before
 *                  calling TCPPut() or TCPPutArray() functions!
 */
BOOL TCPPut(TCP_SOCKET s, BYTE byte)
{
    SOCKET_INFO* ps;

    ps = &TCB[s];

    //This TCP Socket does not contain any unsent data, and currently does not own a TX Buffer!
    //Get next free TX Buffer and assign it to this socket.
    if ( ps->TxBuffer == INVALID_BUFFER )
    {
        //Get handle to next available TX Buffer. The TCPIsPutReady() function that has to be called
        //prior to this function will determine if there is an available TX Buffer.
        ps->TxBuffer = MACGetTxBuffer();

        // This function is used to transmit data only.  And every data packet
        // must be ack'ed by remote node.  Until this packet is ack'ed by
        // remote node, we must preserve its content so that we can retransmit
        // if we need to.
        MACReserveTxBuffer(ps->TxBuffer);

        ps->TxCount = 0;

        //Set TX Buffer access pointer (All future read and writes to the TX Buffer will be to the set location)
        //to given offset in "IP data", which is the first byte after the "TCP Header" = first byte of
        //the "TCP Data". Thus, future reads and writes to TX Buffer will access "TCP Data"
        IPSetTxBuffer(ps->TxBuffer, sizeof(TCP_HEADER));
    }

    if ( ps->TxCount >= TCPGetMaxDataLength() )
        return FALSE;

    //Is set as soon as the TCPPut() or TCPPutArray() function is called for this socket, cleared when flushed
    ps->Flags.bIsTxInProgress = TRUE;

    MACPut(byte);

    //Increment TxCount
    ps->TxCount++;

    //If buffer is full, flush it
    if ( ps->TxCount >= TCPGetMaxDataLength() ) {
        TCPFlush(s);
    }

    return TRUE;
}


/**
 * Given number of data bytes from the given array are put into the given socket's
 * transmit buffer. The data is NOT sent yet, and the TCPFlush() function must be called
 * to send all data contained in the transmit buffer.
 *
 * If there is not enough space in the transmit buffer for all the data, the contents of
 * the transmit buffer will be sent, and this function will return the actual amount of
 * bytes that were sent. In this case, it is VERY IMPORTANT to call the TCPIsPutReady()
 * function again before calling the TCPPut() or TCPPutArray() functions! This will however
 * only happen if the transmit buffer fills up. The transmit buffer for TCP data is
 * = (MAC_TX_BUFFER_SIZE - 54), which is usually 970 bytes. If writing less then this to
 * the transmit buffer before calling TCPFlush(), then this function will always return the
 * requested number of bytes!
 *
 * @preCondition    TCPIsPutReady() == TRUE
 *
 * @param s         socket to use
 * @param[in] buffer Buffer containing data that has to be sent.
 * @param count     Number of bytes to send
 *
 * @return          TRUE if transmit buffer is still ready to accept more data bytes <br>
 *                  FALSE if transmit buffer can no longer accept any more data byte.<br>
 *                  If FALSE is returned, then TCPIsPutReady() has to be called again before
 *                  calling TCPPut() or TCPPutArray() functions!
 */
BOOL TCPPutArray(TCP_SOCKET s, BYTE *buffer, WORD count)
{
    SOCKET_INFO* ps;

    ps = &TCB[s];

    //This TCP Socket does not contain any unsent data, and currently does not own a TX Buffer!
    //Get next free TX Buffer and assign it to this socket.
    if ( ps->TxBuffer == INVALID_BUFFER )
    {
        //Get handle to next available TX Buffer. The TCPIsPutReady() function that has to be called
        //prior to this function will determine if there is an available TX Buffer.
        ps->TxBuffer = MACGetTxBuffer();

        // This function is used to transmit data only.  And every data packet
        // must be ack'ed by remote node.  Until this packet is ack'ed by
        // remote node, we must preserve its content so that we can retransmit
        // if we need to.
        MACReserveTxBuffer(ps->TxBuffer);

        ps->TxCount = 0;

        //Set TX Buffer access pointer (All future read and writes to the TX Buffer will be to the set location)
        //to given offset in "IP data", which is the first byte after the "TCP Header" = first byte of
        //the "TCP Data". Thus, future reads and writes to TX Buffer will access "TCP Data"
        IPSetTxBuffer(ps->TxBuffer, sizeof(TCP_HEADER));
    }

    if ( ps->TxCount >= TCPGetMaxDataLength() )
        return 0;
        
    //This function request more bytes to be written to the TX Buffer then there is space 
    if ((ps->TxCount + count) > TCPGetMaxDataLength()) {
        //Update count with maximum number of bytes that there is place for in the TX Buffer
        count = TCPGetMaxDataLength() - ps->TxCount;
    }

    //Is set as soon as the TCPPut() or TCPPutArray() function is called for this socket, cleared when flushed
    ps->Flags.bIsTxInProgress = TRUE;

    //Write buffer
    MACPutArray(buffer, count);

    //Increment TxCount
    ps->TxCount += count;

    //If buffer is full, flush it
    if ( ps->TxCount >= TCPGetMaxDataLength() ) {
        TCPFlush(s);
    }

    return count;
}


/**
 * Discard any data contained in the given socket's receive buffer.
 *
 * @preCondition    TCPInit() is already called.
 *
 * @param s         socket
 *
 * @return          TRUE if socket received data was discarded <br>
 *                  FALSE if socket received data was already discarded
 */
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


/**
 * This function reads the next byte from the current TCP packet.
 * Reads a single byte from the given socket into the given byte pointer
 *
 * @preCondition    TCPInit() is already called     AND
 *                  TCPIsGetReady(s) == TRUE
 *
 * @param s         socket
 * @param byte      Pointer to a byte.
 *
 * @return          TRUE if a byte was read.
 *                  FALSE if byte was not read.
 */
BOOL TCPGet(TCP_SOCKET s, BYTE *byte)
{
    SOCKET_INFO* ps;

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


/**
 * Read the requested number of bytes from the given socket into the given buffer.
 *
 * @preCondition    TCPInit() is already called AND <br>
 *                  TCPIsGetReady(s) == TRUE
 *
 * @param s         socket
 * @param[in] buffer Buffer to hold received data.
 * @param count     Buffer length
 *
 * @return          Number of bytes loaded into buffer.
 */
WORD TCPGetArray(TCP_SOCKET s, BYTE *buffer, WORD count)
{
    WORD bytesRead;
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
        
        //If more bytes requested then are left in the receive buffer, adjust count
        if (count > ps->RxCount) {
            count = ps->RxCount;
        }

        //Has all TCP Data been read?
        if ( ps->RxCount == 0 )
        {
            MACRxbufDiscard();
            ps->Flags.bIsGetReady = FALSE;
            return FALSE;
        }

        //Read the requested amount of data from the current MAC RX Buffer
        return MACRxbufGetArray(buffer, count);
    }
    else
        return 0;
}


/**
 * A socket is said to be "Get" ready when it has already received some data.
 * Sometime, a socket may be closed, but it still may contain data.
 * Thus in order to ensure reuse of a socket, caller must make sure that it reads
 * a socket, if it is ready. This function also checks that the given socket is
 * valid, is not equal to INVALID_SOCKET for example
 *
 * @preCondition    TCPInit() is already called.
 *
 * @param s         socket to test
 *
 * @return          TRUE if socket 's' contains any data.<br>
 *                  FALSE if socket 's' does not contain any data.
 */
BOOL TCPIsGetReady(TCP_SOCKET s)
{
    //Check if the given socket is valid
    if (s < MAX_SOCKETS) {
        //A socket is said to be "Get" ready when it has already received some data.
        //Sometime, a socket may be closed, but it still may contain data. Thus in
        //order to ensure reuse of a socket, caller must make sure that it reads
        //a socket, if is ready.
        return (TCB[s].Flags.bIsGetReady );
    }
    
    return FALSE;
}


/**
 * Each socket FSM is executed for any timeout situation.
 * 
 * @preCondition    TCPInit() is already called.
 */
void TCPTick(void)
{
#if 1       //!defined(TCP_NO_WAIT_FOR_ACK)
    TCP_SOCKET s;
    SOCKET_INFO* ps;
    DWORD seq;
    BYTE flags;

    flags = 0x00;
    //Periodically all "not closed" sockets must perform timed operation.
    for ( s = 0; s < MAX_SOCKETS; s++ )
    {
        ps = &TCB[s];

        // - bIsTxInProgress is set as soon as the TCPPut() or TCPPutArray() functions is called for a
        //   socket, cleared when flushed
        if ( ps->Flags.bIsGetReady || ps->Flags.bIsTxInProgress )
            continue;


        //Closed or Passively Listening socket do not care about timeout conditions.
        if ( (ps->smState == TCP_CLOSED) ||
             (ps->smState == TCP_LISTEN &&
              ps->Flags.bServer == TRUE) )
            continue;

        // If timeout has not occured, do not do anything.
        if (TickGetDiff16bit(ps->startTick) <= ps->TimeOut )
            continue;

        //All states require retransmission, so check for transmitter
        //availability right here - common for all.
        if ( !IPIsTxReady() )
            return;

        // Restart timeout reference.
        ps->startTick = TickGet16bit();

        // Update timeout value if there is need to wait longer.
        ps->TimeOut++;

        // This will be one more attempt.
        ps->RetryCount++;

        //A timeout has occured.  Respond to this timeout condition
        //depending on what state this socket is in.
        switch(ps->smState)
        {
        case TCP_SYN_SENT:
            #if (DEBUG_TCP >= LOG_WARN)
            debugPutOffsetMsg(s, 40);    //@mxd:40:Timeout to Active open, resending SYN
            #endif

            //Keep sending SYN until we hear from remote node. This may be for infinite
            //time, in that case caller must detect it and do something.
            flags = SYN;
            break;

        case TCP_SYN_RCVD:
            //We must receive ACK before timeout expires. If not, resend SYN+ACK.
            //Abort, if maximum attempts counts are reached.
            if ( ps->RetryCount < TCP_MAX_RETRY_COUNTS )
            {
                #if (DEBUG_TCP >= LOG_WARN)
                debugPutOffsetMsg(s, 27);    //@mxd:27:Timeout, did not rcv ACK. Resending ACK|SYN
                #endif

                flags = SYN | ACK;
            }
            else {
                #if (DEBUG_TCP >= LOG_WARN)
                if ( ps->Flags.bServer )
                    debugPutOffsetMsg(s, 28);    //@mxd:28:state LISTEN: Max Retries, not receive ACK for SYS sent. Can not EST conn
                else
                    debugPutOffsetMsg(s, 29);    //@mxd:29:state CLOSED: Max Retries, not receive ACK for SYS sent. Can not EST conn
                #endif

                CloseSocket(ps);
            }
            break;

        case TCP_EST:
#if !defined(TCP_NO_WAIT_FOR_ACK)
            //Don't let this connection idle for very long time. If we did not receive or
            //send any message before timeout expires, close this connection.
            if ( ps->RetryCount <= TCP_MAX_RETRY_COUNTS )
            {
                //This connection is assigned a TX Buffer - resend the data contained in it!
                if ( ps->TxBuffer != INVALID_BUFFER )
                {
                    #if (DEBUG_TCP >= LOG_WARN)
                    debugPutOffsetMsg(s, 6);    //@mxd:6:Resending TCP Message. Previously sent data was NOT acknowledged!
                    #endif

                    MACSetTxBuffer(ps->TxBuffer, 0);
                    MACFlush();
                }
                else {
                    #if (DEBUG_TCP >= LOG_WARN)
                    debugPutOffsetMsg(s, 41);    //@mxd:41:Timeout, resending ACK
                    #endif

                    flags = ACK;
                }
            }
            else
            {
                // Forget about previous transmission. If this socket is assigned a TX Buffer, discard it and
                // make it available to other connections!
                if ( ps->TxBuffer != INVALID_BUFFER )
                    MACDiscardTx(ps->TxBuffer);
                    
                //By assigning the TxBuffer with INVALID_BUFFER, the first available TX Buffer will be used
                //the first time the TCPPut() or TCPPutArray() function is called.
                ps->TxBuffer = INVALID_BUFFER;

#endif
                #if (DEBUG_TCP >= LOG_WARN)
                debugPutOffsetMsg(s, 5);    //@mxd:5:state FIN-WAIT-1: Max Retries, connection closed! Sending ACK|FIN
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
            //If we do not receive any response to our close request, close it outselves.
            if ( ps->RetryCount > TCP_MAX_RETRY_COUNTS ) {
                #if (DEBUG_TCP >= LOG_WARN)
                if ( ps->Flags.bServer )
                    debugPutOffsetMsg(s, 30);    //@mxd:30:state LISTEN: Max Retries, no responce to our close request, close forcefully!
                else
                    debugPutOffsetMsg(s, 31);    //@mxd:31:state CLOSED: Max Retries, no responce to our close request, close forcefully!
                #endif

                CloseSocket(ps);
            }
            else {
                #if (DEBUG_TCP >= LOG_WARN)
                debugPutOffsetMsg(s, 42);    //@mxd:42:Timeout, resending FIN
                #endif

                flags = FIN;
            }
            break;

        case TCP_CLOSING:
        case TCP_TIMED_WAIT:
            //If we do not receive any response to out close request, close it outselves.
            if ( ps->RetryCount > TCP_MAX_RETRY_COUNTS ) {
                #if (DEBUG_TCP >= LOG_WARN)
                if ( ps->Flags.bServer )
                    debugPutOffsetMsg(s, 32);    //@mxd:32:state LISTEN: Max Retries, no responce to our close(2) request, close forcefully!
                else
                    debugPutOffsetMsg(s, 33);    //@mxd:33:state CLOSED: Max Retries, no responce to our close(2) request, close forcefully!
                #endif

                CloseSocket(ps);
            }
            else {
                #if (DEBUG_TCP >= LOG_WARN)
                debugPutOffsetMsg(s, 41);    //@mxd:41:Timeout, resending ACK
                #endif

                flags = ACK;
            }
            break;
        }

        //Check if any flags have been set, and send requested TCP packet
        //Flags can be: FIN, SYN, RST, PSH, ACK and URG
        if ( flags > 0x00 )
        {
            if ( flags != ACK )
                seq = ps->SND_SEQ++;
            else
                seq = ps->SND_SEQ;

            //Send TCP Message with given flags. Sends a TCP Message with the given data. The sent TCP Message
            //will contain only a "TCP Header" with possible "TCP Options". No "TCP Data" is transmitted.
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

    return checkSum.Val;
}
*/


/**
 * Reads a TCP segment from the MAC.
 *
 * @preCondition    TCPInit() is already called AND <br>
 *                  TCP segment is ready in MAC buffer
 *
 * @param remote    Remote node info
 * @param len       Total length of TCP semgent
 * @param destIP    The destination IP address received in the IP Header
 *
 * @return          TRUE if this function has completed its task <br>
 *                  FALSE otherwise
 */
BOOL TCPProcess(NODE_INFO *remote, IP_ADDR *destIP, WORD len)
{
    TCP_HEADER      TCPHeader;
    PSEUDO_HEADER   pseudoHeader;
    TCP_SOCKET      socket;
    WORD_VAL        checksum;
    BYTE            optionsSize;
    #if (DEBUG_TCP > LOG_OFF)
    //BYTE tmp;
    #endif


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
    pseudoHeader.DestAddress.v[0]   = destIP->v[0];
    pseudoHeader.DestAddress.v[1]   = destIP->v[1];
    pseudoHeader.DestAddress.v[2]   = destIP->v[2];
    pseudoHeader.DestAddress.v[3]   = destIP->v[3];
    pseudoHeader.Zero               = 0x0;
    pseudoHeader.Protocol           = IP_PROT_TCP;
    pseudoHeader.TCPLength          = len;  //The length of the TCP segment, including both header
                                            //and data. Note that this is not a specific field in
                                            //the TCP header; it is computed.

    //Swap bytes of TCPLength field - all other bytes are already correct
    SwapPseudoTCPHeader(pseudoHeader);

    checksum.Val = ~CalcIPChecksum((BYTE*)&pseudoHeader,
                                    sizeof(pseudoHeader));

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

    if ( checksum.Val != TCPHeader.Checksum.Val )
    {
        #if (DEBUG_TCP >= LOG_WARN)
        debugPutMsg(1);

        //Write HEX value of calculated and actual checksums
        /*
        debugPutByteHex(checksum.v[1]);
        debugPutByteHex(checksum.v[0]);

        debugPutByte(' ');
        tmp = TCPHeader.Checksum.v[1];
        debugPutByteHex(tmp);
        tmp = TCPHeader.Checksum.v[0];
        debugPutByteHex(tmp);
        debugPutByte(' ');
        tmp = (BYTE)(len >> 8);
        debugPutByteHex(tmp);
        tmp = (BYTE)len;
        debugPutByteHex(tmp);
        */
        #endif

        MACRxbufDiscard();
        return TRUE;
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
        debugPutMsg(2); //@mxd:2:Found Matching socket, assigned to TCP Handle %d
        debugPutByte(socket);
        #endif

        //This TCP socket is handled by us (addressed to one our of sockets) - handle it
        HandleTCPSeg(socket, remote, &TCPHeader, len);
    }
    else
    {
        #if (DEBUG_TCP >= LOG_WARN)
        debugPutMsg(3); //@mxd:3:Could not find matching socket for destination port 0x%x%x
        debugPutByteHex(TCPHeader.DestPort.v[1]);
        debugPutByteHex(TCPHeader.DestPort.v[0]);
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

            //Send RST message. Sends a TCP Message with the given data. The sent TCP Message will contain
            //only a "TCP Header" with possible "TCP Options". No "TCP Data" is transmitted.
            SendTCP(remote,
                    TCPHeader.DestPort.Val,
                    TCPHeader.SourcePort.Val,
                    TCPHeader.AckNumber,
                    TCPHeader.SeqNumber,
                    RST);
        }

    }

    return TRUE;
}


/**
 * A TCP segment is assembled with the given data, and transmitted. This function will send the
 * Ethernet Header (MAC Header), IP Header and IP Data (which is the TCP Header and TCP Data in this case).
 * The given buffer must contain the TCP Data that has to be sent! If there is no data (when we send a flag
 * like ACK, SYN, FIN,... for example), the buffer should be = INVALID_BUFFER.
 *
 * @preCondition    TCPInit() is already called     AND
 *                  TCPIsPutReady() == TRUE
 *
 * @param remote    Remote node info
 * @param localPort Source port number
 * @param remotePort Destination port number
 * @param seq       Segment sequence number
 * @param ack       Segment acknowledge number
 * @param flags     Segment flags
 * @param buffer    Buffer to which this segment is to be transmitted. If this value is INVALID_BUFFER, a
 *                  available transmit buffer will be assigned to it, and len parameter should be 0!
 * @param len       Total data length for this segment.
 */
static void TransmitTCP(NODE_INFO *remote,
                        WORD localPort,
                        WORD remotePort,
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

    header.SourcePort.Val       = localPort;
    header.DestPort.Val         = remotePort;
    header.SeqNumber            = tseq;
    header.AckNumber            = tack;
    header.Flags.bits.Reserved2 = 0;
    header.DataOffset.Reserved3 = 0;
    header.Flags.byte           = flags;
    // Receive window = MAC Free buffer size - TCP header (20) - IP header (20)
    //                  - ETHERNET header (14 if using NIC) .
    header.Window.Val           = MACGetFreeRxSize();
#if !defined(STACK_USE_SLIP)
    /*
     * Limit one segment at a time from remote host.
     * This limit increases overall throughput as remote host does not
     * flood us with packets and later retry with significant delay.
     */
    if ( header.Window.Val >= MAC_RX_BUFFER_SIZE )
        header.Window.Val = MAC_RX_BUFFER_SIZE;

    else if ( header.Window.Val > 54 )
    {
        header.Window.Val -= 54;
    }
    else
        header.Window.Val = 0;
#else
    if ( header.Window.Val > 40 )
    {
        header.Window.Val -= 40;
    }
    else
        header.Window.Val = 0;
#endif

    header.Checksum.Val         = 0;
    header.UrgentPointer.Val    = 0;

    SwapTCPHeader(&header);

    len += sizeof(header);

    if ( flags & SYN )
    {
        len += sizeof(options);
        options.Kind = TCP_OPTIONS_MAX_SEG_SIZE;
        options.Length = 0x04;

        // Load MSS in already swapped order.
        options.MaxSegSize.v[0]  = (BYTE)(MAC_RX_BUFFER_SIZE >> 8); // 0x05;
        options.MaxSegSize.v[1]  = (BYTE)(MAC_RX_BUFFER_SIZE & 0xff); // 0xb4;

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

    header.Checksum.Val = ~CalcIPChecksum((BYTE*)&pseudoHeader,
                        sizeof(pseudoHeader));
    checkSum.Val = header.Checksum.Val;

    //There is no TCP Data to be send with this Segment. Get next available TX Buffer. If the given
    //buffer != INVALID_BUFFER, then it contains the data that has to be sent!
    if ( buffer == INVALID_BUFFER ) {
        //Get handle to next available TX Buffer. The TCPIsPutReady() function that has to be called
        //prior to this function will determine if there is an available TX Buffer.
        buffer = MACGetTxBuffer();
    }

    //Set current TX Buffer access pointer (All future read and writes to the TX Buffer will be to the
    //set location) to first byte of "IP data" = "TCP Header" in this case
    IPSetTxBuffer(buffer, 0);

    //Write the Ethernet Header (MAC Header) and IP Header to the current TX buffer.
    //The last parameter (len) is the length of the data to follow, which is the "TCP Header" + "TCP Data"
    IPPutHeader(remote, IP_PROT_TCP, len);
    
    //Write the "TCP Header" - is part of the "IP Data"
    IPPutArray((BYTE*)&header, sizeof(header));

    //Write any "TCP options" that have to be sent - is part of the "IP Data"
    if ( flags & SYN )
        IPPutArray((BYTE*)&options, sizeof(options));

    //Set TX Buffer access pointer (All future read and writes to the TX Buffer will be to the set location)
    //to first byte of "IP data" = "TCP Header" in this case
    IPSetTxBuffer(buffer, 0);

    //Performs checksum calculation in MAC buffer itself.
    checkSum.Val = CalcIPBufferChecksum(len);

    //Set TX Buffer access pointer (All future read and writes to the TX Buffer will be to the set location)
    //to 16th byte of "IP data" = 16th byte of "TCP Header" = TCP Header's checksum field
    IPSetTxBuffer(buffer, 16);
    MACPut(checkSum.v[1]);
    MACPut(checkSum.v[0]);
    
    //Set TX Buffer access pointer (All future read and writes to the TX Buffer will be to the set location)
    //to first byte of "IP data" = "TCP Header" in this case
    MACSetTxBuffer(buffer, 0);

    //Transmits the contents of the current transmit buffer = "TCP Header" and "TCP Data"
    MACFlush();
}


/**
 * Go through list of connections, and look check if any of them match the given destination socket.
 *
 * @preCondition    TCPInit() is already called
 *
 * @param hdr       TCP Header to be matched against.
 * @param remote    Node who sent this header.
 *
 * @return          A socket that matches with given header and remote
 *                  node is searched.
 *                  If such socket is found, its index is returned
 *                  else INVALID_SOCKET is returned.
 */
static TCP_SOCKET FindMatchingSocket(TCP_HEADER *hdr, NODE_INFO *remote)
{
    SOCKET_INFO *ps;
    TCP_SOCKET s;
    TCP_SOCKET partialMatch;

    partialMatch = INVALID_SOCKET;

    #if (DEBUG_TCP >= LOG_DEBUG)
    debugPutMsg(44); //@mxd:44:FindMatchingSocket() called for message addressed to port 0x%x%x
    debugPutByteHex(hdr->DestPort.v[1]);
    debugPutByteHex(hdr->DestPort.v[0]);
    #endif

    //Go through all sockets, and search for one 
    for ( s = 0; s < MAX_SOCKETS; s++ )
    {
        ps = &TCB[s];

        if ( ps->smState != TCP_CLOSED )
        {
            #if (DEBUG_TCP >= LOG_DEBUG)
            debugPutMsg(43); //@mxd:43:FindMatchingSocket(), TCB[%d] - comparing local port 0x%x%x to destination port
            debugPutByte(s);
            debugPutByteHex((BYTE)ps->localPort >> 8);
            debugPutByteHex((BYTE)ps->localPort);
            #endif
        
            if ( ps->localPort == hdr->DestPort.Val)
            {
                if ( ps->smState == TCP_LISTEN )
                    partialMatch = s;

                if ( ps->remotePort == hdr->SourcePort.Val &&
                     ps->remote.IPAddr.Val == remote->IPAddr.Val )
                {
                        return s;
                }
            }
        }
    }

    ps = &TCB[partialMatch];

    //Addressed to a socket that is in LISTEN state
    if ( partialMatch != INVALID_SOCKET &&
         ps->smState == TCP_LISTEN )
    {
        memcpy((void*)&ps->remote, (void*)remote, sizeof(*remote));
        //ps->remote              = *remote;
        ps->localPort           = hdr->DestPort.Val;
        ps->remotePort          = hdr->SourcePort.Val;
        ps->Flags.bIsGetReady   = FALSE;
        
        //By assigning the TxBuffer with INVALID_BUFFER, the first available TX Buffer will be used
        //the first time the TCPPut() or TCPPutArray() function is called.
        ps->TxBuffer            = INVALID_BUFFER;
        ps->Flags.bIsPutReady   = TRUE;

        return partialMatch;
    }

    if ( partialMatch == INVALID_SOCKET )
        return UNKNOWN_SOCKET;
    else
        return INVALID_SOCKET;
}


/**
 *
 * @param header    TCP Header to be swapped.
 *
 * @return          Given header is swapped.
 */
static void SwapTCPHeader(TCP_HEADER* header)
{
#if defined(TCP_SPEED_OPTIMIZE)
    BYTE b0;
    b0 = header->SourcePort.v[0];
    header->SourcePort.v[0]     = header->SourcePort.v[1];
    header->SourcePort.v[1]     = b0;

    b0 = header->DestPort.v[0];
    header->DestPort.v[0]     = header->DestPort.v[1];
    header->DestPort.v[1]     = b0;

    header->SeqNumber           = swapl(header->SeqNumber);

    header->AckNumber           = swapl(header->AckNumber);

    b0 = header->Window.v[0];
    header->Window.v[0]     = header->Window.v[1];
    header->Window.v[1]     = b0;

    b0 = header->Checksum.v[0];
    header->Checksum.v[0]     = header->Checksum.v[1];
    header->Checksum.v[1]     = b0;

    b0 = header->UrgentPointer.v[0];
    header->UrgentPointer.v[0]     = header->UrgentPointer.v[1];
    header->UrgentPointer.v[1]     = b0;
#else
    header->SourcePort.Val      = swaps(header->SourcePort.Val);
    header->DestPort.Val        = swaps(header->DestPort.Val);
    header->SeqNumber           = swapl(header->SeqNumber);
    header->AckNumber           = swapl(header->AckNumber);
    header->Window.Val          = swaps(header->Window.Val);
    header->Checksum.Val        = swaps(header->Checksum.Val);
    header->UrgentPointer.Val   = swaps(header->UrgentPointer.Val);
#endif
}


/**
 * Forcefully closed socket, no message is sent via TCP
 *
 * @preCondition    TCPInit() is already called
 *
 * @param ps        Pointer to a socket info that is to be closed.
 *
 * @return          Given socket information is reset and any
 *                  buffer held by this socket is discarded.
 */
static void CloseSocket(SOCKET_INFO* ps)
{
    //This socket has a TX Buffer assigned to it. The contents of this TX Buffer is now discarded, and it is
    //made available to other connections.
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

    if ( ps->Flags.bServer ) {
        ps->smState = TCP_LISTEN;
    }
    else {
        ps->smState = TCP_CLOSED;
    }
    return;
}


/**
 *
 * @preCondition    TCPInit() is already called     AND
 *                  TCPProcess() is the caller.
 *
 * @param s         Socket that owns this segment
 * @param remote    Remote node info
 * @param hdr       TCP Header
 * @param len       Length of TCP data. This does NOT include the header length!
 *
 * @return          TCP FSM is executed on given socket with
 *                  given TCP segment.
 */
static void HandleTCPSeg(TCP_SOCKET s,
                        NODE_INFO *remote,
                        TCP_HEADER *hdr,
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

    ack = hdr->SeqNumber ;
    ack += (DWORD)len;
    seq = ps->SND_SEQ;

    /*
     * Clear retry counts and timeout tick counter.
     */
    ps->RetryCount  = 0;
    ps->startTick   = TickGet16bit();    //Set timeout reference
    ps->TimeOut = TCP_START_TIMEOUT_VAL;

    //Current state = LISTEN
    if ( ps->smState == TCP_LISTEN )
    {
        MACRxbufDiscard();

        ps->SND_SEQ     = ++ISS;
        ps->SND_ACK     = ++ack;
        seq             = ps->SND_SEQ;
        ++ps->SND_SEQ;
        if ( hdr->Flags.bits.flagSYN )
        {
            //This socket has received connection request. Remember calling node,
            //assign next segment seq. number for this potential connection.
            memcpy((void*)&ps->remote, (const void*)remote, sizeof(*remote));
            ps->remotePort  = hdr->SourcePort.Val;

            //Grant connection request.
            flags           = SYN | ACK;
            ps->smState     = TCP_SYN_RCVD;

            #if (DEBUG_TCP >= LOG_INFO)
            debugPutOffsetMsg(s, 21);    //@mxd:21:state SYN_RCVD: Received SYN while in LISTEN, sending ACK|SYN
            #endif
        }
        else
        {
            //Anything other than connection request is ignored in LISTEN state.
            flags               = RST;
            seq                 = ack;
            ack                 = hdr->SeqNumber;
            ps->remote.IPAddr.Val = 0x00;

            #if (DEBUG_TCP >= LOG_DEBUG)
            debugPutOffsetMsg(s, 22);    //@mxd:22:state SYN_RCVD: Received SYN while in LISTEN, sending ACK|SYN
            #endif
        }

    }
    //Current state = NOT LISTEN
    else
    {
        //Reset FSM, if RST is received.
        if ( hdr->Flags.bits.flagRST )
        {
            MACRxbufDiscard();

            #if (DEBUG_TCP >= LOG_WARN)
            if ( ps->Flags.bServer )
                debugPutOffsetMsg(s, 34);    //@mxd:34:state LISTEN: Received RST, close forcefully!
            else
                debugPutOffsetMsg(s, 35);    //@mxd:35:state CLOSED: Received RST, close forcefully!
            #endif

            CloseSocket(ps);

            return;

        }
        else if ( seq == hdr->AckNumber )
        {
            //Current state = SYN_RCVD
            //In this state we are waiting to receive an ACK before we can go into ESTABLISHED state
            if ( ps->smState == TCP_SYN_RCVD )
            {
                //If ACK received, we have established a connection! Go to ESTABLISHED state
                if ( hdr->Flags.bits.flagACK )
                {
                    ps->SND_ACK = ack;
                    ps->RetryCount = 0;
                    ps->startTick = TickGet16bit();  //Set timeout reference

                    ps->smState = TCP_EST;

                    if ( len > 0 )
                    {
                        #if (DEBUG_TCP >= LOG_INFO)
                        debugPutOffsetMsg(s, 12);    //@mxd:12:state EST: Connection Established, ACK was received WITH DATA
                        #endif

                        ps->Flags.bIsGetReady   = TRUE;
                        ps->RxCount             = len;  //Set RxCount = Length of TCP Data (NOT including header, only data)
                        ps->Flags.bFirstRead    = TRUE;
                    }
                    else {
                        #if (DEBUG_TCP >= LOG_INFO)
                        debugPutOffsetMsg(s, 13);    //@mxd:13:state EST: Connection Established, ACK was received (without data)
                        #endif

                        MACRxbufDiscard();
                    }
                }
                //Only waiting for ACK, discard anything else
                else
                {
                    MACRxbufDiscard();
                }
            }
            //Current state = SYN_SENT
            //We have sent a SYN, and are waiting for SYN and ACK
            // - If we receive SYS and ACK, ESTABLISH a connection
            // - If we receive SYN, go to SYN_RCVD state. We are now waiting for an ACK before going to ESTABLISHED state
            else if ( ps->smState == TCP_SYN_SENT )
            {
                if ( hdr->Flags.bits.flagSYN )
                {
                    ps->SND_ACK = ++ack;
                    //We received SYS and ACK, ESTABLISH a connection
                    if ( hdr->Flags.bits.flagACK )
                    {
                        #if (DEBUG_TCP >= LOG_INFO)
                        if (len > 0)
                            debugPutOffsetMsg(s, 14);    //@mxd:14:state EST: Rcvd ACK|SYN with data, sending ACK
                        else
                            debugPutOffsetMsg(s, 15);    //@mxd:15:state EST: Rcvd ACK|SYN (without data), sending ACK
                        #endif

                        flags = ACK;
                        ps->smState = TCP_EST;
                    }
                    //We receive SYN, go to SYN_RCVD state. We are now waiting for an ACK before going to ESTABLISHED state
                    else
                    {
                        #if (DEBUG_TCP >= LOG_INFO)
                        debugPutOffsetMsg(s, 16);    //@mxd:16:state SYN_RCVD: Rcvd ACK, waiting for SYN. Sending ACK|SYN
                        #endif

                        flags = SYN | ACK;
                        ps->smState = TCP_SYN_RCVD;
                        ps->SND_SEQ = ++seq;
                    }

                    //Can data be received when state is set to SYN_RCVD?? If not, place this code in if {} block above!
                    //If we received data with it, set condition so TCP data can be read
                    if ( len > 0 )
                    {
                        ps->Flags.bIsGetReady   = TRUE;
                        ps->RxCount             = len;  //Set RxCount = Length of TCP Data (NOT including header, only data)
                        ps->Flags.bFirstRead    = TRUE;
                    }
                    else {
                        MACRxbufDiscard();
                    }
                }
                //SYN flag not set, discard
                else
                {
                    #if (DEBUG_TCP >= LOG_WARN)
                    if ( hdr->Flags.bits.flagACK )
                        debugPutOffsetMsg(s, 17);    //@mxd:17:Discarding ACK, waiting for SYN
                    else
                        debugPutOffsetMsg(s, 18);    //@mxd:18:Discarding message, waiting for SYN
                    #endif

                    MACRxbufDiscard();
                }
            }
            //Current state is NOT LISTEN, SYN_RCVD or SYN_SENT
            else
            {
                if ( hdr->SeqNumber != ps->SND_ACK )
                {
                    // Discard buffer.
                    MACRxbufDiscard();
                    return;
                }

                ps->SND_ACK = ack;

                //Current state = ESTABLISHED
                if ( ps->smState == TCP_EST )
                {
                    //Received ACK flag while in ESTABLISHED state. This is acknowleging data previously TXed, I think??
                    if ( hdr->Flags.bits.flagACK )
                    {
                        //Data ACK, can now be discarded. Does not have to be retransmitted
                        if ( ps->TxBuffer != INVALID_BUFFER )
                        {
                            MACDiscardTx(ps->TxBuffer);
                            ps->TxBuffer            = INVALID_BUFFER;
                            ps->Flags.bIsPutReady   = TRUE;
                        }
                    }

                    //Received FIN flag
                    if ( hdr->Flags.bits.flagFIN )
                    {
                        flags = FIN | ACK;
                        seq = ps->SND_SEQ++;
                        ack = ++ps->SND_ACK;
                        ps->smState = TCP_LAST_ACK;
                        
                        #if (DEBUG_TCP >= LOG_INFO)
                        debugPutOffsetMsg(s, 19);    //@mxd:19:state LAST_ACK: Received FIN, sending FIN|ACK
                        #endif
                    }

                    //Received data while in ESTABLISHED state - make this data available for reading!
                    if ( len > 0 )
                    {
                        if ( !ps->Flags.bIsGetReady )
                        {
                            ps->Flags.bIsGetReady   = TRUE;
                            ps->RxCount             = len;  //Set RxCount = Length of TCP Data (NOT including header, only data)
                            ps->Flags.bFirstRead    = TRUE;

                             // 4/1/02
                            flags = ACK;

                            #if (DEBUG_TCP >= LOG_DEBUG)
                            debugPutOffsetMsg(s, 23);    //@mxd:23:Received data, sending ACK
                            #endif
                        }
                        else
                        {
                            //Since we cannot accept this packet, restore to previous seq and ack. And do not
                            //send anything back. Host has to resend this packet when we are ready.
                            flags = 0x00;
                            ps->SND_SEQ = prevSeq;
                            ps->SND_ACK = prevAck;

                            MACRxbufDiscard();

                            #if (DEBUG_TCP >= LOG_ERROR)
                            debugPutOffsetMsg(s, 24);    //@mxd:24:RX Buffer overflow! Received data, but had to discard it
                            #endif
                        }
                    }
                    //No data received, only flags. Can discard received data, don't need it anymore
                    else
                    {
                        MACRxbufDiscard();
                    }
                }
                //Current state is LAST_ACK
                //In this state, we are waiting for a ACK for FIN we previously sent before closing connection
                else if ( ps->smState == TCP_LAST_ACK )
                {
                    MACRxbufDiscard();

                    //Close socket
                    if ( hdr->Flags.bits.flagACK )
                    {
                        #if (DEBUG_TCP >= LOG_INFO)
                        if ( ps->Flags.bServer )
                            debugPutOffsetMsg(s, 36);    //@mxd:36:state LISTEN: Received ACK for FIN sent
                        else
                            debugPutOffsetMsg(s, 37);    //@mxd:37:state CLOSED: Received ACK for FIN sent
                        #endif

                        CloseSocket(ps);
                    }
                }
                //Current state is FIN_WAIT_1
                else if ( ps->smState == TCP_FIN_WAIT_1 )
                {
                    MACRxbufDiscard();

                    if ( hdr->Flags.bits.flagFIN )
                    {
                        flags = ACK;
                        ack = ++ps->SND_ACK;
                        if ( hdr->Flags.bits.flagACK )
                        {
                            #if (DEBUG_TCP >= LOG_INFO)
                            if ( ps->Flags.bServer )
                                debugPutOffsetMsg(s, 38);    //@mxd:38:state LISTEN: Received ACK|FIN for FIN sent
                            else
                                debugPutOffsetMsg(s, 39);    //@mxd:39:state CLOSED: Received ACK|FIN for FIN sent
                            #endif

                            CloseSocket(ps);
                        }
                        else
                        {
                            #if (DEBUG_TCP >= LOG_INFO)
                            debugPutOffsetMsg(s, 20);    //@mxd:20:state CLOSING: Received FIN, waiting for ACK. Sending ACK
                            #endif
                            ps->smState = TCP_CLOSING;
                        }
                    }
                }
                //Current state is CLOSING
                else if ( ps->smState == TCP_CLOSING )
                {
                    MACRxbufDiscard();

                    if ( hdr->Flags.bits.flagACK )
                    {
                        #if (DEBUG_TCP >= LOG_INFO)
                        if ( ps->Flags.bServer )
                            debugPutOffsetMsg(s, 36);    //@mxd:36:state LISTEN: Received ACK for FIN sent
                        else
                            debugPutOffsetMsg(s, 37);    //@mxd:37:state CLOSED: Received ACK for FIN sent
                        #endif

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
        //Send TCP Message with given flags. Sends a TCP Message with the given data. The sent TCP Message
        //will contain only a "TCP Header" with possible "TCP Options". No "TCP Data" is transmitted.
        SendTCP(remote,
                hdr->DestPort.Val,
                hdr->SourcePort.Val,
                seq,
                ack,
                flags);
    }
}

/**
 * Returns a pointer to the SOCKET_INFO structure for the given socket.
 *
 * @preCondition    TCPInit() is already called.
 *
 * @param s         socket
 *
 * @return          Returns a pointer to the SOCKET_INFO structure for the given socket.
 */
SOCKET_INFO* TCPGetSocketInfo(TCP_SOCKET s)
{
    return &TCB[s];
}
