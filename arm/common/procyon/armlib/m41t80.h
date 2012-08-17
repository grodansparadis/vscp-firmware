
#ifndef M41T80_H
#define M41T80_H

#include "global.h"

// device base address
#define M41T80_I2C_BASE_ADDR	0x68

// device registers
#define M41T80_REG_DSECONDS		0x00
#define M41T80_REG_SECONDS		0x01
#define M41T80_REG_MINUTES		0x02
#define M41T80_REG_HOURS		0x03
#define M41T80_REG_DAYOFWEEK	0x04
#define M41T80_REG_DAYOFMONTH	0x05
#define M41T80_REG_MONTH		0x06
#define M41T80_REG_YEAR			0x07
#define M41T80_REG_CTRL			0x08
#define M41T80_REG_WATCHDOG		0x09
#define M41T80_REG_AL_MONTH		0x0A
#define M41T80_REG_AL_DATE		0x0B
#define M41T80_REG_AL_HOUR		0x0C
#define M41T80_REG_AL_MINUTES	0x0D
#define M41T80_REG_AL_SECONDS	0x0E
#define M41T80_REG_FLAGS		0x0F
#define M41T80_REG_SQW			0x13

// functions

// initializes RTC for operation
void m41t80Init(void);

// resets RTC and starts clock
// WARNING: this clears the time!
void m41t80Reset(void);

// write RTC register
void m41t80WriteReg(u08 regaddr, u08 data);

// read RTC register
u08 m41t80ReadReg(u08 regaddr);

// set state of OUT pin
void m41t80SetOutState(int state);

// gets time in 32-bit BCD format (HH:MM:SS.ss)
void m41t80SetTime(u32 time);

// gets time in 32-bit BCD format (HH:MM:SS.ss)
u32 m41t80GetTime(void);

// print BCD time format (HH:MM:SS.ss)
void m41t80PrintTime(u32 time);

#endif
