 /**
 * @brief           Events
 * @file            event.c
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
 * as well as to civil liability for the breach of the terms and conditions of this license...
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2009-05-14, David Hosken (DH):
 *    - Initial version
 *********************************************************************/
#define THIS_IS_EVENT_MODULE

#include <string.h>

#include "projdefs.h"
#include "event.h"
#include "debug.h"
#include "appcfg.h"
#include "lcd2s.h"
#include "ior5e.h"
#include "mxd2r.h"
#include "io.h"

#include "net\stacktsk.h"
#include "net\udp.h"
#include "net\helpers.h"
#include "net\security.h"   //Security related code
#include "net\tick.h"


/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xDA, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xDA, msgCode, msgStr)


/////////////////////////////////////////////////
// Function prototypes
BYTE chkLCD(void);
BYTE chkXboard(void);


//Create a UDP socket for the "UDP event port"
UDP_SOCKET udpSocketEvt;

union
{
    struct
    {
        unsigned int bEvtTest1 : 1;     //Test event flag, not used, just for testing
    } bits;
    BYTE val;
} eventFlags;


/////////////////////////////////////////////////
// "Event State Machine"
typedef enum _SM_EVENT
{
    SM_EVT_INIT=0,          /* Initializing UDP port - has no IP and MAC address */
    SM_EVT_INIT_MSG,        /* Send message to remote PC */
    SM_EVT_IDLE,            /* Idle State */
} SM_EVENT;
static BYTE smEvt = SM_EVT_IDLE;     //variable for "Event State Machine"


/////////////////////////////////////////////////
//Configures what event ports are active
BYTE activeEventPorts;


/**
 * Initializes "UDP Command Port" and "UDP Event Port". These
 * ports are used for sending and receiving and commands via the UDP port.
 *
 */
void evtInit(void) {
    NODE_INFO udpServerNode;
    activeEventPorts = 0;
    
    //Initialize to idle state
    smEvt = SM_EVT_INIT;

    //Initialize remote IP and MAC address of udpServerNode with 0, seeing that we don't know them for the node
    //that will send us an UDP message. The first time a message is received addressed to this port, the
    //remote IP and MAC addresses are automatically updated with the addresses of the remote node.
    memclr(&udpServerNode, sizeof(udpServerNode));

    //Configure for local port 54124 and remote port INVALID_UDP_PORT. This opens the socket to
    //listen on the given port.
    udpSocketEvt = UDPOpen(EVENT_UDPPORT, &udpServerNode, INVALID_UDP_PORT);
    
    //An error occurred during the UDPOpen() function
    if (udpSocketEvt == INVALID_UDP_SOCKET) {
        #if (DEBUG_CMD >= LOG_ERROR)
        debugPutMsg(17);     //@mxd:17:Could not open UDP event port
        #endif
    }
}


/**
 * Must be called every couple of ms.
 */
void evtTask(void) {
    static TICK8 tEvt = 0;
    static BYTE heartbeat = 20;

    //Buffer
    char buf[64];
    BYTE buf2Tx;


    /////////////////////////////////////////////////
    //Events
    switch(smEvt) {
    //UDP Event port has not been initialized yet, still waiting for MAC and IP address of remote host
    case SM_EVT_INIT:
        //Is there any data waiting for us on the "UDP Event port"?
        //Because of the design of the Modtronix TCP/IP stack we have to consume all data sent to us
        //as soon as we detect it. Store all data to a buffer as soon as it is detected
        if (UDPIsGetReady(udpSocketEvt)) {
            UDPGetArray((BYTE *)buf, 2);    //Read first 2 bytes from UDP message, indicates what event ports are active. Bit0=UDP Event Port
            UDPDiscard();           //Not using receive, so discart msg
            if (hextob( (char *)buf, &activeEventPorts) == 0) {
                activeEventPorts = 0;     //If error, disable all event ports
            }

            smEvt = SM_EVT_INIT_MSG;
        }
        break;
    case SM_EVT_INIT_MSG:
        //UDP Event port is active
        if ( ((activeEventPorts & EVT_PORT_UDP)!=0) && UDPIsPutReady(udpSocketEvt) ) {
            strcpypgm2ram(buf, (ROM char*)"l40=1;");
            UDPPutArray((BYTE *)buf, strlen(buf));

            // Now transmit it.
            UDPFlush();
            smEvt = SM_EVT_IDLE;
        }
        break;
    case SM_EVT_IDLE:
        buf2Tx = 0;
        
        //Is there any data waiting for us on the "UDP Event port"?
        //Because of the design of the Modtronix TCP/IP stack we have to consume all data sent to us
        //as soon as we detect it. Store all data to a buffer as soon as it is detected
        if (UDPIsGetReady(udpSocketEvt)) {
            UDPGetArray((BYTE *)buf, 2);    //Read first 2 bytes from UDP message, indicates what event ports are active. Bit0=UDP Event Port
            UDPDiscard();           //Not using receive, so discart msg
            if (hextob( (char *)buf, &activeEventPorts) == 0) {
                activeEventPorts = 0;     //If error, disable all event ports
            }
            break;          //Have to break, so stack task can be called, and UDPDiscart is executed
        }

        //Enter every 50ms
        if ( TickGetDiff8bit(tEvt) >= ((TICK8)TICKS_PER_SECOND / (TICK8)20) )
        {
            tEvt = TickGet8bit();

            //Every second
            if (--heartbeat == 0) {
                heartbeat = 20;
                //UDP Event port is active
                if ( ((activeEventPorts & EVT_PORT_UDP)!=0) && UDPIsPutReady(udpSocketEvt) ) {
                    strcpypgm2ram(buf, (ROM char*)"l40=2;");
                    UDPPutArray((BYTE *)buf, strlen(buf));
                    buf2Tx |= EVT_PORT_UDP;     //Indicate that data was added to UDP event port, and it must be TXed
                }
            }

            //Check LCD display every 50ms
            buf2Tx |= chkLCD();

            //Check Expansion Board every 50ms
            buf2Tx |= chkXboard();
        }

        //Was anything added to UDP event port
        if (buf2Tx & EVT_PORT_UDP) {
            UDPFlush(); //Transmit contents of UDP buffer
        }
        break;
    }
}

/**
 * Check LCD display every 50ms
 *
 * @return Set's bit 0 in returned byte if data added to UDP event port
 */
BYTE chkLCD(void) {
    BYTE ret = 0;
    BYTE i;
    BYTE key;

    //Empty all keypad buffers
    for (i=0; i<LCD2S_MAX_NUMBER; i++) {
        while (kpadHasKeys(i)) {
            key = kpadGetKey(i);    //Get next key

            //UDP Event port is active
            if ( ((activeEventPorts & EVT_PORT_UDP)!=0) && UDPIsPutReady(udpSocketEvt) ) {
                ret |= EVT_PORT_UDP; //Indicate that data was added to UDP event port

                //Tags for keypad data from LCD display 1 to 4 is "l34" to "l37"
                UDPPut('l');
                UDPPut('3');
                UDPPut('4' + i);
                UDPPut('=');
                UDPPut(key);
                UDPPut(';');
            }
        }
    }
    
    return ret;
}

/**
 * Check Expansion Board every 50ms
 *
 * @return Set's bit 0 in returned byte if data added to UDP event port
 */
BYTE chkXboard(void) {
    if (appcfgGetc(APPCFG_XBRD_TYPE) == XBRD_TYPE_IOR5E) {
        return ior5eAddEvents();
    }
    else if (appcfgGetc(APPCFG_XBRD_TYPE) == XBRD_TYPE_MXD2R) {
        return mxd2rAddEvents();
    }

    return 0;
}

