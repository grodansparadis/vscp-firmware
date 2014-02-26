/******************************************************************************
*
* ACS SOFTWARE
*
*------------------------------------------------------------------------------
* Copyright(c)2003-2014 by ACS, Sarasota, FL. ALL RIGHTS RESERVED.
*
*	This Information is Proprietary to Ackerman Computer Sciences, Sarasota,
* Florida (ACS) and MAY NOT be copied by any method or incorporated into another
* program without the express written consent of ACS. This Information or any
* portion thereof remains the property of ACS. The Information contained herein
* is believed to be accurate and ACS assumeS no responsibility or liability for
* its use in any way and conveys no license or title under any patent or 
* copyright and makes no representation or warranty that this Information is
* free from patent or copyright infringement.
*------------------------------------------------------------------------------
* Uart.c - ACS R8CM12A Uart Implementation
*------------------------------------------------------------------------------
* written by:
*	Steven J. Ackerman
*	Consultant
*	ACS
*	6233 E. Sawgrass Rd.
*	Sarasota, FL 34240
*
* date		Comments
* --------------------------
* 10-23-03	First written.
*  3-30-11	Changes for use with R8CM12A instead of z8F.
*  2-10-12	Changes for use in R8CM12A Ps2Pwr57 project. Made TXD0 (p4_2) open
*			drain in UART_Init();
*******************************************************************************
*/

#include "SerialIO.h"
#include "Uart.h"

/* UART */

/* uncomment one of the following to select UART BRG clock source */
#define	UART_BRG_CLK_F1		/* UART BRG clock source is (CPU_CLOCK / 1)*/
//#define	UART_BRG_CLK_F8		/* UART BRG clock source is (CPU_CLOCK / 8)*/
//#define	UART_BRG_CLK_F32	/* UART BRG clock source is (CPU_CLOCK / 32)*/

#ifdef	UART_BRG_CLK_F1
	#define	UART_BRG	((CPU_CLOCK / (UART_BAUD_RATE * 16L)) - 1)
	#if (UART_BRG > 255L)
		#error UART_BRG overflow - try UART_BRG_CLK_F8 !
	#endif
#endif

#ifdef	UART_BRG_CLK_F8
	#define	UART_BRG	(((CPU_CLOCK / 8L) / (UART_BAUD_RATE * 16L)) - 1)
	#if (UART_BRG > 255L)
		#error UART_BRG overflow - try UART_BRG_CLK_F32 !
	#endif
#endif

#ifdef	UART_BRG_CLK_F32
	#define	UART_BRG	(((CPU_CLOCK / 32L) / (UART_BAUD_RATE * 16L)) - 1)
	#if (UART_BRG > 255L)
		#error UART_BRG overflow - examine UART_BAUD_RATE and CPU_CLOCK values !
	#endif
#endif

#ifndef	UART_BRG
	#error No UART_BRG_CLK_Fx value defined !
#endif

/*  R8CM12A UART  registers
	-----------------------
	u0mr 
		(7) = 0
		(6) PRYE:  1 = enable parity
		(5) PRY:   1 = Even parity
		(4) STPS:  1 = 2 stop bits
		(3) CKDIR: 1 = external clock, 0 = internal clock
		(2..0)   0 = Serial interface disabled
			1 = Clocked synchronous mode
			2 = reserved
			3 = reserved
			4 = 7 bit UART
			5 = 8 bit UART
			6 = 9 bit UART
			7 = reserved

	u0brg (7..0) = divide clock by n+1;
	
	u0tb = transmit buffer u0tbh + u0tbl for byte access
	
	u0c0 tx/rx control register 0
		(7) UFORM:   0 = LSB first, 1 = MSB first
		(6) CKPOL:   0 = output on falling edge,  1 = output on rising edge
		(5) NCH:     0 = CMOS output, 1 = N-channel open drain
		(4) DFE:     0 = disable digital filter,  1 = enable digital filter
		(3) TXEPT:   0 = transmission in progress, 1 = transmission complete
		(2) reserved
		(1..0)       00 = f1
			01 = f8
			10 = f32
			11 = reserved
			
	u0c1 tx/rx control register 1   (0x85)
		(7..6) reserved
		(5) U0RRM:  1 = enable continuous receive mode, 0 = disable continuous mode
		(4) U0IRS:  0 = interrupt when tx buffer is empty, 1 = interrupt when tx is complete
		(3) RI:     1 = Data in the U0RB register
		(2) RE:     1 = Reception enabled, 0 = reception disabled
		(1) TI:     0 = data in the U0TB register, 1 = the register is empty
		(0) TE:     1 = transmission enabled

	u0rb UART0 receive buffer  =  u0rbh+u0rbl
	
	u0rbh (7) SUM:  Sum error flag
		(6) PER:  parity error
		(5) FER:  framing error
		(4) OER:  overrun error
		(3..1)  reserved
		(0) bit 9 of 9-bit data
	
	u0rbl (7..0) = receive data

	u0ir UART0 interrupt flag
		(7) U0TIF:    1 = Transmission interrupt requested  (write a 0 to clear the bit)
		(6) U0RIF:    1 = receive interrupt requested       (write a 0 to clear the bit)
		(5..4) nothing assigned to these bits
		(3) U0TIE:    1 = transmission interrupt enabled
		(2) U0RIE:    1 = receive interrupt enabled
		(1..0) nothing assigned
*/

#ifdef	UART_IMPLEMENT_RX

/* interrupts */
#define	ENABLE_UART_RX_INTERRUPTS()		u0rie = 1
#define	DISABLE_UART_RX_INTERRUPTS()	u0rie = 0

/* compile-time determination of serial pointer size */
#if (MAX_UART_RX_BUFFER >= 256)
	#define UART_RX_PTR_TYPE	UINT
#else
	#define UART_RX_PTR_TYPE	BYTE
#endif

#define	UART_RX_PTR_MASK	((UART_RX_PTR_TYPE)(MAX_UART_RX_BUFFER - 1))

// UartRxQueueBuf indices range from 0 to UART_RX_PTR_MASK
UART_RX_PTR_TYPE UartRxQueueCount, UartRxQueueBufEnd, UartRxQueueBufStart;
BYTE UartRxQueueBuf[MAX_UART_RX_BUFFER];

/*
 *	// Rx Macro Usage:
 *	BYTE	c;
 *
 *	UART_RX_QUEUE_INIT();
 *
 *	// place value into queue...
 *	c = value;
 *	ES = 0;
 *	if (!UART_RX_QUEUE_FULL())
 *	{
 *		UART_RX_QUEUE_PUT(&c);
 *	}
 *	ES = 1;
 *
 *	...
 *
 *	// remove value from queue...
 *	ES = 0;
 *	if (!UART_RX_QUEUE_EMPTY())
 *	{
 *		UART_RX_QUEUE_GET(&c);
 *	}
 *	ES = 1;
*/

// reset queue start and end pointers
#define	UART_RX_QUEUE_INIT()	UartRxQueueBufStart = 0; \
								UartRxQueueBufEnd = 0; \
								UartRxQueueCount = 0

// true if queue count == max
#define	UART_RX_QUEUE_FULL()	(UartRxQueueCount == (UART_RX_PTR_TYPE)MAX_UART_RX_BUFFER)

// queue[end] = @x, end = end+1
#define	UART_RX_QUEUE_PUT(x)	UartRxQueueBuf[UartRxQueueBufEnd++] = *x;	\
								UartRxQueueBufEnd &= UART_RX_PTR_MASK; \
								UartRxQueueCount++

// true if queue count == 0
#define	UART_RX_QUEUE_EMPTY()	(UartRxQueueCount == 0)

// @x = queue[start], start = start+1
#define	UART_RX_QUEUE_GET(x)	*x = UartRxQueueBuf[UartRxQueueBufStart++]; \
								UartRxQueueBufStart &= UART_RX_PTR_MASK; \
								UartRxQueueCount--

#endif	/* UART_IMPLEMENT_RX */

#ifdef	UART_IMPLEMENT_TX

/* interrupts */
#define	ENABLE_UART_TX_INTERRUPTS()		u0tie = 1
#define	DISABLE_UART_TX_INTERRUPTS()	u0tie = 0

/* compile-time determination of serial pointer size */
#if (MAX_UART_TX_BUFFER >= 256)
	#define UART_TX_PTR_TYPE	UINT
#else
	#define UART_TX_PTR_TYPE	BYTE
#endif

#define	UART_TX_PTR_MASK	((UART_TX_PTR_TYPE)(MAX_UART_TX_BUFFER - 1))

// Uart0TxQueueBuf indices range from 0 to UART_TX_PTR_MASK
UART_TX_PTR_TYPE UartTxQueueCount, UartTxQueueBufEnd, UartTxQueueBufStart;
BYTE UartTxQueueBuf[MAX_UART_TX_BUFFER];

/*
 * // Tx Macro Usage:
 *	BYTE	c;
 *
 *	UART_TX_QUEUE_INIT();
 *
 *	// place value into queue...
 *	c = value;
 *	ES = 0;
 *	if (!UART_TX_QUEUE_FULL())
 *	{
 *		UART_TX_QUEUE_PUT(&c);
 *	}
 *	ES = 1;
 *
 *	...
 *
 *	// remove value from queue...
 *	ES = 0;
 *	if (!UART_TX_QUEUE_EMPTY())
 *	{
 *		UART_TX_QUEUE_GET(&c);
 *	}
 *	ES = 1;
*/

// reset queue start and end pointers
#define	UART_TX_QUEUE_INIT()	UartTxQueueBufStart = 0; \
								UartTxQueueBufEnd = 0; \
								UartTxQueueCount = 0

// true if queue count == max
#define	UART_TX_QUEUE_FULL()	(UartTxQueueCount == (UART_TX_PTR_TYPE)MAX_UART_TX_BUFFER)

// queue[end] = @x, end = end+1
#define	UART_TX_QUEUE_PUT(x)	UartTxQueueBuf[UartTxQueueBufEnd++] = *x;	\
								UartTxQueueBufEnd &= UART_TX_PTR_MASK; \
								UartTxQueueCount++

// true if queue count == 0
#define	UART_TX_QUEUE_EMPTY()	(UartTxQueueCount == 0)

// @x = queue[start], start = start+1
#define	UART_TX_QUEUE_GET(x)	*x = UartTxQueueBuf[UartTxQueueBufStart++]; \
								UartTxQueueBufStart &= UART_TX_PTR_MASK; \
								UartTxQueueCount--

#endif	/* UART_IMPLEMENT_TX */

/////////////////////////////////////////////////////////////////////////////
// uart0 interrupts	(software ints 17 & 18)
// uart0 trance			(software int 17)
#pragma interrupt	_uart0_trance(vect=17)
void _uart0_trance(void);
void _uart0_trance(void)
{
	BYTE	c;

#ifdef	UART_IMPLEMENT_TX
	
	/* Q - transmit interrupt ? */
	if (u0tif)
	{
		/* yes - clear the interrupt flag */
		u0tif = 0;
		
		/* Q - anything to send ? */
		if (!UART_TX_QUEUE_EMPTY())
		{
			/* yes - unqueue tx data */
			UART_TX_QUEUE_GET(&c);
	
			/* write data to uart tx */
			u0tbh = 0;
			u0tbl = c;
		}
		
#ifdef	RS485_MODE_ENABLED

		else
		{
			/* no - Q - is this the transmission complete interrupt ? */
			if (u0irs_u0c1 == 1)
			{
				/* yes - disable RS485 and complete interrupt */
				RS485_DRIVER_DISABLE();
				//u0irs_u0c1 = 0;
			}
			else
			{
				/* no - set next interrupt for transmission complete */
				u0irs_u0c1 = 1;
			}
		}

#endif

	}
	
#endif

}

#pragma interrupt	_uart0_receive(vect=18)
void _uart0_receive(void);
void _uart0_receive(void)
{
	BYTE	c;
	USHORT	rx;
	
#ifdef	UART_IMPLEMENT_RX

	/* Q - received character ? */
	if (u0rif)
	{
		/* yes - retrieve it */
		rx = u0rb;
		c = (BYTE)rx;
		//u0ir = u0ir & 0x04; // clear the interrupt flag
		u0rif = 0;
		
		/* Q - rx queue full ? */
		if (!UART_RX_QUEUE_FULL())
		{
			/* no - queue the received data */
			UART_RX_QUEUE_PUT(&c);
		}
	}

#endif

}

/////////////////////////////////////////////////////////////////////////////
/* initialize UART */
void UART_Init(BOOLEAN enableTransmitter, BOOLEAN enableReceiver)
{
	/* Turn off maskable interrupts */
	DISABLE_INTERRUPTS();

	/* disable uart interrupts */
#ifdef	UART_IMPLEMENT_RX
	DISABLE_UART_RX_INTERRUPTS();
#endif

#ifdef	UART_IMPLEMENT_TX
	DISABLE_UART_TX_INTERRUPTS();
#endif
	
	/* set i/o pins to alternate function (see sheet 158 of the R01UH0050EJ0100 Rev.1.00 manual) */
	
#ifdef	UART_IMPLEMENT_TX

	if (enableTransmitter)
	{
		p42sel0 = 0;	// configure pin P4_2 to be TXD0 
		p42sel1 = 1;
	}
	
#endif

#ifdef	UART_IMPLEMENT_RX

	if (enableReceiver)
	{
		p46sel0 = 1;	// configure pin P4_6 to be the RXD0 input
		p46sel1 = 0;
		p46sel2 = 0;

		// see R8C/M12A Hardware Manual section 11.9.4
		_asm("NOP"); _asm("NOP"); _asm("NOP"); _asm("NOP"); _asm("NOP"); _asm("NOP"); _asm("NOP"); _asm("NOP");
		
		u0rif = 0;		// clear any pending interrupt
	}

#endif

	pur4 = 0x40;	// pullup on RXD
//	pod4_2 = 1;		// TXD0 (p4_2) is open drain

#ifdef	UART_IMPLEMENT_TX

	if (enableTransmitter)
	{
		pd4_2 = 1; 		// TXD0 is output
	}
	
#endif

#ifdef	UART_IMPLEMENT_RX

	if (enableReceiver)
	{
		pd4_6 = 0;		// RXD0 is input
	}
	
#endif

	// Setup Uart	
	mstuart = 0;	// Uart0 off standby
	u0mr = 0x15;	// async 8 data, 1 stop, no parity
	u0tbh = 0;		// bit 0 is bit 8 of transmit word.  write this register first for 9-bit xmission

	u0brg = UART_BRG;	// set to precompiled baud rate divisor value
	
	// set U0BRG clock source
#ifdef UART_BRG_CLK_F1

	u0c0 = 0x00;	// U0BRG=F1, DFE disabled, TXD0 CMOS output, CKPOL=0, data is LSB first

#elif UART_BRG_CLK_F8

	u0c0 = 0x01;	// U0BRG=F8, DFE disabled, TXD0 CMOS output, CKPOL=0, data is LSB first

#elif UART_BRG_CLK_F32

	u0c0 = 0x02;	// U0BRG=F32, DFE disabled, TXD0 CMOS output, CKPOL=0, data is LSB first

#endif

#ifdef	UART_IMPLEMENT_TX

#ifdef	RS485_MODE_ENABLED

	RS485_DRIVER_INIT();

#else

	RTS_CTS_INIT();

#endif

	/* initialize tx queue */
	UART_TX_QUEUE_INIT();
	
	if (enableTransmitter)
	{
		te_u0c1 = 1;
		u0irs_u0c1 = 0;
		u0tbh = 0;		// bit 0 is bit 8 of transmit word.  write this register first for 9-bit xmission

		ilvl84 = 1;		// set transmit complete interrupt to level 3
		ilvl85 = 1;
	}

#endif

#ifdef	UART_IMPLEMENT_RX

	if (enableReceiver)
	{
		/* initialize rx queue */
		UART_RX_QUEUE_INIT();
	
		re_u0c1 = 1;
	
		ilvl90 = 1;		// set receive complete interrupt to level 3
		ilvl91 = 1;

		/* enable rx interrupt */
		ENABLE_UART_RX_INTERRUPTS();
	}	
#endif

	/* restore interrupts */
	RESTORE_INTERRUPTS();

}

#ifdef	UART_IMPLEMENT_RX

/////////////////////////////////////////////////////////////////////////////
BOOLEAN UART_IsReceivedDataAvailable()
{
	BOOLEAN	emptystatus;

	/* uart0 rx interrupt disable */
	DISABLE_UART_RX_INTERRUPTS();

	/* copy uart0 rx queue empty status */
	emptystatus = UART_RX_QUEUE_EMPTY();

	/* uart0 rx interrupt enable */
	ENABLE_UART_RX_INTERRUPTS();
	
	return emptystatus ? FALSE : TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BYTE UART_ReceiveByte()
{
	BYTE	c;
	BOOLEAN	emptystatus;

	do
	{
		/* uart0 rx interrupt disable */
		DISABLE_UART_RX_INTERRUPTS();

		/* Q - is uart0 rx queue empty ? */
		if (!(emptystatus = UART_RX_QUEUE_EMPTY()))
		{
			/* no - unqueue a rx byte */
			UART_RX_QUEUE_GET(&c);
		}

		/* uart0 rx interrupt enable */
		ENABLE_UART_RX_INTERRUPTS();
		
	} while (emptystatus == TRUE);

	/* return the unqueued byte */
	return c;
}

#endif	/* UART_IMPLEMENT_RX */

#ifdef	UART_IMPLEMENT_TX

/////////////////////////////////////////////////////////////////////////////
BOOLEAN UART_IsSendDataFull()
{
	BOOLEAN	fullstatus;

	/* uart tx interrupt disable */
	DISABLE_UART_TX_INTERRUPTS();

	/* copy uart tx queue full status */
	fullstatus = UART_TX_QUEUE_FULL();

	/* uart tx interrupt enable */
	ENABLE_UART_TX_INTERRUPTS();
	
	return fullstatus;
}

/////////////////////////////////////////////////////////////////////////////
BOOLEAN UART_IsSendDataEmpty()
{
	BOOLEAN	emptystatus;

	/* uart tx interrupt disable */
	DISABLE_UART_TX_INTERRUPTS();

	/* copy uart tx queue full status */
	emptystatus = UART_TX_QUEUE_EMPTY();

	/* uart tx interrupt enable */
	ENABLE_UART_TX_INTERRUPTS();
	
	return emptystatus;
}

/////////////////////////////////////////////////////////////////////////////
BOOLEAN UART_SendQueueRoomFor(BYTE count)
{
	BOOLEAN roomstatus;
	
	/* uart tx interrupt disable */
	DISABLE_UART_TX_INTERRUPTS();

	roomstatus = ((MAX_UART_TX_BUFFER - UartTxQueueCount) >= count) ? TRUE : FALSE;

	/* uart tx interrupt enable */
	ENABLE_UART_TX_INTERRUPTS();
	
	return roomstatus;
}

#if 0
/////////////////////////////////////////////////////////////////////////////
void UART_SendByte(BYTE data)
{
	BOOLEAN	emptystatus, fullstatus;

	do
	{
		/* uart0 tx interrupt disable */
		DISABLE_UART_TX_INTERRUPTS();

		/* Q - is the uart tx queue empty ? */
		if (emptystatus = UART_TX_QUEUE_EMPTY())
		{
			/* no - Q - is the UART Tx register currently empty ? */
			if (ti_u0c1)
			{

#ifdef	RS485_MODE_ENABLED

				RS485_DRIVER_ENABLE();
				u0irs_u0c1 = 0;
				
#endif

				/* yes - 'prime the pump' by sending this character */
				u0tbl = data;
			}
			else
			{
				/* no - queue a tx byte */
				UART_TX_QUEUE_PUT(&data);
			}

			fullstatus = FALSE;
		}
		else
		{
			/* Q - is uart0 tx queue full ? */
			if (!(fullstatus = UART_TX_QUEUE_FULL()))
			{
				/* no - queue a tx byte */
				UART_TX_QUEUE_PUT(&data);
			}
		}

		/* uart tx interrupt enable */
		ENABLE_UART_TX_INTERRUPTS();
		
	} while (fullstatus == TRUE);
}

#endif

/////////////////////////////////////////////////////////////////////////////
BOOLEAN UART_QueueSendByte(BYTE data)
{
	BOOLEAN fullstatus;
	
	/* uart0 tx interrupt disable */
	DISABLE_UART_TX_INTERRUPTS();
	
	/* Q - is uart0 tx queue full ? */
	if (!(fullstatus = UART_TX_QUEUE_FULL()))
	{
		/* no - queue a tx byte */
		UART_TX_QUEUE_PUT(&data);
	}
	
	/* uart tx interrupt enable */
	ENABLE_UART_TX_INTERRUPTS();
	
	return fullstatus;
}

/////////////////////////////////////////////////////////////////////////////
void UART_SendQueue(void)
{
	BYTE data;
	
	/* uart0 tx interrupt disable */
	DISABLE_UART_TX_INTERRUPTS();

	/* Q - is the uart tx queue not empty ? */
	if (!UART_TX_QUEUE_EMPTY())
	{
		/* yes - Q - is the UART Tx register currently empty ? */
		if (ti_u0c1)
		{

#ifdef	RS485_MODE_ENABLED

			RS485_DRIVER_ENABLE();
			u0irs_u0c1 = 0;
				
#endif

			/* yes - 'prime the pump' by sending a queued character */
			UART_TX_QUEUE_GET(&data);
			u0tbl = data;
		}
	}

	/* uart tx interrupt enable */
	ENABLE_UART_TX_INTERRUPTS();
}

/////////////////////////////////////////////////////////////////////////////
BOOLEAN UART_IsSendDataCompleted()
{
	BOOLEAN	bResult;

	DISABLE_UART_TX_INTERRUPTS();

#ifdef	RS485_MODE_ENABLED

	bResult = UART_TX_QUEUE_EMPTY() && !RS485_DRIVER_IS_ENABLED();

#else

	bResult = UART_TX_QUEUE_EMPTY();
	
#endif
	
	ENABLE_UART_TX_INTERRUPTS();

	return bResult;
}

#endif	/* UART_IMPLEMENT_TX */
