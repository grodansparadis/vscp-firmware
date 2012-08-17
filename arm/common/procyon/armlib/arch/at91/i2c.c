/*! \file i2c.c \brief I2C Interface Driver for Atmel AT91. */
//*****************************************************************************
//
// File Name	: 'i2c.c'
// Title		: I2C Interface Driver for Atmel AT91
// Author		: Pascal Stang - Copyright (C) 2005-2006
// Created		: 12/10/2005
// Revised		: 10/22/2006
// Version		: 0.1
// Target MCU	: Atmel AT91 series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

// system includes
#include "at91sam7s64.h"
#include "global.h"

// local includes
#include "rprintf.h"
#include "timer.h"
#include "i2c.h"

// functions
void i2cInit(void)
{
	// set I/O pins for I2C
	*AT91C_PIOA_PDR = AT91C_PA3_TWD | AT91C_PA4_TWCK;
	*AT91C_PIOA_ASR = AT91C_PA3_TWD | AT91C_PA4_TWCK;
	// and set for open-drain
	*AT91C_PIOA_MDER = AT91C_PA3_TWD | AT91C_PA4_TWCK;

	// turn on clock into I2C system
	AT91C_BASE_PMC->PMC_PCER = (1<<AT91C_ID_TWI);

	// disable interrupt handling
	//*AT91C_AIC_IDCR = 0xFFFFFFFF;

	// set bitrate
	AT91C_BASE_TWI->TWI_CWGR = 0x0001C0C0;
	//AT91C_BASE_TWI->TWI_CWGR = 0x0000C0C0;

	// initialize interface
	AT91C_BASE_TWI->TWI_CR = AT91C_TWI_MSEN;
}

int i2cMasterSend(u08 deviceAddr, u08 length, u08 *data)
{
	int status;

	// prepare write transmission
	AT91C_BASE_TWI->TWI_MMR = deviceAddr<<16;
	// set slave device internal address
	//AT91C_BASE_TWI->TWI_IADR = 0x10;
	if(length)
	{
		// load transmit holding register
		AT91C_BASE_TWI->TWI_THR = *data++;
		length--;
	}
	// send start condition
	AT91C_BASE_TWI->TWI_CR = AT91C_TWI_START;
	// wait for complete (or no device)
	do
	{
		// required in order for NACK (dev not present) to be detected and work
		delay_us(50);
		// check status
		status = AT91C_BASE_TWI->TWI_SR;
		// watch for address NACK
		if(status & AT91C_TWI_NACK)
			return I2C_ERROR_NODEV;
	} while(!(status & AT91C_TWI_TXRDY));
	// send more data
	while(length)
	{
		// load transmit holding register
		AT91C_BASE_TWI->TWI_THR = *data++;
		length--;
		// wait for complete
		while(!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXRDY));
	}
	// send stop condition
	AT91C_BASE_TWI->TWI_CR = AT91C_TWI_STOP;
	// wait for trasmission complete
	while(!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXCOMP));
	// return ACK/NACK status
	return (AT91C_BASE_TWI->TWI_SR & AT91C_TWI_NACK);
}

int i2cMasterReceive(u08 deviceAddr, u08 length, u08 *data)
{
	int status;

	// prepare read transmission
	AT91C_BASE_TWI->TWI_MMR = (deviceAddr<<16) | AT91C_TWI_MREAD;
	// dummy read the receive holding register
	AT91C_BASE_TWI->TWI_RHR;
	// set slave device internal address
	//AT91C_BASE_TWI->TWI_IADR = 0x10;
	// send start condition
	AT91C_BASE_TWI->TWI_CR = AT91C_TWI_START;
	// wait for complete (or no device)
	do
	{
		// required in order for NACK (dev not present) to be detected and work
		delay_us(50);
		// check status
		status = AT91C_BASE_TWI->TWI_SR;
		// watch for address NACK
		if(status & AT91C_TWI_NACK)
			return I2C_ERROR_NODEV;
	} while(!(status & AT91C_TWI_RXRDY));
	// receive data
	while(length)
	{
		// load transmit holding register
		 *data++ = AT91C_BASE_TWI->TWI_RHR;
		length--;
		// wait for complete
		while(!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_RXRDY));
	}
	// send stop condition
	AT91C_BASE_TWI->TWI_CR = AT91C_TWI_STOP;
	// wait for trasmission complete
	while(!(AT91C_BASE_TWI->TWI_SR & AT91C_TWI_TXCOMP));
	// return ACK/NACK status
	return (AT91C_BASE_TWI->TWI_SR & AT91C_TWI_NACK);
}


void i2cScanBus(void)
{
	int i;
	u08 packet[1];


	for(i=1; i<128; i++)
	{
		if(!i2cMasterSend(i, 1, packet))
			rprintf("I2C Device addr 0x%x -> PRESENT\n", i);
		else
			rprintf("I2C Device addr 0x%x -> NACK\n", i);
	}
}

