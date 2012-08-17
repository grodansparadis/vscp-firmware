//////////////////////////////////////////////////////////////////////////////
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version
// 2 of the License, or (at your option) any later version.
// canald.cpp 
//
// This file is part of the VSCP/firmware (http://sourceforge.net/projects/m2m) 
//
// Copyright (C) 2009 Thomas Schulz, tschulz@iprimus.com.au
// 
// This file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this file see the file COPYING.  If not, write to
// the Free Software Foundation, 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
/////////////////////////////////////////////////////////////////////////////


/**********************************************************
Module              : Olimex AT90CAN128 Development Board
Chip type           : AT90CAN128
Program type        : Application
Clock frequency     : 16 MHz
Memory model        : Small
Internal SRAM size  : 4096
External SRAM size  : 0
Data Stack size     : 1024
**********************************************************/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "can_at90can128.h"
#include "vscp_compiler.h"
#include "vscp_projdefs.h"

#include "uart0_int.h"


#define LED_DEVBOARD   4
#define BTN_DEVBOARD   5
#define LED2_DEVBOARD  7

#define LED_ON       ((PORTE &= ~_BV(LED_DEVBOARD)))
#define LED_OFF      ((PORTE |=  _BV(LED_DEVBOARD)))
#define LED_TOGGLE   ((PORTE ^=  _BV(LED_DEVBOARD)))
#define BTN_PRESSED  (!(PINE &   _BV(BTN_DEVBOARD)))
#define LED2_ON      ((PORTE &= ~_BV(LED2_DEVBOARD)))
#define LED2_OFF     ((PORTE |=  _BV(LED2_DEVBOARD)))
#define LED2_TOGGLE  ((PORTE ^=  _BV(LED2_DEVBOARD)))


volatile uint8_t btn_devboard;
volatile uint16_t time_stamp;


// Local prototypes
void can_to_serial( CANMsg* );
uint8_t buf_to_can( CANMsg* );
int8_t process_cmd( char );


/* setup stdio using UART0 (MCU) */
int
uart_putchar(char c, FILE *stream)
{
    if (c == '\n')
        uart0_putchar('\r');
    uart0_putchar(c);
    return 0;
}


int
uart_getchar(FILE *stream)
{
    return uart0_getchar();
}

static FILE uart0_stdout  = FDEV_SETUP_STREAM(uart_putchar, NULL,
                                              _FDEV_SETUP_WRITE);

static FILE uart0_stdin   = FDEV_SETUP_STREAM(NULL, uart_getchar,
                                              _FDEV_SETUP_READ);


void
timer0_init()
{
    /* This is an 8 bit counter */
    TCCR0A  = 0x5;           // Pre-scaler (1024)
    TIMSK0  |= _BV(TOIE0);   // Enable overflow interrupt
}


ISR(TIMER0_OVF_vect)
{
    //time_stamp++;
}


void
timer1_init()
{
    /* 16 bit counter */
    TCCR1B  = 0x4;           // Pre-scaler (3->64, 4->256, 5->1024)
    TIMSK1 |= _BV(TOIE1);    // Enable overflow interrupt
}


void
io_init()
{
    PORTA   = 0xff;       // Activate pull-ups
    DDRA    = 0x00;       // Inputs

    PORTB   = 0xff;       // Activate pull-ups
    DDRB    = 0x00;       // Inputs

    PORTC   = 0xff;       // Activate pull-ups
    DDRC    = 0x00;       // Inputs

    PORTD   = 0b10010000; // Activate some pull-ups
    DDRD    = 0x00;       // Inputs

    PORTE   = 0x00;       // LEDs on
    DDRE    = 0b10010010; // LED2 output, LED1 output, TX output

    PORTF   = 0xff;       // Activate pull-ups
    DDRF    = 0x00;       // Inputs

    PORTG   = 0xff;       // Activate pull-ups
    DDRG    = 0x00;       // Inputs
}


/* Activate interrupt for the button on the development board. */
void
button_init()
{
    EICRB |= 0b00001000;          // Interrupt on falling edge (INT5)
    EIMSK |= _BV(BTN_DEVBOARD);   // Enable interrupt for button
}


/* Interrupt handler for button on development board.
   Attention: no de-bouning implemented yet. */
ISR(INT5_vect)
{
    btn_devboard = 1;
}


void
init()
{
    io_init();

    //timer0_init();
    //timer1_init();

    // Initialize UART
    uart0_init();
    stdout = &uart0_stdout;
    stdin  = &uart0_stdin;

    button_init();

    // Init can
    if ( ERROR_OK != can_Open( CAN_BITRATE_125K, 0, 0, 0 ) ) {
        printf("Failed to open channel!!\n");
    }
}


#define REC_NONE 0
#define REC_MSG  1
#define REC_BUF_LEN  32
char rec_buf[REC_BUF_LEN];
uint8_t state = REC_NONE;
uint8_t rec_buf_count;



/* Convert a message in rec_buf into a can message structure. */
uint8_t
buf_to_can(CANMsg *p_msg)
{
    uint8_t data_offset;

    if (rec_buf[0] == 't') {

        p_msg->flags = CAN_IDFLAG_STANDARD;
        data_offset = 5;
        p_msg->len = rec_buf[4] - '0';
        rec_buf[4] = 0;
        sscanf(rec_buf+1, "%lx", &p_msg->id);
    }
    else if (rec_buf[0] == 'T') {

        p_msg->flags = CAN_IDFLAG_EXTENDED;
        data_offset = 10;
        p_msg->len = rec_buf[9] - '0';
        rec_buf[9] = 0;
        sscanf(rec_buf+1, "%lx", &p_msg->id);
    }
    else
        return FALSE;

    // Data
    for (uint8_t i=p_msg->len; i>0; i--) {
        unsigned int val;
        *(rec_buf + data_offset + 2*(i-1) + 2) = 0;
        sscanf(rec_buf + data_offset + 2*(i-1), "%x", &val);
        p_msg->byte[i-1] = val;
    }

    return TRUE;
}


int8_t
process_cmd(char cmd)
{
    if ((cmd == 't') || (cmd == 'T')) {
        state = REC_MSG;
        rec_buf_count = 0;
    }

    if (state == REC_MSG) {

        // May test for 0-9,a-f,A-F

        rec_buf[rec_buf_count++] = cmd;

        if (rec_buf_count == REC_BUF_LEN) {
            // Too long, finish recording
            rec_buf_count = 0;
            state = REC_NONE;
            rec_buf[0] = 0;
            return 0;
        }

        if ((cmd == 0xd) || (cmd == 0)) {
            // send message to CAN
            CANMsg msg;
            uint8_t rv;

            if (buf_to_can(&msg)) {
                printf_P(PSTR("\r"));   // can232drv needs this response
                if ((rv = can_SendFrame(&msg)) == ERROR_OK) {
                }
            }
            state = REC_NONE;
            rec_buf[0] = 0;
        }
        return 0;
    }

    if (cmd >= 0x30)
        LED_TOGGLE;

    if (cmd == 'A') {
        // Poll? Not sure what this does do.
        printf_P(PSTR("\r"));
        return 0;
    }

    if (cmd == 'C') {
        // Clear buffers.
        printf_P(PSTR("\r"));
        return 0;
    }

    if (cmd == 'V') {
        // Return version
        printf_P(PSTR("V%04d\r"), 500);
        return 0;
    }

    if (cmd == 's') {
        // Set btr0, btr1
        printf_P(PSTR("\r"));
        return 0;
    }

    if (cmd == 'S') {
        // Speed
        printf_P(PSTR("\r"));
        return 0;
    }

    if (cmd == 'O') {
        // Open
        printf_P(PSTR("\r"));
        return 0;
    }

    if (cmd == 'F') {
        // Status
        printf_P(PSTR("%2x\r"), 0);
        return 0;
    }

    if (cmd == 'X') {
        // Reset
        asm("jmp 0");
    }

    return 1;
}


void
can_to_serial(CANMsg *p_canMsg)
{
    char buf[80];

    // Message ID, length (flags encoded in first character)
    if (p_canMsg->flags == CAN_IDFLAG_STANDARD) {

        // Standard frame
        sprintf(buf, "t%03lX%i", p_canMsg->id, p_canMsg->len);
    }
    else if (p_canMsg->flags == CAN_IDFLAG_RTR) {

        // Standard remote frame
        sprintf(buf, "r%03lX%i", p_canMsg->id, 0);
    }
    else if (p_canMsg->flags == CAN_IDFLAG_EXTENDED) {

        // Extended frame
        sprintf(buf, "T%08lX%i", p_canMsg->id, p_canMsg->len);
    }
    else if (p_canMsg->flags == (CAN_IDFLAG_EXTENDED | CAN_IDFLAG_RTR)) {

        // Extended remote frame
        sprintf(buf, "R%08lX%i", p_canMsg->id, 0);
    }

    // Data
    char hex[5];
    for (uint8_t i=0; i< p_canMsg->len; i++) {
        sprintf(hex, "%02X", p_canMsg->byte[i]);
        strcat(buf, hex);
    }

    // Time stamp
    sprintf(hex, "%04x", time_stamp);
    strcat(buf, hex);

    puts(buf);
}


int
main (void)
{
    init();
    sei();

    printf_P(PSTR("\n\n*** Olimex can232 ***\n"));
    LED_OFF;
    LED2_OFF;

    CANMsg can_msg;
    while (1) {

        if (can_readFrame(&can_msg) == ERROR_OK) {
            time_stamp++;
	    LED2_TOGGLE;
            can_to_serial(&can_msg);
        }

        extern volatile uint8_t rx_counter0;
        if (rx_counter0) {
            char c;
            c = getchar();
            process_cmd(c);
        }

        if (btn_devboard) {

            btn_devboard = 0;

            CANMsg msg;
            msg.id = 0x1f << 8 | 0x0;    // Who-is-there?
            msg.flags = 1;               // Extended can frame
            msg.len = 1;                 // One data byte
            msg.byte[0] = 0xff;          // Query all nodes

            can_SendFrame(&msg);
        }
    }

    return(0);
}
