/*********************************************************************
*
*       UDP example program.
*
* This example program shows how to receive UDP datagrams. This program
* acts as the server, and listens for any data entering on UDP port 54123.
* If a UDP message is received, we set the system LED if the first byte of
* the received message '1', and clear the system LED if it is '0'
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
    //Second tick variables
    TICK8  tsec8Bit = 0;    //8 Bit Second Tick, range of 0 to 255 seconds
    TICK16 tsec = 0;        //16 Bit Second Tick, range of 0 to 65,536 sec = 0 to 18.2 hours
    
    //The default tick period is 10ms
    TICK8  t8Bit = 0;       //Has 8 Bits, range of 0 - 255 = 255 x 10ms = 2.55 seconds
    TICK16 t16Bit = 0;      //Has 16 Bits, range of 0 - 65535 = 65535 x 10ms
                            // = 655 seconds = 10.9 minutes
    TICK   t = 0;           //Default TICK, Has 32 Bits, range of 0 - 2^32
                            // = 11930 hours = 497 days

    //Initialize AppConfig structure
    appcfgInit();
    
    //Initialize any application specific hardware.
    InitializeBoard();

    //Initialize all stack related components.
    TickInit();

    //Initialize the TCP/IP stack
    StackInit();
    
    //Set system LED as output
    TRISB_RB6 = 0;

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
        //Use default tick (32 bit), blink system LED every half a second
        //This method uses a 32 bit tick, which produces the slowest code but has the
        //longest tick range, 0 - 497 days. It has a resolution of 10ms.
        if ( TickGetDiff(t) >= ((TICK)(TICKS_PER_SECOND/2)) )
        {
            t = TickGet();

            LATB6 ^= 1;     //Toggle system LED
        }

        //Use 16 bit tick, execute every half a second
        //This method uses a 16 bit tick, which produces medium size code and
        //has a range of 0 - 655 seconds. It has a resolution of 10ms.
        if ( TickGetDiff16bit(t16Bit) >= ((TICK16)(TICKS_PER_SECOND/2)) )
        {
            t16Bit = TickGet16bit();
            
            //Insert user code here - will be executed every half a second
            //...............
        }

        //Use 8 bit tick, execute every half a second
        //This method uses a 8 bit tick, which produces small and fast
        //code, but only has a range of 0 - 2.55 seconds. It has a resolution of 10ms.
        if ( TickGetDiff8bit(t8Bit) >= ((TICK8)(TICKS_PER_SECOND/2)) )
        {
            t8Bit = TickGet8bit();

            //Insert user code here - will be executed every half a second
            //...............
        }

        //Use 8 bit second tick, execute every second
        //This method uses a 8 bit second tick, which produces small and fast
        //code, but only has a range of 0 - 255 seconds. It has a resolution of
        //1 second.
        if ( TickGetDiff8bitSec(tsec8Bit) >= 1)
        {
            tsec8Bit = TickGet8bitSec();

            //Insert user code here - will be executed every half a second
            //...............
        }

        //Use 16 bit second tick, execute every second
        //This method uses a 16 bit second tick, which produces medium size
        //code, and has a range of 0 to 65,536 sec = 0 to 18.2 hours.
        //It has a resolution of 1 second.
        if ( TickGetDiffSec(tsec) >= 1)
        {
            tsec = TickGetSec();

            //Insert user code here - will be executed every half a second
            //...............
        }

        //This task performs normal stack task including checking for incoming packet,
        //type of packet and calling appropriate stack entity to process it.
        StackTask();
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
