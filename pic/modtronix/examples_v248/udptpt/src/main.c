/*********************************************************************
*       UDP example program.
*********************************************************************
* FileName:        main.c
* Processor:       PIC18
* Complier:        MCC18 v1.00.50 or higher
*                  HITECH PICC-18 V8.10PL1 or higher
* Company:         Microchip Technology, Inc.
 ********************************************************************/

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
 * 2005-12-22, David Hosken (DH):
 *    - Initial version
 *********************************************************************/


//////////////////////////////////////////////////////////////////////
// Documentation
/**
@defgroup mod_examples_udptpt UDP Throughput Test
@ingroup  mod_examples

This example is contained in "./udptpt/src/main.c"

This program is used together with the Netix Network utility program (see
www.modtronix.com/soft/netix) to test the throughput of UDP messages.

The following modules are included in this project:
- ARP
- ICMP
- IP
- TCP
- UDP
- FSEE File System = File System using external EEPROM
- HTTP and HTTP Server
- Debugging via interrupt driven USART
********************************************************************/


/*
 * Following define uniquely define this file as main entry/application in whole
 * project, there should only be one such definition.
 */
#define THIS_IS_STACK_APPLICATION

#include <string.h>

#include "projdefs.h"
#include "debug.h"
#include "serint.h"

#include "net\cpuconf.h"	//Set default CPU configuration bits
#include "net\stacktsk.h"
#include "net\fsee.h"
#include "net\tick.h"
#include "net\helpers.h"
#include "net\udp.h"

#if defined(STACK_USE_HTTP_SERVER)
#include "net\http.h"
#endif

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xD6, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xD6, msgCode, msgStr)


/////////////////////////////////////////////////
//Version number
// - n = major part, and can be 1 or 2 digits long
// - mm is minor part, and must be 2 digits long!
ROM char APP_VER_STR[] = "V3.00";


//Create a UDP socket for receiving and one for transmitting
static UDP_SOCKET udpSocketUserRx = INVALID_UDP_SOCKET;
static UDP_SOCKET udpSocketUserTx = INVALID_UDP_SOCKET;

// Create 256 byte circular buffer for storing UDP received data. Seeing that this segment is 256 bytes
//large, it has to be defined it's own segment.
// - For MPLAB C18 compiler we have to add an entry in the linker file
// - For the Hi-Tech compiler nothing has to be done, compiler does it all
#if defined(MCHP_C18)
    #pragma udata BUFFER1_256BYTES
    BYTE udpRxBuf[256];
    #pragma udata   //Return to default section
    #pragma udata BUFFER2_256BYTES
    BYTE udpTxBuf[256];
    #pragma udata   //Return to default section
#endif

#if defined(HITECH_C18)
    BYTE udpRxBuf[256];
    BYTE udpTxBuf[256];
#endif


//If MAC_USE_ACCESSRAM has been defined, we use access RAM variables
//to optiomize speed and code size
//Microchip C18 compiler
#if defined(__18CXX)
    #pragma udata access accessram  //Indicate following data is in Access RAM section
#endif

//Get and Put pointer for udpRxBuf circular buffer. If Get pointer = Put pointer, the buffer is empty.
near BYTE udpRxBufGet;
near BYTE udpRxBufPut;
near BYTE iTestMode;
near BYTE smTest;
near BYTE iBytesReceived;
near WORD iTxCount;
near BYTE i;        //Temp variable to be used in main thread context

//Microchip C18 compiler
#if defined(__18CXX)
    #pragma udata   //Return to normal section
#endif


#define TSTMODE_IDLE        0   /* Idle */
#define TSTMODE_RX250       1   /* The client requestes to receive given number of 250 byte messages */
#define TSTMODE_LOOPBACK    2   /* The client requestes a loopback test */

//This test requestes the target to send us 8 messages with the following number of data bytes:
//8, 15, 49, 99, 255, 256, 768, 769. Each messages data contains incremented values starting from 0.
#define TSTMODE_RX8         10

#define SM_TEST_START           0
#define SM_LB_WAITING_FOR_MSG   0
#define SM_LB_TX_MSG            1


static void InitializeBoard(void);
static void ProcessIO(void);
static void udpUserInit(void);
static void udpUserProcess(void);


/////////////////////////////////////////////////
//High Interrupt ISR
#if defined(MCHP_C18)
    #pragma interrupt HighISR save=section(".tmpdata")
    void HighISR(void)
#elif defined(HITECH_C18)
    #if defined(STACK_USE_SLIP)
        extern void MACISR(void);
    #endif
    void interrupt HighISR(void)
#endif
{
    //TMR0 is used for the ticks
    if (INTCON_TMR0IF)
    {
        TickUpdate();

        #if defined(STACK_USE_SLIP)
        MACISR();
        #endif

        INTCON_TMR0IF = 0;
    }

    //The USART Receive and Transmit interrupts are used by the "serint.c" module. Is used for writing
    //debug information out onto the serial port and for serial configuration
    #ifdef SER_USE_INTERRUPT
    //USART Receive
	if(PIR1_RCIF && PIE1_RCIE)
	{
        serRxIsr();
	}

    //USART Transmit
	if(PIR1_TXIF && PIE1_TXIE)
	{
        serTxIsr();
	}
    #endif
}

#if defined(MCHP_C18)
#pragma code highVector=HIVECTOR_ADR
void HighVector (void)
{
    _asm goto HighISR _endasm
}
#pragma code /* return to default code section */
#endif

/*
 * Main entry point.
 */
void main(void)
{
    static TICK8 t = 0;

    //Initialize AppConfig structure
    appcfgInit();
    
    //Initialize any application specific hardware.
    InitializeBoard();

    //Initialize system timer
    TickInit();

    //Initialize file system.
    fsysInit();

    //Initialize serial port - is used for debugging
    serInit();

    //Initialize the TCP/IP stack
    StackInit();

#if defined(STACK_USE_HTTP_SERVER)
    HTTPInit();
#endif

    #if (DEBUG_MAIN >= LOG_DEBUG)
        debugPutMsg(1); //@mxd:1:Starting main loop
    #endif
    
    serReset();     //Reset the serial port (USART) - is used for writing out debug information
    serEnable();    //Enable the serial port (USART) - is used for writing out debug information
    udpUserInit();  //Initialize the UDP socket(s)
    
    iTestMode = TSTMODE_IDLE;

    /*
     * Once all items are initialized, go into infinite loop and let stack items execute
     * their tasks. If the application needs to perform its own task, it should be done at
     * the end of while loop. Note that this is a "co-operative mult-tasking" mechanism where
     * every task performs its tasks (whether all in one shot or part of it) and returns so
     * that other tasks can do their job. If a task needs very long time to do its job, it
     * must broken down into smaller pieces so that other tasks can have CPU time.
     */
    while(1)
    {
        //Blink SYSTEM LED every second.
        if ( TickGetDiff8bit(t) >= ((TICK8)TICKS_PER_SECOND / (TICK8)2) )
        {
            t = TickGet8bit();
            TRISB_RB6 = 0;
            LATB6 ^= 1;
        }
    
        //Perform user UDP task - this function must continuously be called
        udpUserProcess(); 

        //This task performs normal stack task including checking for incoming packet,
        //type of packet and calling appropriate stack entity to process it.
        StackTask();

#if defined(STACK_USE_HTTP_SERVER)
        //This is a TCP application.  It listens to TCP port 80
        //with one or more sockets and responds to remote requests.
        HTTPServer();
#endif

        //Add your application specific tasks here.
        ProcessIO();

        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // ADD USER CODE HERE
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
}

/*
 * UDP Initialization
 */
static void udpUserInit(void)
{
    NODE_INFO udpServerNode;

    //Set UDP Receive buffer to be empty
    udpRxBufGet = udpRxBufPut = 0;
    
    smTest = SM_TEST_START;

    //Initialize remote IP and MAC address of udpServerNode with 0, seeing that we don't know them for the node
    //that will send us an UDP message. The first time a message is received addressed to this port, the
    //remote IP and MAC addresses are automatically updated with the addresses of the remote node.
    memclr(&udpServerNode, sizeof(udpServerNode));

    //Configure for local port 54123 and remote port INVALID_UDP_PORT. This opens the socket to
    //listed on the given port.
    udpSocketUserRx = UDPOpen(54123, &udpServerNode, INVALID_UDP_PORT);
    
    //An error occurred during the UDPOpen() function
    if (udpSocketUserRx == INVALID_UDP_SOCKET) {
        //Take any additional action that is required when an error occurs
        #if (DEBUG_GEN >= LOG_ERROR)
        debugPutGenMsg(4);     //@mxd:4:%s
        debugPutRomString((ROM char*)"Could not open UDP socket on port 54123");
        #endif
    }
    else {
        #if (DEBUG_GEN >= LOG_INFO)
        debugPutGenMsg(2);     //@mxd:2:%s
        debugPutRomString((ROM char*)"Successfully created UDP socket on port 54123");
        #endif
        //Take any additional action that is required when successfully opened
    }
}

/*
 * UDP routines
 */
static void udpUserProcess(void)
{
    BYTE c;        //Temp variable to be used in main thread context
    
    if (iTestMode == TSTMODE_IDLE) {
        iTxCount = 0;
        iBytesReceived = 0;
        smTest = SM_TEST_START;
        udpRxBufGet = udpRxBufPut = 0;  //Empty receive buffer
        
        //We are currently NOT in a test mode. Check if test mode request has been received.
        if (UDPIsGetReady(udpSocketUserRx)) {
            //Read all data from socket, and save it to UDP Receive buffer (udpRxBuf)
            while(UDPGet(&c)) {
                //Store byte read from UDP socket to UDP buffer
                udpRxBuf[udpRxBufPut++] = c;
                iBytesReceived++;
            }

            //Discard the socket buffer.
            UDPDiscard();

            //Check if the UDP socket for transmitting has been created yet. We only create this socket after we
            //have received a UDP datagram from the client. The reason is that we use the IP and MAC address
            //from the received UDP datagram
            if (udpSocketUserTx == INVALID_UDP_SOCKET) {
                //Create UDP socket for sending data to remote client on port 54124. We use the UDPGetNodeInfo()
                //go get the IP and MAC address of the remote client. 
                //Configure for local port 54130 and remote port 54124.
                udpSocketUserTx = UDPOpen((WORD)54130, UDPGetNodeInfo(), (WORD)54124);
    
                //An error occurred during the UDPOpen() function
                if (udpSocketUserTx == INVALID_UDP_SOCKET) {
                    //Take any additional action that is required when an error occurs
                    #if (DEBUG_GEN >= LOG_ERROR)
                    debugPutGenMsg(4);     //@mxd:4:%s
                    debugPutRomString((ROM char*)"Could not open UDP socket on port 54123");
                    #endif
                }
                else {
                    //Take any additional action that is required when successfully opened
                    #if (DEBUG_GEN >= LOG_INFO)
                    debugPutGenMsg(2);     //@mxd:2:%s
                    debugPutRomString((ROM char*)"Successfully created UDP socket on port 54124");
                    #endif
                }
            }
            
            if (udpRxBuf[3] == 'M' &&
                    udpRxBuf[4] == 'X' &&
                    udpRxBuf[5] == 'a' &&
                    udpRxBuf[6] == 'u')
            {
                //Request to enter RX250 test mode
                if (udpRxBuf[0] == TSTMODE_RX250) {
                    #if (DEBUG_GEN >= LOG_INFO)
                    debugPutGenMsg(2);     //@mxd:2:%s
                    debugPutRomString((ROM char*)"Entered RX250 Test mode");
                    #endif
                
                    //Get number of messages that we must transmit
                    iTxCount = ((WORD)udpRxBuf[2]) & 0xff;
                    iTxCount += ( (((WORD)udpRxBuf[1]) & 0xff) << 8 );
                
                    iTestMode = TSTMODE_RX250;
                }
                //Request to enter LOOPBACK test mode
                else if (udpRxBuf[0] == TSTMODE_LOOPBACK) {
                    #if (DEBUG_GEN >= LOG_INFO)
                    debugPutGenMsg(2);     //@mxd:2:%s
                    debugPutRomString((ROM char*)"Entered Loopback Test mode");
                    #endif
                
                    //Get number of messages that we must transmit
                    iTxCount = ((WORD)udpRxBuf[2]) & 0xff;
                    iTxCount += ( (((WORD)udpRxBuf[1]) & 0xff) << 8 );
                
                    iTestMode = TSTMODE_LOOPBACK;
                    smTest = SM_LB_WAITING_FOR_MSG;
                }
                //Request to enter LOOPBACK test mode
                else if (udpRxBuf[0] == TSTMODE_RX8) {
                    #if (DEBUG_GEN >= LOG_INFO)
                    debugPutGenMsg(2);     //@mxd:2:%s
                    debugPutRomString((ROM char*)"Start Other 1 Test");
                    #endif
                
                    iTestMode = TSTMODE_RX8;
                    smTest = 0;
                }
            }
        }
    }
    //We have to send the requested amount of 250 byte UDP messages to the client
    else if (iTestMode == TSTMODE_RX250) {
        if (iTxCount != 0) {
            //Is the TCP/IP stack ready to send UDP data?
            if (UDPIsPutReady(udpSocketUserTx)) {
                //Initialize first 250 bytes of tx buffer with 0,1,2,.....249
                for (i = 0; i < 250; i++) {
                    udpTxBuf[i] = i;
                }

                //Transmit it via UDP
                UDPPutArray(udpTxBuf, 250);
                
                /*
                for (i = 0; i < 250; i++) {
                    UDPPut(udpTxBuf[i]);
                }
                */

                // Now transmit it.
                UDPFlush();
                
                iTxCount--;
            }
        }
        else {
            iTestMode = TSTMODE_IDLE;
        }
    }
    //We have to loop back the UDP message just received
    else if (iTestMode == TSTMODE_LOOPBACK) {
        if (iTxCount != 0) {
            switch (smTest) {
            //Waiting to receive message
            case SM_LB_WAITING_FOR_MSG:
                //Check if anything has been received, and loop it back
                if (UDPIsGetReady(udpSocketUserRx)) {
                    udpRxBufPut = 0;
                    iBytesReceived = 0;
                    //Read all data from socket, and save it to UDP Receive buffer (udpRxBuf)
                    while(UDPGet(&c)) {
                        //Store byte read from UDP socket to UDP buffer
                        udpRxBuf[udpRxBufPut++] = c;
                        iBytesReceived++;
                    }

                    //Discard the socket buffer.
                    UDPDiscard();
                    
                    smTest = SM_LB_TX_MSG;
                }
                break;
            //Loop back last message received
            case SM_LB_TX_MSG:
                //Is the TCP/IP stack ready to send UDP data?
                if (UDPIsPutReady(udpSocketUserTx)) {
            
                    //Transmit the received buffer
                    UDPPutArray(udpRxBuf, iBytesReceived);

                    // Now transmit it.
                    UDPFlush();
                    
                    smTest = SM_LB_WAITING_FOR_MSG;
                
                    iTxCount--;
                }
                break;
            }
        }
        else {
            iTestMode = TSTMODE_IDLE;
        }
    }
    //Other 1 test
    else if (iTestMode == TSTMODE_RX8) {
        if (iTxCount > 0) {
            //Is the TCP/IP stack ready to send UDP data?
            if (UDPIsPutReady(udpSocketUserTx)) {
                //Initialize first 255 bytes of tx buffer with 0,1,2,.....2255
                i = 0;
                do {
                    udpTxBuf[i] = i;
                } while (i++ != 255);

                //Send byte array of 256 bytes long
                while (iTxCount >= 512) {
                    UDPPutArray(udpTxBuf, 256);
                    iTxCount -= 256;
                }

                //Send byte array in smaller bits using UDPPutArray() function
                while (iTxCount >= 256) {
                    UDPPutArray(&udpTxBuf[0], 0);
                    UDPPutArray(&udpTxBuf[0], 1);       //1 bytes sent
                    UDPPutArray(&udpTxBuf[1], 1);       //2 bytes sent
                    UDPPutArray(&udpTxBuf[2], 10);      //12 bytes sent
                    UDPPutArray(&udpTxBuf[12], 11);     //23 bytes sent
                    UDPPutArray(&udpTxBuf[23], 50);     //73 bytes sent
                    UDPPutArray(&udpTxBuf[73], 99);     //172 bytes sent
                    UDPPutArray(&udpTxBuf[172], 84);    //256 bytes sent
                    
                    iTxCount -= 256;
                }
                    
                UDPPutArray(udpTxBuf, iTxCount);
                iTxCount = 0;
                smTest++;

                // Now transmit it.
                UDPFlush();
            }
        }
        else {
            switch(smTest) {
                case 0: iTxCount = 8; break;
                case 1: iTxCount = 15; break;
                case 2: iTxCount = 49; break;
                case 3: iTxCount = 99; break;
                case 4: iTxCount = 255; break;
                case 5: iTxCount = 256; break;
                case 6: iTxCount = 768; break;
                case 7: iTxCount = 769; break;
                case 8: iTestMode = TSTMODE_IDLE; break;
            }
        }
    }
}


static void ProcessIO(void)
{
}

/**
 * Initialize the boards hardware
 */
static void InitializeBoard(void)
{
	#if (defined(MCHP_C18) && (defined(__18F458) || defined(__18F448))) \
		|| (defined(HITECH_C18) && (defined(_18F458) || defined(_18F448)))
		CMCON  = 0x07; /* Comparators off CM2:CM0 = 111 for PIC 18F448 & 18F458 */
	#endif

    //Disable external pull-ups
    INTCON2_RBPU = 1;

    /////////////////////////////////////////////////
    //Initialize USART1 Control registers
    TXSTA = 0b00100100;     //High BRG speed
    RCSTA = 0b10010000;
    BAUDCON = 0b00001000;
    //#define BAUD 1040   /* 9600 BAUD at 40MHz clock */
    //#define BAUD 520    /* 19200 BAUD at 40MHz clock */
    //#define BAUD 259    /* 38400 BAUD at 40MHz clock */
    #define BAUD 172    /* 57600 BAUD at 40MHz clock */
    //#define BAUD 86     /* 115200 BAUD at 40MHz clock */
    SPBRG =  (BYTE)BAUD;
    SPBRGH = (BYTE)(BAUD >> 8);



    /////////////////////////////////////////////////
    //Initialize ports    
    TRISA = 0b11111111; //All inputs

    //Ensure port TRISB 0 is always an inputs when NIC INT0 is enabled:
    #if defined(NIC_DISABLE_INT0)
        TRISB = 0b11111111; //All inputs
    #else
        //B0 = Input from RTL8019AS
        TRISB = 0b11111111 | 0x01;
    #endif

    //Ensure following TRISC ports are always inputs:
    //C3 = Is controlled by I2C unit
    //C4 = Is controlled by I2C unit
    //C6 = Is controlled by USART
    //C7 = Is controlled by USART
    TRISC = 0b11111111 | 0xd8;

    //Set PORT G and F direction bits
    TRISF = 0b11111111; //All inputs
    
    //Ensure TRISG 4 is always an input, it is connected to IOCHRDY
    TRISG = 0b11111111 | 0x10;

    //Enable interrupts
    T0CON = 0;
    INTCON_GIEH = 1;
    INTCON_GIEL = 1;
}

/**
 * This function is a "callback" from HTTPServer task. Whenever a remote node performs interactive
 * GET task on page that was served, HTTPServer calls this functions. Use HTTPGetParam()
 * to get all name-value parameters.
 *
 * @param httpInfo  Socket that is currently receiving this HTTP command
 * @param rqstRes   Name of the Requested resource - GET commands action. All characters are
 *                  in uppercase!
 */
void HTTPExecGetCmd(HTTP_INFO* httpInfo, BYTE* rqstRes)
{
    BYTE param[20];
    int PowerLevel, LowPowerSetting, HighPowerSetting;

    //Used as input AND output parameter for HTTPGetParams.
    // - Input parameter indicates size of param buffer
    // - On return of HTTPGerParam() valueIdx will contain index of value string in param
    BYTE valueIdx;
    BYTE moreParams;

    //Get next name-value parameter until we have retrieved them all
    do {
        valueIdx = (BYTE)sizeof(param);   //Input parameter is size of param buffer

        //Get name-value parameters. Returns true if there are more name-value parameters to follow
        //- Pointer to Name parameter = &param[0]
        //- Pointer to Value parameter = &param[valueIdx]
        moreParams = HTTPGetParam(httpInfo->socket, param, &valueIdx);

        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // ADD USER CODE HERE FOR CHECKING NAME-VALUE PAIRS RECEIVED VIA GET COMMAND
        // - The null terminated received name will be contained in @param[0]
        // - The null terminated received value will be contained in ((char*)&param[valueIdx])
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    } while (moreParams);


    // If another page is to be displayed as a result of this command, copy
    // its upper case name into rqstRes.
    //strcpy(rqstRes, "RESULTS.CGI");
}

ROM char SerialNumberStr[] = "123456SER";

#if defined(STACK_USE_HTTP_SERVER)
WORD HTTPGetVar(HTTP_INFO* httpInfo, BYTE* val)
{
    BYTE varValue, varGroup, ref;

    varValue = httpInfo->var.get.tagVal;        //Variable Value of requested variable
    varGroup = httpInfo->var.get.tagGroup;      //Variable Group of requested variable
    ref      = httpInfo->var.get.varRef;        //Current callback reference with respect to 'var' variable.
    
    //In case requested var not found, set it to NULL character and return HTTP_END_OF_VAR
    *val = '\0';    
    
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // ADD USER CODE HERE FOR CHECKING REQUESTED TAG, AND RETURNING CORRECT DATA
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}
#endif
