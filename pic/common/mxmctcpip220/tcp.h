/*********************************************************************
 *
 *                  TCP Module Defs for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        TCP.h
 * Dependencies:    stacktsk.h
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
 * Nilesh Rajbharti     5/8/01  Original        (Rev 1.0)
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 ********************************************************************/
/*******************************************************
----------------- TCP Configuration --------------------
********************************************************
//The following should be defined in the projdefs.h file OR on the command line

//Maximum number of times a connection be retried before closing it down.
#define TCP_MAX_RETRY_COUNTS    (3)

//TCP Timeout value to begin with.
#define TCP_START_TIMEOUT_VAL   ((TICK)TICK_SECOND * (TICK)60)

//Define ports
#define TCP_LOCAL_PORT_START_NUMBER (1024)
#define TCP_LOCAL_PORT_END_NUMBER   (5000)
/********************************************************/

#ifndef TCP_H
#define TCP_H

#include "projdefs.h"
#include "ip.h"

#include "tick.h"

typedef BYTE TCP_SOCKET;
typedef WORD TCP_PORT;

/////////////////////////////////////////////////
//Default defines - they can be overridden in projdefs.h file

/*
 * Maximum number of times a connection be retried before
 * closing it down.
 */
#if !defined(TCP_MAX_RETRY_COUNTS)      //To change this default value, define it in projdefs.h
#define TCP_MAX_RETRY_COUNTS    (3)
#endif

/*
 * TCP Timeout value to begin with.
 */
#if !defined(TCP_START_TIMEOUT_VAL)     //To change this default value, define it in projdefs.h
#define TCP_START_TIMEOUT_VAL   ((TICK)TICK_SECOND * (TICK)60)
#endif

#if !defined(TCP_LOCAL_PORT_START_NUMBER)   //To change this default value, define it in projdefs.h
#define TCP_LOCAL_PORT_START_NUMBER (1024)
#endif

#if !defined(TCP_LOCAL_PORT_END_NUMBER)     //To change this default value, define it in projdefs.h
#define TCP_LOCAL_PORT_END_NUMBER   (5000)
#endif

#define INVALID_SOCKET      (0xfe)
#define UNKNOWN_SOCKET      (0xff)

#define REMOTE_HOST(s)      (TCB[s].remote)

/*
 * TCP States as defined by rfc793
 */
typedef enum _TCP_STATE
{
    TCP_CLOSED,     //RFC793 CLOSED
    TCP_LISTEN,     //RFC793 LISTEN
    TCP_SYN_RCVD,   //RFC793 SYN-RECEIVED
    TCP_SYN_SENT,   //RFC793 SYN-SENT
    TCP_EST,        //RFC793 ESTABLISHED
    TCP_FIN_WAIT_1, //RFC793 FIN-WAIT-1
    TCP_CLOSING,    //RFC793 CLOSING
    TCP_TIMED_WAIT, //RFC793 TIME-WAIT
    TCP_SEND_RST,
    TCP_DATA_READY,
    TCP_LAST_ACK,   //RFC793 LAST-ACK
    TCP_CLOSE,      
    TCP_INVALID
} TCP_STATE;

/*
 * Socket info.
 * Union is used to create anonymous structure members.
 */
typedef struct _SOCKET_INFO
{
    TCP_STATE smState;		//1 byte

    NODE_INFO remote;		//10 bytes
    TCP_PORT localPort;		//2 bytes
    TCP_PORT remotePort;	//2 bytes

    BUFFER TxBuffer;		//1 byte
    WORD TxCount;			//2 bytes
    WORD RxCount;			//Length of TCP Data (NOT including header, only data)

    DWORD SND_SEQ;			//4 bytes
    DWORD SND_ACK;			//4 bytes

    BYTE RetryCount;		//1 byte
    TICK startTick;			//4 bytes
    TICK TimeOut;			//4 bytes

    struct
    {
        unsigned int bServer        : 1;
        unsigned int bIsPutReady    : 1;
        //Indicates that the first byte of the TCP Data has to be read. This is the first byte after the TCP header
        unsigned int bFirstRead     : 1;
        //Only one TCP socket will ever have this flag set at a given time. This indicates that the
        //MAC RX Ring Buffer pointer is currently set to this socket's packet. Any function that
        //reads the MAC RX Ring Buffer, will read data from this socket's packet.
        unsigned int bIsGetReady    : 1;
        unsigned int bIsTxInProgress : 1;
    } Flags;				//1 byte

} SOCKET_INFO;	//38 bytes


#if !defined(THIS_IS_TCP)
/*
 * These are all sockets supported by this TCP.
 */
extern SOCKET_INFO TCB[MAX_SOCKETS];
#endif



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
void        TCPInit(void);



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
TCP_SOCKET  TCPListen(TCP_PORT port);



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
TCP_SOCKET  TCPConnect(NODE_INFO *remote, TCP_PORT port);


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
BOOL        TCPIsConnected(TCP_SOCKET s);


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
void        TCPDisconnect(TCP_SOCKET s);


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
BOOL        TCPIsPutReady(TCP_SOCKET s);


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
BOOL        TCPPut(TCP_SOCKET socket, BYTE byte);


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
BOOL        TCPFlush(TCP_SOCKET socket);

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
BOOL        TCPIsGetReady(TCP_SOCKET s);


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
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
BOOL        TCPGet(TCP_SOCKET socket, BYTE *byte);


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
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
WORD        TCPGetArray(TCP_SOCKET s, BYTE *buffer, WORD count);


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
BOOL        TCPDiscard(TCP_SOCKET socket);

/*********************************************************************
 * Function:        BOOL TCPProcess(NODE_INFO* remote,
 *                                  IP_ADDR *localIP,
 *                                  WORD len)
 *
 * PreCondition:    TCPInit() is already called     AND
 *                  TCP segment is ready in MAC buffer
 *
 * Input:           remote      - Remote node info
 *                  len         - Total length of TCP semgent.
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
BOOL        TCPProcess(NODE_INFO *remote, IP_ADDR *localIP, WORD len);


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
void        TCPTick(void);

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
SOCKET_INFO* TCPGetSocketInfo(TCP_SOCKET s);

#endif
