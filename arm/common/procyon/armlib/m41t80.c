
// system includes
#include "at91sam7s64.h"
#include "global.h"

// local includes
#include "i2c.h"
#include "rprintf.h"
#include "m41t80.h"

// functions
void m41t80Init(void)
{
	// check if clock is running
	if(m41t80ReadReg(M41T80_REG_SECONDS) & 0x80)
	{
		// STOP bit is set, clock is not running
		// lets start it
		m41t80Reset();
	}
	// otherwise, clock is already running
	// Do not disturb.
}

void m41t80Reset(void)
{
	// reset registers to initialized state
	m41t80WriteReg(M41T80_REG_CTRL, 0x80);
	// start clock
	m41t80WriteReg(M41T80_REG_SECONDS, 0x00);
	// reset time
	m41t80SetTime(0);
	// start clock updates
	m41t80WriteReg(M41T80_REG_AL_HOUR, 0x00);
}

void m41t80WriteReg(u08 regaddr, u08 data)
{
	u08 packet[2];
	// setup write
	packet[0] = regaddr;
	packet[1] = data;
	// write the register
	i2cMasterSend(M41T80_I2C_BASE_ADDR, 2, packet);
}

u08 m41t80ReadReg(u08 regaddr)
{
	u08 data;
	// set register address to read
	i2cMasterSend(M41T80_I2C_BASE_ADDR, 1, &regaddr);
	// read the register
	i2cMasterReceive(M41T80_I2C_BASE_ADDR, 1, &data);
	// return data
	return data;
}

void m41t80SetOutState(int state)
{
	// set the OUT pin state
	// if state == FALSE, OUT is set low
	// if state == TRUE, OUT is set open (open-collector)
	
	// modify OUT bit
	if(state)
		m41t80WriteReg(M41T80_REG_CTRL, m41t80ReadReg(M41T80_REG_CTRL) | 0x80);
	else
		m41t80WriteReg(M41T80_REG_CTRL, m41t80ReadReg(M41T80_REG_CTRL) & ~0x80);
}


void m41t80SetTime(u32 time)
{
	u08 packet[5];

	// prepare write
	packet[0] = M41T80_REG_DSECONDS;
	packet[1] = time>>24;
	packet[2] = time>>16;
	packet[3] = time>>8;
	packet[4] = time;
	// write new time
	i2cMasterSend(M41T80_I2C_BASE_ADDR, 5, packet);
}

u32 m41t80GetTime(void)
{
	u08 reg;
	u32 time;

	// set the register to access
	reg = M41T80_REG_DSECONDS;
	i2cMasterSend(M41T80_I2C_BASE_ADDR, 1, &reg);
	// get the current time
	i2cMasterReceive(M41T80_I2C_BASE_ADDR, 4, (unsigned char*)&time);
	// return time fields
	// -- All fields BCD encoded --
	// bits 31-30 : century bits
	// bits 29-28 : 10's hours
	// bits 27-24 : 1's hours
	// bits 23-20 : 10's minutes
	// bits 19-16 : 1's minutes
	// bits 15-12 : 10's seconds
	// bits 11-08 : 1's seconds
	// bits 07-04 : 0.1's seconds
	// bits 03-00 : 0.01's seconds
	return time;
}

void m41t80PrintTime(u32 time)
{
	// mask off century bits
	time &= 0x3FFFFFFF;
	// print hours
	rprintfu08(time>>24);
	rprintfChar(':');
	// print minutes
	rprintfu08(time>>16);
	rprintfChar(':');
	// print seconds
	rprintfu08(time>>8);
	rprintfChar('.');
	// print decimal seconds
	rprintfu08(time);
}
