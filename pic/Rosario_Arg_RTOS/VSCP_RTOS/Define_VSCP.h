/**********************************************************************/
/*                                                                    */
/* File name: Define_VSCP.h                                           */
/*                                                                    */
/* Since:     2007-Marzo-07                                           */
/*                                                                    */
/* Version:   VSCP with RTOS v1.0 (PICos18 2.10)       			          */
/*            Copyright (C) 2007, MAPito							                */
/*                                                                    */
/* Author:    Designed by Marcelo Poma                                */
/*                                                                    */
/*                                                                    */
/* Purpose:   General VSCP initialization and kernel start            */
/*                                                                    */
/*                                                                    */
/* History:                                                           */
/* 2007/03/07 [MAP] Create this file from PICos18 Tutorial            */
/*                                                                    */
/*            PICos18 is free software; you can redistribute it       */
/*            and/or modify it under the terms of the GNU General     */
/*            Public License as published by the Free Software        */
/*            Foundation; either version 2, or (at your option)       */
/*            any later version.                                      */
/*                                                                    */
/*            PICos18 is distributed in the hope that it will be      */
/*            useful, but WITHOUT ANY WARRANTY; without even the      */
/*            implied warranty of MERCHANTABILITY or FITNESS FOR A    */
/*            PARTICULAR PURPOSE.  See the GNU General Public         */
/*            License for more details.                               */
/*                                                                    */
/*            You should have received a copy of the GNU General      */
/*            Public License along with gpsim; see the file           */
/*            COPYING.txt. If not, write to the Free Software         */
/*            Foundation, 59 Temple Place - Suite 330,                */
/*            Boston, MA 02111-1307, USA.                             */
/*                                                                    */
/*          > A special exception to the GPL can be applied should    */
/*            you wish to distribute a combined work that includes    */
/*            PICos18, without being obliged to provide the source    */
/*            code for any proprietary components.                    */
/**********************************************************************/

#ifndef _DEFINE_VSCP_H
#define _DEFINE_VSCP_H

#include <device.h>

#define __EXT29BITS__
#define __NOFILTER__

/***********************************************************************
 * ------------------------ Timer settings -----------------------------
 **********************************************************************/
#define _10MHZ	63320
#define _16MHZ	61768
#define _20MHZ	60768
#define _32MHZ	57768
#define _40MHZ 	55768

/***********************************************************************
 * ----------------------------- Events --------------------------------
 **********************************************************************/
/* LMAGIC : Tmr0.lt = 65536 - (CPU_FREQUENCY_HZ/4/1000 - 232)         */

//Used in TASK0
#define LED_EVENT		       	0x01
#define INIT_EVENT		      0x02
#define PROT_EVENT					0x04
#define WD_EVENT						0x80

//Used in TASK2
#define VSCP_QUEUE_RX				0x01
#define VSCP_QUEUE_TX     	0x02
#ifdef RMT_DRV              //include only if remote control decoder is present
	#define	RMT_NEW_MSG				0x04	
#endif
#define WD_EVENT						0x80

//Used in VSCP Driver
#define VSCP_NEW_MSG        0x01
#define VSCP_RCV_MSG        0x02
#define VSCP_ERR_MSG        0x04

//Used in Application Loop 
#define TIC_EVENT						0x01
#define DOOR_EVENT					0x02
#define ALERT_EVENT_ON			0x04
#define ALERT_EVENT_OFF			0x08
#define WD_EVENT						0x80

//Used in REMOTE Driver
#ifdef RMT_DRV              //include only if remote control decoder is present
	#define RMT_RCV_MSG       0x01
#endif



/***********************************************************************
 * ----------------------------- Task ID -------------------------------
 **********************************************************************/

#define VSCP_INIT_ID				 0
#define VSCP_PROT_ID         1
#define VSCP_LOOP_ID				 2
#define VSCP_DRV_ID          3

#ifdef RMT_DRV              //include only if remote control decoder is present
	#define RMT_DRV_ID           4
#endif 

#define VSCP_INIT_PRIO			 7		// VSCP Protocol INIT
#define VSCP_PROT_PRIO       8		// VSCP Protocol engine
#define VSCP_LOOP_PRIO			 9		// VSCP Loop Application
#define RESOURCE_EE_PRIO    10 		// EEPROM RESOURCE

#ifdef RMT_DRV              //include only if remote control decoder is present
	#define RMT_DRV_PRIO   			11 		// Remote Driver
#endif 

#define VSCP_DRV_PRIO       12		// VSCP CAN Driver


/***********************************************************************
 * ----------------------------- ALARM ID -------------------------------
 **********************************************************************/

#define ALARM_LED      			0
#define ALARM_INIT					1
#define ALARM_PROT					2
#define ALARM_LOOP					3

/***********************************************************************
 * ----------------------------- RESOURCE ID -------------------------------
 **********************************************************************/
#define RESOURCE_EE_ID      0  // First element of the list

#endif /* _DEFINE_VSCP_H */


/* End of File : define_VSCP.h */
