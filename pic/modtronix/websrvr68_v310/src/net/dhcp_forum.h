/**
 * @brief           DHCP client Module for Modtronix TCP/IP Stack
 * @file            dhcp.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    stacktsk.h, udp.h
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_tcpip_user_dhcp
 *
 * @section description Description
 **********************************
 * This module implements a DHCP client.  
 *
 * <br>For a detailed description, see the @ref mod_tcpip_user_dhcp
 * section of this document - in [Modules] [TCP/IP Stack] [TCP/IP User
 * Protocols].
 *
 * @section dhcp_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should
 * be placed in the projdefs.h (or similar) file.  For details, see
 * @ref mod_conf_projdefs "Project Configuration".  To configure the
 * module, the required defines should be uncommended, and the rest
 * commented out.  @code
 * //*********************************************************************
 * //-------------------- DHCP Configuration --------------------
 * //*********************************************************************
 * //Defines DHCP ports #define DHCP_CLIENT_PORT (68) #define
 * DHCP_SERVER_PORT (67)

 // The stack uses the macro STACK_IS_DHCP_ENABLED to determine if DHCP
 // is enabled or not.  The user can for example assign this macro to
 // check if a flag is set. If not defined by the user, it will be set
 // to true.  #define STACK_IS_DHCP_ENABLED (netflags & 0x01)

  @endcode
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on
 * v2.20.04.01 of the Microchip TCP/IP stack. The original code base
 * is owned by Microchip Technology Incorporated (the Company), and is
 * protected under applicable copyright laws. The modifications are
 * owned by Modtronix Engineering, and is protected under applicable
 * copyright laws. The software supplied herewith is intended and
 * supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller based products
 * manufactured by Modtronix Engineering. The code may be modified and
 * can be used free of charge for non commercial and commercial
 * applications. All rights are reserved. Any use in violation of the
 * foregoing restrictions may subject the user to criminal sanctions
 * under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 ********************************************************************
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     3/21/01  Original        (Rev 1.0)
 *
 * 2008-05-08, Andrew Eckhardt: Implemented changes from Microchip
 *              TCP/IP Stack Version v4.18 dated 28 November 2007
 ********************************************************************/


/////////////////////////////////////////////////
// DHCP - documentation module
/**
@defgroup mod_tcpip_user_dhcp DHCP
@ingroup mod_tcpip_user

The DHCP layer of the Modtronix TCP/IP stack is implemented by the
file "dhcp.c". The header file "dhcp.h" defines the services provided
by the layer. DHCP is an active layer that broadcasts DHCP requests
and automatically receives and decodes DHCP responses. Its main
features include: 
@li Configuration of the IP and gateway addresses and subnet mask
@li Automatic DHCP lease time and lease renewal management
@li Fully automatic operation without user application intervention

The DHCP module is implemented as a cooperative task, performing
automatic operations without the knowledge of the main
application. The actual DHCP integration and control is done by the
Stack Manager; it handles all required operations as part of its
standard task, using the DHCP APIs to control the module∆s
behavior. The user does not need to know about DHCP in order to use
it.

A user application may also choose to call some of the APIs to
directly control DHCP operation, such as whether DHCP is configured or
not, and whether to permanently stop DHCP. Normally, the user's
application should not need to directly interact with DHCP at all. To
use the DHCP module, the user project files must be modified as
follows: @li Uncomment STACK_USE_DHCP in the header file "StackTsk.h".
@li Include the "dhcp.c" and "udp.c" files in the project.  @li
Increase MAX_UDP_SOCKETS by one (at least one UDP socket must be
available for DCHP; adjust the numbers of sockets based on UDP and
DCHP needs).

When DHCP is implemented, the user application must not attempt
network communications until DHCP is configured properly. Normally, if
a user application contains one or more client applications that
require communications on power-up or RESET, the application must
check that DHCP is configured before transmitting any data using the
lower layer modules. This can be done with the function DHCPIsBound()

The official DHCP specification (RFC1541) requires the DHCP client to
renew its IP configuration before its lease time expires. To track
lease time, the user application must make sure that TickUpdate() is
called as required, and that reasonable time accuracy is maintained
(refer to the source code file "websrvr.c" for a working example). The
time resolution required is 15 minutes, plus or minus, which means
that TickUpdate() may be called at a very low priority.

For the DHCP module to automatically configure the addresses and
subnet mask, there must be at least one DHCP server on the network. It
is the user∆s responsibility to implement some method for "publishing"
the configurations back to potential users. Options range from
manually reading the information from a display on each node, to
storing the information in a central server. DHCP updates the values
of MY_IP_BYTE?, MY_GATE_BYTE? and MY_MASK_BYTE? as a result of
automatic configuration.

@section mod_tcpip_user_dhcp_conf Configuration
For details on configuring this module, see @ref dhcp_conf section of dhcp.h file.

*/


#ifndef DHCP_H
#define DHCP_H
#define BOOT_REQUEST                    (1u)
#define BOOT_REPLY                      (2u)

#define BOOT_HW_TYPE                    (1u)
#define BOOT_LEN_OF_HW_TYPE             (6u)

#define DHCP_MESSAGE_TYPE               (53u)
#define DHCP_MESSAGE_TYPE_LEN           (1u)

#define DHCP_UNKNOWN_MESSAGE            (0u)

#define DHCP_DISCOVER_MESSAGE           (1u)
#define DHCP_OFFER_MESSAGE              (2u)
#define DHCP_REQUEST_MESSAGE            (3u)
#define DHCP_DECLINE_MESSAGE            (4u)
#define DHCP_ACK_MESSAGE                (5u)
#define DHCP_NAK_MESSAGE                (6u)
#define DHCP_RELEASE_MESSAGE            (7u)

#define DHCP_SERVER_IDENTIFIER          (54u)
#define DHCP_SERVER_IDENTIFIER_LEN      (4u)

#define DHCP_PARAM_REQUEST_LIST         (55u)
#define DHCP_PARAM_REQUEST_LIST_LEN     (4u)
#define DHCP_PARAM_REQUEST_IP_ADDRESS       (50u)
#define DHCP_PARAM_REQUEST_IP_ADDRESS_LEN   (4u)
#define DHCP_SUBNET_MASK                (1u)
#define DHCP_ROUTER                     (3u)
#define DHCP_DNS                        (6u)
#define DHCP_HOST_NAME                    (12u)
#define DHCP_IP_LEASE_TIME              (51u)
#define DHCP_END_OPTION                 (255u)


/////////////////////////////////////////////////
//Default defines - they can be overridden in projdefs.h file

#if !defined(STACK_IS_DHCP_ENABLED)
#define STACK_IS_DHCP_ENABLED (1)
#endif

//Defines DHCP ports
#if !defined(DHCP_CLIENT_PORT)
#define DHCP_CLIENT_PORT    (68)
#endif

#if !defined(DHCP_SERVER_PORT)
#define DHCP_SERVER_PORT    (67)
#endif


typedef enum _SM_DHCP
{
   SM_DHCP_GET_SOCKET,
   SM_DHCP_SEND_DISCOVERY,
   SM_DHCP_GET_OFFER,
   SM_DHCP_SEND_REQUEST,
   SM_DHCP_GET_REQUEST_ACK,
   SM_DHCP_BOUND,
   SM_DHCP_SEND_RENEW,
   SM_DHCP_GET_RENEW_ACK,
   SM_DHCP_SEND_RENEW2,
   SM_DHCP_GET_RENEW_ACK2,
   SM_DHCP_SEND_RENEW3,
   SM_DHCP_GET_RENEW_ACK3,
   SM_DHCP_DISABLED
} SM_DHCP;

#if !defined(THIS_IS_DHCP)
    extern SM_DHCP smDHCPState;
#endif

typedef union _DHCP_CLIENT_FLAGS
{
    struct
    {
        unsigned char bIsBound : 1;
        unsigned char bOfferReceived : 1;
		unsigned char bDHCPServerDetected : 1;
    } bits;
    BYTE Val;
} DHCP_CLIENT_FLAGS;

#if !defined(__DHCP_C)
    extern DHCP_CLIENT_FLAGS DHCPFlags;
    extern SM_DHCP smDHCPState;
    extern BYTE DHCPBindCount;
#endif

void DHCPDisable(void);

/* Fetches pending UDP packet from MAC receive buffer and dispatches it appropriate UDP socket.
 * If not UDP socket is matched, UDP packet is silently discarded.
 * Note:    Caller must make sure that MAC receive buffer
 *          access pointer is set to begining of UDP packet.
 *          Required steps before calling this function is:
 *          
 *          If ( MACIsRxReady() )
 *          {
 *              MACGetHeader()
 *              If MACFrameType == IP
 *                  IPGetHeader()
 *                  if ( IPFrameType == IP_PROT_UDP )
 *                      Call DHCPTask()
 *              ...
 *          }
 *
 * @pre:   DHCPInit() is already called AND IPGetHeader() is called with
 *                  IPFrameType == IP_PROT_UDP
 */
void DHCPTask(void);


/**
 * Checks is DHCP is bound
 * @return  TRUE if DHCP is bound to given configuration
 *          FALSE if DHCP has yet to be bound.
 */
#define DHCPIsBound()       (DHCPFlags.bits.bIsBound)

typedef union _DHCP_STATE
{
    struct
    {
        unsigned char bIsBound : 1;
        unsigned char bOfferReceived : 1;
        unsigned char bStaticIP : 1;    /* Set if no DHCP server was found, and a static IP was assigned */
    } bits;
    BYTE Val;
} DHCP_STATE;


/**
 * Closes any previously opened DHCP socket and resets DHCP state machine so
 * that on next call to DHCPTask will result in new DHCP request.
 */
void DHCPReset(void);


#endif
