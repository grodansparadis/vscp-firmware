//*****************************************************************************
// File Name	: ds1631test.c
// 
// Title		: example usage of DS1631 library functions
// Revision		: 1.0
// Notes		:	
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
// 
// Revision History:
// When			Who			Description of change
// -----------	-----------	-----------------------
// 20-Feb-2004	pstang		Created the program
//*****************************************************************************
 
//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/signal.h>	// include "signal" names (interrupt names)
#include <avr/interrupt.h>	// include interrupt support

#include "global.h"		// include our global settings
#include "uart.h"		// include uart function library
#include "rprintf.h"	// include printf function library
#include "timer.h"		// include timer function library (timing, PWM, etc)
#include "vt100.h"		// include VT100 terminal support
#include "i2c.h"		// include i2c support
#include "ads7828.h"	// include ADS7828 support

void ads7828test(void);

//----- Begin Code ------------------------------------------------------------
int main(void)
{
	// initialize our libraries
	// initialize the UART (serial port)
	uartInit();
	uartSetBaudRate(115200);
	// make all rprintf statements use uart for output
	rprintfInit(uartSendByte);
	// initialize the timer system
	timerInit();
	// clear terminal screen
	vt100ClearScreen();
	vt100SetCursorPos(1,1);
	// print a little intro message so we know things are working
	rprintf("\r\nWelcome to ADS7828 test!\r\n");
	
	// run tests
	ads7828test();

	return 0;
}


void ads7828test(void)
{
	u16 conv=0;

	// initialize i2c function library
	i2cInit();
	i2cSetBitrate(100);

	// initialize
	if(ads7828Init(ADS7828_I2C_ADDR))
	{
		rprintf("ADS7828 detected and initialized!\r\n");
	}
	else
	{
		rprintf("Cannot detect ADS7828!\r\n");
		return;
	}
	
	// use the internal 2.5V voltage reference
	ads7828SetReference(1);

	while(1)
	{
		// convert
		conv = ads7828Convert(ADS7828_I2C_ADDR, 0);
		// print results
		rprintf("Conversion result is: %d\r\n", conv);

//		rprintf("Real Temp is: ");
//		rprintfNum(10, 4, TRUE , ' ', T>>8);
//		rprintf(".");
//		rprintfNum(10, 4, FALSE, '0', (10000*((u32)(T&0x00FF)))/256 );
//		rprintf(" degrees C\r\n");

		timerPause(100);
	}
}
