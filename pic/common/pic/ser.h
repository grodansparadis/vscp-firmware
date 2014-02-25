/*
 * ser.h
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
*/

#ifndef _SER_H_
#define _SER_H_

/* Valid buffer size value are only power of 2 (ex: 2,4,..,64,128) */
#define SER_BUFFER_SIZE		16
		
#define SER_FIFO_MASK 		(SER_BUFFER_SIZE-1)

/* Insert this macro inside the interrupt routine */
#define ser_int() 							\
	if (RCIF) {								\
		rxfifo[rxiptr]=RCREG;				\
		ser_tmp=(rxiptr+1) & SER_FIFO_MASK;	\
		if (ser_tmp!=rxoptr)				\
			rxiptr=ser_tmp;					\
	}										\
	if (TXIF && TXIE) {						\
		TXREG = txfifo[txoptr];				\
		++txoptr;							\
		txoptr &= SER_FIFO_MASK;			\
		if (txoptr==txiptr) {				\
			TXIE = 0;						\
		}									\
	}

bit ser_isrx(void);
unsigned char ser_getch(void);
void ser_putch(unsigned char byte);
void ser_puts(const char * s);
void ser_puts2(unsigned char * s);
void ser_puthex(unsigned char v);
void ser_init(void);

#ifndef _SER_C_
extern unsigned char rxfifo[SER_BUFFER_SIZE];
extern volatile unsigned char rxiptr, rxoptr;
extern bank1 unsigned char txfifo[SER_BUFFER_SIZE];
extern volatile unsigned char txiptr, txoptr;
extern unsigned char ser_tmp;
#endif

#endif
