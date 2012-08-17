/* 
 * serial.c --
 *
 *	Routines for communicating via the 16C550 serial port.
 *	Parts of this file are based on code by Marco Graziano.
 *
 * Copyright (c) 1996 Ben Williamson.
 * All rights reserved.
 *
 * This file is part of nslib, a library used by programs
 * running on the Netslate Development Board.
 *
 * This software is released under the GNU Public License.
 * See the file COPYING for more information.
 */

#include <nslib.h>

#define UART_REGS	0x0302B800		/* UART is on nPCCS1 */
#define UART_CLOCK	8000000			/* 8MHz clock */
#define UART_REF	(UART_CLOCK / 16)

#define RBUF_SIZE	0x10000			/* Hell, memory is cheap */
#define RBUF_SLACK	0x100

#define TBUF_SIZE	0x10
#define TBUF_SLACK	0x4


/*
 * UART registers:
 */

struct UART {
        union {
                ureg    RBR;    /* receive buffer register (R/O) */
                ureg    THR;    /* transmitter holding register (W/O) */
                ureg    DLL;    /* lsb divisor latch (R/W) */
        } Reg1;
        union {
                ureg    IER;    /* interrupt enable register (R/W) */
                ureg    DLM;    /* msb divisor latch (R/W) */
        } Reg2;
        union {
                ureg    IIR;    /* interrupt identification register (R/O) */
                ureg    FCR;    /* FIFO control register (W/O) */
        } Reg3;
        ureg    LCR;            /* line control register (R/W) */
        ureg    MCR;            /* modem control register (R/W) */
        ureg    LSR;            /* line status register (R/W) */
        ureg    MSR;            /* modem status register (R/W) */
        ureg    SCR;            /* scratch register (R/W) */
};

struct UART *UARTReg = (struct UART *)UART_REGS;

#define UART_RBR	(UARTReg->Reg1.RBR)
#define UART_THR	(UARTReg->Reg1.THR)
#define UART_IER	(UARTReg->Reg2.IER)
#define UART_IIR	(UARTReg->Reg3.IIR)
#define UART_FCR	(UARTReg->Reg3.FCR)
#define UART_LCR	(UARTReg->LCR)
#define UART_MCR	(UARTReg->MCR)
#define UART_LSR	(UARTReg->LSR)
#define UART_MSR	(UARTReg->MSR)
#define UART_SCR	(UARTReg->SCR)
#define UART_DLL	(UARTReg->Reg1.DLL)
#define UART_DLM	(UARTReg->Reg2.DLM)


typedef struct {
	volatile char	buf[RBUF_SIZE];		/* XXX */
	volatile int	head;
	volatile int	tail;
	volatile int	count;
} Buffer;

/*
 * Circular send and receive buffers:
 */
 
static Buffer	sendBuf, recvBuf;



/*
 *----------------------------------------------------------------------
 *
 * SerialInit --
 *
 *	Initialises the serial port and sets up support data.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The serial port is initialised.  SerialIsr is installed as
 *	the interrupt handler for INT5, the 16C550 interrupt source.
 *
 *----------------------------------------------------------------------
 */

void
SerialInit(int baud)
{
	int dummy;

	/*
	 * Clear the send and receive buffers
	 */
	 
	sendBuf.head = sendBuf.tail = sendBuf.count = 0;
	recvBuf.head = recvBuf.tail = recvBuf.count = 0;

	/*
	 * Install the ISR
	 */

	IrqInstall(IRQ_INT2, SerialIsr);

	/*
	 * Configure the UART
	 */

	UART_IER = 0x00;	/* disable all interrupts */
	UART_LCR = 0x80;	/* enable divisor latch access */
	UART_DLL = LOBYTE(UART_REF / baud);
	UART_DLM = HIBYTE(UART_REF / baud);
	UART_LCR = 0x03;	/* disable divisor latch, set 8 bit data */
	UART_FCR = 0x00;	/* disable FIFO */
	UART_MCR = 0x03;	/* 2=RTS 1=DTR */

        dummy = UART_LSR;
        dummy = UART_RBR;
        dummy = UART_IIR;
        dummy = UART_MSR;

	UART_IER = 0x01;	/* 1=recv 2=trans 4=line 8=modem */
}

/*
 *----------------------------------------------------------------------
 *
 * SerialSend --
 *
 *	Send a buffer over the serial port.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The buffer buf of length len is added to the transmit buffer.
 *	If there is insufficient space in the buffer, this function
 *	blocks until space becomes available.  The SerialIsr routine
 *	removes bytes from the transmit buffer as they are transmitted.
 *
 *----------------------------------------------------------------------
 */

void
SerialSend(char ch)
{
	/* XXX POLLING: */
	int i;
	ureg lsr;

	do {
		lsr = UART_LSR;
	} while (!(lsr & 0x20));

	for (i = 0; i < 5000; i++) {
		/* NULL */
	} 
	UART_THR = ch;
	return;

#if 0
	if (UART_LSR & 0x20) {
		UART_THR = ch;
		return;
	}
	while (sendBuf.count >= TBUF_SIZE - TBUF_SLACK) {
		Suspend();
	}
#ifdef SAFE
	IntDisable();
#endif
	sendBuf.buf[sendBuf.head] = ch;
	sendBuf.head++;
	if (sendBuf.head >= TBUF_SIZE) {
		sendBuf.head = 0;
	}
	sendBuf.count++;
	UART_IER |= 0x02;	/* enable transmit interrupt */
#ifdef SAFE
	IntEnable();
#endif
#endif
}

/*
 *----------------------------------------------------------------------
 *
 * SerialRecv --
 *
 *	Receive a byte from the serial port.
 *
 * Results:
 *	Returns the byte read.
 *
 * Side effects:
 *	The byte read is removed from the receive buffer.  If there
 *	is no byte waiting, this functions blocks until one is
 *	received.
 *
 *----------------------------------------------------------------------
 */

char
SerialRecv(void)
{
	char retval;

	IntDisable();
	while (recvBuf.count <= 0) {
		IntEnable();
		Suspend();
		IntDisable();
	}
	IntEnable();
	retval = recvBuf.buf[recvBuf.tail];
#ifdef SAFE
	IntDisable();
#endif
	recvBuf.tail++;
	if (recvBuf.tail >= RBUF_SIZE) {
		recvBuf.tail = 0;
	}
	recvBuf.count--;
#ifdef SAFE
	IntEnable();
#endif
	if (recvBuf.count < RBUF_SIZE - RBUF_SLACK) {
		UART_MCR |= 0x02;		/* raise RTS */
	}

	return retval;
}

/*
 *----------------------------------------------------------------------
 *
 * SerialReady --
 *
 *	Check how many bytes are waiting in the serial receive buffer.
 *
 * Results:
 *	Returns the number of bytes in the serial receive buffer.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
SerialReady(void)
{
	return recvBuf.count;
}

/*
 *----------------------------------------------------------------------
 *
 * SerialIsr --
 *
 *	Serial port interrupt service routine.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Examines the IIR and performs the appropriate action to
 *	clear the interrupt.
 *
 *----------------------------------------------------------------------
 */

void
SerialIsr(void)
{
	int cause;
	char temp;

	IRQMSKA &= ~0x01;		/* Mask off INT2 */
	IRQRQA = 0x01;			/* Ack INT2 */
	do {
		cause = UART_IIR & 0x07;
		switch (cause) {
		    case 0x04:
			/*
			 * Received data ready
			 */
			temp = UART_RBR;
			if (recvBuf.count < RBUF_SIZE - 1) {
				recvBuf.buf[recvBuf.head] = temp;
				recvBuf.head++;
				if (recvBuf.head >= RBUF_SIZE) {
					recvBuf.head = 0;
				}
				recvBuf.count++;
			}
			if (recvBuf.count >= RBUF_SIZE - RBUF_SLACK) {
				UART_MCR &= ~0x02;	/* lower RTS */
			}
		    	break;
		    case 0x02:
			/*
			 * Ready to transmit
			 */
			UART_THR = sendBuf.buf[sendBuf.tail];
			sendBuf.tail++;
			if (sendBuf.tail >= TBUF_SIZE) {
				sendBuf.tail = 0;
			}
			sendBuf.count--;
			if (sendBuf.count <= 0) {
				UART_IER &= ~0x02;	/* no more THRE interrupts */
			}
		    	break;
#ifdef DEBUG
		    default:
			putchar('!');
		    	/* Fixme: If we get here, we could be here for ever */
#endif
		}
	} while (!(cause & 0x01));
	IRQMSKA |= 0x01;		/* Unmask INT2 */
}



/*
A very solid method of handling the UART interrupts that avoids all possible
int failures has been suggested by Richard Clayton, and I recommend it as
well. Let your interrupt handler do the following:

    1.	Disarm the UART interrupts by masking them in the IMR of the ICU. 
    2.	Send a specific or an unspecific EOI to the ICU (first slave, then
	master, if you're using channels above 7). 
    3.	Enable CPU interrupts (STI) to allow high priority ints to be processed. 
    4.	Read IIR and follow its contents until bit 0 is set. 
    5.	Check if transmission is to be kicked (when XON received or if CTS goes
	high); if yes, call tx interrupt handler manually. 
    6.	Disable CPU interrupts (CLI). 
    7.	Rearm the UART interrupts by unmasking them in the IMR of the ICU. 
    8.	Return from interrupt. 


This way you can arm all four UART ints at initialization time without
having to worry about stuck interrupts.  Start transmission by simply
calling the tx interrupt handler after you've written characters to the
tx fifo of your program.
*/
