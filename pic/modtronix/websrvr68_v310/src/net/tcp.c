/**
 * @brief           TCP Module for Modtronix TCP/IP Stack - see RFC 793 for details
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
 *    - Modified TCPProcess() to include localIp as third param.
 *      This was done to allow this function to calculate checksum correctly.
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 * 2005-11-23, David Hosken (DH):
 *    - Added "ps->startTick = TickGet();" line to end of TCPConnect() function.
 * 2006-05-03, David Hosken (DH):
 *    - Added logic for new bACKValid flag. Got this fix from new Microchip V3.02 stack!
 *    - Fixed bug with ACK and SEQ numbers
 * 2006-09-06, David Hosken (DH):
 *    - Replaced lots of code with code from new Microchip TCP/IP V3.75
 * 2006-09-08, David Hosken (DH): Implemented changes from Microchip TCP/IP stack 3.75
 *********************************************************************/
 
#define THIS_IS_TCP

#include <string.h>

#include "projdefs.h"
#include "net\checkcfg.h"
#include "net\tcp.h"
#include "net\helpers.h"
#include "net\ip.h"
#include "net\mac.h"
#include "net\tick.h"
#include "net\delay.h"
#include "debug.h"

#if defined(STACK_USE_TCP)

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0x8F, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0x8F, msgCode, msgStr)


//The following functions are only required for debugging
#if !defined(DEBUG_OFF)
    #if (MAX_SOCKETS <= 14)
        #define debugPutOffsetMsg(offset, msgCode) debugPut2Bytes(0x80 + offset, msgCode)
    #else
        /* If there are more then 14 Sockets, then */
       #define debugPutOffsetMsg(offset, msgCode) \
            if (offset <= 14) {debugPut2Bytes(0x80 + offset, msgCode);} \
            else {debugPut2Bytes(0x80, offset);debugPutByte(msgCode); }
    #endif
#endif

// For backwards compatibility with old projects! Undefine any old TCP_START_TIMEOUT_VAL define,
// and replace with new define TCP_START_TIMEOUT_VAL_1
#undef TCP_START_TIMEOUT_VAL
#define TCP_START_TIMEOUT_VAL TCP_START_TIMEOUT_VAL_1

// Max TCP data length is MAC_TX_BUFFER_SIZE - sizeof(TCP_HEADER) -
// sizeof(IP_HEADER) - sizeof(ETHER_HEADER)
#define MAX_TCP_DATA_LEN        (MAC_TX_BUFFER_SIZE - 54)


/*
 * TCP Flags as defined by rfc793
 */
#define FIN     (0x01ul)
#define SYN     (0x02ul)
#define RST     (0x04ul)
#define PSH     (0x08ul)
#define ACK     (0x10ul)
#define URG     (0x20ul)


/*
 * TCP Header def. as per rfc 793.
 */
typedef struct _TCP_HEADER
{
    WORD_VAL    SourcePort;     //Offset 0
    WORD_VAL    DestPort;       //Offset 2
    DWORD       SeqNumber;      //Offset 4
    DWORD       AckNumber;      //Offset 8

    /* bits 0-3 = Data Offset = Number of 32 bit words of data in TCP header. In other words, this value
    times four equals the number of bytes in the header. It indicates by how many 32-bit words the start
    of  the data is offset from the beginning of the TCP segment. */
    struct                      //Offset 12
    {
        unsigned char Reserved3      : 4;
        unsigned char Val            : 4;
    } DataOffset;


    union
    {
        struct
        {
            unsigned char flagFIN    : 1;
            unsigned char flagSYN    : 1;
            unsigned char flagRST    : 1;
            unsigned char flagPSH    : 1;
            unsigned char flagACK    : 1;
            unsigned char flagURG    : 1;
            unsigned char Reserved2  : 2;
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
#define TCP_OPTIONS_END_OF_LIST     (0x00ul)
#define TCP_OPTIONS_NO_OP           (0x01ul)
#define TCP_OPTIONS_MAX_SEG_SIZE    (0x02ul)
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
 * Local temp port numbers.
 */
#ifdef STACK_CLIENT_MODE
static WORD _NextPort = TCP_LOCAL_PORT_START_NUMBER;
#endif

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

//This variable can only be used as long as the TCP/IP stack reads a whole TCP message at a time.
//If this stack would be modified so that multiple TCP messages can be read simultaniously, this
//variable will could not be used anymore!
static WORD _tcpDataLength;

static  void    HandleTCPSeg(TCP_SOCKET s,
                             NODE_INFO *remote,
                             TCP_HEADER *hdr,
                             WORD len);

static void TransmitTCP(NODE_INFO *remote,
                        TCP_PORT localPort,
                        TCP_PORT remotePort,
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
    for(s = 0; s < MAX_SOCKETS; s++)
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
        ps->TxCount                = 0;
    }
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
TCP_SOCKET TCPListen(TCP_PORT port)
{
    TCP_SOCKET s;
    SOCKET_INFO* ps;

    //Search for a CLOSED socket, and if found, initialize it to LISTEN on given port
    for (s = 0; s < MAX_SOCKETS; s++)
    {
        ps = &TCB[s];

        if ( ps->smState == TCP_CLOSED )
        {
            // We have a CLOSED socket.
            // Initialize it with LISTENing state info.
            ps->smState             = TCP_LISTEN;
            ps->localPort           = port;
            ps->remotePort          = 0;

            // There is no remote node IP address info yet.
            ps->remote.IPAddr.Val   = 0x00;

            // If a socket is listened on, it is a SERVER.
            ps->Flags.bServer       = TRUE;

            ps->Flags.bIsGetReady   = FALSE;
            if(ps->TxBuffer != INVALID_BUFFER)
            {
                MACDiscardTx(ps->TxBuffer);
                // By assigning the TxBuffer with INVALID_BUFFER, the first available TX Buffer will be used
                // the first time the TCPPut() or TCPPutArray() function is called.
                ps->TxBuffer        = INVALID_BUFFER;
            }
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
 * After calling this function, the TCP stack will try and connect with
 * the given target. The user should call the TCPIsConnected() function to
 * see if the connection has been made. The TCP stack will continuously try and
 * connect. It is up to the user to terminate the connection attempts if no
 * connection is made after a certian time. The TCPDisconnect() function can
 * be used to stop the TCP stack from trying to connect.
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
TCP_SOCKET TCPConnect(NODE_INFO *remote, TCP_PORT remotePort)
{
    TCP_SOCKET s;
    SOCKET_INFO* ps;
    BOOL lbFound;


    lbFound = FALSE;

    // Find an available socket
    for(s = 0; s < MAX_SOCKETS; s++)
    {
        ps = &TCB[s];
        if(ps->smState == TCP_CLOSED)
        {
            lbFound = TRUE;
            break;
        }
    }

    // If there is no socket available, return error.
    if(!lbFound) {
        #if (DEBUG_TCP >= LOG_ERROR)
        debugPutOffsetMsg(s, 7);    //@mxd:7:Could not connect, no free sockets available
        #endif
        return INVALID_SOCKET;
    }

    // Each new socket that is opened by this node, gets
    // next sequential port number.
    ps->localPort = ++_NextPort;
    if(_NextPort > TCP_LOCAL_PORT_END_NUMBER )
        _NextPort = TCP_LOCAL_PORT_START_NUMBER;

    // This is a client socket.
    ps->Flags.bServer = FALSE;

    // This is the port, we are trying to connect to.
    ps->remotePort = remotePort;

    // Each new socket that is opened by this node, will
    // start with next the next seqeuence number (essentially random)
    ps->SND_SEQ++;
    ps->SND_ACK = 0;        //If ACK flag is not sent, this number has no meaning

    memcpy((BYTE*)&ps->remote, (const void*)remote, sizeof(ps->remote));

    //Send SYN message. Sends a TCP Message with the given data. The sent TCP Message will contain
    //only a "TCP Header" with possible "TCP Options". No "TCP Data" is transmitted.
    //We will now be waiting for an acknowledge from the server with a ack number equel to the
    //sequence number we just sent + 1.
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
    
    //Increment to SEQ number of next segment we will send, is also the ACK number we are expecting to receive
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
    return (TCB[s].smState == TCP_ESTABLISHED);
}


/**
 * A disconnect request is sent for given socket.
 *
 * @preCondition    TCPInit() is already called AND<br>
 *                  TCPIsPutReady(s) != 0
 *
 * @param s         Socket to be disconnected.
 */
void TCPDisconnect(TCP_SOCKET s)
{
    SOCKET_INFO *ps;

    ps = &TCB[s];

    // If socket is not connected, may be it is already closed
    // or in the process of closing.  Since we have called this
    // explicitly, close it forcefully.
    if(ps->smState != TCP_ESTABLISHED && ps->smState != TCP_SYN_RECEIVED)
    {
        #if (DEBUG_TCP >= LOG_WARN)
        if ( ps->Flags.bServer ) {
            debugPutOffsetMsg(s, 25);    //@mxd:10:state LISTEN: TCPDisconnect clossed connection, no message was sent!"
        }
        else {
            debugPutOffsetMsg(s, 26);    //@mxd:11:state CLOSED: TCPDisconnect clossed connection, no message was sent!"
        }
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
    TransmitTCP(&ps->remote,    //Remote node info
        ps->localPort,  //Source port number
        ps->remotePort, //Destination port number
        ps->SND_SEQ,    //Segment sequence number
        ps->SND_ACK,    //Segment acknowledgee number
        ACK + PSH,        // Use PSH to make sure the end application receives the data right away
        ps->TxBuffer,
        ps->TxCount);
    
    //Increment to SEQ number of next segment we will send, is also the ACK number we are expecting to receive
    ps->SND_SEQ += (DWORD)ps->TxCount;
    ps->Flags.bIsPutReady       = FALSE;
    
    //Is set as soon as the TCPPut() or TCPPutArray() functions is called for this socket, cleared when flushed
    ps->Flags.bIsTxInProgress   = FALSE;


    //If we do NOT wait for ACK, we can immediately discard the TxBuffer's data. If not, we have to keep it
    //in case we have to resend the data!
    #ifdef TCP_NO_WAIT_FOR_ACK
    if(ps->TxBuffer != INVALID_BUFFER)
    {
        MACDiscardTx(ps->TxBuffer); //Discard buffer, and mark it as available

        //By assigning the TxBuffer with INVALID_BUFFER, the first available TX Buffer will be used
        //the first time the TCPPut() or TCPPutArray() function is called.
        ps->TxBuffer        = INVALID_BUFFER;
    }
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
    if (s >= MAX_SOCKETS)
        return FALSE;
        
    if(TCB[s].RemoteWindow == 0)
        return FALSE;

    //If this socket's buffer = INVAlID_BUFFER, it means it will be assigned a free buffer when the
    //TCPPut() or TCPPutArray() function is called. Call the IPIsTxReady() function to see if there
    //are any free TX Buffers. This is the case when there is currently no unsent data in this socket's
    //TX Buffer. This socket will be assigned a free buffer during the TCPPut() and TCPPutArray() functions.
    if ( TCB[s].TxBuffer == INVALID_BUFFER )
        return IPIsTxReady(FALSE);
    //This flag is valid when the socket's TX Buffer contains unsent data.
    else
        return TCB[s].Flags.bIsPutReady;
}


/**
 * Write the given byte to the given socket's transmit buffer.
 * The data is NOT sent yet, and the TCPFlush() function must be called
 * to send all data contained in the transmit buffer.
 *
 * @preCondition    TCPIsPutReady() != 0
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

    // Make sure that the remote node is able to accept our data
    if(ps->RemoteWindow == 0)
        return FALSE;

    //This TCP Socket does not contain any unsent data, and currently does not own a TX Buffer!
    //Get next free TX Buffer and assign it to this socket.
    if(ps->TxBuffer == INVALID_BUFFER)
    {
        //Get handle to next available TX Buffer. The TCPIsPutReady() function that has to be called
        //prior to this function will determine if there is an available TX Buffer.
        ps->TxBuffer = MACGetTxBuffer(FALSE);

        // Check to make sure that we received a TX Buffer
        if(ps->TxBuffer == INVALID_BUFFER)
            return FALSE;

        ps->TxCount = 0;
        
        // TODO: Take possible TCP options into consideration when positioning 

        //Set TX Buffer access pointer (All future read and writes to the TX Buffer will be to the set location)
        //to given offset in "IP data", which is the first byte after the "TCP Header" = first byte of
        //the "TCP Data". Thus, future reads and writes to TX Buffer will access "TCP Data"
        IPSetTxBuffer(ps->TxBuffer, sizeof(TCP_HEADER));
    }

    //Is set as soon as the TCPPut() or TCPPutArray() function is called for this socket, cleared when flushed
    ps->Flags.bIsTxInProgress = TRUE;

    MACPut(byte);
    ps->RemoteWindow--;

    //Increment TxCount. If buffer is full, flush it
    if(++ps->TxCount >= TCPGetMaxDataLength() )
        TCPFlush(s);

    return TRUE;
}


/**
 * !!!!!!!!!! Important !!!!!!!!!!!!!
 * This function has not been tested!!
 *
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
 * @preCondition    TCPIsPutReady() != 0
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

    // Make sure that the remote node is able to accept our data
    if(ps->RemoteWindow == 0)
        return FALSE;

    //This TCP Socket does not contain any unsent data, and currently does not own a TX Buffer!
    //Get next free TX Buffer and assign it to this socket.
    if ( ps->TxBuffer == INVALID_BUFFER )
    {
        //Get handle to next available TX Buffer. The TCPIsPutReady() function that has to be called
        //prior to this function will determine if there is an available TX Buffer.
        ps->TxBuffer = MACGetTxBuffer(FALSE);

        // Check to make sure that we received a TX Buffer
        if(ps->TxBuffer == INVALID_BUFFER)
            return FALSE;

        ps->TxCount = 0;

        //Set TX Buffer access pointer (All future read and writes to the TX Buffer will be to the set location)
        //to given offset in "IP data", which is the first byte after the "TCP Header" = first byte of
        //the "TCP Data". Thus, future reads and writes to TX Buffer will access "TCP Data"
        IPSetTxBuffer(ps->TxBuffer, sizeof(TCP_HEADER));
    }

    //This function request more bytes to be written to the TX Buffer then there is space 
    if ((ps->TxCount + count) > TCPGetMaxDataLength()) {
        //Update count with maximum number of bytes that there is place for in the TX Buffer
        count = TCPGetMaxDataLength() - ps->TxCount;
    }
    
    //Check if remote window has enough space, and update count if required
    // TODO!!!!!!

    //Is set as soon as the TCPPut() or TCPPutArray() function is called for this socket, cleared when flushed
    ps->Flags.bIsTxInProgress = TRUE;

    //Write buffer
    MACPutArray(buffer, count);

    //Update remote window
      ps->RemoteWindow -= count;

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
    if(!ps->Flags.bIsGetReady)
        return FALSE;

    //Remove the currently read packet (packet containing this socket) from the MAC RX Buffer
    MACDiscardRx(); //Discard the contents of the current RX buffer
    ps->Flags.bIsGetReady = FALSE;

    return TRUE;
}


/**
 * !!!!!!!!!! Important !!!!!!!!!!!!!
 * This function has not been tested!!
 *
 * Read the requested number of bytes from the given socket into the given buffer.
 *
 * @preCondition    TCPInit() is already called AND <br>
 *                  TCPIsGetReady(s) != 0
 *
 * @param s         socket
 * @param[in] buffer Buffer to hold received data.
 * @param count     Buffer length
 *
 * @return          Number of bytes loaded into buffer.
 */
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
            //IPSetRxBuffer(sizeof(TCP_HEADER));
            TCPSetRxBuffer(s, 0);  //Modtronix fix for Microchip stack bug - does not take TCP options into account!

            ps->Flags.bFirstRead = FALSE;
        }
        
        //If more bytes requested then are left in the receive buffer, adjust count
        if (count > ps->RxCount) {
            count = ps->RxCount;
        }

        //Has all TCP Data been read?
        if ( ps->RxCount == 0 )
        {
            MACDiscardRx(); //Discard the contents of the current RX buffer
            ps->Flags.bIsGetReady = FALSE;
            return 0;
        }

        //Read the requested amount of data from the current MAC RX Buffer
        return MACGetArray(buffer, count);
    }
    else
        return 0;
}


/**
 * Read bytes from the given TCP socket, and copies them to the given buffer until:
 * - The given buffer is full
 * - The given Chr terminating character in found
 * - End of socket is reached
 *
 * For example, when reading ASCII HTTP data from a TCP socket, we can pass
 * a NULL terminating character to read a line of data.
 *
 * @preCondition    TCPInit() is already called AND <br>
 *                  TCPIsGetReady(s) != 0
 *
 * @param s             socket
 * @param[in] buffer    Buffer to hold received data. If NULL, nothing will be written to buffer.
 * @param count         Buffer length, maximum is 255
 * @param chr           Character to look for, that will terminate read
 *
 * @return          The LSB (Bits 0-7) gives number of bytes loaded into buffer, including the
 *                  terminating character if found!
 *                  The MSB (bit 8-15) is a return code.
 *                  The return code in the MSB has the following meaning:
 *                  - TCP_GETARR_ALL: All requested bytes were read, buffer was filled. The LSB will
 *                    contain the number of bytes read = original length we passed this function.
 *                  - TCP_GETARR_END: The end of the socket was reached
 *                  - TCP_GETARR_NOTREADY: The TCP socket was not ready, no bytes were read!
 *                  - TCP_GETARR_TRM: The given terminating character was found. The LSB will
 *                    contain the number of byres read, including the terminating character.
 */
WORD TCPGetArrayChr(TCP_SOCKET s, BYTE *buffer, BYTE count, BYTE chr)
{
    SOCKET_INFO* ps;
    BYTE retCode;
    WORD_VAL ret;

    ps = &TCB[s];
    retCode = TCP_GETARR_ALL;   //Assumre all requested bytes are read
    ret.Val = 0;

    //Write string out (on ERROR level) to General debug window if found
    //debugPutGenRomStr(2, (ROM char*)"TGAC");     //@mxd:2:%s
    
    //The bIsGetReady flag indicates if the MAC RX Buffer pointer is currently set to the
    //packet containing this socket?
    if ( ps->Flags.bIsGetReady )
    {
        if ( ps->Flags.bFirstRead )
        {
            // Position read pointer to first byte of TCP data. Skips the TCP header.
            //IPSetRxBuffer(sizeof(TCP_HEADER));
            TCPSetRxBuffer(s, 0);  //Modtronix fix for Microchip stack bug - does not take TCP options into account!

            ps->Flags.bFirstRead = FALSE;
        }
        
        //If more bytes requested then are left in the receive buffer, adjust length
        if (count > ps->RxCount) {
            count = ps->RxCount;
            retCode = TCP_GETARR_END;   //Indicate that end of TCP socket was reached
        }

        //Has all TCP Data been read?
        if ( ps->RxCount == 0 )
        {
            MACDiscardRx(); //Discard the contents of the current RX buffer
            ps->Flags.bIsGetReady = FALSE;
            return (TCP_GETARR_END<<8);   //Indicate that end of TCP socket was reached, and 0 data was read
        }
        
        //Read the requested amount of data from the current MAC RX Buffer
        ret.Val = MACGetArrayChr(buffer, count, chr);
        
        //The terminating character was found
        if (ret.v[1] == MAC_GETARR_TRM) {
            retCode = TCP_GETARR_TRM;
        }
    }
    else {
        retCode = TCP_GETARR_NOTREADY;
    }

    //Add return code to MSB
    ret.v[1] = retCode;
    
    //Return number of characters read, and return code
    return ret.Val;
}


/**
 * This function reads the next byte from the current TCP packet.
 * Reads a single byte from the given socket into the given byte pointer
 *
 * @preCondition    TCPInit() is already called     AND
 *                  TCPIsGetReady(s) != 0
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
            // Position read pointer to first byte of TCP data.
            // Skips the TCP header.
            //IPSetRxBuffer(sizeof(TCP_HEADER));
            TCPSetRxBuffer(s, 0);  //Modtronix fix for Microchip stack bug - does not take TCP options into account!

            ps->Flags.bFirstRead = FALSE;
        }

        //Has all TCP Data been read?
        if ( ps->RxCount == 0 )
        {
            MACDiscardRx(); //Discard the contents of the current RX buffer
            ps->Flags.bIsGetReady = FALSE;
            return FALSE;
        }

        ps->RxCount--;
        *byte = MACGet();   //Read a single byte from the current MAC Receive buffer
        return TRUE;
    }
    return FALSE;
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
    if (s >= MAX_SOCKETS)
        return FALSE;
        
    /*
    * A socket is said to be "Get" ready when it has already
    * received some data.  Sometime, a socket may be closed,
    * but it still may contain data.  Thus in order to ensure
    * reuse of a socket, caller must make sure that it reads
    * a socket, if is ready.
    */
    return(TCB[s].Flags.bIsGetReady);
}


/**
 * Check each socket for any timeout conditions
 * 
 * @preCondition    TCPInit() is already called.
 */
void TCPTick(void)
{
    TCP_SOCKET s;
    TICK diffTicks;
    TICK tick;
    SOCKET_INFO* ps;
    DWORD seq;
    BYTE flags;

    flags = 0x00;
    // Periodically all "not closed" sockets must perform timed operation.
    for(s = 0; s < MAX_SOCKETS; s++)
    {
        ps = &TCB[s];

        // - bIsTxInProgress is set as soon as the TCPPut() or TCPPutArray() functions is called for a
        //   socket, cleared when flushed
        if ( ps->Flags.bIsGetReady || ps->Flags.bIsTxInProgress )
            continue;


        // Closed or Passively Listening socket do not care
        // about timeout conditions.
        if ( (ps->smState == TCP_CLOSED) ||
            (ps->smState == TCP_LISTEN &&
            ps->Flags.bServer == TRUE) )
            continue;

        // If timeout has NOT occured, do nothing.
        if (TickGetDiff16bit(ps->startTick) <= ps->TimeOut )
            continue;

        // Most states require retransmission, so check for transmitter
        // availability right here - common for all.
        if(!IPIsTxReady(TRUE))
            return;

        // Restart timeout reference.
        ps->startTick = TickGet16bit();

        // Update timeout value if there is need to wait longer.
        ps->TimeOut <<= 1;  //Times two

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

            // Keep sending SYN until we hear from remote node.
            // This may be for infinite time, in that case
            // caller must detect it and do something.
            // Bug Fix: 11/1/02
            flags = SYN;
            break;

        case TCP_SYN_RECEIVED:
            // We must receive ACK before timeout expires.
            // If not, resend SYN+ACK.
            // Abort, if maximum attempts counts are reached.
            if(ps->RetryCount <= TCP_MAX_RETRY_COUNTS)
            {
                #if (DEBUG_TCP >= LOG_WARN)
                debugPutOffsetMsg(s, 27);    //@mxd:27:Timeout, did not rcv ACK. Resending ACK|SYN
                #endif

                flags = SYN | ACK;
            }
            else
            {
                if(ps->Flags.bServer)
                {
                    ps->smState = TCP_LISTEN;

                    #if (DEBUG_TCP >= LOG_WARN)
                    debugPutOffsetMsg(s, 28);    //@mxd:28:state LISTEN: Max Retries, not receive ACK for SYS sent. Can not EST conn
                    #endif
                }
                else
                {
                    flags = SYN;
                    ps->smState = TCP_SYN_SENT;

                    #if (DEBUG_TCP >= LOG_WARN)
                    debugPutOffsetMsg(s, 29);    //@mxd:29:state CLOSED: Max Retries, not receive ACK for SYS sent. Can not EST conn
                    #endif
                }


                CloseSocket(ps);
            }
            break;

        case TCP_ESTABLISHED:
    #if !defined(TCP_NO_WAIT_FOR_ACK)   //If Wait for ACK is enabled, include this code
            // Don't let this connection idle for very long time.
            // If we did not receive or send any message before timeout
            // expires, close this connection.
            if(ps->RetryCount <= TCP_MAX_RETRY_COUNTS)
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
                if(ps->TxBuffer != INVALID_BUFFER)
                {
                    MACDiscardTx(ps->TxBuffer);
                    //By assigning the TxBuffer with INVALID_BUFFER, the first available TX Buffer will be used
                    //the first time the TCPPut() or TCPPutArray() function is called.
                    ps->TxBuffer = INVALID_BUFFER;
                }
    #endif
                #if (DEBUG_TCP >= LOG_WARN)
                debugPutOffsetMsg(s, 5);    //@mxd:5:state FIN-WAIT-1: Max Retries, connection closed! Sending ACK|FIN
                #endif

                // Request closure.
                flags = FIN | ACK;


                ps->smState = TCP_FIN_WAIT_1;
    #if !defined(TCP_NO_WAIT_FOR_ACK)   //If Wait for ACK is enabled, include this code
            }
    #endif
            break;

        case TCP_FIN_WAIT_1:
            if(ps->RetryCount <= TCP_MAX_RETRY_COUNTS)
            {
                // Send another FIN
                flags = FIN;

                #if (DEBUG_TCP >= LOG_WARN)
                debugPutOffsetMsg(s, 42);    //@mxd:42:Timeout, resending FIN
                #endif
            }
            else
            {
                // Close on our own, we can't seem to communicate 
                // with the remote node anymore
                CloseSocket(ps);

                #if (DEBUG_TCP >= LOG_WARN)
                if ( ps->Flags.bServer ) {
                    debugPutOffsetMsg(s, 30);    //@mxd:30:state LISTEN: Max Retries, no responce to our close request, close forcefully!
                }
                else {
                    debugPutOffsetMsg(s, 31);    //@mxd:31:state CLOSED: Max Retries, no responce to our close request, close forcefully!
                }
                #endif
            }
            break;

        case TCP_FIN_WAIT_2:
        case TCP_CLOSING:
            // Close on our own, we can't seem to communicate 
            // with the remote node anymore
            CloseSocket(ps);

            #if (DEBUG_TCP >= LOG_WARN)
            if ( ps->Flags.bServer ) {
                debugPutOffsetMsg(s, 32);    //@mxd:32:state LISTEN: Max Retries, no responce to our close(2) request, close forcefully!
            }
            else {
                debugPutOffsetMsg(s, 33);    //@mxd:33:state CLOSED: Max Retries, no responce to our close(2) request, close forcefully!
            }
            #endif
            break;

        case TCP_TIME_WAIT:
            // Wait around for a while (2MSL) and then goto closed state
            CloseSocket(ps);
            break;
        
        case TCP_CLOSE_WAIT:
            flags = FIN;
            ps->smState = TCP_LAST_ACK;
            break;

        case TCP_LAST_ACK:
            // Send some more FINs or close anyway
            if(ps->RetryCount <= TCP_MAX_RETRY_COUNTS)
                // Added ACK to re-sent FIN packets
                //flags = FIN;
                flags = FIN | ACK;
            else
                CloseSocket(ps);
            break;
        }

        //Check if any flags have been set, and send requested TCP packet
        //Flags can be: FIN, SYN, RST, PSH, ACK and URG
        if(flags)
        {
            if(flags & ACK)
                seq = ps->SND_SEQ;    //Timeout, resending ACK. Set the sequence number equal to the last sequence number we sent.
            else
                seq = ps->SND_SEQ++;      //Increment to SEQ number of next segment we will send, is also the ACK number we are expecting to receive

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
}


/**
 * Reads a TCP segment from the MAC.
 *
 * @preCondition    TCPInit() is already called AND <br>
 *                  TCP segment is ready in MAC buffer
 *
 * @param remote    Remote node info
 * @param len       Total length of TCP semgent
 * @param localIP   The destination IP address received in the IP Header
 *
 * @return          TRUE if this function has completed its task <br>
 *                  FALSE otherwise
 */
BOOL TCPProcess(NODE_INFO *remote, IP_ADDR *localIP, WORD len)
{
    TCP_HEADER      TCPHeader;
    PSEUDO_HEADER   pseudoHeader;
    TCP_SOCKET      socket;
    WORD_VAL        checksum1;
    WORD_VAL        checksum2;
    BYTE            optionsSize;
    #if (DEBUG_TCP > LOG_OFF)
    //BYTE tmp;
    #endif


    // Calculate IP pseudoheader checksum. To calculate the TCP checksum, not only all bytes in
    //the TCP header have to be taken into account, but also bytes of the pseudoheader. This is a
    //a 12 byte segment created with data from TCP and IP
    pseudoHeader.SourceAddress      = remote->IPAddr;
    pseudoHeader.DestAddress        = *localIP;
    //pseudoHeader.DestAddress.v[0]   = localIP->v[0];
    //pseudoHeader.DestAddress.v[1]   = localIP->v[1];
    //pseudoHeader.DestAddress.v[2]   = localIP->v[2];
    //pseudoHeader.DestAddress.v[3]   = localIP->v[3];
    pseudoHeader.Zero               = 0x0;
    pseudoHeader.Protocol           = IP_PROT_TCP;
    pseudoHeader.TCPLength          = len;  // The length of the TCP segment, including both header and data. Note
                                            // that this is not a specific field in the TCP header; it is computed
    SwapPseudoTCPHeader(pseudoHeader);  //Swap bytes of TCPLength field - all other bytes are already correct

    checksum1.Val = ~CalcIPChecksum((BYTE*)&pseudoHeader,
        sizeof(pseudoHeader));


    // Now calculate TCP packet checksum in NIC RAM - should match
    // pesudo header checksum
    checksum2.Val = CalcIPBufferChecksum(len);

    // Compare checksums.  Note that the endianness is different.
    if(checksum1.v[0] != checksum2.v[1] || checksum1.v[1] != checksum2.v[0])
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

        MACDiscardRx(); //Discard the contents of the current RX buffer
        return TRUE;
    }

    // Retrieve TCP header.
    IPSetRxBuffer(0);   //Set current receive buffer access pointer to first byte of IP data = first byte of TCP header
    MACGetArray((BYTE*)&TCPHeader, sizeof(TCPHeader));  // Retrieve TCP header. The data is read from the current MAC RX Buffer
    SwapTCPHeader(&TCPHeader);                          //Swap all WORDs and DWORDs in header


    // Skip over options and retrieve all data bytes.
    optionsSize = (BYTE)((TCPHeader.DataOffset.Val << 2)-
        sizeof(TCPHeader));
    len = len - optionsSize - sizeof(TCPHeader);

    // Position packet read pointer to start of data area.
    IPSetRxBuffer((TCPHeader.DataOffset.Val << 2));

    // Find matching socket.
    socket = FindMatchingSocket(&TCPHeader, remote);
    if(socket != INVALID_SOCKET)
    {
        HandleTCPSeg(socket, remote, &TCPHeader, len);  //This TCP socket is handled by us (addressed to one
                                                        //our of sockets) - handle it
        #if (DEBUG_TCP >= LOG_DEBUG)
        debugPutMsg(2); //@mxd:2:Found Matching socket, assigned to TCP Handle %d
        debugPutByte(socket);
        #endif
    }
    else    //No matching socket found
    {
        // If this is an unknown socket, or we don't have any 
        // listening sockets available, discard it we can't 
        // process it right now
        MACDiscardRx();
        
        #if (DEBUG_TCP >= LOG_WARN)
        debugPutMsg(3); //@mxd:3:Could not find matching socket for destination port 0x%x%x
        debugPutByteHex(TCPHeader.DestPort.v[1]);
        debugPutByteHex(TCPHeader.DestPort.v[0]);
        #endif

//        // Send a RESET to the remote node is it knows that we 
//        // are not available
//        TCPHeader.AckNumber += len;
//        if( TCPHeader.Flags.bits.flagSYN ||
//            TCPHeader.Flags.bits.flagFIN )
//            TCPHeader.AckNumber++;
//        
//        SendTCP(remote,
//            TCPHeader.DestPort,
//            TCPHeader.SourcePort,
//            TCPHeader.AckNumber,
//            TCPHeader.SeqNumber,
//            RST);
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
 *                  TCPIsPutReady() != 0
 *
 * @param remote    Remote node info
 * @param localPort Source port number
 * @param remotePort Destination port number
 * @param seq       Segment sequence number
 * @param ack       Segment acknowledge number
 * @param flags     Segment flags
 * @param buffer    Buffer which this segment has to transmit. If this value is INVALID_BUFFER, a
 *                  available transmit buffer will be assigned to it, and len parameter should be 0!
 * @param len       Total data length for this segment.
 */
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

    //  Make sure that this Tx buffer isn't currently being transmitted
    while( !IPIsTxReady(TRUE) ) FAST_USER_PROCESS();    //TODO: This may need to be conditionally false

    //There is no TCP Data to be send with this Segment. Get next available TX Buffer. If the given
    //buffer != INVALID_BUFFER, then it contains the data that has to be sent!
    if ( buffer == INVALID_BUFFER ) {
        //Get handle to next available TX Buffer. The TCPIsPutReady() function that has to be called
        //prior to this function will determine if there is an available TX Buffer.
        buffer = MACGetTxBuffer(TRUE);
    }

    if(buffer == INVALID_BUFFER)
        return;

    IPSetTxBuffer(buffer, 0);

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
    //CalcIPBufferChecksum() function requires buffer access pointer to be set
    IPSetTxBuffer(buffer, 0);

    checkSum.Val = CalcIPBufferChecksum(len);   //Performs checksum calculation in MAC buffer itself.

    //Set TX Buffer access pointer (All future read and writes to the TX Buffer will be to the set location)
    //to 16th byte of "IP data" = 16th byte of "TCP Header" = TCP Header's checksum field
    IPSetTxBuffer(buffer, 16);
    MACPut(checkSum.v[1]);
    MACPut(checkSum.v[0]);
    
    //Set TX Buffer access pointer (All future read and writes to the TX Buffer will be to the set location)
    //to first byte of "IP data" = "TCP Header" in this case
    MACSetTxBuffer(buffer, 0);

    #if (DEBUG_TCP >= LOG_INFO)
    debugPutMsg(47); //@mxd:47:Sent TCP segment, AckNum=0x%x%x, SeqNum=0x%x%x, Len=0x%x%x
    debugPutByteHex((BYTE)(tack>>8));
    debugPutByteHex((BYTE)tack);
    debugPutByteHex((BYTE)(tseq>>8));
    debugPutByteHex((BYTE)tseq);
    debugPutByteHex((BYTE)(len>>8));
    debugPutByteHex((BYTE)len);
    #endif

    //Transmits the contents of the current transmit buffer = "TCP Header" and "TCP Data"
    MACFlush();

    // If we send the packet again, the remote node might think that we timed 
    // out and retransmitted.  It could thus immediately send back an ACK and 
    // dramatically improve throuput.
    #if !defined(TCP_NO_WAIT_FOR_ACK)
        #if defined(TCP_SEND_EACH_SEGMENT_TWICE)
        while(!IPIsTxReady(TRUE)) FAST_USER_PROCESS();
        MACFlush();
        #endif
    #endif
}


/**
 * Go through list of connections, and look check if any of them match the given destination socket.
 *
 * @preCondition    TCPInit() is already called
 *
 * @param h         TCP Header to be matched against.
 * @param remote    Node who sent this header.
 *
 * @return          A socket that matches with given header and remote
 *                  node is searched.
 *                  If such socket is found, its index is returned
 *                  else INVALID_SOCKET is returned.
 */
static TCP_SOCKET FindMatchingSocket(TCP_HEADER *h, NODE_INFO *remote)
{
    SOCKET_INFO *ps;
    TCP_SOCKET s;
    TCP_SOCKET partialMatch;

    partialMatch = INVALID_SOCKET;

    #if (DEBUG_TCP >= LOG_DEBUG)
    debugPutMsg(44); //@mxd:44:FindMatchingSocket() called for message addressed to port 0x%x%x
    debugPutByteHex(h->DestPort.v[1]);
    debugPutByteHex(h->DestPort.v[0]);
    #endif

    //Go through all sockets, and search for one 
    for ( s = 0; s < MAX_SOCKETS; s++ )
    {
        ps = &TCB[s];

        if ( ps->smState != TCP_CLOSED )
        {
            /*
            #if (DEBUG_TCP >= LOG_DEBUG)
            debugPutMsg(43); //@mxd:43:FindMatchingSocket(), TCB[%d] - comparing local port 0x%x%x to destination port
            debugPutByte(s);
            debugPutByteHex((BYTE)ps->localPort >> 8);
            debugPutByteHex((BYTE)ps->localPort);
            #endif
            */
            
            if ( ps->localPort == h->DestPort.Val)
            {
                if ( ps->smState == TCP_LISTEN )
                    partialMatch = s;

                if ( ps->remotePort == h->SourcePort.Val &&
                    ps->remote.IPAddr.Val == remote->IPAddr.Val )
                {
                    return s;
                }
            }
        }
    }

    // We are not listening on this port
    if(partialMatch == INVALID_SOCKET)
        return INVALID_SOCKET;

    // Copy the remote node IP/MAC address and source TCP port 
    // number into our TCB and return this socket to the caller
    ps = &TCB[partialMatch];
    memcpy((void*)&ps->remote, (void*)remote, sizeof(*remote));
    //ps->remote            = *remote;
    ps->remotePort          = h->SourcePort.Val;
    ps->Flags.bIsGetReady   = FALSE;
    if(ps->TxBuffer != INVALID_BUFFER)
    {
        //By assigning the TxBuffer with INVALID_BUFFER, the first available TX Buffer will be used
        //the first time the TCPPut() or TCPPutArray() function is called.
        MACDiscardTx(ps->TxBuffer);
        ps->TxBuffer        = INVALID_BUFFER;
    }
    ps->Flags.bIsPutReady   = TRUE;
    
    return partialMatch;
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
    if(ps->TxBuffer != INVALID_BUFFER)
    {
        MACDiscardTx(ps->TxBuffer);
        ps->TxBuffer            = INVALID_BUFFER;
        ps->Flags.bIsPutReady   = TRUE;
    }

    ps->remote.IPAddr.Val = 0x00;
    ps->remotePort = 0x00;
    if(ps->Flags.bIsGetReady)
    {
        MACDiscardRx(); //Discard the contents of the current RX buffer
    }
    ps->Flags.bIsGetReady       = FALSE;
    ps->TimeOut                 = TCP_START_TIMEOUT_VAL;

    ps->Flags.bIsTxInProgress   = FALSE;

    if(ps->Flags.bServer)
    {
        ps->smState = TCP_LISTEN;
    }
    else
    {
        ps->smState = TCP_CLOSED;
    }

    ps->TxCount = 0;

    return;
}


/**
 * A TCP Segment addressed to one of our sockets has been received! Handle it.
 *
 * @preCondition    TCPInit() is already called     AND
 *                  TCPProcess() is the caller.
 *
 * @param s         Socket that owns this segment
 * @param remote    Remote node info
 * @param h         TCP Header
 * @param len       Length of TCP data. This does NOT include the header length!
 *
 * @return          TCP FSM is executed on given socket with
 *                  given TCP segment.
 */
static void HandleTCPSeg(TCP_SOCKET s,
                         NODE_INFO *remote,
                         TCP_HEADER *h,
                         WORD len)
{
    DWORD ack;  //SEQ and ACK numbers we will use for tranmission, if there is any transmission
    DWORD seq;
    DWORD prevAck, prevSeq; //TODO Investigate if prevAck and prevSeq have to be included in the TCB structure. This might be neccessary for multiple TCP connections?
    SOCKET_INFO *ps;
    BYTE flags;

    ps = &TCB[s];
    
    //Store length of this TCP message's Data
    _tcpDataLength = len;
    
    flags = 0x00;

    // Clear retry count and timeout info
    ps->RetryCount  = 0;
    ps->startTick   = TickGet16bit();    //Set timeout reference
    ps->TimeOut = TCP_START_TIMEOUT_VAL;

    // Reset FSM, if RST is received.
    if(h->Flags.bits.flagRST)
    {
        MACDiscardRx();
        ps->smState = ps->Flags.bServer ? TCP_LISTEN : TCP_SYN_SENT;
        
        #if (DEBUG_TCP >= LOG_WARN)
        if ( ps->Flags.bServer ) {
            debugPutOffsetMsg(s, 34);    //@mxd:34:state LISTEN: Received RST, close forcefully!
        }
        else {
            debugPutOffsetMsg(s, 35);    //@mxd:35:state CLOSED: Received RST, close forcefully!
        }
        #endif
        
        return;
    }

    seq = ps->SND_SEQ;
    
    // ack is just a temporary variable
    ack = h->Window.Val - (seq - h->AckNumber) - ps->TxCount;
    if((signed long)ack < 0)
        ps->RemoteWindow = 0;
    else
        ps->RemoteWindow = ack;

    #if (DEBUG_TCP >= LOG_INFO)
    debugPutMsg(49); //@mxd:49:Rcvd TCP segment, AckNum=0x%x%x, SeqNum=0x%x%x, Len=0x%x%x
    debugPutByteHex((BYTE)(h->AckNumber>>8));
    debugPutByteHex((BYTE)h->AckNumber);
    debugPutByteHex((BYTE)(h->SeqNumber>>8));
    debugPutByteHex((BYTE)h->SeqNumber);
    debugPutByteHex((BYTE)(len>>8));
    debugPutByteHex((BYTE)len);
    #endif

#ifdef STACK_CLIENT_MODE
    //Current state = SYN_SENT
    //We have sent a SYN, and are waiting for SYN and ACK
    // - If we receive SYS and ACK, ESTABLISH a connection
    // - If we receive SYN, go to SYN_RCVD state. We are now waiting for an ACK before going to ESTABLISHED state
    if(ps->smState == TCP_SYN_SENT)
    {
        // Check if this is a SYN packet.  Unsynchronized, we cannot
        // handle any other packet types.
        if(!h->Flags.bits.flagSYN)
        {
            MACDiscardRx();

            // Send out a RESET if the remote node thinks a connection is already established
            if(h->Flags.bits.flagACK)
            {
                #if (DEBUG_TCP >= LOG_WARN)
                debugPutOffsetMsg(s, 17);    //@mxd:17:Discarding ACK, waiting for SYN
                #endif

                flags = RST;
                goto SendTCPControlPacket;
            }

            #if (DEBUG_TCP >= LOG_WARN)
            debugPutOffsetMsg(s, 18);    //@mxd:18:Discarding message, waiting for SYN
            #endif

            return;
        }
        
        //!! Received SYN !! If the program gets here, we HAVE received a SYNC flag

        // We now have a sequence number for the remote node
        ps->SND_ACK = h->SeqNumber + len + 1;
        ack = ps->SND_ACK;

        //We receive SYN, but no ACK, we must go to TCP_SYN_RECEIVED state. We are now waiting for an ACK before going to ESTABLISHED state
        if(!h->Flags.bits.flagACK)
        {
            ps->smState = TCP_SYN_RECEIVED;
            MACDiscardRx();
            // Send out a SYN+ACK for simultaneous connection open
            flags = SYN | ACK;
            #if (DEBUG_TCP >= LOG_INFO)
            debugPutOffsetMsg(s, 16);    //@mxd:16:state SYN_RCVD: Rcvd ACK, waiting for SYN. Sending ACK|SYN
            #endif
            goto SendTCPControlPacket;
        }

        //!! Received SYN+ACK !! If the program gets here, we HAVE received SYN and ACK flags
        #if (DEBUG_TCP >= LOG_INFO)
        if (len > 0) {
            debugPutOffsetMsg(s, 14);    //@mxd:14:state EST: Rcvd ACK|SYN with data, sending ACK
        }
        else {
            debugPutOffsetMsg(s, 15);    //@mxd:15:state EST: Rcvd ACK|SYN (without data), sending ACK
        }
        #endif

        // We received SYN+ACK, establish the connection now
        ps->smState = TCP_ESTABLISHED;
        // Send out an ACK
        flags = ACK;

        ps->RemoteWindow = h->Window.Val;

        // Check for application data and make it 
        // available, if present
        if(len)
        {
            ps->Flags.bIsGetReady   = TRUE;
            ps->RxCount             = len;      //Set RxCount = Length of TCP Data (NOT including header, only data)
            ps->Flags.bFirstRead    = TRUE;
        }
        else    // No application data in this packet
        {
            MACDiscardRx();
        }
        goto SendTCPControlPacket;
    }
#endif

    // Handle TCP_LISTEN state
    if(ps->smState == TCP_LISTEN )
    {
        MACDiscardRx();     //Discard the contents of the current RX buffer

        // Send a RST if this isn't a SYN packet
        if(!h->Flags.bits.flagSYN)
        {
            flags = RST;

            #if (DEBUG_TCP >= LOG_DEBUG)
            debugPutOffsetMsg(s, 22);    //@mxd:22:Received not-SYN msg while in LISTEN, ignoring and sending RST
            #endif

            goto SendTCPControlPacket;
        }

        ps->SND_ACK = h->SeqNumber + len + 1;
        ps->RemoteWindow = h->Window.Val;

        // This socket has received connection request (SYN).
        // Remember calling node, assign next segment seq. number
        // for this potential connection.
        memcpy((void*)&ps->remote, (const void*)remote, sizeof(*remote));
        ps->remotePort = h->SourcePort.Val;

        // Grant connection request.
        ps->smState = TCP_SYN_RECEIVED;
        seq = ps->SND_SEQ++;
        ack =  ps->SND_ACK;
        flags = SYN | ACK;

        #if (DEBUG_TCP >= LOG_INFO)
        debugPutOffsetMsg(s, 21);    //@mxd:21:state SYN_RCVD: Received SYN while in LISTEN, sending ACK|SYN
        #endif

        goto SendTCPControlPacket;
    }


    // Remember current seq and ack for our connection so that if
    // we have to silently discard this packet, we can go back to
    // previous ack and seq numbers.
    prevAck = ps->SND_ACK;
    prevSeq = ps->SND_SEQ;

    //The ACK number will be the sequence number of the next data byte we are waiting to receive.
    //This is the received SEQ number + length (data received)
    ack = h->SeqNumber;
    ack += (DWORD)len;
    seq = ps->SND_SEQ;

    // State is something other than TCP_LISTEN, handle it.
    {
        // Check to see if the incomming sequence number is what 
        // we expect (last transmitted ACK value).  Throw this packet 
        // away if it is wrong.
        if(h->SeqNumber == prevAck)
        {
            // After receiving a SYNchronization request, we expect an 
            // ACK to our transmitted SYN. After this, we can go to the Established state
            if(ps->smState == TCP_SYN_RECEIVED)
            {
                //If ACK received, we have established a connection! Go to ESTABLISHED state
                if(h->Flags.bits.flagACK)
                {
                    // ACK received as expected, this connection is 
                    // now established
                    ps->SND_ACK = ack;
                    ps->smState = TCP_ESTABLISHED;

                    // Check if this first packet has application data 
                    // in it.  Make it available if so.
                    if(len)
                    {
                        ps->Flags.bIsGetReady   = TRUE;
                        ps->RxCount             = len;  //Set RxCount = Length of TCP Data (NOT including header, only data)
                        ps->Flags.bFirstRead    = TRUE;

                        #if (DEBUG_TCP >= LOG_INFO)
                        debugPutOffsetMsg(s, 12);    //@mxd:12:state EST: Connection Established, ACK was received WITH DATA
                        #endif
                    }
                    else {
                        MACDiscardRx();

                        #if (DEBUG_TCP >= LOG_INFO)
                        debugPutOffsetMsg(s, 13);    //@mxd:13:state EST: Connection Established, ACK was received (without data)
                        #endif
                    }
                }
                else    // No ACK to our SYN, discard
                {
                    MACDiscardRx();
                }
            }
            // Connection is established, closing, or otherwise. Current state is NOT LISTEN, SYN_RCVD or SYN_SENT
            else
            {

                // Save the seq+len value of the packet for our future 
                // ACK transmission, and so out of sequence packets 
                // can be detected in the future.
                ps->SND_ACK = ack;

                // Handle packets received while connection established. Current state = ESTABLISHED
                if(ps->smState == TCP_ESTABLISHED)
                {
                    // If this packet has the ACK set, mark the packet it refers to (Ack Number)
                    // as no longer needed for possible retransmission.
                    // TODO: Make this more sophisticated so that partial ACKs due to fragmentation are handled correctly.  i.e. Keep a real output stream buffer with slidable window capability.
                    if(h->Flags.bits.flagACK && (h->AckNumber == ps->SND_SEQ) && !ps->Flags.bIsPutReady)
                    {
                        //Data ACK, can now be discarded. Does not have to be retransmitted
                        if(ps->TxBuffer != INVALID_BUFFER)
                        {
                            MACDiscardTx(ps->TxBuffer);
                            ps->TxBuffer            = INVALID_BUFFER;
                            ps->Flags.bIsPutReady   = TRUE;
                        }
                    }

                    // Check if the remote node is closing the connection, Received FIN flag
                    if(h->Flags.bits.flagFIN)
                    {
                        flags = FIN | ACK;
                        seq = ps->SND_SEQ++;
                        ack = ++ps->SND_ACK;
                        ps->smState = TCP_LAST_ACK;
                        
                        #if (DEBUG_TCP >= LOG_INFO)
                        debugPutOffsetMsg(s, 19);    //@mxd:19:state LAST_ACK: Received FIN, sending FIN|ACK
                        #endif
                    }

                    // Check if there is any application data in 
                    // this packet.
                    if(len)
                    {
                        // There is data.  Make it available if we 
                        // don't already have data available.
                        if(!ps->Flags.bIsGetReady)
                        {
                            ps->Flags.bIsGetReady   = TRUE;
                            ps->RxCount             = len;
                            ps->Flags.bFirstRead    = TRUE;

                            // 4/1/02
                            flags |= ACK;

                            #if (DEBUG_TCP >= LOG_DEBUG)
                            debugPutOffsetMsg(s, 23);    //@mxd:23:Received data, sending ACK
                            #endif
                        }
                        // There is data, but we cannot handle it at this time.
                        else
                        {
                            // Since we cannot accept this packet,
                            // restore to previous seq and ack.
                            // and do not send anything back.
                            // Host has to resend this packet when
                            // we are ready.
                            ps->SND_SEQ = prevSeq;
                            ps->SND_ACK = prevAck;

                            MACDiscardRx();

                            #if (DEBUG_TCP >= LOG_ERROR)
                            debugPutOffsetMsg(s, 24);    //@mxd:24:RX Buffer overflow! Received data, but had to discard it
                            #endif
                        }
                    }
                    // There is no data in this packet, and thus it 
                    // can be thrown away.
                    else
                    {
                        MACDiscardRx();
                    }
                }
                // Connection is not established; check if we've sent 
                // a FIN and expect our last ACK
                else if(ps->smState == TCP_LAST_ACK)
                {
                    MACDiscardRx();

                    if(h->Flags.bits.flagACK)
                    {
                        #if (DEBUG_TCP >= LOG_INFO)
                        if ( ps->Flags.bServer ) {
                            debugPutOffsetMsg(s, 36);    //@mxd:36:state LISTEN: Received ACK for FIN sent
                        }
                        else {
                            debugPutOffsetMsg(s, 37);    //@mxd:37:state CLOSED: Received ACK for FIN sent
                        }
                        #endif

                        CloseSocket(ps);
                    }
                }
                else if(ps->smState == TCP_FIN_WAIT_1)
                {
                    MACDiscardRx();

                    if(h->Flags.bits.flagFIN)
                    {
                        flags = ACK;
                        ack = ++ps->SND_ACK;
                        if(h->Flags.bits.flagACK)
                        {
                            #if (DEBUG_TCP >= LOG_INFO)
                            if ( ps->Flags.bServer ) {
                                debugPutOffsetMsg(s, 38);    //@mxd:38:state LISTEN: Received ACK|FIN for FIN sent
                            }
                            else {
                                debugPutOffsetMsg(s, 39);    //@mxd:39:state CLOSED: Received ACK|FIN for FIN sent
                            }
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
                    else if(h->Flags.bits.flagACK)
                    {
                        #if (DEBUG_TCP >= LOG_INFO)
                        debugPutOffsetMsg(s, 50);    //@mxd:50:state FIN-WAIT-2: Received ACK, waiting for FIN
                        #endif

                        ps->smState = TCP_FIN_WAIT_2;
                    }
                }
                //Current state is TCP_FIN_WAIT_2
                else if(ps->smState == TCP_FIN_WAIT_2)
                {
                    MACDiscardRx();

                    if(h->Flags.bits.flagFIN)
                    {
                        flags = ACK;
                        ack = ++ps->SND_ACK;

                        #if (DEBUG_TCP >= LOG_INFO)
                        if ( ps->Flags.bServer ) {
                            debugPutOffsetMsg(s, 51);    //@mxd:51:state LISTEN: Received FIN
                        }
                        else {
                            debugPutOffsetMsg(s, 52);    //@mxd:52:state CLOSED: Received FIN
                        }
                        #endif

                        CloseSocket(ps);
                    }
                }
                //Current state is CLOSING
                else if ( ps->smState == TCP_CLOSING )
                {
                    MACDiscardRx(); //Discard the contents of the current RX buffer

                    if ( h->Flags.bits.flagACK )
                    {
                        #if (DEBUG_TCP >= LOG_INFO)
                        if ( ps->Flags.bServer ) {
                            debugPutOffsetMsg(s, 36);    //@mxd:36:state LISTEN: Received ACK for FIN sent
                        }
                        else {
                            debugPutOffsetMsg(s, 37);    //@mxd:37:state CLOSED: Received ACK for FIN sent
                        }
                        #endif

                        CloseSocket(ps);
                    }
                }
            }
        }
        // This packet's sequence number does not match what we were 
        // expecting (the last value we ACKed).  Throw this packet 
        // away.  This may happen if packets are delivered out of order.
        // Not enough memory is available on our PIC or Ethernet 
        // controller to implement a robust stream reconstruction 
        // buffer.  As a result, the remote node will just have to 
        // retransmit its packets starting with the proper sequence number.
        else
        {
            #if (DEBUG_TCP >= LOG_WARN)
            debugPutOffsetMsg(s, 48);    //@mxd:48:state LISTEN: Discarded received packet, out of sequence!
            #endif

            MACDiscardRx();

            // Send a new ACK out in case if the previous one was lost 
            // (ACKs aren't ACKed).  This is required to prevent an 
            // unlikely but possible situation which would cause the 
            // connection to time out if the ACK was lost and the 
            // remote node keeps sending us older data than we are 
            // expecting.
            flags = ACK;    
            ack = prevAck;
        }
    }

SendTCPControlPacket:
    if(flags)
    {
        // If the last message sent was not ackonwledged, send it again with piggyback flags
        if(!ps->Flags.bIsPutReady && (ps->TxBuffer != INVALID_BUFFER) && (ps->TxCount != 0) ) {
            ps->SND_SEQ -= ps->TxCount; //Shift back the sliding window
            TransmitTCP(&ps->remote,    //Remote node info
                ps->localPort,          //Source port number
                ps->remotePort,         //Destination port number
                ps->SND_SEQ,            //Segment sequence number
                ps->SND_ACK,            //Segment acknowledge number
                flags | PSH,            //Add PSH flag since frame contains data
                ps->TxBuffer,
                ps->TxCount);
            ps->SND_SEQ += ps->TxCount;   //Re-align the sliding window
        // If there is no message pending for retries, send a new one without data
        } else {
            
            //Send TCP Message with given flags. Sends a TCP Message with the given data. The sent TCP Message
            //will contain only a "TCP Header" with possible "TCP Options". No "TCP Data" is transmitted.
            SendTCP(remote,
                h->DestPort.Val,
                h->SourcePort.Val,
                seq,
                ack,
                flags);
        }
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

/**
 * Sets the receive buffer access pointer to given offset in TCP Data.
 * For example, if TCP data is a HTTP packet, an offset of 0 will set
 * access to first byte of HTTP header.
 * Layers that use TCP services (HTTP...) should call this function
 * to set the access pointer for the current buffer.
 *
 * @param s         Socket
 * @param offset    Offset, An offset with respect to TCP Data
 *
 */
void TCPSetRxBuffer(TCP_SOCKET s, WORD offset) {
    BYTE            tcpHeaderLength;    //TCP Header length, including any options

    SOCKET_INFO *ps;
    ps = &TCB[s];

    //Get TCP Header Length - including any options
    IPSetRxBuffer(12);                          //Header length (x4) is in bits 4-7 at offet 12 in header
    tcpHeaderLength = (MACGet() >> 2) & 0x3c;   //Bits 4-7 give number of 32 bit words in TCP header - including options

    // !!! Alternative method !!!
    /*
    BYTE            ipHeaderLength;     //IP Header length, including any options
    WORD_VAL        ipTotalLength;

    //Get IP Header Length - including any options
    MACSetRxBuffer(0);                          //Header length (x4) is in bits 0-3 at offet 0 in header
    ipHeaderLength = (MACGet() & 0x0f) << 2;    //Bits 0-3 give number of 32 bit words in IP header length - including options
 
    //Get IP Total Length = IP header and data
    MACSetRxBuffer(2);
    ipTotalLength.v[1] = MACGet();
    ipTotalLength.v[0] = MACGet();

    //Reset RxCount to new receive buffer access point. Is number of bytes remaining of TCP data = bytes that still have to be read
    ps->RxCount = ipTotalLength.Val - (ipHeaderLength + tcpHeaderLength + offset);
    // ----- Two lines of code below only included to better understand code, don't use! -----
    //tcpDataLength = ipTotalLength.Val - (ipHeaderLength + tcpHeaderLength);
    //ps->RxCount = tcpDataLength - offset;
    */

    //The _tcpDataLength variable can only be used as long as the TCP/IP stack reads a whole TCP
    //message at a time. If this stack would be modified so that multiple TCP messages can be read
    //simultaniously, this variable will can not be used anymore! Use alternative method above

    ps->RxCount = _tcpDataLength - offset;

    // Position packet read pointer to start of data area.
    IPSetRxBuffer(tcpHeaderLength + offset);
}


/**
 * Gets the current receive buffer access pointer for given socket. This is the
 * current offset in the TCP data.
 * The value returned can be used as the Offset parameter for the TCPSetRxBuffer() function
 * at a later stage to reset the receive buffer access pointer to it's current
 * location.
 * For example, when the current receive buffer access pointer points to the first
 * byte of the TCP data, this function will return 0.
 *
 * @param s         Socket
 *
 * @return Current receive buffer access pointer
 *
 */
WORD TCPGetRxBufferPos(TCP_SOCKET s) {
    SOCKET_INFO *ps;
    ps = &TCB[s];


    // !!! Alternative method !!!
    /*
    BYTE            tcpHeaderLength;    //TCP Header length, including any options
    BYTE            ipHeaderLength;     //IP Header length, including any options
    WORD_VAL        ipTotalLength;

    //Get TCP Header Length - including any options
    IPSetRxBuffer(12);                          //Header length (x4) is in bits 4-7 at offet 12 in header
    tcpHeaderLength = (MACGet() >> 2) & 0x3c;   //Bits 4-7 give number of 32 bit words in TCP header - including options

    //Get IP Header Length - including any options
    MACSetRxBuffer(0);                          //Header length (x4) is in bits 0-3 at offet 0 in header
    ipHeaderLength = (MACGet() & 0x0f) << 2;    //Bits 0-3 give number of 32 bit words in IP header length - including options
 
    //Get IP Total Length = IP header and data
    MACSetRxBuffer(2);
    ipTotalLength.v[1] = MACGet();
    ipTotalLength.v[0] = MACGet();

    //Return current receive buffer access pointer offset
    return = ipTotalLength.Val - (ipHeaderLength + tcpHeaderLength + ps->RxCount);
    // ----- Two lines of code below only included to better understand code, don't use! -----
    //tcpDataLength = ipTotalLength.Val - (ipHeaderLength + tcpHeaderLength);
    //return tcpDataLength - ps->RxCount;
    */

    //The _tcpDataLength variable can only be used as long as the TCP/IP stack reads a whole TCP
    //message at a time. If this stack would be modified so that multiple TCP messages can be read
    //simultaniously, this variable can not be used anymore! Use alternative method above

    //RxCount contains number of bytes remaining of TCP data = bytes that still have to be read.
    //It is eqaual to TCPDataLengh when no bytes have been read, and offset=0
    return _tcpDataLength - ps->RxCount;
}


#endif //#if defined(STACK_USE_TCP)
