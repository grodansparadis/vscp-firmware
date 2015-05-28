/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  Window Motors and thermal radiator control
 * 	Version: See project header
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 1995-2006 Ake Hedman, eurosource
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
 *
 * This software is part of the VSCP node Chicago
 * 
 * ******************************************************************************
*/

#include <xc.h>
#include "main.h"

#include <eeprom.h>
#include <can18f.h>
#include <vscp_firmware.h>
#include <vscp_class.h>	
#include <vscp_type.h>
#include "../../augusto/project/vscp_appLayer.h"

#include "../../augusto/project/driver.h"

uint8_t firmwareVersion[3] = {0,0,1};
uint8_t GuID[16] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
void _startup (void);
extern timeBasedEventStruct timeEvent, timeOverride;

const char mdfLink[] = "www.salvomusumeci.com/jarvis/augusto.xml";


void interrupt low_priority isr_low( void ){
/*	BYTE temp;
	unsigned short tval0, tval1;	
	
	// Clock
	if ( PIR1bits.TMR2IF ) {	// If a Timer2 Interrupt, Then...
		
		vscp_timer++;
		measurement_clock++;
		clock_10ms++;

		// Check for init button
		if ( INIT_BUTTON ) {
			vscp_initbtncnt = 0;
		}
		else {
			// Active
			vscp_initbtncnt++;
		}

		// Status LED
		vscp_statuscnt++;
		if ( ( VSCP_LED_BLINK1 == vscp_initledfunc ) && ( vscp_statuscnt > 100 ) ) {

			if ( INIT_LED ) {
				INIT_LED = 0; 
			}
			else {
				INIT_LED = 1; 
			}	

			vscp_statuscnt = 0;

		}
		else if ( VSCP_LED_ON == vscp_initledfunc ) {
			INIT_LED = 1;	
			vscp_statuscnt = 0;	
		}
		else if ( VSCP_LED_OFF == vscp_initledfunc ) {
			INIT_LED = 0; 
			vscp_statuscnt = 0;
		}

		PIR1bits.TMR2IF = 0;     // Clear Timer0 Interrupt Flag

	}

/*	
	// CAN error
	if ( PIR3bits.ERRIF ) {
		
		temp = COMSTAT;
		PIR3 = 0;
		
	}
*/	
}

void interrupt isr_high(){
    di();
    TMR0_interrupt();
    ei();
}


void init_app_eeprom(void){
    init_augusto_eeprom();
}

void init_app_ram(void){
    init_augusto_ram();
}
//***************************************************************************
// Main() - Main Routine
//***************************************************************************
void main(){
   
    OSCCON = 0xF0;      //Initialitation of Internal Oscillator
    OSCTUNEbits.PLLEN = 1;
    init_app_ram();     // Initialize data
    hardware_setup();   //Hardware initialization

    if ( !vscp_check_pstorage() )   // Check VSCP persistent storage and
        init_app_eeprom();          // restore if needed
    vscp_init();                    // Initialize the VSCP functionality
    
    while(1){
        ClrWdt();			// Feed the dog
       //Handler scheduler
        if(timeEvent._10mS){    //10mS Event
            timeEvent._10mS = 0;
            vscp_10mS_Running();
        }
        if(timeEvent._100mS){   //100mS Event
            timeEvent._100mS = 0;
            vscp_100mS_Running();
            hardware_100mS();
            vscp_ledActivity();
            //WE_stateMachine_running(&WE_data[0]);
        }
        if(timeEvent._1s){      //1second Event
            timeEvent._1s = 0;
            vscp_doOneSecondWork(); // Do VSCP one second jobs
            
            greenLed_pin = !greenLed_pin;
            //SendInformationEvent( 1 , VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_DETECT, 2 );

        }
        vscp_freeRunning();

        //TODO: Handling the override event
    }

}

void doApplicationDM(int DecisionMatrixIndex){
    //case VSCP_ACTION_WINDOW_ENGINE:
    //                WE_getInputCode(&vscp_imsg, &WE_data[0]);
    //                break;
}