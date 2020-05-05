/**
 * @brief           NetBIOS Name Service (NBNS) Server
 * @file            nbns.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    stacktsk.h, tcp.h
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  Microchip C30 v2.01 or higher
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_tcpip_user_ftp
 *
 * @section description Description
 **********************************
 * Responds to NBNS name requests to allow human name assignment to the board.
 * The user can define a custom NetBIOS name in his/her application. If no
 * custom name is defined, the default name will be "MXBOARD        ". The name
 * must be 16 characters long, with the 16th character a 0x00. To define a
 * custom name, "NETBIOS_NAME_GETCHAR(n)" must be defined in the "projdefs.h"
 * file. An example of defining a custom name is:
 * @code
 * //Default NetBIOS name, must be 16 char, and all upper case, 16th char = 0x00
 * ROM char NETBIOS_NAME_DEF[] =
 *     {'M','Y','B','O','A','D', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0x00};
 * #define NETBIOS_NAME_GETCHAR(n) (NETBIOS_NAME_DEF[n])
 * @endcode
 *
 * Another possibility is for the user application to store a custom name in the internal
 * EEPROM of the PIC. This way it can be changed at run time.
 *
 * <br>For a detailed description, see the @ref mod_tcpip_user_netbios section of this document - in 
 * [Modules] [TCP/IP Stack] [TCP/IP User Protocols].
 *
 *
 * @section netbios_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //-------------------- netbios  Configuration --------------------
 //*********************************************************************
 // Get the requested character of our NetBIOS name. If this is not defined in the
 // "projdefs.h" file, the default name "MXBOAD" is used
 #define NETBIOS_NAME_GETCHAR(n)     Put user code here ......
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
 * Howard Schlunder     06/01/23 Original
 ********************************************************************/

/////////////////////////////////////////////////
// Netbios - documentation module
/**
@defgroup mod_tcpip_user_netbios NetBIOS
@ingroup mod_tcpip_user



@section mod_tcpip_user_netbios_desc Description
NetBIOS is an acronym for Network Basic Input/Output System. The NetBIOS API allows
applications on separate computers to communicate over a local area network. It provides
services related to the session layer of the OSI model.
@if DEMO_WEB_SERVER
The default NetBIOS name for the @appname is "MXBOARD". This name can
be used in stead of the board's IP address in many programs. For example, to access the
web based interface of the board, type "http://mxboard" in the address field of a web
browser.
The following description of NetBIOS was taken from
<a target="_blank" href="http://en.wikipedia.org/wiki/Netbios">Wikipedia, the free encyclopedia</a>. The
@appname only implements a small part of the protocol!
@endif



@section mod_tcpip_user_netbios_services Services
NetBIOS provides three distinct services:
- Name service for name registration and resolution
- Session service for connection-oriented communication
- Datagram distribution service for connectionless communication

Note: SMB, an upper layer, is a service that runs on top of the Session Service and the
Datagram service, and is not to be confused as a necessary and integral part of NetBIOS
itself. It can now run atop TCP with a small adaptation layer that adds a packet length
to each SMB message; this is necessary because TCP only provides a byte-stream service
with no notion of packet boundaries.


@subsection mod_tcpip_user_netbios_services_name Name Service
In order to start Sessions or distribute Datagrams, an application must register its
NetBIOS name using the Name service. NetBIOS names are 16 bytes in length and vary
based on the particular implementation. Frequently, the 16th byte is used to designate
a "type" similar to the use of ports in TCP/IP. In NBT, the name service operates on
UDP port 137 (TCP port 137 can also be used, but it is rarely if ever used).

The name service primitives offered by NetBIOS are:
- <b>Add Name</b> - registers a NetBIOS name
- <b>Add Group Name</b> - registers a NetBIOS "group" name
- <b>Delete Name</b> - un-registers a NetBIOS name or group name
- <b>Find Name</b> - looks up a NetBIOS name on the network


@subsection mod_tcpip_user_netbios_services_session Session Service
Session mode lets two computers establish a connection for a "conversation,"
allows larger messages to be handled, and provides error detection and recovery.
In NBT, the session service runs on TCP port 139.

The session service primitives offered by NetBIOS are:
- <b>Call</b> - opens a session to a remote NetBIOS name
- <b>Listen</b> - listen for attempts to open a session to a NetBIOS name
- <b>Hang Up</b> - close a session
- <b>Send</b> - sends a packet to the computer on the other end of a session
- <b>Send No Ack</b> - like Send, but doesn't require an acknowledgment
- <b>Receive</b> - wait for a packet to arrive from a Send on the other end of a session

In the original protocol used to implement NetBIOS services on PC-Network, to establish
a session, the computer establishing the session sends an Open request which is responded
to by an Open acknowledgment. The computer that started the session will then send a Session
Request packet which will prompt either a Session Accept or Session Reject packet. Data is
transmitted during an established session by data packets which are responded to with either
acknowledgment packets (ACK) or negative acknowledgment packets (NACK). Since NetBIOS is
handling the error recovery, NACK packets will prompt retransmission of the data packet.
Sessions are closed by the non-initiating computer by sending a close request. The computer
that started the session will reply with a close response which prompts the final session
closed packet.


@subsection mod_tcpip_user_netbios_services_dist Datagram Distribution Service
Datagram mode is "connectionless". Since each message is sent independently, they must be
smaller; the application becomes responsible for error detection and recovery. In NBT, the
datagram service runs on UDP port 138.

The datagram service primitives offered by NetBIOS are:
- <b>Send Datagram</b> - send a datagram to a remote NetBIOS name
- <b>Send Broadcast Datagram</b> - send a datagram to all NetBIOS names on the network
- <b>Receive Datagram</b> - wait for a packet to arrive from a Send Datagram operation
- <b>Receive Broadcast Datagram</b> - wait for a packet to arrive from a Send Broadcast Datagram operation



@section mod_tcpip_user_netbios_conf Configuration
For details on configuring this module, see @ref netbios_conf section of nbns.h file.
*/
#ifndef NBNS_H
#define NBNS_H

//This is used by other stack elements
#ifndef THIS_IS_NET_BIOS_NAME_SERVICE
    #ifdef NETBIOS_NAME_GETCHAR_DEFAULT
    extern ROM char* NETBIOS_NAME_DEF;
    #endif
#endif

/**
 * Sends responses to NetBIOS name requests
 */
void NBNSTask(void);


#endif
