/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  RFID Module
 * 	Version: See project header
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 1995-2011 Ake Hedman, eurosource                  
 *                2006-2010 Gediminas Simanskis, edevices
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
 *******************************************************************************
*/

#include <p18cxxx.h>
#include <timers.h>
#include "vscp_compiler.h"
#include "vscp_projdefs.h"
#include "../../../common/inttypes.h"
#include "../../common/eeprom.h"
#include "../../common/can18xx8.h"
#include "../../common/can18f.h"
#include "../../../common/vscp.h"
#include "c:/development/m2m/src/vscp/common/vscp_class.h"	// Set "i" with setenv_vscp.bat in VSCP root
#include "c:/development/m2m/src/vscp/common/vscp_type.h"	// Set "i" with setenv_vscp.bat in VSCP root
#include "rfid.h"
#include "main.h" 
#include "pwm.h"
#include "portb.h"
#include "adc.h"
#include "delays.h"
#include "version.h"


// Prototypes
void WriteData( unsigned char Index,unsigned char Value );
unsigned char ReadData( unsigned char Index );
void InitFSKData( void );
void KeyOn( unsigned int time );
void KeyOff( void );
void RedLedOn( unsigned int time );
void RedLedOff( void );
void GreenLedOn( unsigned int time );
void GreenLedOff( void );
void BeeperOn( unsigned int time );



//#pragma config PBADEN = OFF
#pragma config WDTPS = 128     // 512 ms
#pragma config WDT = ON
#pragma config OSC = HSPLL
#pragma config LVP = OFF
#pragma config CPB = OFF

#pragma config PWRT = ON
#pragma config STVREN = ON     // stack reset
#pragma config BOREN = BOHW    // BOHW
#pragma config BORV = 1        // 4.2V
#pragma config MCLRE = ON


//#define RELOCATE

void PWM_Calibr(unsigned char *pwm_f,unsigned char *pwm_adc);

// ISR Routines
void isr_low( void );
void isr_high( void );

// ASM functions
extern void RotateLeftEmData(void);
extern void RotateRightEmData(void);

// The device URL (max 32 characters including null termination)
const rom char vscp_deviceURL[] = "www.edevices.lt/rfid2000.xml";

/*******************************/

volatile uint32_t measurement_clock; // application 1 ms timer
         
unsigned char PWM_f;      // PWM frequancy after autocalibration
unsigned char PWM_adc;    // Detected signal amplitude value after calibration
enum _PWM_enable {ON=0,OFF} PWM_enable;

unsigned char temp;
unsigned int  fsk_tmr_value = 0;
unsigned char RisingPosition = 0;
unsigned char HeaderCounter = 0;

unsigned char Data = 0;
unsigned char DataWriteIndex = 0;
unsigned char DataReadIndex = 0;
unsigned char DataBitCounter = 0;
unsigned char ParityCounter = 0;

unsigned char EmData[6];        // TAG card serial number
unsigned char EmDataIndex = 0; 
volatile unsigned int  EmDataTimer = 0;

unsigned int  RedLedTimer = 0;
unsigned int  GreenLedTimer = 0;
unsigned int  BeeperTimer = 0;
unsigned int  KeyTimer = 0;

struct _EmDataFlags{
   unsigned char EmDataTrue:1;         // CRC ok
   unsigned char EmDataReceived:1;     // FSK received but col CRC not checked yet 
}EmDataFlags;

typedef enum RXSTATE{HEADER,DATA} _RxState;
_RxState RxState = HEADER;




/**************************************************************/




///////////////////////////////////////////////////////////////////////////////
// Isr() 	- Interrupt Service Routine
//      	- Services
//      	- Services
//////////////////////////////////////////////////////////////////////////////

#ifdef RELOCATE
#pragma code low_vector = 0x218
#else
#pragma code low_vector = 0x18
#endif

//#pragma code low_vector = 0x18
void interrupt_at_low_vector( void ) {
	_asm GOTO isr_low _endasm 
}
#pragma code

#ifdef RELOCATE
#pragma code high_vector = 0x208
#else
#pragma code high_vector = 0x08
#endif

//#pragma code high_vector = 0x08
void interrupt_at_high_vector( void ) {
	_asm GOTO isr_high _endasm 
}
#pragma code

#pragma interruptlow isr_high 
void isr_high( void )
{	         
    fsk_tmr_value = ReadTimer0();
    WriteTimer0(0);  

    if ( ( fsk_tmr_value >=  BIT_0_75 ) && ( fsk_tmr_value < BIT_1_25 ) ) {
       if ( ( RisingPosition & 1) == MIDDLE ) {
          WriteData ( ( ( DataWriteIndex++ ) & 0x07 ), 0 );
       }           
       else {
          WriteData( ( (DataWriteIndex++ ) & 0x07 ), 1 );
       }   
    }
    else if( ( fsk_tmr_value >=  BIT_1_25 ) && ( fsk_tmr_value < BIT_1_75 ) ) {     
       if ( ( RisingPosition & 1 ) == MIDDLE ) {
           WriteData( ( ( DataWriteIndex++ ) & 0x07 ), 0 );
           WriteData( ( ( DataWriteIndex++ ) & 0x07 ), 1 );                   
       }
       else {   
           WriteData( ( DataWriteIndex++ ) & 0x07 , 1 );
       }    

       RisingPosition++;  // change
    }
    else if ( ( fsk_tmr_value >=  BIT_1_75 ) && ( fsk_tmr_value < BIT_2_25 ) ){
       if ( ( RisingPosition & 1) == MIDDLE ) {
         WriteData( ( ( DataWriteIndex++ ) & 0x07 ), 0 );
         WriteData( ( ( DataWriteIndex++ ) & 0x07 ), 1 );        
       }
      else {
        InitFSKData();
      }           
   
       RisingPosition = MIDDLE;
       
    }
    else {
       InitFSKData();
    }   

/************* Analyze ****************/
         
     switch ( RxState )
     {
        case HEADER:
        
            while ( DataReadIndex != DataWriteIndex ) {
                temp = ReadData( ( ( DataReadIndex++ ) & 0x07 ) );

                if ( temp ) {
                    HeaderCounter++;
                }              
                else {          
                    HeaderCounter = 0;          
                }    
 
                if ( HeaderCounter >= 9 ) {           
                    ParityCounter = 0;
                    HeaderCounter = 0;        
                    DataBitCounter = 0;
                    EmDataIndex  = 0;
                    RxState = DATA;           
                    break;
                }
            } // while
            break;
            
        case DATA:

            while ( DataReadIndex != DataWriteIndex ) {
                temp = ReadData( ( ( DataReadIndex++ ) & 0x07 ) );   

                switch ( DataBitCounter ) {                    
            
                    case 4:
                        if ( (temp & 1 ) != ( ParityCounter & 1 ) ) InitFSKData();              
                        ParityCounter = 0;
                        break;
                    
                    case 9:
                        if ( ( temp & 1 ) != ( ParityCounter & 1 ) ) InitFSKData();              
                        ParityCounter = 0;
                        break;
                    
                    case 14:
                        if ( ( temp & 1 ) != ( ParityCounter & 1 ) ) InitFSKData();              
                        ParityCounter = 0;
                        break;
                        
                    case 19:
                        if ( ( temp & 1 ) != ( ParityCounter & 1 ) ) InitFSKData();              
                        ParityCounter = 0;
                        break;
                        
                    case 24:
                        if ( ( temp & 1 ) != ( ParityCounter & 1 ) ) InitFSKData();              
                        ParityCounter = 0;
                        break;
                          
                    case 29:
                        if ( ( temp & 1 ) != ( ParityCounter & 1 ) ) InitFSKData();              
                        ParityCounter = 0;
                        break;
            
                    case 34:
                        if ( ( temp & 1 ) != ( ParityCounter & 1 ) ) InitFSKData();              
                        ParityCounter = 0;
                        break;
            
                    case 39:
                        if ( ( temp & 1 ) != ( ParityCounter & 1 ) ) InitFSKData();              
                        ParityCounter = 0;
                        break;
            
                    case 44:
                        if ( ( temp & 1 ) != ( ParityCounter & 1 ) ) InitFSKData();              
                        ParityCounter = 0;
                        break;
            
                    case 49:
                        if ( ( temp & 1 ) != ( ParityCounter & 1 ) ) InitFSKData();              
                        ParityCounter = 0;
                        break;
                         
                    case 54:
                        if ( ( temp & 1 ) == 0) {
                            RotateLeftEmData(); 
                            RotateLeftEmData(); 
                            RotateLeftEmData(); 
                            RotateLeftEmData(); 
                            EmDataFlags.EmDataReceived = 1;
                        }
                        InitFSKData();
                        break;        
            
                    default:               

                        if ( ( temp & 1 ) ) ParityCounter++; 
                    
                        RotateLeftEmData();  

                        if ( ( temp & 1 ) == 1 ) {                  
                            EmData[5] |= 0x01;    
                        }                                                 
                        else {
                            EmData[5] &= 0xFE;
                        }
                        
                        break;    
                                                                      
                } // switch databit counter
                  
                DataBitCounter++; 
                                     
            } // case DATA
            break;
        }

        INTCONbits.INT0IF = 0;
        return;
}

/***********************************************************************/
 
#pragma interruptlow isr_low 
void isr_low( void )
{
    unsigned char temp;
	unsigned short tval0, tval1;
	char *p;	


	// Clock
	if ( PIR1bits.TMR1IF ) {    // If a Timer1 Interrupt, Then... 
 	
        WriteTimer1( 0xD8F0 );  // 1 mS		

		vscp_timer++;

/************** Handle Init button **************/

		if ( PORTCbits.RC1 ) {
			vscp_initbtncnt = 0;
		}
		else {
			// Active
			vscp_initbtncnt++;
		}

/************** VSCP LED *****************/

		vscp_statuscnt++;
		if ( ( VSCP_LED_BLINK1 == vscp_initledfunc ) && ( vscp_statuscnt > 100 ) ) {

 	    	if ( PORTAbits.RA2 ) {
		       PORTAbits.RA2 = 0;
       	    }
		    else {
               PORTAbits.RA2 = 1;
	     	}	

			vscp_statuscnt = 0;
		}
		else if ( VSCP_LED_ON == vscp_initledfunc ) {
			PORTAbits.RA2 = 1;	
			vscp_statuscnt = 0;	
		}
		else if ( VSCP_LED_OFF == vscp_initledfunc ) {
			PORTAbits.RA2 = 0;
			vscp_statuscnt = 0;
		}

/*****************  Key    *********************/

        if( KeyTimer ) {
            KeyTimer--;
        }   
         else {
            KeyOff();
        }   

/***************** Red LED *********************/

        if( RedLedTimer ) {
            RedLedTimer--;
        }   
        else {
            RedLedOff();
        }   

/***************** Green LED *******************/

        if ( GreenLedTimer ) {
            GreenLedTimer--;
        }   
        else {
            GreenLedOff();
        }   

/***************** Beeper **********************/

        if ( BeeperTimer ) {
            BeeperTimer--;
        }  
        else {
            PORTCbits.RC0 = 0;
            if ( PWM_enable == OFF ) {
                OpenPWM1( PWM_f );
                SetDCPWM1( ( PWM_f * 2 ) - 6 );
                PWM_enable = ON;
            }
        }       

/**************** EmData Timer ****************/

        if ( EmDataTimer ){
            EmDataTimer--;
        }

		PIR1bits.TMR1IF = 0;     // Clear Timer1 Interrupt Flag
		
	}

	return;
}

///////////////////////////////////////////////////////////////////////////////
// Init - Initialization Routine
//  

void init(void)
{
unsigned char msgdata[ 8 ];

     vscp18f_init( TRUE );

	// Initialize the uP

	// PortA
	// RA0 -  SENS  - Input
	// RA1 -        - Output
	// RA2 -        - Output
	// RA3 -        - Output
	// RA4 -        - Output
	// RA5 -        - Output
	// RA6 - OSC
	TRISA = 0b00000001;
    PORTA = 0b00000000;

	// PortB
	// RB0 - RFID_RX- Input
	// RB1 -        - Output
	// RB2 - CAN TX - Output
	// RB3 - CAN RX - input
	// RB4 -  		- input
	// RB5 - 		- input
	// RB6 - 		- input
	// RB7 - 		- input 
	TRISB = 0b11111001;
    PORTB = 0b00000000;
	
	// RC0 -           - Output
	// RC1 - Button    - Input
    // RC2 - PWM       - Output
	// RC3 -           - Output
	// RC4 -           - Output
	// RC5 -           - Output
	// RC6 -           - Output
 	// RC7 -           - Output
	TRISC = 0b00000010;
    PORTC = 0b00000000;    

    // TIMERS    
    OpenTimer0(TIMER_INT_OFF  &               
               T0_16BIT       &
               T0_SOURCE_INT  &
               T0_PS_1_1
              );

    OpenTimer1(TIMER_INT_ON   &
               T1_16BIT_RW    &
               T1_SOURCE_INT  &
               T1_PS_1_1      &
               T1_OSC1EN_OFF
              );

    OpenTimer2(TIMER_INT_OFF  & 
               T2_PS_1_1      &
               T2_POST_1_1
              );

    // ADC

    OpenADC(ADC_FOSC_64       &
            ADC_RIGHT_JUST    &
            ADC_20_TAD        ,
            ADC_CH0           &
            ADC_INT_OFF       &
            ADC_REF_VDD_VREFMINUS /*ADC_VREFPLUS_VDD*/  &
            ADC_REF_VREFPLUS_VSS  /*ADC_VREFMINUS_VSS*/ ,
            0x0E);            
     
    // PWM 125 kHz
    PWM_Calibr(&PWM_f,&PWM_adc);
    OpenPWM1(PWM_f);// 79
    SetDCPWM1((PWM_f*2)-6);  // 156  
    PWM_enable = ON;

    RCONbits.IPEN = 1; // enable HI and LOW priorities

    // INT priority
    IPR1bits.TMR1IP = 0;      // Timer1 LOW priority

    OpenRB0INT( PORTB_CHANGE_INT_ON & RISING_EDGE_INT & PORTB_PULLUPS_OFF );

    // Global INT ebable
	INTCONbits.GIEH = 1;		
    INTCONbits.GIEL = 1;    

	return;
}

//***************************************************************************
// Main() - Main Routine
//***************************************************************************
void main( void )
{
	unsigned char a;
	unsigned char i;
	unsigned char ctrlreg;	// Current control register
	BOOL bOn;
	
    EmDataFlags.EmDataReceived = 0;
    EmDataFlags.EmDataTrue = 0;
    EmDataTimer = 0;

	init();                             // Initialize Microcontroller	

	// Check VSCP persistent storage and
	// restore if needed
	if ( !vscp_check_pstorage() ) {
		
		// Spoiled or not initialized - reinitialize
		init_app_eeprom();	
	}

	vscp_init();						// Initialize the VSCP functionality

	while ( 1 ) {                     	// Loop Forever

        ClrWdt();                       // Give the dog a bone        
        
		if ( ( vscp_initbtncnt > 250 ) && ( VSCP_STATE_INIT != vscp_node_state ) ) {
			// Init button pressed
			vscp_nickname = VSCP_ADDRESS_FREE;
			writeEEPROM( VSCP_EEPROM_NICKNAME, VSCP_ADDRESS_FREE );
			vscp_init();
		}

		// Check for a valid  event
		vscp_imsg.flags = 0;
		vscp_getEvent();

		// do a meaurement if needed
		if ( measurement_clock > 1000 ) {
			
			measurement_clock = 0;
			
			// Do VSCP one second jobs 
			vscp_doOneSecondWork();
			
			// Temperature report timers are only updated if in active 
			// state guid_reset
			if ( VSCP_STATE_ACTIVE == vscp_node_state  ) {
				
					
			}
							
		}

		switch ( vscp_node_state ) {

			case VSCP_STATE_STARTUP:			// Cold/warm reset

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

		} 

		doWork();

    } // while
}
 
///////////////////////////////////////////////////////////////////////////////
// init_app_eeprom
//

void init_app_eeprom( void )
{
    unsigned char i,j;

    writeEEPROM( REG_ZONE, 0 );
    writeEEPROM( REG_SUBZONE, 0 );
    
    writeEEPROM( EEPROM_SUBZONE_LED_RED, 1 );
    writeEEPROM( EEPROM_SUBZONE_LED_GREEN, 2 );
    writeEEPROM( EEPROM_SUBZONE_KEY, 3 );
    

    writeEEPROM( EEPROM_ONTIME_LED_RED, 10 );
    writeEEPROM( EEPROM_ONTIME_LED_GREEN, 10 );
    writeEEPROM( EEPROM_ONTIME_KEY , 10 );

    writeEEPROM( EEPROM_CONFIG, MASK_GREEN_LED | MASK_BEEP );

    writeEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID0 ,'R' );
    writeEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID1 ,'F' );
    writeEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID2 ,'I' );
    writeEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID3 ,'D' );

    writeEEPROM( VSCP_EEPROM_REG_MANUFACTUR_SUBID0,'2' );
    writeEEPROM( VSCP_EEPROM_REG_MANUFACTUR_SUBID1,'0' );
    writeEEPROM( VSCP_EEPROM_REG_MANUFACTUR_SUBID2,'0' );
    writeEEPROM( VSCP_EEPROM_REG_MANUFACTUR_SUBID3,'0' );

	// * * * Decision Matrix * * *
	// All elements disabled.
	for ( i=0; i<DESCION_MATRIX_ELEMENTS; i++ ) {
		for ( j=0; j<8; j++ ) {
	      writeEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + i*8 + j, 0 );
		}
    }
}

///////////////////////////////////////////////////////////////////////////////
// init_app_ram
//

void init_app_ram( void )
{
}

///////////////////////////////////////////////////////////////////////////////
// WriteData
//

void WriteData( unsigned char Index,unsigned char Value )
{
   if ( Value ) Data |= ( 1 << Index );   
   else  Data &= ~(1 << Index); 
}

///////////////////////////////////////////////////////////////////////////////
// ReadData
//

unsigned char ReadData( unsigned char Index )
{
   if ( (Data & ( 1 << Index ) ) ) return 1;   
   else return 0;
}

///////////////////////////////////////////////////////////////////////////////
// InitFSKData
//

void InitFSKData( void )
{
   DataWriteIndex = 0;         
   DataReadIndex = 0;
   HeaderCounter = 0;
   DataBitCounter = 0;
   EmDataIndex = 0;
   RxState = HEADER; 
}

///////////////////////////////////////////////////////////////////////////////
// KeyOn
//

void KeyOn( unsigned int time )
{
 PORTAbits.RA1 = 1;
 RedLedTimer = time;
}

///////////////////////////////////////////////////////////////////////////////
// KeyOff
//

void KeyOff( void )
{
  PORTAbits.RA1 = 0;
}

///////////////////////////////////////////////////////////////////////////////
// RedLedOn
//

void RedLedOn( unsigned int time )
{
  PORTAbits.RA4 = 1;
  RedLedTimer = time;
}

///////////////////////////////////////////////////////////////////////////////
// GreenLedOn
//

void GreenLedOn( unsigned int time )
{
  PORTAbits.RA3 = 1; 
  GreenLedTimer = time;
}

///////////////////////////////////////////////////////////////////////////////
// RedLedOff
//

void RedLedOff( void )
{
  PORTAbits.RA4 = 0;
}

///////////////////////////////////////////////////////////////////////////////
// GreenLedOff
//

void GreenLedOff( void )
{
  PORTAbits.RA3 = 0; 
}

///////////////////////////////////////////////////////////////////////////////
// BeeperOn
//

void BeeperOn( unsigned int time )
{
  PORTCbits.RC0 = 1;
  BeeperTimer = time;
}


///////////////////////////////////////////////////////////////////////////////
// read_app_register
//

uint8_t vscp_readAppReg( uint8_t reg )
{
	int tmpval;
	uint8_t val, checksum;
	uint8_t rv;

	switch ( reg ) {
		
		// Zone
		case 0x00:
			rv =  readEEPROM( EEPROM_ZONE );			
			break;

		// Subzone
		case 0x01:
			rv =  readEEPROM( EEPROM_SUBZONE );
			break;	
		
	    // Red LED subzone
		case REG_SUBZONE_LED_RED:
			writeEEPROM( EEPROM_SUBZONE_LED_RED, val );
			rv = readEEPROM( EEPROM_SUBZONE_LED_RED );
			break;		
			
		// Green LED SubZone
		case REG_SUBZONE_LED_GREEN:
			writeEEPROM( EEPROM_SUBZONE_LED_GREEN, val );
			rv = readEEPROM( EEPROM_SUBZONE_LED_GREEN );
			break;		
			
		// Key SubZone
		case REG_SUBZONE_KEY:
			writeEEPROM( EEPROM_SUBZONE_KEY, val );
			rv = readEEPROM( EEPROM_SUBZONE_KEY );
			break;	
					
		// Red LED on time
		case REG_RED_LED_ON_TIME:
			writeEEPROM( EEPROM_ONTIME_LED_RED, val );
			rv = readEEPROM( EEPROM_ONTIME_LED_RED );
			break;					
						
		// Green LED on time
		case REG_GREEN_LED_ON_TIME:
			writeEEPROM( EEPROM_ONTIME_LED_GREEN, val );
			rv = readEEPROM( EEPROM_ONTIME_LED_GREEN );
			break;	
						
		// Key on time
		case REG_KEY_ON_TIME:
			writeEEPROM( EEPROM_ONTIME_KEY, val );
			rv = readEEPROM( EEPROM_ONTIME_KEY );
			break;	
			
		// Device configuration
		case REG_DEV_CONFIG:
			writeEEPROM( EEPROM_CONFIG, val );
			rv = readEEPROM( EEPROM_CONFIG );
			break;	
			
		case REG_PWM_VALUE:
		    rv = PWM_f;
		    break;																				
			
		case REG_PWM_ADC:
		    rv = PWM_adc;
		    break;
			
		case REG_EMDATA0:
		    rv = EmData[ 0 ];
		    break;	
	
	    case REG_EMDATA1:
	        rv = EmData[ 1 ];
		    break;
		    
		case REG_EMDATA2:
		    rv = EmData[ 2 ];
		    break;
		    
		case REG_EMDATA3:
		    rv = EmData[ 3 ];
		    break;
		    
		case REG_EMDATA4:
		    rv = EmData[ 4 ];
		    break;
		    
		case REG_EMDATA5:
		    rv = EmData[ 5 ];
		    break;			
	 
		default:
			rv = 0;	
			break;
	}

	return rv;
}

///////////////////////////////////////////////////////////////////////////////
// write_app_register
//

uint8_t vscp_writeAppReg( uint8_t reg, uint8_t val )
{
	uint8_t i;
	int tmpval;
	uint8_t checksum;
	uint8_t rv;

	switch ( reg ) {

		// Zone
		case REG_ZONE:
			writeEEPROM( EEPROM_ZONE, val );
			rv = readEEPROM( EEPROM_ZONE );
			break;
			
		// SubZone
		case REG_SUBZONE:
			writeEEPROM( EEPROM_SUBZONE, val );
			rv = readEEPROM( EEPROM_SUBZONE );
			break;
			
		// Red LED subzone
		case REG_SUBZONE_LED_RED:
			writeEEPROM( EEPROM_SUBZONE_LED_RED, val );
			rv = readEEPROM( EEPROM_SUBZONE_LED_RED );
			break;		
			
		// Green LED SubZone
		case REG_SUBZONE_LED_GREEN:
			writeEEPROM( EEPROM_SUBZONE_LED_GREEN, val );
			rv = readEEPROM( EEPROM_SUBZONE_LED_GREEN );
			break;		
			
		// Key SubZone
		case REG_SUBZONE_KEY:
			writeEEPROM( EEPROM_SUBZONE_KEY, val );
			rv = readEEPROM( EEPROM_SUBZONE_KEY );
			break;	
					
		// Red LED on time
		case REG_RED_LED_ON_TIME:
			writeEEPROM( EEPROM_ONTIME_LED_RED, val );
			rv = readEEPROM( EEPROM_ONTIME_LED_RED );
			break;					
						
		// Green LED on time
		case REG_GREEN_LED_ON_TIME:
			writeEEPROM( EEPROM_ONTIME_LED_GREEN, val );
			rv = readEEPROM( EEPROM_ONTIME_LED_GREEN );
			break;	
						
		// Key on time
		case REG_KEY_ON_TIME:
			writeEEPROM( EEPROM_ONTIME_KEY, val );
			rv = readEEPROM( EEPROM_ONTIME_KEY );
			break;	
			
		// Device configuration
		case REG_DEV_CONFIG:
			writeEEPROM( EEPROM_CONFIG, val );
			rv = readEEPROM( EEPROM_CONFIG );
			break;	                
	
		default:
			rv = ~val; // error return	
			break;
	}

	return rv;	
}


///////////////////////////////////////////////////////////////////////////////
//                        VSCP Required Methods
//////////////////////////////////////////////////////////////////////////////

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
// Get the bootloader algorithm code
//

unsigned char getBootLoaderAlgorithm( void ) 
{
	return VSCP_BOOTLOADER_PIC1;	
}

///////////////////////////////////////////////////////////////////////////////
// Get the buffer size
//

unsigned char getBufferSize( void ) 
{
	return 8;	// Standard CAN frame
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
//  vscp_getMatrixInfo
//

void vscp_getMatrixInfo( void )
{
	uint8_t i;
	
	// We don't implement a DM for this module
	// So we just set the data to zero
	for ( i = 0; i < 8; i++ ) {
		vscp_omsg.data[ i ] = 0;
	}	

}

///////////////////////////////////////////////////////////////////////////////
//  vscp_getEmbeddedMdfInfo
//

void vscp_getEmbeddedMdfInfo( void )
{
	// No embedded DM so we respond with info about that
	
	vscp_omsg.priority = VSCP_PRIORITY_NORMAL;
	vscp_omsg.flags = VSCP_VALID_MSG + 3;
	vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

	vscp_omsg.data[ 0 ] = 0;
	vscp_omsg.data[ 1 ] = 0;
	vscp_omsg.data[ 2 ] = 0;	
	
	// send the message
	vscp_sendEvent();
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getZone
//

uint8_t vscp_getZone( void )
{
	return readEEPROM( EEPROM_ZONE );
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getSubzone
//

uint8_t vscp_getSubzone( void )
{
	return readEEPROM( EEPROM_SUBZONE );
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
	_asm reset _endasm
}

///////////////////////////////////////////////////////////////////////////////
// doWork
//
// The actual work is done here.
//

void doWork( void )
{
    unsigned char tmp,x,y;  
    unsigned char parity1,parity2,parity3,parity4;
    //unsigned char TempData[6];

	if ( VSCP_STATE_ACTIVE == vscp_node_state ) {	
    	
        if ( EmDataFlags.EmDataReceived ) {           
            
            parity1 = 0;
            parity2 = 0;
            parity3 = 0;
            parity4 = 0;

            for ( x=0; x<5; x++ ) {
                
                if(EmData[x] & 8)   parity1++;
                if(EmData[x] & 128) parity1++;

                if(EmData[x] & 4)   parity2++;
                if(EmData[x] & 64)  parity2++;

                if(EmData[x] & 2)   parity3++;
                if(EmData[x] & 32)  parity3++;

                if(EmData[x] & 1)   parity4++;
                if(EmData[x] & 16)  parity4++;
            }

            if( (((EmData[5] >> 7)&1)==(parity1&1)) &&
                    (((EmData[5] >> 6)&1)==(parity2&1)) &&
                    (((EmData[5] >> 5)&1)==(parity3&1)) &&
                    (((EmData[5] >> 4)&1)==(parity4&1)) ) {

                EmDataFlags.EmDataTrue = 1;
            }
            else {
                EmDataFlags.EmDataTrue = 0;
            }    

            EmDataFlags.EmDataReceived = 0;
        }

        if ( EmDataFlags.EmDataTrue == 1 ) {
            if ( EmDataTimer != 0 ) {
                EmDataFlags.EmDataTrue = 0;
                EmDataTimer = 800;
                return;
            }

            // memcpy(TempData,EmData,6);
			
			// AKHE 2008-12-16
			// Corrected to two events for 64-bit code. Previously index
			// was used for MSB byte of code whcih was invalid.m 

			// Event part 1 of 2
            vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
            vscp_omsg.flags = VSCP_VALID_MSG + 8;
            vscp_omsg.vscp_class = VSCP_CLASS1_INFORMATION;
            vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_TOKEN_ACTIVITY;

            vscp_omsg.data[ 0 ] = 8;          // Touched and released | RFID Token. 40-bits 
            vscp_omsg.data[ 1 ] = readEEPROM( VSCP_EEPROM_END + REG_ZONE );          // Device zone 
            vscp_omsg.data[ 2 ] = readEEPROM( VSCP_EEPROM_END + REG_SUBZONE );       // Device subzone
			vscp_omsg.data[ 3 ] = 0;	// First part

			vscp_omsg.data[ 4 ] = 0x00;	// To make 64-bits
			vscp_omsg.data[ 5 ] = 0x00;
			vscp_omsg.data[ 6 ] = EmData[ 0 ]; 
            vscp_omsg.data[ 7 ] = EmData[ 1 ];         

            vscp_sendEvent();
			
			// Event Part 2 of 2
			vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
            vscp_omsg.flags = VSCP_VALID_MSG + 8;
            vscp_omsg.vscp_class = VSCP_CLASS1_INFORMATION;
            vscp_omsg.vscp_type = VSCP_TYPE_INFORMATION_TOKEN_ACTIVITY;

            vscp_omsg.data[ 0 ] = 7;          // Touched and released | RFID Token. 64-bits 
            vscp_omsg.data[ 1 ] = readEEPROM( VSCP_EEPROM_END + REG_ZONE );          // Device zone 
            vscp_omsg.data[ 2 ] = readEEPROM( VSCP_EEPROM_END + REG_SUBZONE );       // Device subzone
			vscp_omsg.data[ 3 ] = 1;	// Second part

            vscp_omsg.data[ 4 ] = EmData[ 2 ];
            vscp_omsg.data[ 5 ] = EmData[ 3 ];
            vscp_omsg.data[ 6 ] = EmData[ 4 ];          

            vscp_sendEvent();

            if ( readEEPROM( VSCP_EEPROM_END + REG_DEV_CONFIG ) & MASK_GREEN_LED ) {
                GreenLedOn(500);
            }

            if ( readEEPROM( VSCP_EEPROM_END + REG_DEV_CONFIG ) & MASK_BEEP ) { 
                ClosePWM1();
                PWM_enable = OFF;       
                BeeperOn(100);
            }
            EmDataTimer = 800;
            EmDataFlags.EmDataTrue = 0;
        }
    } // active state
    else {
        EmDataFlags.EmDataTrue = 0;	   
        EmDataFlags.EmDataReceived = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// PWM_Calibr
//

void PWM_Calibr( unsigned char *pwm_f, unsigned char *pwm_adc )
{
    unsigned char rez,rez_temp,x;
    int ADCLastValue, ADCValue ;

    ADCLastValue = 0;
    ADCValue = 0;
    rez = 72;
    rez_temp = 72;

    for ( x=0; x<12; x++ ) {
        
        ClrWdt();

        OpenPWM1( rez_temp + x );           // 79 (0x4F) centras == 125 kHz
        SetDCPWM1( ( rez_temp * 2 ) - 6 );  // 156

        Delay100TCYx( 255 );
        Delay100TCYx( 255 );
        Delay100TCYx( 255 );
        Delay100TCYx( 255 );

        ConvertADC();
        while ( BusyADC() ) temp = temp;      
        ADCValue = ReadADC();
  
        if ( ADCValue > ADCLastValue ) {
            ADCLastValue = ADCValue;
            rez = rez_temp + x;
        }
    }

    writeEEPROM( VSCP_EEPROM_END + REG_PWM_VALUE, rez );
    writeEEPROM( VSCP_EEPROM_END + REG_PWM_ADC, ADCValue );

    *pwm_f = rez;
    *pwm_adc = ADCValue;
}

///////////////////////////////////////////////////////////////////////////////
// read_app_register
//

void read_app_register( unsigned char reg )
{
	int tmpval;
	unsigned char val, checksum;

	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG + 2;
	vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

	vscp_omsg.data[ 0 ] = reg;
	vscp_omsg.data[ 1 ] = 0x00; // default read	 

    vscp_omsg.data[ 1 ] = readEEPROM( VSCP_EEPROM_END + reg );

	// Send data
	vscp_sendEvent();
}

///////////////////////////////////////////////////////////////////////////////
// write_app_register
//

void write_app_register( unsigned char reg, unsigned char val )
{
	unsigned char i;
	int tmpval;
	unsigned char checksum;
	BOOL bInfoEvent = FALSE;
	BOOL bOn = FALSE;

	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG + 2;
	vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_RW_RESPONSE;

	vscp_omsg.data[ 0 ] = reg;
	vscp_omsg.data[ 1 ] = val;
	
    RedLedOn ( 1000 );

	if ( reg <= 0x08 )
     {
      writeEEPROM( VSCP_EEPROM_END + reg,val );
     } 
	
	// Send response
	vscp_sendEvent();	
}

///////////////////////////////////////////////////////////////////////////////
// Send Decsion Matrix Information
//

void sendDMatrixInfo( void )
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG + 2;
	vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.vscp_type = VSCP_TYPE_PROTOCOL_GET_MATRIX_INFO_RESPONSE;

	vscp_omsg.data[ 0 ] = DESCION_MATRIX_ELEMENTS;
	vscp_omsg.data[ 1 ] = REG_DESCION_MATRIX;

	vscp_sendEvent();	// Send data		
}

///////////////////////////////////////////////////////////////////////////////
// SendInformationEvent
//

void SendInformationEvent( unsigned char idx, unsigned char eventClass, unsigned char eventTypeId ) 
{
	vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
	vscp_omsg.flags = VSCP_VALID_MSG + 3;
	vscp_omsg.vscp_class = eventClass;
	vscp_omsg.vscp_type = eventTypeId;

	vscp_omsg.data[ 0 ] = idx;	// Register
	vscp_omsg.data[ 1 ] = readEEPROM( VSCP_EEPROM_END + REG_ZONE );
	vscp_omsg.data[ 2 ] = readEEPROM( VSCP_EEPROM_END + REG_SUBZONE);

	vscp_sendEvent();	// Send data
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
	
	for ( i=0; i<DESCION_MATRIX_ELEMENTS; i++ ) {
		
		// Get DM flags for this row
		dmflags = readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + 1 + ( 8 * i ) );
		
		// Is the DM row enabled?
		if ( dmflags & VSCP_DM_FLAG_ENABLED )
		 {
			
			// Should the originating id be checked and if so is it the same?
			if ( !( dmflags & VSCP_DM_FLAG_CHECK_OADDR ) &&
				!(  vscp_imsg.oaddr == readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) ) ) ) {
				continue;					
			}	
			
			// Check if zone should match and if so if it match
			if ( dmflags & VSCP_DM_FLAG_CHECK_ZONE  )
			{
			  if ( (vscp_imsg.data[ 1 ] != readEEPROM( VSCP_EEPROM_END + REG_ZONE  )) && (vscp_imsg.data[ 1 ] != 255)  )
			   {
			    continue;
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
				
			if ( !( ( class_filter ^ vscp_imsg.vscp_class ) & class_mask ) &&
				 	!( ( type_filter ^ vscp_imsg.vscp_type ) & type_mask )) {
					 						
				// OK Trigger this action
				if ( readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTION  ) ) 
                {
                  doActionPulse( dmflags, readEEPROM( VSCP_EEPROM_END + REG_DESCION_MATRIX + ( 8 * i ) + VSCP_DM_POS_ACTIONPARAM  ) );										
				} // case	
			} // Filter/mask
		} // Row enabled
	} // for each row	
}


///////////////////////////////////////////////////////////////////////////////
// doActionPulse
//

void doActionPulse( unsigned char dmflags, unsigned char arg )
{
    if( readEEPROM( EEPROM_SUBZONE_LED_RED ) == vscp_imsg.data[ 2 ] )  {
        RedLedOn ( readEEPROM( EEPROM_ONTIME_LED_RED ) * 100L );
    }
    else if ( readEEPROM( EEPROM_SUBZONE_LED_GREEN ) == vscp_imsg.data[ 2 ] ) {
        GreenLedOn ( readEEPROM( EEPROM_ONTIME_LED_GREEN ) * 100L );
    }
    else if ( readEEPROM( EEPROM_SUBZONE_KEY ) == vscp_imsg.data[ 2 ] ) {
        KeyOn ( readEEPROM( EEPROM_ONTIME_KEY ) * 100L );
    }
}




///////////////////////////////////////////////////////////////////////////////
//                        VSCP Required Methods
//////////////////////////////////////////////////////////////////////////////





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
	return readEEPROM( VSCP_EEPROM_REG_USERID + idx );	
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
	return readEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx );	
}

///////////////////////////////////////////////////////////////////////////////
// getVSCPManufacturerId
// 
// Get Manufacturer id and subid from EEPROM
//

void vscp_setManufacturerId( uint8_t idx, uint8_t data )
{
	writeEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx, data );	
}

///////////////////////////////////////////////////////////////////////////////
// Get the bootloader algorithm code
//

uint8_t vscp_getBootLoaderAlgorithm( void ) 
{
	return VSCP_BOOTLOADER_PIC1;	
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
// sendVSCPFrame
//

int8_t sendVSCPFrame( uint16_t vscpclass, 
						uint8_t vscptype, 
						uint8_t nodeid,
						uint8_t priority,
						uint8_t size, 
						uint8_t *pData )
{
	uint32_t id = ( (uint32_t)priority << 26 ) |
						( (uint32_t)vscpclass << 16 ) |
						( (uint32_t)vscptype << 8) |
						nodeid;		// nodeaddress (our address)
	
	if ( !sendCANFrame( id, size, pData ) ) {
		// Failed to send message
		vscp_errorcnt++;
		return FALSE;
	}
	
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
	
	if ( !getCANFrame( &id, pSize, pData ) ) {
		return FALSE;
	}

	*pNodeId = id & 0x0ff;
	*pvscptype = ( id >> 8 ) & 0xff;
	*pvscpclass = ( id >> 16 ) & 0x1ff;
    *pPriority = (uint16_t)( 0x07 & ( id >> 26 ) );
    
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// sendCANFrame
//

int8_t sendCANFrame( uint32_t id, uint8_t dlc, uint8_t *pdata )
{
	if ( !vscp18f_sendMsg( id, 
							pdata , 
							dlc, 
							CAN_TX_XTD_FRAME  ) ) {
		
		// Failed to send message
		return FALSE;

	}

	vscp_omsg.flags = 0;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// getCANFrame
//

int8_t getCANFrame( uint32_t *pid, uint8_t *pdlc, uint8_t *pdata )
{
	uint8_t flags;

	// Dont read in new message if there already is a message
	// in the input buffer
	if ( vscp_imsg.flags & VSCP_VALID_MSG ) return FALSE;

	if ( vscp18f_readMsg( pid, pdata, pdlc, &flags ) ) {

		// RTR not interesting
		if ( flags & CAN_RX_RTR_FRAME ) return FALSE;

		// Must be extended frame
		if ( !( flags & CAN_RX_XTD_FRAME ) ) return FALSE;
		
		return TRUE;
	}	
	
	return FALSE;
}


