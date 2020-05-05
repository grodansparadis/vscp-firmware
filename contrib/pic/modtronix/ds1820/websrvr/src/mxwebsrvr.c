/*********************************************************************
*
*       Modtronix Example Web Server Application using Microchip TCP/IP Stack
*
*********************************************************************
* FileName:        WebSrvr.c
* Dependencies:    string.H
*                  usart.h
*                  StackTsk.h
*                  Tick.h
*                  http.h
*                  MPFS.h
* Processor:       PIC18
* Complier:        MCC18 v1.00.50 or higher
*                  HITECH PICC-18 V8.10PL1 or higher
* Company:         Microchip Technology, Inc.
*
* Software License Agreement
*
* The software supplied herewith by Microchip Technology Incorporated
* (the “Company”) for its PICmicro® Microcontroller is intended and
* supplied to you, the Company’s customer, for use solely and
* exclusively on Microchip PICmicro Microcontroller products. The
* software is owned by the Company and/or its supplier, and is
* protected under applicable copyright laws. All rights are reserved.
* Any use in violation of the foregoing restrictions may subject the
* user to criminal sanctions under applicable laws, as well as to
* civil liability for the breach of the terms and conditions of this
* license.
*
* THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
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
* Author               Date         Comment
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* David Hosken         15/02/04     Original (Rev. 1.0)
 ********************************************************************/

/*
 * Following define uniquely deines this file as main
 * entry/application In whole project, there should only be one such
 * definition and application file must define AppConfig variable as
 * described below.
 */
#define THIS_IS_STACK_APPLICATION

#define BAUD_RATE       (19200)     // bps

#define USART_USE_BRGH_LOW
#if defined(USART_USE_BRGH_LOW)
    #define SPBRG_VAL   ( ((CLOCK_FREQ/BAUD_RATE)/64) - 1)
#else
    #define SPBRG_VAL   ( ((CLOCK_FREQ/BAUD_RATE)/16) - 1)
#endif

#if SPBRG_VAL > 255
    #error "Calculated SPBRG value is out of range for currnet CLOCK_FREQ."
#endif



#include <string.h>
#include "ds1820.h"
#include "delay_ds.h"

/*
 * These headers must be included for required defs.
 */
#include "stacktsk.h"
#include "tick.h"

#if defined(STACK_USE_DHCP)
#include "dhcp.h"
#endif

#if defined(STACK_USE_HTTP_SERVER)
#include "http.h"
#endif

#include "mpfs.h"

#if defined(STACK_USE_FTP_SERVER) && defined(MPFS_USE_EEPROM)
#include "ftp.h"
#endif

#if defined(APP_USE_LCD)
#include "xlcd.h"
#endif

#if defined(MPFS_USE_EEPROM)
#include "xeeprom.h"
#endif


// For debug only.
#include "tcp.h"
#include "icmp.h"

#include "delay.h"

#define STARTUP_MSG "MpStack 2.20.04"

ROM char StartupMsg[] = STARTUP_MSG;

#if defined(STACK_USE_DHCP) || defined(STACK_USE_IP_GLEANING)
ROM char DHCPMsg[] = "DHCP/Gleaning...";
#endif

ROM char SetupMsg[] = "Board Setup...";

/*
 * This is used by other stack elements.
 * Main application must define this and initialize it with
 * proper values.
 */
APP_CONFIG AppConfig;

BYTE myDHCPBindCount = 0;
#if defined(STACK_USE_DHCP)
    extern BYTE DHCPBindCount;
#else
    /*
     * If DHCP is not enabled, force DHCP update.
     */
    BYTE DHCPBindCount = 1;
#endif

#if defined(MCHP_C18) && defined(__18F8720)
    #define TXSTAbits       TXSTA1bits
    #define TXREG           TXREG1
    #define TXSTA           TXSTA1
    #define RCSTA           RCSTA1
    #define SPBRG           SPBRG1
    #define RCREG           RCREG1
#endif


/*
 * Set configuration fuses for HITECH compiler.
 * For MCC18 compiler, separately linked config.asm file
 * will set the correct fuses.
 */
#if defined(HITECH_C18)
__CONFIG(1, UNPROTECT & HS);
__CONFIG(2, PWRTEN & BORDIS & WDTDIS);
__CONFIG(4, DEBUGDIS & LVPDIS & STVREN);
__CONFIG(5, UNPROTECT);
#endif


/*
 * Private helper functions.
 * These may or may not be present in all applications.
 */
static void InitAppConfig(void);

static void InitializeBoard(void);
static void ProcessIO(void);

void NotifyRemoteUser(void);
static void DisplayIPValue(IP_ADDR *IPVal, BOOL bToLCD);
static void SetConfig(void);


#if defined(MPFS_USE_EEPROM)
static BOOL DownloadMPFS(void);
static void SaveAppConfig(void);
#endif

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
    TickUpdate();

#if defined(STACK_USE_SLIP)
    MACISR();
#endif
}

#if defined(MCHP_C18)
#pragma code highVector=0x08
void HighVector (void)
{
    _asm goto HighISR _endasm
}
#pragma code /* return to default code section */
#endif



static void USARTPut(BYTE c)
{
    while( !TXSTA_TRMT);
    TXREG = c;
}

static void USARTPutString(BYTE *s)
{
    BYTE c;

    while( (c = *s++) )
        USARTPut(c);
}

void USARTPutROMString(ROM char* str)
{
    BYTE v;

    while( v = *str++ )
        USARTPut(v);
}

#define USARTIsGetReady()   (PIR1_RCIF)
#define USARTGet()          (RCREG)

ROM char PressKeyForConfig[] = "\r\nPress any key for Configuration menu.";

BYTE heater_started = 0;
/*
 * Main entry point.
 */
void main(void)
{
    static TICK t = 0;
    BYTE c, i;

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
     * Following steps must be performed for all applications using
     * PICmicro TCP/IP Stack.
     */
    MPFSInit();


    /*
     * Initialize Stack and application related NV variables.
     */
    InitAppConfig();


    /*
     * Depending on whether internal program memor is used or external
     * EEPROM is used, keep/remove these block.
     */

    /*
     * Wait a couple of seconds for user input.
	 * - If something is detected, start config.
	 * - If nothing detected, start main program.
     */
	USARTPutROMString(PressKeyForConfig);
	for (i = 60; i > 0; --i)	//Delay for 50mS x 60 = 3 sec
	{
		if ((i % 8) == 0) USARTPut('.');
		if (USARTIsGetReady())
		{
			#if defined(APP_USE_LCD)
	       	XLCDGoto(1, 0);
	       	XLCDPutROMString(SetupMsg);
			#endif
	
	        SetConfig();
			break;
		}
		DelayMs(50);
	}
	USARTPut('\r');
	USARTPut('\n');

    StackInit();

#if defined(STACK_USE_HTTP_SERVER)
    HTTPInit();
#endif

#if defined(STACK_USE_FTP_SERVER) && defined(MPFS_USE_EEPROM)
    FTPInit();
#endif


#if defined(STACK_USE_DHCP) || defined(STACK_USE_IP_GLEANING)
    if ( AppConfig.Flags.bIsDHCPEnabled )
    {
		#if defined(APP_USE_LCD)
        XLCDGoto(1, 0);
   	    XLCDPutROMString(DHCPMsg);
		#endif
    }
    else
    {
        /*
         * Force IP address display update.
         */
        myDHCPBindCount = 1;
#if defined(STACK_USE_DHCP)
        DHCPDisable();
#endif
    }
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
        //Turn off the heater after about 1min to prevent over heating
        if(heater_started ==1)
        {
            if ( TickGetDiff(TickGet(), t) >= 9000 )
            {
                LATB4 = 0;
    			heater_started = 0;
            }   
        }else
		{
		    t = TickGet();
		}
         
        /*
         * This task performs normal stack task including checking
         * for incoming packet, type of packet and calling
         * appropriate stack entity to process it.
         */
        StackTask();

#if defined(STACK_USE_HTTP_SERVER)
        /*
         * This is a TCP application.  It listens to TCP port 80
         * with one or more sockets and responds to remote requests.
         */
        HTTPServer();
#endif

#if defined(STACK_USE_FTP_SERVER) && defined(MPFS_USE_EEPROM)
        FTPServer();
#endif

        /*
         * In future, as new TCP/IP applications are written, it
         * will be added here as new tasks.
         */

         /*
          * Add your application speicifc tasks here.
          */
        ProcessIO();

        /*
         * For DHCP information, display how many times we have renewed the IP
         * configuration since last reset.
         */
        if ( DHCPBindCount != myDHCPBindCount )
        {
            DisplayIPValue(&AppConfig.MyIPAddr, TRUE);
            myDHCPBindCount = DHCPBindCount;

			#if defined(APP_USE_LCD)
            if ( AppConfig.Flags.bIsDHCPEnabled )
            {
                XLCDGoto(1, 14);
   	            if ( myDHCPBindCount < 0x0a )
       	            XLCDPut(myDHCPBindCount + '0');
           	    else
               	    XLCDPut(myDHCPBindCount + 'A');
            }
			#endif
        }

    }
}

#if defined(APP_USE_LCD)
//                         1234567890123456
ROM char blankLCDLine[] = "                ";
#endif

static void DisplayIPValue(IP_ADDR *IPVal, BOOL bToLCD)
{
    char IPDigit[8];

	#if defined(APP_USE_LCD)
    if ( bToLCD )
    {
        /*
         * Erase second line.
         */
        XLCDGoto(1, 0);
   	    XLCDPutROMString(blankLCDLine);
    }
	#endif

    /*
     * Rewrite the second line.
     */
	#if defined(APP_USE_LCD)
    XLCDGoto(1, 0);
	#endif

    itoa(IPVal->v[0], IPDigit);
	#if defined(APP_USE_LCD)
    if ( bToLCD )
    {
        XLCDPutString(IPDigit);
        XLCDPut('.');
    }
    else
	#endif
    {
        USARTPutString((BYTE*)IPDigit);
        USARTPut('.');
    }

    itoa(IPVal->v[1], IPDigit);
	#if defined(APP_USE_LCD)
    if ( bToLCD )
    {
        XLCDPutString(IPDigit);
        XLCDPut('.');
    }
    else
	#endif
    {
        USARTPutString((BYTE*)IPDigit);
        USARTPut('.');
    }

    itoa(IPVal->v[2], IPDigit);
	#if defined(APP_USE_LCD)
    if ( bToLCD )
    {
        XLCDPutString(IPDigit);
        XLCDPut('.');
    }
    else
	#endif
    {
        USARTPutString((BYTE*)IPDigit);
        USARTPut('.');
    }

    itoa(IPVal->v[3], IPDigit);
	#if defined(APP_USE_LCD)
    if ( bToLCD )
        XLCDPutString(IPDigit);
    else
	#endif
	{
        USARTPutString((BYTE*)IPDigit);
	}
}



static char AN0String[8];
static char AN1String[8];


static void ProcessIO(void)
{
	char i;
	char temp_buff[10];
	int read_temp;
	float real_temp,temp_count;

    //Get raw data from DS1820  
    convert(temp_buff);

    //Convert raw data to degree C in accorance with datasheet
	read_temp=((temp_buff[1]<<8) | temp_buff[0]) >> 1;
	temp_count=(float)(temp_buff[7] - temp_buff[6])/(float)temp_buff[7];
	real_temp = ((float)read_temp-0.25)+temp_count;

	sprintf(AN1String,"%2.2f",real_temp);

	USARTPutString((BYTE*)AN1String);
	USARTPut(0x0D);
	USARTPut(0x0a);
	

}

/*
 * CGI Command Codes.
 */
#define CGI_CMD_DIGOUT      (0)
#define CGI_CMD_LCDOUT      (1)

/*
 * CGI Variable codes. - There could be 0-255 variables.
 */
#define FAN	                (0)
#define HEATER              (1)
#define VAR_ANAIN_AN0       (2)
#define VAR_ANAIN_AN1       (3)
#define VAR_RA2             (4)
#define VAR_STROUT_LCD      (5)

/*********************************************************************
 * Function:        void HTTPExecCmd(BYTE** argv, BYTE argc)
 *
 * PreCondition:    None
 *
 * Input:           argv        - List of arguments
 *                  argc        - Argument count.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a "callback" from HTTPServer
 *                  task.  Whenever a remote node performs
 *                  interactive task on page that was served,
 *                  HTTPServer calls this functions with action
 *                  arguments info.
 *                  Main application should interpret this argument
 *                  and act accordingly.
 *
 *                  Following is the format of argv:
 *                  If HTTP action was : thank.htm?name=Joe&age=25
 *                      argv[0] => thank.htm
 *                      argv[1] => name
 *                      argv[2] => Joe
 *                      argv[3] => age
 *                      argv[4] => 25
 *
 *                  Use argv[0] as a command identifier and rests
 *                  of the items as command arguments.
 *
 * Note:            THIS IS AN EXAMPLE CALLBACK.
 ********************************************************************/
#if defined(STACK_USE_HTTP_SERVER)

ROM char COMMANDS_OK_PAGE[] = "COMMANDS.CGI";

// Copy string with NULL termination.
#define COMMANDS_OK_PAGE_LEN  (sizeof(COMMANDS_OK_PAGE))

ROM char CMD_UNKNOWN_PAGE[] = "INDEX.HTM";

// Copy string with NULL termination.
#define CMD_UNKNOWN_PAGE_LEN    (sizeof(CMD_UNKNOWN_PAGE))

void HTTPExecCmd(BYTE** argv, BYTE argc)
{
    BYTE command;
    BYTE var;

    /*
     * Design your pages such that they contain command code
     * as a one character numerical value.
     * Being a one character numerical value greatly simplifies
     * the job.
     */
    command = argv[0][0] - '0';

    /*
     * Find out the cgi file name and interpret parameters
     * accordingly
     */
    switch(command)
    {
    case CGI_CMD_DIGOUT:
        /*
         * This DIGOUTS.CGI.  Any arguments with this file
         * must be about controlling digital outputs.
         */

        /*
         * Identify the parameters.
         * Compare it in upper case format.
         */
        var = argv[1][0] - '0';

        switch(var)
        {
        case HEATER:
            /*
             * Toggle RC2.
             */
            if(LATB4)
            {
                LATB4 = 0;
			}
            else
            {
                LATB4 = 1; 
                heater_started = 1;   
            }
            //LATB4 ^= 1;
            break;

        case FAN:
            /*
             * Toggle RC5.
             */
            LATB5 ^= 1;
            break;
         }

         memcpypgm2ram((void*)argv[0],
              (ROM void*)COMMANDS_OK_PAGE, COMMANDS_OK_PAGE_LEN);
         break;

    case CGI_CMD_LCDOUT:
        /*
         * Note implemented.
         */
        break;

    default:
        memcpypgm2ram((void*)argv[0],
              (ROM void*)CMD_UNKNOWN_PAGE, CMD_UNKNOWN_PAGE_LEN);
        break;
    }

}
#endif


/*********************************************************************
 * Function:        WORD HTTPGetVar(BYTE var, WORD ref, BYTE* val)
 *
 * PreCondition:    None
 *
 * Input:           var         - Variable Identifier
 *                  ref         - Current callback reference with
 *                                respect to 'var' variable.
 *                  val         - Buffer for value storage.
 *
 * Output:          Variable reference as required by application.
 *
 * Side Effects:    None
 *
 * Overview:        This is a callback function from HTTPServer() to
 *                  main application.
 *                  Whenever a variable substitution is required
 *                  on any html pages, HTTPServer calls this function
 *                  8-bit variable identifier, variable reference,
 *                  which indicates whether this is a first call or
 *                  not.  Application should return one character
 *                  at a time as a variable value.
 *
 * Note:            Since this function only allows one character
 *                  to be returned at a time as part of variable
 *                  value, HTTPServer() calls this function
 *                  multiple times until main application indicates
 *                  that there is no more value left for this
 *                  variable.
 *                  On begining, HTTPGetVar() is called with
 *                  ref = HTTP_START_OF_VAR to indicate that
 *                  this is a first call.  Application should
 *                  use this reference to start the variable value
 *                  extraction and return updated reference.  If
 *                  there is no more values left for this variable
 *                  application should send HTTP_END_OF_VAR.  If
 *                  there are any bytes to send, application should
 *                  return other than HTTP_START_OF_VAR and
 *                  HTTP_END_OF_VAR reference.
 *
 *                  THIS IS AN EXAMPLE CALLBACK.
 *                  MODIFY THIS AS PER YOUR REQUIREMENTS.
 ********************************************************************/
#if defined(STACK_USE_HTTP_SERVER)
WORD HTTPGetVar(BYTE var, WORD ref, BYTE* val)
{
    /*
     * First of all identify variable.
     */
    switch(var)
    {
    case HEATER:
        if ( LATB4 )
            *val = '1';
        else
            *val = '0';
        break;

    case FAN:
        if ( LATB5 )
            *val = '1';
        else
            *val = '0';
        break;

    case VAR_ANAIN_AN0:
        if ( ref == HTTP_START_OF_VAR )
        {
            ref = (BYTE)0;
        }
        *val = AN0String[(BYTE)ref];
        if ( AN0String[(BYTE)ref] == '\0' )
            return HTTP_END_OF_VAR;

        (BYTE)ref++;
        return ref;

    case VAR_ANAIN_AN1:
        if ( ref == HTTP_START_OF_VAR )
        {
            ref = (BYTE)0;
        }
        *val = AN1String[(BYTE)ref];
        if ( AN1String[(BYTE)ref] == '\0' )
            return HTTP_END_OF_VAR;

        (BYTE)ref++;
        return ref;

    case VAR_RA2:
        if ( PORTA_RA2 )
            *val = '1';
        else
            *val = '0';
        break;
    }

    return HTTP_END_OF_VAR;
}
#endif


#if defined(STACK_USE_FTP_SERVER) && defined(MPFS_USE_EEPROM)
ROM char FTP_USER_NAME[]    = "ftp";
#undef FTP_USER_NAME_LEN
#define FTP_USER_NAME_LEN   (sizeof(FTP_USER_NAME)-1)

ROM char FTP_USER_PASS[]    = "microchip";
#define FTP_USER_PASS_LEN   (sizeof(FTP_USER_PASS)-1)

BOOL FTPVerify(char *login, char *password)
{
    if ( !memcmppgm2ram(login, (ROM void*)FTP_USER_NAME, FTP_USER_NAME_LEN) )
    {
        if ( !memcmppgm2ram(password, (ROM void*)FTP_USER_PASS, FTP_USER_PASS_LEN) )
            return TRUE;
    }
    return FALSE;
}
#endif




/*********************************************************************
 * Function:        void InitializeBoard(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initialize board specific hardware.
 *
 * Note:            None
 ********************************************************************/
static void InitializeBoard(void)
{
	#if (defined(MCHP_C18) && (defined(__18F458) || defined(__18F448))) \
		|| (defined(HITECH_C18) && (defined(_18F458) || defined(_18F448)))
		CMCON  = 0x07; /* Comparators off CM2:CM0 = 111 for PIC 18F448 & 18F458 */
	#endif

    /*
     * Disable ADC, use PORTA as digital outputs.
     */
	ADCON1 = 0x07;
	TRISA  = 0xd0;	//RA0-3 and RA5 = outputs

	#if defined(APP_USE_LCD)
    /*
     * LCD is enabled using RA5.
     */
    //PORTA_RA5 = 0;          // Disable LCD.	//mx disabled - is not valid for SBC45EC!!!
	#endif

    /*
     * Turn off the LED's.
     */
    //LATA2 = 1;
    //LATA3 = 1;

	//Ports C2 and C5 are configured as outputs
	TRISC_RC2 = 0;
	TRISC_RC5 = 0;

	TRISB4 = 0;
	TRISB5 = 0;

	LATB4 = 0;
	LATB5 = 0 ;
    /*
     * External data EEPROM needs pull-ups, so enable internal
     * pull-ups.
     */
    INTCON2_RBPU = 0;

	#if defined(APP_USE_LCD)
    XLCDInit();
    XLCDGoto(0, 0);
    XLCDPutROMString(StartupMsg);
	#endif


    TXSTA = 0b00100000;     // Low BRG speed
    RCSTA = 0b10010000;
    SPBRG = SPBRG_VAL;

    T0CON = 0;
    INTCON_GIEH = 1;
    INTCON_GIEL = 1;

}

/*********************************************************************
 * Function:        void InitAppConfig(void)
 *
 * PreCondition:    MPFSInit() is already called.
 *
 * Input:           None
 *
 * Output:          Write/Read non-volatile config variables.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
static void InitAppConfig(void)
{
#if defined(MPFS_USE_EEPROM)
    BYTE c;
    BYTE *p;
#endif

    /*
     * Load default configuration into RAM.
     */
    AppConfig.MyIPAddr.v[0]     = MY_DEFAULT_IP_ADDR_BYTE1;
    AppConfig.MyIPAddr.v[1]     = MY_DEFAULT_IP_ADDR_BYTE2;
    AppConfig.MyIPAddr.v[2]     = MY_DEFAULT_IP_ADDR_BYTE3;
    AppConfig.MyIPAddr.v[3]     = MY_DEFAULT_IP_ADDR_BYTE4;

    AppConfig.MyMask.v[0]       = MY_DEFAULT_MASK_BYTE1;
    AppConfig.MyMask.v[1]       = MY_DEFAULT_MASK_BYTE2;
    AppConfig.MyMask.v[2]       = MY_DEFAULT_MASK_BYTE3;
    AppConfig.MyMask.v[3]       = MY_DEFAULT_MASK_BYTE4;

    AppConfig.MyGateway.v[0]    = MY_DEFAULT_GATE_BYTE1;
    AppConfig.MyGateway.v[1]    = MY_DEFAULT_GATE_BYTE2;
    AppConfig.MyGateway.v[2]    = MY_DEFAULT_GATE_BYTE3;
    AppConfig.MyGateway.v[3]    = MY_DEFAULT_GATE_BYTE4;

    AppConfig.MyMACAddr.v[0]    = MY_DEFAULT_MAC_BYTE1;
    AppConfig.MyMACAddr.v[1]    = MY_DEFAULT_MAC_BYTE2;
    AppConfig.MyMACAddr.v[2]    = MY_DEFAULT_MAC_BYTE3;
    AppConfig.MyMACAddr.v[3]    = MY_DEFAULT_MAC_BYTE4;
    AppConfig.MyMACAddr.v[4]    = MY_DEFAULT_MAC_BYTE5;
    AppConfig.MyMACAddr.v[5]    = MY_DEFAULT_MAC_BYTE6;

#if defined(STACK_USE_DHCP) || defined(STACK_USE_IP_GLEANING)
    AppConfig.Flags.bIsDHCPEnabled = TRUE;
#else
    AppConfig.Flags.bIsDHCPEnabled = FALSE;
#endif

#if defined(MPFS_USE_EEPROM)
    p = (BYTE*)&AppConfig;


    XEEBeginRead(EEPROM_CONTROL, 0x00);
    c = XEERead();
    XEEEndRead();

    /*
     * When a record is saved, first byte is written as 0x55 to indicate
     * that a valid record was saved.
     */
    if ( c == 0x55 )
    {
        XEEBeginRead(EEPROM_CONTROL, 0x01);
        for ( c = 0; c < sizeof(AppConfig); c++ )
            *p++ = XEERead();
        XEEEndRead();
    }
    else
        SaveAppConfig();
#endif
}

#if defined(MPFS_USE_EEPROM)
static void SaveAppConfig(void)
{
    BYTE c;
    BYTE *p;

    p = (BYTE*)&AppConfig;
    XEEBeginWrite(EEPROM_CONTROL, 0x00);
    XEEWrite(0x55);
    for ( c = 0; c < sizeof(AppConfig); c++ )
    {
        XEEWrite(*p++);
    }

    XEEEndWrite();
}
#endif

ROM char menu[] =
    "\r\n\r\n\r\MxWebSrvr Demo Application v1.0 ("STARTUP_MSG", " __DATE__ ")\r\n\r\n"

    "\t1: Change Board serial number.\r\n"
    "\t2: Change default IP address.\r\n"
    "\t3: Change default gateway address.\r\n"
    "\t4: Change default subnet mask.\r\n"
    "\t5: Enable DHCP & IP Gleaning.\r\n"
    "\t6: Disable DHCP & IP Gleaning.\r\n"
    "\t7: Download MPFS image.\r\n"
    "\t8: Save & Quit.\r\n"
    "\r\n"
    "Enter a menu choice (1-8): ";

typedef enum _MENU_CMD
{
    MENU_CMD_SERIAL_NUMBER          = '1',
    MENU_CMD_IP_ADDRESS,
    MENU_CMD_GATEWAY_ADDRESS,
    MENU_CMD_SUBNET_MASK,
    MENU_CMD_ENABLE_AUTO_CONFIG,
    MENU_CMD_DISABLE_AUTO_CONFIG,
    MENU_CMD_DOWNLOAD_MPFS,
    MENU_CMD_QUIT,
    MENU_CMD_INVALID
} MENU_CMD;

ROM char* menuCommandPrompt[] =
{
    "\r\nSerial Number (",
    "\r\nDefault IP Address (",
    "\r\nDefault Gateway Address (",
    "\r\nDefault Subnet Mask (",
    "\r\nDHCP & IP Gleaning enabled.\r\n",
    "\r\nDHCP & IP Gleaning disabled.\r\n",
    "\r\nReady to download MPFS image - Use Xmodem protocol.\r\n",
    "\r\nNow running application..."
};

ROM char InvalidInputMsg[] = "\r\nInvalid input received - Input ignored.\r\n"
                             "Press any key to continue...\r\n";

BYTE USARTGetString(char *buffer, BYTE bufferLen)
{
    BYTE v;
    BYTE count;

    count = 0;
    do
    {
        while( !USARTIsGetReady() );

        v = USARTGet();

        if ( v == '\r' || v == '\n' )
            break;

        count++;
        *buffer++ = v;
        *buffer = '\0';
        if ( bufferLen-- == 0 )
            break;
    } while(1);
    return count;
}

BOOL StringToIPAddress(char *str, IP_ADDR *buffer)
{
    BYTE v;
    char *temp;
    BYTE byteIndex;

    temp = str;
    byteIndex = 0;

    while( v = *str )
    {
        if ( v == '.' )
        {
            *str++ = '\0';
            buffer->v[byteIndex++] = atoi(temp);
            temp = str;
        }
        else if ( v < '0' || v > '9' )
            return FALSE;

        str++;
    }

    buffer->v[byteIndex] = atoi(temp);

    return (byteIndex == 3);
}



MENU_CMD GetMenuChoice(void)
{
    BYTE c;

    while ( !USARTIsGetReady() );

    c = USARTGet();

    if ( c >= '1' && c < MENU_CMD_INVALID )
        return c;
    else
        return MENU_CMD_INVALID;
}

#define MAX_USER_RESPONSE_LEN   (20)
void ExecuteMenuChoice(MENU_CMD choice)
{
    char response[MAX_USER_RESPONSE_LEN];
    IP_ADDR tempIPValue;
    IP_ADDR *destIPValue;

    USARTPut('\r');
    USARTPut('\n');
    USARTPutROMString(menuCommandPrompt[choice-'0'-1]);

    switch(choice)
    {
    case MENU_CMD_SERIAL_NUMBER:
        itoa(AppConfig.SerialNumber.Val, response);
        USARTPutString((BYTE*)response);
        USARTPut(')');
        USARTPut(':');
        USARTPut(' ');

        if ( USARTGetString(response, sizeof(response)) )
        {
            AppConfig.SerialNumber.Val = atoi(response);

            AppConfig.MyMACAddr.v[4] = AppConfig.SerialNumber.v[1];
            AppConfig.MyMACAddr.v[5] = AppConfig.SerialNumber.v[0];
        }
        else
            goto HandleInvalidInput;

        break;

    case MENU_CMD_IP_ADDRESS:
        destIPValue = &AppConfig.MyIPAddr;
        goto ReadIPConfig;

    case MENU_CMD_GATEWAY_ADDRESS:
        destIPValue = &AppConfig.MyGateway;
        goto ReadIPConfig;

    case MENU_CMD_SUBNET_MASK:
        destIPValue = &AppConfig.MyMask;

    ReadIPConfig:
        DisplayIPValue(destIPValue, FALSE);
        USARTPut(')');
        USARTPut(':');
        USARTPut(' ');

        USARTGetString(response, sizeof(response));

        if ( !StringToIPAddress(response, &tempIPValue) )
        {
HandleInvalidInput:
            USARTPutROMString(InvalidInputMsg);
            while( !USARTIsGetReady() );
            USARTGet();
        }
        else
        {
            destIPValue->Val = tempIPValue.Val;
        }
        break;


    case MENU_CMD_ENABLE_AUTO_CONFIG:
        AppConfig.Flags.bIsDHCPEnabled = TRUE;
        break;

    case MENU_CMD_DISABLE_AUTO_CONFIG:
        AppConfig.Flags.bIsDHCPEnabled = FALSE;
        break;

    case MENU_CMD_DOWNLOAD_MPFS:
#if defined(MPFS_USE_EEPROM)
        DownloadMPFS();
#endif
        break;

    case MENU_CMD_QUIT:
#if defined(MPFS_USE_EEPROM)
        SaveAppConfig();
#endif
        break;
    }
}




static void SetConfig(void)
{
    MENU_CMD choice;

    do
    {
        USARTPutROMString(menu);
        choice = GetMenuChoice();
        if ( choice != MENU_CMD_INVALID )
            ExecuteMenuChoice(choice);
    } while(choice != MENU_CMD_QUIT);

}


#if defined(MPFS_USE_EEPROM)

/*********************************************************************
 * Function:        BOOL DownloadMPFS(void)
 *
 * PreCondition:    MPFSInit() is already called.
 *
 * Input:           None
 *
 * Output:          TRUE if successful
 *                  FALSE otherwise
 *
 * Side Effects:    This function uses 128 bytes of Bank 4 using
 *                  indirect pointer.  This requires that no part of
 *                  code is using this block during or before calling
 *                  this function.  Once this function is done,
 *                  that block of memory is available for general use.
 *
 * Overview:        This function implements XMODEM protocol to
 *                  be able to receive a binary file from PC
 *                  applications such as HyperTerminal.
 *
 * Note:            In current version, this function does not
 *                  implement user interface to set IP address and
 *                  other informations.  User should create their
 *                  own interface to allow user to modify IP
 *                  information.
 *                  Also, this version implements simple user
 *                  action to start file transfer.  User may
 *                  evaulate its own requirement and implement
 *                  appropriate start action.
 *
 ********************************************************************/
#define XMODEM_SOH      0x01
#define XMODEM_EOT      0x04
#define XMODEM_ACK      0x06
#define XMODEM_NAK      0x15
#define XMODEM_CAN      0x18
#define XMODEM_BLOCK_LEN 128

static BOOL DownloadMPFS(void)
{
    enum SM_MPFS
    {
        SM_MPFS_SOH,
        SM_MPFS_BLOCK,
        SM_MPFS_BLOCK_CMP,
        SM_MPFS_DATA,
    } state;

    BYTE c;
    MPFS handle;
    BOOL lbDone;
    BYTE blockLen;
    BYTE lResult;
    BYTE tempData[XMODEM_BLOCK_LEN];
    TICK lastTick;
    TICK currentTick;

    state = SM_MPFS_SOH;
    lbDone = FALSE;

    handle = MPFSFormat();

    /*
     * Notify the host that we are ready to receive...
     */
    lastTick = TickGet();
    do
    {
        /*
         * Update tick here too - just in case interrupt is not used.
         */
        TickUpdate();

        currentTick = TickGet();
        if ( TickGetDiff(currentTick, lastTick) >= (TICK_SECOND/2) )
        {
            lastTick = TickGet();
            USARTPut(XMODEM_NAK);

            /*
             * Blink LED to indicate that we are waiting for
             * host to send the file.
             */
            //LATA2 ^= 1;
        }

    } while( !USARTIsGetReady() );


    while(!lbDone)
    {
        /*
         * Update tick here too - just in case interrupt is not used.
         */
        TickUpdate();

        if ( USARTIsGetReady() )
        {
            /*
             * Toggle LED as we receive the data from host.
             */
            //LATA2 ^= 1;
            c = USARTGet();
        }
        else
        {
            /*
             * Real application should put some timeout to make sure
             * that we do not wait forever.
             */
            continue;
        }

        switch(state)
        {
        default:
            if ( c == XMODEM_SOH )
            {
                state = SM_MPFS_BLOCK;
            }
            else if ( c == XMODEM_EOT )
            {
                /*
                 * Turn off LED when we are done.
                 */
                //LATA2 = 1;

                MPFSClose();
                USARTPut(XMODEM_ACK);
                lbDone = TRUE;
            }
            else
                USARTPut(XMODEM_NAK);

            break;

        case SM_MPFS_BLOCK:
            /*
             * We do not use block information.
             */
            lResult = XMODEM_ACK;
            blockLen = 0;
            state = SM_MPFS_BLOCK_CMP;
            break;

        case SM_MPFS_BLOCK_CMP:
            /*
             * We do not use 1's comp. block value.
             */
            state = SM_MPFS_DATA;
            break;

        case SM_MPFS_DATA:
            /*
             * Buffer block data until it is over.
             */
            tempData[blockLen++] = c;
            if ( blockLen > XMODEM_BLOCK_LEN )
            {
                /*
                 * We have one block data.
                 * Write it to EEPROM.
                 */
                MPFSPutBegin(handle);

                lResult = XMODEM_ACK;
                for ( c = 0; c < XMODEM_BLOCK_LEN; c++ )
                    MPFSPut(tempData[c]);

                handle = MPFSPutEnd();

                USARTPut(lResult);
                state = SM_MPFS_SOH;
            }
            break;

        }

    }


/*
 * This small wait is required if SLIP is in use.
 * If this is not used, PC might misinterpret SLIP
 * module communication and never close file transfer
 * dialog box.
 */
#if defined(STACK_USE_SLIP)
    {
        BYTE i;
        i = 255;
        while( i-- );
    }
#endif
    return TRUE;
}

#endif


#if defined(APP_USE_LCD)
void XLCDDelay15ms(void)
{
    DelayMs(15);
}
void XLCDDelay4ms(void)
{
    DelayMs(4);
}

void XLCDDelay100us(void)
{
    INTCON_GIEH = 0;
    Delay10us(1);
    INTCON_GIEH = 1;
}
#endif
