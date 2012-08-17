/*****************************************************************************
 *
 *              Main.c -- user main program
 *
 *****************************************************************************
 * FileName:        BasicDriver.c
 * Dependencies:
 * Processor:       PIC32
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the "Company") is intended and supplied to you, the Company's
 * customer, for use solely and exclusively with products manufactured
 * by the Company.
 *
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *****************************************************************************/
#include <p32xxxx.h>
#include <plib.h> 
#include "Console.h"
#include "MRF24J40.h"
#include "BasicDriverDefs.h"
#include <stdio.h>
#include "mstimer.h"
#include <string.h>


#define SYSCLK  (80000000)
#define PBCLK   (SYSCLK/8)

#pragma romdata longAddressLocation = 0x0E
const unsigned char myLongAddress[8] = {EUI_0,EUI_1,EUI_2,EUI_3,EUI_4,EUI_5,EUI_6,EUI_7};
#pragma romdata


// * * * * * * * * * * * * * * * * * * * *
// * * * * * * *  Config   * * * * * * * *
// * * * * * * * * * * * * * * * * * * * *


/********************************************************************
 * Oscillator Selection bits [FNOSC]
 *
 * FRC      - Fast RC oscillator  
 * FRCPLL   - Fast RC oscillator w/ PLL  
 * PRI      - Primary oscillator (XT, HS, EC)  
 * PRIPLL   - Primary oscillator (XT, HS, EC) w/ PLL  
 * SOSC     - Secondary oscillator  
 * LPRC     - Low power RC oscillator  
 * FRCDIV16 - Fast RC oscillator with divide by 16  
 * FRCDIV   - Fast RC oscillator with divide  
 ********************************************************************/
#pragma config FNOSC = PRIPLL

/********************************************************************
 * PLL Input Divider [FPLLIDIV]
 * The PLL need to have a value between 4 - 5 MHz.  Use this
 * setting to scale down your input frequency.
 * 
 * DIV_1    - Divide by 1  
 * DIV_2    - Divide by 2  
 * DIV_3    - Divide by 3  
 * DIV_4    - Divide by 4  
 * DIV_5    - Divide by 5  
 * DIV_6    - Divide by 6  
 * DIV_10   - Divide by 10  
 * DIV_12   - Divide by 12  
 ********************************************************************/
#pragma config FPLLIDIV = DIV_2

/********************************************************************
 * PLL Multipler [FPLLMUL]
 * 
 * MUL_15 - Multiply by 15  
 * MUL_16 - Multiply by 16  
 * MUL_17 - Multiply by 17  
 * MUL_18 - Multiply by 18  
 * MUL_19 - Multiply by 19  
 * MUL_20 - Multiply by 20  
 * MUL_21 - Multiply by 21  
 * MUL_24 - Multiply by 24  
 ********************************************************************/
#pragma config FPLLMUL = MUL_20

/********************************************************************
 * PLL Output Divider [FPLLODIV]
 * 
 * DIV_1 - Divide by 1  
 * DIV_2 - Divide by 2  
 * DIV_4 - Divide by 4  
 * DIV_8 - Divide by 8  
 * DIV_16 - Divide by 16  
 * DIV_32 - Divide by 32  
 * DIV_64 - Divide by 64  
 * DIV_256 - Divide by 256  
 ********************************************************************/
#pragma config FPLLODIV = DIV_1

/********************************************************************
 * Bootup Peripheral Bus Clock Divider [FPBDIV]
 * 
 * DIV_1 - Divide by 1  
 * DIV_2 - Divide by 2  
 * DIV_4 - Divide by 4  
 * DIV_8 - Divide by 8  
 ********************************************************************/
#pragma config FPBDIV = DIV_1

/********************************************************************
 * Primary Oscillator [POSCMOD]
 * 
 * EC - EC oscillator  
 * XT - XT oscillator  
 * HS - HS oscillator  
 * OFF - Disabled  
 ********************************************************************/
#pragma config POSCMOD = HS

/********************************************************************
 * Secondary Oscillator Enable [FSOSCEN]
 * 
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config FSOSCEN = OFF

/********************************************************************
 * Internal/External Switch Over [IESO]
 * 
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config IESO = OFF

/********************************************************************
 * Clock Switching and Monitor Selection [FCKSM]
 * 
 * CSECME - Clock Switching Enabled, Clock Monitoring Enabled  
 * CSECMD - Clock Switching Enabled, Clock Monitoring Disabled  
 * CSDCMD - Clock Switching Disabled, Clock Monitoring Disabled  
 ********************************************************************/
#pragma config FCKSM = CSDCMD

/********************************************************************
 * Clock Output Enable [OSCIOFNC]
 * 
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config OSCIOFNC = OFF

/********************************************************************
 * Watchdog Timer Enable [FWDTEN]
 * 
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config FWDTEN = OFF

/********************************************************************
 * Watchdog Timer Postscale Select [WDTPS]
 * 
 * PS1         - 1:1  
 * PS2         - 1:2  
 * PS4         - 1:4  
 * PS8         - 1:8  
 * PS16        - 1:16  
 * PS32        - 1:32  
 * PS64        - 1:64  
 * PS128       - 1:128  
 * PS256       - 1:256  
 * PS512       - 1:512  
 * PS1024      - 1:1024  
 * PS2048      - 1:2048  
 * PS4096      - 1:4096  
 * PS8192      - 1:8,192  
 * PS16384     - 1:16,384  
 * PS32768     - 1:32,768  
 * PS65536     - 1:65,536  
 * PS131072    - 1:131,072  
 * PS262144    - 1:262,144  
 * PS524288    - 1:524,288  
 * PS1048576   - 1:1,048,576  
 ********************************************************************/
#pragma config WDTPS = PS1048576

/********************************************************************
 * Boot Flash Protect Enable [BWP]
 *
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config BWP = OFF

/********************************************************************
 * Code Protect Enable [CP]
 *
 * ON - Enabled  
 * OFF - Disabled  
 ********************************************************************/
#pragma config CP = OFF

/********************************************************************
 * ICE/ICD Comm Channel Select [ICESEL]
 * 
 * ICS_PGx1 - ICE pins are shared with PGC1, PGD1  
 * ICS_PGx2 - ICE pins are shared with PGC2, PGD2  
 ********************************************************************/
#pragma config ICESEL = ICS_PGx2

//#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
//#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1


//Revision history
//v2.0 - change menu system. add single tone modulation and sequential transmit mode
//v1.5 - got the sleep mode working for the MRF24J40
//v1.6 - added option to slow down the tranmission rate of continuous transmission
//v1.7 - added option to change the number of packets used in the RSSI calculation
//v1.8 - added FIFO read tests (option 'p' under MRF menu)
//v1.9 - added test for all non-changing RAM locations
//v1.10 - set the default interpacket spacing to as fast as possible,
//          created the SEASOLVE definition to always remove the menu after reset
//v1.11 - added LO Leakage, Carrier and SideBand Suppression test
//          MRF main menu option 'q'
//v1.12 - added "Teds Tests" menu
//v1.13 - added the UBEC FIFO read workaround of clearing ShortReg0x18[7]
//v1.14 - modified to use 100us before TX or RX when warming up the PA/LNA 
#define VERSION "v2.00"

//#define SEASOLVE

/********************** FUNCTION PROTOTYPES *************************/
void BoardInit(void);
void MRF24J40Init(void);
void SetChannel(BYTE channel);
void PHYSetLongRAMAddr(WORD address, BYTE value);
void PHYSetShortRAMAddr(BYTE address, BYTE value);
BYTE PHYGetShortRAMAddr(BYTE address);
BYTE PHYGetLongRAMAddr(WORD address);
void PHYSetDeviceAddress(WORD PANID, WORD shortAddress);


/********************** CONSTANTS ***********************************/
const char PredefinedPacket[] = {0x03,0x08,0xC4,0xFF,0xFF,0xFF,0xFF,0x07,0x00,0x00,0x00,0x00,0x00};
const char PERCommand[] = {0x03,0x08,0xC4,0xFF,0xFF,0xFF,0xFF,0xAA};
const unsigned char testShortRegisters [] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x1C,0x1D,0x1E,0x1F,0x21,0x22,0x23,0x26,0x27,0x28,0x29,0x2E,0x34,0x3D,0x3F};
const unsigned short int testLongRegisters [] = {0x0222,0x0223,0x0224,0x0225,0x0226,0x0227,0x0228,0x022A,0x022B,0x022C,0x0230,0x0231,0x0232,0x0233,0x0234,0x0235,0x0236,0x0237,0x0238,0x0239,0x0240,0x0241,0x0242,0x0243,0x0244,0x0245,0x0246,0x0247,0x0248,0x0249,0x024A,0x024B,0x024C};

//1-100 only
#define RSSI_BUFFER_SIZE 50

/********************** Variables ***********************************/
volatile BYTE rssiWrite;
volatile BYTE rssiBuffer[RSSI_BUFFER_SIZE];
BYTE numRSSISamples;
BYTE input;
volatile DWORD_VAL numPacketsRx;
volatile DWORD_VAL PERnum;
volatile DWORD_VAL PERnumFailed;
volatile BOOL foundPERPacket;
volatile BYTE StartSendingPER = 0;
BOOL PrintMenus;
volatile BOOL ReceivingMode;
BYTE RxPrintMenu = 20;

typedef union
{
    BYTE txData[128];
    struct 
    {
        BYTE ShortReg[sizeof(testShortRegisters)];
        BYTE LongReg[sizeof(testLongRegisters)];
    }RegTest;
} TEST_DATA;
    
TEST_DATA testData;


/******************************************************************************
 * Inline Functions
 ******************************************************************************/

 
 
///////////////////////////////////////////////////////////////////////////////
// openSPI
//
 
static inline void openSPI( unsigned int config1, unsigned int config2 )
{
	OpenSPI1( config1, config2 );
}

///////////////////////////////////////////////////////////////////////////////
// closeSPI
//

static inline void closeSPI( void )
{
	CloseSPI1();
}

///////////////////////////////////////////////////////////////////////////////
// putcSPI
//

static inline void putcSPI( unsigned int data_out )
{
	putcSPI1( data_out );
}

///////////////////////////////////////////////////////////////////////////////
// getcSPI
//

static inline unsigned int getcSPI( void )
{
    return getcSPI1();
}

///////////////////////////////////////////////////////////////////////////////
// changeSPIBRG
//

static inline  __attribute__((always_inline)) void changeSPIBRG( unsigned int brg )
{
	SpiChnSetBrg( 1, brg );
}


///////////////////////////////////////////////////////////////////////////////
// PrintHeader
//

void PrintHeader( const char *app )
{
    BYTE i;
    BYTE startIndex;
    BYTE stringlen = 0;
    
    while ( i = app[stringlen++] );
    stringlen--;

    ConsolePutROMString((const char*)"\r\n****************************************\r\n");    
    ConsolePutROMString((const char*)"* Microchip MRF24J40 Testing Interface *\r\n");
    ConsolePutROMString((const char*)"*            Version: ");
    ConsolePutROMString((const char*)VERSION);
    ConsolePutROMString((const char*)"            *\r\n"); 
    if( stringlen )
    { 
        startIndex = (38 - stringlen) / 2;
        ConsolePutROMString((const char *)"*");
        for(i = 0; i < startIndex; i++)
        {
            ConsolePutROMString((const char*)" ");
        }
        ConsolePutROMString(app);
        for(i = startIndex + stringlen; i < 38; i++)
        {
            ConsolePut(' ');
        }
        ConsolePutROMString((const char *)"*\r\n");
    }
    ConsolePutROMString((const char*)"****************************************\r\n");    
}

/*********************************************************************
 * Function:        void	SpiInitDevice(int chn, int isMaster, int frmEn, int frmMaster)
 *
 * PreCondition:    None
 *
 * Input:           chn			- the SPI channel to use, 1 or 2
 * 					isMaster	-	1: the device is to act as a bus master
 * 									0: the device is an SPI slave
 * 					frmEn		-	1: frame mode is enabled
 * 								-	0: frame mode is disabled
 * 					frmMaster	-	0: if frame mode is enabled, the device is a frame slave (FRMSYNC is input)
 * 									1: if frame mode is enabled, the device is a frame master (FRMSYNC is output)
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Inits the SPI channel 1 to use 16 bit words
 * 					Performs the device initialization in both master/slave modes.
 *
 * Note:            None
 ********************************************************************/
void SpiInitDevice(int chn, int isMaster, int frmEn, int frmMaster)
{
	unsigned int config = SPI_CON_MODE8 | SPI_CON_SMP| SPI_CON_ON;	// SPI configuration word
	
	if ( isMaster ) {
		config |= SPI_CON_MSTEN;
	}

	SpiChnOpen( chn, config, 1 );	// divide fpb by 4, configure the I/O ports. Not using SS in this example

}

#define SYS_FREQ 		(80000000L)

/*********************************************************************
 * Function:        void BoardInit( void )
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Board is initialized
 *
 * Overview:        This function configures the board for the PICDEM-z
 *                  MRF24J40 usage 
 *
 * Note:            This routine needs to be called before the function 
 *                  to initialize MiWi stack or any other function that
 *                  operates on the stack
 ********************************************************************/
void BoardInit( void )
{
	// Configure the device for maximum performance with Cache Enabled.
    // PBCLK divider will be set at 1:1.
    //SYSTEMConfigPerformance( 80000000L );

    SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
   
	MSTimerInit( PBCLK );
	
	// Set up port directions
    mSetupCSDirection();
    mSetupResetDirection();
    mSetupWakeDirection();
    mSetupINTDirection();
        
    //OpenSPI1( SPI_OPEN_CFG_1, SPI_OPEN_CFG_2 );
    SpiChnOpen( 1, SPICON_MSTEN | SPICON_ON, 1 );
    
    //SpiInitDevice(1, 1, 1, 1);
    putcSPI1( 0x55 );
   
/*    
	while( 1 )
    {
    
    }
*/  
    
    INTClearFlag( INT_INT1 );
    mINT1SetIntPriority( 2 );
    mINT1ClearIntFlag();
    mINT1IntEnable( 1 );
    	
    // Enable Interrupts   
    INTEnableSystemMultiVectoredInt();
}

/*********************************************************************
 * Function:        void MRF24J40Init(void)
 *
 * PreCondition:    BoardInit (or other initialzation code is required)
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    MRF24J40 is initialized
 *
 * Overview:        This function initializes the MRF24J40 and is required
 *                  before stack operation is available
 ********************************************************************/
void MRF24J40Init( void )
{  
    WORD i;
    WORD j;
    
    mSetCSOff();
    mSetWakeOff();
    
    mSetResetOn();
    MSTimerWait( 10 );

    mSetResetOff();
    MSTimerWait( 10 );
  
    // do a soft reset
    PHYSetShortRAMAddr( WRITE_SOFTRST, 0x07 );
      
    do
    {
    	i = PHYGetShortRAMAddr( READ_SOFTRST );
    }
    while ( ( i & 0x07 ) != (BYTE)0x00);

    MSTimerWait( 10 );
             
    PHYSetShortRAMAddr( WRITE_RFCTL, 0x04 );
    PHYSetShortRAMAddr( WRITE_RFCTL, 0x00 );
    
    // flush the RX fifo 
    PHYSetShortRAMAddr( WRITE_RXFLUSH, 0x01 );
    
    unsigned bb;
    bb = PHYGetShortRAMAddr( READ_BBREG6 );
    bb = PHYGetShortRAMAddr( READ_INTMSK );
    
    bb = PHYGetShortRAMAddr( WRITE_SADRL );
    PHYSetShortRAMAddr( WRITE_SADRL, 0x55 );
    bb = PHYGetShortRAMAddr( WRITE_SADRL );
    
    // Program the short MAC Address, 0xffff
    PHYSetShortRAMAddr( WRITE_SADRL, 0xFF );
    PHYSetShortRAMAddr( WRITE_SADRH, 0xFF );
    PHYSetShortRAMAddr( WRITE_PANIDL, 0xFF );
    PHYSetShortRAMAddr( WRITE_PANIDH, 0xFF );
    
    // Program Long MAC Address
    for ( i=0; i<(BYTE)8; i++ ) {
        PHYSetShortRAMAddr( WRITE_EADR0 + i*2, myLongAddress[ i ] );
    }

    // program the RF and Baseband Register 
    //PHYSetLongRAMAddr(RFCTRL4,0x02);
    
    // Enable the RX 
    //PHYSetLongRAMAddr(RFRXCTRL,0x01);
    
    // setup     
    PHYSetLongRAMAddr( RFCTRL1, 0x01 );
    PHYSetLongRAMAddr( RFCTRL2, 0x80 );
    
    PHYSetLongRAMAddr( RFCTRL3, 0x00 );
    // program RSSI ADC with 2.5 MHz clock 
    //PHYSetLongRAMAddr(RFCTRL6,0x04);
    PHYSetLongRAMAddr( RFCTRL6, 0x90 );

    PHYSetLongRAMAddr( RFCTRL7, 0x00 );
    //PHYSetLongRAMAddr(RFCTRL7, 0x98 );
    
    PHYSetLongRAMAddr(RFCTRL8, 0x10 );

    // Program CCA mode using RSSI 
    PHYSetShortRAMAddr( WRITE_BBREG2, 0xB8 );
    //PHYSetShortRAMAddr(WRITE_BBREG2,0x80);
    // Enable the packet RSSI 
    PHYSetShortRAMAddr( WRITE_BBREG6, 0x40 );
    
    // Program CCA, RSSI threshold values 
    //PHYSetShortRAMAddr(WRITE_RSSITHCCA,0x60);
    PHYSetShortRAMAddr( WRITE_RSSITHCCA, 0x00 );
    
    // errata issues workaround 
//    PHYSetShortRAMAddr(WRITE_FFOEN, 0x98);
//    PHYSetShortRAMAddr(WRITE_TXPEMISP, 0x95);

    do {
    	i = PHYGetLongRAMAddr( RFSTATE );
    }
    while ( ( i & 0xA0 ) != 0xA0 );
    
    //PHYSetShortRAMAddr(WRITE_RXMCR, 0b00100011);
    
    SetChannel( CHANNEL_11 );

    PHYSetShortRAMAddr( WRITE_INTMSK, 0xF6 );
}


/*********************************************************************
 * Function:        void SetChannel(BYTE channel)
 *
 * PreCondition:    MRF24J40 is initialized
 *
 * Input:           BYTE channel - this is the channel that you wish
 *                  to operate on.  This should be CHANNEL_11, CHANNEL_12,
 *                  ..., CHANNEL_26. 
 *
 * Output:          None
 *
 * Side Effects:    the MRF24J40 now operates on that channel
 *
 * Overview:        This function sets the current operating channel
 *                  of the MRF24J40
 ********************************************************************/
void SetChannel( BYTE channel )
{
    PHYSetLongRAMAddr( RFCTRL0, ( channel | 0x02 ) );
    PHYSetShortRAMAddr( WRITE_RFCTL, 0x04 );
    PHYSetShortRAMAddr( WRITE_RFCTL, 0x00 );
}

/*********************************************************************
 * Function:        void PHYSetLongRAMAddr(WORD address, BYTE value)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           WORD address is the address of the LONG RAM address
 *                      that you want to write to
 *                  BYTE value is the value that you want to write to
 *                      that register
 *
 * Output:          None
 *
 * Side Effects:    The register value is changed
 *
 * Overview:        This function writes a value to a LONG RAM address
 ********************************************************************/
void PHYSetLongRAMAddr( WORD address, BYTE value )
{
    volatile WORD tmpRFIE = INTGetFlag( INT_INT1 );
    INTEnable( INT_INT1, 0 );
    mSetCSOn();
    putcSPI( ( ( (BYTE)( address >> 3 ) ) & 0x7f ) | 0x80 );
    putcSPI( ( ( (BYTE)( address << 5 ) ) & 0xe0 ) | 0x10 );
    putcSPI( value );
    mSetCSOff();
    INTEnable( INT_INT1, tmpRFIE );
}

/*********************************************************************
 * Function:        void PHYSetShortRAMAddr(BYTE address, BYTE value)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           BYTE address is the address of the short RAM address
 *                      that you want to write to.  Should use the
 *                      WRITE_ThisAddress definition in the MRF24J40 
 *                      include file.
 *                  BYTE value is the value that you want to write to
 *                      that register
 *
 * Output:          None
 *
 * Side Effects:    The register value is changed
 *
 * Overview:        This function writes a value to a short RAM address
 ********************************************************************/
void PHYSetShortRAMAddr( BYTE address, BYTE value )
{
    volatile WORD tmpRFIE = INTGetFlag( INT_INT1 );
    INTEnable( INT_INT1, 0 );
    mSetCSOn();       
    putcSPI( ( ( address << 1 ) & 0x7f ) | 0x01  );
    putcSPI( value );
    mSetCSOff();
    INTEnable( INT_INT1, tmpRFIE );
}


/*********************************************************************
 * Function:        void PHYSetShortRAMAddr(BYTE address, BYTE value)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           BYTE address is the address of the short RAM address
 *                      that you want to read from.  Should use the
 *                      READ_ThisAddress definition in the MRF24J40 
 *                      include file.
 *
 * Output:          BYTE the value read from the specified short register
 *
 * Side Effects:    None
 *
 * Overview:        This function reads a value from a short RAM address
 ********************************************************************/
BYTE PHYGetShortRAMAddr( BYTE address )
{
    BYTE toReturn;
    
    volatile BYTE tmpRFIE = INTGetFlag( INT_INT1 );
    INTEnable( INT_INT1, 0 );
    mSetCSOn();
    putcSPI( ( address << 1 ) & 0x7e );
    toReturn = getcSPI();
    mSetCSOff();
    INTEnable( INT_INT1, tmpRFIE );
    return toReturn;
}


/*********************************************************************
 * Function:        BYTE PHYGetLongRAMAddr(WORD address)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           WORD address is the address of the long RAM address
 *                      that you want to read from.  
 *
 * Output:          BYTE the value read from the specified Long register
 *
 * Side Effects:    None
 *
 * Overview:        This function reads a value from a long RAM address
 ********************************************************************/
BYTE PHYGetLongRAMAddr( WORD address )
{
    BYTE toReturn;
    volatile BYTE tmpRFIE = INTGetFlag( INT_INT1 );
    INTEnable( INT_INT1, 0 );
    mSetCSOn();
    putcSPI( ( ( address >> 3 ) & 0x7f ) | 0x80 );
    putcSPI( ( ( address << 5 ) & 0xe0 ) );
    toReturn = getcSPI();
    mSetCSOff();
    INTEnable( INT_INT1, tmpRFIE );
    return toReturn;
}


/*********************************************************************
 * Function:        void PHYSetDeviceAddress(WORD PANID, WORD shortAddress)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           WORD PANID is the PANID you want to operate on
 *                  WORD shortAddress is the short address you want to use on that network
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function sets the short address for the IEEE address
 *                      filtering
 ********************************************************************/
void PHYSetDeviceAddress(WORD PANID, WORD shortAddress)
{
    PHYSetShortRAMAddr( WRITE_SADRL, (BYTE)( PANID & 0x00FF ) );
    PHYSetShortRAMAddr( WRITE_SADRH, (BYTE)( PANID >> 8 ) );
    PHYSetShortRAMAddr( WRITE_PANIDL, (BYTE)( shortAddress & 0x00FF ) );
    PHYSetShortRAMAddr( WRITE_PANIDH, (BYTE)( shortAddress >> 8 ) );
}


/*********************************************************************
 * Function:        void HighISR(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Various flags and registers set.
 *
 * Overview:        This is the interrupt handler for the MRF24J40 and
 *                  MiWi stack.
 ********************************************************************/

INTERRUPT( ipl2,_EXTERNAL_1_VECTOR, Int1_Handler )
{       
    if ( INTGetFlag( INT_INT1 ) ) {
	                        
        // clear the interrupt flag as soon as possible such that another interrupt can
        // occur quickly.
        INTClearFlag( INT_INT1 );
        
        // create a new scope for the MRF24J40 interrupts so that we can clear the interrupt
        // flag quickly and then handle the interrupt that we have already received
        {
            MRF24J40_IFREG flags;        

            //read the interrupt status register to see what caused the interrupt        
            flags.Val = PHYGetShortRAMAddr( READ_ISRSTS );
            //PrintChar(flags.Val);
           
            if ( flags.bits.RF_TXIF )
            {
                // if the TX interrupt was triggered
                BYTE_VAL results;
                
                // read out the results of the transmission
                results.Val = PHYGetShortRAMAddr( READ_TXSR );
                
                if ( results.bits.b0 == 1 )
                {
                    //the transmission wasn't successful and the number
                    //of retries is located in bits 7-6 of TXSR
                    //failed to Transmit
                }
                else
                {
                    //transmission successful
                    //MAC ACK received if requested else packet passed CCA
                }
            }
            
            if ( flags.bits.RF_RXIF ) {   
                BYTE i,j,k,RSSI_VAL;
                WORD p;
                DWORD_VAL v;
                   
                //packet received
                //Need to read it out
                
                i=PHYGetLongRAMAddr( 0x300 ); //first byte of the receive buffer is the packet length (this 
                j=PHYGetLongRAMAddr( 0x308 );
                numPacketsRx.Val++;

                RSSI_VAL = PHYGetLongRAMAddr( 0x300 + i + 2 );
                
                if ( j == 0xAA ) {
                    StartSendingPER = 1; 
                }
                else {
                    if( ReceivingMode ) {
						if ( RxPrintMenu++ == 20 ) {
							putsUART1("\r\nRSSI | Packet Length | Packet Data\r\n\r\n");
							RxPrintMenu = 0;
						}
                        PrintChar(RSSI_VAL);
						putsUART1(" | ");
                        PrintChar(i);   //print the packet length
						putsUART1(" | ");
                        for ( j=1; j<=i; j++ ) {
                            PrintChar( PHYGetLongRAMAddr( 0x300 + j ) );    //read out the rest of the buffer
                        }
                        putsUART1("\r\n");
                    }
                }
                
                rssiBuffer[rssiWrite++] = RSSI_VAL;
                if ( rssiWrite >= numRSSISamples ) {
                    rssiWrite = 0;
                }
                PHYSetShortRAMAddr( WRITE_RXFLUSH, 0x01 );
            
            } //end of RXIF check
            
        } //end of scope of RF interrupt handler
        
    } //end of if(RFIE && RFIF)
    
} //end of interrupt handler




 
///////////////////////////////////////////////////////////////////////////////
// main
//

int main(void)
{   
    BYTE channel;
    BYTE power;
    BYTE mode;
    BOOL UseExternal;
    BOOL AutoCRC;
    BYTE packetDelay;
    BYTE i;
    char buf[32];
    
    for (i=0; i<RSSI_BUFFER_SIZE; i++ ) {
        rssiBuffer[ i ] = 0;
    }

    // decide if it is a hard reset or soft reset
    if ( ( RCON & 0x03/*0b00000011*/ ) != 0x03/*0b00000011*/ ) {   
        // hard reset, intialize the variables     
        AutoCRC = TRUE;    
        PrintMenus = FALSE;    
        UseExternal = FALSE;
        ReceivingMode = FALSE;
        RCON |= 0x03/*0b00000011*/;
    }
    else {
        // soft reset, keep them the way they were
        RCON |= 0x03/*0b00000011*/;
    }
    
    numPacketsRx.Val = 0;
    rssiWrite = 0;
    PERnumFailed.Val = 0;
    PERnum.Val = 0;
    foundPERPacket = FALSE;
    numRSSISamples = 1;
    
    // Enable PORTB pull-ups (INTCON2bits.RBPU)
    //RBPU = 0;

    // Make the PORTB switch connections inputs.
    //TRISB4 = 1;
    //TRISB5 = 1;
    
    packetDelay = 1;
    
#if defined( SEASOLVE )
    PrintMenus = FALSE;
#else 
    PrintMenus = TRUE;
#endif
    
    BoardInit(); 
    ConsoleInit();

MainMenu:

#if defined( ENABLE_CONSOLE )
    if ( PrintMenus ) {
        putsUART1("\r\n");
        putsUART1("\r\n");
        putsUART1("\r\n");
    }
    else {
        putsUART1("+");
    }
#endif    

    
MRF24J40_Start: 
    
    //****************//
#if defined( ENABLE_CONSOLE )    
    if ( PrintMenus ) {
        putsUART1("Trying to initialize the MRF24J40\r\n\r\n");
    }    
#endif
    
    MRF24J40Init();
                        
MRF24J40_SubMenu:

#if defined( ENABLE_CONSOLE )
    if ( PrintMenus ) {
        PrintHeader((const char *)"Main Menu");
        putsUART1("    (a) Settings\r\n");
        putsUART1("    (b) Testings\r\n");
        putsUART1(">>");
    }
#endif
    
    while ( !ConsoleIsGetReady() ) {
        
        INTEnableInterrupts();
        if ( mPORTEReadBits( BIT_8 ) ) {
            INTSetFlag( INT_INT1 );
        } 
        
        input = ' ';
        if ( StartSendingPER ) {
        
            //goto SendingPER;     
            while ( 1 ) {
                BYTE i;
                
                RA1 = 1;
                // tx packets fast
                PHYSetLongRAMAddr(0x000,sizeof(PredefinedPacket));
                PHYSetLongRAMAddr(0x001,sizeof(PredefinedPacket));
                
                for (i=2; i<(sizeof(PredefinedPacket)+2); i++ ) {
                    PHYSetLongRAMAddr(i, PredefinedPacket[i-2]); /* Dest Address  */
                }

                if ( UseExternal ) {
                    PHYSetShortRAMAddr( WRITE_GPIO, 0x03/*0b00000011*/ ); //TX
                    Delay100TCYx(4);
                    putsUART1("Using external PA\r\n");
                }
                
                //PHYSetShortRAMAddr( WRITE_TXNMTRIG, 0x01/*0b00000001*/ );  //transmit packet without ACK requested 

                {
                    unsigned int blah;
                    
                    blah = 0;
                    
                    while ( 1 )
                    {
                        DWORD j;
                        DWORD k;
                        
                        if ( ConsoleIsGetReady() ) {
                            if ( PrintMenus ) {
                                goto MRF24J40_SubMenu;
                            }
                            else {
                                goto MRF24J40_NewOption;
                            }
                        }
                        
                        k = ((DWORD)packetDelay)*((DWORD)packetDelay);
                        
                        do
                        {
                            for(j=0;j<2000;j++)
                            {
                            }
                        }
                        while ( k-- );

                        PHYSetShortRAMAddr( WRITE_TXNMTRIG, 0x01/*0b00000001*/ );  //transmit packet without ACK requested 

                        blah++;
                        if ( ( blah % 100 ) == 0 ) {
                            for ( j = 0; j < 0xFFFF; j++ ) {}
                        }
                        
                        if ( blah == 1000 ) {
                            break;
                        }
                    }
                }
                
                RA1 = 0;
                StartSendingPER = 0;
                putsUART1("Completed sending 1000 packets\r\n");
                
                if ( UseExternal ) {
                    PHYSetShortRAMAddr( WRITE_GPIO, 0x0c/*0b00001100*/ ); //RX
                    Delay100TCYx(4);
                    putsUART1("Using external LNA\r\n");   
                }
                goto MRF24J40_SubMenu;                       
            }  
        }
    }
    
MRF24J40_NewOption:        

    input = ConsoleGet();
    ConsolePut(input);

    if ( PrintMenus ) {
        putsUART1("\r\n");
    }

    switch ( input ) {
        case 'a':
        case 'A':
            if ( PrintMenus ) {
                PrintHeader((const char *)"Settings");
                putsUART1("    (a) Set the Operating Channel\r\n");
                putsUART1("    (b) Set the Output Power\r\n");
                putsUART1("    (c) Set Hardware CRC checking\r\n");
                putsUART1("    (d) Configure External Power Amplifier and Low Noise Amplifier\r\n");
                putsUART1("    (e) Set Transmission Delay Between Packets\r\n");
                putsUART1("    (f) Set the Number of Averaged RSSI Samples\r\n");
                putsUART1("    (g) Turbo Mode Configuration\r\n");
                putsUART1(">>");
            }
            
            while( !DataRdyUART1() );
            input = ReadUART1();
            
            switch(input) {
                case 'a':
                case 'A':
                
MRF24J40_Channel_submenu:
                
                    if ( PrintMenus ) {
                        PrintHeader((const char *)"Set the Operating Channel");
                        putsUART1("    (a) Channel 11\r\n");
                        putsUART1("    (b) Channel 12\r\n");
                        putsUART1("    (c) Channel 13\r\n");
                        putsUART1("    (d) Channel 14\r\n");
                        putsUART1("    (e) Channel 15\r\n");
                        putsUART1("    (f) Channel 16\r\n");
                        putsUART1("    (g) Channel 17\r\n");
                        putsUART1("    (h) Channel 18\r\n");
                        putsUART1("    (i) Channel 19\r\n");
                        putsUART1("    (j) Channel 20\r\n");
                        putsUART1("    (k) Channel 21\r\n");
                        putsUART1("    (l) Channel 22\r\n");
                        putsUART1("    (m) Channel 23\r\n");
                        putsUART1("    (n) Channel 24\r\n");
                        putsUART1("    (o) Channel 25\r\n");
                        putsUART1("    (p) Channel 26\r\n");
                        putsUART1("    (~) Reset Device\r\n");
                        putsUART1(">>");
                    }
                        
                    while( !DataRdyUART1() );
            		input = ReadUART1();
                    
                    if ( ( input < 0x61 ) || ( input > 0x70 ) ) {
                        if ( input == '~' ) {
                            if(PrintMenus) {
                                putsUART1("Device Reset\r\n");
                            }
                           {WORD i; for (i=0; i<30000; i++ ){} }
                            Reset();
                        }
                        
                        if ( PrintMenus )  {
                            putsUART1("Incorrect Menu Choice: ");
                            ConsolePut(input);
                            putsUART1("\r\n");putsUART1("\r\n");
                        }
                        goto MRF24J40_Channel_submenu;
                    }
                    else
                    {
                        BYTE var;
                        
                        var = ((input-0x61)<<4);
                        PrintChar(var);
                        SetChannel(var);
                        if (PrintMenus)
                        {
                            putsUART1("Channel successfully changed\r\n");
							PrintChar(var);
                        }
                        
                        goto MRF24J40_SubMenu;
                        
                    }
                    break;
                    
                case 'b':
                case 'B':
                
MRF24J40_output_power_submenu:
                    if ( PrintMenus )
                    {
                        PrintHeader((const char *)"Set Output Power");
                        putsUART1("    (a) 0 dB\r\n");
                        putsUART1("    (b) -1.25 dB\r\n");
                        putsUART1("    (c) -2.5 dB\r\n");
                        putsUART1("    (d) -3.75 dB\r\n");
                        putsUART1("    (e) -5 dB\r\n");
                        putsUART1("    (f) -6.25 dB\r\n");
                        putsUART1("    (g) -7.5 dB\r\n");
                        putsUART1("    (h) -8.75 dB\r\n");
                        putsUART1("    ...\r\n");
                        putsUART1("    (z) -31.25 dB\r\n");
                        putsUART1("    (0) -32.5 dB\r\n");
                        putsUART1("    (1) -33.75 dB\r\n");
                        putsUART1("    (2) -35 dB\r\n");
                        putsUART1("    (3) -36.25 dB\r\n");
                        putsUART1("    (4) -37.5 dB\r\n");
                        putsUART1("    (5) -38.75 dB\r\n");
                        putsUART1(">>");
                    }
                    
                    while( !DataRdyUART1() );
            		input = ReadUART1();
                    
                    if((input>=0x61) && (input<=0x7A))
                    {
                        power = ((input-0x61)<<3);
                        PHYSetLongRAMAddr(RFCTRL3,power);
                        if(PrintMenus)
                        {                        
                            putsUART1("Output power successfully changed\r\n");
                        }
                        goto MRF24J40_SubMenu;
                    }
                    else if((input>=0x30) && (input<=0x35))
                    {
                        power = ((input-0x30+0x1A)<<3);
                        PHYSetLongRAMAddr(RFCTRL3,power);
                        if(PrintMenus)
                        {                        
                            putsUART1("Output power successfully changed\r\n");
                        }
                        goto MRF24J40_SubMenu;
                    }
                    else
                    {
                        if(input == '~')
                        {
                            if(PrintMenus)
                            {
                                putsUART1("Device Reset\r\n");
                            }
                            {WORD i; for(i=0;i<30000;i++){}}                            
                            Reset();
                        }                        
                        if(PrintMenus)
                        {
                            putsUART1("Incorrect Menu Choice: ");
                            ConsolePut(input);
                            putsUART1("\r\n");putsUART1("\r\n");
                        }
                        goto MRF24J40_output_power_submenu;
                    }    
                    break;
                
                case 'c':
                case 'C':
MRF24J40_CRC_Checking_submenu:                
                    if( PrintMenus )
                    {
                        PrintHeader((const char *)"Hardware CRC checking");
                        putsUART1("    (a) Disable CRC Checking\r\n"); 
                        putsUART1("    (b) Enable CRC Checking\r\n");
                        putsUART1(">>");
                    }
                    
                    while( !DataRdyUART1() );
            		input = ReadUART1();
                    
                    switch(input)
                    {
                        case 'a':
                            PHYSetShortRAMAddr(WRITE_RXMCR,0xA3);
                            AutoCRC = FALSE;
                            if(PrintMenus)
                            {
                                putsUART1("Automatic CRC is disabled\r\n");
                            }
                            break;
                        case 'b':
                            PHYSetShortRAMAddr(WRITE_RXMCR,0x23);
                            AutoCRC = TRUE;
                            if(PrintMenus)
                            {
                                putsUART1("Automatic CRC is enabled\r\n");
                            }
                            break;
                        default:
                            putsUART1("Incorrect Menu Choice: ");
                            ConsolePut(input);
                            goto MRF24J40_CRC_Checking_submenu;        
                    }
                    goto MRF24J40_SubMenu;
                    
                case 'd':
                case 'D':
                    PHYSetShortRAMAddr( WRITE_GPIO, 0x0c/*0b00001100*/ ); //RX
                    PHYSetShortRAMAddr( WRITE_GPIODIR, 0x0f/*0b00001111*/ );
                    UseExternal = TRUE;
                    if(PrintMenus)
                    {
                        putsUART1("Set device to use External LNA/PA\r\nDevice reset required to remove this feature.\r\n\r\n");
                    }
                    PHYSetShortRAMAddr(WRITE_GPIO, 0x0c/*0b00001100*/ ); //RX
                    MSTimerWait( 1 ); // Delay100TCYx( 4 );
                    putsUART1("Using external LNA\r\n");
                    goto MRF24J40_SubMenu;

                case 'e':
                case 'E':
MRF24J40_Transmission_Delay_submenu:                
                    if(PrintMenus)
                    {
                        PrintHeader((const char *)"Transmission Delay Between Packets");
                        putsUART1("    (0) no delay\r\n");
                        putsUART1("    (1) 1 unit of delay\r\n");
                        putsUART1("    (2) 4 units of delay\r\n");
                        putsUART1("    ...\r\n");
                        putsUART1("    (9) 81 units of delay\r\n");
                        putsUART1("    (a) 100 units of delay\r\n");
                        putsUART1("    (b) 121 units of delay\r\n");
                        putsUART1("    ...\r\n");
                        putsUART1("    (z) 1225 units of delay (approx 1 sec)\r\n");
                        putsUART1(">>");
                    }
                    
                    while( !DataRdyUART1() );
            		input = ReadUART1();
                    
                    if(input<0x30)
                    {
                        if(PrintMenus)
                        {
                            putsUART1("Incorrect Menu Choice: ");
                            ConsolePut(input);
                            putsUART1("\r\n"); putsUART1("\r\n");
                        }
                        goto MRF24J40_Transmission_Delay_submenu;
                    }
                    
                    input -= 0x30;
                    
                    if(input>9)
                    {
                        input-=0x21;
                    }
                    
                    packetDelay = input;
                    
                    goto MRF24J40_SubMenu;

                case 'f':
                case 'F':
                    if( PrintMenus )
                    {
                        PrintHeader((const char *)"RSSI Sample Number");
                        putsUART1("    Please enter 2 decimal digits (1-49)\r\n");
                        putsUART1("      i.e. - enter 05 for 5 samples\r\n");
                        putsUART1(">>");    
                    }
_sampleRSSILoop1:
                    numRSSISamples = 0;                    
                    while(!ConsoleIsGetReady()){}                
                    input = ConsoleGet();
                    switch(input)
                    {
                        case '4':
                            numRSSISamples += 10;
                            //fall through
                        case '3':
                            numRSSISamples += 10;
                            //fall through
                        case '2':
                            numRSSISamples += 10;
                            //fall through
                        case '1':
                            numRSSISamples += 10;
                            //fall through
                        case '0':
                            break;
                        default:
                            goto _sampleRSSILoop1;
                    }
                    
                    ConsolePut(input);
                    
_sampleRSSILoop2:                    
                    while(!ConsoleIsGetReady()){}
                
                    input = ConsoleGet();
                    switch(input)
                    {
                        case '9':
                            numRSSISamples += 1;
                            //fall through
                        case '8':
                            numRSSISamples += 1;
                            //fall through
                        case '7':
                            numRSSISamples += 1;
                            //fall through
                        case '6':
                            numRSSISamples += 1;
                            //fall through                                                                                                              
                        case '5':
                            numRSSISamples += 1;
                            //fall through
                        case '4':
                            numRSSISamples += 1;
                            //fall through
                        case '3':
                            numRSSISamples += 1;
                            //fall through
                        case '2':
                            numRSSISamples += 1;
                            //fall through
                        case '1':
                            numRSSISamples += 1;
                            //fall through
                        case '0':
                            break;
                        default:
                            goto _sampleRSSILoop2;
                    }
                    
                    ConsolePut(input);
                    
                    if(numRSSISamples == 0)
                    {
                        if(PrintMenus)
                        {
                            putsUART1("\r\n\r\ndivide by 0 not allowed.  setting number of samples to 1.\r\n");
                        }
                        
                        numRSSISamples = 1;
                    }
                    
                    putsUART1("\r\n\r\n");
                    
                    goto MRF24J40_SubMenu;
                    
                case 'g':
                case 'G':
MRF24J40_Turbo_Mode_submenu:                
                    if( PrintMenus )
                    {
                        PrintHeader((const char *)"Turbo Mode");
                        putsUART1("    (a) Enable Turbo Mode\r\n");
                        putsUART1("    (b) Disable Turbo Mode\r\n");
                        putsUART1(">>");  
                    }
                    
                    while( !DataRdyUART1() );
            		input = ReadUART1();
                    
                    switch(input)
                    {
                        case 'a':
                        case 'A':
                            PHYSetShortRAMAddr(WRITE_BBREG0, 0x01);
                            PHYSetShortRAMAddr(WRITE_BBREG3, 0x38);
                            PHYSetShortRAMAddr(WRITE_BBREG4, 0x5C);
                            
                            PHYSetShortRAMAddr(WRITE_RFCTL,0x04);
                            PHYSetShortRAMAddr(WRITE_RFCTL,0x00);
                            if( PrintMenus )
                            {
                                putsUART1("\r\nDevice set to turbo mode.\r\n");
                            }
                            break;
                           
                       case 'b':
                       case 'B':
                            PHYSetShortRAMAddr(WRITE_BBREG0, 0x00);
                            PHYSetShortRAMAddr(WRITE_BBREG3, 0xD8);
                            PHYSetShortRAMAddr(WRITE_BBREG4, 0x9C);
                            
                            PHYSetShortRAMAddr(WRITE_RFCTL,0x04);
                            PHYSetShortRAMAddr(WRITE_RFCTL,0x00);
                            if( PrintMenus )
                            {
                                putsUART1("\r\nDevice set to normal mode.\r\n");
                            }
                            break;
                          
                        default:
                            if( PrintMenus )
                            {
                                putsUART1("Incorrect Menu Choice: ");
                                ConsolePut(input);
                                putsUART1("\r\n"); putsUART1("\r\n"); 
                            }
                            goto MRF24J40_Turbo_Mode_submenu;
                    }
                    break;
                    
                default:
                    goto MRF24J40_SubMenu;
            }
            break;
            
            
        case 'b':
        case 'B':
            if(PrintMenus)
            {
                PrintHeader((const char *)"Testings");
                putsUART1("    (a) Set the Radio in Receiving Mode\r\n");
                putsUART1("    (b) Transmit Precoded Packet Continuously\r\n");
                putsUART1("    (c) Transmit Packet Defined by User\r\n");
                putsUART1("    (d) Test Low Power Mode\r\n");
                putsUART1("    (e) Energy Detection on All Channels\r\n");
                putsUART1("    (f) Test Simple Local Oscillator\r\n");
                putsUART1("    (g) Test Single Tone Modulation\r\n");
                putsUART1("    (h) Test Sequential Transmit Mode for All Channels\r\n");
                putsUART1("    (i) PER Test between Two Devices\r\n");
                putsUART1("    (z) Dump Values of Transceiver's Registers\r\n");
                putsUART1(">>");
            }
            
            while( !DataRdyUART1() );
            input = ReadUART1();
            
            switch(input)
            {
                case 'a':
                case 'A':
                    PHYSetShortRAMAddr(WRITE_RXMCR,0x23);
                    PHYSetShortRAMAddr(WRITE_RXFLUSH,0x01);
                    
                    if( UseExternal )
                    {
                        PHYSetShortRAMAddr( WRITE_GPIO, 0x0c/*0b00001100*/ ); //RX
                        MSTimerWait( 1 ); //  Delay100TCYx(4);
                        putsUART1("Using external LNA\r\n");
                    }
                    
                    if(PrintMenus)
                    {
						putsUART1("\r\n");
                        putsUART1("Placing transceiver in receive mode\r\n");
                        putsUART1("Please reset the board to return to menu\r\n");
                        putsUART1("\r\n");
                    }
                    
                    INTEnableInterrupts();       //enable interrupts
                    
                    ReceivingMode = TRUE;
                
                    while(1)
                    {
                        if(ConsoleIsGetReady())
                        {
                            INTDisableInterrupts();
                            ReceivingMode = FALSE;
                            if(PrintMenus)
                            {
                                goto MRF24J40_SubMenu;
                            }
                            else
                            {
                                goto MRF24J40_NewOption;
                            }
                        }
                    }
                    
                    break;
                
                case 'b':
                case 'B':
                    if(PrintMenus)
                    {
						putsUART1("\r\n");
                        ConsolePutROMString((const char*)"Entering transmit mode.\r\n");
                        putsUART1("Press Any key to return to menu\r\n");
                        putsUART1("\r\n");
                    }
                    {
                        BYTE i;
                        
                        PHYSetLongRAMAddr(0x000,sizeof(PredefinedPacket));
                        PHYSetLongRAMAddr(0x001,sizeof(PredefinedPacket));
                        
                        for(i=2;i<(sizeof(PredefinedPacket)+2);i++)
                        {
                            PHYSetLongRAMAddr(i, PredefinedPacket[i-2]); 
                        }
                    }

                    if(UseExternal)
                    {
                        PHYSetShortRAMAddr( WRITE_GPIO, 0x03/*0b00000011*/ ); //TX
                        MSTimerWait( 1 ); // Delay100TCYx(4);
                        putsUART1("Using external PA\r\n");
                    }
                    PHYSetShortRAMAddr( WRITE_TXNMTRIG, 0x01/*0b00000001*/ );  //transmit packet without ACK requested 

        
                    while(1)
                    {
                        unsigned int j;
                        DWORD k;
                        
                        if(ConsoleIsGetReady())
                        {
                            if(PrintMenus)
                            {
                                goto MRF24J40_SubMenu;
                            }
                            else
                            {
                                goto MRF24J40_NewOption;
                            }
                        }
                        
                        k = ((DWORD)packetDelay)*((DWORD)packetDelay);
                        
                        do
                        {
                            for(j=0;j<200;j++)
                            {
                            }
                        }
                        while(k--);

                        PHYSetShortRAMAddr( WRITE_TXNMTRIG, 0x01/*0b00000001*/ );  //transmit packet without ACK requested 
                    }

                    break;
                
                
                case 'c':
                case 'C':
                    if(PrintMenus)
                    {
                        putsUART1("Please enter the packet you want to transmit.\r\n");
                        putsUART1("Use all uppercase HEX values starting from the frame control (no length)\r\n");
                        putsUART1(" Press '=' when you are done.\r\n");
                        putsUART1(">>");
                    }
                    {
                        BYTE pingPong;
                        BYTE txPtr;
                        BYTE A,B;
                        
                        txPtr = 0;
                        pingPong = 0;
                        
                        while(1)
                        {
MRF24J40_read_tx:
                            while(!ConsoleIsGetReady()){}    
                            input = ConsoleGet();
                            
                            switch(input)
                            {
                                case '0':
                                case '1':
                                case '2':
                                case '3':
                                case '4':
                                case '5':
                                case '6':
                                case '7':
                                case '8':
                                case '9':
                                    ConsolePut(input);
                                    B = (input - '0');
                                    break;
                                case 'A':
                                case 'B':
                                case 'C':
                                case 'D':
                                case 'E':
                                case 'F':
                                    ConsolePut(input);
                                    B = (input - 'A' + 0x0A);
                                    break;
                                case '~':
                                    if(PrintMenus)
                                    {
                                        putsUART1("Device Reset");
                                    }
                                    {WORD i; for(i=0;i<30000;i++){}}
                                    Reset();
                                    break;
                                case '=':
                                    if(PrintMenus)
                                    {
                                        putsUART1("\r\n\r\nTransmitting Packet.\r\nReturning to the main menu.\r\n\r\n");
                                    }
                                    {
                                        BYTE i;
                                        
                                        for(i=0;i<txPtr;i++)
                                        {
                                            PHYSetLongRAMAddr(i+2, testData.txData[i]); /* Dest Address  */
                                        }
                                    }
                                    PHYSetLongRAMAddr(0x000,txPtr);
                                    PHYSetLongRAMAddr(0x001,txPtr);
                                    
                                    if(UseExternal)
                                    {
                                        PHYSetShortRAMAddr( WRITE_GPIO, 0x03/*0b00000011*/ ); //TX
                                        MSTimerWait( 1 ); // Delay100TCYx(4);
                                        putsUART1("Using external PA\r\n");
                                    }
                                    
                                    PHYSetShortRAMAddr( WRITE_TXNMTRIG, 0x01/*0b00000001*/ );  //transmit packet without ACK requested 
                                                                        
                                    goto MRF24J40_SubMenu;
                                    break;
                                default:
                                    goto MRF24J40_read_tx;
                                    break;
                            }
                            if(pingPong == 1)
                            {
                                testData.txData[txPtr++] = A+B;
                                pingPong = 0;
                            }
                            else
                            {
                                A = B<<4;
                                pingPong = 1;
                            }
                        }
                    }
                    goto MRF24J40_SubMenu;
                
                case 'd':
                case 'D':

MRF24J40_Low_Power_submenu:
                
                    if ( PrintMenus )
                    {
                        PrintHeader((const char *)"Test Low Power Mode");
                        putsUART1("    (a) Disable Pullups\r\n");
                        putsUART1("    (b) Place MRF in sleep mode\r\n");
                        putsUART1("    (c) Place MRF in reset\r\n");
                        putsUART1("    (d) Place Controller to sleep\r\n");
                        putsUART1("    (e) Wake up the MRF from sleep mode \r\n");
                        putsUART1(">>");
                    }
                    
                    while( !DataRdyUART1() );
            		input = ReadUART1();
            		
                    switch(input)
                    {
                        case 'a':
                        case 'A':
                            if ( PrintMenus )
                            {
                                putsUART1("\r\n");
								putsUART1("Pullups disabled\r\n");
								putsUART1("Reset the device to get out of disabled pullups\r\n");
								putsUART1("\r\n");
                                //RBPU = 1;
                            }
                            break;
                        
                        case 'b':
                        case 'B':
                            if ( PrintMenus )
                            {
                                putsUART1("MRF24J40 in sleep mode\r\n");
								putsUART1("Reset the device to get out of sleep mode\r\n");
                            }
                            
                            RC1 = 0;
                            TRISC1 = 0;
                            
                            PHYSetShortRAMAddr( WRITE_RFCTL, 0x00 );
                            
                            PHYSetShortRAMAddr( WRITE_TXBCNINTL, 0x80 );
                            PHYSetShortRAMAddr( WRITE_RXFLUSH, 0x60/*0b01100000*/ );
                            PHYSetShortRAMAddr( WRITE_SLPACK, 0x80 );
                            break;
                        
                        case 'c':
                        case 'C':
                            if ( PrintMenus )
                            {
                                putsUART1("MRF24J40 in reset mode\r\n");
                            }
                            
                            mSetResetOn();
                                
                            break;
                        
                        case 'd':
                        case 'D':
                            if ( PrintMenus )
                            {
								putsUART1("\r\n");
                                putsUART1("Placing controller in sleep\r\n");
                                putsUART1("Reset the device to get out of sleep mode\r\n");
                            }
                            {
                                unsigned int i;
                                
                                for ( i=0; i<60000; i++ )
                                {
                                    asm("NOP");/*Nop();*/
                                }
                                
                                //Sleep();
                                
                            }
                            break;
                        
                        case 'e':
                        case 'E':
                            if(PrintMenus)
                            {
                                putsUART1("Waking up the MRF24J40\r\n");
                            }
                            RC1 = 1;
                            break;
                        
                        default:
                            if( PrintMenus )
                            {
                                putsUART1("Incorrect Menu Choice: ");
                                ConsolePut(input);
                                putsUART1("\r\n"); putsUART1("\r\n");
                            }
                            goto MRF24J40_Low_Power_submenu;
                        
                    }
                    break;
                
                case 'e':
                case 'E':
                     {
                        BYTE currentChannel = 0;
                        BYTE i, j;
                        WORD k;
                        BYTE maxRSSI;

                        putsUART1("Please wait!\r\n");
						putsUART1("Spectral energy values will be displayed for each channel in RSSI units:\r\n");

						putsUART1("\r\n"); putsUART1("\r\n");
                        for(i = currentChannel; i <= 15; i++)
                        {
                            BYTE RSSIcheck;
                            BYTE r[2] = {0x54, 0x00};
                            
                            maxRSSI = 0;
                            // set channel
                            j = i << 4;
                            SetChannel(j);
                            for(k = 0; k < 0xFFF; k++); 
                            
                            for(j = 0; j < 200; j++)
                            {
                                // set RSSI request
                                PHYSetShortRAMAddr(WRITE_BBREG6, 0x80);
                                
                                // check RSSI
                                RSSIcheck = PHYGetShortRAMAddr(READ_BBREG6);
                                while((RSSIcheck & 0x01) != 0x01 || (RSSIcheck & 0x80))
                                {
                                    RSSIcheck = PHYGetShortRAMAddr(READ_BBREG6);
                                    
                                }
                                RSSIcheck = PHYGetLongRAMAddr(0x210);
                                
                                if( RSSIcheck > maxRSSI )
                                {
                                    maxRSSI = RSSIcheck;
                                }
                                for(k = 0; k < 0xFFF; k++);
                                
                            }
                            ConsolePutROMString((const char *)"Channel ");

							if(i>=9)
							{
								ConsolePut('2');
							}
							else
							{
								ConsolePut('1');
							}

							{
								unsigned a;

								a = 0x30 + ((i+1)%10);
								ConsolePut(a);
							}

                            //PrintChar(i+11);
                            ConsolePutROMString((const char *)": ");
                            j = maxRSSI/5;
                            for(k = 0; k < j; k++)
                            {
                                ConsolePut('-');
                            }
                            putsUART1(" ");
                            PrintChar(maxRSSI);
                            ConsolePutROMString((const char *)"\r\n");
                        }
                        putsUART1("Press any key to return to the main menu\r\n");
                        while(!ConsoleIsGetReady()){}
                        input = ConsoleGet();
                        goto MRF24J40_SubMenu;
                    
                    } 
                    break;                
                    
                case 'f':
                case 'F':
                    if(PrintMenus)
                    {
                        putsUART1("Running in CW mode\r\n");
                        putsUART1("Reset board to return to the main menu\r\n");
                        putsUART1("\r\n");
                    }
                           
                    PHYSetLongRAMAddr(0x000, 22); /* packet header length */
                    PHYSetLongRAMAddr(0x001, 23); /* total packet length (not including the FCS/CRC or length) */
                    PHYSetLongRAMAddr(0x002, 0x61/*0b0110 0001*/ ); /* first byte of packet (the LSB of the frame control p112 of IEEE 802.15.4-2003) -- Data packet, no security, no frame pending, ACK requested, intra-pan transmission */ 
                    PHYSetLongRAMAddr(0x003, 0xCC); /* first byte of packet (the MSB of the frame control p112 of IEEE 802.15.4-2003), use long addresses for both cases */     
                    PHYSetLongRAMAddr(0x004, 0x01); /* IEEE sequence number */
                    PHYSetLongRAMAddr(0x005, 0xFF); /* PANID - broadcast */
                    PHYSetLongRAMAddr(0x006, 0xFF); /* PANID - broadcast */
                    PHYSetLongRAMAddr(0x007, 0x01); /* Dest Address LSB */
                    PHYSetLongRAMAddr(0x008, 0x23); /* Dest Address  */
                    PHYSetLongRAMAddr(0x009, 0x45); /* Dest Address  */
                    PHYSetLongRAMAddr(0x00a, 0x67); /* Dest Address  */
                    PHYSetLongRAMAddr(0x00b, 0x89); /* Dest Address  */
                    PHYSetLongRAMAddr(0x00c, 0xab); /* Dest Address  */
                    PHYSetLongRAMAddr(0x00d, 0xcd); /* Dest Address  */
                    PHYSetLongRAMAddr(0x00e, 0xef); /* Dest Address MSB */
                    PHYSetLongRAMAddr(0x00f, EUI_0); /* Source Address LSB */
                    PHYSetLongRAMAddr(0x010, EUI_1); /* Source Address  */
                    PHYSetLongRAMAddr(0x011, EUI_2); /* Source Address  */
                    PHYSetLongRAMAddr(0x012, EUI_3); /* Source Address  */
                    PHYSetLongRAMAddr(0x013, EUI_4); /* Source Address  */
                    PHYSetLongRAMAddr(0x014, EUI_5); /* Source Address  */
                    PHYSetLongRAMAddr(0x015, EUI_6); /* Source Address  */
                    PHYSetLongRAMAddr(0x016, EUI_7); /* Source Address MSB */
                    PHYSetLongRAMAddr(0x017, 0x55); /* data byte */
                    PHYSetLongRAMAddr(0x018, 0xAA); /* data byte */
             
                    PHYSetShortRAMAddr( WRITE_RFCTL, 0x02/*0b0000 0010*/ );  //force transmit
                    
                    while(1)
                    {
                        if(ConsoleIsGetReady())
                        {
                            if(PrintMenus)
                            {
                                goto MRF24J40_SubMenu;
                            }
                            else
                            {
                                goto MRF24J40_NewOption;
                            }
                        }
                    }
                    break;                
                
                
                case 'g':
                case 'G':
                    {
                        BYTE currentChannel = CHANNEL_11;
                        putsUART1("\r\nEntering Single Tone Test Mode. Reset device to return to the main menu.");
                        
                        while(1)
                        {
                            SetChannel(currentChannel);
                            currentChannel += 0x10;
                            
                            PHYSetLongRAMAddr( RFCTRL3,0x00 );
                            PHYSetShortRAMAddr( WRITE_RFCTL, 0x02/*0b0000 0010*/ );
                            PHYSetLongRAMAddr( TESTMODE, 0x0d ); 
                            MSTimerWait( 1 ); // Delay10KTCYx(255);
                            MSTimerWait( 1 ); // Delay10KTCYx(255);
                        }
                    }
                    break;
                
                case 'h':
                case 'H':
                    {
                        BYTE currentChannel = CHANNEL_11;
                        putsUART1("\r\nEntering Sequential Transmit Mode. Reset device to return to the main menu.");
                        
                        while ( 1 )
                        {
                            WORD i;
                            WORD j;
                            
                            SetChannel(currentChannel);
                            currentChannel += 0x10;
                            
                            PHYSetLongRAMAddr( RFCTRL3,0x00 );
                            PHYSetShortRAMAddr( WRITE_RXMCR, 0x21/*0b0010 0001*/ );    //promiscuous mode (no address filtering) [bit 0], never send ACKs [bit 5]
                            INTEnableInterrupts();
                            PHYSetLongRAMAddr(0x000, 12); /* packet header length */
                            PHYSetLongRAMAddr(0x001, 16); /* total packet length (not including the FCS/CRC or length) */
                            PHYSetLongRAMAddr(0x002, 0x00); /* data byte */
                            PHYSetLongRAMAddr(0x003, 0x01); /* data byte */
                            PHYSetLongRAMAddr(0x004, 0x02); /* data byte */
                            PHYSetLongRAMAddr(0x005, 0x03); /* data byte */
                            PHYSetLongRAMAddr(0x006, 0x04); /* data byte */
                            PHYSetLongRAMAddr(0x007, 0x05); /* data byte */
                            PHYSetLongRAMAddr(0x008, 0x06); /* data byte */
                            PHYSetLongRAMAddr(0x009, 0x07); /* data byte */
                            PHYSetLongRAMAddr(0x00a, 0x08); /* data byte */
                            PHYSetLongRAMAddr(0x00b, 0x09); /* data byte */
                            PHYSetLongRAMAddr(0x00c, 0x0a); /* data byte */
                            PHYSetLongRAMAddr(0x00d, 0x0b); /* data byte */
                            PHYSetLongRAMAddr(0x00e, 0x0c); /* data byte */
                            PHYSetLongRAMAddr(0x00f, 0x0d); /* data byte */
                            PHYSetLongRAMAddr(0x010, 0x0e); /* data byte */
                            PHYSetLongRAMAddr(0x011, 0x0f); /* data byte */
                            
                            for ( i=0; i<400; i++ )
	                        {
		                        PHYSetShortRAMAddr( WRITE_TXNMTRIG, 0x01/*0b00000001*/ );
		                        for ( j=0; j<5000; j++ ) ;
		                    }
                        }
                    }
                    break;

                
                case 'i':
                case 'I':
                    {
                        BYTE i;
                        
                        PHYSetLongRAMAddr(0x000,sizeof(PERCommand));
                        PHYSetLongRAMAddr(0x001,sizeof(PERCommand));
                                    
                        for(i=2;i<(sizeof(PERCommand)+2);i++)
                        {
                            PHYSetLongRAMAddr(i, PERCommand[i-2]); 
                        }
                        
                        if(UseExternal)
                        {
                            PHYSetShortRAMAddr( WRITE_GPIO, 0x03/*0b00000011*/ ); //TX
                            MSTimerWait( 1 ); // Delay100TCYx(4);
                            putsUART1("Using external PA\r\n");
                        }
                        INTEnableInterrupts();       //enable interrupts
                        PHYSetShortRAMAddr( WRITE_TXNMTRIG, 0x01/*0b00000001*/ );
                        
                        numPacketsRx.Val = 0;
                        if(UseExternal)
                         {
                             PHYSetShortRAMAddr( WRITE_GPIO, 0x0c/*0b00001100*/ ); 
                             MSTimerWait( 1 ); // Delay100TCYx(4);
                             putsUART1("Using external LNA\r\n");
                         }
                         
                         numPacketsRx.Val = 0;
                         while ( 1 )
                         {
                             WORD k;
                             if ( RB0 == 0 )
                             {
                                 INTSetFlag( INT_INT1 );
                             }
                             
                             for ( k = 0; k < 50; k++ ) {}
                             #if defined(__18F67J11)
                                sprintf( buf, (const const char*)"%d packets received\r", numPacketsRx.word.LW);
                                for(i = 0; i < 32; i++)
                                {
                                    if( buf[i] == 0x00 )
                                    {
                                        break;
                                    }
                                    ConsolePut(buf[i]);
                                }
                             #else
                                printf( (const char*)"%d packets received\r",numPacketsRx.word.LW);
                             #endif
                             if ( ConsoleIsGetReady() )
                             {
                                 ConsoleGet();
                                 break;
                             }
                         }
                         INTDisableInterrupts();       //enable interrupts
                         //print results
                         #if defined(__18F67J11)
                            sprintf(buf, (far const char*)"%d packets received\r\n", numPacketsRx.word.LW);
                            for(i = 0; i < 32; i++)
                            {
                                if( buf[i] == 0x00 )
                                {
                                    break;
                                }
                                ConsolePut(buf[i]);
                            }
                         #else
                            printf( (const char*)"%d packets received\r\n",numPacketsRx.word.LW);
                         #endif
                    }
                    break;
                
                
                
                case 'z':
                case 'Z':
                    {
                        BYTE i,data;
                        
                        putsUART1("-- Short RAM Addresses\r\nAddress\tValue\r\n");
                        for(i=0x00;i<=0x3F;i++)
                        {
                            data = PHYGetShortRAMAddr(i<<1);
                            putsUART1("Register 0x");
                            PrintChar(i);
                            putsUART1(":\t");
                            PrintChar(data);
                            putsUART1("\r\n");
                            if( (i & 0x0F) == 0x0F )
                            {
                                putsUART1("Press Any Key to Continue>>");
                                while( !DataRdyUART1() );
                            }
                        }
                        
                        putsUART1("\r\n-- Long RAM Addresses\r\nAddress\tValue\r\n");
                        for(i=0x00;i<=0x4C;i++)
                        {
                            data = PHYGetLongRAMAddr(0x200+i);
                            putsUART1("Register 0x2");
                            PrintChar(i);
                            putsUART1(":\t");
                            PrintChar(data);
                            putsUART1("\r\n");
                            if( (i & 0x0F) == 0x0F )
                            {
                                putsUART1("Press Any Key to Continue>>");
                                while( !DataRdyUART1() );
                            }
                           
                        }
                        putsUART1("\r\n");                        
                    }
                    goto MRF24J40_SubMenu;
                    break;
                
                default:
                    break;
            }
            break;
        
        default:
            break;
    }
    goto MRF24J40_SubMenu;
}

