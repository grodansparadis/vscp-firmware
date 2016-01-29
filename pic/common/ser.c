/*
 * Distributed by www.microchipC.com - one of the web's largest
 * repositories of C source code for Microchip PIC micros.
 *
 * ser.c
 *
 * Interrupt Driven Serial Routines with Circular FIFO
 * Copyright (c) 2006, Regulus Berdin
 * All rights reserved. 
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL REGULUS BERDIN BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
*/

#define _SER_C_
#include <pic.h>
#include "ser.h"

unsigned char rxfifo[SER_BUFFER_SIZE];
volatile unsigned char rxiptr, rxoptr;
bank1 unsigned char txfifo[SER_BUFFER_SIZE];
volatile unsigned char txiptr, txoptr;
unsigned char ser_tmp;

bit ser_isrx(void)
{
	if(OERR) {
		CREN = 0;
		CREN = 1;
		return 0;
	}
	return (rxiptr!=rxoptr);
}

unsigned char ser_getch(void)
{
	unsigned char c;

	while (ser_isrx()==0)
		continue;

	GIE=0;
	c=rxfifo[rxoptr];
	++rxoptr;
	rxoptr &= SER_FIFO_MASK;
	GIE=1;
	return c;
}

void ser_putch(unsigned char c)
{
	while (((txiptr+1) & SER_FIFO_MASK)==txoptr)
		continue;
	GIE=0;
	txfifo[txiptr] = c;
	txiptr=(txiptr+1) & SER_FIFO_MASK;
	TXIE=1;
	GIE=1;
}

void ser_puts(const unsigned char * s)
{
	while(*s)
		ser_putch(*s++);
}

void ser_puts2(unsigned char * s)
{
	while(*s)
		ser_putch(*s++);
}

void ser_puthex(unsigned char v)
{
	unsigned char c;

	c = v >> 4;
	if (c>9) {
		ser_putch('A'-10+c);
	} else {
		ser_putch('0'+c);
	}

	c = v & 0x0F;
	if (c>9) {
		ser_putch('A'-10+c);
	} else {
		ser_putch('0'+c);
	}
}


void ser_init(void)
{
	BRGH=1;					//high speed
//	SPBRG=25;				//9,600 @ 4MHz, SPBRG = (4MHz/(16*BAUD_RATE))-1;
//	SPBRG=12;				//19.2K @ 4MHz, SPBRG = (4MHz/(16*BAUD_RATE))-1;
	SPBRG=10;				//115.2K @ 20MHz, SPBRG = (20MHz/(16*BAUD_RATE))-1;

	TX9=0;					//8 bits
	RX9=0;					//

	SYNC=0;					//uart settings
	SPEN=1;
	CREN=1;
	TXIE=0;
	RCIE=1;
	TXEN=1;
	PEIE=1;

	rxiptr=rxoptr=txiptr=txoptr=0;
}

