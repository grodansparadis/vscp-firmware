/**
 * @brief           TCP Helper functions
 * @file            tcputils.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    stacktsk.h
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_tcpip_utils
 *
 *
 * @section description Description
 **********************************
 * This module contains TCP helper functions.
 * <br>For a detailed description, see the @ref mod_tcpip_utils section of this document - in 
 * [Modules] [TCP/IP Stack] [Helper Functions].
 *
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
 * 2006-10-10, David Hosken (DH):
 *    - Initial version
 *********************************************************************/


#ifndef TCPUTILS_H
#define TCPUTILS_H

#include "net\ip.h"
#include "net\tcp.h"
#include "net\tick.h"
#include "net\fsee.h"

typedef struct _TCP_FILESEND_INFO
{
    NODE_INFO   serverNode;
    WORD        port;           //Port to send file to
    TCP_SOCKET  tcpSocket;
    FILE        f;              //File handle
    TICK16      tmr;            //General purpose wait timer
    BYTE        tries;
    BYTE        sm;             //State machine
} TCP_FILESEND_INFO;

#define TCPU_NOT_FINISHED           (0xfful)
#define TCPU_ERR_IP_RESOLVE         (1ul)   /* Could not resolve IP address */
#define TCPU_ERR_TCP_OPEN           (2ul)   /* TCP Open error */
#define TCPU_ERR_TCP_TIMEOUT        (3ul)   /* TCP Timeout */
#define TCPU_ERR_FILE_RD            (4ul)   /* File read error */
#define TCPU_ERR_FILE_NOT_FOUND     (5ul)   /* File not found */
#define TCPU_ERR_FSYS_NOT_AVAIL     (6ul)   /* File system not available */

/**
 * Send the given file to the given port and server. To use this function, a TCP_FILESEND_INFO
 * structure must be created and initialized with desired values. Then sendFileToTcpServer()
 * is called with the created TCP_FILESEND_INFO structure, and the filename of the file to send.
 *
 * This function has to be repetitively called as long as it returns 0xff.
 *
 * @param filename      Name of file to send to the server
 * @param tcb           Info used by this function
 *
 * @return              Returns 0 if success, else error: <br>
 *                      - 0:   File successfully sent <br>
 *                      - 255: File partially sent, this function has to be called again
 *                             to complete this operation!<br>
 *                      - 1:   Could not resolve IP address <br>
 *                      - 2:   Could not open TCP socket <br>
 *                      - 3:   TCPConnect timed out <br>
 *                      - 4:   File read error
 *                      - 5:   File not found
 *                      - 6:   File system not available
 */
BYTE sendFileToTcpServer(BYTE* filename, TCP_FILESEND_INFO* tcb);

#endif

