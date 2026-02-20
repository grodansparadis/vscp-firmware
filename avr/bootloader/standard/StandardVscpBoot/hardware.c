///////////////////////////////////////////////////////////////////////////////
// File: hardware.c
// 
// Hardware dependent implementions in this file.
//

/* ******************************************************************************
 * VSCP (Very Simple Control Protocol) 
 * http://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2010-2016 Sven Sven Zwiers
 * Copyright (c) 2016-2026 Ake Hedman, the VSCP Project <info@grodansparadis.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sub license, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 *	This file is part of VSCP - Very Simple Control Protocol 	
 *	http://www.vscp.org
 *
 * ******************************************************************************
 */

// all hardware related routines should be placed here
#include "vscp_firmware.h"
#include "vscp_compiler.h"
#include "vscp_projdefs.h"
#include "hardware.h"

///////////////////////////////////////////////////////////////////////////////
//                  From here implemention specific
///////////////////////////////////////////////////////////////////////////////
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/boot.h>
#include <avr/wdt.h>
#include "can_at90can128.h"
#include "uart.h"

extern uint16_t vscpboot_timer;



///////////////////////////////////////////////////////////////////////////////
// Timer 0 Compare interrupt
//
// Once every millisecond
//

SIGNAL( SIG_OUTPUT_COMPARE0 )
{
	// handle millisecond counter
	vscpboot_timer++;
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

void init_hardware( void )
{

#ifndef VSCPBOOT_BOARD
#error You must define the used VSCPBOOT_BOARD in vscp_projdefs.h or in Makefile
#elif ( VSCPBOOT_BOARD == ATMEL_STK501 )

    PORTA   = 0xff;     // Activate pull-ups
    DDRA    = 0x00;     // Inputs

    PORTB   = 0x00;		// All LEDs on
    DDRB    = 0xFF;		// all output

    PORTC   = 0x00;
    DDRC    = 0x00;

    PORTD   = 0x00;
    DDRD    = 0x20;

    PORTE   = 0x00;     // LED on
    DDRE    = 0xDF;     // all output except button

    PORTF   = 0x00;
    DDRF    = 0x00;

    PORTG   = 0x00;
    DDRG    = 0x00;

#elif ( VSCPBOOT_BOARD == OLIMEX_AT90CAN128 )

    PORTA   = 0xff;     // Activate pull-ups
    DDRA    = 0x00;     // Inputs

    PORTB   = 0x00;		// All LEDs on
    DDRB    = 0xFF;		// all output

    PORTC   = 0x00;
    DDRC    = 0x00;

    PORTD   = 0x00;
    DDRD    = 0x20;

    PORTE   = 0x00;     // LED on
    DDRE    = 0xDF;     // all output except button

    PORTF   = 0x00;
    DDRF    = 0x00;

    PORTG   = 0x00;
    DDRG    = 0x00;

#elif ( VSCPBOOT_BOARD == AAMINI00 )

    PORTA   = 0xff;     // Activate pull-ups
    DDRA    = 0x00;     // Inputs K1

    PORTB   = 0xFF;		// all leds off
    DDRB    = 0xFF;		// all output K2

    PORTC   = 0x00;	    // below not used for now
    DDRC    = 0x00;

    PORTD   = 0x00;
    DDRD    = 0x00;

    PORTE   = 0x00;
    DDRE    = 0x00;

    PORTF   = 0x00;
    DDRF    = 0x00;

    PORTG   = 0x00;
    DDRG    = 0x00;

#else
#error This board is currently not supported, add it to hardware.c
#endif

    // Initialize timer
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
    
    // Enable timer0 compare interrupt
    TIMSK0 = ( 1 << OCIE0A );

    // Initialize UART needed for debug messages
#ifdef PRINT_DEBUG_EVENTS
    UCSRA = 0;
    UCSRC = MSK_UART_8BIT;	// 8N1
    Uart_set_baudrate( 9600 );
    UCSRB = MSK_UART_ENABLE_TX | MSK_UART_ENABLE_RX;
#endif

    // Initialize CAN
    if ( ERROR_OK != can_Open( CAN_BITRATE_125K, 0, 0, 0 ) ) {
#ifdef PRINT_DEBUG_EVENTS
        uart_puts("CAN Failed!!\n");
#endif
    }

}


///////////////////////////////////////////////////////////////////////////////
//   			     Bootloader requires callback Functions
//					 -----------------------------
//
// All methods below must be implemented to handle bootloader functionality
//
///////////////////////////////////////////////////////////////////////////////

uint8_t vscpboot_readInitButton( void )
{
    return ( PORTA & 1 );
}

///////////////////////////////////////////////////////////////////////////////
// vscpboot_getBootFlag
//

uint8_t vscpboot_getBootFlag( void )
{
    return 0xff;    
}

///////////////////////////////////////////////////////////////////////////////
// vscpboot_setBootFlag
//

void vscpboot_setBootFlag( uint8_t bootflag )
{
    ;
}


uint8_t vscpboot_getNickname( void )
{
    return 0xFE;
}    


void vscpboot_setNickname( uint8_t nickname )
{
    ;    
}

///////////////////////////////////////////////////////////////////////////////
// boot_program_page
//

void boot_program_page (uint32_t page, uint8_t *buf)
{
    uint16_t i;
    uint8_t sreg;

    // Disable interrupts.
    sreg = SREG;
//        cli();
    
    //eeprom_busy_wait();

    boot_page_erase( page );
    boot_spm_busy_wait();      // Wait until the memory is erased.

    for ( i=0; i<(VSCPBOOT_BLOCKSIZE-1); i+=2 ) {
            
        // Set up little-endian word.

        uint16_t w = *buf++;
        w += (*buf++) << 8;
        
        boot_page_fill (page + i, w);
//		boot_page_fill (i, 0x1123);
    }

    boot_page_write (page);     // Store buffer in flash page.
    boot_spm_busy_wait();       // Wait until the memory is written.

    // Reenable RWW-section again. We need this if we want to jump back
    // to the application after bootloading.

    boot_rww_enable ();

    // Re-enable interrupts (if they were ever enabled).
    SREG = sreg;
}



/*
int vscpboot_sendEvent( vscpevent *pEvent )
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
}*/


/*int vscpboot_sendEvent( vscpevent *pEvent )
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
}*/


///////////////////////////////////////////////////////////////////////////////
// vscpboot_sendEvent
//

int8_t vscpboot_sendEvent( vscpevent *pmsg )
{
    int8_t rv;

    /*
    if (!(rv = sendVSCPFrame( pmsg->vscp_class,
                                pmsg->vscp_type,
                                vscp_nickname,
                                mpsg->priority,
                                (pmsg->flags & 0x0f),
                                pmsg->data))) {
      
    }  */

    return rv;
}

///////////////////////////////////////////////////////////////////////////////
// vscpboot_getEvent
//

int8_t vscpboot_getEvent( vscpevent *pmsg )
{
    int8_t rv;

    /*
    if ( ( rv = getVSCPFrame( msg.vscp_class,
                                &vscp_imsg.vscp_type,
                                &vscp_imsg.oaddr,
                                &vscp_imsg.priority,
                                &vscp_imsg.flags,
                                vscp_imsg.data ) ) ) {

        pmsg->flags |= VSCP_VALID_MSG;
    }
    */

    return rv;

}

		

