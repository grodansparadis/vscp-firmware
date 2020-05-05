 /**
 * @brief           Reference implemention for VSCP on Modtronix hardware SBC65EC and 
 * 					SBC68EC
 * @file            nova.c
 * @author          Ake Hedman, eurosource, <a href="www.vscp.org">VSCP Project</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_vscp
 *
 *
 * @section description Description
 **********************************
 * This module contains the code that implements that 
 *
 *********************************************************************/

/*********************************************************************
*
* Software License Agreement
*
* The software supplied herewith by Microchip Technology Incorporated
* (the Company) for its PICmicro® Microcontroller is intended and
* supplied to you, the Company’s customer, for use solely and
* exclusively on Microchip PICmicro Microcontroller products. The
* software is owned by the Company and/or its supplier, and is
* protected under applicable copyright laws. All rights are reserved.
* Any use in violation of the foregoing restrictions may subject the
* user to criminal sanctions under applicable laws, as well as to
* civil liability for the breach of the terms and conditions of this
* license.
*
* THIS SOFTWARE IS PROVIDED IN AN AS IS CONDITION. NO WARRANTIES,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
* TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
* IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
* CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
*
*
* HiTech PICC18 Compiler Options excluding device selection:
*                  -FAKELOCAL -G -O -Zg -E -C
*
*
*
*
* Author               	Date         	Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* David Hosken         	15/02/04     	Original (Rev. 1.0)
* Ake Hedman			18/05/06		VSCP reference implemention
 ********************************************************************/

/*
 * Following define uniquely deines this file as main
 * entry/application In whole project, there should only be one such
 * definition and application file must define AppConfig variable as
 * described below.
 */
#define THIS_IS_STACK_APPLICATION

#include <inttypes.h>
#include <string.h>
#include "can18f.h"

#include "version.h"
#include "vscp_projdefs.h"
#include "projdefs.h"
#include "debug.h"
#include "serint.h"
#include "sercfg.h"
#include "appcfg.h"
#include "httpexec.h"

#include "net\cpuconf.h"
#include "net\stacktsk.h"
#include "net\fsee.h"
#include "net\tick.h"
#include "net\helpers.h"
#include "net\tcp.h"
#include "net\udp.h"

#include <vscp_2.h>
#include <vscp_task.h>
#include <vscptcp_task.h>
#include <can_msg.h>

#include "can_msg.h"

#if defined(STACK_USE_DHCP)
#include "net\dhcp.h"
#endif

#if defined(STACK_USE_HTTP_SERVER)
#include "net\http.h"
#endif

#if defined(STACK_USE_FTP_SERVER)
#include "net\ftp.h"
#endif

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xD6, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xD6, msgCode, msgStr)

/////////////////////////////////////////////////
// Nova globals
//

BOOL bInitialized;	// Workaround for stack startup delay/problem.
volatile unsigned char g_can_error = 0;

#if defined(STACK_USE_NTP_SERVER)

// Time - for ntptask
unsigned char hour;
unsigned char minute;
unsigned char second;

#endif

/////////////////////////////////////////////////
//Version number
// - n = major part, and can be 1 or 2 digets long
// - mm is minor part, and must be 2 digets long!
ROM char APP_VER_STR[] = "V3.04";

BYTE myDHCPBindCount = 0;
#if defined(STACK_USE_DHCP)
    extern BYTE DHCPBindCount;
#else
    /*
     * If DHCP is not enabled, force DHCP update.
     */
    BYTE DHCPBindCount = 1;
#endif

#if defined(APP_USE_ADC8)
extern BYTE AdcValues[ADC_CHANNELS];
#elif defined(APP_USE_ADC10)
extern WORD AdcValues[ADC_CHANNELS];
#endif

static union
{
    struct
    {
        unsigned int bFreezeADCBuf : 1;     //Stop updating AdcValues[] ADC buffer
    } bits;
    BYTE Val;
} mainFlags;

static void InitializeBoard(void);
static void ProcessIO(void);


/////////////////////////////////////////////////
//High Interrupt ISR
#if defined(MCHP_C18)
    #pragma interrupt HighISR save = section(".tmpdata")
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

	// ERRIF: CAN bus Error Interrupt Flag bit
	if ( PIR3bits.ERRIF ) 
	{
		// save the comstat register for analysis later on
		g_can_error = COMSTAT;
		COMSTAT = 0;
		PIR3 = 0;	
	}
	// CAN support
}

#if defined(MCHP_C18)
#pragma code highVector=HIVECTOR_ADR
void HighVector (void)
{
    _asm goto HighISR _endasm
}
#pragma code /* return to default code section */
#endif

///////////////////////////////////////////////////////////////////////////////
// Main entry point.
//
void main(void)
{
    static TICK8 t = 0;
    BYTE i;
    char strBuf[10];

    // Initialize any application specific hardware.
    InitializeBoard();

    // Initialize all stack related components.
    // Following steps must be performed for all applications using
    // PICmicro TCP/IP Stack.
    TickInit();

    // Initialize file system.
    fsysInit();

    // Intialize HTTP Execution unit
    htpexecInit();

    // Initialze serial port
    serInit();
    
    // Initialize Stack and application related NV variables.
    appcfgInit();
    appcfgUSART();        	// Configure the USART

#ifdef SER_USE_INTERRUPT    // Interrupt enabled serial ports have to be enabled
    serEnable();
#endif

    appcfgCpuIO();          // Configure the CPU's I/O port pin directions - input or output
    appcfgCpuIOValues();    // Configure the CPU's I/O port pin default values
    appcfgADC();            // Configure ADC unit
	appcfgPWM();			// Configure PWM unit
	
    // Serial configuration menu - display it for configured time and 
    // allow user to enter configuration menu
    scfInit( appcfgGetc( APPCFG_STARTUP_SER_DLY ) );

    StackInit();

#if defined(STACK_USE_HTTP_SERVER)
    HTTPInit();
#endif


#if defined( STACK_USE_DHCP ) || defined( STACK_USE_IP_GLEANING )
    // If DHCP is NOT enabled
    if ( ( appcfgGetc( APPCFG_NETFLAGS ) & APPCFG_NETFLAGS_DHCP ) == 0 ) {
		// Force IP address display update.
        myDHCPBindCount = 1;
        
#if defined( STACK_USE_DHCP )
        DHCPDisable();
#endif
    }
#endif

#if ( DEBUG_MAIN >= LOG_DEBUG )
        debugPutMsg(1); //@mxd:1:Starting main loop
#endif

	// Init VSCP functionality
	vscp_init();

	bInitialized = FALSE;	// Not initialized
    
#if defined(STACK_USE_NTP_SERVER)    
    // Initialize time 
    hour = 0;
	minute = 0;
	second = 0;
#endif	
	
	appcfgPutc( VSCP_DM_MATRIX_BASE, 0x00 );
 	appcfgPutc( VSCP_DM_MATRIX_BASE+1, 0x00 );
 	appcfgPutc( VSCP_DM_MATRIX_BASE+2, 0x00 );
 	appcfgPutc( VSCP_DM_MATRIX_BASE+3, 0x00 );
    
    
	//
    // Once all items are initialized, go into infinite loop and let
    // stack items execute their tasks.
    // If application needs to perform its own task, it should be
    // done at the end of while loop.
    // Note that this is a "co-operative mult-tasking" mechanism
    // where every task performs its tasks (whether all in one shot
    // or part of it) and returns so that other tasks can do their
    // job.
    // If a task needs very long time to do its job, it must broken
    // down into smaller pieces so that other tasks can have CPU time.
    //
    while ( 1 ) {
	    
	    // Used for initial delay to give stack and chip some time to
	    // initialize. If not used messages sent during this time will 
	    // fail.
        if  ( TickGet() > ( 5 * TICK_SECOND ) ) {
        	bInitialized = TRUE;
        }
	    
	    // We should do the ftp download every three hours
        //if ( TickGetDiff( TickGet(), loadTime ) >= ( 3 * 3600 * TICK_SECOND ) ) {
	    //	loadTime = TickGet();
	    //	bftpLoadWork = TRUE;
	    //}
	    
        // Blink SYSTEM LED every second.
        if ( appcfgGetc( APPCFG_SYSFLAGS ) & APPCFG_SYSFLAGS_BLINKB6 ) {
            if ( TickGetDiff8bit( t ) >= ((TICK8)( TICKS_PER_SECOND / 2 ) ) ) {
                t = TickGet8bit();
                TRISB_RB6 = 0;
                LATB6 ^= 1;
            }
        }

        // This task performs normal stack task including checking for incoming packet,
        // type of packet and calling appropriate stack entity to process it.
        StackTask();

#if defined(STACK_USE_HTTP_SERVER)
        // This is a TCP application.  It listens to TCP port 80
        // with one or more sockets and responds to remote requests.
        HTTPServer();
#endif

#if defined(STACK_USE_FTP_SERVER)
        FTPServer();
#endif

        // Add your application speicifc tasks here.
        ProcessIO();
        
#if defined(VSCP_USE_TCP )        
        // VSCP Task
        if ( bInitialized ) {
        	vscp_tcp_task();
        }
#endif        
        
        if ( bInitialized ) {
        	vscp_main_task();
			process_can_message();
			if ( g_can_error )
			{
				send_can_error_message( g_can_error );
				g_can_error = 0;
			}
        }
        
#if defined(STACK_USE_NTP_SERVER)        
        if ( bInitialized ) {
        	//ntp_task();
        }	
#endif        

        // For DHCP information, display how many times we have renewed the IP
        // configuration since last reset.
        if ( DHCPBindCount != myDHCPBindCount ) {
#if (DEBUG_MAIN >= LOG_INFO)
        	debugPutMsg( 2 ); 		// @mxd:2:DHCP Bind Count = %D
            debugPutByteHex(DHCPBindCount);
#endif
            
            // Display new IP address
#if (DEBUG_MAIN >= LOG_INFO)
            debugPutMsg( 3 ); 	//@mxd:3:DHCP complete, IP = %D.%D.%D.%D
            debugPutByteHex( AppConfig.MyIPAddr.v[ 0 ] );
            debugPutByteHex( AppConfig.MyIPAddr.v[ 1 ] );
            debugPutByteHex( AppConfig.MyIPAddr.v[ 2 ] );
            debugPutByteHex( AppConfig.MyIPAddr.v[ 3 ] );
#endif
            myDHCPBindCount = DHCPBindCount;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// ProcessIO
//

static void ProcessIO(void)
{
// Convert next ADC channel, and store result in adcChannel array
#if (defined(APP_USE_ADC8) || defined(APP_USE_ADC10)) && (ADC_CHANNELS > 0)
    static BYTE adcChannel; //Current ADC channel. Value from 0 - n

    // We are allowed to update AdcValues[] buffer
    // if (!mainFlags.bits.bFreezeADCBuf)
    {
        // Increment to next ADC channel    
        if ( ( ++adcChannel) >= ADC_CHANNELS ) {
            adcChannel = 0;
        }

        // Check if current ADC channel (adcChannel) is configured to be ADC channel
        if ( adcChannel < ((~ADCON1) & 0x0F ) ) {
            //Convert next ADC Channel
            ADCON0 &= 0x03;
            ADCON0 |= (adcChannel << 2);

            ADCON0_ADON = 1;    		// Switch on ADC
            ADCON0_GO = 1;      		// Go
    
            while (ADCON0_GO); 			// Wait for conversion to finish

#if defined(APP_USE_ADC8)
            AdcValues[adcChannel] = ADRESH;
#elif defined(APP_USE_ADC10)
            AdcValues[adcChannel] = ((WORD)ADRESH << 8) | ADRESL;
#endif
        }
        // Not ADC channel, set to 0
        else {
            AdcValues[ adcChannel ] = 0;
        }
    }
#endif

}

/////////////////////////////////////////////////
// Implement callback for FTPVerify() function

#if defined(STACK_USE_FTP_SERVER)
#if (FTP_USER_NAME_LEN > APPCFG_USERNAME_LEN)
#error "The FTP Username length can not be shorter then the APPCFG Username length!"
#endif


///////////////////////////////////////////////////////////////////////////////
// FTPVerify
//

BOOL FTPVerify(char *login, char *password)
{
#if (DEBUG_MAIN >= LOG_INFO)
	debugPutMsg(4); //@mxd:4:Received FTP Login (%s) and Password (%s)
    debugPutString(login);
    debugPutString(password);
#endif

    if (strcmpee2ram(login, APPCFG_USERNAME0) == 0) {
        if (strcmpee2ram(password, APPCFG_PASSWORD0) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}
#endif


///////////////////////////////////////////////////////////////////////////////
// InitializeBoard
//
// Initialize the boards hardware
//
 
static void InitializeBoard(void)
{
	
#if (defined(MCHP_C18) && (defined(__18F458) || defined(__18F448))) \
		|| (defined(HITECH_C18) && (defined(_18F458) || defined(_18F448)))
	CMCON  = 0x07; /* Comparators off CM2:CM0 = 111 for PIC 18F448 & 18F458 */
#endif


    /////////////////////////////////////////////////
    //Enable 4 x PLL if configuration bits are set to do so
#if ( defined(MCHP_C18) && (defined(__18F6621 ) || defined(__18F6680 )))
    OSCCON = 0x04;              // Enable PLL (PLLEN = 1)
    while (OSCCON_LOCK == 0);   // Wait until PLL is stable (LOCK = 1)
    
    // Seeing that this code does currently not work with Hi-Tech compiler, disable this feature
    OSCCON_SCS1 = 1;
    
    //Switch on software 4 x PLL if flag is set in configuration data
    //if (appcfgGetc(APPCFG_SYSFLAGS) & APPCFG_SYSFLAGS_PLLON) {
    //    OSCCON_SCS1 = 1;
    //}
#endif

#if defined(_18F6680) || defined(__18F6680)    
	vscp18f_init( FALSE );

	// Enable peripheral interrupt	
	INTCONbits.PEIE = 1;
	
	// EnableCAN Receive Error Interrupt
	PIE3bits.IRXIE = 1;
#endif    
	// Disable external pull-ups
    INTCON2_RBPU = 1;

    // Enable interrupts
    T0CON = 0;
    INTCON_GIEH = 1;
    INTCON_GIEL = 1;
}
