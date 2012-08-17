/*! \file ad9854.h \brief Analog Devices AD9854 Digital Synthesizer Driver. */
//*****************************************************************************
//
// File Name	: 'ad9854.h'
// Title		: Analog Devices AD9854 Digital Synthesizer Driver
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 2004.05.05
// Revised		: 2004.07.12
// Version		: 0.1
// Target MCU	: ARM processors
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef AD9854_H
#define AD9854_H

#include "global.h"

// defines

// hardware
//#define AD9854_REFCLK_FREQ		24000000l
#define AD9854_REFCLK_FREQ		50000000l
#define AD9854_IO_UPDATE		P04
#define AD9854_RESET			P05

// base address
//#define AD9854_REG_BASE			0x8400
#define AD9854_REG_BASE			0x81000400

// registers
#define AD9854_REG_PHASE1H		0x00
#define AD9854_REG_PHASE1L		0x01

#define AD9854_REG_PHASE2H		0x02
#define AD9854_REG_PHASE2L		0x03

#define AD9854_REG_FREQ15		0x04
#define AD9854_REG_FREQ14		0x05
#define AD9854_REG_FREQ13		0x06
#define AD9854_REG_FREQ12		0x07
#define AD9854_REG_FREQ11		0x08
#define AD9854_REG_FREQ10		0x09

#define AD9854_REG_FREQ25		0x0A
#define AD9854_REG_FREQ24		0x0B
#define AD9854_REG_FREQ23		0x0C
#define AD9854_REG_FREQ22		0x0D
#define AD9854_REG_FREQ21		0x0E
#define AD9854_REG_FREQ20		0x0F

#define AD9854_REG_DELTA5		0x10
#define AD9854_REG_DELTA4		0x11
#define AD9854_REG_DELTA3		0x12
#define AD9854_REG_DELTA2		0x13
#define AD9854_REG_DELTA1		0x14
#define AD9854_REG_DELTA0		0x15

#define AD9854_REG_UPDCLOCK3	0x16
#define AD9854_REG_UPDCLOCK2	0x17
#define AD9854_REG_UPDCLOCK1	0x18
#define AD9854_REG_UPDCLOCK0	0x19

#define AD9854_REG_RAMPCLOCK2	0x1A
#define AD9854_REG_RAMPCLOCK1	0x1B
#define AD9854_REG_RAMPCLOCK0	0x1C

#define AD9854_REG_CTRL3		0x1D
#define AD9854_REG_CTRL2		0x1E
#define AD9854_REG_CTRL1		0x1F
#define AD9854_REG_CTRL0		0x20

#define AD9854_REG_AMPLIH		0x21
#define AD9854_REG_AMPLIL		0x22

#define AD9854_REG_AMPLQH		0x23
#define AD9854_REG_AMPLQL		0x24

#define AD9854_REG_OSKEYRAMP	0x25

#define AD9854_REG_QDACH		0x26
#define AD9854_REG_QDACL		0x27

// mode defines
#define AD9854_MODE_SINGLE		0x00
#define AD9854_MODE_FSK			0x01
#define AD9854_MODE_RAMPFSK		0x02
#define AD9854_MODE_CHIRP		0x03
#define AD9854_MODE_BPSK		0x04

// control register
#define AD9854_INVSINC_BYPASS	BIT(6)

#define AD9854_INT_IO_UPDATE	BIT(8)
#define AD9854_MODE_MASK		0x00000E00
#define AD9854_MODE0			BIT(9)
#define AD9854_MODE1			BIT(10)
#define AD9854_MODE2			BIT(11)

#define AD9854_TRIANGLE			BIT(13)
#define AD9854_CLR_ACC2			BIT(14)
#define AD9854_CLR_ACC1			BIT(15)

#define AD9854_PLL_MULT_MASK	0x001F0000
#define AD9854_PLL_MULT0		BIT(16)
#define AD9854_PLL_MULT1		BIT(17)
#define AD9854_PLL_MULT2		BIT(18)
#define AD9854_PLL_MULT3		BIT(19)
#define AD9854_PLL_MULT4		BIT(20)
#define AD9854_PLL_BYPASS		BIT(21)
#define AD9854_PLL_RANGE		BIT(22)
#define AD9854_PD_DIG			BIT(24)
#define AD9854_PD_DAC			BIT(25)
#define AD9854_PD_QDAC			BIT(26)
#define AD9854_PD_COMP			BIT(28)

void ad9854Init(void);
void ad9854SetupIOUpdate(int state);
void ad9854ClockIOUpdate(void);

void ad9854WritePhase1(u16 phase);
void ad9854WritePhase2(u16 phase);
void ad9854WriteFreq1(u64 freq);
void ad9854WriteFreq2(u64 freq);
void ad9854WriteDeltaFreq(u64 freq);
void ad9854WriteUpdateClock(u32 updclock);
void ad9854WriteRampRateClock(u32 rampclock);
void ad9854WriteControlReg(u32 cr);
void ad9854WriteAmplitudeI(u16 ampi);
void ad9854WriteAmplitudeQ(u16 ampq);
void ad9854WriteOSKeyRampRate(u16 ramprate);
void ad9854WriteQDAC(u16 qdac);

u32 ad9854ReadControlReg(void);
void ad9854ControlClear(u32 cr_clear);
void ad9854ControlSet(u32 cr_set);


void ad9854SetMode(int mode);
void ad9854SetPLL(int enabled, int mult);

void ad9854SetFreq1(u32 freqHz);
void ad9854SetUpdateClock(u32 updclockHz);

void ad9854ShowRegisters(void);


#endif
