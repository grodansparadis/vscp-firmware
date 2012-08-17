/*********************************************************************
*
*       UDP example program.
*
* This example program shows how to send and receive UDP datagrams. This program
* acts as the server, and listens for any data entering on UDP port 54123.
* If a UDP message is received, we read it and return data to the same socket
* (IP and port) that just sent us data.
*
* The following modules included in this project:
* - ARP
* - ICMP
* - IP
* - UDP
*
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
#include "net\udp.h"

#if defined(STACK_USE_HTTP_SERVER)
#include "net\http.h"
#endif


//Create a UDP socket for receiving and sending data
static UDP_SOCKET udpSocketUser = INVALID_UDP_SOCKET;


// Create 256 byte circular buffer for storing UDP received data. Seeing that this segment is 256 bytes
//large, it has to be defined it's own segment.
// - For MPLAB C18 compiler we have to add an entry in the linker file
// - For the Hi-Tech compiler nothing has to be done, compiler does it all
#if defined(MCHP_C18)
    #pragma udata BUFFER1_256BYTES
    BYTE udpRxBuf[256];
    #pragma udata   //Return to default section
#endif

#if defined(HITECH_C18)
    BYTE udpRxBuf[256];
#endif

//Get and Put pointer for udpRxBuf circular buffer. If Get pointer = Put pointer, the buffer is empty.
BYTE udpRxBufGet;
BYTE udpRxBufPut;


static void InitializeBoard(void);
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
    NODE_INFO udpServerNode;
    static TICK8 t = 0;

    //Initialize AppConfig structure
    appcfgInit();
    
    //Initialize any application specific hardware.
    InitializeBoard();

    //Initialize all stack related components.
    TickInit();

    //Initialize the TCP/IP stack
    StackInit();


    /////////////////////////////////////////////////
    //Initialize UDP socket

    //Set UDP Receive buffer to be empty
    udpRxBufGet = udpRxBufPut = 0;

    //Initialize remote IP and MAC address of udpServerNode with 0, seeing that we don't know them for the node
    //that will send us an UDP message. The first time a message is received addressed to this port, the
    //remote IP and MAC addresses are automatically updated with the addresses of the remote node.
    memclr(&udpServerNode, sizeof(udpServerNode));

    //Configure for local port 54123 and remote port INVALID_UDP_PORT. This opens the socket to
    //listed on the given port.
    udpSocketUser = UDPOpen(54123, &udpServerNode, INVALID_UDP_PORT);
    
    //An error occurred during the UDPOpen() function
    if (udpSocketUser == INVALID_UDP_SOCKET) {
        //Take any additional action that is required when an error occurs
    }



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
        if ( TickGetDiff8bit(t) >= ((TICK8)(TICKS_PER_SECOND/2)) )
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

        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // ADD USER CODE HERE
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
}

/*
 * UDP routines
 */
static void udpUserProcess(void)
{
BYTE c;

    /////////////////////////////////////////////////
    //Is there any data waiting for us on the UDP socket?
    //Because of the design of the Modtronix TCP/IP stack we have to consume all data sent to us as soon
    //as we detect it. Store all data to a buffer as soon as it is detected
    if (UDPIsGetReady(udpSocketUser)) {

        //Read all data from socket, and save it to UDP Receive buffer (udpRxBuf)
        while(UDPGet(&c)) {
            //Store byte read from UDP socket to UDP buffer
            udpRxBuf[udpRxBufPut++] = c;
            
            //If Put pointer caught up to Get pointer, our buffer is full and we have lost data!
            //Increment Get pointer.
            if (udpRxBufPut == udpRxBufGet) {
                udpRxBufGet++;
            }
        }

        //Discard the socket buffer.
        UDPDiscard();
    }


    /////////////////////////////////////////////////
    //Does the UDP receive buffer contain any data? If so, we sent this data back to the same socket
    //that just send us data. 
    if(udpRxBufPut != udpRxBufGet) {
        //Checks if there is a transmit buffer ready for accepting data, and that the given socket
        //is valid (not equal to INVALID_UDP_SOCKET for example)
        if (UDPIsPutReady(udpSocketUser)) {
            
            //Transmit entire buffer
            while (udpRxBufGet != udpRxBufPut) {
                UDPPut(udpRxBuf[udpRxBufGet++]);
            }

            // Now transmit it.
            UDPFlush();
        }
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

    //Disable external pull-ups
    INTCON2_RBPU = 1;

    //Enable interrupts
    T0CON = 0;
    INTCON_GIEH = 1;
    INTCON_GIEL = 1;


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
}
