/**************************************************************************
 *
 * FILE NAME:			common.c
 * FILE DESCRIPTION:	Common routines
 *
 * FILE CREATION DATE:	24-05-2004
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 01a,24may03 erd	written
 *
 ***************************************************************************/

#include "sysdef.h"
#include "common.h"
#include "system.h"

// for time delays
unsigned char time_delayUsLeft;

// ==========================================================================
// uint_8 sys_calculateChecksum(uint_8 *buffer, uint_8 size)
//
// Caluclates an 1's complement checksum over contents of buffer
// 
// Arguments:
//	buffer: ptr to start of buffer on which chksum is calculated over
//	size: size, in bytes, of buffer
//
// Return value:
//	checksum value
//
uint_8 sys_calculateChecksum(uint_8 *buffer, uint_8 size)
{
	uint_8 chksum = 0;

	// calculate the checksum over the buffer
	while (size--) 
	{
		chksum += *buffer;
		buffer++;
	}

	// return the checksum
	return (~chksum + 1);
}
// ==========================================================================
// uint_16 calculateChecksum16(uint_8 *buffer, uint_16 size)
//
// Caluclates an 16bit checksum over contents of buffer
// 
// Arguments:
//	buffer: ptr to start of buffer on which chksum is calculated over
//	size: size, in bytes, of buffer
//
// Return value:
//	checksum value
//

uint_16 sys_calculateChecksum16(uint_8 *buffer, uint_16 size)
{
	uint_16 chksum = 0;

	// calculate the checksum over the buffer
	while (size--) 
	{
		chksum += *buffer;
		buffer++;
	}

	// return the checksum
	return (~chksum + 1);
}

// ==========================================================================
// void time_delayMs(int_16 ms)
//
// delay one mS
//
// Arguments:
//	ms: the amount of miliseconds to delay
// 

void time_delayMs(int_16 ms)
{
	uint_8 i;

	do 
	{
		// reset i
		i = 4;
		
		do 
		{
			// delay a quarter of a ms
			time_delayUs(250);
			CLRWDT();
		
		} while(--i);
	
	} while(--ms);
}

// ==========================================================================
// void time_startSyncTimerMs(const uint_16 ms)
//
// Starts a synchronous hardware timer. The function returns immediately and
// can be accompanied by time_waitSyncTimer() to perform blocks for precise
// delays. On expiration of the timer, no interrupt is generated. Uses TMR0.
// This routine works with an a-priori calculation of ticks per milisecond as
// a function of the system clock and the TMR0 prescaler. The calculations were
// done manually due to a compiler quirk. 
// 
// Arguments:
//	ms: number of miliseconds for timer expiration (range: 1 - (65535 / TICKS_PER_MS))
//

/*
#define TMR_INPUT_TICKS_PER_US 	(SYS_CLK_RATE / (4UL * 1000000UL)) // @ ~24MHz = 6.144 ticks
#define TMR_PRESCALER			(256UL)
#define TMR_TICKS_PER_MS		(TMR_INPUT_TICKS_PER_US * 1000UL / TMR_PRESCALER) // @ 24MHz, 128 prescaler = 48
*/

#define TMR_TICKS_PER_MS		(24UL)

void time_startSyncTimerMs(const uint_16 ms)
{
	static uint_16 val;

	// set the timer configuration, prescale = 256
    T0CON = 0B00000111;

	// prepare value
	val = ((0xFFFFUL - TMR_TICKS_PER_MS * ms));

	// must write to tmr0h first, then to tmr0l
	// by writing to tmr0l, the tmr0h and tmr0l values
	// are written to the timer
	TMR0H = (val >> 8);
	TMR0L = val & 0xFF;

	// turn off int
	TMR0IF = 0;

	// turn on the timer
	TMR0ON = 1;
}
// ==========================================================================
// void time_waitSyncTimer()
//
// Blocks until the timer started by time_startSyncTimerMs expires.
// 

void time_waitSyncTimer()
{
	while (!TMR0IF);

	// turn on the timer
	TMR0ON = 0;
}
// ==========================================================================
// void time_getSyncTimerMsLeft()
//
// Returns values:
//	Number of ms left before the timer started by time_startSyncTimerMs expires. 
//

void time_stopSyncTimer()
{
	// turn off the timer
	TMR0ON = 0;
}
// ==========================================================================
// void time_getSyncTimerMsLeft()
//
// Returns values:
//	Number of ms left before the timer started by time_startSyncTimerMs expires. 
//

uint_16 time_getSyncTimerMsLeft()
{
	// get ticks left
	uint_16 ticksLeft = TMR0;

	// return number of ticks left divided by how many
	// ticks per miliseconds
	return (uint_16)(ticksLeft / TMR_TICKS_PER_MS);
}
