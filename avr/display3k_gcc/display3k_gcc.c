/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	2006-04-21
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 2006-2009 Ake Hedman, eurosource
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

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "vscp_projdefs.h"
#include "vscp_compiler.h"
#include "version.h"
#include <can_at90can128.h>
#include "vscp_firmware.h"
#include "vscp_class.h"
#include "vscp_type.h"
#include "vscp_registers.h"
#include "vscp_actions.h"

//my include files, stefan
#include <display3k_system.h>
#include <display3k_gcc.h>

//for the tft display
#define LCD_DDR DDRB
#include <glcd-Display3000-211.h>
#include <glcd-Display3000-211.c>
#include <font_f-5x8.c>	
#include <font_f-8x14.c>
//#include <nasa256c.h>

#ifndef GUID_IN_EEPROM
// GUID is stored in ROM for this module
//		IMPORTANT!!!
//		if using this GUID in a real environment
//		please set the four least LSB to a unique id.
uint8_t GUID[16] PROGMEM = { 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x02, 0x01
	};
#endif

// Device string is stored in ROM for this module (max 32 bytes)
// 		here we use the MDF file for the Kelvin SHT module
uint8_t vscp_deviceURL[] PROGMEM = "datenheim.de/vx/digiprobe.xml";

// Manufacturer id's is stored in ROM for this module
// offset 0 - Manufacturer device id 0
// offset 1 - Manufacturer device id 1
// offset 2 - Manufacturer device id 2
// offset 3 - Manufacturer device id 3
// offset 4 - Manufacturer subdevice id 0
// offset 5 - Manufacturer subdevice id 1
// offset 6 - Manufacturer subdevice id 2
// offset 7 - Manufacturer subdevice id 3
const uint8_t vscp_manufacturer_id[8] PROGMEM = { 
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0a
};


// Variables
volatile uint16_t measurement_clock=0;	// 1 ms timer counter
volatile uint32_t loopcounter=0;			//Schleifenzähler, ob das Prog noch läuft
volatile uint32_t msgcounter=0;			//messagecounter

volatile int32_t sens_current_raw=0;
volatile double sens_current_off=1210; //offset bei 21°C
volatile double sens_current_offslope=2.3; //slope des dunkeloffsets in digits/K
volatile double sens_current_rng=9.827234e-5;
volatile double sens_current=0;

volatile int32_t sens_temp_raw=0;
volatile double sens_temp_rng=0.4;
volatile double sens_temp_off=-4;
volatile double sens_temp=0;

char txt [32];

// Prototypes
void initTimer(void);
void init_app_eeprom(void);
void doDM();
void doWork();


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
	/*
	if ( !( PINA & 0x01 ) ) {
		// Active
		PORTB &= ~0x02;
		vscp_initbtncnt++;
	}
	else {
		vscp_initbtncnt = 0;
	}
	*/
	// Status LED
	/*
	vscp_statuscnt++;
	if ( ( VSCP_LED_BLINK1 == vscp_initledfunc ) && ( vscp_statuscnt > 100 ) ) {
		if ( PORTB & 0x80 ) {
			PORTB &= ~0x80;
		}
		else {
			PORTB |= 0x80;
		}	
		vscp_statuscnt = 0;
	}
	else if ( VSCP_LED_ON == vscp_initledfunc ) {
		PORTB &= ~0x80;	
		vscp_statuscnt = 0;	
	}
	else if ( VSCP_LED_OFF == vscp_initledfunc ) {
		PORTB |= 0x80;
		vscp_statuscnt = 0;
	}
	*/
}

///////////////////////////////////////////////////////////////////////////////
//  initTimer
//

void initTimer()
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

int main( void )
{	
	loopcounter=0;

	Orientation = Landscape;
	LCD_Init();
	LCD_Cls(white);
	//         text, x,y, font, xscale, yscale, 
	LCD_Backlight(255);

	LCD_Print("sg    Digiprobe", 20, 2, 2, 1, 1, black, white);
	LCD_Print("lux", 36, 2, 2, 1, 1, orange, white);
	LCD_Print("Prototype, not for resale", 8, 20, 1, 1, 1, grey, white);

	LCD_Rect(1,1,174,48,THIN,blue);
	LCD_Draw(1,96,174,96,THIN,blue);

	
	// Initialize the 1ms timer
	initTimer();
	
	sei(); // Enable interrupts
	
	// Init can
	if ( ERROR_OK != can_Open( CAN_BITRATE_125K, 0, 0, 0 ) ) {
//    	uart_puts("Failed to open channel!!\n");
	}
	
  
    // Check VSCP persistent storage and
	// restore if needed
	if ( !vscp_check_pstorage() )
		{
		// Spoiled or not initialized - reinitialize
		init_app_eeprom();
		}
		
	vscp_init();			// Initialize the VSCP functionality
    
    while ( 1 ) 			// Loop Forever
		{  
		
		if ( vscp_initbtncnt > 100 && ( VSCP_STATE_INIT != vscp_node_state ) ) 
			{
			// State 0 button pressed
			vscp_nickname = VSCP_ADDRESS_FREE;
			writeEEPROM( VSCP_EEPROM_NICKNAME, VSCP_ADDRESS_FREE );
			vscp_init();
			}
		
		// Check for any valid CAN message
		vscp_imsg.flags = 0;
		// if there is a valid event, examine it
		if (vscp_getEvent())
			{
			msgcounter+=1;
			#include <display3k_imsgwork.c>
			}

		// do a meaurement if needed
		if ( measurement_clock > 1000 )
			{
			
			measurement_clock = 0;
		  
			// Do VSCP one second jobs 
			vscp_doOneSecondWork();

			//sprintf(txt, "ID=%hu ", vscp_readNicknamePermanent());
			//LCD_Print(txt, 8, 34, 1, 1, 1, red, white);

			loopcounter+=1;
			sprintf(txt, "LC=%lu", loopcounter);
			LCD_Print(txt, 88, 34, 1, 1, 1, red, white);

			sprintf(txt, "MC=%lu", msgcounter);
			LCD_Print(txt, 8, 34, 1, 1, 1, red, white);
			}
	
		switch ( vscp_node_state )
			{
			case VSCP_STATE_STARTUP:			// Cold/warm reset
				// Get nickname from EEPROM
				if ( VSCP_ADDRESS_FREE == vscp_nickname )
				{
					// new on segment need a nickname
					vscp_node_state = VSCP_STATE_INIT; 	
				}
				else
				{
					// been here before - go on
					vscp_node_state = VSCP_STATE_ACTIVE;
					vscp_goActiveState();
				}
				break;

			case VSCP_STATE_INIT:			// Assigning nickname
				vscp_handleProbeState();
				break;

			case VSCP_STATE_PREACTIVE:		// Waiting for host initialisation
				vscp_goActiveState();					
				break;

			case VSCP_STATE_ACTIVE:			// The normal state
					
				if ( vscp_imsg.flags & VSCP_VALID_MSG ) {	// incoming message?
					vscp_handleProtocolEvent();
					}
				break;

			case VSCP_STATE_ERROR:			// Everything is *very* *very* bad.
				vscp_error();
				break;

			default:					// Should not be here...
				vscp_node_state = VSCP_STATE_STARTUP;
				break;

			} // switch 

		doWork();
    
		} // while
     
  /*
		if ( vscp_timer >= 1000 ) {
			
			vscp_timer = 0;
			
     
        // Check for received frame
        if ( ERROR_OK == can_readFrame( &msg ) ) {
        	msg.id++;
			can_SendFrame( &msg );
        }
  */      
}

///////////////////////////////////////////////////////////////////////////////
// init_app_eeprom
//

void init_app_eeprom( void )
{
    uint8_t pos;
    
    writeEEPROM( REG_ZONE + VSCP_EEPROM_END, 0 );
    writeEEPROM( REG_SUBZONE + VSCP_EEPROM_END, 0 );
    
    // Decision matrix storage
    for ( pos = REG_DM_DUMMY; pos < ( REG_DM_DUMMY + DESCION_MATRIX_ELEMENTS * 8 ); pos++ ) {
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


#ifdef PRINT_CAN_EVENTS
	char buf[32];
	uint8_t i;

	sprintf(buf, "tx: %03x/%02x/%02x/", vscpclass, vscptype, nodeid);
	for (i=0; i<size; i++) {
		char dbuf[5];
		sprintf(dbuf, "/%02x", pData[i]);
		strcat(buf, dbuf);
	}
	uart_puts(buf);
#endif

	msg.id = ( (uint32_t)priority << 26 ) |
		( (uint32_t)vscpclass << 16 ) |
		( (uint32_t)vscptype << 8) |
		nodeid;		// nodeaddress (our address)


    msg.flags = CAN_IDFLAG_EXTENDED;
    msg.len = size;
	if ( size ) {
		memcpy( msg.byte, pData, size );
	}

    if ( ERROR_OK != can_SendFrame( &msg ) ) {
        return FALSE;
    }

    return TRUE;
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
        //rv =  readEEPROM( REG_ZONE + VSCP_EEPROM_END );
		rv = 32;
    }

    // SubZone
    else if ( REG_SUBZONE == reg ) {
        rv =  readEEPROM( REG_SUBZONE + VSCP_EEPROM_END );
    }
            
    
    // DM register space    for ( pos = REG_DM_DUMMY; pos < ( REG_DM_DUMMY + DESCION_MATRIX_ELEMENTS * 8 ); pos++ ) {
    else if ( ( reg >= REG_DMATRIX_START ) && ( reg < 0x80) ) {
        rv =  readEEPROM( REG_DM_DUMMY + VSCP_EEPROM_END + ( reg - REG_DMATRIX_START ) );
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

    rv = ~val; // error return

    // Zone
    if ( REG_ZONE == reg ) {
        writeEEPROM( REG_ZONE + VSCP_EEPROM_END, val );
        rv =  readEEPROM( REG_ZONE + VSCP_EEPROM_END );
    }

    // SubZone
	else if ( REG_SUBZONE == reg ) {
            writeEEPROM( REG_SUBZONE + VSCP_EEPROM_END, val );
            rv =  readEEPROM( REG_SUBZONE + VSCP_EEPROM_END );
    }	
            
   
    // DM register space
    else if ( ( reg >= REG_DMATRIX_START ) && ( reg < 0x80) ) {
        writeEEPROM( REG_DM_DUMMY + VSCP_EEPROM_END + ( reg - REG_DMATRIX_START ), val );
        rv =  readEEPROM( REG_DM_DUMMY + VSCP_EEPROM_END + ( reg - REG_DMATRIX_START ) );
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
//                       Implemention of Decision Matrix
//////////////////////////////////////////////////////////////////////////////




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
        dmflags = readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + 
                                    1 + ( VSCP_SIZE_STD_DM_ROW * i ) );

        // Is the DM row enabled?
        if ( dmflags & VSCP_DM_FLAG_ENABLED ) {

            // Should the originating id be checked and if so is it the same?
            if ( !( dmflags & VSCP_DM_FLAG_CHECK_OADDR ) &&
                    !(  vscp_imsg.oaddr == 
                        readEEPROM( VSCP_EEPROM_END + 
                                        REG_DESCION_MATRIX + 
                                        ( VSCP_SIZE_STD_DM_ROW * i ) ) ) ) {
                continue;					
            }	
			

            // Check if zone should match and if so if it match
            if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE  ) {
                if ( vscp_imsg.data[ 1 ] != readEEPROM( VSCP_EEPROM_END + REG_ZONE  ) ) {
                    continue;
                }	
            }				
			
            class_filter = ( ( dmflags & VSCP_DM_FLAG_CLASS_FILTER ) << 8 ) + 
                                    readEEPROM( VSCP_EEPROM_END + 
                                                    REG_DESCION_MATRIX + 
                                                    ( VSCP_SIZE_STD_DM_ROW * i ) + 
                                                    VSCP_DM_POS_CLASSFILTER  );
            class_mask = ( ( dmflags & VSCP_DM_FLAG_CLASS_MASK ) << 8 ) + 
                                    readEEPROM( VSCP_EEPROM_END + 
                                                    REG_DESCION_MATRIX + 
                                                    ( VSCP_SIZE_STD_DM_ROW * i ) +
                                                    VSCP_DM_POS_CLASSMASK  );
            type_filter = readEEPROM( VSCP_EEPROM_END + 
                                    REG_DESCION_MATRIX + 
                                        ( VSCP_SIZE_STD_DM_ROW * i ) + 
                                        VSCP_DM_POS_TYPEFILTER );
            type_mask = readEEPROM( VSCP_EEPROM_END + 
                                        REG_DESCION_MATRIX + 
                                        ( VSCP_SIZE_STD_DM_ROW * i ) + 
                                        VSCP_DM_POS_TYPEMASK  );


            if ( !( ( class_filter ^ vscp_imsg.class ) & class_mask ) &&
                    !( ( type_filter ^ vscp_imsg.type ) & type_mask )) {

                // OK Trigger this action
                switch ( readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTION  ) ) {

                    case ACTION_CTRL_LED:			// Enable relays in arg. bitarry
                        doActionCtrlLed( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );
                        break;

                    case ACTION_HELLO_WORLD: 		// Disable relays in arg. bitarry
                        doActionHelloWorld(dmflags,  readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );	
                        break;


                } // case	
            } // Filter/mask
        } // Row enabled
    } // for each row	
	
	*/

}



///////////////////////////////////////////////////////////////////////////////
// Send Decsion Matrix Information
//

void vscp_getMatrixInfo( char *pData )
{
    pData[ 0 ] = DESCION_MATRIX_ELEMENTS;
    pData[ 1 ] = REG_DESCION_MATRIX;
    pData[ 2 ] = 0;
    pData[ 3 ] = 0;
    pData[ 4 ] = 0;
    pData[ 5 ] = 0;
    pData[ 6 ] = 0;
}


///////////////////////////////////////////////////////////////////////////////
// SendInformationEvent
//
// überarbeiten!

void SendInformationEvent( uint8_t idx, uint8_t eventClass, uint8_t eventTypeId )
{
    vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
    vscp_omsg.flags = VSCP_VALID_MSG + 3;
    vscp_omsg.class = eventClass;
    vscp_omsg.type = eventTypeId;

    vscp_omsg.data[ 0 ] = idx;	// Register
    vscp_omsg.data[ 1 ] = readEEPROM( VSCP_EEPROM_END + REG_ZONE );
    vscp_omsg.data[ 2 ] = readEEPROM( VSCP_EEPROM_END + REG_SUBZONE);

    vscp_sendEvent();	// Send data
}






///////////////////////////////////////////////////////////////////////////////
// doWork
//
// Do work here
//

void doWork( void )
{
;
}


//---------------------------------------------------------------------
// (C) 2010 Stefan Langer
// Einfaches Lesen und speichern von Daten in/aus einem Byte-Array

void array_write(uint8_t* array, uint16_t pos,  uint16_t bytes, void* data)
	{
	uint8_t* d = (uint8_t*) data;
	for(uint16_t i=0; i<bytes; i++) array[i+pos] = d[i];
	}
void array_read (uint8_t* array, uint16_t pos,  uint16_t bytes, void* data)
	{
	uint8_t* d = (uint8_t*) data;
	for(uint16_t i=0; i<bytes; i++) d[i] = array[i+pos];
	}
//------------------------------------
// (C) 2010 Stefan Langer
// Benutzt wird das so:
// int32_t wert=-200000;
// array_write(vscp_omsg.data, 1, 4, (void*)&wert);
//------------------------------------


void LCD_Backlight( unsigned char level)
{
	LCD_BL_DDR |= LCD_BL_OMASK;
	
	if (level==0) 
		{
		LCD_BL_PORT &= LCD_BL_UMASK;
		}
	else LCD_BL_PORT |= LCD_BL_OMASK;
};
