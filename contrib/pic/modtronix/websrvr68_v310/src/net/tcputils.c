/*********************************************************************
 * tcphelpers.c -   TCP Helper functions
 * Dependencies:    
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.35PL3 or higher
 * Company:         Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description
 *
 * TCP Helper functions.
 *
 *********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ********************************************************************/

#include <string.h>

#include "projdefs.h"
#include "net\checkcfg.h"
#include "net\tcputils.h"
#include "net\arp.h"
#include "net\arptsk.h"
#include "net\fsee.h"
#include "net\helpers.h"
#include "net\stacktsk.h"
#include "net\tick.h"
#include "net\tcp.h"
#include "debug.h"


/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xE9, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xE9, msgCode, msgStr)

/**
 * Send the given file to the given port and server.
 *
 * @param filename      Name of file to send to the server
 * @param tcb           Info used by this function
 *
 * @return              Returns:
 *                      - 0:   File successfully sent <br>
 *                      - 255: File partially sent, this function has to be called again
 *                             to complete this operation!<br>
 *                      - 1:   Could not resolve IP address <br>
 *                      - 2:   Could not open TCP socket <br>
 *                      - 3:   TCPConnect timed out <br>
 *                      - 4:   File read error
 *                      - 5:   File not found
 *                      - 6:   File system not available
 *                     
 */
BYTE sendFileToTcpServer(BYTE* filename, TCP_FILESEND_INFO* tcb) {
    char    c;
    //Create a TCP socket for receiving and sending data
    
    //Timeouts in seconds
    #define TIMEOUT_ARP         10ul
    #define TIMEOUT_TCPCONNECT  10ul

    //TCP State machine
    static enum _smTcp {
        SM_TCP_SEND_ARP = 0,
        SM_TCP_WAIT_RESOLVE,
        SM_TCP_WAIT_CONNECTED,
        SM_TCP_WAIT_PUT_READY,
        SM_TCP_PUT_FILE,
        SM_TCP_DISCONNECT
    };

    /////////////////////////////////////////////////
    //Send the file to the given TCP socket

    switch (tcb->sm) {
    case SM_TCP_SEND_ARP:
        if (ARPIsTxReady()) {
            tcb->tmr = TickGet16bit();    //Remember when we sent last request

            //Send ARP request for given IP address
            ARPResolve(&tcb->serverNode.IPAddr);

            tcb->sm = SM_TCP_WAIT_RESOLVE;

            #if (DEBUG_TCPUTILS >= LOG_INFO)
            debugPutMsg(11);     //@mxd:11:Sent ARP Request
            #endif
        }
        break;
    case SM_TCP_WAIT_RESOLVE:
        //The IP address has been resolved, we now have the MAC address of the given node
        if (ARPIsResolved(&tcb->serverNode.IPAddr, &tcb->serverNode.MACAddr)) {
            #if (DEBUG_TCPUTILS >= LOG_INFO)
            debugPutMsg(12);     //@mxd:12:ARP Resolved
            #endif

            //After calling TCPConnect(), we have to wait for a connection
            //to be established. Stack will continuously try to connect!
            tcb->sm = SM_TCP_WAIT_CONNECTED;

            //We can now connect, seeing that we have the remote server node
            //info (MAC and IP address)
            tcb->tcpSocket = TCPConnect(&tcb->serverNode, tcb->port);
            tcb->tmr = TickGet16bit();    //Remember when we requested to connect

            //Error
            if (tcb->tcpSocket == INVALID_SOCKET) {
                return 1;   //Could not open TCP socket
            }
        }
        //If not resolved after 1 seconds, send next request
        else {
            if (TickGetDiff16bit(tcb->tmr) >= ((TICK16)TICKS_PER_SECOND) ) {
                if (--tcb->tries == 0) {
                    return 1;   //Return error, could not resolve IP address
                }
                
                tcb->sm = SM_TCP_SEND_ARP;

                #if (DEBUG_TCPUTILS >= LOG_DEBUG)
                debugPutMsg(10);     //@mxd:10:ARP Not resolved, will try again!
                #endif
            }
        }
        break;
    case SM_TCP_WAIT_CONNECTED:
        //Connection has been established
        if (TCPIsConnected(tcb->tcpSocket)) {
        
            /////////////////////////////////////////////////
            //Open file
            tcb->f = fileOpen(filename, 0);   //Open the given file

            //File not found
            if (tcb->f == FILE_INVALID )
            {
                #if (DEBUG_TCPUTILS >= LOG_ERROR)
                debugPutMsg(1);     //@mxd:1:File Not Found!
                #endif
    
                return 5;   //Return error, file not found
            }
            else if (tcb->f == FSYS_NOT_AVAILABLE)
            {
                //File System not available

                #if (DEBUG_TCPUTILS >= LOG_ERROR)
                debugPutMsg(2);     //@mxd:2:File system not available!
                #endif
    
                return 6;   //Return error, file system not available
            }
        
            tcb->sm = SM_TCP_PUT_FILE;
        }
        else {
            //Has TCPConnect timeout expired?
            //The TCP stack will continuously try and connect. It is up to the user to
            //terminate the connection attempts if no connection is made after a
            //certian time. The TCPDisconnect() function can be used to stop the
            //stack from trying to connect.
            if (TickGetDiff16bit(tcb->tmr) >= ((TICK16)TICKS_PER_SECOND * (TICK16)TIMEOUT_TCPCONNECT) ) {
                #if (DEBUG_TCPUTILS >= LOG_ERROR)
                debugPutMsg(21);     //@mxd:21:TCPConnect timeout!
                #endif

                TCPDisconnect(tcb->tcpSocket);
                
                return 3;   //Return error, TCPConnect timed out
            }
        }
        break;
    case SM_TCP_WAIT_PUT_READY:
        //Wait for TCP put to be ready. When sending long files, the TCPPut() function will
        //automatically flush when it's transmit buffer is full. This will normally be what
        //happened when we wait here. We are basically waiting for the transmit buffer to
        //be sent via the MAC, and for a transmit buffer to become available.
        if (TCPIsPutReady(tcb->tcpSocket)) {
            tcb->sm = SM_TCP_PUT_FILE;
        }
        break;
    case SM_TCP_PUT_FILE:
        //Checks if there is a transmit buffer ready for accepting data, and that the given socket
        //is valid (not equal to INVALID_SOCKET for example)
        while (TCPIsPutReady(tcb->tcpSocket)) {
            //Check if file has reached end
            if ( fileIsEOF(tcb->f) ) {
                fileClose(tcb->f);

                //Send contents of transmit buffer, and free buffer
                TCPFlush(tcb->tcpSocket);
                
                tcb->sm = SM_TCP_DISCONNECT;
                return 0xff;    //Return "to be continued"
            }

            //Read byte from file
            c = fileGetByte(tcb->f);

            //Check if error occured
            if ( fileHasError(tcb->f) ) {
                fileClose(tcb->f);

                #if (DEBUG_TCPUTILS >= LOG_ERROR)
                debugPutMsg(3);     //@mxd:3:File read error!
                debugPutRomString((ROM char*)"");
                #endif

                //Forcefully disconnect
                TCPDisconnect(tcb->tcpSocket);
                
                return 4;   //Return error, File read error
            }
            else {
                //Write the given byte to the given socket's transmit buffer.
                TCPPut(tcb->tcpSocket, c);
            }
        }
        
        //If program gets here, the TCPPut() function must have called TCPFlush() because
        //it's transmit buffer filled up. Release file, and wait until TCP is ready again.
        
        // Release file. The file still remains open, and we can continue later on.
        fileRelease(tcb->f);
        
        tcb->sm = SM_TCP_WAIT_PUT_READY;
        
        break;
    case SM_TCP_DISCONNECT:
        if ( TCPIsConnected(tcb->tcpSocket) )
        {
            //Check if the given socket can be disconnected yet
            if ( TCPIsPutReady(tcb->tcpSocket) )
            {
                TCPDisconnect(tcb->tcpSocket);
                return 0;   //Return OK
            }
        }
        //We are disconnected
        else {
            return 0;   //Return OK
        }
        break;
    }
    
    return 0xff;    //Return "to be continued"
}
