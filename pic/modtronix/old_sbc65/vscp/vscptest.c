/*********************************************************************
*
*       Modtronix Example Web Server Application using Microchip TCP/IP Stack
*
*********************************************************************
* FileName:        WebSrvr.c
* Dependencies:    string.H
*                  usart.h
*                  stacktsk.h
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

#include <string.h>

/*
 * These headers must be included for required defs.
 */
#include "projdefs.h"
#include "debug.h"
#include "stacktsk.h"
#include "appcfg.h"
#include "httpexec.h"
#include "tick.h"
#include <udp.h>
#include "vscp_task.h"


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

#if defined(MPFS_USE_EEPROM)
#include "xeeprom.h"
#endif


// For debug only.
#include "tcp.h"
#include "udp.h"
#include "icmp.h"
#include "delay.h"


#define STARTUP_MSG "eurosource VSCP test"

ROM char StartupMsg[] = STARTUP_MSG;

#if defined(STACK_USE_DHCP) || defined(STACK_USE_IP_GLEANING)
ROM char DHCPMsg[] = "DHCP/Gleaning...";
#endif

ROM char SetupMsg[] = "Board Setup...";

BYTE myDHCPBindCount = 0;
#if defined(STACK_USE_DHCP)
    extern BYTE DHCPBindCount;
#else
    /*
     * If DHCP is not enabled, force DHCP update.
     */
    BYTE DHCPBindCount = 1;
#endif


/*
 * Set configuration fuses for HITECH compiler.
 * For MCC18 compiler, separately linked config.asm file
 * will set the correct fuses.
 */
#if defined(HITECH_C18)
__CONFIG(1, HSHW);
__CONFIG(2, PWRTEN & WDTDIS);
__CONFIG(4, LVPDIS & DEBUGDIS);
#endif

static void InitializeBoard(void);
static void ProcessIO(void);

void NotifyRemoteUser(void);
static void DisplayIPValue(IP_ADDR *IPVal, BOOL bToLCD);
static void SetConfig(void);


#if defined(MPFS_USE_EEPROM)
static BOOL DownloadMPFS(void);
#endif

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
#pragma code highVector=0x08
void HighVector (void)
{
    _asm goto HighISR _endasm
}
#pragma code /* return to default code section */
#endif

ROM char PressKeyForConfig[] = "\r\nPress any key for Configuration menu.";

/*
 * Main entry point.
 */
void main(void)
{
    static TICK t = 0;
    BYTE c, i;
    WORD w;
    BYTE buf[10];
    
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
     * Initialize MPFS file system.
     */
    MPFSInit();

    //Intialize HTTP Execution unit
    htpexecInit();

    //Initialze serial port
    serInit();

    /*
     * Initialize Stack and application related NV variables.
     */
    appcfgInit();
    appcfgUSART();  //Configure the USART
#ifdef SER_USE_INTERRUPT    //Interrupt enabled serial ports have to be enabled
    serEnable();
#endif
    appcfgCpuIO();  // Configure the CPU's I/O port pins
    appcfgADC();    // Configure ADC unit
    appcfgPWM();	// Configure PWM unit

    //Clear Screen
    serPutRomString(AnsiEscClearScreen);

    /*
     * Wait a couple of seconds for user input.
	 * - If something is detected, start config.
	 * - If nothing detected, start main program.
     */
	serPutRomString(PressKeyForConfig);

	for (i = 60; i > 0; --i)	//Delay for 50mS x 60 = 3 sec
	{
		if ((i % 8) == 0) serPutByte('.');
		if (serIsGetReady())
		{
	        SetConfig();
			break;
		}
		DelayMs(50);
	}
	serPutByte('\r');
	serPutByte('\n');

    StackInit();

#if defined(STACK_USE_HTTP_SERVER)
    HTTPInit();
#endif

#if defined(STACK_USE_FTP_SERVER) && defined(MPFS_USE_EEPROM)
    FTPInit();
#endif


#if defined(STACK_USE_DHCP) || defined(STACK_USE_IP_GLEANING)
    if (!AppConfig.Flags.bIsDHCPEnabled )
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

#if defined( STACK_USE_VSCP )
	vscp2_udpinit();	// init VSCP subsystem
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
    while ( 1 )
    {
        /*
         * Blink SYSTEM LED every second.
         */
        if ( TickGetDiff( TickGet(), t ) >= TICK_SECOND/2 )
        {
            t = TickGet();
            LATB6 ^= 1;
        }

        //Perform routine tasks
        MACTask();

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
        XEEBeginRead( EEPROM_CONTROL, 0x0530 );
        while ( 1 ) {
	        c = XEERead();
        	c = 1;
        }
        //c = XEERead();
        XEEEndRead();
        */
#if defined( STACK_USE_VSCP )
		vscp2_Task();
#endif        
        

        /*
         * For DHCP information, display how many times we have renewed the IP
         * configuration since last reset.
         */
        if ( DHCPBindCount != myDHCPBindCount )
        {
            DisplayIPValue(&AppConfig.MyIPAddr, TRUE);
            myDHCPBindCount = DHCPBindCount;
        }
    }
}

static void DisplayIPValue(IP_ADDR *IPVal, BOOL bToLCD)
{
    char IPDigit[8];

    /*
     * Rewrite the second line.
     */
    itoa(IPVal->v[0], IPDigit);
    {
        serPutString((BYTE*)IPDigit);
        serPutByte('.');
    }

    itoa(IPVal->v[1], IPDigit);
    {
        serPutString((BYTE*)IPDigit);
        serPutByte('.');
    }

    itoa(IPVal->v[2], IPDigit);
    {
        serPutString((BYTE*)IPDigit);
        serPutByte('.');
    }

    itoa(IPVal->v[3], IPDigit);
	{
        serPutString((BYTE*)IPDigit);
	}
}

static void ProcessIO(void)
{
//Convert next ADC channel, and store result in adcChannel array
#if (defined(APP_USE_ADC8) || defined(APP_USE_ADC10)) && (ADC_CHANNELS > 0)
    static BYTE adcChannel; //Current ADC channel. Value from 0 - n

    //We are allowed to update AdcValues[] buffer
    if ( !AppConfig.Flags.bFreezeADCBuf )
    {
        //Increment to next ADC channel    
        if ((++adcChannel) >= ADC_CHANNELS)
        {
            adcChannel = 0;
        }

        //Check if current ADC channel (adcChannel) is configured to be ADC channel
        if (adcChannel < ((~ADCON1) & 0x0F))
        {
            //Convert next ADC Channel
            ADCON0 &= ~0x3C;
            ADCON0 |= (adcChannel << 2);

            ADCON0_ADON = 1;    //Switch on ADC
            ADCON0_GO = 1;      //Go
    
            while (ADCON0_GO); //Wait for conversion to finish

#if defined(APP_USE_ADC8)
            AdcValues[adcChannel] = ADRESH;
#elif defined(APP_USE_ADC10)
            AdcValues[ adcChannel ] = ((WORD)ADRESH << 8) + ADRESL;  // AKHE
#endif
        }
        //Not ADC channel, set to 0
        else {
            AdcValues[adcChannel] = 0;
        }
    }
#endif

}

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
     * Turn off the LED's.
     */
    TRISB_RB6 = 0;

    /*
     * External data EEPROM needs pull-ups, so enable internal
     * pull-ups.
     */
    INTCON2_RBPU = 0;

    //
    T0CON = 0;
    INTCON_GIEH = 1;
    INTCON_GIEL = 1;
}

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
        while( !serIsGetReady() );

        v = serGetByte();

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

    while ( !serIsGetReady() );

    c = serGetByte();

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

    serPutByte('\r');
    serPutByte('\n');
    serPutRomString(menuCommandPrompt[choice-'0'-1]);

    switch(choice)
    {
    case MENU_CMD_SERIAL_NUMBER:
        itoa(AppConfig.SerialNumber.Val, response);
        serPutString((BYTE*)response);
        serPutByte(')');
        serPutByte(':');
        serPutByte(' ');

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
        serPutByte(')');
        serPutByte(':');
        serPutByte(' ');

        USARTGetString(response, sizeof(response));

        if ( !StringToIPAddress(response, &tempIPValue) )
        {
HandleInvalidInput:
            serPutRomString(InvalidInputMsg);
            while( !serIsGetReady() );
            serGetByte();
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
        appcfgSave();
#endif
        break;
    }
}

static void SetConfig(void)
{
    MENU_CMD choice;

    do
    {
        serPutRomString(menu);
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
#define XMODEM_SOH      	0x01
#define XMODEM_EOT      	0x04
#define XMODEM_ACK     	 	0x06
#define XMODEM_NAK      	0x15
#define XMODEM_CAN      	0x18
#define XMODEM_BLOCK_LEN 	64

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
        currentTick = TickGet();
        if ( TickGetDiff(currentTick, lastTick) >= (TICK_SECOND/2) )
        {
            lastTick = TickGet();
            serPutByte(XMODEM_NAK);

            /*
             * Blink LED to indicate that we are waiting for
             * host to send the file.
             */
            //LATA2 ^= 1;
        }

    } while( !serIsGetReady() );


    while(!lbDone)
    {
        if ( serIsGetReady() )
        {
            /*
             * Toggle LED as we receive the data from host.
             */
            //LATA2 ^= 1;
            c = serGetByte();
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
                serPutByte(XMODEM_ACK);
                lbDone = TRUE;
            }
            else
                serPutByte(XMODEM_NAK);

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

                serPutByte(lResult);
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


