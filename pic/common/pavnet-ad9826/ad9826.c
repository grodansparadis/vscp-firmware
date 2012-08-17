/**************************************************************************
 *
 * FILE NAME:			ad9826.h
 * FILE DESCRIPTION:	15MHz A2D driver implementation
 * 
 * FILE CREATION DATE:	26-06-2004
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 01a,26jun04 erd	written
 *
 ***************************************************************************/

#include "sysdef.h"
#include "system.h"
#include "ad9826_prv.h"
#include "post.h"
#include "cm.h"

// ==================================================================
// ad9826_init()
//
// Initialize the A2D to SHA 1-channel mode.
//
// Return value:
//   TRUE if initialized successfully, FALSE if failed to initialize   
//

BOOL ad9826_init()
{
	uint_16 regValue;
	
	// set configuration register
	//
	// D8 = always low					= 0
	// D7 = 4V input range 				= 1
	// D6 = Internal reference  		= 1
	// D5 = Not 3-channel				= 0
	// D4 = Disable CDS					= 0
	// D3 = High bias clamp				= 1
	// D2 = Not In power down			= 0
	// D1 = always low					= 0
	// D0 = 16 bit result mode			= 0
	regValue = 0b011001000;

	// write the configuration word
	ad9826_cfgWriteWord(AD9826_CONFIG_REG_OFFSET, regValue);
	
	// set MUX configuration register
	//
	// D8 = always low					= 0
	// D7 = RGB mode					= 1
	// D6 = Sample red, always  		= 1
	// D5 = Don't sample green			= 0
	// D4 = Don't sample blue			= 0
	// D3 = always low					= 0
	// D2 = always low					= 0
	// D1 = always low					= 0
	// D0 = always low					= 0
	regValue = 0b011000000;

	// write the mux word
	ad9826_cfgWriteWord(AD9826_MUX_REG_OFFSET, regValue);

	// set gains
	ad9826_cfgWriteWord(AD9826_RED_PGA_REG_OFFSET, 			0);
	ad9826_cfgWriteWord(AD9826_GREEN_PGA_REG_OFFSET, 		0);
	ad9826_cfgWriteWord(AD9826_BLUE_PGA_REG_OFFSET, 		0);

	// set offsets - The red offset is set when the gain is set since it is 
	// function of the configured gain
	ad9826_cfgWriteWord(AD9826_RED_OFFSET_REG_OFFSET, 		0);
	ad9826_cfgWriteWord(AD9826_GREEN_OFFSET_REG_OFFSET, 	0);
	ad9826_cfgWriteWord(AD9826_BLUE_OFFSET_REG_OFFSET, 		AD9826_TEST_PATTERN);
	
	// perform the test, return if success
	return ad9826_test();
}
// ==================================================================
// ad9826_test()
//
// Performs a test on the device - simply reads back a written register
// and compares it with what was previously written
//
// Return values:
//	TRUE if value matches (test passed) or FALSE if mismatch (test failed)
//
BOOL ad9826_test()
{
	uint_16 regValue;
	BOOL match;
	
	// read back the blue offset ergister
	ad9826_cfgReadWord(AD9826_BLUE_OFFSET_REG_OFFSET, &regValue);

	// check if matches
	match = (regValue == AD9826_TEST_PATTERN);

	// if it doesn't, set the error
	if (!match)
	{
		// set post error
		post_errorType = POST_ET_AD9826_FAILED_CONFIGURE;
	}
	
	// return if matches
	return (match);
}

// ==================================================================
// ad9826_pulseClock()
//
// Assumes SDATA has data, and pulses the clock one time
//
#define ad9826_pulseClock() BIT_PULSE(IO_SPI_SCLK)

// ==================================================================
// ad9826_cfgStartTransaction(const uint_8 readWriteMode, const uint_8 address)
//
// Starts a transaction with the device. Assumes chip is selected.
// 
// Arguments:
//  readWriteMode: AD9826_SDATA_READ_MODE or AD9826_SDATA_WRITE_MODE
//  address: 3 bit address of register
//
// Return value:
//	None
//

void ad9826_cfgStartTransaction(const uint_8 readWriteMode, const uint_8 address)
{
	uint_8 mask, val;
	
	// set read mode
	IO_SPI_SDATA = readWriteMode;
	
	// pulse the clock
	ad9826_pulseClock();

	// write address
	for (mask = 0x4; mask; mask >>= 1)
	{
		// set address bit, A2 -> A1 -> A0
		val = (address & mask);
		IO_SPI_SDATA = val ? 1 : 0;

		// pulse the clock
		ad9826_pulseClock();
	}

	// clear data
	IO_SPI_SDATA = 0;
	
	// set up time (TODO: may not be required)
	for (mask = 3; mask; --mask)
	{
		// pulse the clock
		ad9826_pulseClock();
	}
}

// ==================================================================
// ad9826_cfgReadWord()
//
// Reads a word from the configuration memory
//
// Arguments:
//  offset: register offset to read from
//  buffer: buffer to store read data
//
// Return value:
//	If TRUE, successfully received data from IC. If FALSE, 
//  device did not respond correctly and buffer does not hold proper
//  data

BOOL ad9826_cfgReadWord(const uint_8 offset, uint_16 *buffer)
{
	uint_8 bitIndex;
	uint_16 readWord = 0;
	
	// disable interrupts
	sys_disableInterrupts();

	// chip select
	IO_SPI_SLOAD = 0;
	
	// start the transaction
	ad9826_cfgStartTransaction(AD9826_SDATA_READ_MODE, offset);

	// change data to input
	sys_setIoDirection(IO_SPI_SDATA, IO_PIN_INPUT);

	// iterate through the bits to read data
	for (bitIndex = 9; bitIndex; --bitIndex)
	{
		// shift the data
		readWord <<= 1;

		// read data
 		readWord |= IO_SPI_SDATA;

		// pulse the clock
		ad9826_pulseClock();
	}

	// change data back to output
	sys_setIoDirection(IO_SPI_SDATA, IO_PIN_OUTPUT);

	// chip select
	IO_SPI_SLOAD = 1;

	// disable interrupts
	sys_enableInterrupts();

	// copy the buffer
	*buffer = readWord;

	// success
	return TRUE;
}
// ==================================================================
// ad9826_cfgWriteWord()
//
// Writes a byte to the configuration memory
//
// Arguments:
//  offset: register offset to write to
//  buffer: byte to write to device
//
// Return value:
//	If TRUE, successfully wrote data from IC. If FALSE, 
//  device did not respond correctly and buffer was not written correctly

BOOL ad9826_cfgWriteWord(const uint_8 offset, const uint_16 buffer)
{
	uint_8 bitIndex;
	uint_16 mask, val;
	
	// disable interrupts
	sys_disableInterrupts();

	// chip select
	IO_SPI_SLOAD = 0;

	// start the transaction
	ad9826_cfgStartTransaction(AD9826_SDATA_WRITE_MODE, offset);

	// init mask
	mask = (1 << 8);
	
	// iterate through the bits to read data
	for (bitIndex = 9; bitIndex; --bitIndex, mask >>= 1)
	{
		// read data
 		val = (buffer & mask);
		IO_SPI_SDATA = val ? 1 : 0;

		// pulse the clock
		ad9826_pulseClock();
	}

	// clear data
	IO_SPI_SDATA = 0;

	// chip select
	IO_SPI_SLOAD = 1;

	// disable interrupts
	sys_enableInterrupts();

	// success
	return TRUE;
}
// ==================================================================
// ad9826_sample()
//
// Read a sample. This samlpe cycle in accordance with the SHA 1 channel
// read waveform in the datasheet.
//
// Arguments:
//  sample: 16 bit sample result, of the n-3'th cycle. 
//
// Return value:
//	If TRUE, successfully read data from IC. If FALSE, 
//  device did not respond correctly and buffer was not written correctly

BOOL ad9826_sample(uint_16 *sample)
{
	uint_8 hiByte, loByte;

	// start sample clock
	BIT_SET(IO_A2D_CDSCLK2);

	// clear sample clock
	BIT_CLR(IO_A2D_CDSCLK2);

	// start conversion cycle
	BIT_SET(IO_A2D_ADCLK);

	time_delayOneUs();

	// read data on bus
	hiByte = IO_DBUS_PORT;
	
	// end conversion cycle
	BIT_CLR(IO_A2D_ADCLK);

	// wait a bit
	time_delayOneUs();

	// read data on bus
	loByte = IO_DBUS_PORT;

	// copy result back to 16bit counter
	*sample = ((hiByte) << 8);
	*sample |= (loByte);

	// success
	return TRUE;
}
// ==================================================================

// ad9826_setSampleState(const AD9826_SAMPLING_STATE state)
//
// Perform all initializations and de-initializations required to reduce
// operations in the sampling loop
//
// Arguments:
//  state: AD9826_SS_STARTED will initialize and prepare for repeated samples
//			while AD9826_SS_STOPPED will de-initialize and return to normal
//			operation
//
// Return value:
//	If TRUE, successfully read data from IC. If FALSE, 
//  device did not respond correctly and buffer was not written correctly

BOOL ad9826_setSampleState(const AD9826_SAMPLING_STATE state)
{
	// check state
	if (state == AD9826_SS_STARTED)
	{
		// select chip (output enable)
		BIT_CLR(IO_A2D_ADCS);

		// set data bus as input
		IO_DBUS_PORT_DIR = 0xFF;
	}
	else if (state == AD9826_SS_STOPPED)
	{
		// deselect chip (output disable)
		BIT_SET(IO_A2D_ADCS);
	
		// set data bus as output
		IO_DBUS_PORT_DIR = 0;
	}
	// unknown state
	else return FALSE;

	// success
	return TRUE;
}
// ==================================================================
// ad9826_setPowerDown(const BOOL state)
//
// Sets power down mode
//
// Arguments:
//  state: TRUE to power down, FALSE to activate
//

void ad9826_setPowerDown(const BOOL state)
{
	uint_16 regValue;
	
	// set configuration register
	//
	// D8 = always low					= 0
	// D7 = 4V input range 				= 1
	// D6 = Internal reference  		= 1
	// D5 = Not 3-channel				= 0
	// D4 = Disable CDS					= 0
	// D3 = High bias clamp				= 1
	// D2 = In power down				= state
	// D1 = always low					= 0
	// D0 = 16 bit result mode			= 0
	regValue = 0b011001000;
	
	// set the power down bit if necessary
	if (state == TRUE) regValue |= (1 << 2);

	// write the configuration word
	ad9826_cfgWriteWord(AD9826_CONFIG_REG_OFFSET, regValue);
}
// ==================================================================
// ad9826_setGain(const float gain)
//
// Set A2D gain
//
// Arguments:
//	gain: required gain. Values from 1.0 - 6.0 are valid
//
// Return value:
//	If TRUE, successfully wrote data from IC. If FALSE, 
//  device did not respond correctly and buffer was not written correctly
//

BOOL ad9826_setGain(const float gain)
{
	// offset after taking gain into consideration
	int_16 newOffset;
	static BOOL doFirst = TRUE;

	// convert the required gain to the appropriate register value
	// since the gain is not linear to teh register value, we must
	// convert the gain to the register value with help of the equation
	// in Figure 17 (page 18) in the 9826 datasheet.
	uint_16 registerValue = (uint_8)(75.6 - 75.6 / gain);

	// write only 6 LSb
	registerValue &= 0x3F;

	// calculate the new offset (its basically the desired offset divided
	// by the gain)
	// newOffset = (uint_16)((cm_configParams.spectOffset) / cm_configParams.spectGain);
	newOffset = cm_configParams.spectOffset;
		
	// reconfigure the offset as a function of the gain
	ad9826_setOffset(newOffset);

	// write to device
	ad9826_cfgWriteWord(AD9826_RED_PGA_REG_OFFSET, registerValue);

	// success
	return TRUE;
}
// ==================================================================
// ad9826_setOffset(const int_16 offset)
//
// Set A2D offset
//
// Arguments:
//	offset: injected offset (-300 - 300)
//
// Return value:
//	If TRUE, successfully wrote data to IC. If FALSE, 
//  device did not respond correctly and buffer was not written correctly
//

BOOL ad9826_setOffset(const int_16 offset)
{
	// get if number is negative
	uint_8 signBit = (offset < 0) ? 1 : 0;

	// calculate offset counts
	float offsetCounts = ((float)offset / 1.2);
	
	// get the value of the offset
	uint_16 registerValue = (offsetCounts);
	
	// set the sign bit
	if (signBit) registerValue |= (1 << 8);

	// set register
	ad9826_cfgWriteWord(AD9826_RED_OFFSET_REG_OFFSET, (registerValue & 0x1FF));

	// success
	return TRUE;
}

