 /**
 * @brief           Serial Buses
 * @file            buses.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 *
 *********************************************************************/

 /*********************************************************************
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
 **********************************************************************
 * File History
 *
 * 2006-12-08, David Hosken (DH):
 *    - Initial version
 *********************************************************************/

#define THIS_IS_BUSES_MODULE

#include "projdefs.h"
#include "buses.h"
#include "debug.h"
#include "appcfg.h"


/////////////////////////////////////////////////
//Debug defines
//#define debugPutMsg(msgCode) debugPut2Bytes(0xDA, msgCode)
//#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xDA, msgCode, msgStr)


/**
 * Initializes "UDP Command Port" and "UDP Command Responce Port". These
 * ports are used for sending and receiving and commands via the UDP port.
 *
 */
void busInit(void) {
}


/**
 * Must be called every couple of ms
 *
 */
void busProcess(void) {
    /*
    //Is there any data waiting for us on this UDP port?
    if (UDPIsGetReady(udpSocket1)) {

        //Read bytes
        while (UDPGet(&c)) {

        }

        //Discard the socket buffer.
        UDPDiscard();
    }
    */
}

