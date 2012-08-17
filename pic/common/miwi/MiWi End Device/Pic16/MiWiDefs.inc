;*********************************************************************
;* FileName:		MiWiDefs.inc
;* Dependencies:    none
;* Processor:   	PIC18F877A
;* Hardware:		PICDEM Z
;* Assembler:	    MPASMWIN 5.01 or higher
;* Linker:		    MPLINK 4.05 or higher
;* Company:		    Microchip Technology, Inc.
;*
;* Copyright and Disclaimer Notice for MiWi Software:
;*
;* Microchip Technology Inc. (“Microchip”) licenses this software to you 
;* solely for use with Microchip products, as described in the license 
;* agreement accompanying this software (“License”). The software is owned 
;* by Microchip, and is protected under applicable copyright laws.  
;* All rights reserved.
;* 
;* Distribution of this software (in object code or source code) is not 
;* permitted, except as described in Section 2 of the License.
;* 
;* SOFTWARE IS PROVIDED “AS IS.”  MICROCHIP EXPRESSLY DISCLAIM ANY 
;* WARRANTY OF ANY KIND, WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT 
;* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
;* PARTICULAR PURPOSE, OR NON-INFRINGEMENT. IN NO EVENT SHALL MICROCHIP 
;* BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, 
;* LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, 
;* TECHNOLOGY OR SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT 
;* LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, 
;* OR OTHER SIMILAR COSTS.
;*
;********************************************************************/
;* File Description:
;*   Defines all of the compile time options used in the stack
;* 
;* Change History:
;*  Rev   Date         Description
;*  0.1   11/15/2006   Initial revision
;*  1.0   01/15/2007   Initial release
;********************************************************************/

#include "P16F877A.INC"         ;include for the controller

#ifndef MIWIDEFS_H
#define MIWIDEFS_H

#define CLOCK_FREQ d'4000000'   ;FOSC of the microcontroller
#define BAUD_RATE d'19200'      ;desired baud rate of the UART

;Define the size of the RX and TX buffers
#define RX_BUFFER_SIZE d'96'
#define TX_BUFFER_SIZE d'85'

;Enable the modules used by the stack
;#define DEBUG_DUMP_NETWORK_TABLE
;#define ENABLE_CONSOLE
#define ENABLE_MSPI

;Define the Mirco - MRF24J40 interface
#define PHY_CS PORTC,0
#define PHY_CS_TRIS TRISC,0
#define PHY_CS_TRIS_REG TRISC
#define PHY_CS_REG PORTC

#define PHY_RESETn PORTC,2
#define PHY_RESETn_TRIS TRISC,2
#define PHY_RESETn_TRIS_REG TRISC
#define PHY_RESETn_REG PORTC

#define PHY_WAKE PORTC,1
#define PHY_WAKE_TRIS TRISC,1
#define PHY_WAKE_REG PORTC
#define PHY_WAKE_TRIS_REG

#define PHY_SDO PORTC,5
#define PHY_SDO_TRIS TRISC,5
#define PHY_SDO_TRIS_REG TRISC
#define PHY_SDO_REG PORTC

#define RFIF INTCON,INT0IF
#define RFIE INTCON,INT0IE
#define RF_INT_PIN PORTB,0
#define RF_INT_TRIS TRISB,0


#define PA_LEVEL 0x00

;NETWORK_TABLE_SIZE is limited by the amount of RAM available
;This requires 13 bytes per entry
#define NETWORK_TABLE_SIZE 6

;NETWORK_SEARCH is the time a device will search a channel for a network
#define NETWORK_SEARCH_0 0x00
#define NETWORK_SEARCH_1 0x00
#define NETWORK_SEARCH_2 0x01
#define NETWORK_SEARCH_3 0x00

;ASSOC_REQ_WAIT is the time between the association request and data request
#define ASSOC_REQ_WAIT_0 0x00
#define ASSOC_REQ_WAIT_1 0x00
#define ASSOC_REQ_WAIT_2 0x01
#define ASSOC_REQ_WAIT_3 0x00

;ORPHAN_WAIT is the time between the association request and data request
#define ORPHAN_WAIT_0 0x00
#define ORPHAN_WAIT_1 0x00
#define ORPHAN_WAIT_2 0x01
#define ORPHAN_WAIT_3 0x00

;DATA_REQ_WAIT is the time between sending a data request and its following response
#define DATA_REQ_WAIT_0 0x00
#define DATA_REQ_WAIT_1 0x00
#define DATA_REQ_WAIT_2 0x01
#define DATA_REQ_WAIT_3 0x00

;EUI address of the device
#define EUI_0 0x01
#define EUI_1 0x02
#define EUI_2 0x03
#define EUI_3 0x04
#define EUI_4 0x05
#define EUI_5 0x06
#define EUI_6 0x07
#define EUI_7 0x08

;Channels to scan for a network on
#define SCAN_CHANNEL_12

;Enable this define in order to support cluster sockets
#define ENABLE_CLUSTER_SOCKETS

;Enable this define in order to support EUI address searchs
#define ENABLE_EUI_ADDRESS_SEARCH

;Enable this define to be able to search the network table by short address
;#define ENABLE_SEARCH_NWK_TBL_BY_SHORT_ADDRESS

#endif
