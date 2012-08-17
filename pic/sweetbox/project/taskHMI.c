/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  File: taskHMI.c
 *
 *  SweetBox Module
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

// Standard includes. 
#include <stdio.h>
#include <string.h>

// Scheduler includes. 
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "sweetbox.h"
#include "taskHMI.h"


// LED Control bytes
// Bit 7 - Led is onI mitt fall if bit is set.
// Bit 6 - Led is blinking if bit is set.
// Bit 5 - reserved.
// Bit 4 - reserved.
// Bit 0-3 - Blink frequency (ms)/100 meaning values from 
// 0 ms (ON) - 1500 ms can be set.
uint8_t prvLEDCtrl_IR = 0;                  // IR LED
uint8_t prvLEDCtrl_HostConnection = 0;      // Host connection
uint8_t prvLEDCtrl_Status = 0;              // Module status

// The methods that control the induvidual LED's 
void taskIRLEDCtrl( void *pvParameters );
void taskHostConnectionLEDCtrl( void *pvParameters );
void taskStatusLEDCtrl( void *pvParameters );

// HMI commands


// The length of the queue used to send messages to the HIM control task.
#define sizeHMI		3



/////////////////////////////////////////////////////////////////////////////////
// taskHMICtrl
//

void taskHMICtrl( void *pvParameters ) 
{
    messageHMI msg;
    
    // Parameters are not used - suppress compiler error. 
	( void ) pvParameters;
    
    // All LED's off 
    mPORTDClearBits( LED_IR_TRANSMIT | LED_HOST_CONNECTION | LED_STATUS );  
    
    // Set all LED pins as outputs       
    mPORTDSetPinsDigitalOut( LED_IR_TRANSMIT | LED_HOST_CONNECTION | LED_STATUS ); 
    
    sys_thread_new( ( signed portCHAR * ) "irled",
                    taskIRLEDCtrl,
                    NULL,
                    stackLEDCtrl,
                    HMI_PRIORITY );
		        
	sys_thread_new( ( signed portCHAR * ) "conled", 
	                taskHostConnectionLEDCtrl,
                    NULL,
                    stackLEDCtrl,
                    HMI_PRIORITY );	                	              
		            
	sys_thread_new( ( signed portCHAR * ) "statled",
	                taskStatusLEDCtrl,
                    NULL,
                    stackLEDCtrl,
                    HMI_PRIORITY );	
    
    // Create the LED control queue
    queueHMI = xQueueCreate( sizeHMI, sizeof( messageHMI ) );	
    
    messageHMI msg2;
    msg2.command = HMI_COMMAND_LED_STATUS_BLINK_ON;
    msg2.ctrlbyte = 10;
    xQueueSend( queueHMI, ( void * ) &msg2, ( portTickType ) 0 );
	            
		            
    while ( 1 )
    {
        // Wait for HMI commands
	    while ( pdPASS != xQueueReceive( queueHMI, &msg, portMAX_DELAY ) );
	    
        switch ( msg.command ) {

            case HMI_COMMAND_LED_IR_CONTROL:
                prvLEDCtrl_IR = msg.ctrlbyte;
                break;
                
            case HMI_COMMAND_LED_IR_ON:
                prvLEDCtrl_IR |= LEDCTRL_BIT_ON;
                break;
                
            case HMI_COMMAND_LED_IR_OFF:
                prvLEDCtrl_IR &= ~LEDCTRL_BIT_ON;
                break;
                
            case HMI_COMMAND_LED_IR_BLINK_ON:
                prvLEDCtrl_IR |= LEDCTRL_BIT_BLINK;
                // Set blink rate
                prvLEDCtrl_IR = ( prvLEDCtrl_IR & ~LEDCTRL_MASK_FREQ ) + 
                                    ( msg.ctrlbyte & LEDCTRL_MASK_FREQ );
                break;
                
            case HMI_COMMAND_LED_IR_BLINK_OFF:
                prvLEDCtrl_IR &= ~LEDCTRL_BIT_BLINK;
                break;
                
            case HMI_COMMAND_LED_CONNECTION_CONTROL:
                prvLEDCtrl_HostConnection = msg.ctrlbyte;
                break;
                
            case HMI_COMMAND_LED_CONNECTION_ON:
                prvLEDCtrl_HostConnection |= LEDCTRL_BIT_ON;
                break;
                
            case HMI_COMMAND_LED_CONNECTION_OFF:
                prvLEDCtrl_HostConnection &= ~LEDCTRL_BIT_ON;
                break;
                
            case HMI_COMMAND_LED_CONNECTION_BLINK_ON:
                prvLEDCtrl_HostConnection |= LEDCTRL_BIT_BLINK;
                // Set blink rate
                prvLEDCtrl_HostConnection = ( prvLEDCtrl_HostConnection & ~LEDCTRL_MASK_FREQ ) + 
                                                ( msg.ctrlbyte & LEDCTRL_MASK_FREQ );
                break;
                
            case HMI_COMMAND_LED_CONNECTION_BLINK_OFF:
                prvLEDCtrl_HostConnection &= ~LEDCTRL_BIT_BLINK;
                break;
                
            case HMI_COMMAND_LED_STATUS_CONTROL:
                prvLEDCtrl_Status = msg.ctrlbyte;
                break;     
                
            case HMI_COMMAND_LED_STATUS_ON:
                prvLEDCtrl_Status |= LEDCTRL_BIT_ON;
                break;
                
            case HMI_COMMAND_LED_STATUS_OFF:
                prvLEDCtrl_Status &= ~LEDCTRL_BIT_ON;
                break;
                
            case HMI_COMMAND_LED_STATUS_BLINK_ON:
                prvLEDCtrl_Status |= LEDCTRL_BIT_BLINK;
                // Set blink rate
                prvLEDCtrl_Status = ( prvLEDCtrl_Status & ~LEDCTRL_MASK_FREQ ) + 
                                                ( msg.ctrlbyte & LEDCTRL_MASK_FREQ );
                break;
                
            case HMI_COMMAND_LED_STATUS_BLINK_OFF:
                prvLEDCtrl_Status &= ~LEDCTRL_BIT_BLINK;
                break;
        } 
        
        vTaskDelay( 300 );  
         
    }    		            
		            	            
}

///////////////////////////////////////////////////////////////////////////////
// taskIRLEDCtrl
//

void taskIRLEDCtrl( void *pvParameters )
{
    // Parameters are not used - suppress compiler error. 
	( void ) pvParameters;
	
	while ( 1 ) {
        if ( prvLEDCtrl_IR & LEDCTRL_BIT_BLINK ) {	
            mPORTDToggleBits( LED_IR_TRANSMIT );
		    vTaskDelay( ( prvLEDCtrl_IR & LEDCTRL_MASK_FREQ ) * 100 );
        }
        else {
            if ( prvLEDCtrl_IR & LEDCTRL_BIT_ON ) {
                mPORTDSetBits( LED_IR_TRANSMIT );
                vTaskDelay( 500 );    
            }
            else {
                mPORTDClearBits( LED_IR_TRANSMIT );
                vTaskDelay( 500 );
            }           
        }    
	}
}


///////////////////////////////////////////////////////////////////////////////
// taskHostConnectionLEDCtrl
//

void taskHostConnectionLEDCtrl( void *pvParameters )
{
    // Parameters are not used - suppress compiler error. 
	( void ) pvParameters;
	
	while ( 1 ) {
        if ( prvLEDCtrl_HostConnection & LEDCTRL_BIT_BLINK ) {	
            mPORTDToggleBits( LED_HOST_CONNECTION );
		    vTaskDelay( ( prvLEDCtrl_HostConnection & LEDCTRL_MASK_FREQ ) * 100 );
        }
        else {
            if ( prvLEDCtrl_HostConnection & LEDCTRL_BIT_ON ) {
                mPORTDSetBits( LED_HOST_CONNECTION ); 
                vTaskDelay( 500 );   
            }
            else {
                mPORTDClearBits( LED_HOST_CONNECTION );
                vTaskDelay( 500 );
            }           
        }    
	}
}

///////////////////////////////////////////////////////////////////////////////
// taskStatusLEDCtrl
//

void taskStatusLEDCtrl( void *pvParameters )
{
    // Parameters are not used - suppress compiler error. 
	( void ) pvParameters;
	
	while ( 1 ) {
        if ( prvLEDCtrl_Status & LEDCTRL_BIT_BLINK ) {	
            mPORTDToggleBits( LED_STATUS );
		    vTaskDelay( ( prvLEDCtrl_Status & LEDCTRL_MASK_FREQ ) * 100 );
        }
        else {
            if ( prvLEDCtrl_Status & LEDCTRL_BIT_ON ) {
                mPORTDSetBits( LED_STATUS ); 
                vTaskDelay( 500 );   
            }
            else {
                mPORTDClearBits( LED_STATUS );
                vTaskDelay( 500 );
            }           
        }    
	}
}

