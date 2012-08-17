/*! \file i2c.c \brief I2C interface using Philips LPC internal I2C hardware. */
//*****************************************************************************
//
// File Name	: 'i2c.c'
// Title		: I2C interface using Philips LPC internal I2C hardware
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 2004.05.05
// Revised		: 2004.07.12
// Version		: 0.9
// Target MCU	: ARM, Philips LPC2100-series
// Editor Tabs	: 4
//
// Description : I2C (pronounced "eye-squared-see") is a two-wire bidirectional
//		network designed for easy transfer of information between a wide variety
//		of intelligent devices.  Many of the Atmel AVR series processors have
//		hardware support for transmitting and receiving using an I2C-type bus.
//		In addition to the AVRs, there are thousands of other parts made by
//		manufacturers like Philips, Maxim, National, TI, etc that use I2C as
//		their primary means of communication and control.  Common device types
//		are A/D & D/A converters, temp sensors, intelligent battery monitors,
//		MP3 decoder chips, EEPROM chips, multiplexing switches, etc.
//
//		I2C uses only two wires (SDA and SCL) to communicate bidirectionally
//		between devices.  I2C is a multidrop network, meaning that you can have
//		several devices on a single bus.  Because I2C uses a 7-bit number to
//		identify which device it wants to talk to, you cannot have more than
//		127 devices on a single bus.
//
//		I2C ordinarily requires two 4.7K pull-up resistors to power (one each on
//		SDA and SCL), but for small numbers of devices (maybe 1-4), it is enough
//		to activate the internal pull-up resistors in the AVR processor.  To do
//		this, set the port pins, which correspond to the I2C pins SDA/SCL, high.
//		For example, on the mega163, sbi(PORTC, 0); sbi(PORTC, 1);.
//
//		For complete information about I2C, see the Philips Semiconductor
//		website.  They created I2C and have the largest family of devices that
//		work with I2C.
//
// Note: Many manufacturers market I2C bus devices under a different or generic
//		bus name like "Two-Wire Interface".  This is because Philips still holds
//		"I2C" as a trademark.  For example, SMBus and SMBus devices are hardware
//		compatible and closely related to I2C.  They can be directly connected
//		to an I2C bus along with other I2C devices are are generally accessed in
//		the same way as I2C devices.  SMBus is often found on modern motherboards
//		for temp sensing and other low-level control tasks.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include "lpc210x.h"
#include "config.h"
#include "global.h"
#include "timer.h"
#include "i2c.h"

void i2cInit(void)
{
	// setup SCL pin P02
	PINSEL0 &= ~(3<<4);
	PINSEL0 |=   1<<4;
	// setup SDA pin P03
	PINSEL0 &= ~(3<<6);
	PINSEL0 |=   1<<6;

	// set default bitrate of 100KHz
	i2cSetBitrate(400);

	// disable and reset interface
	I2CONCLR = 0xFF;
	delay(10);

	// enable interface
	I2CONSET = BIT(I2CON_I2EN);
}
/*
void InitialiseI2C(void)
{

REG(I2C_I2SCLL) = 0x18;

REG(I2C_I2SCLH) = 0x18;

REG(I2C_I2CONCLR) = 0xFF;

// Set pinouts as scl and sda
REG(PCB_PINSEL0) = 0x50;

REG(I2C_I2CONSET) = 0x40;
delay(10);
REG(I2C_I2CONSET) = 0x64;

REG(I2C_I2DAT) = 0x42;

REG(I2C_I2CONCLR) = 0x08;

REG(I2C_I2CONCLR) = 0x20;

}
*/

void i2cSetBitrate(u16 bitrateDiv)
{
	// @60MHz and VPB=2, set to 75 for 400KHz
	// @60MHz and VPB=2, set to 400 for 75KHz
	I2SCLL = bitrateDiv;
	I2SCLH = bitrateDiv;
}

void i2cSetLocalDeviceAddr(u08 deviceAddr, u08 genCallEn)
{
	// set local device address (used in slave mode only)
	I2ADR = ((deviceAddr&0xFE) | (genCallEn?1:0));
}

void i2cSendStart(void)
{
	I2CONSET = BIT(I2CON_STA);
	I2CONCLR = BIT(I2CON_SI);
}

void i2cSendStop(void)
{
	I2CONSET = BIT(I2CON_STO);
}

void i2cWaitForComplete(void)
{
	// wait for a valid status code
	while(I2STAT == TW_NO_INFO);
}

void i2cSendByte(u08 data)
{
	// save data into data register
	I2DAT = data;
	// clear SI bit to begin transfer
	I2CONCLR = BIT(I2CON_SI);
}

void i2cReceiveByte(u08 ackFlag)
{
	// begin receive over i2c
	if( ackFlag )
	{
		// ackFlag = TRUE: ACK the recevied data
		I2CONSET = BIT(I2CON_AA);
		//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
	}
	else
	{
		// ackFlag = FALSE: NACK the recevied data
		I2CONCLR = BIT(I2CON_AA);
		//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
	}
	// clear SI bit to begin transfer
	I2CONCLR = BIT(I2CON_SI);
}

u08 i2cGetReceivedByte(void)
{
	return I2DAT;
}

u08 i2cGetStatus(void)
{
	return I2STAT;
}


u08 i2cMasterSendNI(u08 deviceAddr, u08 length, u08* data)
{
	u08 retval = I2C_OK;

	// disable TWI interrupt
	//cbi(TWCR, TWIE);

	// send start condition
	i2cSendStart();
	i2cWaitForComplete();
	I2CONCLR = BIT(I2CON_STA);

	// send device address with write
	i2cSendByte( deviceAddr & 0xFE );
	i2cWaitForComplete();

	// check if device is present and live
	if( I2STAT == TW_MT_SLA_ACK)
	{
		// send data
		while(length)
		{
			i2cSendByte( *data++ );
			i2cWaitForComplete();
			length--;
		}
	}
	else
	{
		// device did not ACK it's address,
		// data will not be transferred
		// return error
		retval = I2C_ERROR_NODEV;
	}

	// transmit stop condition
	// leave with TWEA on for slave receiving
	I2CONSET = BIT(I2CON_STA);
	delay(10);
	I2CONCLR = BIT(I2CON_STO);
	//while( !(inb(TWCR) & BV(TWSTO)) );

	// enable TWI interrupt
	//sbi(TWCR, TWIE);

	return retval;
}

u08 i2cMasterReceiveNI(u08 deviceAddr, u08 length, u08 *data)
{
	u08 retval = I2C_OK;

	// disable TWI interrupt
	///cbi(TWCR, TWIE);

	// send start condition
	i2cSendStart();
	i2cWaitForComplete();

	// send device address with read
	i2cSendByte( deviceAddr | 0x01 );
	i2cWaitForComplete();

	// check if device is present and live
	if( I2STAT == TW_MR_SLA_ACK)
	{
		// accept receive data and ack it
		while(length > 1)
		{
			i2cReceiveByte(TRUE);
			i2cWaitForComplete();
			*data++ = i2cGetReceivedByte();
			// decrement length
			length--;
		}

		// accept receive data and nack it (last-byte signal)
		i2cReceiveByte(FALSE);
		i2cWaitForComplete();
		*data++ = i2cGetReceivedByte();
	}
	else
	{
		// device did not ACK it's address,
		// data will not be transferred
		// return error
		retval = I2C_ERROR_NODEV;
	}

	// transmit stop condition
	// leave with TWEA on for slave receiving
	i2cSendStop();

	// enable TWI interrupt
	//sbi(TWCR, TWIE);

	return retval;
}

