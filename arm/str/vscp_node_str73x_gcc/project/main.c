/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  IAR STR 73x demo board Module
 * 	Version information in version.h
 * 	akhe@eurosource.se   
 *
 * Copyright (C) 1995-2011 Ake Hedman, 
 * eurosource, <akhe@eurosource.se>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 
 *	This file is part of VSCP - Very Simple Control Protocol 	
 *	http://www.vscp.org
 *
 * ******************************************************************************/

#include "73x_lib.h"
#include "../../common/73x_lcd.h"
#include "../../common/systime.h"
#include "../../common/delay.h"
#include <inttypes.h>
#include <vscp.h>
#include <vscp_class.h>
#include <vscp_type.h>
#include "vscp_compiler.h" 	// This file should be in your project folder
#include "vscp_projdefs.h"	// This file should be in your project folder


// Prototypes
void wait( void );
void MCLK_Config (void);
void CAN0_Demo_Polling( void );
void CAN0_Demo_Interrupt(void);

void init( void );
void init_app_eeprom( void );
void init_app_ram( void );

void doWork( void );

void read_app_register( unsigned char reg );
void write_app_register( unsigned char reg, unsigned char val );

unsigned int get1000Digit( unsigned int val );
unsigned int get100Digit( unsigned int val );
unsigned int get10Digit( unsigned int val );
unsigned int get1Digit( unsigned int val );

// Peripherals Init Structures
GPIO_InitTypeDef GPIO0_InitStructure;
GPIO_InitTypeDef GPIO3_InitStructure;
ADC_InitTypeDef  ADC_InitStructure;
CMU_InitTypeDef  CMU_InitStructure;
RTC_InitTypeDef  RTC_InitStructure;

GPIO_InitTypeDef CAN0_RX;
GPIO_InitTypeDef CAN0_TX;
GPIO_InitTypeDef CAN1_RX;
GPIO_InitTypeDef CAN1_TX;
GPIO_InitTypeDef CAN2_RX;
GPIO_InitTypeDef CAN2_TX;
GPIO_InitTypeDef Led_Config;
GPIO_InitTypeDef Button_Config;

// VSCP Module firmware version
#define FIRMWARE_MAJOR_VERSION				0;
#define FIRMWARE_MINOR_VERSION				0;
#define FIRMWARE_SUB_MINOR_VERSION		1;


enum {
	CAN0_TX_MSGOBJ = 0,
	CAN0_RX_MSGOBJ0,
	CAN0_RX_MSGOBJ1,
	CAN0_RX_MSGOBJ2,
	CAN0_RX_MSGOBJ3,
	CAN0_RX_MSGOBJ4,
	CAN0_RX_MSGOBJ5,
	CAN0_RX_MSGOBJ6,
	CAN0_RX_MSGOBJ7,
	CAN0_RX_MSGOBJ8,
	CAN0_RX_MSGOBJ9,
	CAN0_RX_MSGOBJ10,
	CAN0_RX_MSGOBJ11,
	CAN0_RX_MSGOBJ12,
	CAN0_RX_MSGOBJ13,
	CAN0_RX_MSGOBJ14,
	CAN0_RX_MSGOBJ15,
	CAN0_RX_MSGOBJ16,
	CAN0_RX_MSGOBJ17,
	CAN0_RX_MSGOBJ18,
	CAN0_RX_MSGOBJ19,
	CAN0_RX_MSGOBJ20,
	CAN0_RX_MSGOBJ21,
	CAN0_RX_MSGOBJ22,
	CAN0_RX_MSGOBJ23,
	CAN0_RX_MSGOBJ24,
	CAN0_RX_MSGOBJ25,
	CAN0_RX_MSGOBJ26,
	CAN0_RX_MSGOBJ27,
	CAN0_RX_MSGOBJ28,
	CAN0_RX_MSGOBJ29
};

u16 potentiometer_ad_Value = 0;
u16 termistor_ad_Value = 0;

// The device URL (max 32 characters including null termination)
const char vscp_deviceURL[] = "www.eurosource.se/demo73x_001.xml";

// The device GUID
const char vscp_deviceGUID[] = { 0x01, 0x00, 0x00, 0x00,
																	0x00, 0x00, 0x00, 0x00,
																	0x00, 0x00, 0x00, 0x00,
																	0xff, 0xff, 0xff, 0xff
																};

// The device USER id	 - normally in EEPROM															
const char vscp_userID[] = { 0x10, 0x20, 0x30, 0x40, 0x50 };	

// The device manufacturer id	 - normally in EEPROM														
const char vscp_ManufacturerDeviceID[] = { 0x11, 0x20, 0x30, 0x40,
																						0x64, 0x65, 0x40, 0x00 };	
											
// Simulated eeprom storage
uint8_t eeprom_nickname;
uint8_t eeprom_segmentCRC;

volatile unsigned long measurement_clock;	// Clock for measurments
																						
uint8_t seconds;							// counter for seconds
uint8_t minutes;							// counter for minutes
uint8_t hours;								// Counter for hours


int main(void)
{
	
#ifdef LIBDEBUG
	libdebug();
#endif

	

	init();   // Initialize Microcontroller	
	
	
	
	// Check VSCP persistent storage and
	// restore if needed
	if ( !vscp_check_pstorage() ) {
		
		// Spoiled or not initialized - reinitialize
		init_app_eeprom();	

	}

	vscp_init();			// Initialize the VSCP functionality


	
	///////////////
	// Main LOOP //
	///////////////



	while ( 1 )	 {
	
	
		if ( ( vscp_initbtncnt > 100 ) && ( VSCP_STATE_INIT != vscp_node_state ) ) {
			// Init button pressed
			vscp_nickname = VSCP_ADDRESS_FREE;
			eeprom_nickname = VSCP_ADDRESS_FREE;
			vscp_init();
		}

		// Check for any valid CAN message
		vscp_imsg.flag = 0;
		vscp_getEvent();
	
		// do a meaurement if needed
		if ( measurement_clock > 1000 ) {
			
			measurement_clock = 0;
			//doMeasurement();
			seconds++;
			
			if ( seconds > 59 ) {
			
				seconds = 0;
				minutes++;
				
				if ( minutes > 59 ) {
					minutes = 0;
					hours++;
				}
				
				if ( hours > 23 ) hours = 0;
				
			}				
		}
	
		switch ( vscp_node_state ) {

			case VSCP_STATE_STARTUP:			// Cold/warm reset

				LCD_SendString( "STARTUP         ", 2, 1 );
				
				// Get nickname from EEPROM
				if ( VSCP_ADDRESS_FREE == vscp_nickname ) {
					// new on segment need a nickname
					vscp_node_state = VSCP_STATE_INIT; 	
				}
				else {
					// been here before - go on
					vscp_node_state = VSCP_STATE_ACTIVE;
					vscp_active();
				}
				break;

			case VSCP_STATE_INIT:					// Assigning nickname
				LCD_SendString( "INIT            ", 2, 1 );
				vscp_probe();
				break;

			case VSCP_STATE_PREACTIVE:		// Waiting for host initialisation
				LCD_SendString( "PRECATIVATE     ", 2, 1 );
				vscp_rcv_preactive();					
				break;

			case VSCP_STATE_ACTIVE:				// The normal state
			
				LCD_SendString( "ACTIVE          ", 2, 1 );
				
				if ( vscp_imsg.flag & VSCP_VALID_MSG ) {	// incoming event?
					
					// Yes, incoming message
					if ( VSCP_CLASS1_PROTOCOL == vscp_imsg.class ) {
						switch( vscp_imsg.type ) {

							case VSCP_TYPE_PROTOCOL_SEGCTRL_HEARTBEAT:
								vscp_rcv_heartbeat();
								break;

							case VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE:
								vscp_rcv_new_node_online();
								break;
							
							case VSCP_TYPE_PROTOCOL_SET_NICKNAME:
								vscp_rcv_set_nickname();
								break;
							
							case VSCP_TYPE_PROTOCOL_DROP_NICKNAME:
								vscp_rcv_drop_nickname();
								break;

							case VSCP_TYPE_PROTOCOL_READ_REGISTER:
								if ( ( 2 == ( vscp_imsg.flag & 0x0f ) ) && 
										( vscp_nickname == vscp_imsg.data[ 0 ] ) ) {

									if ( vscp_imsg.data[ 1 ] < 0x80 )  {
										// Read application specific register
										read_app_register( vscp_imsg.data[ 1 ] );
									}
									else {
										// Read VSCP register
										vscp_rcv_readreg();
									}
								}
								break;

							case VSCP_TYPE_PROTOCOL_WRITE_REGISTER:
								if ( ( 3 == ( vscp_imsg.flag & 0x0f ) ) && 
										( vscp_nickname == vscp_imsg.data[ 0 ] ) ) {
									if ( vscp_imsg.data[ 1 ] < 0x80 ) {
										// Write application specific register
										write_app_register( vscp_imsg.data[ 1 ], vscp_imsg.data[ 2 ] );
									}
									else {
										// Read VSCP register
										vscp_rcv_writereg();
									}
								}
								break;
								
							case VSCP_TYPE_PROTOCOL_ENTER_BOOT_LOADER:
								// No bootloader implemented
								vscp_omsg.flags = VSCP_VALID_MSG + 1 ;	// one databyte 
								vscp_omsg.priority = VSCP_PRIORITY_HIGH;	
								vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
								vscp_omsg.type = VSCP_TYPE_PROTOCOL_NACK_BOOT_LOADER;
								vscp_omsg.data[ 0 ] = 0x00;			
				
								// send the probe
								vscp_sendEvent();
								break;

							default:
								// Do work load
								break;
						}							
					} // class1 protocol
				} // Incoming event
				break;

			case VSCP_STATE_ERROR:			// Everything is *very* *very* bad.
				LCD_SendString( "ERROR           ", 2, 1 );
				vscp_error();
				break;

			default:					// Should not be here...
				vscp_node_state = VSCP_STATE_STARTUP;
				break;

		} // switch 

		doWork();	

	} // while
	
}


///////////////////////////////////////////////////////////////////////////////
// init
//
// Initialize the controller and pheriperals
//

void init( void ) 
{
	uint8_t i;
	
	init_app_ram();
	
	MCLK_Config();	
	
	// Enable clock on both CAN channels
	CFG_PeripheralClockConfig( CFG_CLK_CAN0, ENABLE );
  CFG_PeripheralClockConfig( CFG_CLK_CAN1, ENABLE );

  CFG_PeripheralClockConfig( CFG_CLK_GPIO0, ENABLE );
  CFG_PeripheralClockConfig( CFG_CLK_GPIO1, ENABLE );
  CFG_PeripheralClockConfig( CFG_CLK_GPIO2, ENABLE );
  CFG_PeripheralClockConfig( CFG_CLK_GPIO6, ENABLE );

  Led_Config.GPIO_Mode = GPIO_Mode_OUT_PP;
  Led_Config.GPIO_Pins = 0xFFFF;
  GPIO_Init( GPIO0, &Led_Config );
	
	// Configure ADC Pins
	GPIO3_InitStructure.GPIO_Mode = GPIO_Mode_HI_AIN_TRI;
  GPIO3_InitStructure.GPIO_Pins = GPIO_PIN_ALL;
  GPIO_Init( GPIO3, &GPIO3_InitStructure );

	// CAN 0 RC pin
  CAN0_RX.GPIO_Mode = GPIO_Mode_IN_TRI_TTL;
  CAN0_RX.GPIO_Pins = GPIO_PIN_14;
  GPIO_Init( GPIO1, &CAN0_RX );

	// CAN 0 TX pin
  CAN0_TX.GPIO_Mode = GPIO_Mode_AF_PP;
  CAN0_TX.GPIO_Pins = GPIO_PIN_15;
  GPIO_Init( GPIO1, &CAN0_TX );

	// CAN 1 RX pin
  CAN1_RX.GPIO_Mode = GPIO_Mode_IN_TRI_TTL;
  CAN1_RX.GPIO_Pins = GPIO_PIN_1;
  GPIO_Init( GPIO2, &CAN1_RX );

	// CAN 1 TX pin
  CAN1_TX.GPIO_Mode = GPIO_Mode_AF_PP;
  CAN1_TX.GPIO_Pins = GPIO_PIN_2;
  GPIO_Init( GPIO2, &CAN1_TX );
	
	// Init button
	Button_Config.GPIO_Mode = GPIO_Mode_INOUT_WP;
  Button_Config.GPIO_Pins = GPIO_PIN_10;
  GPIO_Init( GPIO2, &Button_Config );

  // Init 1 ms timebase
  initTimebase2();

  GPIO_WordWrite( GPIO0, 0x0000 );  // All LED's off

  // * * * EIC * * *

  // EIC Clock Enable
  CFG_PeripheralClockConfig( CFG_CLK_EIC , ENABLE );

  // EIC Config
  EIC_IRQChannelPriorityConfig( RTC_IRQChannel, 1 );
  EIC_IRQChannelPriorityConfig( TB2_IRQChannel, 2 );

  //EIC_IRQChannelConfig( RTC_IRQChannel , ENABLE );
  

  EIC_IRQCmd( ENABLE );
	
	// * * * CMU * * *
	
  // CMU Initialization 
  CMU_DeInit( );
  CMU_StructInit(&CMU_InitStructure);
	
  // Enable the external oscillator 
  CMU_InitStructure.CMU_CKSEL0 = CMU_CKSEL0_CKOSC;
  CMU_Init(&CMU_InitStructure);
	
  // Disable DIV2 
  PRCCU->CFR &= 0x7FFF;


  // * * * RTC * * *

  // Set RTC clock-------
  // (clock:0-->9) fOSC=4Mhz/RTCP=(2,4,8,16,32,64,128,256,512,1024)
  //            -->EXTCLK=(4M,2M,1M,500K,250K,125K,62.5K,31.25K,15.625K,7.812K)


  // RTC Clock Enable
  CFG_PeripheralClockConfig( CFG_CLK_RTC , ENABLE );

  // RTC Interrupt Config
  RTC_ITConfig( RTC_IT_SEC | RTC_IT_ALA | RTC_IT_OV, ENABLE );

  // RTC Configuration
  RTC_InitStructure.RTC_Alarm = 6; 		// 	0xFFFFFFFF;
  RTC_InitStructure.RTC_Counter = 0; 	//	0xFFFFFFFA;
  // configure RTC=1S  ( RCK=1000000/64=CKL32=15625Hz )
  // 8MHz/64 = 125000 = 0x1E848
  RTC_InitStructure.RTC_Prescaler = 0x3D09; // == 15625
  //RTC_InitStructure.RTC_Prescaler = 0x1E848; // 0x3D09; // == 15625

  RTC_Init( &RTC_InitStructure );


  // * * * CAN * * *
  CAN_InitTypeDef CANInit = { 0x0, CAN_BITRATE_500K };

  // initialize the CAN at a standard bitrate, interrupts disabled
  CAN_Init( CAN0, &CANInit );

  // configure the message objects
	// Obj 0 used for TX
	// obj 1-31 used for RX
  CAN_EnterInitMode( CAN0, CAN_CR_CCE );
  CAN_InvalidateAllMsgObj( CAN0 );
  CAN_SetTxMsgObj( CAN0, CAN0_TX_MSGOBJ, CAN_EXT_ID );
	for ( i=1; i<32; i++ ) {
		CAN_SetRxMsgObj( CAN0, i, CAN_EXT_ID, 0, CAN_LAST_EXT_ID, TRUE );
	}
	CAN_LeaveInitMode( CAN0 );
	

	
  // * * * LCD * * *
	
  // LCD Configuration
  LCD_Init();
	LCD_Clear();

  LCD_SetPosCur(1,1);

  // Display text message on LCD: Line 1
  LCD_SendString( "VSCP TEST NODE", 1, 1 );
  
	LCD_SetPosCur(2,1);
	

	
	// * * * ADC * * *
	
	
  // ADC Clock Enable 
  CFG_PeripheralClockConfig(CFG_CLK_ADC, ENABLE);
	
  // ADC Configuration 
  ADC_DeInit ();
  ADC_StructInit( &ADC_InitStructure );
  ADC_InitStructure.ADC_Calibration = ADC_Calibration_ON;
  ADC_InitStructure.ADC_CalibAverage = ADC_CalibAverage_Enable;
  ADC_InitStructure.ADC_AutoClockOff = ADC_AutoClockOff_Disable ;
  ADC_InitStructure.ADC_ConversionMode = ADC_ConversionMode_Scan ;
  ADC_InitStructure.ADC_SamplingPrescaler = 0x2;
  ADC_InitStructure.ADC_ConversionPrescaler = 0x4;
  ADC_InitStructure.ADC_FirstChannel = ADC_CHANNEL0 ;
  ADC_InitStructure.ADC_ChannelNumber = 2;
  ADC_Init( &ADC_InitStructure );


  // EOC interrupt Enable 
  ADC_ITConfig( ADC_IT_EOC, ENABLE );
	
  // Enable ADC 
  ADC_Cmd( ENABLE );
	
}


///////////////////////////////////////////////////////////////////////////////
// init_app_ram
//

void init_app_ram( void )
{
	eeprom_nickname = VSCP_ADDRESS_FREE;
	eeprom_segmentCRC = 0;
}


////////////////////////////////////////////////////////////////////////////////
// init_app_eeprom
//

void init_app_eeprom( void )
{
	// Nothing to do as we are out of EEPROM
}





////////////////////////////////////////////////////////////////////////////////
// get1000Digit
//

unsigned int get1000Digit( unsigned int val )
{
  return ( ( val / 1000 ) + 0x30 );
}

////////////////////////////////////////////////////////////////////////////////
// get100Digit
//

unsigned int get100Digit( unsigned int val )
{
  unsigned int valCopy;
  valCopy = val % 1000;
  return (  (valCopy / 100 ) + 0x30 );
}

////////////////////////////////////////////////////////////////////////////////
// get10Digit
//

unsigned int get10Digit( unsigned int val )
{
  unsigned int valCopy;
  valCopy = val % 100;
  return ( ( valCopy / 10 ) + 0x30 );
}

////////////////////////////////////////////////////////////////////////////////
// get1Digit
//

unsigned int get1Digit( unsigned int val )
{
  unsigned int valCopy;
  valCopy = val % 100;
  return ( ( valCopy % 10 ) + 0x30 );
}

////////////////////////////////////////////////////////////////////////////////
//  wait
//

void wait( void )
{
  u32 i;

  for (i=100000L; i!=0 ; i-- ) {}
}

////////////////////////////////////////////////////////////////////////////////
//  MCLK_Config
//

void MCLK_Config (void)
{

  PRCCU_InitTypeDef Clock;
  CMU_InitTypeDef ClockInit;

  ClockInit.CMU_RCOscControl = 0x0D;  			// RC OSC Adjust
  ClockInit.CMU_EndCountValue = 0x0F; 			//
  ClockInit.CMU_FreqRef_High = 0xFF;  			//
  ClockInit.CMU_FreqRef_Low = 0xFE;   			//	

  ClockInit.CMU_CKSEL0 = CMU_CKSEL0_CKOSC;  // Clock for CLOCK1 (4MHz)
  ClockInit.CMU_CKSEL1 = CMU_CKSEL1_CKPLL;  // Clock for Freq. Meter (8MHz)
  ClockInit.CMU_CKSEL2 = CMU_CKSEL2_CKOSC;  // Clock to drive CMU logic

  CMU_Lock( DISABLE );
  CMU_Init( &ClockInit );

   // Init. Power Reset Clock unit
  //    4 MHz * 12 / 6  = 8 MHz = CLOCK2
  Clock.PRCCU_DIV2 = DISABLE;
  Clock.PRCCU_MCLKSRC_SRC = PRCCU_MCLKSRC_PLL;  // Use PLL in
  Clock.PRCCU_PLLDIV = PRCCU_PLLDIV_6;
  Clock.PRCCU_PLLMUL = PRCCU_PLLMUL_12;

  PRCCU_Init( &Clock );
  PRCCU_SetExtClkDiv(4);

}


///////////////////////////////////////////////////////////////////////////////
//                             APPLICATION
//////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// doWork
//

void doWork( void ) 
{
	char buffer[33];
	u32 CPIO_Value;
	
	// Start ADC conversion 
	ADC_ConversionCmd( ADC_ConversionStart );
	buffer[8] = get1Digit( potentiometer_ad_Value );
	buffer[7] = get10Digit( potentiometer_ad_Value );
	buffer[6] = get100Digit( potentiometer_ad_Value );
	buffer[5] = get1000Digit( potentiometer_ad_Value );
	buffer[9] = 0;
		
	// Display ADC conversion Value on LCD 
	LCD_SendString( &buffer[5], 2, 10 );
	CPIO_Value = 0;

	for ( u32 idx = (u32)((float)potentiometer_ad_Value/60); idx; idx--) {
		CPIO_Value <<= 1;
		CPIO_Value |= 1;
	}
		
	GPIO_WordWrite( GPIO0, CPIO_Value );

	// Wait for 500 ms
	//delay_ms( 500 );
}


///////////////////////////////////////////////////////////////////////////////
// read_app_register
//

void read_app_register( unsigned char reg )
{
	// TODO
	
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG + 2;
	vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

	vscp_omsg.data[ 0 ] = reg;
	vscp_omsg.data[ 1 ] = 0x00; // default read
	
	
	// Send data
	vscp_sendEvent();
}


///////////////////////////////////////////////////////////////////////////////
// write_app_register
//

void write_app_register( unsigned char reg, unsigned char val )
{
	// TODO
	
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG + 2;
	vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

	vscp_omsg.data[ 0 ] = reg;
	vscp_omsg.data[ 1 ] = ~val; // error return
	
	// Send data
	vscp_sendEvent();
}




///////////////////////////////////////////////////////////////////////////////
//                        VSCP Required Methods
//////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//  sendCANFrame
//

int8_t sendCANFrame( uint32_t id, uint8_t dlc, uint8_t *pdata )
{
	canmsg msg;
	
	msg.IdType = CAN_EXT_ID;
	msg.Id = id;
	msg.Dlc = dlc;
	memcpy( msg.Data, pdata, dlc );
	
	
	// Send the frame frame
	if ( !CAN_SendMessage( CAN0, CAN0_TX_MSGOBJ, &msg ) ) {
		// Failed to send message
		return FALSE;
	}

  // wait until end of transmission
	CAN_WaitEndOfTx( CAN0 );

	// release the TX message object
	CAN_ReleaseTxMessage( CAN0, CAN0_TX_MSGOBJ );

	vscp_omsg.flags = 0;	// Out msg sent
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
//  getCANFrame
//

int8_t getCANFrame( uint32_t *pid, uint8_t *psize, uint8_t *pData )
{
	uint8_t i;
	canmsg msg;

	// Dont read in new message if there already is a message
	// in the input buffer
	if ( vscp_imsg.flag & VSCP_VALID_MSG ) return FALSE;

	for ( i=1; i<32; i++ ) {
		
		if ( CAN_ReceiveMessage( CAN0, i, TRUE, &msg ) ) {
		
			// We already no this is an extended id so we dont't bother
			// with that info.
			*pid = msg.Id;	
			*psize = msg.Dlc;
			memcpy( pData, msg.Data, msg.Dlc );
			
			return TRUE;
			
		}
		
	}		
	
	return FALSE;
}



///////////////////////////////////////////////////////////////////////////////
// Get Major version number for this hardware module
//

unsigned char vscp_getMajorVersion()
{
	return FIRMWARE_MAJOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get Minor version number for this hardware module
//

unsigned char vscp_getMinorVersion()
{
	return FIRMWARE_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get Subminor version number for this hardware module
//

unsigned char vscp_getSubMinorVersion()
{
	return FIRMWARE_SUB_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// getVSCP_GUID
//
// Get GUID from EEPROM
//

uint8_t vscp_getGUID( uint8_t idx )
{
	return vscp_deviceGUID[ idx ];		
}


///////////////////////////////////////////////////////////////////////////////
// getDeviceURL
//
// Get device URL from EEPROM
//

uint8_t vscp_getMDF_URL( uint8_t idx )
{
	return vscp_deviceURL[ idx ];
}


//////////////////////////////////////////////////////////////////////////////
// Get Manufacturer id and subid from EEPROM
//

uint8_t vscp_getUserID( uint8_t idx )
{
	return vscp_userID[ idx ];	
}


///////////////////////////////////////////////////////////////////////////////
//  setVSCPUserID
//

void vscp_setUserID( uint8_t idx, uint8_t data )
{
	// Normally this should be stired in EEPROM but
	// no EEPROM is available on this board so we just
	// ignore this request.
}

///////////////////////////////////////////////////////////////////////////////
// getVSCPManufacturerId
// 
// Get Manufacturer id and subid from EEPROM
//

uint8_t vscp_getManufacturerId( uint8_t idx )
{
	return vscp_ManufacturerDeviceID[ idx ];	
}

///////////////////////////////////////////////////////////////////////////////
// setVSCPManufacturerId
// 
// Set Manufacturer id and subid from EEPROM
//

void vscp_setManufacturerId( uint8_t idx, uint8_t data )
{
	// Normally this should be stired in EEPROM but
	// no EEPROM is available on this board so we just
	// ignore this request.	
}


///////////////////////////////////////////////////////////////////////////////
// Get the bootloader algorithm code
//

uint8_t vscp_getBootLoaderAlgorithm( void ) 
{
	return VSCP_BOOTLOADER_NONE;	
}


//////////////////////////////////////////////////////////////////////////////
// Get the buffer size
//

uint8_t vscp_getBufferSize( void ) 
{
	return 8;	// Standard CAN frame
}


///////////////////////////////////////////////////////////////////////////////
//  getNickname
//

uint8_t vscp_getNickname( void )
{
	// Normally this should be stired in EEPROM but
	// no EEPROM is available on this board so we just
	// ignore this request and retrn the dynamically 
	// assigned id.	
	return eeprom_nickname;
}


///////////////////////////////////////////////////////////////////////////////
//  setNickname
//

void vscp_setNickname( uint8_t nickname )
{
	// Normally this should be stired in EEPROM but
	// no EEPROM is available on this board so we just
	// ignore this request and set the dynamically 
	// assigned id.	
	eeprom_nickname = nickname;
}

///////////////////////////////////////////////////////////////////////////////
//  getSegmentCRC
//

uint8_t vscp_getSegmentCRC( void )
{
	// Normally this should be stired in EEPROM but
	// no EEPROM is available on this board so we just
	// ignore this request
	return eeprom_segmentCRC;
}

///////////////////////////////////////////////////////////////////////////////
//  setSegmentCRC
//

void vscp_setSegmentCRC( uint8_t crc )
{
	// Normally this should be stired in EEPROM but
	// no EEPROM is available on this board so we just
	// ignore this request
	eeprom_segmentCRC = crc;
}


///////////////////////////////////////////////////////////////////////////////
//  setVSCPControlByte
//

void vscp_setControlByte( uint8_t ctrl )
{
	// Normally this should be stired in EEPROM but
	// no EEPROM is available on this board so we just
	// ignore this request
}


///////////////////////////////////////////////////////////////////////////////
//  getVSCPControlByte
//

uint8_t vscp_getControlByte( void )
{
	// Normally this should be stired in EEPROM but
	// no EEPROM is available on this board so we just
	// ignore this request
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
//  getVSCPControlByte
//

uint8_t vscp_getPageSelect( uint8_t idx )
{
	// No paging used so we ignore this
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//  getVSCPControlByte
//

void vscp_setPageSelect( uint8_t idx, uint8_t data )
{
	// No paging used so we ignore this
}























