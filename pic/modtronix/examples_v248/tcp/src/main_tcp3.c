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
 * 2005-12-22, David Hosken (DH):
 *    - Initial version
 *********************************************************************/


//////////////////////////////////////////////////////////////////////
// Documentation
/**
@defgroup mod_examples_tcp3 TCP Client Example
@ingroup  mod_examples

This example is contained in "./tcp/src/main_tcp3.c"

This example program shows how to use the TCP/IP stack in client mode.
It sends a TCP message each time PIC port pin B0 is 0. The message
is sent to socket 10.1.0.101:54124 (IP address 10.1.0.101, port 54124).

The following modules are included in this project:
- ARP
- ICMP
- IP
- UDP
- TCP
********************************************************************/


/*
 * Following define uniquely define this file as main entry/application in whole
 * project, there should only be one such definition.
 */
#define THIS_IS_STACK_APPLICATION

#include <string.h>

#include "projdefs.h"

#include "net\cpuconf.h"	//Set default CPU configuration bits
#include "net\stacktsk.h"
#include "net\tick.h"
#include "net\helpers.h"
#include "net\tcp.h"
#include "net\arp.h"
#include "net\arptsk.h"

#if defined(STACK_USE_HTTP_SERVER)
#include "net\http.h"
#endif


//Create a TCP socket for receiving and sending data
static TCP_SOCKET tcpSocketUser = INVALID_SOCKET;

//TCP State machine
#define SM_TCP_SEND_ARP         0
#define SM_TCP_WAIT_RESOLVE     1
#define SM_TCP_RESOLVED         2
#define SM_TCP_FINISHED         3
static BYTE smTcp = SM_TCP_SEND_ARP;

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
    TICK8  tsecWait = 0;           //General purpose wait timer
    TICK16 tsecMsgSent = 0;        //Time last message was sent
    char c;
    NODE_INFO tcpServerNode;       //Remote server information

    //Initialize AppConfig structure
    appcfgInit();
    
    //Initialize any application specific hardware.
    InitializeBoard();

    //Initialize all stack related components.
    TickInit();

    //Initialize the TCP/IP stack
    StackInit();


    /////////////////////////////////////////////////
    //Initialize TCP socket

    //Initialize remote IP and address with 10.1.0.101. The MAC address is
    //is not intialized yet, but after we receive an ARP responce.
    tcpServerNode.IPAddr.v[0] = 10;
    tcpServerNode.IPAddr.v[1] = 1;
    tcpServerNode.IPAddr.v[2] = 0;
    tcpServerNode.IPAddr.v[3] = 101;

    tcpSocketUser = INVALID_SOCKET;

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
        switch (smTcp) {
        case SM_TCP_SEND_ARP:
            if (ARPIsTxReady()) {
                tsecWait = TickGet8bitSec();   //Remember when we sent last request
                
                //Send ARP request for given IP address
                ARPResolve(&tcpServerNode.IPAddr);
                
                smTcp = SM_TCP_WAIT_RESOLVE;
            }
            break;
        case SM_TCP_WAIT_RESOLVE:
            //The IP address has been resolved, we now have the MAC address of the
            //node at 10.1.0.101
            if (ARPIsResolved(&tcpServerNode.IPAddr, &tcpServerNode.MACAddr)) {
                //After calling TCPConnect(), we have to wait for a connection
                //to be established. Stack will continuously try to connect!
                smTcp = SM_TCP_RESOLVED;

                //We can now connect, seeing that we have the remote server node
                //info (MAC and IP address)
                tcpSocketUser = TCPConnect(&tcpServerNode, 54124);

                //An error occurred during the TCPListen() function
                if (tcpSocketUser == INVALID_SOCKET) {
                    //Add user code here to take action if required!
                }
            }
            //If not resolved after 2 seconds, send next request
            else {
                if (TickGetDiff8bitSec(tsecWait) >= (TICK8)2) {
                    smTcp = SM_TCP_SEND_ARP;
                }
            }
            break;
        case SM_TCP_RESOLVED:
            //Connection has been established
            if (TCPIsConnected(tcpSocketUser)) {
                if ( !PORTB_RB0) {
                    //Send a message every second for as long as PIC port pin B0 is = 0
                    if (TickGetSecDiff(tsecMsgSent) >= (TICK16)1) {
                        //Checks if there is a transmit buffer ready for accepting data, and that the given socket
                        //is valid (not equal to INVALID_SOCKET for example)
                        if (TCPIsPutReady(tcpSocketUser)) {
                            tsecMsgSent = TickGetSec();     //Update with current time

                            //Send a TCP message with one byte only indicating the status. We are only
                            //interrested in the first byte of the message.
                            TCPPut(tcpSocketUser, 1);

                            //Send contents of transmit buffer, and free buffer
                            TCPFlush(tcpSocketUser);
                        }

                        //Toggle system LED each time a message is sent
                        TRISB_RB6 = 0;
                        LATB6 ^= 1;
                    }
                }
            
                //Close connection if not needed anymore!
                //TCPDisconnect(tcpSocketUser)
            }
            else {
                //The TCP stack will continuously try and connect. It is up to the user to
                //terminate the connection attempts if no connection is made after a
                //certian time. The TCPDisconnect() function can be used to stop the
                //stack from trying to connect.
                
                //TCPDisconnect();
                //smTcp = SM_TCP_FINISHED;    //Set to "message sent" state
            }
            
            break;
        case SM_TCP_FINISHED:
            //Message sent state
            break;
        }

        //This task performs normal stack task including checking for incoming packet,
        //type of packet and calling appropriate stack entity to process it.
        StackTask();

        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // ADD USER CODE HERE
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
