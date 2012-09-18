/*********************************************************************
*
*               VSCP Modtronix raw ethernet sample
*
*********************************************************************
* FileName:        main.c
*
* Processor:       PIC18
* Complier:        MPLABX with C18 v1.00.50 or higher
*                  
* Company:         Grodans Paradis AN.
*
* VSCP Sample
* Copyright (C) 2011-2012 Ake Hedman, Grodans Paradis AB
*
**********************************************************************
* File History
*
* 2012-09-18 - Checking and claning.
* 2011 - AKHE Intial History
*********************************************************************/
 
/*
 * Following define uniquely deines this file as main
 * entry/application In whole project, there should only be one such
 * definition and application file must define AppConfig variable as
 * described below.
 */
#define THIS_IS_STACK_APPLICATION

#include <string.h>

#include "projdefs.h"
#include "appcfg.h"
#include "inttypes.h"	// from VSCP firmware/common
#include "debug.h"
#include "debug.h"
#include "io.h"

#include "cpuconf.h"
#include "stacktsk.h"
#include "tick.h"
#include "helpers.h"
#include "serint.h"
#include "i2c.h"

#include "inttypes.h"
#include "vscp_class.h"     
#include "vscp_type.h"
#include "vscp_firmware_level2.h"
#include "vscp_raw_ethernet.h"
#include "vscpmain.h"

extern APP_CONFIG AppConfig;  // from appcfg.c

#if defined( APP_USE_ADC8 )
extern uint8_t AdcValues[ ADC_CHANNELS ];
#elif defined( APP_USE_ADC10 )
extern uint16_t AdcValues[ ADC_CHANNELS ];
#endif

/////////////////////////////////////////////////
// Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xD6, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xD6, msgCode, msgStr)


/////////////////////////////////////////////////
// Global variables used for alive counters.
BYTE aliveCntrMain; //Alive counter must be reset each couple of ms to prevent
                    // board reset. Set to 0xff to disable.
BOOL aliveCntrDec;

MAC_ADDR broadcastTargetMACAddr;

/////////////////////////////////////////////////
// Version number
// - n = major part, and can be 1 or 2 digets long
// - mm is minor part, and must be 2 digets long!
///ROM char APP_VER_STR[] = "V0.1";

// From vscp_2.c
extern uint8_t vscp_function_flags;
extern uint16_t vscp_page_select;		// Current register page

static void InitializeBoard(void);
static void ProcessIO(void);

/////////////////////////////////////////////////
//High Interrupt ISR
#if defined(MCHP_C18)
    #pragma interrupt HighISR
    void HighISR(void)
#elif defined(HITECH_C18)
    #if defined(STACK_USE_SLIP)
        extern void MACISR(void);
    #endif
    void interrupt HighISR(void)
#endif
{
    /////////////////////////////////////////////////
    //TMR0 is used for the ticks
    if (INTCON_TMR0IF)
    {
        TickUpdate();

        #if defined(STACK_USE_SLIP)
        MACISR();
        #endif

        INTCON_TMR0IF = 0;
    }


    /////////////////////////////////////////////////
    //USART1
    if(PIR1_RCIF && PIE1_RCIE)  //USART1 Receive
    {
        serRxIsr();
    }

    //USART1 Transmit
    if(PIR1_TXIF && PIE1_TXIE)
    {
        serTxIsr();
    }


    #if defined(BRD_SBC65EC)
    /////////////////////////////////////////////////
    //USART2
    if(PIR3_RC2IF && PIE3_RC2IE)  //USART2 Receive
    {
        ser2RxIsr();
    }

    //USART2 Transmit
    if(PIR3_TX2IF && PIE3_TX2IE)
    {
        ser2TxIsr();
    }
    #endif
}

#if defined(MCHP_C18)
#pragma code highVector = HIVECTOR_ADR
void HighVector (void)
{
    _asm goto HighISR _endasm
}
#pragma code /* return to default code section */
#endif

/*
 * Fast User process. Place all high priority code that has to be called often
 * in this function.
 *
 * !!!!! IMPORTANT !!!!!
 * This function is called often, and should be very fast!
 */
void fastUserProcess(void)
{
    T1CON_RD16 = 0; //Enable reading TMR1L and TMR1H individually

    //Decrement all alive counters every 26.2144ms x 2 = 52.42ms.
    //Timer1 is a free running timer that is incremented every 800ns. Bit 7
    // of TMR1H is toggled every 3.2512ms.
    if (TMR1H & 0x80) {
        if (aliveCntrDec) {
            aliveCntrDec = 0;

            //Decrement all alive counters. If alive counter is 0xff, it is
            // disabled
            if ((aliveCntrMain!=0xff) && (aliveCntrMain!=0)) aliveCntrMain--;
        }
    }
    else
        aliveCntrDec=1;

    //Only clear watchdog timer if no alive counters are 0
    if (aliveCntrMain!=0)   {
        CLRWDT();
    }
}



/*
 * Main entry point.
 */
void main(void)
{
    vscpEvent vscpevent;

    static TICK tickHeartBeat = 0xffffffff;
    static BYTE testLED;
   
    testLED = 1;

    // Destination address - Always MAC broadcast address
    broadcastTargetMACAddr.v[ 0 ] = 0xff;
    broadcastTargetMACAddr.v[ 1 ] = 0xff;
    broadcastTargetMACAddr.v[ 2 ] = 0xff;
    broadcastTargetMACAddr.v[ 3 ] = 0xff;
    broadcastTargetMACAddr.v[ 4 ] = 0xff;
    broadcastTargetMACAddr.v[ 5 ] = 0xff;

    //Set SWDTEN bit, this will enable the watch dog timer
    WDTCON_SWDTEN = 1;
    aliveCntrMain = 0xff;   //Disable alive counter during initialization. Setting to 0xff disables it.

    //Initialize any application specific hardware.
    InitializeBoard();

    //Initialize all stack related components. Following steps must
    //be performed for all applications using PICmicro TCP/IP Stack.
    TickInit();

    //Initialize serial ports early, because they could be required for debugging
    if (appcfgGetc(APPCFG_USART1_CFG & APPCFG_USART_ENABLE)) {
        appcfgUSART();              //Configure the USART1
    }

    #if defined(BRD_SBC65EC)
    if (appcfgGetc(APPCFG_USART2_CFG & APPCFG_USART_ENABLE)) {
        appcfgUSART2();             //Configure the USART2
    }
    #endif

    //After initializing all modules that use interrupts, enable global interrupts
    INTCON_GIEH = 1;
    INTCON_GIEL = 1;

    //Initialize Stack and application related NV variables.
    appcfgInit();

    //First call appcfgCpuIOValues() and then only appcfgCpuIO()!!! This ensures the value are set, before enabling ports.
    appcfgCpuIOValues();    //Configure the CPU's I/O port pin default values
    appcfgCpuIO();          //Configure the CPU's I/O port pin directions - input or output
    
    appcfgADC();            //Configure ADC unit
    appcfgPWM();            //Configure PWM Channels

    MACInit();

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
    while ( 1 ) {

        //aliveCntrMain = 38;     //Reset if not services in 52.42ms x 38 = 2 seconds
        aliveCntrMain = 0xff;
        CLRWDT();

        // Check for event
        if ( vscp_getRawPacket( &vscpevent ) ) {
            feedVSCP();
        }

        // Send heartbeat every 30 seconds
        if ( TickGetDiff( tickHeartBeat ) >= ( TICKS_PER_SECOND * 30 ) ) {

            //vscpevent.head = VSCP_PRIORITY_NORMAL;
            //vscpevent.vscp_class = VSCP_CLASS2_LEVEL1_INFORMATION;
            //vscpevent.vscp_type = VSCP_TYPE_INFORMATION_NODE_HEARTBEAT;
            //vscpevent.sizeData = 3;
            //vscpevent.data[ 0 ] = 0;
            //vscpevent.data[ 1 ] = 0; // Zone
            //vscpevent.data[ 2 ] = 0; // subzone
            //vscp_sendRawPacket( &vscpevent );
            //SendTestVSCPPacket();
	            
            tickHeartBeat = TickGet();
/*                
          	//If B6 is configured as input, change to output
            if (appcfgGetc(APPCFG_TRISB) & 0x40) {
            	appcfgPutc(APPCFG_TRISB, appcfgGetc(APPCFG_TRISB) & 0b10111111);
            }
          
            TRISB_RB6 = 0;
            LATB6 ^= 1;     //Toggle
                
          	//Toggle IOR5E LED, if IOR5E is present
          	if (appcfgGetc(APPCFG_XBRD_TYPE) == XBRD_TYPE_IOR5E) {
            	ior5eLatchData.bits.ledPWR ^= 1;    // Toggle
           	}
*/           	
      	}

        // Do MAC work
        StackTask();
        //MACTask();;

        //I2C Task
        i2cTask();

        //Add your application specific tasks here.
        ProcessIO();

        // Do VSCP periodic tasks
        periodicVSCPWork();

    }
}


static void ProcessIO(void)
{
    static TICK8 tmr10ms = 0;
    
    //Enter each 10ms
    if ( TickGetDiff8bit(tmr10ms) >= ((TICK8)TICKS_PER_SECOND / (TICK8)100) )
    {
        tmr10ms = TickGet8bit();


    }
    

    //Convert next ADC channel, and store result in adcChannel array
    #if (defined(APP_USE_ADC8) || defined(APP_USE_ADC10)) && (ADC_CHANNELS > 0)
    {
        static BYTE adcChannel; //Current ADC channel. Value from 0 - n
        static BOOL ADC_Wait = 0;

        //Increment ADCChannel and start new convertion
        if (!ADC_Wait)
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
                ADC_Wait  = 1;
            }
            //Not ADC channel, set to 0
            else {
                AdcValues[ adcChannel ] = 0;
            }
        }
        
        //End of ADC Convertion: save data
        if ( ( ADC_Wait ) && ( !ADCON0_GO ) ) {
#if defined(APP_USE_ADC8)
            AdcValues[adcChannel] = ADRESH;
#elif defined(APP_USE_ADC10)
            //AdcValues[adcChannel] = (WORD) ((((WORD)ADRESH) << 8) | ((WORD)ADRESL));
            //AdcValues[adcChannel] = (WORD)((ADRESH*256)+ADRESL);
            AdcValues[adcChannel] = ((WORD)ADRESH << 8) | (WORD)ADRESL;
#endif
            ADC_Wait = 0;
        }
    }
    #endif
}



/**
 * Initialize the boards hardware
 */
static void InitializeBoard(void)
{
    #if (defined(MCHP_C18) && (defined(__18F458) || defined(__18F448) || defined(__18F6680))) \
        || (defined(HITECH_C18) && (defined(_18F458) || defined(_18F448) || defined(_18F6680)))
        CMCON  = 0x07; /* Comparators off CM2:CM0 = 111 for PIC 18F448, 18F458, 18F6680 */
    #endif


    /////////////////////////////////////////////////
    //Enable 4 x PLL if configuration bits are set to do so
    #if ( defined(MCHP_C18) && defined(__18F6621))
    OSCCON = 0x04;              //Enable PLL (PLLEN = 1)
    while (OSCCON_LOCK == 0);   //Wait until PLL is stable (LOCK = 1)
    
    //Seeing that this code does currently not work with Hi-Tech compiler, disable this feature
    OSCCON_SCS1 = 1;
    //Switch on software 4 x PLL if flag is set in configuration data
    //if (appcfgGetc(APPCFG_SYSFLAGS) & APPCFG_SYSFLAGS_PLLON) {
    //    OSCCON_SCS1 = 1;
    //}
    #endif
    

    /////////////////////////////////////////////////
    //Setup timer1 as a free running 16 bit timer.
    //TMR1L is incremented every 800ns. TMR1H is incremented every 204.8us
    //Is used to measure events in code.
    //TMR1H.bit0 is toggled every 204.8us
    //TMR1H.bit1 is toggled every 409.6us
    //TMR1H.bit2 is toggled every 819.2us
    //TMR1H.bit3 is toggled every 1.6384ms
    //TMR1H.bit4 is toggled every 3.2768ms
    //TMR1H.bit5 is toggled every 6.5536ms
    //TMR1H.bit6 is toggled every 13.1072ms
    //TMR1H.bit7 is toggled every 26.2144ms
    //1xxx xxxx = Enable read/write as a 16bit times. TMR1H is a buffer that is loaded when TMR1L is accessed.
    //xx11 xxxx = No prescaler
    //xxxx 0xxx = Timer1 oscillator disabled (RC0 and RC1)
    //xxxx xx0x = Internal clock source = Fosc/4 = 40/4 = 10MHz for a 40MHz clock
    //xxxx xxx1 = Timer 1 on
    T1CON = 0xB1;

    T1CON_RD16 = 0; //Enable reading TMR1L and TMR1H individually

    //When reading TMR1 in code, execute following command if TMR1L and TMR1H byte has to be read as a single value
    //T1CON_RD16 = 1; //Enable reading TMR1L and TMR1H as 16-bit value
    //If TMR1H has to be read on it's own (without first reading TMR1L), execute following command.
    //T1CON_RD16 = 0; //Enable reading TMR1L and TMR1H individually

    //Disable external pull-ups
    INTCON2_RBPU = 1;

    //Enable T0 interrupt
    T0CON = 0;
}
















