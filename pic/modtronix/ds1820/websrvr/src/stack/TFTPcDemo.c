/*********************************************************************
 *
 *                  TFTP Client Demo App for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        TFTPcDemo.c
 * Dependencies:    tftpc.h
 *                  stacktsk.h
 *                  xlcd.h
 *                  tick.h
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
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     8/7/03  Original        (Rev 1.0)
 *
 *
 * This demo application demonstartes Microchip TFTP client module usage
 * See TFTPDemo(), TFTPWrite() and TFTPRead() for actual implementation.
 *
 * This application monitors RB5 switch. On first push it
 * writes 'tftpwr.bin' file to server.  tftpwr.bin file is made up of
 * first 16KB of program memory.
 * On second push, it reads 'tftprd.txt' file from server.
 *
 * Server IP address is set in "Board Setup" mode using Hyperterminal.
 *
 * It displays rotating '-' to indicate command in progress.
 * 'Y' to indicate success
 * 'N' for failure
 * 'T' for timeout
 * 'E' for error.
 *
 *
 * If running this applicaton on PICDEM.net use
 * HS Oscillator
 * Debug disabled
 * Low Voltage Disabled
 * Watchdog timer disabled
 ********************************************************************/
/*
 * Following define uniquely deines this file as main
 * entry/application In whole project, there should only be one such
 * definition and application file must define AppConfig variable as
 * described below.
 */
#define THIS_IS_STACK_APPLICATION

#include "stacktsk.h"
#include "dhcp.h"
#include "tftpc.h"
#include "xlcd.h"
#include "tick.h"
#include "xeeprom.h"
#include "delay.h"

#define BAUD_RATE       (19200)     // bps


// All TFTP command statuts display will be done on first line of LCD.
#define TFTP_COMMAND_DISPLAY_LINE       0

// Result will be displayed at y = 15.
#define TFTP_COMMAND_RESULT_POSITION    15


#define STARTUP_MSG "MpStack 2.20.02+"

ROM char StartupMsg[] = STARTUP_MSG;

#if defined(STACK_USE_DHCP) || defined(STACK_USE_IP_GLEANING)
ROM char DHCPMsg[] = "DHCP/Gleaning...";
#endif

ROM char SetupMsg[] = "Board Setup...";

//                         1234567890123456
ROM char blankLCDLine[] = "                ";

// Name of TFTP Put file
static char tftpFilePutName[] = "tftpwr.bin";

// Name of TFTP Get file
static char tftpFileGetName[] = "tftprd.txt";

// Strings to display on appropriate command.
ROM char tftpPutString[] = "PUT";
ROM char tftpGetString[] = "GET";


#define USART_USE_BRGH_LOW
#if defined(USART_USE_BRGH_LOW)
    #define SPBRG_VAL   ( ((CLOCK_FREQ/BAUD_RATE)/64) - 1)
#else
    #define SPBRG_VAL   ( ((CLOCK_FREQ/BAUD_RATE)/16) - 1)
#endif

#if SPBRG_VAL > 255
    #error "Calculated SPBRG value is out of range for currnet CLOCK_FREQ."
#endif


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


// This demo application issues PUT on first push of RB5
// GET on second push.
static union
{
    struct
    {
        unsigned int bFirstPush : 1;
    } bits;
    BYTE Val;
} tftpDemoFlag;



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
#endif


/*
 * Private helper functions.
 * These may or may not be present in all applications.
 */
static void InitAppConfig(void);
static void InitializeBoard(void);
static void DisplayIPValue(IP_ADDR *IPVal, BOOL bToLCD);
static void SetConfig(void);
static void SaveAppConfig(void);
static void USARTPut(BYTE c);
static void USARTPutString(BYTE *s);

static BOOL TFTPRead(void);
static BOOL TFTPWrite(void);
static void TFTPDemo(void);



#define USARTIsGetReady()   (PIR1_RCIF)
#define USARTGet()          (RCREG)


void main(void)
{
    // Tick to blink SYSTEM led.
    static TICK t = 0;

    // Remember that next RB5 push is the first one.
    tftpDemoFlag.bits.bFirstPush = TRUE;

    /*
     * Initialize any application specific hardware.
     */
    InitializeBoard();

    /*
     * Since we are not using MPFS, initialize XEEPROM directly.
     * If MPFS was used, calling MPFSInit() would automatically
     * initialize  XEEPROM.
     */
    XEEInit(EE_BAUD(CLOCK_FREQ, 400000));

    /*
     * Initialize all stack related components.
     * Following steps must be performed for all applications using
     * PICmicro TCP/IP Stack.
     */
    TickInit();

    /*
     * Initialize Stack and application related NV variables.
     */
    InitAppConfig();



    /*
     * This implementation, initiates Board setup process if RB5
     * is detected low on startup.
     */
    if ( PORTB_RB5 == 0 )
    {
        XLCDGoto(1, 0);
        XLCDPutROMString(SetupMsg);

        SetConfig();
    }

    StackInit();

#if defined(STACK_USE_DHCP) || defined(STACK_USE_IP_GLEANING)
    if ( AppConfig.Flags.bIsDHCPEnabled )
    {
        XLCDGoto(1, 0);
        XLCDPutROMString(DHCPMsg);
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

        /*
         * This task performs normal stack task including checking
         * for incoming packet, type of packet and calling
         * appropriate stack entity to process it.
         */
        StackTask();

        /*
         * For DHCP information, display how many times we have renewed the IP
         * configuration since last reset.
         */
        if ( DHCPBindCount != myDHCPBindCount )
        {
            DisplayIPValue(&AppConfig.MyIPAddr, TRUE);
            myDHCPBindCount = DHCPBindCount;

            if ( AppConfig.Flags.bIsDHCPEnabled )
            {
                XLCDGoto(1, 14);
                if ( myDHCPBindCount < 0x0a )
                    XLCDPut(myDHCPBindCount + '0');
                else
                    XLCDPut(myDHCPBindCount + 'A');
            }
        }


        /*
         * This demo applicaton, monitors RB5 switch on-board PICDEM.net board.
         * On first push, it uses TFTP PUT to send a file named "put.txt".
         * On second push, it gets same "put.txt" file from server.
         *
         * Wait until IP address is assigned to this board.
         */
        if ( !stackFlags.bits.bInConfigMode )
            TFTPDemo();

    }
}


static void TFTPDemo(void)
{
    static enum
    {
        SM_TFTP_DEMO_IDLE,
        SM_TFTP_DEMO_WAIT,
        SM_TFTP_DEMO_READY,
        SM_TFTP_DEMO_WRITE,
        SM_TFTP_DEMO_READ,
        SM_TFTP_DEMO_CYCLE
    } smTFTPDemo = SM_TFTP_DEMO_IDLE;
    TFTP_RESULT result;

    // Display rotating '-' while we execute command.
    if ( smTFTPDemo == SM_TFTP_DEMO_WRITE ||
         smTFTPDemo == SM_TFTP_DEMO_READ )
    {
        static char displayChar[] = { '-', '\\', '|', '/' };
        static char i = 0;
        static TICK lastTick = 0;

        if ( TickGetDiff(TickGet(), lastTick) >= TICK_SECOND/2 )
        {
            XLCDGoto(TFTP_COMMAND_DISPLAY_LINE, TFTP_COMMAND_RESULT_POSITION);
            XLCDPut(displayChar[i++]);
            if ( i == 4 )
                i = 0;

            lastTick = TickGet();
        }
    }


    // Main state machine for demo.
    switch(smTFTPDemo)
    {
    case SM_TFTP_DEMO_IDLE:
        // On startup, we try to resolve TFTP server IP address.
        // Application may decide to do this when actual TFTP operation is required
        // This is completely application dependent.
        // By doing it here, we can reuse ARP on subsequent TFTP operation
        TFTPOpen(&AppConfig.TFTPServerAddr);

        // Now we must wait for ARP to get resolved.
        smTFTPDemo = SM_TFTP_DEMO_WAIT;
        break;

    case SM_TFTP_DEMO_WAIT:
        // Check to see if connection was established.
        result = TFTPIsOpened();
        if ( result == TFTP_OK )
            // ARP is resolved and UDP port is opened.
            // Now we are ready for future TFTP commands.
            smTFTPDemo = SM_TFTP_DEMO_READY;

        else if ( result == TFTP_TIMEOUT )
            // Timeout has occured.
            // Application may decide to count attempts and give-up after
            // so many retries or as in this case, continue to try forever.
            // We must have resolved server IP address or else nothing
            // can be done.
            smTFTPDemo = SM_TFTP_DEMO_IDLE;
        break;

    case SM_TFTP_DEMO_READY:
        // Scan for RB5 switch.
        if ( PORTB_RB5 == 0 )
        {
            // RB5 was pushed.  Decide if this is first push or second push
            // and perform appropriate command.

            // Blank out TFTP command display line in LCD.
            XLCDGoto(TFTP_COMMAND_DISPLAY_LINE, 0);
            XLCDPutROMString(blankLCDLine);
            XLCDGoto(TFTP_COMMAND_DISPLAY_LINE, 0);

            if ( tftpDemoFlag.bits.bFirstPush )
            {
                // For first push, perform PUT operation.
                XLCDPutROMString(tftpPutString);
                smTFTPDemo = SM_TFTP_DEMO_WRITE;
            }

            else
            {
                // On second push, perform GET operation.
                XLCDPutROMString(tftpGetString);
                smTFTPDemo =  SM_TFTP_DEMO_READ;
            }

        }
        break;

    case SM_TFTP_DEMO_WRITE:
        // Perform actual operation.  This may take
        // some time depending on file size and network condition.
        // This function returns FALSE if has not finished or TRUE
        // when it is finished - successful or failed.
        if ( TFTPWrite() )
            // Upon completion, we go back to monitoring switch.
            smTFTPDemo = SM_TFTP_DEMO_CYCLE;
        break;

    case SM_TFTP_DEMO_READ:
        // This function too may take time to finish.
        if ( TFTPRead() )
            // Read has finished, go back to monitoring switch.
            smTFTPDemo = SM_TFTP_DEMO_CYCLE;
        break;

    case SM_TFTP_DEMO_CYCLE:
        // Wait for switch to be released.
        if ( PORTB_RB5 == 1 )
        {
            // Remember what push this is.
            tftpDemoFlag.bits.bFirstPush = !tftpDemoFlag.bits.bFirstPush;

            // And go back to monitoring...
            smTFTPDemo =  SM_TFTP_DEMO_READY;
        }
        break;
    }
}


// This is template function to perform TFTP Put operation.
// Most application should be able to take code from this function and
// perform TFTP Put operation.
// Remove/Enhance LCD related code as per your requirements.
//
// This function simply writes first 16KB of program memory to
// tftpwr.bin file.
static BOOL TFTPWrite(void)
{
    BYTE v;
    BOOL lbReturn;
    TFTP_RESULT result;

    // State machine state.
    static enum
    {
        SM_TFTP_PUT_IDLE,
        SM_TFTP_PUT_OPEN_FILE,
        SM_TFTP_PUT_OPEN_FILE_WAIT,
        SM_TFTP_PUT_DATA,
        SM_TFTP_PUT_CLOSE_FILE_WAIT
    } smTFTPPut = SM_TFTP_PUT_OPEN_FILE;

    // Since we are writing a file, we must be prepared to resend
    // last data block.  these pointer will keep track of data block
    static ROM BYTE *retryPtr;
    static ROM BYTE *currentPtr;
    BOOL lbIsBeginingOfBlock;

    lbReturn = FALSE;

    lbIsBeginingOfBlock = TRUE;

    switch(smTFTPPut)
    {
    // Since this demo application has already opened TFTPOpen on startup, we
    // do not need to do again.  If application will be communicating with
    // several TFTP servers, it would need to close and open connection with
    // different servers.  In that case for every TFTP operation, it must
    // perform TFTPOpen and related sequence.
    //
    // In this case, we will simply start opening file.
#if 0
    case SM_TFTP_PUT_IDLE:
        TFTPOpen(&AppConfig.TFTPServerAddr);
        smTFTPPut = SM_TFTP_PUT_WAIT;
        break;

    case SM_TFTP_PUT_WAIT:
        result = TFTPIsOpened();
        if ( result == TFTP_OK )
            smTFTPPut = SM_TFTP_PUT_OPEN_FILE;

        else if ( result == TFTP_RETRY )
            smTFTPPut = SM_TFTP_PUT_IDLE;

        break;
#endif
    case SM_TFTP_PUT_OPEN_FILE:
        // Make sure that it is okay to send file open request.
        if ( TFTPIsFileOpenReady() )
        {
            // Initialize file data pointer.
            currentPtr = (ROM BYTE*)0x0;
            retryPtr = (ROM BYTE*)0x0;

            // Issue file open request for WRITE access.
            TFTPOpenFile(tftpFilePutName, TFTP_FILE_MODE_WRITE);

            // Wait for it get accepted.
            smTFTPPut = SM_TFTP_PUT_OPEN_FILE_WAIT;
        }
        break;

    case SM_TFTP_PUT_OPEN_FILE_WAIT:
        // Is file opened?  or can we write to the file?
        result = TFTPIsFileOpened();
        if ( result == TFTP_OK )
            // Yes.  Start writing data.
            smTFTPPut = SM_TFTP_PUT_DATA;

        else if ( result == TFTP_RETRY )
            // Somehow previous request for file open was not
            // received by server or server simly did not
            // respond.  Try again...
            smTFTPPut = SM_TFTP_PUT_OPEN_FILE;

        break;


    case SM_TFTP_PUT_DATA:
        // Once it is okay to write data, as per TFTP spec.
        // we can write up to 512 bytes of data in one block.
        // Stay in loop and send all 512 bytes of data in one shot.
        while(1)
        {
            // Must make sure that it is okay to put any more data.
            // If block is fulled, this function will return TFTP_NOT_READY
            // and we must wait.
            result = TFTPIsPutReady();
            if( result == TFTP_OK )
            {
                // If this is very first block put, remember retry pointer
                // so that in case we need to retry it, we can simply
                // reset the pointer and supply retry data.
                if ( lbIsBeginingOfBlock )
                {
                    lbIsBeginingOfBlock = FALSE;
                    retryPtr = currentPtr;
                }

                // Get data byte that we want to write.
                v = *currentPtr++;

                // Check for end of data.
                if ( currentPtr >= (ROM BYTE*)0x3FFFL )
                {
                    // If end of data is reached, initiated FileClose.
                    TFTPCloseFile();

                    // Wait for file to get closed.
                    smTFTPPut = SM_TFTP_PUT_CLOSE_FILE_WAIT;
                    break;
                }
                else
                    // or else put it in block.
                    TFTPPut(v);

            }

            else if ( result == TFTP_RETRY )
            {
                // Retry last block.
                currentPtr = retryPtr;
                break;
            }

            else
                // For all other returns, break out of this loop and
                // let common logic at the end of function handle it.
                break;

        }
        break;

    case SM_TFTP_PUT_CLOSE_FILE_WAIT:
        // Is file close request compelte?
        result = TFTPIsFileClosed();
        if ( result == TFTP_OK )
            // Yes, Remember to return TRUE.
            lbReturn = TRUE;

        else if ( result == TFTP_RETRY )
        {
            // No.  We need to retry.
            // In this case, we need to send last data block again.
            // and redo file close sequence.
            currentPtr = retryPtr;

            // Go back to PUT_DATA state.
            smTFTPPut = SM_TFTP_PUT_DATA;
        }
        break;
    }

    // Common action to all states.
    if ( result == TFTP_OK && lbReturn )
    {
        // Operation is complete and successful.
        // For visual indication, display 'Y' on LCD first line.
        XLCDGoto(TFTP_COMMAND_DISPLAY_LINE, TFTP_COMMAND_RESULT_POSITION);
        XLCDPut('Y');
    }

    else if ( result == TFTP_TIMEOUT || result == TFTP_ERROR )
    {
        // Either timeout or error has occurred.
        XLCDGoto(TFTP_COMMAND_DISPLAY_LINE, TFTP_COMMAND_RESULT_POSITION);

        // Display 'T' for timeout
        if ( result == TFTP_TIMEOUT )
            XLCDPut('T');
        else
            // 'E' for error.
            XLCDPut('E');
        lbReturn = TRUE;
    }

    // If command is complete, reset the state machine.
    if ( lbReturn )
        smTFTPPut = SM_TFTP_PUT_OPEN_FILE;

    return lbReturn;
}



// This is template function to perform TFTP Get operation.
// Most application should be able to take code from this function and
// perform TFTP Put operation.
// Remove/Enhance LCD related code as per your requirements.
//
// This function trie to read tftprd.txt file from remote server.
static BOOL TFTPRead(void)
{
    BYTE v;
    BOOL lbReturn;
    TFTP_RESULT result;

    // State machine states.
    static enum
    {
        SM_TFTP_GET_IDLE,
        SM_TFTP_GET_WAIT,
        SM_TFTP_GET_OPEN_FILE,
        SM_TFTP_GET_OPEN_FILE_WAIT,
        SM_TFTP_GET_DATA
    } smTFTPGet = SM_TFTP_GET_OPEN_FILE;


    lbReturn = FALSE;

    switch(smTFTPGet)
    {
    // Since this demo application has already opened TFTPOpen on startup, we
    // do not need to do again.  If application will be communicating with
    // several TFTP servers, it would need to close and open connection with
    // different servers.  In that case for every TFTP operation, it must
    // perform TFTPOpen and related sequence.
    //
    // In this case, we will simply start opening file.
#if 0
    case SM_TFTP_GET_IDLE:
        TFTPOpen(&AppConfig.TFTPServerAddr);
        smTFTPGet = SM_TFTP_GET_WAIT;
        break;

    case SM_TFTP_GET_WAIT:
        result = TFTPIsOpened();
        if ( result == TFTP_OK
            smTFTPGet = SM_TFTP_GET_OPEN_FILE;

        else if ( result == TFTP_RETRY )
            smTFTPGet = SM_TFTP_GET_IDLE;
        break;
#endif

    case SM_TFTP_GET_OPEN_FILE:
        // Is is okay to issue file open?
        if ( TFTPIsFileOpenReady() )
        {
            // Issue file open command.
            // For read operation, there is no need to data retry. All retries
            // are done automatically by module.
            TFTPOpenFile(tftpFileGetName, TFTP_FILE_MODE_READ);

            // Wait for file get opened.
            smTFTPGet = SM_TFTP_GET_OPEN_FILE_WAIT;
        }
        break;

    case SM_TFTP_GET_OPEN_FILE_WAIT:
        // Is file opened?
        result = TFTPIsFileOpened();
        if ( result == TFTP_OK )
            // Yes.  Go and read the first data block.
            smTFTPGet = SM_TFTP_GET_DATA;

        else if ( result == TFTP_RETRY )
        {
            // Somehow fileopen was not acknowledged, try again.
            smTFTPGet = SM_TFTP_GET_OPEN_FILE;
            break;
        }
        else
            break;

    case SM_TFTP_GET_DATA:
        // Once a data block is available, application may read up to 512 bytes
        // of data.  Read up to 512 bytes in one shot.
        while(1)
        {
            // Make sure that it is okay to call TFTPGet.  If all 512 bytes are
            // read or if there is no more data byte, this function will return
            // TFTP_NOT_READY.
            result = TFTPIsGetReady();
            if ( result == TFTP_OK )
            {
                // Fetch data byte.
                // In this demo, it is simply ignored.
                v = TFTPGet();

                // One may output it so another port - for example UART.
                //USARTPut(v);
            }
            else
                break;
        }
        break;
    }

    // Common logic for all states.
    if ( result == TFTP_END_OF_FILE )
    {
        // If end of file is reached, display it.
        lbReturn = TRUE;
        XLCDGoto(TFTP_COMMAND_DISPLAY_LINE, TFTP_COMMAND_RESULT_POSITION);
        XLCDPut('Y');
    }

    else if ( result == TFTP_TIMEOUT || result == TFTP_ERROR )
    {
        lbReturn = TRUE;
        XLCDGoto(TFTP_COMMAND_DISPLAY_LINE, TFTP_COMMAND_RESULT_POSITION);

        // For timeout, display 'T'
        if ( result == TFTP_TIMEOUT )
            XLCDPut('T');
        else
            // an for error, display 'E'
            XLCDPut('E');
    }

    // On completion of command, reset the state machine.
    if ( lbReturn )
        smTFTPGet = SM_TFTP_GET_OPEN_FILE;

    return lbReturn;
}







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




static void DisplayIPValue(IP_ADDR *IPVal, BOOL bToLCD)
{
    char IPDigit[8];

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

    itoa(IPVal->v[0], IPDigit);
    if ( bToLCD )
    {
        XLCDPutString(IPDigit);
        XLCDPut('.');
    }
    else
    {
        USARTPutString((BYTE*)IPDigit);
        USARTPut('.');
    }

    itoa(IPVal->v[1], IPDigit);
    if ( bToLCD )
    {
        XLCDPutString(IPDigit);
        XLCDPut('.');
    }
    else
    {
        USARTPutString((BYTE*)IPDigit);
        USARTPut('.');
    }

    itoa(IPVal->v[2], IPDigit);
    if ( bToLCD )
    {
        XLCDPutString(IPDigit);
        XLCDPut('.');
    }
    else
    {
        USARTPutString((BYTE*)IPDigit);
        USARTPut('.');
    }

    itoa(IPVal->v[3], IPDigit);
    if ( bToLCD )
        XLCDPutString(IPDigit);
    else
        USARTPutString((BYTE*)IPDigit);
}

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
    ADCON1  = 0b10001110;       // RA0 as analog input, Right justified
    TRISA   = 0x03;

    /*
     * LCD is enabled using RA5.
     */
    PORTA_RA5 = 0;          // Disable LCD.

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

    XLCDInit();
    XLCDGoto(0, 0);
    XLCDPutROMString(StartupMsg);


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
    "\r\n\r\n\r\MCHPStack TFTP Client Demo 1.0 ("STARTUP_MSG", " __DATE__ ")\r\n\r\n"

    "\t1: Change Board serial number.\r\n"
    "\t2: Change default IP address.\r\n"
    "\t3: Change default gateway address.\r\n"
    "\t4: Change default subnet mask.\r\n"
    "\t5: Enable DHCP & IP Gleaning.\r\n"
    "\t6: Disable DHCP & IP Gleaning.\r\n"
    "\t7: Set TFTP Server IP address:\r\n"
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
    MENU_CMD_TFTP_SERVER_ADDRESS,
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
    "\r\nTFTP Serer IP Address (",
    "\r\nNow running application..."
};

ROM char InvalidInputMsg[] = "\r\nInvalid input received - Input ignored.\r\n"
                             "Press any key to continue...\r\n";

void USARTPutROMString(ROM char* str)
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

    case MENU_CMD_TFTP_SERVER_ADDRESS:
        destIPValue = &AppConfig.TFTPServerAddr;
        goto ReadIPConfig;

    case MENU_CMD_QUIT:
        SaveAppConfig();
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
