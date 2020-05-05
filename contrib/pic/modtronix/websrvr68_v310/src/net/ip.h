/**
 * @brief           IP Module for Modtronix TCP/IP Stack
 * @file            ip.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    stacktsk.h, mac.h
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_tcpip_base_ip
 *
 *
 * @section description Description
 **********************************
 * This module contains the IP code.
 * <br>For a detailed description, see the @ref mod_tcpip_base_ip section of this document - in 
 * [Modules] [TCP/IP Stack] [TCP/IP Base Protocols].
 *
 *
 * @subsection ip_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //-------------------- IP Configuration --------------------
 //*********************************************************************
 //Time-To-Live in Seconds
 #define MY_IP_TTL   (100ul)
 //When defined, the code will be compiled for optimal speed. If not defined, code is defined
 //for smallest size.
 #define IP_SPEED_OPTIMIZE
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
 ********************************************************************
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     4/27/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 ********************************************************************/


/////////////////////////////////////////////////
// IP - documentation module
/**
@defgroup mod_tcpip_base_ip IP
@ingroup mod_tcpip_base
The IP layer of the Modtronix TCP/IP Stack is implemented by the file "ip.c". The header file "ip.h"
defines the services provided by the layer.
In this architecture, the IP layer is passive; it does not respond to IP data packets. Instead, higher
level layers use IP primitives and fetch the IP packet, interpret it and take appropriate action.

The IP specification requires that the local host generate a unique packet identifier for each packet
transmitted by it. The identifier allows remote host to identify duplicate packets and discard them.
The Modtronix TCP/IP Stack'ss IP layer maintains a private 16-bit variable to track packet identifiers.

@section mod_tcpip_user_ip_conf Configuration
For details on configuring this module, see @ref ip_conf section of ip.h file.
*/

#ifndef IP_H
#define IP_H

#include "net\mac.h"

/////////////////////////////////////////////////
//Default defines - they can be overridden in projdefs.h file
#if !defined(MY_IP_TTL)
#define MY_IP_TTL   (100ul)   /* Time-To-Live in Seconds */
#endif


#define IP_PROT_ICMP    (1ul)
#define IP_PROT_TCP     (6ul)
#define IP_PROT_UDP     (17ul)

/**
 * Structure to store an IP address. For example, if IP address is "192.168.1.0", then:
 * - IP_ADDR.v[0] = 192
 * - IP_ADDR.v[1] = 168
 * - IP_ADDR.v[2] = 1
 * - IP_ADDR.v[3] = 10
 */
typedef union _IP_ADDR
{
    BYTE        v[4];
    DWORD       Val;
} IP_ADDR;


/** Structure to store a nodes info */
typedef struct _NODE_INFO
{
    MAC_ADDR    MACAddr;    //6 bytes
    IP_ADDR     IPAddr;     //4 bytes
} NODE_INFO;    //14 bytes long


/** IP packet header definition */
typedef struct _IP_HEADER
{
    BYTE        VersionIHL;
    BYTE        TypeOfService;
    WORD_VAL    TotalLength;
    /** This field contains a 16 bit value that is common to each of the fragments beloning to a particular
    message. For datagrams originally sent unfragmented, it is still filled out, so it can be used if the
    datagram must be fragmented by a router during delivery. This field is used by the recipient to reassemble
    without accidentally mixing fragments from different messages. This is needed because fragments may arrive
    from multiple messages mixed together, since IP datagrams can be received out of order from any device. */
    WORD_VAL    Identification;
    /**
     * x--- ---- ---- ---- = RF flag: Reserved
     * -x-- ---- ---- ---- = DF flag: Don't fragment. When set to 1, indicates that the datagram should not be
     *                       fragmented. 
     * --x- ---- ---- ---- = MF flag: More Fragments. When set to 0, indicates that this is the last fragment in
     *                       a message. When set to 1, indicates that more fragments for this message are to
     *                       come. If no fragmentation is used, then there will always only be one fragment, and
     *                       this flag will always be 0.
     * ---x xxxx xxxx xxxx = Fragment offset: When fragmentation of a message occurs, this field specifies the
     *                       the offset, or position, in the overall message where the data in this fragment
     *                       goes. It is specified in units of 8 bytes (64 bits). The first fragment has an
     *                       offset of 0.
     */
    WORD_VAL    FragmentInfo;
    BYTE        TimeToLive;
    BYTE        Protocol;
    WORD_VAL    HeaderChecksum;
    IP_ADDR     SourceAddress;
    IP_ADDR     DestAddress;

} IP_HEADER;


/**
 * Check if ready for next transmission.
 *
 * @param HighPriority  High priority messages are those that don't need to be acknowledged
 *                      before being discarded (TCP control packets, all ICMP packets, all UDP packets, etc.)
 *
 * @return      TRUE if transmit buffer is empty <br>
 *              FALSE if transmit buffer is not empty
 *
 */
#define IPIsTxReady(HighPriority)       MACIsTxReady(HighPriority)



/**
 * This function makes the given transmit buffer active, and sets it's access pointer to be: <br>
 * - At the given offset after the IP header <br>
 * - In the given TX Buffer <br>
 * So, if we pass 0 in as the offset, we will set the pointer to the first byte of after the
 * IP header. All future read and writes to the TX Buffer will be to the set location.
 *
 * @param buffer    Buffer identifier
 * @param offset    Offset
 *
 */
#define IPSetTxBuffer(buffer, offset) MACSetTxBuffer(buffer, offset+sizeof(IP_HEADER))



/**
 * Write the Ethernet Header (MAC Header) and IP Header to the current TX buffer.
 * The last parameter (len) is the length of the data to follow.
 * This function will do the following: <ul>
 *  <li> Reset the NIC Remote DMA write pointer to the first byte of the current TX Buffer </li>
 *  <li> Write the given header </li>
 *  <li> Set the NIC Remote DMA byte count to the given len. This configures the Remote DMA
 *  to receive the given number of bytes. Only one IP message can be transmitted at any time.
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
WORD    IPPutHeader(NODE_INFO *remote,
                    BYTE protocol,
                    WORD len);


/**
 * Data is copied to IP data area.
 *
 * @preCondition    IPIsTxReady() == TRUE
 *
 * @param a         Data buffer
 * @param b         Buffer length
 *
 */
#define IPPutArray(a, b)    MACPutArray(a, b)




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
                 WORD *len);


/**
 *
 * @preCondition    MACGetHeader() == TRUE
 *
 * @return          Current packet is discarded and buffer is freed up
 *
 */
#define IPDiscard()         MACDiscard()


/**
 * Data is copied from IP data to given buffer
 *
 * @preCondition    MACGetHeader() == TRUE
 *
 * @param a         Data buffer
 * @param b         Buffer length
 *
 */
#define IPGetArray(a, b)    MACGetArray(a, b)


/**
 * Sets the receive buffer access pointer to given offset in IP Data.
 * For example, if IP data is a TCP packet, an offset of 0 will set
 * access to first byte of TCP header.
 * Layers that use IP services (TCP, UDP...) should call this macro
 * to set the access pointer for the current buffer.
 *
 * @param offset   Offset, An offset with respect to IP Data
 *
 */
void IPSetRxBuffer(WORD offset);

#endif
