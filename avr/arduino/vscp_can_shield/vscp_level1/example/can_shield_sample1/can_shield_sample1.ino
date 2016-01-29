/* ******************************************************************************
 * VSCP (Very Simple Control Protocol) 
 * http://www.vscp.org
 *
 *    Arduino VSCP CAN shield example
 *    This board
 *    https://www.sparkfun.com/products/10039
 *    http://www.skpang.co.uk/catalog/arduino-canbus-shield-with-usd-card-holder-p-706.html
 *    is used for this sample.
 *
 *  Copyright (C) 1995-2012 Ake Hedman, Grodans Paradis AB, http://www.auto.grodansparadis.com/products.html
 *                          <akhe@grodansparadis.com>  
 *  Part of the VSCP Project - http://www.vscp.org
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

#include <EEPROM.h>
#include <SPI.h>

#include <vscp_class.h>
#include <vscp_compiler.h>
#include <vscp_firmware.h>
#include <vscp_projdefs.h>
#include <vscp_type.h>

#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>

// Pin definitions specific to how the MCP2515 is wired up.
#define CS_PIN    10
#define INT_PIN    2

// Pin 7/8 has an LED connected on most the CAN Shields.
int LED_USER = 7;
int LED_STATUS = 8;

EEPROMClass eeprom;
MCP2515 mcp2515( CS_PIN, INT_PIN );
 
// Set to non zero to get debug messages on the serial channel
// The control of the serial channel will be disabled in debug mode.
#define DEBUG  1

// Debound time in milliseconds for joystick
#define DEBOUNCE_TIMER  100

// Firmware version

#define FIRMWARE_MAJOR_VERSION                  0
#define FIRMWARE_MINOR_VERSION		        0
#define FIRMWARE_SUB_MINOR_VERSION	        2

// Number of rows in decision matrix
#define SIZE_OF_DM                              8

// EEPROM Storage
#define VSCP_EEPROM_BOOTLOADER_FLAG             0x00	// Reserved for bootloader	 

#define VSCP_EEPROM_NICKNAME			0x01	// Persistant nickname id storage
#define VSCP_EEPROM_SEGMENT_CRC			0x02	// Persistant segment crc storage
#define VSCP_EEPROM_CONTROL			0x03	// Persistant control byte

#define VSCP_EEPROM_REG_USERID			0x04
#define VSCP_EEPROM_REG_USERID1			0x05
#define VSCP_EEPROM_REG_USERID2			0x06
#define VSCP_EEPROM_REG_USERID3			0x07
#define VSCP_EEPROM_REG_USERID4			0x08

// The following can be stored in flash or eeprom

#define VSCP_EEPROM_REG_MANUFACTUR_ID0		0x09
#define VSCP_EEPROM_REG_MANUFACTUR_ID1		0x0A
#define VSCP_EEPROM_REG_MANUFACTUR_ID2		0x0B
#define VSCP_EEPROM_REG_MANUFACTUR_ID3		0x0C

#define VSCP_EEPROM_REG_MANUFACTUR_SUBID0	0x0D	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID1	0x0E	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID2	0x0F	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID3	0x10

// The following can be stored in program ROM (recommended) or in EEPROM 

#define VSCP_EEPROM_REG_GUID			0x11	// Start of GUID MSB	
						        // 0x11 - 0x20				

#define VSCP_EEPROM_REG_DEVICE_URL		0x21	// Start of Device URL storage
							// 0x21 - 0x40

#define VSCP_EEPROM_END				0x41	// marks end of VSCP EEPROM usage
							// (next free position)
// -----------------------------------------------

// * * *  Default values * * *

#define DEFAULT_ZONE                             0 
#define DEFAULT_SUBZONE                          0
#define DEFAULT_CTRL_D3                          0 // Input, no pull-up
#define DEFAULT_CTRL_D4                          0 // Input, no pull-up
#define DEFAULT_CTRL_D5                          0 // Input, no pull-up
#define DEFAULT_CTRL_D6                          0 // Input, no pull-up
#define DEFAULT_CTRL_A0                          0 // 
#define DEFAULT_CTRL_SERIAL                      0 // Closed, no stream events
#define DEFAULT_BAUD_SERIAL_MSB                  0x25 // 9600 baud
#define DEFAULT_BAUD_SERIAL_LSB                  0x80 // 
#define DEFAULT_CTRL_JOYSTICK                    0 
#define DEFAULT_ZONE_ON                          0
#define DEFAULT_SUBZONE_ON                       0
#define DEFAULT_ZONE_BUTTON                      0
#define DEFAULT_SUBZONE_BUTTON                   0
#define DEFAULT_SECONDS_IO                       0   // Periodic I/O events disabled.
#define DEFAULT_SECONDS_AD                       0   // Periodic A/D events disabled.


// * * *  Persistent storage * * *

#define EEPROM_ZONE                             (VSCP_EEPROM_END + 1) 
#define EEPROM_SUBZONE                          (VSCP_EEPROM_END + 2)
#define EEPROM_CTRL_D3                          (VSCP_EEPROM_END + 3)
#define EEPROM_CTRL_D4                          (VSCP_EEPROM_END + 4)
#define EEPROM_CTRL_D5                          (VSCP_EEPROM_END + 5)
#define EEPROM_CTRL_D6                          (VSCP_EEPROM_END + 6)
#define EEPROM_CTRL_A0                          (VSCP_EEPROM_END + 7)
#define EEPROM_CTRL_SERIAL                      (VSCP_EEPROM_END + 8)
#define EEPROM_BAUD_SERIAL_MSB                  (VSCP_EEPROM_END + 9)
#define EEPROM_BAUD_SERIAL_LSB                  (VSCP_EEPROM_END + 10)
#define EEPROM_CTRL_JOYSTICK                    (VSCP_EEPROM_END + 11)
#define EEPROM_ZONE_ON                          (VSCP_EEPROM_END + 12)
#define EEPROM_SUBZONE_ON                       (VSCP_EEPROM_END + 13)
#define EEPROM_ZONE_BUTTON                      (VSCP_EEPROM_END + 14)
#define EEPROM_SUBZONE_BUTTON                   (VSCP_EEPROM_END + 15)
#define EEPROM_SECONDS_IO_EVENT                 (VSCP_EEPROM_END + 16)
#define EEPROM_SECONDS_AD_EVENT                 (VSCP_EEPROM_END + 17)
#define EEPROM_DM_START                         (VSCP_EEPROM_END + 18)

// * * *  Registers * * *

#define REG_ZONE                      0
#define REG_SUBZONE                   1
#define REG_CTRL_D3                   2
#define REG_CTRL_D4                   3
#define REG_CTRL_D5                   4
#define REG_CTRL_D6                   5
#define REG_CTRL_A0                   6
#define REG_ANALOG_A0_MSB             7
#define REG_ANALOG_A0_LSB             8
#define REG_CTRL_SERIAL               9
#define REG_BAUD_SERIAL_MSB           10
#define REG_BAUD_SERIAL_LSB           11
#define REG_CTRL_JOYSTICK             12
#define REG_ZONE_ON                   13
#define REG_SUBZONE_ON                14
#define REG_ZONE_BUTTON               15
#define REG_SUBZONE_BUTTON            16
#define REG_SECONDS_IO_EVENT          17
#define REG_SECONDS_AD_EVENT          18
#define REG_CTRL_LEDS                 19

#define REG_DM_START                  64

// * * *  Actions * * *

#define ACTION_NOOP                   0
#define ACTION_GOOD_DAY               1
#define ACTION_SERIAL_OPEN            2
#define ACTION_SERIAL_CLOSE           3
#define ACTION_SERIAL_SEND            4
#define ACTION_EVENT_IO               5
#define ACTION_EVENT_AD               6
#define ACTION_TURNON_LEDS            7
#define ACTION_TURNOFF_LEDS           8

#ifndef FALSE
#define FALSE           0
#endif

#ifndef TRUE
#define TRUE            !FALSE
#endif
 
// Define Joystick connection 

#define JOYSTICK_UP     A1
#define JOYSTICK_RIGHT  A2
#define JOYSTICK_DOWN   A3
#define JOYSTICK_CLICK  A4
#define JOYSTICK_LEFT   A5
 
#define CODE_JOYSTICK_UP       1
#define CODE_JOYSTICK_RIGHT    2
#define CODE_JOYSTICK_DOWN     3
#define CODE_JOYSTICK_CLICK    4
#define CODE_JOYSTICK_LEFT     5

// The device URL (max 32 characters including null termination)
// this is the path to a file that describe this unit in a machine
// readable form for use by user software applications.
const uint8_t vscp_deviceURL[] = "www.eurosource.se/arduino01.xml";

// GUID
// All VSCP units should have a glovally unique ID. A base for such an
// if can be obtained by mailing guid@vscp.org. Read more about GUID's in
// the specification section 5. GUID for this unit is set to all seros which
// is an id that can be used for la. usage.
const uint8_t vscp_GUID[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

// The manufacturer id is a code that the manufacturer of the module
// can set and use at there own will. Can for example be a serial 
// number a hardware version and so on.
const uint8_t vscp_manufacturerID[] = {
  1,2,3,4,5,6,7,8
};

///////////////////////////////////////////////////////////////////////////////
// setup
//
// the setup routine runs once when you press reset:
//

void setup() 
{    
#ifdef DEBUG  
  Serial.begin( 9600 );
#endif  
  
  // Set up SPI Communication
  // dataMode can be SPI_MODE0 or SPI_MODE3 only for MCP2515
  SPI.setClockDivider( SPI_CLOCK_DIV2 );
  SPI.setDataMode( SPI_MODE0 );
  SPI.setBitOrder( MSBFIRST );
  SPI.begin();
  
  pinMode( 13, OUTPUT );
  
  // Set directions for joystick pins
  pinMode( JOYSTICK_UP, INPUT );
  pinMode( JOYSTICK_RIGHT, INPUT );
  pinMode( JOYSTICK_DOWN, INPUT );
  pinMode( JOYSTICK_CLICK, INPUT );
  pinMode( JOYSTICK_LEFT, INPUT );
  
  // Enable internal pull-ups 
  digitalWrite( JOYSTICK_UP, HIGH );       
  digitalWrite( JOYSTICK_DOWN, HIGH );
  digitalWrite( JOYSTICK_LEFT, HIGH );
  digitalWrite( JOYSTICK_RIGHT, HIGH );
  digitalWrite( JOYSTICK_CLICK, HIGH );
  
  // initialize the digital pins used ofr LED's as an outputs.
  pinMode(LED_USER, OUTPUT);  
  pinMode(LED_STATUS, OUTPUT);   
 
 /*
  int baudRate = mcp2515.Init( 125, 16 );
#ifdef DEBUG   
  if ( baudRate > 0 ) {
    Serial.println( "MCP2515 Init OK ..." );
    Serial.print( "Baud Rate (kbps): " );
    Serial.println( baudRate, DEC );
  }
  else {
    Serial.println("MCP2515 Init Failed ...");
  }
#endif
*/

  // http://www.kvaser.com/en/support/bit-timing-calculator.html
  // T1=11, T2=5, BTQ=16, SP%=68.75, SJW=3, Bps=125, Err%=0,CNF1=0x83,CNF2=0xa4,CNF3=0x04
  if ( !mcp2515.Init( 0x43, 0xA4, 0x04 ) ) {
    Serial.println("MCP2515 Init Failed ...");
  }
    
  // Check VSCP persistent storage and
  // restore if needed
  if ( !vscp_check_pstorage() ) {

    // Spoiled or not initialized - reinitialize
    init_app_eeprom();
    init_app_ram();   // Needed becuse some ram positions
                      // are initialized from EEPROM
  }

  vscp_init();    // Initialize the VSCP functionality
    
}

// init button check 
// flag becomes true when init button is pressed
bool bInitBtn = false;  
bool bInitBtnClick = false; 
uint32_t initbtnclock = millis();

// Init LED
bool bInitLed = false;  // false for off, true for on
bool bClickEvent = false;
uint32_t initledclock;

// Time keeping
uint32_t local_vscp_timer = micros();
uint32_t secondClock = millis();
uint8_t ioevents = 0;
uint8_t adevents = 0;

// Up button
bool bUpBtn = false;
bool bUpEvent = false;
uint32_t initUpClock;

// Right button
bool bRightBtn = false;
bool bRightEvent = false;
uint32_t initRightClock;

// Down button
bool bDownBtn = false;
bool bDownEvent = false;
uint32_t initDownClock;

// Left button
bool bLeftBtn = false;
bool bLeftEvent = false;
uint32_t initLeftClock;

// Running index used for serial stream events
uint8_t serial_index = 0;

///////////////////////////////////////////////////////////////////////////////
// loop
//
// Main loop
//

void loop() 
{  
  // VSCP timekeeping
  if ( ( micros() - local_vscp_timer ) > 1000 ) {
    local_vscp_timer = micros();
    vscp_timer++;
    vscp_initbtncnt;
    vscp_statuscnt;
  }
  
  // Check if the init button
  if ( ( LOW == digitalRead( JOYSTICK_CLICK ) ) && ( VSCP_STATE_INIT != vscp_node_state ) ) {
    if ( bInitBtn ) {
      // Check if the button has been down for enough 
      // time to (>1 second ) detect this as an init 
      // request
      if ( millis()-initbtnclock > 2000 ) {
        // We have an init request
        bInitBtn = false;
#ifdef DEBUG        
        Serial.println(F("INIT"));
#endif        
        // Init button pressed
        vscp_nickname = VSCP_ADDRESS_FREE;
        eeprom.write( VSCP_EEPROM_NICKNAME, VSCP_ADDRESS_FREE );
        vscp_init();
      }
      else if ( bInitBtnClick && 
                ( millis()-initbtnclock > DEBOUNCE_TIMER ) ) {
        // Just button down - Stay to check for init.
        bInitBtnClick = false;
        bClickEvent = true;
        SendJoystickEvent( CODE_JOYSTICK_CLICK );
#ifdef DEBUG        
        Serial.println(F("CLICK"));
#endif        
      }
    }
    else {
      bInitBtn = true;
      bInitBtnClick = true;
      initbtnclock = millis();
    }
  }
  else {
    bInitBtn = false; 
    bClickEvent = false;
    bInitBtnClick = false; 
  }
  
  
  // Init LED handling
  if ( VSCP_STATE_INIT == vscp_node_state ) {
    // We are in INIT state. The init LED should blink
    if ( bInitLed ) {
      if ( millis()-initledclock > 500 ) {
        digitalWrite( LED_STATUS, LOW );
        bInitLed = false;    
        initledclock = millis();
      }
    }
    else {
      if ( millis()-initledclock > 500 ) {
        digitalWrite( LED_STATUS, HIGH );
        bInitLed = true;    
        initledclock = millis();
      }
    } 
  }
  
  // Check for a valid  event
  vscp_imsg.flags = 0;
  vscp_getEvent();

  switch ( vscp_node_state ) {

    case VSCP_STATE_STARTUP: // Cold/warm reset
       
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

    case VSCP_STATE_INIT: // Assigning nickname
      vscp_handleProbeState();
      break;

    case VSCP_STATE_PREACTIVE:  // Waiting for host initialisation
      vscp_goActiveState();
      // All is well
      digitalWrite( LED_STATUS, HIGH );
      break;

    case VSCP_STATE_ACTIVE:     // The normal state

      // Check for incoming event?
      if ( vscp_imsg.flags & VSCP_VALID_MSG ) {

        if ( VSCP_CLASS1_PROTOCOL == vscp_imsg.vscp_class  ) {
          // Handle protocol event
          vscp_handleProtocolEvent();
          Serial.println(F("Protocol event"));
        }
          
        doDM(); // Handle decision matrix
          
      }
      
      // Check if the up button is down
      if ( ( LOW == digitalRead( JOYSTICK_UP ) ) ) {
        if ( bUpBtn ) {
          if ( millis()-initUpClock > DEBOUNCE_TIMER ) {
            // We have an init request
            bUpBtn = false;
            if ( !bUpEvent ) {
              // Send button up event
              bUpEvent = true;
              SendJoystickEvent( CODE_JOYSTICK_UP );
#ifdef DEBUG              
              Serial.println(F("UP"));
#endif              
            }
          }  
        }
        else {
          bUpBtn = true;
          initUpClock = millis();
        }
      }
      else {
        bUpBtn = false; 
        bUpEvent = false;
      }
        
      // Check if the right button is down
      if ( ( LOW == digitalRead( JOYSTICK_RIGHT ) ) ) {
        if ( bRightBtn ) {
          if ( millis()-initRightClock > DEBOUNCE_TIMER ) {
            // We have an init request
            bRightBtn = false;
            if ( !bRightEvent ) {
              // Send button right event
              bRightEvent = true;
              SendJoystickEvent( CODE_JOYSTICK_RIGHT );
#ifdef DEBUG              
              Serial.println(F("RIGHT"));
#endif              
            }
          }  
        }
        else {
          bRightBtn = true;
          initRightClock = millis();
        }
      }
      else {
        bRightBtn = false; 
        bRightEvent = false;
      }
        
        
      // Check if the down button is down
      if ( ( LOW == digitalRead( JOYSTICK_DOWN ) ) ) {
        if ( bDownBtn ) {
          if ( millis()-initDownClock > DEBOUNCE_TIMER ) {
            // We have an init request
            bDownBtn = false;
            if ( !bDownEvent ) {
              // Send button down event
              bDownEvent = true;
              SendJoystickEvent( CODE_JOYSTICK_DOWN );
#ifdef DEBUG              
              Serial.println(F("DOWN"));
#endif              
            }
          }  
        }
        else {
          bDownBtn = true;
          initDownClock = millis();
        }
      }
      else {
        bDownBtn = false; 
        bDownEvent = false;
      }
        
      // Check if the left button is down
      if ( ( LOW == digitalRead( JOYSTICK_LEFT ) ) ) {
        if ( bLeftBtn ) {
          if ( millis()-initLeftClock > DEBOUNCE_TIMER ) {
            // We have an init request
            bLeftBtn = false;
            if ( !bLeftEvent ) {
              // Send button left event
              bLeftEvent = true;
              SendJoystickEvent( CODE_JOYSTICK_LEFT );
#ifdef DEBUG              
              Serial.println(F("LEFT"));
#endif              
            }
          }  
        }
        else {
          bLeftBtn = true;
          initLeftClock = millis();
        }
      }
      else {
        bLeftBtn = false; 
        bLeftEvent = false;
      }
      
//#ifndef DEBUG  
      // Handle serial stream data
      if ( mcp2515.isFreeTxBuffer() && 
              ( eeprom.read( EEPROM_CTRL_SERIAL ) & 2 )  ) {
        SendSerialStreamEvent();
      }
//#endif      
      
      break;

    case VSCP_STATE_ERROR: // Everything is *very* *very* bad.
      digitalWrite( LED_STATUS, LOW );
      vscp_error();
      break;

    default: // Should not be here...
      vscp_node_state = VSCP_STATE_STARTUP;
      break;
      
  }
  
  // Check if it's time for second work
  if ( ( millis()-secondClock ) > 1000 ) {
          
    secondClock = millis();
    
    // Do VSCP one second jobs
    vscp_doOneSecondWork();  
            
    ioevents++;
    if ( mcp2515.isFreeTxBuffer() && 
          eeprom.read( EEPROM_SECONDS_IO_EVENT ) &&
          ( ioevents >= eeprom.read( EEPROM_SECONDS_IO_EVENT ) ) ) {
      ioevents = 0; 
      doActionEventIO(0,0);      
    }
            
    adevents++;
    if ( mcp2515.isFreeTxBuffer() &&
          eeprom.read( EEPROM_SECONDS_AD_EVENT ) && 
          ( adevents >= eeprom.read( EEPROM_SECONDS_AD_EVENT ) ) ) {
      adevents = 0;
      doActionEventAD(0,0);        
    }
 
#ifdef DEBUG    
//    Serial.println( millis() );
#endif    
            
  }
  
}

///////////////////////////////////////////////////////////////////////////////
// init_app_ram
//

void init_app_ram()
{

}

///////////////////////////////////////////////////////////////////////////////
// init_app_eeprom
//
// Write defaults to eeprom
//

void init_app_eeprom()
{
    int i;
     
    vscp_writeAppReg( REG_ZONE, DEFAULT_ZONE );    
    vscp_writeAppReg( REG_SUBZONE, DEFAULT_SUBZONE );   
    vscp_writeAppReg( REG_CTRL_D3, DEFAULT_CTRL_D3 );    
    vscp_writeAppReg( REG_CTRL_D4, DEFAULT_CTRL_D4 );    
    vscp_writeAppReg( REG_CTRL_D5, DEFAULT_CTRL_D5 );    
    vscp_writeAppReg( REG_CTRL_D6, DEFAULT_CTRL_D6 );    
    vscp_writeAppReg( REG_CTRL_A0, DEFAULT_CTRL_A0 );
    vscp_writeAppReg( REG_CTRL_SERIAL, DEFAULT_CTRL_SERIAL );
    vscp_writeAppReg( REG_BAUD_SERIAL_MSB, DEFAULT_BAUD_SERIAL_MSB );
    vscp_writeAppReg( REG_BAUD_SERIAL_LSB, DEFAULT_BAUD_SERIAL_LSB );
    vscp_writeAppReg( REG_CTRL_JOYSTICK, DEFAULT_CTRL_JOYSTICK );
    vscp_writeAppReg( REG_ZONE_ON, DEFAULT_ZONE_ON );
    vscp_writeAppReg( REG_SUBZONE_ON, DEFAULT_SUBZONE_ON );
    vscp_writeAppReg( REG_ZONE_BUTTON, DEFAULT_ZONE_BUTTON );
    vscp_writeAppReg( REG_SUBZONE_BUTTON, DEFAULT_SUBZONE_BUTTON );
    vscp_writeAppReg( REG_SECONDS_IO_EVENT, DEFAULT_SECONDS_IO );                      
    vscp_writeAppReg( REG_SECONDS_AD_EVENT, DEFAULT_SECONDS_AD );
    
    for ( i=0; i<(SIZE_OF_DM * 8 ); i++ ) {
        eeprom.write(  EEPROM_DM_START + i, 0 );        
    }
}


///////////////////////////////////////////////////////////////////////////////
// SendJoystickEvent
//
// Send a joystick event
//

void SendJoystickEvent( uint8_t index )
{
    uint8_t ctrljoystick = eeprom.read( EEPROM_CTRL_JOYSTICK );
    
    vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
    vscp_omsg.vscp_class = VSCP_CLASS1_INFORMATION;
    if ( ctrljoystick & 4 ) {    
      vscp_omsg.flags = VSCP_VALID_MSG + 4;
      vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_BUTTON;
      vscp_omsg.data[ 0 ] = eeprom.read( EEPROM_ZONE );
      if ( ctrljoystick & 1 ) {
        vscp_omsg.data[ 1 ] = eeprom.read( EEPROM_SUBZONE ) + index;
      }
      else {
        vscp_omsg.data[ 1 ] = eeprom.read( EEPROM_SUBZONE );
      }
      vscp_omsg.data[ 2 ] = 0;
      vscp_omsg.data[ 3 ] = index;
    }
    else {
      vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_ON;
      vscp_omsg.flags = VSCP_VALID_MSG + 3;
      vscp_omsg.data[ 0 ] = index;
      vscp_omsg.data[ 1 ] = eeprom.read( EEPROM_ZONE );
      if ( ctrljoystick & 2 ) {
        vscp_omsg.data[ 2 ] = eeprom.read( EEPROM_SUBZONE ) + index;
      }
      else {
        vscp_omsg.data[ 2 ] = eeprom.read( EEPROM_SUBZONE );
      }

    }
    
    // send the event
    vscp_sendEvent(); 
}

///////////////////////////////////////////////////////////////////////////////
// SendSerialStreamEvent
//
// Send serial stream events for incoming serial data
//

void SendSerialStreamEvent( void )
{
   uint8_t i,length;
   uint8_t cnt = Serial.available(); 
   uint8_t serialCtrl = eeprom.read( EEPROM_CTRL_SERIAL );
    
   // Noting to do if no characters available
   if ( !cnt ) return;
      
   vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
   vscp_omsg.vscp_class = VSCP_CLASS1_INFORMATION;
   if ( serialCtrl & 4 ) {
     length = 3;
     vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_STREAM_DATA_WITH_ZONE;
     vscp_omsg.data[ 0 ] = serial_index++;
     vscp_omsg.data[ 1 ] = eeprom.read( EEPROM_ZONE );
     vscp_omsg.data[ 2 ] = eeprom.read( EEPROM_SUBZONE );
   }
   else {
     length = 1;
     vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_STREAM_DATA;
     vscp_omsg.data[ 0 ] = serial_index++;
   }
   
   while ( ( length < 8 ) &&  Serial.available() ) {
     vscp_omsg.data[ length++ ] = Serial.read();
   }
    
   vscp_omsg.flags = VSCP_VALID_MSG + length;
   
   // send the event
   vscp_sendEvent();
}

///////////////////////////////////////////////////////////////////////////////
// Get GUID from EEPROM
//

#ifdef GUID_IN_EEPROM
extern "C" {
unsigned char getGuidFromEEprom(unsigned char idx) {
    eeprom.read(EEPROM_REG_GUIID0 + idx);
}
}
#endif

///////////////////////////////////////////////////////////////////////////////
// Get Manufacturer id and subid from EEPROM
//

#ifdef MANUFACTURER_INFO_IN_EEPROM
extern "C" {
unsigned char getManufacturerIdFromEEprom(unsigned char idx) {
    eeprom.read(EEPROM_REG_MANUFACTUR_ID0 + idx);
}
}
#endif

///////////////////////////////////////////////////////////////////////////////
// Get the bootloader algorithm code
//

unsigned char getBootLoaderAlgorithm(void)
{
    return VSCP_BOOTLOADER_PIC1;
}


///////////////////////////////////////////////////////////////////////////////
// Get the buffer size
//

unsigned char getBufferSize(void)
{
    return 8; // Standard CAN frame
}


///////////////////////////////////////////////////////////////////////////////
//  vscp_readNicknamePermanent
//

uint8_t vscp_readNicknamePermanent(void)
{
    return eeprom.read( VSCP_EEPROM_NICKNAME );
}


///////////////////////////////////////////////////////////////////////////////
//  vscp_writeNicknamePermanent
//

void vscp_writeNicknamePermanent(uint8_t nickname)
{
    eeprom.write( VSCP_EEPROM_NICKNAME, nickname );
}


///////////////////////////////////////////////////////////////////////////////
// vscp_getZone
//

uint8_t vscp_getZone(void)
{
    return eeprom.read( VSCP_EEPROM_END + EEPROM_ZONE );
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getSubzone
//

uint8_t vscp_getSubzone(void)
{
    return eeprom.read( VSCP_EEPROM_END + EEPROM_SUBZONE );
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


uint8_t vscp_getGUID(uint8_t idx)
{
    return vscp_GUID[ idx ];
}



///////////////////////////////////////////////////////////////////////////////
// getDeviceURL
//
// Get device URL from EEPROM
//


uint8_t vscp_getMDF_URL(uint8_t idx)
{
    return vscp_deviceURL[ idx ];
}


///////////////////////////////////////////////////////////////////////////////
// Get Manufacturer id and subid from EEPROM
//

uint8_t vscp_getUserID(uint8_t idx)
{
    return eeprom.read( VSCP_EEPROM_REG_USERID + idx );
}

///////////////////////////////////////////////////////////////////////////////
//  setVSCPUserID
//

void vscp_setUserID(uint8_t idx, uint8_t data)
{
    eeprom.write( idx + VSCP_EEPROM_REG_USERID, data );
}


///////////////////////////////////////////////////////////////////////////////
// getVSCPManufacturerId
// 
// Get Manufacturer id and subid from EEPROM
//

uint8_t vscp_getManufacturerId(uint8_t idx)
{
    return vscp_manufacturerID[ idx ];
}

///////////////////////////////////////////////////////////////////////////////
// getVSCPManufacturerId
// 
// Get Manufacturer id and subid from EEPROM
//

void vscp_setManufacturerId(uint8_t idx, uint8_t data)
{
    // Not writeable
    //eeprom.write( VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx, data );
}

///////////////////////////////////////////////////////////////////////////////
// Get the bootloader algorithm code
//

uint8_t vscp_getBootLoaderAlgorithm(void)
{
    return 0xff;  // No boot loader
}

///////////////////////////////////////////////////////////////////////////////
// Get the buffer size
//

uint8_t vscp_getBufferSize(void)
{
    return 8; // Standard CAN frame
}


///////////////////////////////////////////////////////////////////////////////
//  getNickname
//

uint8_t vscp_getNickname(void)
{
    return eeprom.read( VSCP_EEPROM_NICKNAME );
}

///////////////////////////////////////////////////////////////////////////////
//  setNickname
//

void vscp_setNickname(uint8_t nickname)
{
    eeprom.write( VSCP_EEPROM_NICKNAME, nickname );
}

///////////////////////////////////////////////////////////////////////////////
//  getSegmentCRC
//

uint8_t vscp_getSegmentCRC(void)
{
    return ( VSCP_EEPROM_SEGMENT_CRC );
}

///////////////////////////////////////////////////////////////////////////////
//  setSegmentCRC
//

void vscp_setSegmentCRC(uint8_t crc)
{
    eeprom.write( VSCP_EEPROM_SEGMENT_CRC, crc );
}

///////////////////////////////////////////////////////////////////////////////
//  setVSCPControlByte
//

void vscp_setControlByte(uint8_t ctrl)
{
    eeprom.write( VSCP_EEPROM_CONTROL, ctrl );
}


///////////////////////////////////////////////////////////////////////////////
//  getVSCPControlByte
//

uint8_t vscp_getControlByte(void)
{
    return eeprom.read( VSCP_EEPROM_CONTROL );
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_getEmbeddedMdfInfo
//

void vscp_getEmbeddedMdfInfo(void)
{
    // No embedded DM so we respond with info about that

    vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
    vscp_omsg.flags = VSCP_VALID_MSG + 3;
    vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
    vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

    vscp_omsg.data[ 0 ] = 0;
    vscp_omsg.data[ 1 ] = 0;
    vscp_omsg.data[ 2 ] = 0;

    // send the event
    vscp_sendEvent();

}

///////////////////////////////////////////////////////////////////////////////
// vscp_goBootloaderMode
//


void vscp_goBootloaderMode(void)
{
  // No bootloadr so we ignore this
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_getMatrixInfo
//

void vscp_getMatrixInfo(char *pData)
{
    uint8_t i;

    pData[ 0 ] = 8;              // Matrix is eight rows
    pData[ 1 ] = REG_DM_START;   // Matrix start offset

    // The resr set to zero no paging is used
    for ( i = 2; i < 8; i++ ) {
        pData[ i ] = 0;
    }
}


///////////////////////////////////////////////////////////////////////////////
// vscp_getRegisterPagesUsed
//

uint8_t vscp_getRegisterPagesUsed( void )
{
    return 1; // One pae used
}

///////////////////////////////////////////////////////////////////////////////
// vscp_writeAppReg
//

uint8_t vscp_readAppReg(uint8_t reg)
{
    uint8_t rv;

#ifdef DEBUG
    Serial.print( "Read app. reg " );
    Serial.println( reg, HEX );
#endif    
    
    rv = 0x00; // default read
    
    if ( REG_ZONE == reg ) {
      rv = eeprom.read( EEPROM_ZONE ); 
    }
    else if ( REG_SUBZONE == reg ) {
      rv = eeprom.read( EEPROM_SUBZONE );
    }
    else if ( REG_CTRL_D3 == reg ) {
      rv = eeprom.read( EEPROM_CTRL_D3 );
      rv &= 0xfe;  // Clear bit 0
      if ( digitalRead( 3 ) ) rv |= 1; 
    }
    else if ( REG_CTRL_D4 == reg ) {
      rv = eeprom.read( EEPROM_CTRL_D4 );
      rv &= 0xfe;  // Clear bit 0
      if ( digitalRead( 4 ) ) rv |= 1; 
    }
    else if ( REG_CTRL_D5 == reg ) {
      rv = eeprom.read( EEPROM_CTRL_D5 );
      rv &= 0xfe;  // Clear bit 0
      if ( digitalRead( 5 ) ) rv |= 1;
    }
    else if ( REG_CTRL_D6 == reg ) {
      rv = eeprom.read( EEPROM_CTRL_D6 );
      rv &= 0xfe;  // Clear bit 0
      if ( digitalRead( 6 ) ) rv |= 1;
    }
    else if ( REG_CTRL_A0 == reg ) {
      rv = eeprom.read( EEPROM_CTRL_A0 );
    }
    else if ( REG_ANALOG_A0_MSB == reg ) {
      rv = ( analogRead( A0 ) >> 8 );
    }
    else if ( REG_ANALOG_A0_LSB == reg ) {
      rv = ( analogRead( A0 ) & 0x0f );
    }
    else if ( REG_CTRL_SERIAL == reg ) {
      rv = eeprom.read( EEPROM_CTRL_SERIAL );
    }
    else if ( REG_BAUD_SERIAL_MSB == reg ) {
      rv = eeprom.read( EEPROM_BAUD_SERIAL_MSB );
    }
    else if ( REG_BAUD_SERIAL_LSB == reg ) {
      rv = eeprom.read( EEPROM_BAUD_SERIAL_LSB  );
    }
    else if ( REG_CTRL_JOYSTICK == reg ) {
      rv = eeprom.read( EEPROM_CTRL_JOYSTICK ); 
    }
    else if ( REG_ZONE_ON == reg ) {
      rv = eeprom.read( EEPROM_ZONE_ON );
    }
    else if ( REG_SUBZONE_ON == reg ) {
      rv = eeprom.read( EEPROM_SUBZONE_ON );
    }
    else if ( REG_ZONE_BUTTON == reg ) {
      rv = eeprom.read( EEPROM_ZONE_BUTTON ); 
    }
    else if ( REG_SUBZONE_BUTTON == reg ) {
      rv = eeprom.read( EEPROM_SUBZONE_BUTTON ); 
    }
    else if ( REG_SECONDS_IO_EVENT == reg ) {
      rv = eeprom.read( EEPROM_SECONDS_IO_EVENT );
    }
    else if ( REG_SECONDS_AD_EVENT == reg ) {
      rv = eeprom.read( EEPROM_SECONDS_AD_EVENT );
    }
    else if ( REG_CTRL_LEDS == reg ) {
      return ( digitalRead( 7 ) | (digitalRead( 8 )<<1) );
    }
    else if ( ( reg >= REG_DM_START ) && ( reg < 128 ) ) {
      rv = eeprom.read( EEPROM_DM_START + ( reg - REG_DM_START ) );
    }

    return rv;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_writeAppReg
//

uint8_t vscp_writeAppReg(uint8_t reg, uint8_t val)
{
    uint8_t rv = ~val;
    
#ifdef DEBUG    
    Serial.print( "Write app. reg " );
    Serial.println( reg, HEX );
#endif    
    
    if ( REG_ZONE == reg ) {
      eeprom.write( EEPROM_ZONE, val );
      return  vscp_readAppReg( reg ); 
    }
    else if ( REG_SUBZONE == reg ) {
      eeprom.write( EEPROM_SUBZONE, val );
      return  vscp_readAppReg( reg );
    }
    else if ( REG_CTRL_D3 == reg ) {
      eeprom.write( EEPROM_CTRL_D3, val );
      // Set pin direction
      ( val & 2 ) ? pinMode( 3,  OUTPUT ) : pinMode( 3, INPUT );
      // If input check if pull ups should be activated
      if (!( val & 2 ) && ( val & 4 )) pinMode( 3, INPUT_PULLUP );      
      ( val & 1 ) ? digitalWrite( 3, HIGH ) : digitalWrite( 3, LOW );
      return  vscp_readAppReg( reg );
    }
    else if ( REG_CTRL_D4 == reg ) {
      eeprom.write( EEPROM_CTRL_D4, val );
      // Set pin direction
      ( val & 2 ) ? pinMode( 4,  OUTPUT ) : pinMode( 4, INPUT );
      // If input check if pull ups should be activated
      if (!( val & 2 ) && ( val & 4 )) pinMode( 4, INPUT_PULLUP ); 
      ( val & 1 ) ? digitalWrite( 4, HIGH ) : digitalWrite( 4, LOW );
      return  vscp_readAppReg( reg ); 
    }
    else if ( REG_CTRL_D5 == reg ) {
      eeprom.write( EEPROM_CTRL_D5, val );
      // Set pin direction
      ( val & 2 ) ? pinMode( 5, OUTPUT ) : pinMode( 5, INPUT );
      // If input check if pull ups should be activated
      if (!( val & 2 ) && ( val & 4 )) pinMode( 5, INPUT_PULLUP ); 
      ( val & 1 ) ? digitalWrite( 5, HIGH ) : digitalWrite( 5, LOW );
      return  vscp_readAppReg( reg );
    }
    else if ( REG_CTRL_D6 == reg ) {
      eeprom.write( EEPROM_CTRL_D6, val );
      // Set pin direction
      ( val & 2 ) ? pinMode( 6,  OUTPUT ) : pinMode( 6, INPUT );
      // If input check if pull ups should be activated
      if (!( val & 2 ) && ( val & 4 )) pinMode( 6, INPUT_PULLUP ); 
      ( val & 1 ) ? digitalWrite( 6, HIGH ) : digitalWrite( 6, LOW );
      return  vscp_readAppReg( reg );
    }
    else if ( REG_CTRL_A0 == reg ) {
      eeprom.write( EEPROM_CTRL_A0, val );
      return  vscp_readAppReg( reg );
    }
    else if ( REG_CTRL_SERIAL == reg ) {
      eeprom.write( EEPROM_CTRL_SERIAL, val );
      return  vscp_readAppReg( reg );
    }
    else if ( REG_BAUD_SERIAL_MSB == reg ) {
      eeprom.write( EEPROM_BAUD_SERIAL_MSB, val );
      return  vscp_readAppReg( reg );
    }
    else if ( REG_BAUD_SERIAL_LSB == reg ) {
      eeprom.write( EEPROM_BAUD_SERIAL_LSB, val );
      // Don't set aerial channel in debug mode
#ifndef DEBUG
        Serial.begin( eeprom.read( EEPROM_BAUD_SERIAL_LSB ) * 256 + 
                      eeprom.read( EEPROM_BAUD_SERIAL_LSB ) );
#endif
      return  vscp_readAppReg( reg );
    }
    else if ( REG_CTRL_JOYSTICK == reg ) {
      eeprom.write( EEPROM_CTRL_JOYSTICK, val );
      return  vscp_readAppReg( reg ); 
    }
    else if ( REG_ZONE_ON == reg ) {
      eeprom.write( EEPROM_ZONE_ON, val );
      return  vscp_readAppReg( reg );
    }
    else if ( REG_SUBZONE_ON == reg ) {
      eeprom.write( EEPROM_SUBZONE_ON, val );
      return  vscp_readAppReg( reg );
    }
    else if ( REG_ZONE_BUTTON == reg ) {
      eeprom.write( EEPROM_ZONE_BUTTON, val );
      return  vscp_readAppReg( reg );
    }
    else if ( REG_SUBZONE_BUTTON == reg ) {
      eeprom.write( EEPROM_SUBZONE_BUTTON, val );
      return  vscp_readAppReg( reg );
    }
    else if ( REG_SECONDS_IO_EVENT == reg ) {
      eeprom.write( EEPROM_SECONDS_IO_EVENT, val );
      return  vscp_readAppReg( reg );
    }
    else if ( REG_SECONDS_AD_EVENT == reg ) {
      eeprom.write( EEPROM_SECONDS_AD_EVENT, val );
      return  vscp_readAppReg( reg );
    }
     else if ( REG_CTRL_LEDS == reg ) {
      
       if ( val & 1 ) {
        // Trun on LED D7
        digitalWrite( 7, HIGH );  
      }
      else {
        // Trun on LED D7
        digitalWrite( 7, LOW );
      }
      
      if ( val & 2 ) {
        // Trun on LED D8 
        digitalWrite( 8, HIGH );  
      }
      else {
        // Trun off LED D8
        digitalWrite( 8, LOW );
      }
      
      return  vscp_readAppReg( reg );
      
    }  
    else if ( ( reg >= REG_DM_START ) && ( reg < 128 ) ) {
      Serial.println( "Writing DM" );
      eeprom.write( EEPROM_DM_START + ( reg - REG_DM_START ), val );
      return  vscp_readAppReg( reg );
    }
    
    Serial.println( "end" );
    return rv;
}

///////////////////////////////////////////////////////////////////////////////
// sendVSCPFrame
//

int8_t sendVSCPFrame( uint16_t vscpclass,
                        uint8_t vscptype,
                        uint8_t nodeid,
                        uint8_t priority,
                        uint8_t size,
                        uint8_t *pData)
{
    uint32_t id = ( (uint32_t)priority << 26 ) |
                      ( (uint32_t)vscpclass << 16 ) |
                      ( (uint32_t)vscptype << 8 ) |
                      nodeid; // nodeaddress (our address)

    if ( !sendCANFrame( id, size, pData ) ) {
        // Failed to send event
        vscp_errorcnt++;
        return FALSE;
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// getVSCPFrame
//

int8_t getVSCPFrame(uint16_t *pvscpclass,
                        uint8_t *pvscptype,
                        uint8_t *pNodeId,
                        uint8_t *pPriority,
                        uint8_t *pSize,
                        uint8_t *pData)
{
    uint32_t id;

    if ( !getCANFrame(&id, pSize, pData) ) {
        return FALSE;
    }

    *pNodeId = id & 0x0ff;
    *pvscptype = (id >> 8) & 0xff;
    *pvscpclass = (id >> 16) & 0x1ff;
    *pPriority = (uint16_t) (0x07 & (id >> 26));
    
#ifdef DEBUG        
     Serial.print("Event received! Class=");
     Serial.print( *pvscpclass, HEX );
     Serial.print(" Type=");
     Serial.print( *pvscptype, HEX );
     if ( *pSize ) Serial.print(" Data=");
     for ( int i=0; i<*pSize; i++ ) {
       Serial.print( pData[i], HEX );
       Serial.print(",");
     }
     Serial.println();
#endif

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// sendCANFrame
//

bool sendCANFrame(uint32_t id, uint8_t dlc, uint8_t *pdata)
{
  CANMSG canmsg;
  
  canmsg.id = id;
  canmsg.xtd = 1;
  canmsg.rtr = 0;
  canmsg.dlc = dlc;
  memcpy( canmsg.data, pdata, dlc );
    
  if ( !mcp2515.SendMessage( canmsg ) ) {
    // Failed to send event
    return false;
  }
   
   vscp_omsg.flags = 0;
   return true;
}

///////////////////////////////////////////////////////////////////////////////
// getCANFrame
//

bool getCANFrame(uint32_t *pid, uint8_t *pdlc, uint8_t *pdata)
{
    CANMSG canmsg;

    // Dont read in new event if there already is a event
    // in the input buffer
    if ( vscp_imsg.flags & VSCP_VALID_MSG ) return false;
    
    if ( mcp2515.GetMessage( canmsg ) ) {
        
        // RTR not interesting
        if ( canmsg.rtr ) return false;

        // Must be extended frame
        if ( !canmsg.xtd ) return false;

        *pid = canmsg.id;
        *pdlc = canmsg.dlc;
        memcpy( pdata, canmsg.data, canmsg.dlc ); 
        
        return true;
    }
    
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Do decision Matrix handling
// 
// The routine expects vscp_imsg to contain a vaild incoming event
//

void doDM( void )
{
    unsigned char i;
    unsigned char dmflags;
    unsigned short class_filter;
    unsigned short class_mask;
    unsigned char type_filter;
    unsigned char type_mask;

    // Don't deal with the control functionality
    if ( VSCP_CLASS1_PROTOCOL == vscp_imsg.vscp_class ) return;

    for (i = 0; i < SIZE_OF_DM; i++) {

        // Get DM flags for this row
        dmflags = eeprom.read( EEPROM_DM_START + 1 + (8 * i) );

        // Is the DM row enabled?
        if ( dmflags & VSCP_DM_FLAG_ENABLED ) {

            // Should the originating id be checked and if so is it the same?
            if ( ( dmflags & VSCP_DM_FLAG_CHECK_OADDR ) &&
                    ( vscp_imsg.oaddr != eeprom.read( EEPROM_DM_START + (8 * i) ) ) ) {
                continue;
            }

            // Check if zone should match and if so if it match
            if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE ) {
                if ( 255 != vscp_imsg.data[ 1 ] ) {
                    if ( vscp_imsg.data[ 1 ] != eeprom.read( EEPROM_ZONE ) ) {
                        continue;
                    }
                }
            }

            class_filter = ( dmflags & VSCP_DM_FLAG_CLASS_FILTER)*256 +
                    eeprom.read( EEPROM_DM_START +
                    (8 * i) +
                    VSCP_DM_POS_CLASSFILTER);
            class_mask = ( dmflags & VSCP_DM_FLAG_CLASS_MASK)*256 +
                    eeprom.read( EEPROM_DM_START +
                    (8 * i) +
                    VSCP_DM_POS_CLASSMASK);
            type_filter = eeprom.read( EEPROM_DM_START +
                    (8 * i) +
                    VSCP_DM_POS_TYPEFILTER);
            type_mask = eeprom.read( EEPROM_DM_START +
                    (8 * i) +
                    VSCP_DM_POS_TYPEMASK);

            if ( !( ( class_filter ^ vscp_imsg.vscp_class ) & class_mask ) &&
                    !( ( type_filter ^ vscp_imsg.vscp_type ) & type_mask ) ) {

                // OK Trigger this action
                switch ( eeprom.read( EEPROM_DM_START + (8 * i) + VSCP_DM_POS_ACTION ) ) {

                    case ACTION_GOOD_DAY: 
                        doActionEventGoodDay( dmflags, 
                                          eeprom.read( EEPROM_DM_START + (8 * i) + VSCP_DM_POS_ACTIONPARAM ) );
                        break;

                    case ACTION_SERIAL_OPEN: 
                        doActionSerialOpen( dmflags, 
                                          eeprom.read( EEPROM_DM_START + (8 * i) + VSCP_DM_POS_ACTIONPARAM ) );
                        break;

                    case ACTION_SERIAL_CLOSE: 
                        doActionSerialClose( dmflags, 
                                          eeprom.read( EEPROM_DM_START + (8 * i) + VSCP_DM_POS_ACTIONPARAM ) );
                        break;

                    case ACTION_SERIAL_SEND: 
                        doActionSerialSend( dmflags, 
                                          eeprom.read( EEPROM_DM_START + (8 * i) + VSCP_DM_POS_ACTIONPARAM ) );
                        break;

                    case ACTION_EVENT_IO: 
                        doActionEventIO( dmflags, 
                                          eeprom.read( EEPROM_DM_START + (8 * i) + VSCP_DM_POS_ACTIONPARAM ) );
                        break;

                    case ACTION_EVENT_AD: 
                        doActionEventAD( dmflags, 
                                          eeprom.read( EEPROM_DM_START + (8 * i) + VSCP_DM_POS_ACTIONPARAM ) );
                        break;
                        
                    case ACTION_TURNON_LEDS:
                      doActionTrunOnLEDs( dmflags, 
                                          eeprom.read( EEPROM_DM_START + (8 * i) + VSCP_DM_POS_ACTIONPARAM ) );
                      break;
                
                    case ACTION_TURNOFF_LEDS:
                      doActionTrunOffLEDs( dmflags, 
                                          eeprom.read( EEPROM_DM_START + (8 * i) + VSCP_DM_POS_ACTIONPARAM ) );
                      break;    

                } // case
            } // Filter/mask
        } // Row enabled
    } // for each row
}

///////////////////////////////////////////////////////////////////////////////
// doActionEventGoodDay
//
// Send CLASS1.INFORMATION(20), Type = 18 (0x12) Good day event
//

void doActionEventGoodDay( uint8_t dmflags, uint8_t param )
{
    vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
    vscp_omsg.flags = VSCP_VALID_MSG + 3;
    vscp_omsg.vscp_class = VSCP_CLASS1_INFORMATION;
    vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_GOOD_DAY;

    vscp_omsg.data[ 0 ] = 0;
    vscp_omsg.data[ 1 ] = eeprom.read( EEPROM_ZONE ); 
    vscp_omsg.data[ 2 ] = eeprom.read( EEPROM_SUBZONE );

    // send the event
    vscp_sendEvent();
}

///////////////////////////////////////////////////////////////////////////////
// doActionSerialOpen
//
// Open serial port
//

void doActionSerialOpen( uint8_t dmflags, uint8_t param )
{
    Serial.begin( eeprom.read( EEPROM_BAUD_SERIAL_MSB )*256 + 
                      eeprom.read( EEPROM_BAUD_SERIAL_LSB ) );  
}


///////////////////////////////////////////////////////////////////////////////
// doActionSerialClose
//
// Close serial port
//

void doActionSerialClose( uint8_t dmflags, uint8_t param )
{
    Serial.end();
}

///////////////////////////////////////////////////////////////////////////////
// doActionSerialSend
//
// Send data on serial port.
//

void doActionSerialSend( uint8_t dmflags, uint8_t param )
{
    Serial.write( vscp_omsg.data + param, 8-param );  
}

///////////////////////////////////////////////////////////////////////////////
// doActionEventIO
//
// Send CLASS1.DATA(15), Type = 1 (0x01) I/O – value event
//

void doActionEventIO( uint8_t dmflags, uint8_t param )
{
    vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
    vscp_omsg.flags = VSCP_VALID_MSG + 2;
    vscp_omsg.vscp_class = VSCP_CLASS1_DATA;
    vscp_omsg.vscp_type = VSCP_TYPE_DATA_IO;

    
    vscp_omsg.data[ 0 ] = 0; // Bit format, default unit, index=0
    vscp_omsg.data[ 1 ] = ( ( HIGH == digitalRead( 6 )?1:0 ) << 3 ) |
                          ( ( HIGH == digitalRead( 5 )?1:0 ) << 2 ) |  
                          ( ( HIGH == digitalRead( 4 )?1:0 ) << 1 ) |
                          (   HIGH == digitalRead( 3 )?1:0 );
    // send the event
    vscp_sendEvent();
}

///////////////////////////////////////////////////////////////////////////////
// doActionEventAD
//
// Send CLASS1.DATA(15), Type = 1 (0x01) I/O – value event
//

void doActionEventAD( uint8_t dmflags, uint8_t param )
{
    vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
    vscp_omsg.flags = VSCP_VALID_MSG + 3;
    vscp_omsg.vscp_class = VSCP_CLASS1_DATA;
    vscp_omsg.vscp_type = VSCP_TYPE_DATA_AD;

    vscp_omsg.data[ 0 ] = 0x60;
    vscp_omsg.data[ 1 ] = analogRead( A0 ) >> 8;
    vscp_omsg.data[ 2 ] = analogRead( A0 ) & 0xff;

    // send the event
    vscp_sendEvent(); 
}

///////////////////////////////////////////////////////////////////////////////
// doActionTrunOnLEDs
//
// Turn ON LED's as of parameter
//

void doActionTrunOnLEDs( uint8_t dmflags, uint8_t param )
{
  if ( param & 1 ) {
    // Trun on LED D7
    digitalWrite( 7, HIGH );  
  }
  else if ( param & 2 ) {
    // Trun on LED D8 
    digitalWrite( 7, HIGH );  
  }  
}

///////////////////////////////////////////////////////////////////////////////
// doActionTrunOffLEDs
//
// Turn OFF LED's as of parameter
//

void doActionTrunOffLEDs( uint8_t dmflags, uint8_t param )
{
  if ( param & 1 ) {
    // Trun off LED D7 
   digitalWrite( 7, LOW ); 
  }
  else if ( param & 2 ) {
    // Trun off LED D8  
    digitalWrite( 8, LOW );
  }      
}

