/**
 * @brief           UDP Module for Modtronix TCP/IP Stack
 * @file            udp.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    stacktsk.h, mac.h
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_tcpip_base_udp
 *
 * @section description Description
 **********************************
 * This module contains the UDP code.
 * <br>For a detailed description, see the @ref mod_tcpip_base_udp section of this document - in 
 * [Modules] [TCP/IP Stack] [TCP/IP Base Protocols].
 *
 * @subsection udp_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //-------------------- UDP Configuration --------------------
 //*********************************************************************

 //When defined, the code will be compiled for optimal speed. If not defined, code is defined for smallest size.
 #define UDP_SPEED_OPTIMIZE
 @endcode
 *********************************************************************/

 /*********************************************************************
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
 * 2005-09-01, David Hosken:
 *    - Created documentation for existing code
 * 2001-03-19, Nilesh Rajbharti:
 *    - Original
 *********************************************************************/


/////////////////////////////////////////////////
// UDP - documentation module
/**
@defgroup mod_tcpip_base_udp UDP
@ingroup mod_tcpip_base
The UDP layer of the Modtronix TCP/IP stack is implemented by the file "udp.c". The header
file "udp.h" defines the services provided by the layer. In this stack architecture, UDP is
an active layer. It fetches UDP packets and notifies corresponding UDP socket of data arrival
or transmission. The UDP module is implemented as a cooperative task, performing automatic
operations without the knowledge of the main application.

"udp.h" provides UDP socket services and hides all UDP packet handling from the caller. The layer
allows up to 254 UDP sockets (the number limited only by available memory and compiler used). With
more than one socket, higher level applications can maintain multiple simultaneous UDP connections;
more than one application could be using this layer. It is important to know that each socket
consumes approximately 21 bytes (check 'UDP.h' file for actual consumption) and increases overall
UDP processing time.

Unlike other socket implementations, all sockets in the Modtronix TCP/IP
Stack share one or more common transmit buffers. This approach reduces overall RAM requirements,
but it may create a potential problem, where a few sockets reserve all available transmit buffers
and do not release them on time for other sockets to use. Under these circumstances, remote hosts
and/or local applications would not be able to contact the stack. To avoid this, users must make
sure that there are enough transmit buffers for all sockets. On the receive side, there is only
one receive buffer. If a socket receives its data, the owner of that socket must <b>fetch and discard
the receive buffer in one task time</b> in order for the other sockets to receive their data. This design
mandates that once a task detects a packet it is interested in, it must consume the complete packet
in one task time. A task cannot fetch part of a packet during one task time and expect to fetch
the rest of the packet later.

The UDP specifications do not mandate that checksum calculation be performed on UDP packets. To reduce
overall program and data memory requirements, the Modtronix TCP/IP Stack does not implement UDP
checksum calculation; instead, it sets the checksum fields to zero to indicate that no checksum
calculation is performed. This design decision requires that all modules utilizing the UDP module
ensure their data integrity.
*/


#ifndef UDP_H
#define UDP_H

#include "net\ip.h"
#include "net\mac.h"

/** Handle to a UDP port */
typedef WORD UDP_PORT;

/** Handle to a UDP socket */
typedef BYTE UDP_SOCKET;

/** UDP Socket - is 22 bytes long */
typedef struct _UDP_SOCKET_INFO
{
    NODE_INFO   remoteNode;    //10 bytes
    WORD        remotePort;    //2 bytes
    WORD        localPort;    //2 bytes
    WORD        TxCount;    //2 bytes

    //The number of bytes left to be read from the receive buffer. When 0, there are no bytes to be read
    WORD        RxCount;    //2 bytes
    BUFFER      TxBuffer;    //1 byte
    //WORD        TxOffset;   //2 bytes

    struct
    {
        unsigned int bFirstRead     : 1;
    } Flags;

} UDP_SOCKET_INFO;


#define INVALID_UDP_SOCKET      (0xfful)
#define INVALID_UDP_PORT        (0ul)

/*
 * All module utilizing UDP module will get extern definition of
 * activeUDPSocket.  While UDP module itself will define activeUDPSocket.
 */
#if !defined(THIS_IS_UDP_MODULE)
    extern UDP_SOCKET activeUDPSocket;
    extern UDP_SOCKET_INFO  UDPSocketInfo[MAX_UDP_SOCKETS];
#endif

/** UDP Packet header */
typedef struct _UDP_HEADER
{
    WORD_VAL    SourcePort;
    WORD_VAL    DestinationPort;
    WORD_VAL    Length;
    WORD_VAL    Checksum;
} UDP_HEADER;


/**
 * Initializes internal variables.
 *
 * @preCondition     None
 */
void UDPInit(void);


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
                   UDP_PORT remotePort);


/**
 * Given socket is marked as available for future new communcations.
 *
 * This function does not affect previous active UDP socket designation.
 *
 * @preCondition    UDPOpen() is already called
 *
 * @param s         Socket that is to be closed.
 */
void UDPClose(UDP_SOCKET s);


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
BOOL UDPIsPutReady(UDP_SOCKET s);


/**
 * Returns the maximum size the UDP data is allowed to be. This value should never be execeed when
 * writting data to the UDP transmit buffer before calling UDPFlush(). 
 * For slip:
 *  = MAC_TX_BUFFER_SIZE - sizeof(IP_HEADER) - sizeof(UDP_HEADER)
 *  = MAC_TX_BUFFER_SIZE - 20 - 8 = MAC_TX_BUFFER_SIZE - 28
 * For RTL MAC
 *  = MAC_TX_BUFFER_SIZE - SIZEOF_MAC_HEADER - sizeof(IP_HEADER) - sizeof(UDP_HEADER)
 *  = MAC_TX_BUFFER_SIZE - 14 - 20 - 8 = MAC_TX_BUFFER_SIZE - 42
 */
#define SIZEOF_MAC_HEADER (14ul)
#if defined(STACK_USE_SLIP)
    #define UDPGetMaxDataLength() (MAC_TX_BUFFER_SIZE - sizeof(IP_HEADER) - sizeof(UDP_HEADER))
#else
    #define UDPGetMaxDataLength() (MAC_TX_BUFFER_SIZE - SIZEOF_MAC_HEADER - sizeof(IP_HEADER) - sizeof(UDP_HEADER))
#endif


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
BOOL UDPPut(BYTE v);


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
WORD UDPPutArray(BYTE *buffer, WORD count);


/**
 * This function transmit all data from an active UDP socket.
 *
 * @preCondition    UDPPut() is already called and desired UDP socket
 *                  is set as an active socket by calling
 *                  UDPIsPutReady().
 *
 * @return          All and any data associated with active UDP socket
 *                  buffer is marked as ready for transmission.
 *
 */
void UDPFlush(void);


/**
 * Check if given socket contains any data that is ready to be read, and that the given socket
 * is valid (not equal to INVALID_UDP_SOCKET for example). It also sets the given socket as
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
BOOL UDPIsGetReady(UDP_SOCKET s);

/**
 * UDP function that returns number of UDP bytes that are available
 *
 * @param s         A valid UDP socket that is already "Listen"ed on
 *                  or opened.
 *
 * @return          Number of bytes ready to be read.
 */
WORD UDPIsGetReady_(UDP_SOCKET s);


/**
 * Get a byte of data.
 *
 * Note: This function fetches data from the currently active UDP socket. The active UDP
 * socket is set by the UDPIsGetReady() call.
 *
 * @preCondition    UDPInit() is already called     AND
 *                  UDPIsGetReady(s) == TRUE
 *
 * @param[out] v    Buffer to receive UDP data byte
 *
 * @return          TRUE if a data byte was read <br>
 *                  FALSE if no data byte was read or available
 */
BOOL UDPGet(BYTE *v);


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
WORD UDPGetArray(BYTE *buffer, WORD count);


/**
 * This function discards an active UDP socket content.
 *
 * @preCondition     UDPInit() is already called    AND
 *                  UDPIsGetReady() == TRUE with desired UDP socket.
 */
void UDPDiscard(void);



/**
 * Performs UDP related tasks. Must continuesly be called.
 *
 * @preCondition    UDPInit() is already called AND <br>
 *                  UDP segment is ready in MAC buffer
 *
 * @param remoteNode    Remote node info
 * @param len           Total length of UDP semgent.
 * @param localIP       The IP Address of the currently received packet.
 *
 * @return          TRUE if this function has completed its task <br>
 *                  FALSE otherwise
 */
BOOL UDPProcess(NODE_INFO *remoteNode, IP_ADDR *localIP, WORD len);


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
NODE_INFO * UDPGetNodeInfo(void);


/**
 * This function makes the given transmit buffer active, and sets it's access pointer to be: <br>
 * - At the given offset after the UDP header <br>
 * - In the given TX Buffer <br>
 * So, if we pass 0 in as the offset, we will set the pointer to the first byte of after the
 * UDP header. All future read and writes to the TX Buffer will be to the set location.
 *
 * @param buffer    Buffer identifier
 * @param offset    Offset
 *
 */
#define UDPSetTxBuffer(buffer, offset) (UDPSocketInfo[activeUDPSocket].TxOffset = offset,  IPSetTxBuffer(buffer, offset+sizeof(UDP_HEADER)))


/**
 * Sets the receive buffer access point to given offset in UDP Data.
 * For example, if UDP data is a HTTP message, an offset of 0 will set
 * access to first byte of HTTP message (UDP data).
 * Layers that use UDP services (HTTP) should call this macro
 * to set the access pointer for the current buffer.
 *
 * @param a     Offset, An offset with respect to UDP Data
 *
 */
#define UDPSetRxBuffer(a) IPSetRxBuffer(a+sizeof(UDP_HEADER))

#endif
