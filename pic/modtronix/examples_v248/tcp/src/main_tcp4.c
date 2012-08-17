/*********************************************************************
*       TCP example program.
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
 * 2006-10-11, David Hosken (DH):
 *    - Initial version
 *********************************************************************/


//////////////////////////////////////////////////////////////////////
// Documentation
/**
@defgroup mod_examples_tcp4 TCP Send File Example
@ingroup  mod_examples

This example is contained in "./tcp/src/main_tcp4.c"

This example program shows how to send a file via TCP. The file is sent
to socket 10.1.0.201:54124 (IP address 10.1.0.201, port 54124).
It uses the sendFileToTcpServer() funtion in two different ways, one
blocking and the other non blocking.

This project is compiled with the DEBUG_OFF not defined. This causes debug
data to be sent out via the serial port. The Modtronix Embedded Debugger
can be used to view the debug outputs.

The following modules are included in this project:
- ARP
- ICMP
- IP
- TCP
********************************************************************/


/*
 * Following define uniquely define this file as main entry/application in whole
 * project, there should only be one such definition.
 */
#define THIS_IS_STACK_APPLICATION

#include <string.h>

#include "projdefs.h"
#include "debug.h"
#include "net\cpuconf.h"	//Set default CPU configuration bits
#include "net\stacktsk.h"
#include "net\tcputils.h"

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xD6, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xD6, msgCode, msgStr)


//TCP Send File State machine
static enum _smSendFile {
    SM_SENDFILE_INIT = 0,
    SM_SENDFILE,
    SM_SENDFILE_DONE
} smSendFile = SM_SENDFILE_INIT;

static void InitializeBoard(void);


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
    BYTE filename[12];
    BYTE sendFileRet;
    TCP_FILESEND_INFO tcpFileSendInfo;

    //Initialize AppConfig structure
    appcfgInit();
    
    //Initialize any application specific hardware.
    InitializeBoard();

    //Initialize all stack related components.
    TickInit();

    //Initialize file system.
    fsysInit();

    //Initialze serial port
    serInit();
    serEnable();

    //Initialize the TCP/IP stack
    StackInit();

    #if (DEBUG_GEN >= LOG_INFO)
    debugPutGenInfo((ROM char*)"Starting Application");
    #endif


    /************************************************/
    /**************** Method 1 begin ****************/
    //This method shows how to send a file with the sendFileToTcpServer()
    //function that blocks until the file has been sent. The while()
    //statement only breaks once the entire file has been
    
    //Initialize TCP_FILESEND_INFO structure
    tcpFileSendInfo.serverNode.IPAddr.v[0]  = 10;       //Server's IP address
    tcpFileSendInfo.serverNode.IPAddr.v[1]  = 1;
    tcpFileSendInfo.serverNode.IPAddr.v[2]  = 0;
    tcpFileSendInfo.serverNode.IPAddr.v[3]  = 201;
    tcpFileSendInfo.port                    = 54124;    //Port to send file to
    tcpFileSendInfo.sm                      = 0;        //Start state
    
    //Initialize filename array with name of file to open, including terminating NULL
    strcpypgm2ram((char*)filename, (ROM char*)"IOVAL.CGI");
    
    //Repetitively call sendFileToTcpServer() until done - returns 0xff is not done yet!
    while( (sendFileRet=sendFileToTcpServer(filename, &tcpFileSendInfo)) == 0xff)
        StackTask();
    if (sendFileRet != 0) {
        //Error during file send, take action!
    }
    /////////////////////////////////////////////////
    ///////////////// Method 1 end //////////////////


    //Initialize state machine to 0 to cause file to be sent
    smSendFile = SM_SENDFILE_INIT;

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
        /////////////////////////////////////////////////
        //////////////// Method 2 begin /////////////////
        //This method shows how to send a file with the sendFileToTcpServer()
        //function that does NOT block until the file has been sent. It will
        //return 0xff if the operation is not done yet. This function has to
        //be repetitively called as long as it returns 0xff.
        switch (smSendFile) {
            //Initialize tcpFileSendInfo structure
            case SM_SENDFILE_INIT:
                //Initialize TCP_FILESEND_INFO structure
                tcpFileSendInfo.serverNode.IPAddr.v[0]  = 10;       //Server's IP address
                tcpFileSendInfo.serverNode.IPAddr.v[1]  = 1;
                tcpFileSendInfo.serverNode.IPAddr.v[2]  = 0;
                tcpFileSendInfo.serverNode.IPAddr.v[3]  = 201;
                tcpFileSendInfo.port                    = 54124;    //Port to send file to
                tcpFileSendInfo.sm                      = 0;        //Start state
                
                //Initialize filename array with name of file to open
                strcpypgm2ram((char*)filename, (ROM char*)"AVAL.CGI");

                #if (DEBUG_GEN >= LOG_INFO)
                debugPutGenInfo((ROM char*)"Sending file");
                #endif
                smSendFile++;
                //break;    //No break
            //Send file. We remain in this state as long as sendFileToTcpServer() returns 0xff
            case SM_SENDFILE:
                if ( (sendFileRet=sendFileToTcpServer(filename, &tcpFileSendInfo)) == 0) {

                    //File successfully sent!
                    //Take action if required
                    
                    smSendFile++;       //File send done, go to done state

                    #if (DEBUG_GEN >= LOG_INFO)
                    debugPutGenInfo((ROM char*)"File successfully sent!");
                    #endif
                }
                //An error occured during file send, take action if required!
                else if (sendFileRet != 0xff) {
                        
                    //Error while sending file, see sendFileRet for details on type of error
                    
                    smSendFile++;   //File send error, go to done state

                    #if (DEBUG_GEN >= LOG_INFO)
                    debugPutGenInfo((ROM char*)"File send error!");
                    #endif
                }
                break;
            //File send finished
            case SM_SENDFILE_DONE:
                break;
        }
        /////////////////////////////////////////////////
        ///////////////// Method 2 end //////////////////

        //This task performs normal stack task including checking for incoming
        //packet, type of packet and calling appropriate stack entity to
        //process it.
        StackTask();

        /////////////////////////////////////////////////
        ////////////// Add user code here ///////////////
    }
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

    //Enable external pull-ups on Port B
    INTCON2_RBPU = 0;

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
    
    
    //Set system led port as an output, and switch it on
    TRISB_RB6 = 0;
    LATB6 = 0;    //Switch system LED on

    //Enable interrupts
    T0CON = 0;
    INTCON_GIEH = 1;
    INTCON_GIEL = 1;
}
