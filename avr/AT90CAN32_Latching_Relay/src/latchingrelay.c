/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol)
 * 	http://www.vscp.org
 *
 * 	2006-04-21
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 2006-2011 Ake Hedman, eurosource
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

/*
		Used Hardware resources
		=======================

    PORTC Pin 7 - Status LED
    PORTC Pin 0 - set relay ON
    PORTC Pin 1 - set relay OFF
		PORTA Pin 0 - Init button
*/

#define LED_STATUS_ON       ((PORTC &= ~_BV(7)))
#define LED_STATUS_OFF      ((PORTC |= _BV(7)))
#define LED_STATUS_TOGGLE   ((PORTC ^= _BV(7)))

#define RELAY_ON_ON       ((PORTC |= _BV(0)))
#define RELAY_ON_OFF      ((PORTC &= ~_BV(0)))
#define RELAY_ON_TOGGLE   ((PORTC ^= _BV(0)))

#define RELAY_OFF_ON       ((PORTC |= _BV(1)))
#define RELAY_OFF_OFF      ((PORTC &= ~_BV(1)))
#define RELAY_OFF_TOGGLE   ((PORTC ^= _BV(1)))

#define BTN_INIT_PRESSED    (!(PINA & _BV(0)))
#define BTN_SW1_PRESSED     (!(PINA & _BV(1)))
#define BTN_SW2_PRESSED     (!(PINA & _BV(2)))
#define BTN_SW3_PRESSED     (!(PINA & _BV(3)))
#define BTN_SW4_PRESSED     (!(PINA & _BV(4)))
#define BTN_SW5_PRESSED     (!(PINA & _BV(5)))
#define BTN_SW6_PRESSED     (!(PINA & _BV(6)))
#define BTN_SW7_PRESSED     (!(PINA & _BV(7)))

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "vscp_projdefs.h"
#include "vscp_compiler.h"
#include "version.h"
#include "can_at90can32.h"
#include "uart.h"
#include "vscp_firmware.h"
#include "vscp_class.h"
#include "vscp_type.h"
#include "vscp_registers.h"
#include "vscp_actions.c"
#include "latchingrelay.h"

#ifndef GUID_IN_EEPROM
// GUID is stored in ROM for this module
//		IMPORTANT!!!
//		if using this GUID in a real environment
//		please set the four MSB to a unique id.
//
// IMPORTANT!!!!!
// The GUID is stored MSB byte first
const uint8_t GUID[ 16 ] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01
};
#endif

// Device string is stored in ROM for this module (max 32 bytes)
// a webserver should be running on the pc used for VSCPworks (example apache)
const uint8_t vscp_deviceURL[]  = "lljm/mdf/templ_at90can32.xml";

// Manufacturer id's is stored in ROM for this module
// offset 0 - Manufacturer device id 0
// offset 1 - Manufacturer device id 1
// offset 2 - Manufacturer device id 2
// offset 3 - Manufacturer device id 3
// offset 4 - Manufacturer subdevice id 0
// offset 5 - Manufacturer subdevice id 1
// offset 6 - Manufacturer subdevice id 2
// offset 7 - Manufacturer subdevice id 3
const uint8_t vscp_manufacturer_id[8] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


// Variables
volatile uint16_t measurement_clock;  // 1 ms timer counter

int16_t btncnt[ 8 ];    // Switch counters

// Prototypes
static void initTimer();
static void init_app_eeprom( void );
static void doDM( void );
static void doWork();


///////////////////////////////////////////////////////////////////////////////
// Timer 0 Compare interupt
//
// We should come to this point once every millisecond
//

SIGNAL( SIG_OUTPUT_COMPARE0 )
{
  // handle millisecond counter
  vscp_timer++;
  measurement_clock++;

  // Check for init button
  if ( BTN_INIT_PRESSED ) {
    // Active
    vscp_initbtncnt++;
  }
  else {
    vscp_initbtncnt = 0;
  }
  
    
  // Status LED
  vscp_statuscnt++;
  if ( ( VSCP_LED_BLINK1 == vscp_initledfunc ) && ( vscp_statuscnt > 100 ) ) {
    LED_STATUS_TOGGLE;
    vscp_statuscnt = 0;
  }
  else if ( VSCP_LED_ON == vscp_initledfunc ) {
    LED_STATUS_ON;
    vscp_statuscnt = 0;
  }
  else if ( VSCP_LED_OFF == vscp_initledfunc ) {
    LED_STATUS_OFF;
    vscp_statuscnt = 0;
  }

  relay_pulse_width++;
  if(relay_pulse_width > 200) // pulse width in miliseconds
  {
    RELAY_ON_OFF;
    RELAY_OFF_OFF;
  }

}

///////////////////////////////////////////////////////////////////////////////
//  initTimer
//

static void initTimer()
{
	// Enable timer0 compare interrupt
	TIMSK0 = ( 1 << OCIE0A );

    // Sets the compare value
	// OCR0A = fclk/(2nf) - 1 = fclk/512000 - 1
#ifndef FOSC
#error You must define FOSC in yor makefile
#elif FOSC == 8000
	OCR0A = 32;
#elif FOSC == 16000
	OCR0A = 61;
#endif

	// Set Clear on Timer Compare (CTC) mode, CLK/256 prescaler
	TCCR0A = ( 1 << WGM01 ) | ( 0 << WGM00 ) | ( 4 << CS00 );
}


///////////////////////////////////////////////////////////////////////////////
// main
//

//static int uart1_putchar(char c, FILE *stream);
//static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

int main( void )

{
  //  stdout = &mystdout;
  uint8_t i;

    PORTA   = 0xff;     // Activate pull-ups
    DDRA = 0x00;	    // Port A all inputs
	
    DDRB = 0xFF;	    // Port B all outputs 
    PORTB = 0x00;	    // all LEDS off
    
    DDRC = 0xFF;      // Port C all outputs 
    PORTC = 0x00;     // all LEDS off

    // Initialize UART
    UCSRA = 0;
    UCSRC = MSK_UART_8BIT;	// 8N1
    
    Uart_set_baudrate( 38400 );
    
    UCSRB = MSK_UART_ENABLE_TX | MSK_UART_ENABLE_RX;

    // Initialize the 1ms timer
    initTimer();

    sei(); // Enable interrupts

    // Init can
    if ( ERROR_OK != can_Open( CAN_BITRATE_125K, 0, 0, 0 ) ) {
        uart_puts("Failed to open channel!!\n");
    }

    uart_puts( "VSCP AT90CAN32\n" );
    uart_puts( "Latching relay module\n" );


	// Check VSCP persistent storage and
	// restore if needed
	if ( !vscp_check_pstorage() ) {

		// Spoiled or not initialized - reinitialize
		init_app_eeprom();

	}

	vscp_init();			// Initialize the VSCP functionality

	while ( 1 ) {  // Loop Forever
	  
	  
	  if ( ( vscp_initbtncnt > 200 ) && ( VSCP_STATE_INIT != vscp_node_state ) ) {
	    
            // State 0 button pressed
            vscp_nickname = VSCP_ADDRESS_FREE;
            writeEEPROM( VSCP_EEPROM_NICKNAME, VSCP_ADDRESS_FREE );
            vscp_init();
            uart_puts("Node initialization");
            
	  }
	  
	  
	  // Check for any valid CAN message
	  vscp_imsg.flags = 0;
	  vscp_getEvent();
	  
	  // do a work if it's time for it
	  if ( measurement_clock > 1000 ) {
	    
            measurement_clock = 0;

            relay_pulse_width--;

            // Do VSCP one second jobs 
            vscp_doOneSecondWork();
	    
	    
	  }
	  
	  switch ( vscp_node_state ) {
	    
	  case VSCP_STATE_STARTUP:        // Cold/warm reset
	    
	    
	    // Get nickname from EEPROM
	    if ( VSCP_ADDRESS_FREE == vscp_nickname ) {
	      // new on segment need a nickname
	      vscp_node_state = VSCP_STATE_INIT; 	
	    }
	    else {
	      // been here before - go on
	      vscp_node_state = VSCP_STATE_ACTIVE;
	      vscp_goActiveState();
	    }
	    break;
	    
	    
	  case VSCP_STATE_INIT:           // Assigning nickname
	    vscp_handleProbeState();
	    break;
	    
	  case VSCP_STATE_PREACTIVE:      // Waiting for host initialisation
	    vscp_goActiveState();					
	    break;
	    
	    
	  case VSCP_STATE_ACTIVE:         // The normal state
	    
	    
	    if ( vscp_imsg.flags & VSCP_VALID_MSG ) {	// incoming message?
#ifdef PRINT_CAN_EVENTS
	      char buf[30];
	      uint8_t i;
	      sprintf(buf, "rx: %03x/%02x/%02x/",
          vscp_imsg.vscp_class, vscp_imsg.vscp_type, vscp_imsg.oaddr);
	      for (i=0; i<(vscp_imsg.flags&0xf); i++) {
		char dbuf[5];
		sprintf(dbuf, "/%02x", vscp_imsg.data[i]);
		strcat(buf, dbuf);
	      }
	      uart_puts(buf);
#endif
	      vscp_handleProtocolEvent();
	      
	      doDM();
	      
	    }
	    break;
	    
	    
	  case VSCP_STATE_ERROR:          // Everything is *very* *very* bad.
	    vscp_error();
	    break;
	    
	  default:                        // Should not be here...
	    vscp_node_state = VSCP_STATE_STARTUP;
	    break;
	    
	    
	  } // switch 
	  
	  
	  
	  for ( i=1; i<8; i++ ) {
            
            if ( btncnt[ i ] > 200 ) {
	      uart_puts("Button Pressed!");
	      SendInformationEvent( i-1, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON );
	      btncnt[ i ] = -300;
            }
            
	  }
	  
	  doWork();
	  
	} // while
	
}

///////////////////////////////////////////////////////////////////////////////
// init_app_eeprom
//

static void init_app_eeprom( void )
{
  uint8_t pos;
  
  writeEEPROM( REG_ZONE + VSCP_EEPROM_END, 0 );
  writeEEPROM( REG_SUBZONE + VSCP_EEPROM_END, 0 );
//  writeEEPROM( REG_LED_CONTROL + VSCP_EEPROM_END, 0 );
  
  // Decision matrix storage
  for ( pos = REG_DM_START; pos < ( REG_DM_START + DESCION_MATRIX_ELEMENTS * 8 ); pos++ ) {
    writeEEPROM( pos + VSCP_EEPROM_END, 0 );
  }
}




///////////////////////////////////////////////////////////////////////////////
//   						VSCP Functions
//							--------------
//
// All methods below must be implemented to handle VSCP functionality
//
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
// sendVSCPFrame
//
//

int8_t sendVSCPFrame( uint16_t vscpclass,
          uint8_t vscptype,
          uint8_t nodeid,
          uint8_t priority,
          uint8_t size,
          uint8_t *pData )
{
  CANMsg msg;
  uint8_t timeout = 200;
  int8_t canRet = ERROR_OK;
  int8_t rv = FALSE;

  /* If necessary, limit the size to avoid buffer out of bounce access. */
  if (8 < size)
  {
    size = 8;
  }
  
#ifdef PRINT_CAN_EVENTS
  char buf[32];
  uint8_t i;
  
  sprintf(buf, "tx: %03x/%02x/%02x/\n", vscpclass, vscptype, nodeid);
  uart_puts(buf);
  for (i = 0; i < size; i++) {
    sprintf(buf, "/%02x", pData[i]);
    uart_puts(buf);
  }
#endif
  
  msg.id = ( (uint32_t)priority << 26 ) |
    ( (uint32_t)vscpclass << 16 ) |
    ( (uint32_t)vscptype << 8) |
    nodeid;   // nodeaddress (our address)
  
  
  msg.flags = CAN_IDFLAG_EXTENDED;
  msg.len = size;
  if ( size ) {
    memcpy( msg.byte, pData, size );
  }
  
  do {

      canRet = can_SendFrame( &msg );
      --timeout;

  } while((ERROR_OK != canRet) && (0 < timeout));
  
  if (ERROR_OK == canRet)
  {
    rv = TRUE;
  }
  
  return rv;

}

///////////////////////////////////////////////////////////////////////////////
// getVSCPFrame
//
//

int8_t getVSCPFrame( uint16_t *pvscpclass,
		     uint8_t *pvscptype,
		     uint8_t *pNodeId,
		     uint8_t *pPriority,
		     uint8_t *pSize,
		     uint8_t *pData )
{
  CANMsg msg;
  
  // Check for received frame
  if ( ERROR_OK != can_readFrame( &msg ) ) {
    return FALSE;
  }
  
  *pNodeId = msg.id & 0x0ff;
  *pvscptype = ( msg.id >> 8 ) & 0xff;
  *pvscpclass = ( msg.id >> 16 ) & 0x1ff;
  *pPriority = (uint16_t)( 0x07 & ( msg.id >> 26 ) );
  *pSize = msg.len;
  if ( msg.len ) {
    memcpy( pData, msg.byte, msg.len );
  }
  
  return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// readEEPROM
//
//

int readEEPROM( uint8_t addr )
{
    while ( EECR & ( 1 << EEWE ) );
    EEAR = addr;
    EECR |= ( 1 << EERE );
    return EEDR;
}

///////////////////////////////////////////////////////////////////////////////
// writeEEPROM
//
//

int writeEEPROM( uint8_t addr, uint8_t data )
{
    while ( EECR & ( 1 << EEWE ) );
    EEAR = addr;
    EEDR=data;
    EECR |= ( 1 << EEMWE );
    EECR |= ( 1 << EEWE );
    return TRUE;
}



///////////////////////////////////////////////////////////////////////////////
//                        VSCP Required Methods
//////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// vscp_readAppReg
//

uint8_t vscp_readAppReg( uint8_t reg )
{
    uint8_t rv;


    // Zone
    if ( REG_ZONE == reg ) {
        rv =  readEEPROM( REG_ZONE + VSCP_EEPROM_END );
    }

    // SubZone
    else if ( REG_SUBZONE == reg ) {
        rv =  readEEPROM( REG_SUBZONE + VSCP_EEPROM_END );
    }
            
    
    // DM register space    for ( pos = REG_DM_DUMMY; pos < ( REG_DM_DUMMY + DESCION_MATRIX_ELEMENTS * 8 ); pos++ ) {
    else if ( ( reg >= REG_DM_START ) && ( reg < REG_DM_START + DESCION_MATRIX_ELEMENTS * 8) ) {
        rv =  readEEPROM( VSCP_EEPROM_END +  reg  );
    }

    else {
        rv = 0xff;
    }


    return rv;

}

///////////////////////////////////////////////////////////////////////////////
// vscp_writeAppReg
//

uint8_t vscp_writeAppReg( uint8_t reg, uint8_t val )
{
    uint8_t rv;
    char buf[30];

    sprintf(buf, "Writing app reg: %x", VSCP_EEPROM_END + reg);
    uart_puts( buf );
    sprintf(buf, "Writing value: %x", val);
    uart_puts( buf );

    rv = ~val; // error return

    // Zone
    if ( REG_ZONE == reg ) {
        writeEEPROM( REG_ZONE + VSCP_EEPROM_END, val );
        rv =  readEEPROM( REG_ZONE + VSCP_EEPROM_END );
        uart_puts("wrote reg_zone\n");
    }

    // SubZone
	else if ( REG_SUBZONE == reg ) {
            writeEEPROM( REG_SUBZONE + VSCP_EEPROM_END, val );
            rv =  readEEPROM( REG_SUBZONE + VSCP_EEPROM_END );
    }	
            

    
    // DM register space
    else if ( ( reg >= REG_DM_START ) && ( reg < 0x80) ) {
        writeEEPROM(( VSCP_EEPROM_END + reg ), val );
        rv =  readEEPROM(( VSCP_EEPROM_END + reg ) );
    }
	

    else {
        rv = ~val; // error return	
    }


    return rv;	

}

//////////////////////////////////////////////////////////////////////////////
// vscp_readNicknamePermanent
//
// Fetch nickname from permanent storage
// return read nickname.
//

uint8_t vscp_readNicknamePermanent( void )
{
    return readEEPROM( VSCP_EEPROM_NICKNAME );
}

///////////////////////////////////////////////////////////////////////////////
// 	vscp_writeNicknamePermanent
//
//	Write nickname to permanent storage
//

void vscp_writeNicknamePermanent( uint8_t nickname )
{
    writeEEPROM( VSCP_EEPROM_NICKNAME, nickname );
}


///////////////////////////////////////////////////////////////////////////////
//	vscp_getZone
//
//	Get Zone for device
//	Just return zero if not used.
//

uint8_t vscp_getZone( void )
{
    return readEEPROM( VSCP_EEPROM_REG_ZONE );
}


///////////////////////////////////////////////////////////////////////////////
//	vscp_getSubzone
//
//	Get Subzone for device
// 	Just return zero if not used.
//

uint8_t vscp_getSubzone( void )
{
    return readEEPROM( VSCP_EEPROM_REG_SUBZONE );
}

///////////////////////////////////////////////////////////////////////////////
//	vscp_goBootloaderMode
//
//	Go bootloader mode
//	This routine force the system into bootloader mode according
//	to the selected protocol.
//

void vscp_goBootloaderMode( void )
{
    // TODO
}


///////////////////////////////////////////////////////////////////////////////
// vscp_getEmbeddedMdfInfo
//
//	Get embedded MDF info
//	If available this routine sends an embedded MDF file
//	in several events. See specification CLASS1.PROTOCOL
//	Type=35/36
//

void vscp_getEmbeddedMdfInfo( void )
{
    // TODO
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getMajorVersion
//
// Get Major version number for this hardware module
//

uint8_t vscp_getMajorVersion()
{
    return FIRMWARE_MAJOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getMinorVersion
//
// Get Minor version number for this hardware module
//

uint8_t vscp_getMinorVersion()
{
    return FIRMWARE_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getSubMinorVersion
//
// Get Subminor version number for this hardware module
//

uint8_t vscp_getSubMinorVersion()
{
    return FIRMWARE_SUB_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getGUID
//
// Get GUID from EEPROM
//

uint8_t vscp_getGUID( uint8_t idx )
{
#ifdef GUID_IN_EEPROM
    return readEEPROM( VSCP_EEPROM_REG_GUID + idx );
#else
    return GUID[ idx ];
#endif
}


///////////////////////////////////////////////////////////////////////////////
// vscp_setGUID
//
// Write GUID if it is in EEPROM
//

void vscp_setGUID( uint8_t idx, uint8_t data )
{
#ifdef GUID_IN_EEPROM
    writeEEPROM( VSCP_EEPROM_REG_GUID + idx, data );	
#endif	

}

///////////////////////////////////////////////////////////////////////////////
// vscp_getMDF_URL
//
// Get device URL from EEPROM
//

uint8_t vscp_getMDF_URL( uint8_t idx )
{
    uart_puts("Passing through vscp_getMDF_URL()\n");
    return vscp_deviceURL[ idx ];  
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getUserID
//
// Get Manufacturer id and subid from EEPROM
//

uint8_t vscp_getUserID( uint8_t idx )
{
    return readEEPROM( VSCP_EEPROM_REG_USERID + idx );	

}

///////////////////////////////////////////////////////////////////////////////
//  vscp_setUserID
//

void vscp_setUserID( uint8_t idx, uint8_t data )
{
    writeEEPROM( VSCP_EEPROM_REG_USERID + idx, data );
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getManufacturerId
//
// Get Manufacturer id and subid from EEPROM
//

uint8_t vscp_getManufacturerId( uint8_t idx )
{
    return readEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx );	

}

///////////////////////////////////////////////////////////////////////////////
// vscp_setManufacturerId
//
// Get Manufacturer id and subid from EEPROM
//

void vscp_setManufacturerId( uint8_t idx, uint8_t data )
{
    writeEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx, data );	

}

///////////////////////////////////////////////////////////////////////////////
// vscp_getBootLoaderAlgorithm
//
// Get the bootloader algorithm code
//

uint8_t vscp_getBootLoaderAlgorithm( void )
{
    return VSCP_BOOTLOADER_AVR1; 	

}

///////////////////////////////////////////////////////////////////////////////
// vscp_getBufferSize
//
// Get the buffer size
//

uint8_t vscp_getBufferSize( void )
{
    return VSCP_SIZE_STD_DM_ROW;	// Standard CAN frame
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getRegisterPagesUsed
//
// Return number of register pages this application uses.
//

uint8_t vscp_getRegisterPagesUsed( void )
{
    return 1;   // Application uses one register page
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_getNickname
//

uint8_t vscp_getNickname( void )
{
    return readEEPROM( VSCP_EEPROM_NICKNAME );
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_setNickname
//

void vscp_setNickname( uint8_t nickname )
{
    writeEEPROM( VSCP_EEPROM_NICKNAME, nickname );
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_getSegmentCRC
//

uint8_t vscp_getSegmentCRC( void )
{
    return readEEPROM( VSCP_EEPROM_SEGMENT_CRC );
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_setSegmentCRC
//

void vscp_setSegmentCRC( uint8_t crc )
{
    writeEEPROM( VSCP_EEPROM_SEGMENT_CRC, crc );
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_setControlByte
//

void vscp_setControlByte( uint8_t ctrl )
{
    writeEEPROM( VSCP_EEPROM_CONTROL, ctrl );
}


///////////////////////////////////////////////////////////////////////////////
//  vscp_getControlByte
//

uint8_t vscp_getControlByte( void )
{
    return readEEPROM( VSCP_EEPROM_CONTROL );
}


///////////////////////////////////////////////////////////////////////////////
//  vscp_getFamilyCode
//
uint32_t vscp_getFamilyCode(void)
{
        return 0;
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_getFamilyType
//
uint32_t vscp_getFamilyType(void)
{
        return 0;
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_restoreDefaults
//
void vscp_restoreDefaults(void)
{
// to do
}

///////////////////////////////////////////////////////////////////////////////
//                       Implemention of Decision Matrix
//////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
// Do decision Matrix handling
//
// The routine expects vscp_imsg to contain a vaild incoming event
//

static void doDM( void )
{
    uint8_t i;
    uint8_t dmflags;
    uint16_t class_filter;
    uint16_t class_mask;
    uint8_t type_filter;
    uint8_t type_mask;

uart_puts("doDM\n");
    // Don't deal with the control functionality
    if ( VSCP_CLASS1_PROTOCOL == vscp_imsg.vscp_class ) return;

    for ( i=0; i<DESCION_MATRIX_ELEMENTS; i++ ) {

        // Get DM flags for this row
        dmflags = readEEPROM( VSCP_EEPROM_END + REG_DM_START + 1 + ( VSCP_SIZE_STD_DM_ROW * i ) );

        char buf[30];
        sprintf(buf, "dmflags for row %i: %i\n", i, dmflags);
        uart_puts(buf);


		uart_puts( "Checking DM rows\n" );

        // Is the DM row enabled?
        if ( dmflags & VSCP_DM_FLAG_ENABLED ) {

			uart_puts( "DM flag enabled - [OK]\n" );

            // Should the originating id be checked and if so is it the same?
            if ( !( dmflags & VSCP_DM_FLAG_CHECK_OADDR ) &&
                    !(  vscp_imsg.oaddr == 
                        readEEPROM( VSCP_EEPROM_END + REG_DM_START +
                                        ( VSCP_SIZE_STD_DM_ROW * i ) ) ) ) {
                continue;					
            }	
			

            // Check if zone should match and if so if it match
            if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE  ) {
                    uart_puts( "Zone should match...\n" );

                if ( vscp_imsg.data[ 1 ] != readEEPROM( VSCP_EEPROM_END + REG_ZONE  ) ) {

                    uart_puts( "Zone mismatch, continue...\n" );
                    continue;

                }	
                    uart_puts( "Zone match - [OK]\n" );
            }				
			
            class_filter = ( ( dmflags & VSCP_DM_FLAG_CLASS_FILTER ) << 8 ) + 
                                    readEEPROM( VSCP_EEPROM_END + 
                                                    REG_DM_START + 
                                                    ( VSCP_SIZE_STD_DM_ROW * i ) + 
                                                    VSCP_DM_POS_CLASSFILTER  );
            class_mask = ( ( dmflags & VSCP_DM_FLAG_CLASS_MASK ) << 8 ) + 
                                    readEEPROM( VSCP_EEPROM_END + 
                                                    REG_DM_START + 
                                                    ( VSCP_SIZE_STD_DM_ROW * i ) +
                                                    VSCP_DM_POS_CLASSMASK  );
            type_filter = readEEPROM( VSCP_EEPROM_END + 
                                    REG_DM_START + 
                                        ( VSCP_SIZE_STD_DM_ROW * i ) + 
                                        VSCP_DM_POS_TYPEFILTER );
            type_mask = readEEPROM( VSCP_EEPROM_END + 
                                        REG_DM_START + 
                                        ( VSCP_SIZE_STD_DM_ROW * i ) + 
                                        VSCP_DM_POS_TYPEMASK  );


            if ( !( ( class_filter ^ vscp_imsg.vscp_class ) & class_mask ) &&
                    !( ( type_filter ^ vscp_imsg.vscp_type ) & type_mask )) {

                // OK Trigger this action
                switch ( readEEPROM( VSCP_EEPROM_END + REG_DM_START + ( 8 * i ) + VSCP_DM_POS_ACTION  ) ) {

                    case ACTION_ACTION1:
                        uart_puts( "Executing action 1\n" );
                        doActionAction1();
                        sprintf(buf, "relay_pulse_width : %i\n", relay_pulse_width);
                        uart_puts(buf);

                        break;

                    case ACTION_ACTION2:
                        doActionAction2();	
                        break;


                } // case	
            } // Filter/mask
        } // Row enabled
    } // for each row	

}



///////////////////////////////////////////////////////////////////////////////
// Send Decsion Matrix Information
//

void vscp_getMatrixInfo( char *pData )
{
    pData[ 0 ] = DESCION_MATRIX_ELEMENTS;
    pData[ 1 ] = REG_DM_START;
    pData[ 2 ] = 0;
    pData[ 3 ] = 0;
    pData[ 4 ] = 0;
    pData[ 5 ] = 0;
    pData[ 6 ] = 0;
    uart_puts("DM\n");
}


///////////////////////////////////////////////////////////////////////////////
// SendInformationEvent
//

void SendInformationEvent( uint8_t idx, uint8_t eventClass, uint8_t eventTypeId )
{
    vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
    vscp_omsg.flags = VSCP_VALID_MSG + 3;
    vscp_omsg.vscp_class = eventClass;
    vscp_omsg.vscp_type = eventTypeId;

    vscp_omsg.data[ 0 ] = idx;	// Register
    vscp_omsg.data[ 1 ] = readEEPROM( VSCP_EEPROM_END + REG_ZONE );
    vscp_omsg.data[ 2 ] = readEEPROM( VSCP_EEPROM_END + REG_SUBZONE + idx );

    vscp_sendEvent();	// Send data
}

///////////////////////////////////////////////////////////////////////////////
// doWork
//
// Do work here
//

void doWork( void )
{

  if(relay_pulse_width == 0)
  {
    RELAY_ON_OFF;
  }
//  else
//  {
//    relay_pulse_width--;
//  }

}


