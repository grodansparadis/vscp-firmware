/*********************************************************************
*
*       Example Web Server Application using Microchip TCP/IP Stack
*
*********************************************************************
* FileName:        MainDemo.c
* Dependencies:    string.H
*                  usart.h
*                  StackTsk.h
*                  Tick.h
*                  http.h
*                  MPFS.h
*				   mac.h
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
* Nilesh Rajbharti     4/19/01      Original (Rev. 1.0)
* Nilesh Rajbharti     2/09/02      Cleanup
* Nilesh Rajbharti     5/22/02      Rev 2.0 (See version.log for detail)
* Nilesh Rajbharti     7/9/02       Rev 2.1 (See version.log for detail)
* Nilesh Rajbharti     4/7/03       Rev 2.11.01 (See version log for detail)
* Howard Schlunder	   10/1/04		Beta Rev 0.9 (See version log for detail)
* Howard Schlunder	   10/8/04		Beta Rev 0.9.1 Announce support added
* Howard Schlunder	   11/29/04		Beta Rev 0.9.2 (See version log for detail)
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

/*
 * These headers must be included for required defs.
 */
#include "StackTsk.h"
#include "Tick.h"
#include "MAC.h"

#if defined(STACK_USE_DHCP)
#include "DHCP.h"
#endif

#if defined(STACK_USE_HTTP_SERVER)
#include "HTTP.h"
#endif

#include "MPFS.h"

#if defined(STACK_USE_FTP_SERVER) && defined(MPFS_USE_EEPROM)
#include "FTP.h"
#endif

#if defined(USE_LCD)
#include "XLCD.h"
#endif

#if defined(STACK_USE_ANNOUNCE)
#include "Announce.h"
#endif

#if defined(MPFS_USE_EEPROM)
#include "XEEPROM.h"
#endif


#include "Delay.h"

#define STARTUP_MSG "MpStack 0.9.2"

ROM char const StartupMsg[] = STARTUP_MSG;

#if (defined(STACK_USE_DHCP) || defined(STACK_USE_IP_GLEANING)) && defined(USE_LCD)
ROM char const DHCPMsg[] = "DHCP/Gleaning...";
#endif

ROM char const SetupMsg[] = "Board Setup...";

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
 * For MCC18 compiler, separately linked C18cfg.asm file
 * will set the correct fuses.
 */
#if defined(HITECH_C18)
// Settings for PIC18F4620
__CONFIG(1, UNPROTECT & 0x32FF);	// Fail-safe clock monitor disable, oscillator switch over disabled, HS
//__CONFIG(1, UNPROTECT & 0x36FF);	// Fail-safe clock monitor disable, oscillator switch over disabled, HS_PLL
//__CONFIG(1, UNPROTECT & 0x35FF);	// Fail-safe clock monitor disable, oscillator switch over disabled, ECRA6
__CONFIG(2, PWRTDIS & BORDIS & WDTDIS);

// Old settings for PIC18F452
//__CONFIG(1, UNPROTECT & HS);
//__CONFIG(2, PWRTEN & BORDIS & WDTDIS);
#endif

/*
 * Private helper functions.
 * These may or may not be present in all applications.
 */
static void InitAppConfig(void);

static void InitializeBoard(void);
static void ProcessIO(void);

void NotifyRemoteUser(void);
void USARTPutROMString(ROM char const *str);
static void DisplayIPValue(IP_ADDR *IPVal, BOOL bToLCD);
static void SetConfig(void);

#if defined(MPFS_USE_EEPROM)
static BOOL DownloadMPFS(void);
static void SaveAppConfig(void);
#endif

// NOTE: Several PICs, including the PIC18F4620 revision A3 have a RETFIE FAST/MOVFF bug
// The interruptlow keyword is used to work around the bug when using C18
#if defined(MCHP_C18)
    #pragma interruptlow HighISR save=section(".tmpdata")
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

#define USARTIsGetReady()   (PIR1_RCIF)
#define USARTGet()          (RCREG)

ROM char const NewIP[] = "New IP Address: ";
ROM char const CRLF[] = "\r\n";


/*
 * Main entry point.
 */
void main(void)
{
    static TICK t = 0;
    
    
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
     * This implementation, initiates Board setup process if RB5
     * is detected low on startup.
     */
    if ( PORTB_RB5 == 0 )
    {
#if defined(USE_LCD)
        XLCDGoto(1, 0);
        XLCDPutROMString(SetupMsg);
#endif

        SetConfig();
    }

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
#if defined(USE_LCD)
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
        /*
         * Blink SYSTEM LED every second.
         */
        if ( TickGetDiff(TickGet(), t) >= TICK_SECOND/2 )
        {
            t = TickGet();
            LATA4 ^= 1;
        }

// TODO: Remove
// This is a quick test to see if the stack can recover from going to 
// sleep and waking back up.  If uncommented, the ENC28J60 would stay 
// sleeping whenever RB5 is held down and would wake back up when 
// released.
//		if( !PORTB_RB5 )
//		{
//			MACPowerDown();
//			while( !PORTB_RB5 );
//			MACPowerUp();
//		}


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
            myDHCPBindCount = DHCPBindCount;

			USARTPutROMString(NewIP);
            DisplayIPValue(&AppConfig.MyIPAddr, FALSE);	// Print to USART
			USARTPutROMString(CRLF);
#if defined(STACK_USE_ANNOUNCE)
			AnnounceIP();
#endif

#if defined(USE_LCD)
            DisplayIPValue(&AppConfig.MyIPAddr, TRUE);

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

#if defined(USE_LCD)
//                               1234567890123456
ROM char const blankLCDLine[] = "                ";
#endif


static void DisplayIPValue(IP_ADDR *IPVal, BOOL bToLCD)
{
    char IPDigit[8];

#ifdef USE_LCD
    if ( bToLCD )
    {
        /*
         * Erase second line.
         */
        XLCDGoto(1, 0);
        XLCDPutROMString(blankLCDLine);

    }

    /*
     * Rewrite the second line.
     */
    XLCDGoto(1, 0);
#endif

    itoa(IPVal->v[0], IPDigit);
#ifdef USE_LCD
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
#ifdef USE_LCD
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
#ifdef USE_LCD
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
#ifdef USE_LCD
    if ( bToLCD )
        XLCDPutString(IPDigit);
    else
#endif
        USARTPutString((BYTE*)IPDigit);
}


static char AN0String[8];
static char AN1String[8];

static void ProcessIO(void)
{
    WORD_VAL ADCResult;

    /*
     * Select AN0 channel, Fosc/64 clock
     * Works for both compatible and regular A/D modules
     */
    ADCON0      = 0b10000001;

    /*
     * Wait for acquisition time.
     * Here, rather than waiting for exact time, a simple wait is
     * used.  Real applications requiring high accuracy should
     * calculate exact acquisition time and wait accordingly.
     */
    ADCResult.v[0] = 100;
    while( ADCResult.v[0]-- );

    /*
     * First convert AN0 channel.
     * AN0 is already setup as an analog input.
     */
    ADCON0_GO   = 1;

    /*
     * Wait until conversion is done.
     */
    while( ADCON0_GO );

    /*
     * Save the result.
     */
    ADCResult.v[0] = ADRESL;
    ADCResult.v[1] = ADRESH;

    /*
     * Convert 10-bit value into ASCII String.
     */
    itoa(ADCResult.Val, AN0String);

    /*
     * Now, convert AN1 channel.
     *
     * In PICDEM.net board, RA2 thru RA7 should be digital or else
     * LED, LCD and NIC would not operate correctly.
     * Since there is no mode where only AN0 and AN1 be analog inputs
     * while rests are digial pins, we will temperoraily switch
     * select a mode where RA2 becomes analog input while we do
     * conversion of RA1.  Once conversion is done, we will convert
     * RA2 back to digital pin.
     */
#if defined(USE_COMPATIBLE_AD)
	// Change AN1 to be an analog input
    ADCON1      = 0b11000100;

    // Select AN1 channel.
    ADCON0      = 0b10001001;
#else
    // Select AN1 channel.
	ADCON0 		= 0b00000101;
#endif


    /*
     * Wait for acquisition time.
     * Here, rather than waiting for exact time, a simple wait is
     * used.  Real applications requiring high accuracy should
     * calculate exact acquisition time and wait accordingly.
     */
    ADCResult.v[0] = 100;
    while( ADCResult.v[0]-- );

    /*
     * Start the conversion.
     */
    ADCON0_GO   = 1;

    /*
     * Wait until it is done.
     */
    while( ADCON0_GO );

    /*
     * Save the result.
     */
    ADCResult.v[0] = ADRESL;
    ADCResult.v[1] = ADRESH;

    /*
     * Convert 10-bit value into ASCII String.
     */
    itoa(ADCResult.Val, AN1String);

    /*
     * Reset RA2 pin back to digital output.
     */
#if defined(USE_COMPATIBLE_AD)     
    ADCON1      = 0b11001110;       // RA0 as analog input.
#endif
}

/*
 * CGI Command Codes.
 */
#define CGI_CMD_DIGOUT      (0)
#define CGI_CMD_LCDOUT      (1)

/*
 * CGI Variable codes. - There could be 0-255 variables.
 */
#define VAR_LED_D5          (0)
#define VAR_LED_D6          (1)
#define VAR_ANAIN_AN0       (2)
#define VAR_ANAIN_AN1       (3)
#define VAR_DIGIN_RB5       (4)
#define VAR_STROUT_LCD      (5)		// Obsolete.  No LCD present.
#define VAR_MAC_ADDRESS     (6)		// Reserved for future use
#define VAR_SERIAL_NUMBER   (7)		// Reserved for future use
#define VAR_IP_ADDRESS      (8)		// Reserved for future use
#define VAR_SUBNET_MASK     (9)		// Reserved for future use
#define VAR_GATEWAY_ADDRESS (10)	// Reserved for future use
#define VAR_DHCP_TRUE       (11)	// Reserved for future use
#define VAR_DHCP_FALSE      (12)	// Reserved for future use


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

ROM char const COMMANDS_OK_PAGE[] = "COMMANDS.CGI";

// Copy string with NULL termination.
#define COMMANDS_OK_PAGE_LEN  (sizeof(COMMANDS_OK_PAGE))

ROM char const CMD_UNKNOWN_PAGE[] = "INDEX.HTM";

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
        case VAR_LED_D5:
            /*
             * This is "D5".
             * Toggle D5.
             */
            LATA3 ^= 1;
            break;

        case VAR_LED_D6:
            /*
             * This is "D6".
             * Toggle it.
             */
            LATA2 ^= 1;
            break;
         }

         memcpypgm2ram((void*)argv[0],
              (ROM void*)COMMANDS_OK_PAGE, COMMANDS_OK_PAGE_LEN);
         break;

    case CGI_CMD_LCDOUT:
        /*
         * Not implemented.
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

    case VAR_LED_D5:
        if ( LATA3 )
            *val = '0';
        else
            *val = '1';
        break;

    case VAR_LED_D6:
        if ( LATA2 )
            *val = '0';
        else
            *val = '1';
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

    case VAR_DIGIN_RB5:
        if ( PORTB_RB5 )
            *val = '1';
        else
            *val = '0';
        break;

/*
// TODO: Options reserved for future use
    case VAR_MAC_ADDRESS:
    	break;
    	
    case VAR_SERIAL_NUMBER:
    	break;
    	
    case VAR_IP_ADDRESS:
    	break;
    	
    case VAR_SUBNET_MASK:
    	break;
    	
    case VAR_GATEWAY_ADDRESS:
	   	break;
    	
    case VAR_DHCP_TRUE:
    	break;
    	
    case VAR_DHCP_FALSE:
    	break;
*/
    }
    return HTTP_END_OF_VAR;
}
#endif


#if defined(STACK_USE_FTP_SERVER) && defined(MPFS_USE_EEPROM)
ROM char const FTP_USER_NAME[]    = "ftp";
#undef FTP_USER_NAME_LEN
#define FTP_USER_NAME_LEN   (sizeof(FTP_USER_NAME)-1)

ROM char const FTP_USER_PASS[]    = "microchip";
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
    /*
     * Setup for PORTA.RA0 as analog input while rests
     * as digital i/o lines.
     */
#if defined(USE_COMPATIBLE_AD)     
    ADCON1  = 0b11001110;       // RA0 as analog input, Right justified
#else
	ADCON0 = 0b00000001;	// ADON, Channel 0
	ADCON1 = 0b00001101;	// Vdd/Vss is +/-REF, AN0 and AN1 are analog
	ADCON2 = 0b10000110;	// Right justify, no ACQ time, Fosc/64
#endif

#if defined(USE_LCD)
    TRISA   = 0x03;

    /*
     * LCD is enabled using RA5.
     */
    PORTA_RA5 = 0;          // Disable LCD.
#else
    TRISA   = 0x23;
#endif

    /*
     * Turn off the LED's.
     */
    LATA2 = 1;
    LATA3 = 1;

    /*
     * External data EEPROM needs pull-ups, so enable internal
     * pull-ups.
     */
    INTCON2_RBPU = 0;

#if defined(USE_LCD)
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

ROM char const menu[] =
    "\r\n\r\n\r\MCHPStack Beta Config Application v0.9.2 ("STARTUP_MSG", " __DATE__ ")\r\n\r\n"

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

ROM char* const menuCommandPrompt[] =
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

void USARTPutROMString(ROM char const * str)
{
    BYTE v;

    while( v = *str++ )
        USARTPut(v);
}


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
            LATA2 ^= 1;
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
            LATA2 ^= 1;
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
                LATA2 = 1;

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


#if defined(USE_LCD)
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
