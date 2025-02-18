/* This is an modification of the demo_vscp_node_can128, credits to Akhe
 * see original header below this file
 * goal is to implement a small module for home automation including:
 * - 8 input (on/off)
 * - 8 output (on/off)
 * - DM (change output, change DM-rows)
 * - each input can be used as button or state based
 * - each input can have own event
 * - bootloader support		see bootloader project
 *
 * hardware supported:
 * olimex AVR-CAN board
 * custom board, AAmini 0.0, 0.1 available as openhardware
 *
 * version 1.0.0
 * 29/08/2016
 *
 * Sven Zwiers
 *
 * more information about VSCP: http://www.vscp.org
 *---------------------------------------------------------------------------
*/

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "methods.h"
#include <stdio.h>
#include <string.h>
#include "AAmini.h"
#include "hardware.h"
#include "vscp_projdefs.h"
#include "vscp_compiler.h"
#include "version.h"
#include "can_at90can128.h"
#include "uart.h"
#include "vscp_firmware.h"
#include "vscp_class.h"
#include "vscp_type.h"
#include "vscp_registers.h"
#include "vscp_actions.c"
#include "../AAboot/bootsupport.c"



#ifndef GUID_IN_EEPROM
// GUID is stored in ROM for this module
//		IMPORTANT!!!
//		if using this GUID in a real environment
//		please set the four MSB to a unique id.
//
// IMPORTANT!!!!!
// The GUID is stored MSB byte first
const uint8_t GUID[ 16 ] = {
	0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00, 0x13
};
#endif
#include "methods.c"

// Device string is stored in ROM for this module (max 32 bytes)
//const uint8_t vscp_deviceURL[]  = "127.0.0.1/mdf/aamini02.xml";
const uint8_t vscp_deviceURL[]  = "mdfhost/mdf/AAMini03.xml";
// add mdfhost to hosts file on PC to link to ip/host name.


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
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x11
};


// Variables
unsigned char btnstate[ 8 ] = {	0,0,0,0,0,0,0,0};    // Switch state
unsigned char captureInput;

// Prototypes
static void initTimer();
//static void init_app_eeprom( void );
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
	io_timer++;
	// Check for init button
	if ( BTN_INIT_PRESSED ) {
		// Active
		vscp_initbtncnt++;
	}
	else {
		vscp_initbtncnt = 0;
	}
	

	// Status LED
/*	vscp_statuscnt++;
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
*/
}

///////////////////////////////////////////////////////////////////////////////
//  initTimer
//

static void initTimer()
{
    // Sets the compare value
	// OCR0A = fclk/(2nf) - 1 = fclk/512000 - 1
	#ifndef FOSC
	#error You must define FOSC in your makefile
	#elif FOSC == 8000
	OCR0A = 32;
	#elif FOSC == 16000
	OCR0A = 61;
	#endif
	// Set Clear on Timer Compare (CTC) mode, CLK/256 prescaler
	TCCR0A = ( 1 << WGM01 ) | ( 0 << WGM00 ) | ( 4 << CS00 );
	// Enable timer0 compare interrupt
	TIMSK0 = ( 1 << OCIE0A );
	
}

///////////////////////////////////////////////////////////////////////////////
// main
//

int main( void )
{
    uint8_t i;
	unsigned char lastoutput, currentoutput; //detection change on output

	ini_hardware();
    
    // Initialize UART
    UCSRA = 0;
    UCSRC = MSK_UART_8BIT;	// 8N1
    Uart_set_baudrate( 9600 );
     
    UCSRB = MSK_UART_ENABLE_TX | MSK_UART_ENABLE_RX;
	
	//read certain eeprom values into ram
	//unsigned char _debounce  = readEEPROM( VSCP_EEPROM_END + REG_INPUT_DEBOUNCE ) ;
	//unsigned char _start	 = readEEPROM( VSCP_EEPROM_END + REG_INPUT_START ) ;
	for ( i=0; i<8; i++ ) 
	{
	btnstate[ i ] = (readEEPROM(VSCP_EEPROM_END + REG_INPUTBEHAVIOUR ) & _BV(i));
	btnstate[i] = !(btnstate[i]>>i);
	}

    // Initialize the 1ms timer
    initTimer();
    sei(); // Enable interrupts


    // Init can
    if ( ERROR_OK != can_Open( CAN_BITRATE_125K, 0, 0, 0 ) ) {
        uart_puts("Failed to open channel!!\n");
    }

	#ifdef PRINT_GENERAL_EVENTS
	#if BOARD == 0
			uart_puts( "OLIMEX AT90CAN128\n" );
	#elif BOARD == 1
		uart_puts( "AAMINI 1.0xx\n" );
	#endif
	#endif
	
	// Check VSCP persistent storage and
	// restore if needed
	if ( !vscp_check_pstorage() ) 
	{
		// Spoiled or not initialized - reinitialize
		#ifdef PRINT_GENERAL_EVENTS
		uart_puts( "init_app_eeprom" );
		#endif
		//init_app_eeprom();
	}
	
	
	// blink leds x times. x= node id, 
	if (readEEPROM ( VSCP_EEPROM_REGISTER + REG_GENERAL_CONFIG) & enable_blink_ID ) //we can disable this
	{
	if (readEEPROM ( VSCP_EEPROM_NICKNAME) !=0xFF) //no blinking if no id
	{
	for (i = readEEPROM ( VSCP_EEPROM_NICKNAME) ; (i > 0) ; i--)
		{
		io_timer = 0;
		outputport =0xFF;	//off
		while (io_timer < 1000) {};
		io_timer = 0;
		outputport = 0x00; //on
		while (io_timer < 1500) {};
		}
	}
	}
	
	// Initialize the VSCP functionality
	vscp_init();

	//initialize output detection
	lastoutput = read_output;

	//read default values for output ports
	outputport = ~readEEPROM(VSCP_EEPROM_REGISTER + REG_OUTPUT_DEFAULT_STATE);

    while ( 1 ) {  // Loop Forever

        /* no button 0 on the aamini
		if ( ( vscp_initbtncnt > 200 ) && ( VSCP_STATE_INIT != vscp_node_state ) ) 
		{
	        // State 0 button pressed
            vscp_nickname = VSCP_ADDRESS_FREE;
            writeEEPROM( VSCP_EEPROM_NICKNAME, VSCP_ADDRESS_FREE );
            vscp_init();
			#ifdef PRINT_CAN_EVENTS
            uart_puts("Node initialization");
			#endif
        }
		*/

        // Check for any valid CAN message
        vscp_imsg.flags = 0;
        vscp_getEvent();

        // do a work if it's time for it
        if ( measurement_clock > 1000 ) 
		{
            measurement_clock = 0;
            // Do VSCP one second jobs 
            vscp_doOneSecondWork();
        }

        switch ( vscp_node_state ) 
		{
		    case VSCP_STATE_STARTUP:        // Cold/warm reset

                // Get nickname from EEPROM
                if ( VSCP_ADDRESS_FREE == vscp_nickname ) 
				{
                    // new on segment need a nickname
					#ifdef PRINT_GENERAL_EVENTS
					uart_puts( "need nickname" );
					#endif
					vscp_node_state = VSCP_STATE_INIT; 	
                }
                else 
				{
                    // been here before - go on
                    vscp_node_state = VSCP_STATE_ACTIVE;
					#ifdef PRINT_GENERAL_EVENTS
					uart_puts( "go active" );
					#endif
					
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
                if ( vscp_imsg.flags & VSCP_VALID_MSG ) 
				{	// incoming message?
					/*#ifdef PRINT_CAN_EVENTS
	                    char buf[30];
	                    uint8_t i;
	                    sprintf(buf, "rx: %03x/%02x/%02x/",
	                    vscp_imsg.vscp_class, vscp_imsg.vscp_type, vscp_imsg.oaddr);
	                    for (i=0; i<(vscp_imsg.flags&0xf); i++) 
						{
	                        char dbuf[5];
	                        sprintf(dbuf, "/%02x", vscp_imsg.data[i]);
	                        strcat(buf, dbuf);
	                    }
	                    uart_puts(buf);
					#endif
                    */
					 if (VSCP_CLASS1_PROTOCOL == vscp_imsg.vscp_class) {
	                     vscp_handleProtocolEvent();
                     }
                     else {
	                     doDM();
						 doFollow();
                     }

                }
                break;

            case VSCP_STATE_ERROR:          // Everything is *very* *very* bad.
				uart_puts("vscp_error");
                vscp_error();
                break;

            default:                        // Should not be here...
				uart_puts("vscp_default");
                vscp_node_state = VSCP_STATE_STARTUP;
                break;

        } // switch 


		// check i/o every 100ms
		// TODO: on startup check actual input & set btnstate accordingly?
		if ( io_timer > 100 )
		{
			io_timer = 0;
			captureInput = (read_input); //read input into memory

			for ( i=0; i<8; i++ ) 
			{
				// input low
				if (CheckInput (captureInput,i))
				{
					if (btnstate[i] >= inputButtonActive)
					{
						btnstate[i] +=1;
					}
					switch (btnstate[i])
						{
						case inputStateStart:
							//on startup sent out current state + set state according
						case inputStateLow:
							// 1 event should be sent when a input changes
							// here we check state (= input pulled down)
							#ifdef PRINT_IO_EVENTS
							uart_puts("s_l");
							#endif
							SendInformationEventExtended(VSCP_PRIORITY_MEDIUM,
							readEEPROM( VSCP_EEPROM_REGISTER + REG_ZONE ),
							readEEPROM( VSCP_EEPROM_REGISTER + REG_SWITCH1_SUBZONE + i),
							0x00, readEEPROM( VSCP_EEPROM_REGISTER + REG_SW1_SHORT_CLASS + (5*i)), readEEPROM( VSCP_EEPROM_REGISTER + REG_SW1_SHORT_TYPE + (5*i)));
							btnstate[ i ] = inputStateHigh;	
							break;
						case inputButtonStart:
							//button is pushed (input pulled down)
							btnstate[i] = inputButtonActive;
							#ifdef PRINT_IO_EVENTS
							uart_puts("b_st");
							#endif
							break;
						case inputButtonActive+15:
							//button is pushed for a long time
							#ifdef PRINT_IO_EVENTS
							uart_puts("b_long");
							#endif
							SendInformationEventExtended(VSCP_PRIORITY_MEDIUM,
							readEEPROM( VSCP_EEPROM_REGISTER + REG_ZONE ),
							readEEPROM( VSCP_EEPROM_REGISTER + REG_SWITCH1_SUBZONE + i),
							0x00, readEEPROM( VSCP_EEPROM_REGISTER + REG_SW1_LONG_CLASS + (5*i)), readEEPROM( VSCP_EEPROM_REGISTER + REG_SW1_LONG_TYPE + (5*i)));
							btnstate[i] =inputButtonActiveLongSent;
							break;
						}
				}
			
				//input high
				else
				{
				if ((btnstate[i] >= inputButtonActive)& (btnstate[i] < inputButtonActive+7))  //short push?
				{
						#ifdef PRINT_IO_EVENTS
						uart_puts("b_short");
						#endif
						SendInformationEventExtended(VSCP_PRIORITY_MEDIUM,
						readEEPROM( VSCP_EEPROM_REGISTER + REG_ZONE ),
						readEEPROM( VSCP_EEPROM_REGISTER + REG_SWITCH1_SUBZONE + i),
						0x00, readEEPROM( VSCP_EEPROM_REGISTER + REG_SW1_SHORT_CLASS + (5*i)), readEEPROM( VSCP_EEPROM_REGISTER + REG_SW1_SHORT_TYPE + (5*i)));
					btnstate[i] =inputButtonStart;
				}
				if (btnstate[i] >= inputButtonActive+7)  // to long for short push, not yet long push
				{
					#ifdef PRINT_IO_EVENTS
					uart_puts("b_mid");
					#endif
					//send NO events
					btnstate[i] =inputButtonStart;
				}	
				switch (btnstate[i])
					{
					case inputStateStart:
					//on startup sent out current state + set state according
					case inputStateHigh:	
					// 1 event should be sent when a input changes
					// here we check state (=input high)
						#ifdef PRINT_IO_EVENTS
						uart_puts("s_h");
						#endif
						SendInformationEventExtended(VSCP_PRIORITY_MEDIUM,
						readEEPROM( VSCP_EEPROM_REGISTER + REG_ZONE ),
						readEEPROM( VSCP_EEPROM_REGISTER + REG_SWITCH1_SUBZONE + i),
						0x00, readEEPROM( VSCP_EEPROM_REGISTER + REG_SW1_LONG_CLASS + (5*i)), readEEPROM( VSCP_EEPROM_REGISTER + REG_SW1_LONG_TYPE + (5*i)));
						btnstate[ i ] = inputStateLow;
						break;
					case inputButtonActiveLongSent:
						#ifdef PRINT_IO_EVENTS
						uart_puts("b_long_rel");
						#endif
						SendInformationEventExtended(VSCP_PRIORITY_MEDIUM,
							readEEPROM( VSCP_EEPROM_REGISTER + REG_ZONE ),
							readEEPROM( VSCP_EEPROM_REGISTER + REG_SWITCH1_SUBZONE + i),
							0x00, readEEPROM( VSCP_EEPROM_REGISTER + REG_SW1_LONG_CLASS + (5*i)), readEEPROM( VSCP_EEPROM_REGISTER + REG_SW1_LONG_TYPE_RELEASE + (5*i)));
						btnstate[i] = inputButtonStart;
						break;
						
					}
				}
			} //end input check

			//check outputs and if change detected > send out event
			currentoutput = read_output;
			if (currentoutput != lastoutput) vscp_outputevent(currentoutput,lastoutput);
			lastoutput = currentoutput;	    
		} //end io_timer
        doWork();

    } // while
}

///////////////////////////////////////////////////////////////////////////////
// init_app_eeprom
// should  only be performed after first startup!
// all registers cleared!


void vscp_init_pstorage( void )
{
	#ifdef PRINT_GENERAL_EVENTS
	uart_puts( "cold start" );
	#endif
	// clear all eeprom registers to 0x00
    uint8_t pos;
    for (pos = VSCP_EEPROM_REGISTER ; pos <= (VSCP_EEPROM_REGISTER + PAGE1_END) ; pos++)
	{
	    writeEEPROM(pos, 0x00 );
	}
	
	//set default values
	//writeEEPROM(VSCP_EEPROM_REGISTER + REG_INPUT_DEBOUNCE, time_debounce);
	//writeEEPROM(VSCP_EEPROM_REGISTER + REG_INPUT_START, time_start);
	for (pos=0; pos <=7 ;pos++)
		{
			writeEEPROM(VSCP_EEPROM_REGISTER + REG_SW1_SHORT_CLASS+(pos*5) ,VSCP_CLASS1_INFORMATION);
			writeEEPROM(VSCP_EEPROM_REGISTER + REG_SW1_SHORT_TYPE+(pos*5) ,VSCP_TYPE_INFORMATION_BUTTON);
			//only button values written
			//writeEEPROM(VSCP_EEPROM_REGISTER + REG_SW1_LONG_CLASS+(pos*4) ,VSCP_CLASS1_INFORMATION);
			//writeEEPROM(VSCP_EEPROM_REGISTER + REG_SW1_LONG_TYPE+(pos*4) ,VSCP_TYPE_INFORMATION_LONG_CLICK);
		}
	#ifdef PRINT_GENERAL_EVENTS
	uart_puts( "default values written" );
	#endif
		
}

///////////////////////////////////////////////////////////////////////////////
//   						VSCP Functions
//							--------------
//
// All methods below must be implemented to handle VSCP functionality
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// common routines moved to avr\common\methods.c
// only routines that are hardware dependant are left here
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
//                        VSCP Required Methods
//////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// vscp_readAppReg
//

uint8_t vscp_readAppReg( uint8_t reg )
{
    
	uint8_t rv;	
	//#ifdef PRINT_VSCP_EVENTS
	//char buf[30];
	//uart_puts( "vscp_rdAppReg\n" );
	//sprintf(buf,"page: %02x",vscp_page_select);
	//uart_puts(buf);
	//#endif
	// Assign the requested page, this variable is used in the implementation
	// specific function 'vscp_readAppReg()' and 'vscp_writeAppReg()' to actually
	// switch pages there
	//vscp_page_select = ((vscp_imsg.data[1] << 8) | vscp_imsg.data[2]);
	switch (vscp_page_select)
	{
		case 1:
			#ifdef PRINT_VSCP_EVENTS
			uart_puts( "page1\n" );
			#endif
			if ((reg >= REG_ZONE-PAGE1_START) & (reg <= REG_ZONE_END-PAGE1_START))
			{
				rv =  readEEPROM( VSCP_EEPROM_REGISTER + reg + PAGE1_START);
			}
	
			else 
			{
				rv = 0xAB;
			}
			
		   // Read actual output state
		   if ( REG_OUTPUT_STATE-PAGE1_START == reg ) rv =  ~read_output;
			return rv;
			break;
			
		
		case 0:
			// page 0 contains DM
			#ifdef PRINT_VSCP_EVENTS
			uart_puts( "page0\n" );
			#endif
			// DM register space
			if ( ( reg >= REG_DM_START ) & ( reg <  REG_DM_START + (DESCION_MATRIX_ELEMENTS * 8) ) )
			{
				rv =  readEEPROM( VSCP_EEPROM_REGISTER +  reg  );
			}
			else
			{
				rv = 0xAA;
			}
			return rv;
			break;
		default:
			return 0xAA;		
	} //end switch page select

}

///////////////////////////////////////////////////////////////////////////////
// vscp_writeAppReg
//

uint8_t vscp_writeAppReg( uint8_t reg, uint8_t val )
{
    uint8_t rv;

    rv = ~val; // error return
	switch (vscp_page_select)
	{
		case 0:
		// DM register space
		#ifdef PRINT_VSCP_EVENTS
			uart_puts( "WRpage0\n" );
			#endif
		if ( ( reg >= REG_DM_START ) & ( reg <= REG_DM_START + (DESCION_MATRIX_ELEMENTS * 8) ) )
		{
			writeEEPROM(VSCP_EEPROM_REGISTER + reg, val);
			rv =  readEEPROM( VSCP_EEPROM_REGISTER +  reg  );
		}

		return rv;
		break;
		
		case 1:
		// page 1
			#ifdef PRINT_VSCP_EVENTS
			uart_puts( "WRpage1\n" );
			#endif
   		// write actual output state
   		if ( REG_OUTPUT_STATE-PAGE1_START == reg )
   		{
	   		outputport = ~val;
	   		__asm__ __volatile__ ("nop");
	   		rv =  ~read_output;
   		}

		else if ((reg >= REG_ZONE-PAGE1_START) & (reg <= REG_ZONE_END-PAGE1_START))
		{
			#ifdef PRINT_VSCP_EVENTS
			uart_puts( "WRp1R\n" );
			#endif
			writeEEPROM(VSCP_EEPROM_REGISTER + reg + PAGE1_START, val); 
			rv =  readEEPROM( reg + VSCP_EEPROM_REGISTER + PAGE1_START);
		}
		
		else
		{
			 rv=0xAC;
		}
		
		// set input behaviour
		if ( REG_INPUTBEHAVIOUR-PAGE1_START == reg )
		{
			uint8_t i;
			for ( i=0; i<8; i++ )
			{
				btnstate[ i ] = (readEEPROM(VSCP_EEPROM_END + REG_INPUTBEHAVIOUR ) & _BV(i));
				btnstate[i] = !(btnstate[i]>>i);
			}
			
		}
		
		break;
		
		default:
			rv=0xAA;
	}//switch
    	
return rv;
}



///////////////////////////////////////////////////////////////////////////////
//                       Implemention of Decision Matrix
//////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Do decision Matrix handling
//
// The routine expects vscp_imsg to contain a valid incoming event
//

static void doDM( void )
{
	#ifdef PRINT_DM_EVENTS
	uart_puts( "debug  doDM\n" );
    #endif

	uint8_t i;
    uint8_t dmflags;
    uint16_t class_filter;
    uint16_t class_mask;
    uint8_t type_filter;
    uint8_t type_mask;

    // Don't deal with the control functionality
    if ( VSCP_CLASS1_PROTOCOL == vscp_imsg.vscp_class ) return;

	#ifdef PRINT_DM_EVENTS
	uart_puts( "debug  doDM2\n" );
    #endif
    for ( i=0; i<DESCION_MATRIX_ELEMENTS; i++ ) {
        // Get DM flags for this row
        dmflags = readEEPROM( VSCP_EEPROM_END + REG_DM_START + 
                                    1 + ( VSCP_SIZE_STD_DM_ROW * i ) );
		#ifdef PRINT_DM_EVENTS
		uart_puts( "debug  doDM check rows\n" );
    	#endif
        // Is the DM row enabled?
        if ( dmflags & VSCP_DM_FLAG_ENABLED ) {
			#ifdef PRINT_DM_EVENTS
			uart_puts( "debug  doDM row enabled\n" );
    		#endif
            // Should the originating id be checked and if so is it the same?
            if ( ( dmflags & VSCP_DM_FLAG_CHECK_OADDR ) &&
                    !(  vscp_imsg.oaddr == 
                        readEEPROM( VSCP_EEPROM_END + REG_DM_START + 
                                        ( VSCP_SIZE_STD_DM_ROW * i ) ) ) ) {
                continue;					
            }	
			
			#ifdef PRINT_DM_EVENTS
			uart_puts( "debug  doDM check zone\n" );
    		#endif
            // Check if zone should match and if so if it match
            if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE  ) {
                if ( vscp_imsg.data[ 1 ] != readEEPROM( VSCP_EEPROM_END + REG_ZONE  ) ) {
					#ifdef PRINT_DM_EVENTS
					uart_puts( "zone mismatch\n" );
                    #endif
            		continue;
                }	
            }				
			#ifdef PRINT_DM_EVENTS
			uart_puts( "zone match\n" );
			#endif
			// Check if subzone should match and if so if it match
            /* checksubzone done in actions.c
			if ( dmflags & VSCP_DM_FLAG_CHECK_SUBZONE  ) {
                if (readEEPROM( VSCP_EEPROM_REGISTER + REG_SUBZONE  ) == 0x00) 
				{
				uart_puts( "subzone 0x00\n" );
				}
				else if ( vscp_imsg.data[ 2 ] != readEEPROM( VSCP_EEPROM_REGISTER + REG_SUBZONE  ) ) {
					uart_puts( "subzone mismatch\n" );
                    continue;
                }	
            }				
			#ifdef PRINT_DM_EVENTS
			uart_puts( "subzone match\n" );
			#endif
*/ 
			
		
            class_filter = readEEPROM( VSCP_EEPROM_END + REG_DM_START + ( VSCP_SIZE_STD_DM_ROW * i ) + VSCP_DM_POS_CLASSFILTER  );
            class_mask = ( ( dmflags & VSCP_DM_FLAG_CLASS_MASK ) << 8 ) + readEEPROM( VSCP_EEPROM_END + REG_DM_START + ( VSCP_SIZE_STD_DM_ROW * i ) + VSCP_DM_POS_CLASSMASK  );
            type_filter = readEEPROM( VSCP_EEPROM_END + REG_DM_START + ( VSCP_SIZE_STD_DM_ROW * i ) + VSCP_DM_POS_TYPEFILTER );
            type_mask = readEEPROM( VSCP_EEPROM_END + REG_DM_START + ( VSCP_SIZE_STD_DM_ROW * i ) + VSCP_DM_POS_TYPEMASK  );


            if ( !( ( class_filter ^ vscp_imsg.vscp_class ) & class_mask ) && !( ( type_filter ^ vscp_imsg.vscp_type ) & type_mask )) {
			
			#ifdef PRINT_DM_EVENTS
			uart_puts( "debug  doDMtrigger\n" );
			#endif

            // OK Trigger this action
            switch ( readEEPROM( VSCP_EEPROM_END + REG_DM_START + ( 8 * i ) +
				 VSCP_DM_POS_ACTION  ) ) {
				// actions can be found in vscp_actions
                case ACTION_OUTP_TOGGLE:			// Toggle relays
                        doActionToggleOut( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DM_START + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );
                        break;
				
				case ACTION_OUTP_ON:			// Turn on relays
                        doActionOnOut( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DM_START + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );
                        break;

				case ACTION_OUTP_OFF:			// Turn off relays
                        doActionOffOut( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DM_START + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );
                        break;
						
				case ACTION_DM_TOGGLE:			// Toggle DM row
						doActionToggleDM( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DM_START + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );
						break;
				case ACTION_DM_ON:			// DM row ON
						doActionOnDM( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DM_START + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );
						break;
				case ACTION_DM_OFF:			// DM row OFF
						doActionOffDM( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DM_START + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );
						break;

				} // case	
            } 
			#ifdef PRINT_DM_EVENTS
			else uart_puts( "// Filter/mask");
			#endif
        } // Row enabled
    } // for each row	
}


uint32_t vscp_getFamilyCode(void)
{
	return 0;
}

uint32_t vscp_getFamilyType(void)
{
	return 0;
}

void vscp_restoreDefaults(void)
{
	vscp_init_pstorage();
	//reboot using WD
	cli();
	wdt_enable(WDTO_1S);
	while(1);
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
    vscp_omsg.data[ 1 ] = readEEPROM( VSCP_EEPROM_REGISTER + REG_ZONE + idx );
    vscp_omsg.data[ 2 ] = readEEPROM( VSCP_EEPROM_REGISTER + REG_SUBZONE + idx );

    vscp_sendEvent();	// Send data
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       end of VSCP Required Methods
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// SendInformationEventExtended same function as SendInformationEvent but 
// with more options to have more control over the events sent
// SendInformationEvent is not altered to remain compatible with common routines

void SendInformationEventExtended(uint8_t priority, uint8_t zone, uint8_t subzone, uint8_t idx, uint8_t eventClass, uint8_t eventTypeId )
{
    if ((eventClass != 0)  & (eventTypeId !=0)  )
	{
		
		vscp_omsg.priority = priority;
		vscp_omsg.flags = VSCP_VALID_MSG + 3;
		vscp_omsg.vscp_class = eventClass;
		vscp_omsg.vscp_type = eventTypeId;

		vscp_omsg.data[ 0 ] = idx;	// dummy Register
		vscp_omsg.data[ 1 ] = zone;
		vscp_omsg.data[ 2 ] = subzone;

		vscp_sendEvent();	// Send data
	}
	#ifdef PRINT_VSCP_EVENTS
	else uart_puts( "FAULTY EVENT BLOCKED");
	#endif
	
}

///////////////////////////////////////////////////////////////////////////////

// doWork
//
// Do work here
//


void doWork( void )
{

}





//debug routines

void sendCharHex (char data)
{
	// sends out a byte in its HEX-notation
	unsigned char dataC = 0;
	dataC = data; //backup
	
	data = (data >> 4);
	data = (data & 0x0F);
	if ((data >= 0) && (data <= 9)) sendChar(data+0x30);
	else sendChar(data+0x37);

	data = (dataC & 0x0F); //start from the backup
	if ((data >= 0) && (data <= 9)) sendChar(data+0x30);
	else sendChar(data+0x37);
	
	sendChar (' ');

	
}

void sendChar (char data)
{	
	int i = 0;
	// to send data with the usart put the data in the usart data register
	UDR0 = data;
	
	// check to see if the global interrupts are enabled
	if (SREG & 0x80) 
	{
		// wait until the byte is sent or we count out
			while ( !(UCSR0A&0x40) && (i<10000) )
			{
				asm ("nop");
				i++;
			}
	}
	else
		// wait until the byte is sent
		while ( !(UCSR0A&0x40) );
		
		// clear the TXCflag
		UCSR0A=(UCSR0A|0x40);
}

// original header
/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol)
 * 	http://www.vscp.org
 *
 * 	2006-04-21
 * 	akhe@eurosource.se
 *
 *  Copyright (c) 2006-2025 Ake Hedmanm Grodans Paradis AB
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

