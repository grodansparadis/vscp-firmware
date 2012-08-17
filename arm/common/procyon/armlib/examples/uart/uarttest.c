//*****************************************************************************
// File Name	: uarttest.c
// 
// Title		: example usage of uart library functions
// Revision		: 1.0
// Notes		:	
// Target MCU	: ARM processors
// Editor Tabs	: 4
// 
// Revision History:
// When			Who			Description of change
// -----------	-----------	-----------------------
// 30-Apr-2004	pstang		Created the program
//*****************************************************************************


//----- Include Files ---------------------------------------------------------
#include "global.h"			// include our global project settings
#include "lpc2000.h"		// include LPC210x defines
#include "processor.h"		// include processor initialization functions
#include "timer.h"			// include timer functions
#include "uart.h"			// include uart library functions
#include "rprintf.h"		// include printf library functions

//----- Begin Code ------------------------------------------------------------

// prototypes
void uartTest(void);

int main(void)
{
	// initialize processor
	processorInit();
	// initialize timers
	timerInit();
	// initialize uarts
	uart0Init(UART_BAUD(115200), UART_8N1, UART_FIFO_8);
	uart1Init(UART_BAUD(115200), UART_8N1, UART_FIFO_8);
	// initialize rprintf to use UART1 for output
	rprintfInit(uart1SendByte);
	
	// Wait for a moment to allow hardware to stabilize.
	// This may be important if a serial port level-converter
	// like the MAX232 is used.  Charge-pump based converters
	// need some time after power-up to charge before
	// communication is reliable.
	timerPause(50);		// waits 50 milliseconds

	// run the test
	uartTest();

	return 0;
}

void uartTest(void)
{
	int c;

	// print a little intro message so we know things are working
	rprintfStr("\r\n\n\nWelcome to the uart library test program!\r\n");
	rprintfStr("Press any key...\r\n");

	// read in serial port traffic from UART1 
	// and echo it back in hex and/or ascii format as appropriate
	while(1)
	{
		if((c = uart1GetByte()) != -1)
		{
			// we got a byte
			rprintf("Ascii code: 0x%x ", c);
			// see if it's printable
			if((c >= 0x20) && (c < 0x7F))
			{
				uart1SendByte(c);
			}
			rprintfStr("\r\n");
		}
	}
}
