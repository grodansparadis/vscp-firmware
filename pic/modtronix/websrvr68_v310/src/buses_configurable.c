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
#define debugPutMsg(msgCode) debugPut2Bytes(0xD1, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xD1, msgCode, msgStr)


#if defined(MCHP_C18)
    #pragma udata BUS_BUFFER_SEG
    BYTE busBuf0[BUSBUF0_SIZE];
    BYTE busBuf1[BUSBUF1_SIZE];
    BYTE busBuf2[BUSBUF2_SIZE];
    BYTE busBuf3[BUSBUF3_SIZE];
    BYTE busBuf4[BUSBUF4_SIZE];
    BYTE busBuf5[BUSBUF5_SIZE];
    BYTE busBuf6[BUSBUF6_SIZE];
    BYTE busBuf7[BUSBUF7_SIZE];
    #pragma udata   //Return to default section
#endif

#if defined(HITECH_C18)
    BYTE busBuf0[BUSBUF0_SIZE];
    BYTE busBuf1[BUSBUF1_SIZE];
    BYTE busBuf2[BUSBUF2_SIZE];
    BYTE busBuf3[BUSBUF3_SIZE];
    BYTE busBuf4[BUSBUF4_SIZE];
    BYTE busBuf5[BUSBUF5_SIZE];
    BYTE busBuf6[BUSBUF6_SIZE];
    BYTE busBuf7[BUSBUF7_SIZE];
#endif

//Bus info structure
BUS_INFO busInfo;

//Bus configuration structure
BUS_CONFIG busConfig;

//Pointer to current active buffer
static BYTE* activeBuf;

//Current put index to current active buffer
static BYTE activePut;

//Current get index to current active buffer
static BYTE activeGet;


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
    BYTE i;

#if (BUS_BUFFERS > 0)
    busInfo.buf[0] = busBuf0;
#endif
#if (BUS_BUFFERS > 1)
    busInfo.buf[1] = busBuf1;
#endif
#if (BUS_BUFFERS > 2)
    busInfo.buf[2] = busBuf2;
#endif
#if (BUS_BUFFERS > 3)
    busInfo.buf[3] = busBuf3;
#endif
#if (BUS_BUFFERS > 4)
    busInfo.buf[4] = busBuf4;
#endif
#if (BUS_BUFFERS > 5)
    busInfo.buf[5] = busBuf5;
#endif
#if (BUS_BUFFERS > 6)
    busInfo.buf[6] = busBuf6;
#endif
#if (BUS_BUFFERS > 7)
    busInfo.buf[7] = busBuf7;
#endif
#if (BUS_BUFFERS > 8)
    #error "add buffer initialization code to busInit() for additional buffers"
#endif

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


/**
 * Returns the number of bytes that can be written to the given buffer.
 *
 * @param bufID The buffer ID, is a number from 0 to n identifying the buffer
 *
 * @return The number of bytes that can be written to the given buffer
 */
BYTE busIsPutReady(BYTE bufID)
{
    if (bufID >= BUS_BUFFERS) {
        #if (DEBUG_BUS >= LOG_ERROR)
        debugPutMsg(2);     //@mxd:2:busIsPutReady called with invalid buffer ID: %d
        debugPutByte(bufID);
        #endif

        return 0;
    }

    // TEST TEST
    return 8;
}


/**
 * Adds the given byte to the given buffer. If any bus is assigned to this buffer, it will transmit
 * this data.
 *
 * @param bufID The buffer ID, is a number from 0 to n identifying the buffer
 * @param b     The byte to add to the given buffer
 */
void busPutByte(BYTE bufID, BYTE b)
{


}


/**
 * Adds the given byte array to the given buffer. If any bus is assigned to this buffer, it will transmit
 * this data.
 *
 * @param bufID The buffer ID, is a number from 0 to n identifying the buffer
 * @param buf   The byte array to add to the given buffer
 * @param len   The length of the buf array
 */
void busPutArray(BYTE bufID, BYTE* buf, BYTE len)
{

}

    // TEST TEST
    static BYTE put;
    static BYTE get;


/**
 * Returns the number of bytes that can be read from the given buffer.
 *
 * @param bufID The buffer ID, is a number from 0 to n identifying the buffer
 *
 * @return The number of bytes that can be read from the given buffer
 */
BYTE busIsGetReady(BYTE bufID)
{
    
    if (bufID >= BUS_BUFFERS) {
        #if (DEBUG_BUS >= LOG_ERROR)
        debugPutMsg(1);     //@mxd:1:busIsGetReady called with invalid bufID: %d
        debugPutByte(bufID);
        #endif

        return 0;
    }
    
    //Get pointer to buffer
    activeBuf = busInfo.buf[bufID];
    
    /*
    debugPutGenMsg(2);     //@mxd:2:%s
    debugPutRomStringXNull("busGetReady = ");
    debugPutByte(5);
    debugPutByte(0);    //Null terminate string
    */

    #if (DEBUG_BUS >= LOG_DEBUG)
    debugPutMsg(3);     //@mxd:3:busIsGetReady returned %d
    debugPutByte(bufID);
    #endif

    //return ((busInfo.put[bufID] - busInfo.get[bufID]) & (getBufSize(bufID)-1));
    //return (busInfo.put[bufID] - busInfo.get[bufID]);
    return (busInfo.put[1] - busInfo.get[1]);
    //return (put - get);
}


/**
 * Gets a single byte from the given buffer. If any bus is assigned to this buffer, it will transmit
 * this data.
 *
 * @param bufID The buffer ID, is a number from 0 to n identifying the buffer
 * @param b   The byte to add to the given buffer
 */
void busGetByte(BYTE bufID, BYTE b)
{

}

/**
 * Gets an array of bytes from the given buffer. 
 *
 * @param bufID The buffer ID, is a number from 0 to n identifying the buffer
 * @param buf   The byte array to add to the read bytes to
 * @param len   The number of bytes to read
 *
 * @return The actual number of bytes read
 */
BYTE busGetArray(BYTE bufID, BYTE* buf, BYTE len)
{
    return 0;
}

