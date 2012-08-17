/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  File: main.c
 *
 *  The SweetBox
 * 	Version information in version.h
 * 	akhe@eurosource.se
 *
 * Copyright (C) 2008-2011 Ake Hedman, eurosource, <akhe@eurosource.se>
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
 * ******************************************************************************
*/

#include <p32xxxx.h>
#include <plib.h> 
#include "GenericTypedefs.h"

// FreeRTOS Scheduler Includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// lwIP includes. 
#include "lwip/api.h" 

#include "vscp_compiler.h"
#include "vscp_projdefs.h"
#include <inttypes.h>                       // Include path set to .../firmware/common/
#include "vscp_level2.h"
#include "i:/src/vscp/common/vscp_class.h"	// Set "i" with setenv_vscp.bat in VSCP root
#include "i:/src/vscp/common/vscp_type.h"	// Set "i" with setenv_vscp.bat in VSCP root
#include "timebase.h"
#include "sweetbox.h"   
#include "version.h"

#include "24lc1025.h"           // EEPROM support
#include "canlib2515.h"         // CAN support

// Scheduler includes.
#include "FreeRTOS.h"
#include "task.h"

// lwIP
#include "lwip/init.h"
#include "lwip/tcpip.h"

// Tasks
#include "taskTcpip.h"
#include "taskHMI.h"
#include "taskTemperature.h"
#include "enc28j60drv/enc28j60.h"

#include "ethernet.def"
#include "helpers.h"

#include "mrf24j40.h"
#include "miwi.def"

// * * * * MiWi Defines * * * *

const uint8_t longAddressLocation = 0x0E;
const uint8_t myLongAddress[8] = {EUI_0,EUI_1,EUI_2,EUI_3,EUI_4,EUI_5,EUI_6,EUI_7};

const uint8_t securityKey = 0x2A;
const uint8_t mySecurityKey[16] = {SECURITY_KEY_00, SECURITY_KEY_01, SECURITY_KEY_02, SECURITY_KEY_03, SECURITY_KEY_04,
	SECURITY_KEY_05, SECURITY_KEY_06, SECURITY_KEY_07, SECURITY_KEY_08, SECURITY_KEY_09, SECURITY_KEY_10, SECURITY_KEY_11, 
	SECURITY_KEY_12, SECURITY_KEY_13, SECURITY_KEY_14, SECURITY_KEY_15};
const uint8_t mySecurityLevel = SECURITY_LEVEL;
const uint8_t myKeySequenceNumber = KEY_SEQUENCE_NUMBER;


const uint8_t availableChannels[ AVAILABLE_CHANNELS_SIZE ] = { ALLOWED_CHANNELS };
const uint8_t myManufacturerString[]="eurosource.se";

extern volatile unsigned portBASE_TYPE *pxCurrentTCB;

// Configuration Bit settings
// SYSCLK = 80 MHz (8MHz Crystal/ FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 10 MHz
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
// Other options are don't care
//

// * * * Hardware * * *

#define SYSCLK  (80000000)
#define PBCLK   (SYSCLK/8)

// I2C data rate
#define Fsck	100000              // I2C busspeed
#define BRG_VAL ((PBCLK/2/Fsck)-2)  // I2C channel config value

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
#pragma config FPBDIV = DIV_8

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



// * * * VSCP * * *
// The device URL (max 32 characters including null termination)
const char vscp_deviceURL[] = "www.eurosource.se/sweet_001.mdf";

sweetbox_regs vscp_user_reg;    // VSCP application registers

// * * * Application * * *

// queues
xQueueHandle queueHMI;  // For human interface

// Private Prototypes

void init_hardware( void );
uint8_t initTempSensor( uint8_t slaveAddress, uint8_t config );
uint16_t readTempSensor( uint8_t slaveAddress );

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName );
void _general_exception_handler( unsigned portLONG ulCause, unsigned portLONG ulStatus );

/*
 * The idle hook is just used to ....
 */
void vApplicationIdleHook( void );
int main( void );

// Timebase
static volatile uint32_t __timer16 = 0;
static volatile uint32_t __timer32 = 0;


///////////////////////////////////////////////////////////////////////////////
// Interrupt Change Notice
//
// Possible sources:
//      Init button.        CN15
//      Froward button.     CN16
//      Backwad button.     CN19

//int debounce_flag;
INTERRUPT( ipl3,_CHANGE_NOTICE_VECTOR, ChangeNotice_Handler )
{
    // clear the interrupt flag
    mCNClearIntFlag();
    mPORTDToggleBits( LED_STATUS );   
}



///////////////////////////////////////////////////////////////////////////////
// init_hardware
//

void init_hardware( void )
{
    
    // Since PBCLK is set Previously
    // Config PBCLK divider to 1:8 for SPI related Pe
    //SYSTEMConfig(( 80000000L), SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE );   
       	
    // Configure the device for maximum performance with Cache Enabled.
    // PBCLK divider will be set at 1:1.
    SYSTEMConfigPerformance( 80000000L );
                                                              
    // Config button inputs as Change Notice Interrupt 
    mPORTDSetPinsDigitalIn( BUTTON_INIT | BUTTON_FORWARD | BUTTON_BACKWARD );
    mCNOpen( ( CN_ON | CN_IDLE_CON),
                ( CN15_ENABLE | CN16_ENABLE | CN19_ENABLE  ),
                ( CN15_PULLUP_ENABLE | CN16_PULLUP_ENABLE | CN19_PULLUP_ENABLE  ) ); 
    mPORTDRead();   // Read the Port to make sure that there is no spurious CN interrups
    ConfigIntCN( ( CHANGE_INT_ON | CHANGE_INT_PRI_3 ) ); // Config interrupt priority for Level3
          
    // Configure I2C channel 1
	OpenI2C1( I2C_EN, BRG_VAL );
	
	// Configure I2C channel 2
	OpenI2C2( I2C_EN, BRG_VAL );
       
    // Enable Interrupts   
    INTEnableSystemMultiVectoredInt();
    
    portDISABLE_INTERRUPTS();
    
    // Test the two EEPROM's
    //uint8_t eeprom_addr = 0x51; // 0x51/0x55 first IC, 0x52/0x56 second IC
    //m24lc1025_writeByte( eeprom_addr, 0x0040, 0x55 );
    //uint16_t rv = m24lc1025_readByte( eeprom_addr, 0x0040 );
   
}

 //////////////////////////////////////////////////////////////////////////////
 // initCAN
 //
 
static void initCAN( void )
{
    // Init CAN pins
    mSetupCANCSDirection();
    mSetupCANResetDirection();
    
	CAN2515InitSPI( 1, PBCLK, SYSCLK );  
	
	CANReset( 1 );
	
	// setup the filter to recieve all messages
	uint16_t filter[] = {0,0,0,0,0,0}; 
	CAN2515SetRXB0Filters( 1, 0, filter );
	CAN2515SetRXB1Filters( 1, 0, &filter[ 2 ] );
	
	CANEnable( 1, CAN_125kbps );
}

 //////////////////////////////////////////////////////////////////////////////
 // initMiWi
 //
 
static void initMiWi( void )
{
    mSetupMiWiCSDirection();
    mSetupMiWiResetDirection();
    mSetupMiWiINTDirection();  
    
    MiWiInit();  
}

///////////////////////////////////////////////////////////////////////////////
// main
//

int main( void )
{  
    init_hardware();    // Init the hardware
    
    // Setup lwIP. 
    lwip_init(); 
    
    // Init TCP layer
    tcpip_init( NULL, NULL );
    
    // Init CAN
    initCAN();
    
    // Init MiWi
    //      Ethernet must be initialised before MiWi as
    //      the SPI channel is initialised by the Ethernet routines
    //initMiWi();
        
        
    
        
    // Start the Web server task    
    sys_thread_new( ( signed portCHAR *)"web", 
                        taskWEBServer, 
                        NULL, 
                        300, 
                        WEBSERVER_PRIORITY );
       
    // Start temp sensor read task
    sys_thread_new( ( signed portCHAR * ) "temp",
                    taskTemperature,
                    NULL,
                    configMINIMAL_STACK_SIZE,
                    TEMPERATURE_PRIORITY );
              
    // Start HMI Control                     	            
    sys_thread_new( ( signed portCHAR * ) "hmi", 
                    taskHMICtrl,
                    NULL,
                    configMINIMAL_STACK_SIZE,
                    HMI_PRIORITY );                                                

    // Finally start the scheduler. 
	vTaskStartScheduler();

	// Will only reach here if there is insufficient heap available to start
	// the scheduler. 
	return 0;

}

///////////////////////////////////////////////////////////////////////////////
// _general_exception_handler
//
//
void _general_exception_handler( unsigned portLONG ulCause, unsigned portLONG ulStatus )
{
	// This overrides the definition provided by the kernel.  Other exceptions 
	// should be handled here. 	
	for( ;; );
}



///////////////////////////////////////////////////////////////////////////////
//                               H O O K E R S 
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// vApplicationStackOverflowHook
//
//

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
	// Look at pxCurrentTCB to see which task overflowed its stack. 
	char *p = pcTaskName;
	for( ;; );
}

///////////////////////////////////////////////////////////////////////////////
// vApplicationIdleHook
//
//

void vApplicationIdleHook( void )
{
/*    
    static portTickType xLastTx = 0;
    portCHAR cTxByte;

	// The idle hook simply sends a string of characters to the USB port.
	// The characters will be buffered and sent once the port is connected. 
	if( ( xTaskGetTickCount() - xLastTx ) > mainUSB_TX_FREQUENCY )
	{
		xLastTx = xTaskGetTickCount();
		for( cTxByte = mainFIRST_TX_CHAR; cTxByte <= mainLAST_TX_CHAR; cTxByte++ )
		{
			vUSBSendByte( cTxByte );
		}		
	}
*/
}

///////////////////////////////////////////////////////////////////////////////
// vApplicationTickHook
//
//

void vApplicationTickHook( void ) 
{
    __timer32++;    // Update timebase 
    __timer16++;   
}    


///////////////////////////////////////////////////////////////////////////////
//                               T I M E B A S E
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// timeBase16Clear
//
//

void timeBase16Clear( void ) 
{
    __timer16 = 0;   
}    

///////////////////////////////////////////////////////////////////////////////
// timeBaseGet16
//
//

uint16_t timeBaseGet16( void ) 
{
    return __timer16;   
}    

///////////////////////////////////////////////////////////////////////////////
// timeBaseGet32
//
//

uint32_t timeBaseGet32( void ) 
{
    return __timer32;   
}



///////////////////////////////////////////////////////////////////////////////
//                                  V S C P
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
// vscp_sendTCPEvent
//

#ifdef VSCP_USE_TCP
int8_t vscp_sendTCPEvent( PVSCPEVENT pevent )
{
    return 0;
}
#endif

///////////////////////////////////////////////////////////////////////////////
// vscp_getTCPEvent
//

#ifdef VSCP_USE_TCP
int8_t vscp_getTCPEvent( PVSCPEVENT pevent )
{
    return 0;
}
#endif

///////////////////////////////////////////////////////////////////////////////
// vscp_sendUDPEvent
//

#ifdef VSCP_USE_UDP
int8_t vscp_sendUDPEvent( PVSCPEVENT pevent )
{
    return 0;
}
#endif


///////////////////////////////////////////////////////////////////////////////
// vscp_getUDPEvent
//

#ifdef VSCP_USE_UDP
int8_t vscp_getUDPEvent( PVSCPEVENT pevent )
{
    return 0;
}
#endif

///////////////////////////////////////////////////////////////////////////////
// readEEPROM
//

uint8_t readEEPROM( uint16_t addr )
{
    return 0; // TODO
}

///////////////////////////////////////////////////////////////////////////////
// writeEEPROM
//

uint8_t writeEEPROM( uint16_t addr, uint8_t data )
{
    return 0; // TODO
}

///////////////////////////////////////////////////////////////////////////////
// Get Major version number for this hardware module
//

uint8_t vscp_getFirmwareMajorVersion()
{
	return SWEETBOX_MAJOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get Minor version number for this hardware module
//

uint8_t vscp_getFirmwareMinorVersion()
{
	return SWEETBOX_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get Subminor version number for this hardware module
//

uint8_t vscp_getFirmwareSubMinorVersion()
{
	return SWEETBOX_SUB_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get GUID from EEPROM
//

#ifdef GUID_IN_EEPROM
uint8_t getGuidFromEEprom( uint8_t idx )
{
	return readEEPROM( VSCP_EEPROM_REG_GUID + idx );		
}
#endif

///////////////////////////////////////////////////////////////////////////////
// Get Manufacturer id and subid from EEPROM
//

#ifdef MANUFACTURER_INFO_IN_EEPROM
uint8_t getManufacturerIdFromEEprom( uint8_t idx )
{
	return readEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID + idx );	
}
#endif

///////////////////////////////////////////////////////////////////////////////
// Get the bootloader algorithm code
//

uint8_t getBootLoaderAlgorithm( void ) 
{
	return VSCP_BOOTLOADER_VSCP;	
}

///////////////////////////////////////////////////////////////////////////////
// Get the buffer size
//

uint8_t getBufferSize( void ) 
{
	return 128;	// Standard CAN frame
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_readNicknamePermanent
//

uint8_t vscp_readNicknamePermanent( void )
{
	return readEEPROM( VSCP_EEPROM_NICKNAME );
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_writeNicknamePermanent
//

void vscp_writeNicknamePermanent( uint8_t nickname )
{
	writeEEPROM( VSCP_EEPROM_NICKNAME, nickname );
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getZone
//

uint8_t vscp_getZone( void )
{
	return readEEPROM( VSCP_EEPROM_ZONE );
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getSubzone
//

uint8_t vscp_getSubzone( void )
{
	return readEEPROM( VSCP_EEPROM_SUBZONE );
}

///////////////////////////////////////////////////////////////////////////////
// vscp_readAppReg
//

uint8_t vscp_readAppReg( uint32_t reg )
{
    uint8_t rv;
	//int tmpval;
	//unsigned char val, checksum;

	rv = 0x00; // default read
	
	return rv;

}

///////////////////////////////////////////////////////////////////////////////
// vscp_writeAppReg
//

uint8_t vscp_writeAppReg( uint32_t reg, uint8_t val )
{
    uint8_t rv;


	rv = ~val; // error return



	// Send response
	//vscp_sendEvent();	
	

	
	return rv;
	
}

///////////////////////////////////////////////////////////////////////////////
// Send Decsion Matrix Information
//

void sendDMatrixInfo( void )
{
/*    
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG + 2;
	vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.type = VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO_RESPONSE;

	vscp_omsg.data[ 0 ] = DESCION_MATRIX_ELEMENTS;
	vscp_omsg.data[ 1 ] = REG_DESCION_MATRIX;

	vscp_sendEvent();	// Send data		
*/
}




///////////////////////////////////////////////////////////////////////////////
// Do decision Matrix handling
// 
// The routine expects vscp_imsg to contain a vaild incoming event
//

void doDM( void )
{
/*
	uint8_t i;
	uint8_t dmflags;
	uint16_t class_filter;
	uint16_t class_mask;
	uint8_t type_filter;
	uint8_t type_mask;
	
	// Don't deal with the control functionality
	if ( VSCP_CLASS1_PROTOCOL == vscp_imsg.class ) return;
	
	for ( i=0; i<DESCION_MATRIX_ELEMENTS; i++ ) {
		
		// Get DM flags for this row
		dmflags = readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + 1 + ( 8 * i ) );
		
		// Is the DM row enabled?
		if ( dmflags & VSCP_DM_FLAG_ENABLED ) {
			
			// Should the originating id be checked and if so is it the same?
			if ( !( dmflags & VSCP_DM_FLAG_CHECK_OADDR ) &&
				!(  vscp_imsg.oaddr == readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) ) ) ) {
				continue;					
			}	
			
			// Check if zone should match and if so if it match
			if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE  ) {
				if ( 255 != vscp_imsg.data[ 1 ] ) {
					if ( vscp_imsg.data[ 1 ] != readEEPROM( VSCP_EEPROM_END + VSCP_EEPROM_SUBZONE  ) ) {
						continue;
					}
				}	
			}				
			
			class_filter = ( dmflags & VSCP_DM_FLAG_CLASS_FILTER )*256 + 
									readEEPROM( VSCP_EEPROM_END + 
													REG_DESCION_MATRIX + 
													( 8 * i ) + 
													VSCP_DM_POS_CLASSFILTER  );
			class_mask = ( dmflags & VSCP_DM_FLAG_CLASS_MASK )*256 + 
									readEEPROM( VSCP_EEPROM_END + 
													REG_DESCION_MATRIX + 
													( 8 * i ) +
													 VSCP_DM_POS_CLASSMASK  );
			type_filter = readEEPROM( VSCP_EEPROM_END + 
										REG_DESCION_MATRIX + 
										( 8 * i ) + 
										VSCP_DM_POS_TYPEFILTER );
			type_mask = readEEPROM( VSCP_EEPROM_END + 
										REG_DESCION_MATRIX + 
										( 8 * i ) + 
										VSCP_DM_POS_TYPEMASK  );
				
			if ( !( ( class_filter ^ vscp_imsg.class ) & class_mask ) &&
				 	!( ( type_filter ^ vscp_imsg.type ) & type_mask )) {

					 						
				// OK Trigger this action
				switch ( readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTION  ) ) {
					
					case ACTION_ON:			// Enable relays in arg. bitarry
						doActionOn( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );
						break;
						
					case ACTION_OFF: 		// Disable relays in arg. bitarry
						doActionOff(dmflags,  readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );	
						break;
						
					case ACTION_PULSE:		// Pulse relays in arg. bitarry, zone, subzone
						doActionPulse( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );
						break;
						
					case ACTION_STATUS:		// Send status for all relays
						doActionStatus( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );
						break;
						
					case ACTION_DISABLE:	// Disable realys in bitarray
						doActionDisable( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );
						break;						
				} // case	
			
			} // Filter/mask
		} // Row enabled
	} // for each row	
*/
}


///////////////////////////////////////////////////////////////////////////////
//                        VSCP Required Methods
//////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// Get Major version number for this hardware module
//

uint8_t vscp_getMajorVersion()
{
	return SWEETBOX_MAJOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get Minor version number for this hardware module
//

uint8_t vscp_getMinorVersion()
{
	return SWEETBOX_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// Get Subminor version number for this hardware module
//

uint8_t vscp_getSubMinorVersion()
{
	return SWEETBOX_SUB_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// getVSCP_GUID
//
// Get GUID from EEPROM
//

uint8_t vscp_getGUID( uint8_t idx )
{
	return readEEPROM( VSCP_EEPROM_REG_GUID + idx );		
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

///////////////////////////////////////////////////////////////////////////////
// Get Manufacturer id and subid from EEPROM
//

uint8_t vscp_getUserID( uint8_t idx )
{
	return readEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID + idx );	
}

///////////////////////////////////////////////////////////////////////////////
//  setVSCPUserID
//

void vscp_setUserID( uint8_t idx, uint8_t data )
{
	writeEEPROM( idx + VSCP_EEPROM_REG_USERID, data );
}

///////////////////////////////////////////////////////////////////////////////
// getVSCPManufacturerId
// 
// Get Manufacturer id and subid from EEPROM
//

uint8_t vscp_getManufacturerId( uint8_t idx )
{
	return readEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID + idx );	
}

///////////////////////////////////////////////////////////////////////////////
// getVSCPManufacturerId
// 
// Get Manufacturer id and subid from EEPROM
//

void vscp_setManufacturerId( uint8_t idx, uint8_t data )
{
	writeEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID + idx, data );	
}

///////////////////////////////////////////////////////////////////////////////
// Get the bootloader algorithm code
//

uint8_t vscp_getBootLoaderAlgorithm( void ) 
{
	return VSCP_BOOTLOADER_VSCP;	
}

///////////////////////////////////////////////////////////////////////////////
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
	return readEEPROM( VSCP_EEPROM_NICKNAME );
}

///////////////////////////////////////////////////////////////////////////////
//  setNickname
//

void vscp_setNickname( uint8_t nickname )
{
	writeEEPROM( VSCP_EEPROM_NICKNAME, nickname );
}

///////////////////////////////////////////////////////////////////////////////
//  getSegmentCRC
//

uint8_t vscp_getSegmentCRC( void )
{
	return readEEPROM( VSCP_EEPROM_SEGMENT_CRC );
}

///////////////////////////////////////////////////////////////////////////////
//  setSegmentCRC
//

void vscp_setSegmentCRC( uint8_t crc )
{
	writeEEPROM( VSCP_EEPROM_SEGMENT_CRC, crc );
}

///////////////////////////////////////////////////////////////////////////////
//  setVSCPControlByte
//

void vscp_setControlByte( uint8_t ctrl )
{
	writeEEPROM( VSCP_EEPROM_CONTROL, ctrl );
}


///////////////////////////////////////////////////////////////////////////////
//  getVSCPControlByte
//

uint8_t vscp_getControlByte( void )
{
	return readEEPROM( VSCP_EEPROM_CONTROL );
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_getEmbeddedMdfInfo
//

void vscp_getEmbeddedMdfInfo( void )
{
/*    
	// No embedded MDF so we respond with info about that
	
	vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
	vscp_omsg.flags = VSCP_VALID_MSG + 3;
	vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

	vscp_omsg.data[ 0 ] = 0;
	vscp_omsg.data[ 1 ] = 0;
	vscp_omsg.data[ 2 ] = 0;	
	
	// send the message
	vscp_sendEvent();
*/
}

///////////////////////////////////////////////////////////////////////////////
// vscp_goBootloaderMode
//

void vscp_goBootloaderMode( void )
{	 											
	// OK, We should enter boot loader mode
	// 	First, activate bootloader mode
	writeEEPROM( VSCP_EEPROM_BOOTLOADER_FLAG, VSCP_BOOT_FLAG );
					
	//_asm goto _startup reset _endasm
	//_asm reset _endasm TODO
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_getMatrixInfo
//

void vscp_getMatrixInfo( void )
{
	uint8_t i;
	
	// We don't implement a DM for this module
	// So we just set the data to zero
	for ( i = 0; i < 8; i++ ) {
		//vscp_omsg.data[ i ] = 0;
	}	

}

///////////////////////////////////////////////////////////////////////////////
// sendVSCPFrame
//

int8_t sendVSCPFrame( uint16_t vscpclass, 
						uint8_t vscptype, 
						uint8_t nodeid,
						uint8_t priority,
						uint8_t size, 
						uint8_t *pData )
{
/*    
	uint32_t id = ( (uint32_t)priority << 26 ) |
						( (uint32_t)vscpclass << 16 ) |
						( (uint32_t)vscptype << 8) |
						nodeid;		// nodeaddress (our address)

	
	if ( !sendCANFrame( id, size, pData ) ) {
		// Failed to send message
		vscp_errorcnt++;
		return FALSE;
	}
*/
	
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// getVSCPFrame
//


int8_t getVSCPFrame( uint16_t *pvscpclass, 
						uint8_t *pvscptype, 
						uint8_t *pNodeId, 
						uint8_t *pPriority, 
						uint8_t *pSize, 
						uint8_t *pData )
{
	uint32_t id;

	
//	if ( !getCANFrame( &id, pSize, pData ) ) {
//		return FALSE;
//	}

	*pNodeId = id & 0x0ff;
	*pvscptype = ( id >> 8 ) & 0xff;
	*pvscpclass = ( id >> 16 ) & 0x1ff;
    *pPriority = (uint16_t)( 0x07 & ( id >> 26 ) );
    
	return TRUE;
}



