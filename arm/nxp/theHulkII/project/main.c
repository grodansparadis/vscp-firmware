///////////////////////////////////////////////////////////////////////////////
// File: main.c
//
// Main file for the HULK II project
// Keil MCB2130 board 
//
// This is the HULK definitions
// Copyright (C) 2005 Ake Hedman, akhe@eurosource.se
// http://www.vscp.org
//
// $RCSfile: main.c,v $
// $Revision: 1.28 $
//
// Some code also from Bill Knight, R O SoftWare <BillK@rosw.com>
// Martin THOMAS <eversmith@heizung-thomas.de>
//

#include "types.h"
#include "lpc213x.h"
//#include <stdint.h> C99 types int32_t, uint32_t, etc.
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "armVIC.h"
#include "sysTime.h"
#include "uart.h"
#include "lcd.h"
#include "itoa.h"
#include "ds18x20.h"
#include "onewire.h"
#include "lawicel232can.h"
#include "eeprom.h"
#include "display.h"
#include <vscp_class.h>
#include <vscp_type.h>
#include "vscp.h"
#include "hulkevents.h"
#include "debug.h"
#include "hulk.h"

// Prototypes
static void lowInit( void );
static void sysInit( void );
static int button_state( uint8_t nButton );
static unsigned int getFireDetectorLevel( void );
static unsigned int getFireReferenceLevel( void );
static int getHeaterTemp( void );
static int getFurnaceTemp( void );
static void ctrFan( BOOL state );
static void ctrTransportMotorA( BOOL state );
static void ctrTransportMotorB( BOOL state );
static void ctrFurnaceHeater( BOOL state );
static void ctrAlarm( BOOL state );
static void ctrElementA( BOOL state );
static void ctrElementB( BOOL state );
static BOOL isOverTemp( void );
static uint8_t getConfigValue( uint8_t state );

static void init_app_eeprom( void );

static BOOL BtnForward( void );
static BOOL BtnUp( void );
static BOOL BtnDown( void );
static BOOL BtnEnter( void );
static BOOL BtnForceMotorOn( void );

void read_app_register( unsigned char reg );
void write_app_register( unsigned char reg, unsigned char val );

void wdtISR( void );
								
// Voltage at V_ddA/V3A (LPC2139 Pin7)
// With jumber enabled connected to 3,3V on MCB2130 
#define VREF  33/10

BOOL bDoTempConversion = TRUE;	// Flag for temperature conversions

// The device URL (max 32 characters including null termination)
const char vscp_deviceURL[] = "www.eurosource.se/hulk_001.xml";

// Global variables
uint8_t message[ 4 ] = {0x11,0x02,0x03,0x04};
uint8_t	messageIn[ 4 ];

BOOL button_lock[ 6 ] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };

char gdisplaybuf1[ 17 ];
char gdisplaybuf2[ 17 ];

// Stat to measure Furnace out temperature
uint8_t state_temp_measurement = TEMPSENSOR_FURNACE_OUT;

uint8_t tempHeater = 0x00;			// Electrical heater core temperature
uint8_t tempFurnace = 0x00;			// Furnace core temperature
uint8_t cntRestarts = 0;			// # of restarts done 

///////////////////////////////////////////////////////////////////////////////
// wdtISR
//

void wdtISR( void )
{
	ISR_ENTRY();
	
	if ( !( IOPIN1 & PIO1_FAN_BIT ) ) {
		IOSET1 = PIO1_FAN_BIT;
	}
	else {
		IOCLR1 = PIO1_FAN_BIT;
	}
	
	IOSET1 = PIO1_EHEATER_ELEMENTB_BIT;
	
	//WDMOD &= ~BIT(WDTOF);
	VICVectAddr = 0x00000000;	// Clear interrupt in 

	ISR_EXIT();					// recover registers and return
}




/******************************************************************************
 *
 * Function Name: lowInit()
 *
 * Description:
 *    This function starts up the PLL then sets up the GPIO pins before
 *    waiting for the PLL to lock.  It finally engages the PLL and
 *    returns
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
static void lowInit( void )
{
	// set PLL multiplier & divisor.
	// values computed from config.h
	PLLCFG = PLLCFG_MSEL | PLLCFG_PSEL;

	// enable PLL
	PLLCON = PLLCON_PLLE;
	PLLFEED = 0xAA;      		// Make it happen.  These two updates
	PLLFEED = 0x55;             // MUST occur in sequence.
	
	// Setup A/D: 10-bit AIN0 @ 4,2MHz "Non-Burst"-Mode 
	//PINSEL1 |= ( 1UL << 22 ); // set function P0.27 as AIN0
	//PINSEL1 |= ( 1UL << 24 ); // set function P0.28 as AIN1
	PINSEL1 = ( PINSEL1 & ~0x03C00000 ) | 0x01600000;

	// setup the PIO0 port pin
	IOCLR0 = PIO0_ZERO_BITS;                // clear the ZEROs output
	IOSET0 = PIO0_ONE_BITS;                 // set the ONEs output
	IODIR0 = PIO0_OUTPUT_BITS;              // set the output bit direction
	
	// setup the PIO1 port pin
	IOCLR1 = PIO1_ZERO_BITS;                // clear the ZEROs output
	IOSET1 = PIO1_ONE_BITS;                 // set the ONEs output
	IODIR1 = PIO1_OUTPUT_BITS;              // set the output bit direction

	// wait for PLL lock
	while ( !(PLLSTAT & PLLSTAT_LOCK ) ) {
		continue;
	}

	// enable & connect PLL
	PLLCON = PLLCON_PLLE | PLLCON_PLLC;
	PLLFEED = 0xAA;                       // Make it happen.  These two updates
	PLLFEED = 0x55;                       // MUST occur in sequence.
	
	// setup & enable the MAM
	MAMTIM = MAMTIM_CYCLES;
	MAMCR = MAMCR_FULL;

	// set the peripheral bus speed
	// value computed from config.h
	VPBDIV = VPBDIV_VALUE;                // set the peripheral bus clock speed
	
}

/******************************************************************************
 *
 * Function Name: sysInit()
 *
 * Description:
 *    This function is responsible for initializing the program
 *    specific hardware
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:   
 *    void
 *
 *****************************************************************************/
static void sysInit( void )
{
	lowInit();                            	// setup clocks and processor port pins

	// set the interrupt controller defaults
#if defined(RAM_RUN)
	MEMMAP = MEMMAP_SRAM;                 	// map interrupt vectors space into SRAM
#elif defined(ROM_RUN)
	MEMMAP = MEMMAP_FLASH;                	// map interrupt vectors space into FLASH
#else
#error RUN_MODE not defined!
#endif
	VICIntEnClr = 0xFFFFFFFF;  				// clear all interrupts
	VICIntSelect = 0x00000000;           	// clear all FIQ selections
	VICDefVectAddr = (uint32_t)reset;   	// point unvectored IRQs to reset()
	
	initSysTime();                       	// initialize the system timer
	
	uart0Init( UART_BAUD( HOST_BAUD0 ), UART_8N1, UART_FIFO_8 ); // setup the UART
	uart1Init( UART_BAUD( HOST_BAUD1 ), UART_8N1, UART_FIFO_8 ); // setup the UART
	
	// Initiate I2C
	i2cInit();	

	// initialize the watchdog timer	
/*	VICIntSelect &= ~VIC_BIT (VIC_WDT );	// WDT selected as IRQ
	VICIntEnable = VIC_BIT( VIC_WDT );   	// WDT interrupt enabled
	VICVectCntl8 = VIC_ENABLE | VIC_WDT;
	VICVectAddr8 = (uint32_t)wdtISR;    	// address of the ISR
*/  
	WDTC = 15000000;						// One second = 15000000
	WDTV = 0;
	WDMOD = WDEN | WDRESET;  				// Activate watchdog
	WDFEED = 0xAA; WDFEED = 0x55;
	
}


/******************************************************************************
 *
 * Function Name: button_state()
 *
 * Description:
 *    This function checks if a key has been pressed. Assumes
 *    key to be "active low" (PIN-Bit==0 -> pressed). Does
 *    debouncing for given debounce time-difference
 *
 * Calling Sequence: 
 *    GPIO-Initialisation for Inputs
 *
 * Returns:
 *    -1 : key changed or bouncing
 *     0 : key released
 *     1 : key pressed
 *
 *****************************************************************************/

#define KEY_DEBOUNCE FIFTY_MS

static int button_state( uint8_t nButton )
{
	static uint32_t lastchangetime[ 6 ];
	static BOOL laststate[ 6 ] = { FALSE,
										FALSE,
										FALSE,
										FALSE,
										FALSE,
										FALSE };
	static BOOL actstate[ 6 ] = { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };
	
	switch ( nButton ) {
		
		case BUTTON_STATE0:
			// TRUE if pressed (active low)
			actstate[ nButton ] = ( IOPIN0 & PIO0_BTN_STATE0_BIT ) ? FALSE : TRUE; 
			break;

		case BUTTON_FORWARD:
			// TRUE if pressed (active low)
			actstate[ nButton ] = ( IOPIN0 & PIO0_BTN_PG_FORWARD_BIT ) ? FALSE : TRUE; 
			break;
			
		case BUTTON_UP:
			// TRUE if pressed (active low)
			actstate[ nButton ] = ( IOPIN0 & PIO0_BTN_UP_BIT ) ? FALSE : TRUE; 
			break;
			
		case BUTTON_DOWN:
			// TRUE if pressed (active low)
			actstate[ nButton ] = ( IOPIN0 & PIO0_BTN_DOWN_BIT ) ? FALSE : TRUE; 
			break;

		case BUTTON_ENTER:
			// TRUE if pressed (active low)
			actstate[ nButton ] = ( IOPIN0 & PIO0_BTN_ENTER_BIT ) ? FALSE : TRUE; 
			break;
			
		case BUTTON_FORCE_MOTOR_ON:
			// TRUE if pressed (active low)
			actstate[ nButton ] = ( IOPIN0 & PIO0_BTN_FORCE_MON_BIT ) ? FALSE : TRUE; 
			break;	
	}
	
	if ( laststate[ nButton ] != actstate[ nButton ] ) {
		lastchangetime[ nButton ] = getSysTICs();
		laststate[ nButton ] = actstate[ nButton ];
	}
	else {
		if ( getElapsedSysTICs( lastchangetime[ nButton ] ) > KEY_DEBOUNCE ) {
			if ( actstate[ nButton ] ) {
				return 1;
			}
			else {
				return 0;
			}
		}
	}
	
	return -1; // changed or bouncing
}




/******************************************************************************
 *
 * Function Name: main()
 *
 * Description:
 *    This function is the program entry point.  After initializing the
 *    system, it sends a greeting out UART0 then enters an endless loop
 *    echoing chracters on the UART and blinking an LED every half
 *    second.
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
int main(void)
{
	unsigned mycpsr;
	
	char txtFurnaceState[ 12 ];	// Furnace state in text for display
	char txtHeaterState[ 12 ];		// Heater state in text for display
	char wrkbuf[ 80 ];				// Buffer for temprary text strings
	
	// State machine states
	uint8_t state_furnace;	// Furnace states
	uint8_t state_heater;	// Heater states
	uint8_t state_display;	// Display states
	uint8_t state_config;	// Configuration states

	// Time keeping
	uint32_t startTime_millisec;
	uint32_t startTime_halfsec;
	uint32_t startTime_second;
	uint32_t startTime_fivesec;
	uint32_t startTime_minute;			
	
	uint32_t startTime_FuelLoad;			// Transport A+B run to load fuel in burning chamber
	uint32_t startTime_TransportEmpty;  	// Transport A run after preset temp reached
	uint32_t startTime_FireWait;			// Time to wait for fire in furnace
	uint32_t startTime_BurnOutWait;			// Time to wait for fuel to burn out before stop
	uint32_t startTime_WaterHeaterStage2; 	// Timer to activate water heater stage 2
	uint32_t startTime_LightDetect;			// Fire detected.
	uint32_t startTime_LightsOut;			// No fire detected.
		
	short config_val = 0;					// Current configuration value
	short nDispHulk = 0;					// Display state variable switch - HULK
	short nDispHeater = 0;					// Display state variable switch - Heater
	
	// One wire defines
	uint8_t timecnt;
	
  	// System initialization
	sysInit();
	
#if defined(UART0_TX_INT_MODE) || defined(UART0_RX_INT_MODE)
	enableIRQ();
#endif
		
	// Check VSCP persistent storage and
	// restore if needed
	
	if ( !vscp_check_pstorage() ) {	
		// Spoiled or not initialized - reinitialize
		init_app_eeprom();
	}
	
	// Initialize the VSCP functionality
	vscp_init();			

	// Init. Lawicel CAN adapter
	uart1Puts("C\r");	// Close in case channel is open
	uart1Puts("Z0\r");	// No timestamp
	uart1Puts("S6\r");	// 57600 kbps
	uart1Puts("O\r");	// Open
	
	startTime_millisec = getSysTICs();
	startTime_halfsec = getSysTICs();
	startTime_second = getSysTICs();
	startTime_fivesec = getSysTICs();
	startTime_minute = getSysTICs();
	
	startTime_FuelLoad = getSysTICs();
	startTime_TransportEmpty = getSysTICs();
	startTime_FireWait = getSysTICs();
	startTime_BurnOutWait = getSysTICs();
	startTime_WaterHeaterStage2 = getSysTICs();
	startTime_LightDetect = getSysTICs();
	startTime_LightsOut = getSysTICs();
	timecnt = 0;
	
	clrDisplayBuf();

	// LCD initialization
	LCDInit();                    		// LCD Initialization
 	
	LCDSendCommand( LCD_TWO_ROWS );   	// LCD Two rows
	LCDSendCommand( LCD_DISP_ON );      // LCD on
	LCDSendCommand( LCD_CLR_DISP );     // LCD clear
	
	// Start the show....
	DEBUGWRITE("\r\nthe HULK II\r\n");
	DEBUGWRITE("(Control for heat comfort @ home.)\r\n\r\n");	
			
	// Set default state
	state_furnace = FURNACE_STATE_REST;
	DEBUGWRITECRLF("state_furnace = FURNACE_STATE_START");
	
	state_heater = HEATER_STATE_REST;
	DEBUGWRITECRLF("state_heater = HEATER_STATE_REST");
	
	state_display = DISPLAY_STATE_HULK; // DISPLAY_STATE_HULK;
	state_config = CONFIG_STATE_FURNACE_ENABLE;
	
	
	
	// The water heater core temperature
	tempHeater = getHeaterTemp();
	
	while ( TRUE ) {
	
		// Feed the dog
		mycpsr = disableIRQ();
		WDFEED = 0xAA; WDFEED = 0x55;
		restoreIRQ( mycpsr );
				
		///////////////////////////////////////////////////////////////////////
		// 							Reset Buttons
		///////////////////////////////////////////////////////////////////////
		
		// release button locks
		if ( 0 == button_state( BUTTON_STATE0 ) ) {
			button_lock[ BUTTON_STATE0 ] = FALSE; 	// release lock if button is released
		}
		
		if ( 0 == button_state( BUTTON_FORWARD ) ) {
			button_lock[ BUTTON_FORWARD ] = FALSE; 	// release lock if button is released
		}
		
		if ( 0 == button_state( BUTTON_UP ) ) {
			button_lock[ BUTTON_UP ] = FALSE; 		// release lock if button is released
		}
		
		if ( 0 == button_state( BUTTON_DOWN ) ) {
			button_lock[ BUTTON_DOWN ] = FALSE; 	// release lock if button is released
		}
		
		if ( 0 == button_state( BUTTON_ENTER ) ) {
			button_lock[ BUTTON_ENTER ] = FALSE; 	// release lock if button is released
		}
		
		if ( 0 == button_state( BUTTON_FORCE_MOTOR_ON ) ) {
			button_lock[ BUTTON_FORCE_MOTOR_ON ] = FALSE; 	// release lock if button is released
		}
		
		//
		//  If the state0 button is pressed we reset the 
		//  machine.
		//
		if ( ( 1 == button_state( BUTTON_STATE0 ) ) && 
					!button_lock[ BUTTON_STATE0 ] && 
					( VSCP_STATE_INIT != vscp_node_state ) )  {
					
			uart0Puts("\r\nState 0 Button Pressed!\r\n");
			button_lock[ BUTTON_STATE0 ] = TRUE;
			
			// State 0 button pressed
			vscp_nickname = VSCP_ADDRESS_FREE;
			writeEEPROM( VSCP_EEPROM_NICKNAME, VSCP_ADDRESS_FREE );
			vscp_init();
			
			// Set default state
			state_furnace = FURNACE_STATE_REST;
			DEBUGWRITECRLF("state_furnace = FURNACE_STATE_START");
			state_heater = HEATER_STATE_REST;
			DEBUGWRITECRLF("state_heater = HEATER_STATE_REST");
			state_display = DISPLAY_STATE_HULK; // DISPLAY_STATE_HULK;
			state_config = CONFIG_STATE_FURNACE_ENABLE;
		}
		
		//
		// If the force motor ON button is pressed both the
		// transport motors should run
		//
		if ( BtnForceMotorOn() ) {
			
			ctrTransportMotorA( ON );
			ctrTransportMotorB( ON );
			
			// Wait until released
			while ( button_state( BUTTON_FORCE_MOTOR_ON ) ) {
				mycpsr = disableIRQ();
				WDFEED = 0xAA; WDFEED = 0x55;
				restoreIRQ( mycpsr );
			}
		}
		
		// Check for any valid CAN message
		vscp_imsg.flag = 0;
		vscp_receiveMsg();
		
		///////////////////////////////////////////////////////////////////////
		// 								VSCP 
		///////////////////////////////////////////////////////////////////////
		
		switch ( vscp_node_state ) {

			case VSCP_STATE_STARTUP:		// Cold/warm reset

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

			case VSCP_STATE_INIT:			// Assigning nickname
				vscp_probe();
				break;

			case VSCP_STATE_PREACTIVE:		// Waiting for host initialisation
				vscp_rcv_preactive();					
				break;

			case VSCP_STATE_ACTIVE:			// The normal state
				
				if ( vscp_imsg.flag & VSCP_VALID_MSG ) {	// incoming message?
					
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
								if ( ( vscp_nickname == vscp_imsg.data[ 0 ] ) &&
										( 1 == vscp_imsg.data[ 1 ] ) &&
										( readEEPROM( VSCP_EEPROM_REG_GUIID0 + 0 ) == vscp_imsg.data[ 2 ] ) &&
										( readEEPROM( VSCP_EEPROM_REG_GUIID0 + 3 ) == vscp_imsg.data[ 3 ] ) &&
										( readEEPROM( VSCP_EEPROM_REG_GUIID0 + 5 ) == vscp_imsg.data[ 4 ] ) &&
										( readEEPROM( VSCP_EEPROM_REG_GUIID0 + 7 ) == vscp_imsg.data[ 5 ] ) &&
										( ( vscp_page_select >> 8 ) == vscp_imsg.data[ 6 ] ) &&
										( ( vscp_page_select & 0xff ) == vscp_imsg.data[ 7 ] ) ) {											
									// OK, We should enter boot loader mode
									// 	First, activate bootloader mode
									writeEEPROM( VSCP_EEPROM_BOOTLOADER_FLAG, VSCP_BOOT_FLAG );
									while( 1 ); // Let bootloader Reset us
								}
								break;

							default:
								// Do work load
								break;
						}							
					}
				}
				break;

			case VSCP_STATE_ERROR:			// Everything is *very* *very* bad.
				vscp_error();
				break;

			default:					// Should not be here...
				vscp_node_state = VSCP_STATE_STARTUP;
				break;

		}
		
		///////////////////////////////////////////////////////////////////////
		// 						Every milli-Second
		///////////////////////////////////////////////////////////////////////
		if ( getElapsedSysTICs( startTime_millisec ) > ONE_MS ) {
			vscp_timer++;
			startTime_millisec += ONE_MS;
		}
		
		///////////////////////////////////////////////////////////////////////
		// 						Every Half Second 
		///////////////////////////////////////////////////////////////////////
		if ( getElapsedSysTICs( startTime_halfsec ) > HALF_SEC ) {

			// Toggle LED
			if ( IOPIN1 & PIO1_STATUS_LED ) {
				IOCLR1 = PIO1_STATUS_LED;
			}
			else {
				IOSET1 = PIO1_STATUS_LED; 
			}
			
			startTime_halfsec += HALF_SEC;
		}
		
		
		///////////////////////////////////////////////////////////////////////
		// 				            Every Second
		///////////////////////////////////////////////////////////////////////
		if ( getElapsedSysTICs( startTime_second ) > ONE_SEC ) {
						
			// Should the display be updated?				
			LCDSendCommand( LCD_DD_RAM_ADDR1 ); // Set first row
			LCDSendTxt( gdisplaybuf1 );
			LCDSendCommand(LCD_DD_RAM_ADDR2); 	// Set second row
			LCDSendTxt( gdisplaybuf2 );
			
			// Alarm if high temp on Transport B
/*			
			if ( tempTransportB > TRANSPORTB_ALARM_TEMP ) {
				ctrAlarm( ON );
			}
			else {
				ctrAlarm( OFF );
			}
*/			
			
			startTime_second += ONE_SEC;
		}
		
		///////////////////////////////////////////////////////////////////////
		// 						  Every Five Seconds 
		///////////////////////////////////////////////////////////////////////
		if ( getElapsedSysTICs( startTime_fivesec ) > ( 5 * ONE_SEC ) ) {
		
			// The water heater core temperature
			tempHeater = getHeaterTemp();
			
			// The Furnace Core temperature
			tempFurnace = getFurnaceTemp();
	
			DEBUGWRITECRLF( txtFurnaceState );
			
			uart0Puts("\r\n");
			
			startTime_fivesec += ( 5 * ONE_SEC );
		}
		
		///////////////////////////////////////////////////////////////////////
		// 						     Every Minute 
		///////////////////////////////////////////////////////////////////////
		if ( getElapsedSysTICs( startTime_minute ) > (uint32_t)( 60L * ONE_SEC ) ) {
				
			// Send heartbeat
			sendHeartBeat();
			
			// Send temperature info
			sendTempEvent( index_furnace_temp_out, tempFurnace );
			sendTempEvent( index_heater_temp_core, tempHeater );
			
			startTime_minute += ( 60L * ONE_SEC );
		}
		
		
		
		///////////////////////////////////////////////////////////////////////
		// 					   Furnace State Machine 
		///////////////////////////////////////////////////////////////////////
		
		switch ( state_furnace ) {
		
			// * * * * * * * * * * * * * * * * * * * * *
			// A failure has occured	
			//		Must be taken out of here manually
			//		or by a register write.
			// * * * * * * * * * * * * * * * * * * * * *
			case FURNACE_STATE_FAILURE:
			
				ctrFan( OFF );
				ctrFurnaceHeater ( OFF );
				ctrTransportMotorA( OFF );
				ctrTransportMotorB( OFF );
				strcpy( txtFurnaceState, "FAILURE   ");
				
				ctrAlarm( ON );
				
				// If enabled again go active
				if ( 0x80 & readEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END ) ) {
					state_furnace = FURNACE_STATE_REST;
					DEBUGWRITECRLF("state_furnace = FURNACE_STATE_REST");
				}
				
				break;
				
			// * * * * * * * * * * * * * * * * * * * * *	
			// The HULK is off
			//		Must be taken out of here manually
			//		or by a register write.
			// * * * * * * * * * * * * * * * * * * * * *
			case FURNACE_STATE_OFF:
			
				ctrFan( OFF );
				ctrFurnaceHeater ( OFF );
				ctrTransportMotorA( OFF );
				ctrTransportMotorB( OFF );
				strcpy( txtFurnaceState, "OFF       ");
				
				// If enabled again go active
				if ( 0x80 & readEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END ) ) {
					state_furnace = FURNACE_STATE_REST;
					DEBUGWRITECRLF("state_furnace = FURNACE_STATE_REST");
				}
				
				break;
				
			// * * * * * * * * * * * * * * * * * * * * *	
			// The HULK is about to rest
			// * * * * * * * * * * * * * * * * * * * * *	
			case FURNACE_STATE_PREREST:
				
				ctrFan( OFF );
				ctrFurnaceHeater ( OFF );
				ctrTransportMotorA( OFF );
				ctrTransportMotorB( ON );
				strcpy( txtFurnaceState, "PRE-REST  ");
				
				// Empty transport 
				if ( getElapsedSysTICs( startTime_TransportEmpty ) > 
						( ONE_SEC * readEEPROM( REGISTER_TRANSPORTB_POST_RUN_TIME + VSCP_EEPROM_VSCP_END ) ) ) {
					state_furnace = FURNACE_STATE_REST;
					DEBUGWRITECRLF("Tr B Timeout => state_furnace = FURNACE_STATE_REST");
				}
				
				break;
				
			// * * * * * * * * * * * * * * * * * * * * *	
			// The HULK is resting
			//		Preset temp reached
			// * * * * * * * * * * * * * * * * * * * * *
			case FURNACE_STATE_REST:
			
				ctrFan( OFF );
				ctrFurnaceHeater ( OFF );
				ctrTransportMotorA( OFF );
				ctrTransportMotorB( OFF );
				strcpy( txtFurnaceState, "REST      ");
				
				if ( tempFurnace < 
						( readEEPROM( REGISTER_FURNACE_PRESET_TEMP + VSCP_EEPROM_VSCP_END ) - 
						readEEPROM( REGISTER_FURNACE_HYSTERERIS + VSCP_EEPROM_VSCP_END ) ) ) {
				//if ( !isOverTemp() ) {
					startTime_FireWait = getSysTICs();
					startTime_FuelLoad = getSysTICs();
					state_furnace = FURNACE_STATE_PRESTART;
					DEBUGWRITECRLF("temp low => state_furnace = FURNACE_STATE_PRESTART");
				}
				
				// If disabled go to off state
				if ( !( 0x80 & readEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END ) ) ) {
					DEBUGWRITECRLF( itoa( readEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END ) ) );
					state_furnace = FURNACE_STATE_OFF;
					DEBUGWRITECRLF("Disabled => state_furnace = FURNACE_STATE_OFF");
				}
				
				break;
				
			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
			// Furnace should be started up
			//	Fuel should be transported into the burning chamber
			// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
			case FURNACE_STATE_PRESTART:
			
				ctrFan( ON );
				ctrFurnaceHeater ( ON );
				ctrTransportMotorA( ON );
				ctrTransportMotorB( ON );
				strcpy( txtFurnaceState, "PRESTART  ");
				
				// If fire detected we go active
				if ( getFireDetectorLevel() < getFireReferenceLevel() ) {
					startTime_LightDetect = getSysTICs();
					state_furnace = FURNACE_STATE_FIREDETECT;
					DEBUGWRITECRLF("Fire detected => state_furnace = FURNACE_STATE_FIREDETECT");
				}
				
				// If the fuel load time has ellapsed.
				if ( getElapsedSysTICs( startTime_FuelLoad ) > 
						( ONE_SEC * readEEPROM( REGISTER_TRANSPORT_FUEL_FILL_TIME + VSCP_EEPROM_VSCP_END )  ) ) {
					
					ctrTransportMotorA( OFF );
					ctrTransportMotorB( OFF );
					
					startTime_FireWait = getSysTICs();	
					state_furnace = FURNACE_STATE_START;
					
					DEBUGWRITECRLF("fuel load done => state_furnace = FURNACE_STATE_START");
				}
				
				// If preset temp reached we go down
				if ( tempFurnace >= readEEPROM( REGISTER_FURNACE_PRESET_TEMP + VSCP_EEPROM_VSCP_END ) ) {
				//if ( isOverTemp() ) {
					startTime_TransportEmpty = getSysTICs();
					state_furnace = FURNACE_STATE_PREWAIT;
					DEBUGWRITECRLF("temp high => state_furnace = FURNACE_STATE_PREWAIT");
				}
				
				// If disabled go to prewait state
				if ( !( 0x80 & readEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END ) ) ) {
					startTime_TransportEmpty = getSysTICs();
					state_furnace = FURNACE_STATE_PREWAIT;
					DEBUGWRITECRLF("Deactivated => state_furnace = FURNACE_STATE_PREWAIT");
				}
	
				break;
			
			// * * * * * * * * * * * * * * * * * * * * *
			// Fire Detected
			//		Must be detected for a specific amount
			//		of time before recognized.
			// * * * * * * * * * * * * * * * * * * * * *	
			case FURNACE_STATE_FIREDETECT:
			
				// If fire detected we go active if wait time passed
				if ( getFireDetectorLevel() < getFireReferenceLevel() ) {
					// If timedout go to start
					if ( getElapsedSysTICs( startTime_LightDetect ) > ( FIRE_DETECT_TIMEOUT * ONE_SEC ) ) {
						state_furnace = FURNACE_STATE_ACTIVE;
						DEBUGWRITECRLF("Fire detected => state_furnace = FURNACE_STATE_ACTIVE");
					}
				}
				else {
					// Go back to Pre-start
					state_furnace = FURNACE_STATE_START;
					DEBUGWRITECRLF("return from fire detected => state_furnace = FURNACE_STATE_START");
				}
				break;	
			
			// * * * * * * * * * * * * * * * * * * * * *
			// Start
			//		Wait until fire is detected	
			// * * * * * * * * * * * * * * * * * * * * *
			case FURNACE_STATE_START:
				
				ctrFurnaceHeater ( ON );
				ctrFan( ON );
				ctrTransportMotorA( OFF );
				ctrTransportMotorB( OFF );
				strcpy( txtFurnaceState, "START     ");
				
				// If preset temp reached we go down
				if ( tempFurnace >= readEEPROM( REGISTER_FURNACE_PRESET_TEMP + VSCP_EEPROM_VSCP_END ) ) {
				//if ( isOverTemp() ) {
					startTime_TransportEmpty = getSysTICs();
					state_furnace = FURNACE_STATE_PREWAIT;
					DEBUGWRITECRLF("Furnace temp reached.");
					DEBUGWRITECRLF("state_furnace = FURNACE_STATE_PREWAIT");
				}
				
				// If fire detected we go active
				if ( getFireDetectorLevel() < getFireReferenceLevel() ) {
					startTime_LightDetect = getSysTICs();
					state_furnace = FURNACE_STATE_FIREDETECT;
					DEBUGWRITECRLF("Fire detected => state_furnace = FURNACE_STATE_FIREDETECT");
				}
				
				// Check for timeout
				if ( getElapsedSysTICs( startTime_FireWait ) > ( 60L * ONE_SEC * (uint32_t)readEEPROM( REGISTER_FURNACE_RESTART_TIMEOUT + VSCP_EEPROM_VSCP_END ) ) ) {
					
					cntRestarts++;	// Another start attempt
					
					if ( cntRestarts < readEEPROM( REGISTER_FURNACE_RESTART_ATTEMPTS + VSCP_EEPROM_VSCP_END ) ) {
						startTime_FireWait = getSysTICs();
						startTime_FuelLoad = getSysTICs();
						state_furnace = FURNACE_STATE_PRESTART;
						DEBUGWRITECRLF("Another restart.");
						DEBUGWRITECRLF("state_furnace = FURNACE_STATE_PRESTART");
					}
					else {
						
						state_furnace = FURNACE_STATE_FAILURE;
						DEBUGWRITECRLF("Final restart attempt.");
						DEBUGWRITECRLF("state_furnace = FURNACE_STATE_FAILURE");
						
						// Disable Furnace
						writeEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END, 
										( 0x7f & readEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END ) ) );
					}
				}
				
				// If disabled go to prewait state
				if ( !( 0x80 & readEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END ) ) ) {
					startTime_TransportEmpty = getSysTICs();
					state_furnace = FURNACE_STATE_PREWAIT;
					DEBUGWRITECRLF("Furnace disabled.");
					DEBUGWRITECRLF("state_furnace = FURNACE_STATE_PREWAIT");
				}
				
				break;
				
			// * * * * * * * * * * * * * * * * * * * * *
			// Prewait
			//	Preset Temp reached
			//	Get ready to wait 
			// * * * * * * * * * * * * * * * * * * * * *	
			case FURNACE_STATE_PREWAIT:
			
				ctrFurnaceHeater ( OFF );
				ctrFan( ON );
				ctrTransportMotorA( OFF );
				ctrTransportMotorB( ON );
				strcpy( txtFurnaceState, "PREWAIT   ");
				
				// First transport empty
				if ( getElapsedSysTICs( startTime_TransportEmpty ) > ( ONE_SEC * 7 ) ) {
					startTime_BurnOutWait = getSysTICs();
					state_furnace = FURNACE_STATE_WAIT;
					DEBUGWRITECRLF("Tr B Timeout => state_furnace = FURNACE_STATE_WAIT");
				}
				
				break;
				
			// * * * * * * * * * * * * * * * * * * * * *
			// Wait
			// 		Preset Temp reached
			//		Burn out fule in burning chamber
			// * * * * * * * * * * * * * * * * * * * * *
			case FURNACE_STATE_WAIT:
			
				ctrFurnaceHeater ( OFF );
				ctrFan( ON );
				ctrTransportMotorA( OFF );
				ctrTransportMotorB( OFF );
				strcpy( txtFurnaceState, "WAIT      ");
				
				// If temp low start up again
				if ( tempFurnace < 
						( readEEPROM( REGISTER_FURNACE_PRESET_TEMP + VSCP_EEPROM_VSCP_END ) - 
						readEEPROM( REGISTER_FURNACE_HYSTERERIS + VSCP_EEPROM_VSCP_END ) ) ) {
				//if ( !isOverTemp() ) {
							
					// Must be active to turn on again
					if ( 0x80 & readEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END ) ) {
						startTime_FuelLoad = getSysTICs();
						state_furnace = FURNACE_STATE_PRESTART;
						DEBUGWRITECRLF("Temp Low => state_furnace = FURNACE_STATE_PRESTART");
					}
					
				}
				
				// If fan timeout go to rest
				if (  getElapsedSysTICs( startTime_BurnOutWait ) > ( 60L * ONE_SEC * (uint32_t)readEEPROM( REGISTER_FAN_POST_RUN_TIME + VSCP_EEPROM_VSCP_END ) ) ) {
					startTime_TransportEmpty = getSysTICs();
					state_furnace = FURNACE_STATE_PREREST;
					DEBUGWRITECRLF("Fan Timeout => state_furnace = FURNACE_STATE_PREREST");
				}
				
				break;
				
			// * * * * * * * * * * * * * * * * * * * * *
			// Furnace Active
			// 		Wait for fire out and preset temp reached
			// * * * * * * * * * * * * * * * * * * * * *
			case FURNACE_STATE_ACTIVE:
			
				cntRestarts = 0;	
				
				ctrFurnaceHeater ( OFF );
				ctrFan( ON );
				ctrTransportMotorA( ON );
				ctrTransportMotorB( ON );
				strcpy( txtFurnaceState, "ACTIVE    ");
				
				// Check that we still detect a fire
				if ( getFireDetectorLevel() > 
						( getFireReferenceLevel() + ( 256 * readEEPROM( REGISTER_FIRE_DETECTOR_HYSTERERSIS_MSB + VSCP_EEPROM_VSCP_END ) + 
							readEEPROM( REGISTER_FIRE_DETECTOR_HYSTERERSIS_LSB + VSCP_EEPROM_VSCP_END ) ) ) ) {
					state_furnace = FURNACE_STATE_LIGHTS_OUT;
					startTime_LightsOut = getSysTICs();
					DEBUGWRITECRLF("state_furnace = FURNACE_STATE_LIGHTS_OUT");
				}
				
				// Have preset temp been reached
				if ( tempFurnace >= readEEPROM( REGISTER_FURNACE_PRESET_TEMP + VSCP_EEPROM_VSCP_END ) ) {
				//if ( isOverTemp() ) {
					startTime_TransportEmpty = getSysTICs();
					state_furnace = FURNACE_STATE_PREWAIT;
					DEBUGWRITECRLF("state_furnace = FURNACE_STATE_PREWAIT");
				}
				
				// If disabled go to prewait state
				if ( !( 0x80 & readEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END ) ) ) {
					startTime_TransportEmpty = getSysTICs();
					state_furnace = FURNACE_STATE_PREWAIT;
					DEBUGWRITECRLF("state_furnace = FURNACE_STATE_PREWAIT");
				}
				
				break;
				
			// * * * * * * * * * * * * * * * * * * * * *
			// Fire gone out in active state
			//		Try to restart
			// * * * * * * * * * * * * * * * * * * * * *
			case FURNACE_STATE_LIGHTS_OUT:
			
				ctrFurnaceHeater ( ON );
				ctrFan( ON );
				ctrTransportMotorA( OFF );
				ctrTransportMotorB( OFF );
				strcpy( txtFurnaceState, "LIGHTS OUT");
				
				startTime_FireWait = getSysTICs();
				
				// If disabled go to prewait state
				if ( !( 0x80 & readEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END ) ) ) {
					startTime_TransportEmpty = getSysTICs();
					state_furnace = FURNACE_STATE_PREWAIT;
					DEBUGWRITECRLF("state_furnace = FURNACE_STATE_PREWAIT");
				}
				else {
				
					// Lights out has been detected for timeout period and
					// we try to go back to a running state again
					if ( getElapsedSysTICs( startTime_LightsOut ) > ( LIGHTS_OUT_TIMEOUT * ONE_SEC ) ) {
						startTime_FireWait = getSysTICs();
						state_furnace = FURNACE_STATE_START;
						DEBUGWRITECRLF("state_furnace = FURNACE_STATE_PRESTART");
					}
					
					// Have preset temp been reached
					if ( tempFurnace >= readEEPROM( REGISTER_FURNACE_PRESET_TEMP + VSCP_EEPROM_VSCP_END ) ) {
					//if ( isOverTemp() ) {
						startTime_TransportEmpty = getSysTICs();
						state_furnace = FURNACE_STATE_PREWAIT;
						DEBUGWRITECRLF("state_furnace = FURNACE_STATE_PREWAIT");
					}
					
					// If disabled go to prewait state
					if ( !( 0x80 & readEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END ) ) ) {
						startTime_TransportEmpty = getSysTICs();
						state_furnace = FURNACE_STATE_PREWAIT;
						DEBUGWRITECRLF("state_furnace = FURNACE_STATE_PREWAIT");
					}
					
				}
				
				break;
			
			default:
				// This should not happen - set known state.
				state_furnace = FURNACE_STATE_WAIT;
				DEBUGWRITECRLF("state_furnace = FURNACE_STATE_WAIT");
				break;
				
		}
			
		
		///////////////////////////////////////////////////////////////////////
		// 					  Water heater state machine
		///////////////////////////////////////////////////////////////////////
		
		switch ( state_heater ) {
			
			// * * * * * * * * * * * * * * * * * * * * *
			// The Water heater is off
			//		Must be taken out of here manually
			//		or by a register write. 
			// * * * * * * * * * * * * * * * * * * * * *
			case HEATER_STATE_OFF:
				
				ctrElementA( OFF );
				ctrElementB( OFF );
				strcpy( txtHeaterState, "OFF" );
				
				// If enabled again go active
				if ( 0x80 & readEEPROM( REGISTER_STATES1 + VSCP_EEPROM_VSCP_END ) ) {
					state_heater = HEATER_STATE_REST;
					DEBUGWRITECRLF("state_heater = HEATER_STATE_REST");
				}
				break;
				
			// * * * * * * * * * * * * * * * * * * * * *
			// Rest state
			//		 The preset temp has been reached.
			// * * * * * * * * * * * * * * * * * * * * *	
			case HEATER_STATE_REST:
			
				ctrElementA( OFF );
				ctrElementB( OFF );
				strcpy( txtHeaterState, "Rest" );
				
				if ( tempHeater < 
						( readEEPROM( REGISTER_WATER_HEATER_PRESET_TEMP + VSCP_EEPROM_VSCP_END ) - 
						readEEPROM( REGISTER_WATER_HEATER_HYSTERERIS + VSCP_EEPROM_VSCP_END ) ) ) {
					startTime_WaterHeaterStage2 = getSysTICs();
					state_heater = HEATER_STATE_ON_LEVEL1;
					DEBUGWRITECRLF("state_heater = HEATER_STATE_ON_LEVEL1");
				}
				
				// If disabled go to off state
				if ( !( 0x80 & readEEPROM( REGISTER_STATES1 + VSCP_EEPROM_VSCP_END ) ) ) {
					state_heater = HEATER_STATE_OFF;
					DEBUGWRITECRLF("state_heater = HEATER_STATE_OFF");
				}
				
				break;
				
			// * * * * * * * * * * * * * * * * * * * * *
			// Active State 1
			//		 Element A on
			// * * * * * * * * * * * * * * * * * * * * *	
			case HEATER_STATE_ON_LEVEL1:
				
				ctrElementA( ON );
				ctrElementB( OFF );
				strcpy( txtHeaterState, "A" );
				
				if ( tempHeater > ( readEEPROM( REGISTER_WATER_HEATER_PRESET_TEMP + VSCP_EEPROM_VSCP_END ) ) ) {
					state_heater = HEATER_STATE_REST;
					DEBUGWRITECRLF("state_heater = HEATER_STATE_REST");
				}
				
				if ( getElapsedSysTICs( startTime_WaterHeaterStage2 ) > 
						( ONE_SEC * ( ( readEEPROM( REGISTER_WATER_HEATER_STAGE2_DELAY_MSB + VSCP_EEPROM_VSCP_END ) << 8 ) + 
							readEEPROM( REGISTER_WATER_HEATER_STAGE2_DELAY_LSB + VSCP_EEPROM_VSCP_END ) ) ) * ONE_SEC ) {
					state_heater = HEATER_STATE_ON_LEVEL2;
					DEBUGWRITECRLF("state_heater = HEATER_STATE_ON_LEVEL2");
				}
				
				// If disabled go to off state
				if ( !( 0x80 & readEEPROM( REGISTER_STATES1 + VSCP_EEPROM_VSCP_END ) ) ) {
					state_heater = HEATER_STATE_OFF;
					DEBUGWRITECRLF("state_heater = HEATER_STATE_OFF");
				}
				
				break;
				
			// * * * * * * * * * * * * * * * * * * * * *
			// Active State 2
			//		Element A on
			//		Element B on
			// * * * * * * * * * * * * * * * * * * * * *	
			case HEATER_STATE_ON_LEVEL2:
			
				ctrElementA( ON );
				ctrElementB( ON );
				strcpy( txtHeaterState, "A+B" );
				
				if ( tempHeater > 
						( readEEPROM( REGISTER_WATER_HEATER_PRESET_TEMP + VSCP_EEPROM_VSCP_END ) ) ) {
					state_heater = HEATER_STATE_REST;
					DEBUGWRITECRLF("state_heater = HEATER_STATE_REST");
				}
				
				// If disabled go to off state
				if ( !( 0x80 & readEEPROM( REGISTER_STATES1 + VSCP_EEPROM_VSCP_END ) ) ) {
					state_heater = HEATER_STATE_OFF;
					DEBUGWRITECRLF("state_heater = HEATER_STATE_OFF");
				}
				
				break;
				
			default:
				// This should not happen - set known state.
				state_heater = HEATER_STATE_REST;
				DEBUGWRITECRLF("state_heater = HEATER_STATE_REST");
				break;
				
		}
		
		
		
		///////////////////////////////////////////////////////////////////////
		// 						Display state machine
		///////////////////////////////////////////////////////////////////////
		switch ( state_display ) {
		
			// * * * * * * * * * * * * * * * *
			//            STARTUP
			// * * * * * * * * * * * * * * * *
			case DISPLAY_STATE_STARTUP:
			
				strcpy( gdisplaybuf1, "the HULK II" );
				strcpy( gdisplaybuf2, FIRMWARE_TEXT_VERSION );
				break;
			
			// * * * * * * * * * * * * * * * *
			//             HULK 
			// * * * * * * * * * * * * * * * *
			case DISPLAY_STATE_HULK:
			
				strcpy( gdisplaybuf1, "HULK: " );
				strcat( gdisplaybuf1, txtFurnaceState );
				
				// Check buttons
				if ( BtnUp() ) {
					nDispHulk++;
					if ( nDispHulk > 2 ) nDispHulk = 0;
				}
				
				if ( BtnDown() ) {
					nDispHulk--;
					if ( nDispHulk < 0 ) nDispHulk = 2;
				}
				
				if ( BtnForward() ) {
					// Next menu
					state_display = DISPLAY_STATE_HEATER;
				}
				
				switch( nDispHulk ) {
				
					case 0:
						strcpy( gdisplaybuf2, "Temp out: " );
						strcat( gdisplaybuf2, _itoa( tempFurnace, wrkbuf, 10 ) );
						strcat( gdisplaybuf2, "C" );
						break;
						
					case 1:
						strcpy( gdisplaybuf2, "Temp return: " );
						strcat( gdisplaybuf2, _itoa( tempHeater, wrkbuf, 10 ) );
						strcat( gdisplaybuf2, "C" );
						break;
	
					case 2:
						strcpy( gdisplaybuf2, "Temp Tr.B: " );
						strcat( gdisplaybuf2, _itoa( tempFurnace, wrkbuf, 10 ) );
						strcat( gdisplaybuf2, "C" );
						break;
	
					default:	
						nDispHulk = 0;
						break;
						
				}
				
				break;
				
				
				
			// * * * * * * * * * * * * * * * *
			//            HEATER
			// * * * * * * * * * * * * * * * *	
			case DISPLAY_STATE_HEATER:
			
				// Check buttons
				if ( BtnUp() ) {
					nDispHeater++;
					if ( nDispHeater > 1 ) nDispHeater = 0;
				}
				
				if ( BtnDown() ) {
					nDispHeater--;
					if ( nDispHeater < 0 ) nDispHeater = 1;
				}
				
				if ( BtnForward() ) {
					// Next menu
					state_display = DISPLAY_STATE_STATUS1;
				}
			
				strcpy( gdisplaybuf1, "Heater: " );
				strcat( gdisplaybuf1, txtHeaterState );
				
				switch( nDispHeater ) {
				
					case 0:
						strcpy( gdisplaybuf2, "Temp core: " );
						strcat( gdisplaybuf2, _itoa( tempHeater, wrkbuf, 10 ) );
						strcat( gdisplaybuf2, "C" );
						break;
						
					case 1:
						strcpy( gdisplaybuf2, "Temp water: " );
						strcat( gdisplaybuf2, _itoa( tempHeater, wrkbuf, 10 ) );
						strcat( gdisplaybuf2, "C" );
						break;
	
					default:	
						nDispHulk = 0;
						break;
						
				}
				
				break;
			
			// * * * * * * * * * * * * * * * *
			//            STATUS1
			// * * * * * * * * * * * * * * * *
			case DISPLAY_STATE_STATUS1:
				
				if ( BtnForward() ) {
					// Next menu
					state_display = DISPLAY_STATE_STATUS2;
				}
				
				strcpy( gdisplaybuf1, "Temp out: " );
				strcat( gdisplaybuf1, _itoa( tempHeater, wrkbuf, 10 ) );
				strcat( gdisplaybuf1, "C" );
				
				strcpy( gdisplaybuf2, "Temp return: " );
				strcat( gdisplaybuf2, _itoa( tempHeater, wrkbuf, 10 ) );
				strcat( gdisplaybuf2, "C" );
				
				break;
			
			// * * * * * * * * * * * * * * * *
			//            STATUS2
			// * * * * * * * * * * * * * * * *
			case DISPLAY_STATE_STATUS2:
			
				if ( BtnForward() ) {
					// Next menu
					state_display = DISPLAY_STATE_STATUS3;
				}
				
				strcpy( gdisplaybuf1, "Temp core: " );
				strcat( gdisplaybuf1, _itoa( tempHeater, wrkbuf, 10 ) );
				strcat( gdisplaybuf1, "C" );
				
				strcpy( gdisplaybuf2, "Temp water: " );
				strcat( gdisplaybuf2, _itoa( tempHeater, wrkbuf, 10 ) );
				strcat( gdisplaybuf2, "C" );
				
				break;
				
			// * * * * * * * * * * * * * * * *
			//            STATUS3
			// * * * * * * * * * * * * * * * *
			case DISPLAY_STATE_STATUS3:
			
				if ( BtnForward() ) {
					// Next menu
					state_display = DISPLAY_STATE_PRECONFIGURE;
				}
				
				strcpy( gdisplaybuf1, "Fire level: " );
				strcat( gdisplaybuf1, _itoa( getFireDetectorLevel(), wrkbuf, 10 ) );
				
				strcpy( gdisplaybuf2, "Ref. level: " );
				strcat( gdisplaybuf2, _itoa( getFireReferenceLevel(), wrkbuf, 10 ) );
				
				break;	
				
			// * * * * * * * * * * * * * * * *
			//           PRE-CONFIGURE
			// * * * * * * * * * * * * * * * *	
			case DISPLAY_STATE_PRECONFIGURE:
			
				strcpy( gdisplaybuf1, "Press enter for" );
				strcpy( gdisplaybuf2, "config state." );
				
				if ( BtnEnter() ) {
					state_display = DISPLAY_STATE_CONFIGURE;
					state_config = CONFIG_STATE_FURNACE_ENABLE;
					config_val = getConfigValue( CONFIG_STATE_FURNACE_ENABLE );
				}
				
				if ( BtnForward() ) {
					// Next menu
					state_display = DISPLAY_STATE_HULK;
				}
				
				break;
				
			// * * * * * * * * * * * * * * * *
			//           CONFIGURE
			// * * * * * * * * * * * * * * * *	
			case DISPLAY_STATE_CONFIGURE:
				// We are in the configure state machine
				break;	
				
			// * * * * * * * * * * * * * * * *
			//             ALARM
			// * * * * * * * * * * * * * * * *	
			case DISPLAY_STATE_ALARM:
			
				if ( BtnForward() ) {
					// Next menu
					state_display = DISPLAY_STATE_HULK;
				}
				
				break;
			
			default:
				// This should not happen - set known state.
				state_display = DISPLAY_STATE_HULK;
				break;	
		}
		
		// Pad to fill display area
		padBuf( gdisplaybuf1 );
		padBuf( gdisplaybuf2 );
		
		///////////////////////////////////////////////////////////////////////
		// 						Configuration state machine 
		///////////////////////////////////////////////////////////////////////
		
		if ( DISPLAY_STATE_CONFIGURE == state_display ) {
		
			switch ( state_config ) {
			
				// * * * * * * * * * * * * * * * *
				//       HEATER TEMP. SETPOINT
				// * * * * * * * * * * * * * * * *	
				case CONFIG_STATE_PRESET_HEATER_TEMP:
				
					if ( BtnForward() ) {
						// Next config value
						state_config = CONFIG_STATE_PRESET_FURNACE_TEMP;
						config_val = getConfigValue( CONFIG_STATE_PRESET_FURNACE_TEMP );
					}
					
					if ( BtnUp() ) {
						config_val++;
						if ( config_val > 95 ) config_val = 100;
					}
					
					if ( BtnDown() ) {
						config_val--;
						if ( config_val < 0 ) config_val = 0;
					}
					
					if ( BtnEnter() ) {  
						writeEEPROM( REGISTER_WATER_HEATER_PRESET_TEMP + VSCP_EEPROM_VSCP_END, config_val );
						config_val = getConfigValue( CONFIG_STATE_PRESET_HEATER_TEMP );
					}
				
					strcpy( gdisplaybuf1, "cfg: Hot water" );
					strcpy( gdisplaybuf2, "Setpoint " );
					strcat( gdisplaybuf2, _itoa( config_val, wrkbuf, 10 ) );
					strcat( gdisplaybuf2, "C " );
					if ( config_val != readEEPROM( REGISTER_WATER_HEATER_PRESET_TEMP + VSCP_EEPROM_VSCP_END ) ) {
						strcat( gdisplaybuf2, "*" );
					}
					
					break;
				
				// * * * * * * * * * * * * * * * *
				//      FURNACE TEMP. SETPOINT
				// * * * * * * * * * * * * * * * *
				case CONFIG_STATE_PRESET_FURNACE_TEMP:
				
					if ( BtnForward() ) {
						// Next config value
						state_config = CONFIG_STATE_FUEL_TRANSPORT_FILL_TIME;
						config_val = getConfigValue( CONFIG_STATE_FUEL_TRANSPORT_FILL_TIME );
					}
					
					if ( BtnUp() ) {
						config_val++;
						if ( config_val > 100 ) config_val = 100;
					}
					
					if ( BtnDown() ) {
						config_val--;
						if ( config_val < 0 ) config_val = 0;
					}
					
					if ( BtnEnter() ) {
						writeEEPROM( REGISTER_FURNACE_PRESET_TEMP + VSCP_EEPROM_VSCP_END, config_val );
						config_val = getConfigValue( CONFIG_STATE_PRESET_FURNACE_TEMP );
					}
				
					strcpy( gdisplaybuf1, "cfg:Furnace temp" );
					strcpy( gdisplaybuf2, "Setpoint " );
					strcat( gdisplaybuf2, _itoa( config_val, wrkbuf, 10 ) );
					strcat( gdisplaybuf2, " C" );
					if ( config_val != readEEPROM( REGISTER_FURNACE_PRESET_TEMP + VSCP_EEPROM_VSCP_END  ) ) {
						strcat( gdisplaybuf2, "*" );
					}
					
					break;
				
				// * * * * * * * * * * * * * * * *
				//       TRANSPORT FILL TIME
				// * * * * * * * * * * * * * * * *
				case CONFIG_STATE_FUEL_TRANSPORT_FILL_TIME:
				
					if ( BtnForward() ) {
						// Next config value
						state_config = CONFIG_STATE_FUEL_TRANSPORT_POST_TIME;
						config_val = getConfigValue( CONFIG_STATE_FUEL_TRANSPORT_POST_TIME );
					}
					
					if ( BtnUp() ) {
						config_val++;
						if ( config_val > 255 ) config_val = 255;
					}
					
					if ( BtnDown() ) {
						config_val--;
						if ( config_val < 0 ) config_val = 0;
					}
					
					if ( BtnEnter() ) {
						writeEEPROM( REGISTER_TRANSPORT_FUEL_FILL_TIME + VSCP_EEPROM_VSCP_END, config_val );
						config_val = getConfigValue( CONFIG_STATE_FUEL_TRANSPORT_FILL_TIME );
					}
				
					strcpy( gdisplaybuf1, "cfg:Fill time" );
					strcpy( gdisplaybuf2, "Setpoint " );
					strcat( gdisplaybuf2, _itoa( config_val, wrkbuf, 10 ) );
					strcat( gdisplaybuf2, "s " );
					if ( config_val != readEEPROM( REGISTER_TRANSPORT_FUEL_FILL_TIME + VSCP_EEPROM_VSCP_END  ) ) {
						strcat( gdisplaybuf2, "*" );
					}
					
					break;
				
				// * * * * * * * * * * * * * * * *
				//       TRANSPORT POST TIME     
				// * * * * * * * * * * * * * * * *
				case CONFIG_STATE_FUEL_TRANSPORT_POST_TIME:
				
					if ( BtnForward() ) {
						// Next config value
						state_config = CONFIG_STATE_FAN_END_TIME;
						config_val = getConfigValue( CONFIG_STATE_FAN_END_TIME );
					}
					
					if ( BtnUp() ) {
						config_val++;
						if ( config_val > 255 ) config_val = 255;
					}
					
					if ( BtnDown() ) {
						config_val--;
						if ( config_val < 0 ) config_val = 0;
					}
					
					if ( BtnEnter() ) {
						writeEEPROM( REGISTER_TRANSPORTB_POST_RUN_TIME + VSCP_EEPROM_VSCP_END, config_val );
						config_val = getConfigValue( CONFIG_STATE_FUEL_TRANSPORT_POST_TIME );
					}
				
					strcpy( gdisplaybuf1, "cfg:Tr. B end" );
					strcpy( gdisplaybuf2, "Setpoint " );
					strcat( gdisplaybuf2, _itoa( config_val, wrkbuf, 10 ) );
					strcat( gdisplaybuf2, "s " );
					if ( config_val != readEEPROM( REGISTER_TRANSPORTB_POST_RUN_TIME + VSCP_EEPROM_VSCP_END  ) ) {
						strcat( gdisplaybuf2, "*" );
					}
					
					break;
				
				// * * * * * * * * * * * * * * * *
				//           FAN END TIME
				// * * * * * * * * * * * * * * * *
				case CONFIG_STATE_FAN_END_TIME:
				
					if ( BtnForward() ) {
						// Next config value
						state_config = CONFIG_STATE_RESTART_TIMEOUT;
						config_val = getConfigValue( CONFIG_STATE_RESTART_TIMEOUT );
					}
					
					if ( BtnUp() ) {
						config_val++;
						if ( config_val > 60 ) config_val = 60;
					}
					
					if ( BtnDown() ) {
						config_val--;
						if ( config_val < 0 ) config_val = 0;
					}
					
					if ( BtnEnter() ) {
						writeEEPROM( REGISTER_FAN_POST_RUN_TIME + VSCP_EEPROM_VSCP_END, config_val );
						config_val = getConfigValue( CONFIG_STATE_FAN_END_TIME );
					}
				
					strcpy( gdisplaybuf1, "cfg:Fan end" );
					strcpy( gdisplaybuf2, "Setpoint " );
					strcat( gdisplaybuf2, _itoa( config_val, wrkbuf, 10 ) );
					strcat( gdisplaybuf2, "m " );
					if ( config_val != readEEPROM( REGISTER_FAN_POST_RUN_TIME + VSCP_EEPROM_VSCP_END  ) ) {
						strcat( gdisplaybuf2, "*" );
					}
					
					break;
				
				// * * * * * * * * * * * * * * * *
				//         RESTART TIMEOUT
				// * * * * * * * * * * * * * * * *
				case CONFIG_STATE_RESTART_TIMEOUT:
				
					if ( BtnForward() ) {
						// Next config value
						state_config = CONFIG_STATE_RESTART_ATTEMPTS;
						config_val = getConfigValue( CONFIG_STATE_RESTART_ATTEMPTS );
					}
					
					if ( BtnUp() ) {
						config_val++;
						if ( config_val > 255 ) config_val = 255;
					}
					
					if ( BtnDown() ) {
						config_val--;
						if ( config_val < 0 ) config_val = 0;
					}
					
					if ( BtnEnter() ) {
						writeEEPROM( REGISTER_FURNACE_RESTART_TIMEOUT + VSCP_EEPROM_VSCP_END, config_val );
						config_val = getConfigValue( CONFIG_STATE_RESTART_TIMEOUT );
					}
				
					strcpy( gdisplaybuf1, "cfg:restart t.o." );
					strcpy( gdisplaybuf2, "Setpoint " );
					strcat( gdisplaybuf2, _itoa( config_val, wrkbuf, 10 ) );
					strcat( gdisplaybuf2, "m " );
					if ( config_val != readEEPROM( REGISTER_FURNACE_RESTART_TIMEOUT + VSCP_EEPROM_VSCP_END  ) ) {
						strcat( gdisplaybuf2, "*" );
					}
					
					break;
				
				// * * * * * * * * * * * * * * * *
				//       SET RESTART ATTEMPTS
				// * * * * * * * * * * * * * * * *
				case CONFIG_STATE_RESTART_ATTEMPTS:
				
					if ( BtnForward() ) {
						// Next config value
						state_config = CONFIG_STATE_LEAVE;
					}
					
					if ( BtnUp() ) {
						config_val++;
						if ( config_val > 255 ) config_val = 255;
					}
					
					if ( BtnDown() ) {
						config_val--;
						if ( config_val < 0 ) config_val = 0;
					}
					
					if ( BtnEnter() ) {
						writeEEPROM( REGISTER_FURNACE_RESTART_ATTEMPTS + VSCP_EEPROM_VSCP_END, config_val );
						config_val = getConfigValue( CONFIG_STATE_RESTART_ATTEMPTS );
					}
				
					strcpy( gdisplaybuf1, "cfg:# restarts" );
					strcpy( gdisplaybuf2, "Setpoint " );
					strcat( gdisplaybuf2, _itoa( config_val, wrkbuf, 10 ) );
					strcat( gdisplaybuf2, " " );
					if ( config_val != readEEPROM( REGISTER_FURNACE_RESTART_ATTEMPTS + VSCP_EEPROM_VSCP_END  ) ) {
						strcat( gdisplaybuf2, "*" );
					}
					
					break;
				
				// * * * * * * * * * * * * * * * *
				//         ENABLE FURNACE    
				// * * * * * * * * * * * * * * * *
				case CONFIG_STATE_FURNACE_ENABLE:
					
					if ( BtnForward() ) {
						// Next config value
						state_config = CONFIG_STATE_HEATER_ENABLE;
						config_val = getConfigValue( CONFIG_STATE_HEATER_ENABLE );
					}
					
					if ( BtnUp() ) {
						config_val = config_val ? FALSE : TRUE;
						
					}
					
					if ( BtnDown() ) {
						config_val = config_val ? FALSE : TRUE;
					}
					
					if ( BtnEnter() ) {
						if ( config_val ) {
							config_val = 0x80 | readEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END  );
						}
						else {
							config_val = 0x7F & readEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END  );
						}
						
						writeEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END, config_val );
						config_val = getConfigValue( CONFIG_STATE_FURNACE_ENABLE );
						
						state_furnace = FURNACE_STATE_REST;
						startTime_millisec = getSysTICs();
						startTime_FuelLoad = getSysTICs();
						startTime_TransportEmpty = getSysTICs();
						startTime_FireWait = getSysTICs();
			 
					}
				
					strcpy( gdisplaybuf1, "cfg: Furnace" );
					strcpy( gdisplaybuf2, "enabled = " );
					if ( config_val ) {
						strcat( gdisplaybuf2, "ON" );
					}
					else {
						strcat( gdisplaybuf2, "OFF" );
					}
					
					break;
				
				// * * * * * * * * * * * * * * * *
				//          ENABLE HEATER 
				// * * * * * * * * * * * * * * * *
				case CONFIG_STATE_HEATER_ENABLE:
				
					if ( BtnForward() ) {
						// Next config value
						state_config = CONFIG_STATE_ALARM_ENABLE;
						config_val = getConfigValue( CONFIG_STATE_ALARM_ENABLE );
					}
					
					if ( BtnUp() ) {
						config_val = config_val ? FALSE : TRUE;
						
					}
					
					if ( BtnDown() ) {
						config_val = config_val ? FALSE : TRUE;
					}
					
					if ( BtnEnter() ) {
						if ( config_val ) {
							config_val = 0x80 | readEEPROM( REGISTER_STATES1 + VSCP_EEPROM_VSCP_END  );
						}
						else {
							config_val = 0x7F & readEEPROM( REGISTER_STATES1 + VSCP_EEPROM_VSCP_END  );
						}
						writeEEPROM( REGISTER_STATES1 + VSCP_EEPROM_VSCP_END, config_val );
						config_val = getConfigValue( CONFIG_STATE_HEATER_ENABLE );
					}
				
					strcpy( gdisplaybuf1, "cfg: Heater" );
					strcpy( gdisplaybuf2, "enabled = " );
					if ( config_val ) {
						strcat( gdisplaybuf2, "ON" );
					}
					else {
						strcat( gdisplaybuf2, "OFF" );
					}
					
					break;
				
				// * * * * * * * * * * * * * * * *
				//         ENABLE ALARM     
				// * * * * * * * * * * * * * * * *
				case CONFIG_STATE_ALARM_ENABLE:
				
					if ( BtnForward() ) {
						// Next config value
						state_config = CONFIG_STATE_PRESET_HEATER_TEMP;
						config_val = getConfigValue( CONFIG_STATE_PRESET_HEATER_TEMP );
					}
					
					if ( BtnUp() ) {
						config_val = config_val ? FALSE : TRUE;
						
					}
					
					if ( BtnDown() ) {
						config_val = config_val ? FALSE : TRUE;
					}
					
					if ( BtnEnter() ) {
						if ( config_val ) {
							config_val = 0x40 | readEEPROM( REGISTER_STATES1 + VSCP_EEPROM_VSCP_END  );
						}
						else {
							config_val = 0xBF & readEEPROM( REGISTER_STATES1 + VSCP_EEPROM_VSCP_END  );
						}
						writeEEPROM( REGISTER_STATES1 + VSCP_EEPROM_VSCP_END, config_val );
						config_val = getConfigValue( CONFIG_STATE_ALARM_ENABLE );
					}
				
					strcpy( gdisplaybuf1, "cfg: Alarm" );
					strcpy( gdisplaybuf2, "enabled = " );
					if ( config_val ) {
						strcat( gdisplaybuf2, "ON" );
					}
					else {
						strcat( gdisplaybuf2, "OFF" );
					}
					break;
				
				// * * * * * * * * * * * * * * * *
				//           LEAVE CONFIG
				// * * * * * * * * * * * * * * * *
				case CONFIG_STATE_LEAVE:
				
					if ( BtnForward() ) {
						// Next config value
						state_config = CONFIG_STATE_FURNACE_ENABLE;
						config_val = getConfigValue( CONFIG_STATE_FURNACE_ENABLE );
					}
					
					if ( BtnEnter() ) {
						// Leave config state
						state_config = CONFIG_STATE_PRESET_HEATER_TEMP;
						state_display = DISPLAY_STATE_HULK;
					}
					
					strcpy( gdisplaybuf1, "Press Enter to" );
					strcpy( gdisplaybuf2, "leave config." );
					
					break;
			
	
				default:
					// This should not happen - set known state.
					state_config = CONFIG_STATE_LEAVE;
					break;
					
			} // case
			
			// Pad to fill display area
			padBuf( gdisplaybuf1 );
			padBuf( gdisplaybuf2 );
		
		} // if configure state
		
    } // While forever....

	return 0; // Should never come here
  
} // Main


///////////////////////////////////////////////////////////////////////////////
// getConfigValue
//

static uint8_t getConfigValue( uint8_t state )
{
	uint8_t rv;
	
	switch ( state ) {
			
		case CONFIG_STATE_PRESET_HEATER_TEMP:
			rv = readEEPROM( REGISTER_WATER_HEATER_PRESET_TEMP + VSCP_EEPROM_VSCP_END  );
			break;
				
		case CONFIG_STATE_PRESET_FURNACE_TEMP:
			rv = readEEPROM( REGISTER_FURNACE_PRESET_TEMP + VSCP_EEPROM_VSCP_END  );
			break;
				
		case CONFIG_STATE_FUEL_TRANSPORT_FILL_TIME:
			rv = readEEPROM( REGISTER_TRANSPORT_FUEL_FILL_TIME + VSCP_EEPROM_VSCP_END  );	
			break;
				
		case CONFIG_STATE_FUEL_TRANSPORT_POST_TIME:
			rv = readEEPROM( REGISTER_TRANSPORTB_POST_RUN_TIME + VSCP_EEPROM_VSCP_END  );	
			break;
				
		case CONFIG_STATE_FAN_END_TIME:
			rv = readEEPROM( REGISTER_FAN_POST_RUN_TIME + VSCP_EEPROM_VSCP_END  );
			break;
				
		case CONFIG_STATE_RESTART_TIMEOUT:
			rv = readEEPROM( REGISTER_FURNACE_RESTART_TIMEOUT + VSCP_EEPROM_VSCP_END  );
			break;
				
		case CONFIG_STATE_RESTART_ATTEMPTS:
			rv = readEEPROM( REGISTER_FURNACE_RESTART_ATTEMPTS + VSCP_EEPROM_VSCP_END  );
			break;
				
		case CONFIG_STATE_FURNACE_ENABLE:
			rv = ( 0x80 & readEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END  ) );
			break;
				
		case CONFIG_STATE_HEATER_ENABLE:
			rv = ( 0x80 & readEEPROM( REGISTER_STATES1 + VSCP_EEPROM_VSCP_END  ) );
			break;
				
		case CONFIG_STATE_ALARM_ENABLE:
			rv = ( 0x40 & readEEPROM( REGISTER_STATES1 + VSCP_EEPROM_VSCP_END  ) );
			break;
				
		case CONFIG_STATE_LEAVE:
			rv = 0;
			break;
			
		default:
			rv = 0;
			break;
					
	} // case

	return rv;
}

 

/******************************************************************************
 *
 * Function Name: getFireDetectorLevel()
 *
 * Description:
 *    Get A/D value from channel 0 of A/D converter 0. This is the fire detector.
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    the A/D value.
 *
 *****************************************************************************/
static unsigned int getFireDetectorLevel( void )
{
	unsigned mycpsr;
	unsigned int val; // chan;
	
	// sample AIN0 only => bit 0 = 1 
	// CLKDIV = 14 (59/14 = 4.21 < 4.5 MHz) => Bits 8ff = 14-1
	// BURST = 1 => set Bit 16 - wuff: disabled below
	// PDN   = 1 => set Bit 21
	AD0CR = ( 1 | ( ( 14 - 1 ) << 8 ) /*| (1UL<<16)*/ | ( 1UL << 21 ) );
	
	AD0CR  |= ( 1UL << 24 );  					// Start A/D Conversion (START:0=1) 
	
	mycpsr = disableIRQ();
	WDFEED = 0xAA; WDFEED = 0x55;
	restoreIRQ( mycpsr );
		
	while ( ( AD0DR & ( 1UL << 31 ) ) == 0 ); // Wait for the conversion to complete (DONE=1)
	val = ( ( AD0DR >> 6) & 0x03FF );			// Extract the A/D result
	//chan = ( ( AD0DR >> 24) &0x0007 ); 		// Channel (should be 0)
	
	return val;
}

/******************************************************************************
 *
 * Function Name: getFireReferenceLevel()
 *
 * Description:
 *    Get A/D value from channel 1 of A/D converter 0. This is the reference value
 *	  for the fire detector 0-3V.
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    the A/D value.
 *
 *****************************************************************************/
static unsigned int getFireReferenceLevel( void )
{
	unsigned mycpsr;
	unsigned int val; // chan;
	
	// sample AIN1 only => bit 1 = 1 
	// CLKDIV = 14 (59/14 = 4.21 < 4.5 MHz) => Bits 8ff = 14-1
	// BURST = 1 => set Bit 16 - wuff: disabled below
	// PDN   = 1 => set Bit 21
	AD0CR = ( 2 | ( ( 14 - 1 ) << 8 ) /*| (1UL<<16)*/ | ( 1UL << 21 ) );
	
	AD0CR  |= ( 1UL << 24 );  					// Start A/D Conversion (START:0=1) 
	
	mycpsr = disableIRQ();
	WDFEED = 0xAA; WDFEED = 0x55;
	restoreIRQ( mycpsr );
		
	while ( ( AD0DR & ( 1UL << 31 ) ) == 0 ); // Wait for the conversion to complete (DONE=1)
	val = ( ( AD0DR >> 6) & 0x03FF );			// Extract the A/D result
	
	return val;
}

/******************************************************************************
 *
 * Function Name: getHeaterTemp()
 *
 * Description:
 *    Get electrical heater core temperature.
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    Current temperature as an integer.
 *
 *****************************************************************************/
static int getHeaterTemp( void )
{
	unsigned mycpsr;
	uint32_t low;
	uint32_t high;
	uint32_t i;
	
	high = low = 0;
	
	mycpsr = disableIRQ();
	
	for ( i=0; i<0x10000; i++) {
		
		WDFEED = 0xAA; WDFEED = 0x55;
		
		if ( IOPIN0 & PIO0_ECORE_TEMP_BIT ) {
			high++;
		}
		else {
			low++;
		}
	}
	
	restoreIRQ( mycpsr );
	return ( ( ( 100000ul * (unsigned long)high ) / 0xffff ) - 32000ul ) / 470l;	
}


/******************************************************************************
 *
 * Function Name: getFurnaceTemp()
 *
 * Description:
 *    Get Furnace core temperature.
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    Current temperature as an integer.
 *
 *****************************************************************************/
static int getFurnaceTemp( void )
{
	unsigned mycpsr;
	uint32_t low;
	uint32_t high;
	uint32_t i;
	
	high = low = 0;
	
	mycpsr = disableIRQ();
	
	for ( i=0; i<0x10000; i++) {
		
		WDFEED = 0xAA; WDFEED = 0x55;
		
		if ( IOPIN0 & PIO0_FURNACE_TEMP_BIT ) {
			high++;
		}
		else {
			low++;
		}
	}
	
	restoreIRQ( mycpsr );
	return ( ( ( 100000ul * (unsigned long)high ) / 0xffff ) - 32000ul ) / 470l;	
}



///////////////////////////////////////////////////////////////////////////////
// ctrlFan
//

static void ctrFan( BOOL state )
{
	if ( state ) {
		if ( !( IOPIN1 & PIO1_FAN_BIT ) ) {
			IOSET1 = PIO1_FAN_BIT;
			sendRelayState( subzone_furnace_fan, ON );
			DEBUGWRITECRLF("Fan ON");
		}
	}
	else {
		if ( IOPIN1 & PIO1_FAN_BIT ) {
			IOCLR1 = PIO1_FAN_BIT;
			sendRelayState( subzone_furnace_fan, OFF );
			DEBUGWRITECRLF("Fan OFF");
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// ctrTransportMotorA
//

static void ctrTransportMotorA( BOOL state )
{
	if ( state ) {
		if ( !( IOPIN1 & PIO1_TR_MOTOR_A_BIT ) ) {
			IOSET1 = PIO1_TR_MOTOR_A_BIT;
			sendRelayState( subzone_transport_A, ON );
			DEBUGWRITECRLF("Motor A ON");
		}
	}
	else {
		if ( IOPIN1 & PIO1_TR_MOTOR_A_BIT ) {
			IOCLR1 = PIO1_TR_MOTOR_A_BIT;
			sendRelayState( subzone_transport_A, OFF );
			DEBUGWRITECRLF("Motor A OFF");
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// ctrTransportMotorB
//

static void ctrTransportMotorB( BOOL state )
{
	if ( state ) {
		if ( !( IOPIN1 & PIO1_TR_MOTOR_B_BIT ) ) {
			IOSET1 = PIO1_TR_MOTOR_B_BIT;
			sendRelayState( subzone_transport_B, ON );
			DEBUGWRITECRLF("Motor B ON");
		}
	}
	else {
		if ( IOPIN1 & PIO1_TR_MOTOR_B_BIT ) {
			IOCLR1 = PIO1_TR_MOTOR_B_BIT;
			sendRelayState( subzone_transport_B, OFF );
			DEBUGWRITECRLF("Motor B OFF");
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// ctrFurnaceHeater
//

static void ctrFurnaceHeater( BOOL state )
{
	if ( state ) {
		if ( !( IOPIN1 & PIO1_FURNACE_HEATER_BIT ) ) {
			IOSET1 = PIO1_FURNACE_HEATER_BIT;
			sendRelayState( subzone_furnace_heater, ON );
			DEBUGWRITECRLF("Furnace heater ON");
		}
	}
	else {
		if ( IOPIN1 & PIO1_FURNACE_HEATER_BIT ) {
			IOCLR1 = PIO1_FURNACE_HEATER_BIT;
			sendRelayState( subzone_furnace_heater, OFF );
			DEBUGWRITECRLF("Furnace heater OFF");
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// ctrAlarm
//

static void ctrAlarm( BOOL state )
{
	if ( state ) {
		if ( !( IOPIN1 & PIO1_ALARM_BIT ) ) {
			IOSET1 = PIO1_ALARM_BIT;
			DEBUGWRITECRLF("Alarm ON");
		}
	}
	else {
		if ( IOPIN1 & PIO1_ALARM_BIT ) {
			IOCLR1 = PIO1_ALARM_BIT;
			DEBUGWRITECRLF("Alarm OFF");
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// ctrElementA
//

static void ctrElementA( BOOL state )
{
	if ( state ) {
		if ( !( IOPIN1 & PIO1_EHEATER_ELEMENTA_BIT ) ) {
			IOSET1 = PIO1_EHEATER_ELEMENTA_BIT;
			sendRelayState( subzone_heater_ElementA, ON );
			DEBUGWRITECRLF("Water heater elemet A ON");
		}
	}
	else {
		if ( IOPIN1 & PIO1_EHEATER_ELEMENTA_BIT ) {
			IOCLR1 = PIO1_EHEATER_ELEMENTA_BIT;
			sendRelayState( subzone_heater_ElementA, OFF );
			DEBUGWRITECRLF("Water heater elemet A OFF");
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// ctrElementB
//

static void ctrElementB( BOOL state )
{
	if ( state ) {
		if ( !( IOPIN1 & PIO1_EHEATER_ELEMENTB_BIT ) ) {
			IOSET1 = PIO1_EHEATER_ELEMENTB_BIT;
			sendRelayState( subzone_heater_ElementB, ON );
			DEBUGWRITECRLF("Water heater element B ON");
		}
	}
	else {
		if ( IOPIN1 & PIO1_EHEATER_ELEMENTB_BIT ) {
			IOCLR1 = PIO1_EHEATER_ELEMENTB_BIT;
			sendRelayState( subzone_heater_ElementB, OFF );
			DEBUGWRITECRLF("Water heater element B OFF");
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// isOverTemp
//
// Returns true if temperature above setpoint
//

static BOOL isOverTemp( void )
{
	BOOL p1,p2,p3;
	
	p1 = ( IOPIN0 & PIO0_OVER_TEMP_BIT );
	pause( 10 );
	
	p2 = ( IOPIN0 & PIO0_OVER_TEMP_BIT );
	pause( 10 );
	
	p3 = ( IOPIN0 & PIO0_OVER_TEMP_BIT );
	
	return !( p1 || p2 || p3);
}


///////////////////////////////////////////////////////////////////////////////
// init_app_eeprom
//

static void init_app_eeprom( void )
{
	writeEEPROM( REGISTER_ZONE + VSCP_EEPROM_VSCP_END, DEFAULT_ZONE );
	writeEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END, DEFAULT_STATES0 );
	writeEEPROM( REGISTER_STATES1 + VSCP_EEPROM_VSCP_END, DEFAULT_STATES1 );
	writeEEPROM( REGISTER_TRANSPORTB_ALARM_POINT + VSCP_EEPROM_VSCP_END, DEFAULT_TRANSPORTB_ALARM_POINT );
	writeEEPROM( REGISTER_FURNACE_PRESET_TEMP + VSCP_EEPROM_VSCP_END, DEFAULT_FURNACE_PRESET_TEMP );
	writeEEPROM( REGISTER_WATER_HEATER_PRESET_TEMP + VSCP_EEPROM_VSCP_END, DEFAULT_WATER_HEATER_PRESET_TEMP );
	writeEEPROM( REGISTER_WATER_HEATER_CONTROL + VSCP_EEPROM_VSCP_END, DEFAULT_WATER_HEATER_CONTROL );
	writeEEPROM( REGISTER_WATER_HEATER_STAGE2_DELAY_MSB + VSCP_EEPROM_VSCP_END, DEFAULT_WATER_HEATER_STAGE2_DELAY_MSB );
	writeEEPROM( REGISTER_WATER_HEATER_STAGE2_DELAY_LSB + VSCP_EEPROM_VSCP_END, DEFAULT_WATER_HEATER_STAGE2_DELAY_LSB );
	writeEEPROM( REGISTER_FAN_POST_RUN_TIME + VSCP_EEPROM_VSCP_END, DEFAULT_FAN_POST_RUN_TIME );
	writeEEPROM( REGISTER_TRANSPORTB_POST_RUN_TIME + VSCP_EEPROM_VSCP_END, DEFAULT_TRANSPORTB_POST_RUN_TIME );
	writeEEPROM( REGISTER_TRANSPORT_FUEL_FILL_TIME + VSCP_EEPROM_VSCP_END, DEFAULT_TRANSPORT_FUEL_FILL_TIME );
	writeEEPROM( REGISTER_FURNACE_RESTART_TIMEOUT + VSCP_EEPROM_VSCP_END, DEFAULT_FURNACE_RESTART_TIMEOUT );
	writeEEPROM( REGISTER_FURNACE_RESTART_ATTEMPTS + VSCP_EEPROM_VSCP_END, DEFAULT_FURNACE_RESTART_ATTEMPTS );
	writeEEPROM( REGISTER_WATER_HEATER_HYSTERERIS + VSCP_EEPROM_VSCP_END, DEFAULT_WATER_HEATER_HYSTERERIS );
	writeEEPROM( REGISTER_FURNACE_HYSTERERIS + VSCP_EEPROM_VSCP_END, DEFAULT_FURNACE_HYSTERERIS );
	writeEEPROM( REGISTER_FIRE_DETECTOR_HYSTERERSIS_MSB + VSCP_EEPROM_VSCP_END, DEFAULT_FIRE_DETECTOR_HYSTERERSIS_MSB );
	writeEEPROM( REGISTER_FIRE_DETECTOR_HYSTERERSIS_LSB + VSCP_EEPROM_VSCP_END, DEFAULT_FIRE_DETECTOR_HYSTERERSIS_LSB );
	
	DEBUGWRITECRLF("Init Epprom");
}


///////////////////////////////////////////////////////////////////////////////
// read_app_register
//

void read_app_register( unsigned char reg )
{
	//int tmpval;
	uint8_t val;
	//unsigned char unit = readEEPROM( EEPROM_TEMP_UNIT );

	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flag = VSCP_VALID_MSG + 2;
	vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

	vscp_omsg.data[ 0 ] = reg;
	vscp_omsg.data[ 1 ] = 0x00; // default read

	switch ( reg ) {
	
		// Zone
		case REGISTER_ZONE:
			vscp_omsg.data[ 1 ] =  readEEPROM( REGISTER_ZONE + VSCP_EEPROM_VSCP_END );
			break;
			
		// States 0
		case REGISTER_STATES0:
			val = 0;
			if ( IOPIN1 & PIO1_TR_MOTOR_A_BIT ) val |= 0x01;
			if ( IOPIN1 & PIO1_TR_MOTOR_B_BIT ) val |= 0x02;
			if ( IOPIN1 & PIO1_FAN_BIT ) val |= 0x04;
			if ( IOPIN1 & PIO1_FURNACE_HEATER_BIT ) val |= 0x08;
			if ( 0x80 & readEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END ) ) val |= 0x80;
			vscp_omsg.data[ 1 ] =  val;
			break;	
			
		// States 1
		case REGISTER_STATES1:
			val = 0;
			if ( IOPIN1 & PIO1_EHEATER_ELEMENTA_BIT ) val |= 0x01;
			if ( IOPIN1 & PIO1_EHEATER_ELEMENTB_BIT ) val |= 0x02;
			if ( IOPIN1 & PIO1_ALARM_BIT ) val |= 0x20;
			if ( 0x40 & readEEPROM( REGISTER_STATES1 + VSCP_EEPROM_VSCP_END ) ) val |= 0x40;
			if ( 0x80 & readEEPROM( REGISTER_STATES1 + VSCP_EEPROM_VSCP_END ) )val |= 0x80;
			vscp_omsg.data[ 1 ] =  val;
			break;	
			
		// Fire Detector MSB
		case REGISTER_FIRE_DETECTOR_MSB:
			vscp_omsg.data[ 1 ] = getFireDetectorLevel() >> 8;
			break;
			
		// Fire Detector LSB
		case REGISTER_FIRE_DETECTOR_LSB:
			vscp_omsg.data[ 1 ] = getFireDetectorLevel() & 0xff;
			break;	
			
		// Fire Detector reference MSB
		case REGISTER_FIRE_DETECTOR_REFERENCE_MSB:
			vscp_omsg.data[ 1 ] = getFireReferenceLevel() >> 8;
			break;
			
		// Fire Detector reference LSB
		case REGISTER_FIRE_DETECTOR_REFERENCE_LSB:
			vscp_omsg.data[ 1 ] = getFireReferenceLevel() & 0xff;
			break;		
						
		case REGISTER_FURNACE_WATER_OUT_TEMP:
			vscp_omsg.data[ 1 ] = tempFurnace;
			break;
		
		case REGISTER_FURNACE_WATER_RETURN_TEMP:
			vscp_omsg.data[ 1 ] = 0;
			break;
			
		case REGISTER_TRANSPORTB_TEMP:
			vscp_omsg.data[ 1 ] = 0;
			break;
			
		case REGISTER_TRANSPORTB_ALARM_POINT:
			vscp_omsg.data[ 1 ] = 0;
			break;
			
		case REGISTER_FURNACE_PRESET_TEMP:
			break;
			
		case REGISTER_WATER_HEATER_CORE_TEMP:
			vscp_omsg.data[ 1 ] = tempHeater;
			break;	
			
		case REGISTER_WATER_HEATER_PRESET_TEMP:
			break;
			
		case REGISTER_WATER_HEATER_WATER_TEMP:
			vscp_omsg.data[ 1 ] = 0;
			break;
			
		case REGISTER_SMOKE_GAS_TEMP_MSB:
			vscp_omsg.data[ 1 ] = 0;
			break;
			
		case REGISTER_SMOKE_GAS_TEMP_LSB:
			vscp_omsg.data[ 1 ] = 0;
			break;
			
		case REGISTER_CO_MEASUREMENT_MSB:
			vscp_omsg.data[ 1 ] = 0;
			break;
			
		case REGISTER_CO_MEASUREMENT_LSB:
			vscp_omsg.data[ 1 ] = 0;
			break;
			
		case REGISTER_WATER_HEATER_CONTROL:
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_WATER_HEATER_CONTROL + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_WATER_HEATER_STAGE2_DELAY_MSB:
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_WATER_HEATER_STAGE2_DELAY_MSB + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_WATER_HEATER_STAGE2_DELAY_LSB:
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_WATER_HEATER_STAGE2_DELAY_LSB + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_FAN_POST_RUN_TIME:
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_FAN_POST_RUN_TIME + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_TRANSPORTB_POST_RUN_TIME:
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_TRANSPORTB_POST_RUN_TIME + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_TRANSPORT_FUEL_FILL_TIME:
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_TRANSPORT_FUEL_FILL_TIME + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_FURNACE_RESTART_ATTEMPTS:
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_FURNACE_RESTART_ATTEMPTS + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_WATER_HEATER_HYSTERERIS:
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_WATER_HEATER_HYSTERERIS + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_FURNACE_HYSTERERIS:
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_FURNACE_HYSTERERIS + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_FIRE_DETECTOR_HYSTERERSIS_MSB:
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_FIRE_DETECTOR_HYSTERERSIS_MSB + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_FIRE_DETECTOR_HYSTERERSIS_LSB:
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_FIRE_DETECTOR_HYSTERERSIS_LSB + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_FURNACE_RESTART_TIMEOUT:
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_FURNACE_RESTART_TIMEOUT + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_RESERVED0:
			vscp_omsg.data[ 1 ] = 0;
			break;
			
			
		case REGISTER_FURNACE_TEMP_MSB:
			vscp_omsg.data[ 1 ] = 0;
			break;
			
		case REGISTER_FURNACE_TEMP_LSB:	
			vscp_omsg.data[ 1 ] = 0;
			break;
			
		default:
			vscp_omsg.data[ 1 ] = 0;	
			break;
	}

	// Send data
	vscp_sendMsg();

}

///////////////////////////////////////////////////////////////////////////////
// write_app_register
//

void write_app_register( unsigned char reg, unsigned char val )
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flag = VSCP_VALID_MSG + 2;
	vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

	vscp_omsg.data[ 0 ] = reg;
	vscp_omsg.data[ 1 ] = ~val; // error return

	switch ( reg ) {
	
		// Zone
		case REGISTER_ZONE:
			writeEEPROM( REGISTER_ZONE + VSCP_EEPROM_VSCP_END, val );
			vscp_omsg.data[ 1 ] =  readEEPROM( REGISTER_ZONE + VSCP_EEPROM_VSCP_END );
			break;
			
		// States 0
		case REGISTER_STATES0:
			writeEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_STATES0 + VSCP_EEPROM_VSCP_END );
			break;	
			
		// States 1
		case REGISTER_STATES1:
			writeEEPROM( REGISTER_STATES1 + VSCP_EEPROM_VSCP_END, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_STATES1 + VSCP_EEPROM_VSCP_END );
			break;			
						
		case REGISTER_TRANSPORTB_ALARM_POINT:
			writeEEPROM( REGISTER_TRANSPORTB_ALARM_POINT + VSCP_EEPROM_VSCP_END, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_TRANSPORTB_ALARM_POINT + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_FURNACE_PRESET_TEMP:
			writeEEPROM( REGISTER_FURNACE_PRESET_TEMP + VSCP_EEPROM_VSCP_END, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_FURNACE_PRESET_TEMP + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_WATER_HEATER_PRESET_TEMP:
			writeEEPROM( REGISTER_WATER_HEATER_PRESET_TEMP + VSCP_EEPROM_VSCP_END, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_WATER_HEATER_PRESET_TEMP + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_WATER_HEATER_CONTROL:
			writeEEPROM( REGISTER_WATER_HEATER_CONTROL + VSCP_EEPROM_VSCP_END, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_WATER_HEATER_CONTROL + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_WATER_HEATER_STAGE2_DELAY_MSB:
			writeEEPROM( REGISTER_WATER_HEATER_STAGE2_DELAY_MSB + VSCP_EEPROM_VSCP_END, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_WATER_HEATER_STAGE2_DELAY_MSB + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_WATER_HEATER_STAGE2_DELAY_LSB:
			writeEEPROM( REGISTER_WATER_HEATER_STAGE2_DELAY_LSB + VSCP_EEPROM_VSCP_END, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_WATER_HEATER_STAGE2_DELAY_LSB + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_FAN_POST_RUN_TIME:
			writeEEPROM( REGISTER_FAN_POST_RUN_TIME + VSCP_EEPROM_VSCP_END, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_FAN_POST_RUN_TIME + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_TRANSPORTB_POST_RUN_TIME:
			writeEEPROM( REGISTER_TRANSPORTB_POST_RUN_TIME + VSCP_EEPROM_VSCP_END, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_TRANSPORTB_POST_RUN_TIME + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_TRANSPORT_FUEL_FILL_TIME:
			writeEEPROM( REGISTER_TRANSPORT_FUEL_FILL_TIME + VSCP_EEPROM_VSCP_END, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_TRANSPORT_FUEL_FILL_TIME + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_FURNACE_RESTART_ATTEMPTS:
			writeEEPROM( REGISTER_FURNACE_RESTART_ATTEMPTS + VSCP_EEPROM_VSCP_END, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_FURNACE_RESTART_ATTEMPTS + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_WATER_HEATER_HYSTERERIS:
			writeEEPROM( REGISTER_WATER_HEATER_HYSTERERIS + VSCP_EEPROM_VSCP_END, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_WATER_HEATER_HYSTERERIS + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_FURNACE_HYSTERERIS:
			writeEEPROM( REGISTER_FURNACE_HYSTERERIS + VSCP_EEPROM_VSCP_END, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_FURNACE_HYSTERERIS + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_FIRE_DETECTOR_HYSTERERSIS_MSB:
			writeEEPROM( REGISTER_FIRE_DETECTOR_HYSTERERSIS_MSB + VSCP_EEPROM_VSCP_END, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_FIRE_DETECTOR_HYSTERERSIS_MSB + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_FIRE_DETECTOR_HYSTERERSIS_LSB:
			writeEEPROM( REGISTER_FIRE_DETECTOR_HYSTERERSIS_LSB + VSCP_EEPROM_VSCP_END, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_FIRE_DETECTOR_HYSTERERSIS_LSB + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_FURNACE_RESTART_TIMEOUT:
			writeEEPROM( REGISTER_FURNACE_RESTART_TIMEOUT + VSCP_EEPROM_VSCP_END, val ); 
			vscp_omsg.data[ 1 ] = readEEPROM( REGISTER_FURNACE_RESTART_TIMEOUT + VSCP_EEPROM_VSCP_END );
			break;
			
		case REGISTER_RESERVED0:
			vscp_omsg.data[ 1 ] = 0;
			break;
						
		case REGISTER_FURNACE_TEMP_MSB:
			vscp_omsg.data[ 1 ] = 0;
			break;
			
		case REGISTER_FURNACE_TEMP_LSB:	
			vscp_omsg.data[ 1 ] = 0;
			break;
	
		default:
			vscp_omsg.data[ 1 ] = ~val; // error return	
			break;
	}

	// Send data
	vscp_sendMsg();
}

///////////////////////////////////////////////////////////////////////////////
// Get Major version number for this hardware module
//

unsigned char getMajorVersion()
{
	return FIRMWARE_MAJOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get Minor version number for this hardware module
//

unsigned char getMinorVersion()
{
	return FIRMWARE_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get Subminor version number for this hardware module
//

unsigned char getSubMinorVersion()
{
	return FIRMWARE_SUB_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get GUID from EEPROM
//

#ifdef GUID_IN_EEPROM
unsigned char getGuidFromEEprom( unsigned char idx )
{
	return readEEPROM( VSCP_EEPROM_REG_GUIID0 + idx );		
}
#endif

///////////////////////////////////////////////////////////////////////////////
// Get Manufacturer id and subid from EEPROM
//

#ifdef MANUFACTURER_INFO_IN_EEPROM
unsigned char getManufacturerIdFromEEprom( unsigned char idx )
{
	return readEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx );	
}
#endif

///////////////////////////////////////////////////////////////////////////////
// Get the bootloader algorithm code
//

unsigned char getBootLoaderAlgorithm( void ) 
{
	return VSCP_BOOTLOADER_LPC1;	
}

///////////////////////////////////////////////////////////////////////////////
// Get the buffer size
//

unsigned char getBufferSize( void ) 
{
	return 8;	// Standard CAN frame
}

///////////////////////////////////////////////////////////////////////////////
// BtnForward
//

static BOOL BtnForward( void ) 
{
	if ( ( 1 == button_state( BUTTON_FORWARD ) ) && 
			!button_lock[ BUTTON_FORWARD ] )  {
		button_lock[ BUTTON_FORWARD ] = TRUE;
		DEBUGWRITECRLF("Button 'Forward'.");
		return TRUE;
	}
	else {
		return FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////
// BtnUp
//

static BOOL BtnUp( void ) 
{
	if ( ( 1 == button_state( BUTTON_UP ) ) && 
			!button_lock[ BUTTON_UP ] )  {
		button_lock[ BUTTON_UP ] = TRUE;
		DEBUGWRITECRLF("Button 'Up'.");
		return TRUE;
	}
	else {
		return FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////
// BtnDown
//

static BOOL BtnDown( void ) 
{
	if ( ( 1 == button_state( BUTTON_DOWN ) ) && 
			!button_lock[ BUTTON_DOWN ] )  {
		button_lock[ BUTTON_DOWN ] = TRUE;
		DEBUGWRITECRLF("Button 'Down'.");
		return TRUE;
	}
	else {
		return FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////
// BtnEnter
//

static BOOL BtnEnter( void ) 
{
	if ( ( 1 == button_state( BUTTON_ENTER ) ) && 
			!button_lock[ BUTTON_ENTER ] )  {
		button_lock[ BUTTON_ENTER ] = TRUE;
		DEBUGWRITECRLF("Button 'Enter'.");
		return TRUE;
	}
	else {
		return FALSE;
	}
}


///////////////////////////////////////////////////////////////////////////////
// BtnForceMotorOn
//

static BOOL BtnForceMotorOn( void ) 
{
	if ( ( 1 == button_state( BUTTON_FORCE_MOTOR_ON ) ) && 
			!button_lock[ BUTTON_FORCE_MOTOR_ON ] )  {
		button_lock[ BUTTON_FORCE_MOTOR_ON ] = TRUE;
		DEBUGWRITECRLF("Button 'Force Motor On'.");
		return TRUE;
	}
	else {
		return FALSE;
	}
}


