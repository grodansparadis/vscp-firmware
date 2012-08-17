/*! \file uart.c \brief UART driver for ARM ADuC7000 16450. */
//*****************************************************************************
//
// File Name	: 'uart.c'
// Title		: UART driver for ARM ADuC7000 16450
// Author		: Pascal Stang - Copyright (C) 2005
// Created		: 2/5/2005
// Revised		: 2/5/2005
// Version		: 0.1
// Target MCU	: Analog Devices ARM ADuC7000 Series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include "aduc7026.h"

#include "global.h"
#include "uart.h"

//! enable and initialize the uart
void uartInit(uint16_t baud, uint32_t mode)
{
	// set port pins for UART
	GP1CON = 0x11;

	COMCON0 = 0x80;
//	COMDIV0 = 0x0C;
	COMDIV0 = 0x0B;
	COMDIV1 = 0x00;
	COMCON0 = 0x07;
}

int uartSendByte(int data)
{
	while(!(COMSTA0 & 0x20))		// wait for TX buffer to empty
		continue;					// also either WDOG() or swap()

	COMTX = (uint8_t)data;
	return (uint8_t)data;
}

int uartGetByte(void)
{
	if(COMSTA0 & 0x01)				// check if character is available
		return COMRX;				// return character
	return -1;
}

