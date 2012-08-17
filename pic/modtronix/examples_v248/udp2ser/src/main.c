/*********************************************************************
*
*       UDP to serial interface converter example program
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


//////////////////////////////////////////////////////////////////////
/**
********************************************************************/


/*
 * Following define uniquely deines this file as main entry/application in whole
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
//Version number
// - n = major part, and can be 1 or 2 digets long
// - mm is minor part, and must be 2 digets long!
ROM char APP_VER_STR[] = "V3.00";


BYTE myDHCPBindCount = 0;
#if defined(STACK_USE_DHCP)
    extern BYTE DHCPBindCount;
#else
    //If DHCP is not enabled, force DHCP update.
    BYTE DHCPBindCount = 1;
#endif

static void InitializeBoard(void);
static void ProcessIO(void);


//Transmit and receive buffers for I2C
/*
static BYTE txBufI2C[256];
static BYTE rxBufI2C[256];

//Transmit and receive buffers for UDP
static BYTE txBufUDP[256];
static BYTE rxBufUDP[256];

//Transmit and receive buffers for SPI
static BYTE txBufSPI[256];
static BYTE rxBufSPI[256];
*/


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
    static TICK8 t = 0;
    BYTE i;
    char strBuf[10];

    //Initialize AppConfig structure
    appcfgInit();
    
    /*
     * Initialize any application specific hardware.
     */
    InitializeBoard();

    /*
     * Initialize all stack related components.
     * Following steps must be performed for all applications using
     * PICmicro TCP/IP Stack.
     */
    TickInit();

    /*
     * Initialize file system.
     */
    fsysInit();

    //Initialze serial port
    serInit();
    
    StackInit();

#if defined(STACK_USE_HTTP_SERVER)
    HTTPInit();
#endif

#if defined(STACK_USE_FTP_SERVER)
    FTPInit();
#endif

#if defined(STACK_USE_DHCP) || defined(STACK_USE_IP_GLEANING)
    //To disable DHCP in software, execute this code. This will asigne the default IP address to the module
    if (1)
    {
        //Force IP address display update.
        myDHCPBindCount = 1;
        
        #if defined(STACK_USE_DHCP)
        DHCPDisable();
        #endif
    }
#endif

    #if (DEBUG_MAIN >= LOG_DEBUG)
        debugPutMsg(1); //@mxd:1:Starting main loop
    #endif
    
    /*
     * Once all items are initialized, go into infinite loop and let
     * stack items execute their tasks.
     * If application needs to perform its own task, it should be
     * done at the end of while loop.
     * Note that this is a "co-operative mult-tasking" mechanism
     * where every task performs its tasks (whether all in one shot
     * or part of it) and returns so that other tasks can do their
     * job.
     * If a task needs very long time to do its job, it must broken
     * down into smaller pieces so that other tasks can have CPU time.
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

        //This task performs normal stack task including checking for incoming packet,
        //type of packet and calling appropriate stack entity to process it.
        StackTask();

#if defined(STACK_USE_HTTP_SERVER)
        //This is a TCP application.  It listens to TCP port 80
        //with one or more sockets and responds to remote requests.
        HTTPServer();
#endif

#if defined(STACK_USE_FTP_SERVER)
        FTPServer();
#endif

        //Add your application speicifc tasks here.
        ProcessIO();

        //For DHCP information, display how many times we have renewed the IP
        //configuration since last reset.
        if ( DHCPBindCount != myDHCPBindCount )
        {
            #if (DEBUG_MAIN >= LOG_INFO)
                debugPutMsg(2); //@mxd:2:DHCP Bind Count = %D
                debugPutByteHex(DHCPBindCount);
            #endif
            
            //Display new IP address
            #if (DEBUG_MAIN >= LOG_INFO)
                debugPutMsg(3); //@mxd:3:DHCP complete, IP = %D.%D.%D.%D
                debugPutByteHex(MY_IP_BYTE1);
                debugPutByteHex(MY_IP_BYTE2);
                debugPutByteHex(MY_IP_BYTE3);
                debugPutByteHex(MY_IP_BYTE4);
            #endif
            myDHCPBindCount = DHCPBindCount;
        }
    }
}


static void ProcessIO(void)
{
}

/////////////////////////////////////////////////
//Implement callback for FTPVerify() function
#if defined(STACK_USE_FTP_SERVER)

BOOL FTPVerify(char *login, char *password)
{
    #if (DEBUG_MAIN >= LOG_INFO)
        debugPutMsg(4); //@mxd:4:Received FTP Login (%s) and Password (%s)
        debugPutString(login);
        debugPutString(password);
    #endif

    //FTP Username and Password is "admin" and "pw"
    if (strcmppgm2ram(login, (ROM char*)"admin") == 0) {
        if (strcmppgm2ram(password, (ROM char*)"pw") == 0) {
            return TRUE;
        }
    }
    return FALSE;
}
#endif


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
    //#define BAUD 1040     /* 9600 BAUD at 40MHz clock */
    //#define BAUD 520      /* 19200 BAUD at 40MHz clock */
    //#define BAUD 259      /* 38400 BAUD at 40MHz clock */
    #define BAUD 172        /* 57600 BAUD at 40MHz clock */
    //#define BAUD 86       /* 115200 BAUD at 40MHz clock */
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

/**
 * This function is a "callback" from HTTPServer task. Whenever a remote node performs interactive
 * GET task on page that was served, HTTPServer calls this functions. Use HTTPGetParam()
 * to get all name-value parameters.
 *
 * @param httpInfo  Socket that is currently receiving this HTTP command
 * @param rqstRes   Name of the Requested resource - GET command's action. All characters are
 *                  in uppercase!
 */
void HTTPExecGetCmd(HTTP_INFO* httpInfo, BYTE* rqstRes)
{
    BYTE param[20];

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

    } while (moreParams);
}


WORD HTTPGetVar(HTTP_INFO* httpInfo, BYTE* val)
{
    BYTE varValue, varGroup, ref;

    varValue = httpInfo->var.get.tagVal;        //Variable Value of requested variable
    varGroup = httpInfo->var.get.tagGroup;      //Variable Group of requested variable
    ref      = httpInfo->var.get.varRef;        //Current callback reference with respect to 'var' variable.
    
    //In case requested var not found, set it to NULL character and return HTTP_END_OF_VAR
    *val = '\0';    
}
