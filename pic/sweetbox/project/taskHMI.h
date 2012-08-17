/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  File: taskHMI.h
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

#ifndef taskFreeRTOS_LED_H
#define taskFreeTOS_LED_H

#define LEDCTRL_BIT_ON      0x80
#define LEDCTRL_BIT_BLINK   0x40
#define LEDCTRL_MASK_FREQ   0x0f

// HMI commad message
typedef struct {
    uint8_t command;
    uint8_t ctrlbyte;
} messageHMI;    

#define HMI_COMMAND_LED_IR_CONTROL              1
#define HMI_COMMAND_LED_IR_ON                   2
#define HMI_COMMAND_LED_IR_OFF                  3
#define HMI_COMMAND_LED_IR_BLINK_ON             4
#define HMI_COMMAND_LED_IR_BLINK_OFF            5

#define HMI_COMMAND_LED_CONNECTION_CONTROL      6
#define HMI_COMMAND_LED_CONNECTION_ON           7
#define HMI_COMMAND_LED_CONNECTION_OFF          8
#define HMI_COMMAND_LED_CONNECTION_BLINK_ON     9
#define HMI_COMMAND_LED_CONNECTION_BLINK_OFF    10

#define HMI_COMMAND_LED_STATUS_CONTROL          11
#define HMI_COMMAND_LED_STATUS_ON               12
#define HMI_COMMAND_LED_STATUS_OFF              13
#define HMI_COMMAND_LED_STATUS_BLINK_ON         14
#define HMI_COMMAND_LED_STATUS_BLINK_OFF        15


// The queue used to send messages to the HIM control task.
// defined in main.c
extern xQueueHandle queueHMI;

/*!
    This routine initialize the LED's,
    starts a controller task for each of them and 
    then wait for commands to set there status.
*/
void  taskHMICtrl( void *pvParameters ); 



#endif

