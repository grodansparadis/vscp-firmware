/*********************************************************************
*
*       Example SNMP Agent Application using Microchip TCP/IP Stack
*
*********************************************************************
* FileName:        DemoSNMPAgent.c
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
* Nilesh Rajbharti     5/23/03      Original (Rev. 1.0)
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
#include "stacktsk.h"
#include "tick.h"
#include "snmp.h"
#include "mib.h"
#include "icmp.h"
#include "delay.h"
#include "mpfs.h"
#include "xlcd.h"

#if defined(STACK_USE_DHCP)
#include "dhcp.h"
#endif


#if defined(MPFS_USE_EEPROM)
#include "xeeprom.h"
#endif

#define DEMO_SNMP_MSG   "DemoSNMP 1.0"
ROM char StartupMsg[] = DEMO_SNMP_MSG;

#if defined(STACK_USE_DHCP) || defined(STACK_USE_IP_GLEANING)
ROM char DHCPMsg[] = "DHCP/Gleaning...";
#endif

ROM char SetupMsg[] = "Board Setup...";

/*
 * These are hardware I/O labels to ease access.
 */
#define SW_S3               PORTB_RB5
#define LED_D5_CONTROL      LATA2
#define LED_D6_CONTROL      LATA3

/*
 * Analog pots.
 */
static WORD_VAL AN0Value;
static WORD_VAL AN1Value;


/*
 * LCD Display
 */
#define LCD_DISPLAY_LEN     (16)
char LCDDisplayString[LCD_DISPLAY_LEN] = DEMO_SNMP_MSG;
BYTE LCDDisplayStringLen = sizeof(DEMO_SNMP_MSG)-1;

/*
 * Trap information.
 * This table maintains list of intereseted receivers
 * who should receive notifications when some interesting
 * event occurs.
 */
#define TRAP_TABLE_SIZE         (2)
#define MAX_COMMUNITY_LEN       (8)
typedef struct _TRAP_INFO
{
    BYTE Size;
    struct
    {
        BYTE communityLen;
        char community[MAX_COMMUNITY_LEN];
        IP_ADDR IPAddress;
        struct
        {
            unsigned int bEnabled : 1;
        } Flags;
    } table[TRAP_TABLE_SIZE];
} TRAP_INFO;

/*
 * Initialize trap table with no entries.
 */
TRAP_INFO trapInfo = { TRAP_TABLE_SIZE };

//                         1234567890123456
ROM char blankLCDLine[] = "                ";



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

static BOOL SendNotification(BYTE receiverIndex, SNMP_ID var, SNMP_VAL val);

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
        XLCDGoto(1, 0);
        XLCDPutROMString(SetupMsg);

        SetConfig();
    }

    StackInit();

    SNMPInit();


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
         * This task performs common SNMP Agent tasks.  When
         * a request is received and processed,
         * it callbacks to application.
         */
        SNMPTask();

        /*
         * More Microchip Stack tasks may be added as required.
         */

         /*
          * Add your application speicifc tasks here.
          * This task monitors I/O and notifies remote NMS
          * if configured to do so.
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

            if ( AppConfig.Flags.bIsDHCPEnabled )
            {
                XLCDGoto(1, 14);
                if ( myDHCPBindCount < 0x0a )
                    XLCDPut(myDHCPBindCount + '0');
                else
                    XLCDPut(myDHCPBindCount + 'A');
            }
        }

    }
}


static BOOL SendNotification(BYTE receiverIndex, SNMP_ID var, SNMP_VAL val)
{
    static enum { SM_PREPARE, SM_NOTIFY_WAIT } smState = SM_PREPARE;
    IP_ADDR IPAddress;

    // Convert local to network order.
    IPAddress.v[0] = trapInfo.table[receiverIndex].IPAddress.v[3];
    IPAddress.v[1] = trapInfo.table[receiverIndex].IPAddress.v[2];
    IPAddress.v[2] = trapInfo.table[receiverIndex].IPAddress.v[1];
    IPAddress.v[3] = trapInfo.table[receiverIndex].IPAddress.v[0];

    switch(smState)
    {
    case SM_PREPARE:
        SNMPNotifyPrepare(&IPAddress,
                          trapInfo.table[receiverIndex].community,
                          trapInfo.table[receiverIndex].communityLen,
                          MICROCHIP,            // Agent ID Var
                          6,                    // Notification code
                          (DWORD)TickGet());
        smState = SM_NOTIFY_WAIT;

        break;

    case SM_NOTIFY_WAIT:
        if ( SNMPIsNotifyReady(&IPAddress) )
        {
            smState = SM_PREPARE;
            SNMPNotify(var, val, 0);
            return TRUE;
        }
    }
    return FALSE;
}

BOOL SNMPValidate(SNMP_ACTION SNMPAction, char* community)
{
    return TRUE;
}



// Only dynamic variables with ASCII_STRING or OCTET_STRING data type
// needs to be handled.
BOOL SNMPIsValidSetLen(SNMP_ID var, BYTE len)
{
    switch(var)
    {
    case TRAP_COMMUNITY:
        if ( len < MAX_COMMUNITY_LEN+1 )
            return TRUE;
        break;

    case LCD_DISPLAY:
        if ( len < LCD_DISPLAY_LEN+1 )
            return TRUE;
        break;
    }
    return FALSE;
}


// Only dynamic read-write variables needs to be handled.
BOOL SNMPSetVar(SNMP_ID var, SNMP_INDEX index, BYTE ref, SNMP_VAL val)
{
    switch(var)
    {
    case LED_D5:
        LED_D5_CONTROL = val.byte;
        return TRUE;

    case LED_D6:
        LED_D6_CONTROL = val.byte;
        return TRUE;

    case TRAP_RECEIVER_IP:
        // Make sure that index is within our range.
        if ( index < trapInfo.Size )
        {
            // This is just an update to an existing entry.
            trapInfo.table[index].IPAddress.Val = val.dword;
            return TRUE;
        }
        else if ( index < TRAP_TABLE_SIZE )
        {
            // This is an addition to table.
            trapInfo.table[index].IPAddress.Val = val.dword;
            trapInfo.table[index].communityLen = 0;
            trapInfo.Size++;
            return TRUE;
        }
        break;

    case TRAP_RECEIVER_ENABLED:
        // Make sure that index is within our range.
        if ( index < trapInfo.Size )
        {
            // Value of '1' means Enabled".
            if ( val.byte == 1 )
                trapInfo.table[index].Flags.bEnabled = 1;
            // Value of '0' means "Disabled.
            else if ( val.byte == 0 )
                trapInfo.table[index].Flags.bEnabled = 0;
            else
                // This is unknown value.
                return FALSE;
            return TRUE;
        }
        // Given index is more than our current table size.
        // If it is within our range, treat it as an addition to table.
        else if ( index < TRAP_TABLE_SIZE )
        {
            // Treat this as an addition to table.
            trapInfo.Size++;
            trapInfo.table[index].communityLen = 0;
        }

        break;

    case TRAP_COMMUNITY:
        // Since this is a ASCII_STRING data type, SNMP will call with
        // SNMP_END_OF_VAR to indicate no more bytes.
        // Use this information to determine if we just added new row
        // or updated an existing one.
        if ( ref ==  SNMP_END_OF_VAR )
        {
            // Index equal to table size means that we have new row.
            if ( index == trapInfo.Size )
                trapInfo.Size++;

            // Length of string is one more than index.
            trapInfo.table[index].communityLen++;

            return TRUE;
        }

        // Make sure that index is within our range.
        if ( index < trapInfo.Size )
        {
            // Copy given value into local buffer.
            trapInfo.table[index].community[ref] = val.byte;
            // Keep track of length too.
            // This may not be NULL terminate string.
            trapInfo.table[index].communityLen = (BYTE)ref;
            return TRUE;
        }
        break;

    case LCD_DISPLAY:
        // Copy all bytes until all bytes are transferred
        if ( ref != SNMP_END_OF_VAR )
        {
            LCDDisplayString[ref] = val.byte;
            LCDDisplayStringLen++;
        }
        else
        {
            XLCDGoto(0, 0);
            XLCDPutROMString(blankLCDLine);
            XLCDGoto(0, 0);
            XLCDPutString(LCDDisplayString);
        }

        return TRUE;

    }

    return FALSE;
}

// Only sequence index needs to be handled in this function.
BOOL SNMPGetNextIndex(SNMP_ID var, SNMP_INDEX *index)
{
    SNMP_INDEX tempIndex;

    tempIndex = *index;

    switch(var)
    {
    case TRAP_RECEIVER_ID:
        // There is no next possible index if table itself is empty.
        if ( trapInfo.Size == 0 )
            return FALSE;

        // INDEX_INVALID means start with first index.
        if ( tempIndex == SNMP_INDEX_INVALID )
        {
            *index = 0;
            return TRUE;
        }
        else if ( tempIndex < (trapInfo.Size-1) )
        {
            *index = tempIndex+1;
            return TRUE;
        }
        break;
    }
    return FALSE;
}


BOOL SNMPGetVar(SNMP_ID var, SNMP_INDEX index, BYTE *ref, SNMP_VAL* val)
{
    BYTE myRef;

    myRef = *ref;

    switch(var)
    {
    case SYS_UP_TIME:
        val->dword = TickGet();
        return TRUE;

    case LED_D5:
        val->byte = LED_D5_CONTROL;
        return TRUE;

    case LED_D6:
        val->byte = LED_D6_CONTROL;
        return TRUE;

    case PUSH_BUTTON:
        // There is only one button - meaning only index of 0 is allowed.
        val->byte = SW_S3;
        return TRUE;

    case ANALOG_POT0:
        val->word = AN0Value.Val;
        return TRUE;

    case ANALOG_POT1:
        val->word = AN1Value.Val;
        return TRUE;

    case TRAP_RECEIVER_ID:
        if ( index < trapInfo.Size )
        {
            val->byte = index;
            return TRUE;
        }
        break;

    case TRAP_RECEIVER_ENABLED:
        if ( index < trapInfo.Size )
        {
            val->byte = trapInfo.table[index].Flags.bEnabled;
            return TRUE;
        }
        break;

    case TRAP_RECEIVER_IP:
        if ( index < trapInfo.Size )
        {
            val->dword = trapInfo.table[index].IPAddress.Val;
            return TRUE;
        }
        break;

    case TRAP_COMMUNITY:
        if ( index < trapInfo.Size )
        {
            if ( trapInfo.table[index].communityLen == 0 )
                *ref = SNMP_END_OF_VAR;
            else
            {
                val->byte = trapInfo.table[index].community[myRef];

                myRef++;

                if ( myRef == trapInfo.table[index].communityLen )
                    *ref = SNMP_END_OF_VAR;
                else
                    *ref = myRef;
            }
            return TRUE;
        }
        break;

    case LCD_DISPLAY:
        if ( LCDDisplayStringLen == 0 )
            myRef = SNMP_END_OF_VAR;
        else
        {
            val->byte = LCDDisplayString[myRef++];
            if ( myRef == LCDDisplayStringLen )
                myRef = SNMP_END_OF_VAR;
        }

        *ref = myRef;
        return TRUE;

    }

    return FALSE;
}



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



static void ProcessIO(void)
{
    static BOOL lbNotify = FALSE;
    static BYTE i = 0;
    SNMP_VAL val;

    if ( PORTB_RB5 == 0 && !lbNotify )
        lbNotify = TRUE;

    if ( i == trapInfo.Size )
    {
        i = 0;
        lbNotify = FALSE;
    }

    if ( lbNotify )
    {
        if ( trapInfo.table[i].Flags.bEnabled )
        {
           val.byte = 0;
            if ( SendNotification(i, PUSH_BUTTON, val) )
                i++;
        }
        else
            i++;
    }

    /*
     * Select AN0 channel, Fosc/32 clock
     */
    ADCON0      = 0b10000001;

    /*
     * Wait for acquisition time.
     * Here, rather than waiting for exact time, a simple wait is
     * used.  Real applications requiring high accuracy should
     * calculate exact acquisition time and wait accordingly.
     */
    AN0Value.v[0] = 100;
    while( AN0Value.v[0]-- );

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
    AN0Value.v[0] = ADRESL;
    AN0Value.v[1] = ADRESH;

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
    ADCON1      = 0b10000100;

    /*
     * Select AN1 channel.
     */
    ADCON0      = 0b10001001;

    /*
     * Wait for acquisition time.
     * Here, rather than waiting for exact time, a simple wait is
     * used.  Real applications requiring high accuracy should
     * calculate exact acquisition time and wait accordingly.
     */
    AN1Value.v[0] = 100;
    while( AN1Value.v[0]-- );

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
    AN1Value.v[0] = ADRESL;
    AN1Value.v[1] = ADRESH;

    /*
     * Reset RA2 pin back to digital output.
     */
    ADCON1      = 0b10001110;       // RA0 as analog input.

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
    "\r\n\r\n\r\MCHPStack SNMP Agent Demo Application 1.0 (Microchip TCP/IP Stack 2.20+ " __DATE__ ")\r\n\r\n"

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
