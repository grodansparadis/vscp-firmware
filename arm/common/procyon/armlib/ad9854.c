/*! \file ad9854.c \brief Analog Devices AD9854 Digital Synthesizer Driver. */
//*****************************************************************************
//
// File Name	: 'ad9854.c'
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

#include "lpc2000.h"

#include "global.h"
#include "timer.h"
#include "membus.h"
#include "rprintf.h"
#include "ad9854.h"

// global variables
int Ad9854RefClkMult;


void ad9854Write(unsigned int addr, unsigned char data)
{
	((u16*)AD9854_REG_BASE)[addr] = data;
}

unsigned char ad9854Read(unsigned int addr)
{
	return ((u16*)AD9854_REG_BASE)[addr];
}

void ad9854Init(void)
{
	//membusInit();

/*	// reset device
	IOSET = AD9854_RESET;
	IODIR |= AD9854_RESET;
	delay(10000);
	IOCLR = AD9854_RESET;
*/
	// setup IO Update pin
	ad9854SetupIOUpdate(0);

	// initialize variables
	Ad9854RefClkMult = 1;
}

void ad9854SetupIOUpdate(int state)
{
	// clear IO Update pin
//	IOCLR = AD9854_IO_UPDATE;
/*
	if(state)
		// set IO update line to output
		IODIR |= AD9854_IO_UPDATE;
	else
		// set IO update line to input
		IODIR &= ~AD9854_IO_UPDATE;
*/
}

void ad9854ClockIOUpdate(void)
{
	// assert IO Update line
//	IOSET = AD9854_IO_UPDATE;
	// wait
	asm volatile ("nop\r\n nop\r\n");
	// clear IO Update line
//	IOCLR = AD9854_IO_UPDATE;
}

void ad9854WritePhase1(u16 phase)
{
	ad9854Write(AD9854_REG_PHASE1L, phase);
	ad9854Write(AD9854_REG_PHASE1H, phase>>8);
}

void ad9854WritePhase2(u16 phase)
{
	ad9854Write(AD9854_REG_PHASE2L, phase);
	ad9854Write(AD9854_REG_PHASE2H, phase>>8);
}

void ad9854WriteFreq1(u64 freq)
{
	ad9854Write(AD9854_REG_FREQ10, freq);
	ad9854Write(AD9854_REG_FREQ11, freq>>8);
	ad9854Write(AD9854_REG_FREQ12, freq>>16);
	ad9854Write(AD9854_REG_FREQ13, freq>>24);
	ad9854Write(AD9854_REG_FREQ14, freq>>32);
	ad9854Write(AD9854_REG_FREQ15, freq>>40);
}

void ad9854WriteFreq2(u64 freq)
{
	ad9854Write(AD9854_REG_FREQ20, freq);
	ad9854Write(AD9854_REG_FREQ21, freq>>8);
	ad9854Write(AD9854_REG_FREQ22, freq>>16);
	ad9854Write(AD9854_REG_FREQ23, freq>>24);
	ad9854Write(AD9854_REG_FREQ24, freq>>32);
	ad9854Write(AD9854_REG_FREQ25, freq>>40);
}

void ad9854WriteDeltaFreq(u64 freq)
{
	ad9854Write(AD9854_REG_DELTA0, freq);
	ad9854Write(AD9854_REG_DELTA1, freq>>8);
	ad9854Write(AD9854_REG_DELTA2, freq>>16);
	ad9854Write(AD9854_REG_DELTA3, freq>>24);
	ad9854Write(AD9854_REG_DELTA4, freq>>32);
	ad9854Write(AD9854_REG_DELTA5, freq>>48);
}

void ad9854WriteUpdateClock(u32 updclock)
{
	ad9854Write(AD9854_REG_UPDCLOCK0, updclock);
	ad9854Write(AD9854_REG_UPDCLOCK1, updclock>>8);
	ad9854Write(AD9854_REG_UPDCLOCK2, updclock>>16);
	ad9854Write(AD9854_REG_UPDCLOCK3, updclock>>24);
}

void ad9854WriteRampRateClock(u32 rampclock)
{
	ad9854Write(AD9854_REG_RAMPCLOCK0, rampclock);
	ad9854Write(AD9854_REG_RAMPCLOCK1, rampclock>>8);
	ad9854Write(AD9854_REG_RAMPCLOCK2, rampclock>>16);
}

void ad9854WriteControlReg(u32 cr)
{
	ad9854Write(AD9854_REG_CTRL0, cr);
	ad9854Write(AD9854_REG_CTRL1, cr>>8);
	ad9854Write(AD9854_REG_CTRL2, cr>>16);
	ad9854Write(AD9854_REG_CTRL3, cr>>24);
}

void ad9854WriteAmplitudeI(u16 ampi)
{
	ad9854Write(AD9854_REG_AMPLIL, ampi);
	ad9854Write(AD9854_REG_AMPLIH, ampi>>8);
}

void ad9854WriteAmplitudeQ(u16 ampq)
{
	ad9854Write(AD9854_REG_AMPLQL, ampq);
	ad9854Write(AD9854_REG_AMPLQH, ampq>>8);
}

void ad9854WriteOSKeyRampRate(u16 ramprate)
{
	ad9854Write(AD9854_REG_OSKEYRAMP, ramprate);
}

void ad9854WriteQDAC(u16 qdac)
{
	ad9854Write(AD9854_REG_QDACL, qdac);
	ad9854Write(AD9854_REG_QDACH, qdac>>8);
}

u32 ad9854ReadControlReg(void)
{
	u32 cr=0;
	cr  = (ad9854Read(AD9854_REG_CTRL0) & 0x000000FF);
	cr |= (ad9854Read(AD9854_REG_CTRL1) & 0x000000FF)<<8;
	cr |= (ad9854Read(AD9854_REG_CTRL2) & 0x000000FF)<<16;
	cr |= (ad9854Read(AD9854_REG_CTRL3) & 0x000000FF)<<24;
	return cr;
}

void ad9854ControlClear(u32 cr_clear)
{
	u32 cr;
	// read in control register
	cr = ad9854ReadControlReg();
	// clear bits
	cr &= ~cr_clear;
	// write control register
	ad9854WriteControlReg(cr);
}

void ad9854ControlSet(u32 cr_set)
{
	u32 cr;
	// read in control register
	cr = ad9854ReadControlReg();
	// clear bits
	cr |= cr_set;
	// write control register
	ad9854WriteControlReg(cr);
}

void ad9854SetMode(int mode)
{
	u32 cr;
	// read in control register
	cr = ad9854ReadControlReg();
	// set mode bits
	cr &= ~AD9854_MODE_MASK;
	cr |= (mode<<9) & AD9854_MODE_MASK;
	// write control register
	ad9854WriteControlReg(cr);
}

void ad9854SetPLL(int enabled, int mult)
{
	u32 cr;
	// read in control register
	cr = ad9854ReadControlReg();
	// set PLL state
	if(enabled)
	{
		cr &= ~AD9854_PLL_BYPASS;
		// set local state reference
		Ad9854RefClkMult = mult;
	}
	else
	{
		cr |= AD9854_PLL_BYPASS;
		// set local state reference
		Ad9854RefClkMult = 1;
	}
	// set PLL multiplication ratio
	cr &= ~AD9854_PLL_MULT_MASK;
	cr |= (mult<<16) & AD9854_PLL_MULT_MASK;
	// write control register
	ad9854WriteControlReg(cr);
}

void ad9854SetFreq1(u32 freqHz)
{
	u64 ftw;
	// FTW = (Desired Output Frequency × 2^N)/SYSCLK
	// where N = 48 bits
	
	// split up the 2^N multiplication to avoid overflow/underflow
	if(freqHz < 0x1000)
	{
		ftw = (((u64)freqHz<<33)/(AD9854_REFCLK_FREQ*Ad9854RefClkMult))<<15;
	}
	else if(freqHz < 0x10000)
	{
		ftw = (((u64)freqHz<<32)/(AD9854_REFCLK_FREQ*Ad9854RefClkMult))<<16;
	}
	else if(freqHz < 0x1000000)
	{
		ftw = (((u64)freqHz<<32)/(AD9854_REFCLK_FREQ*Ad9854RefClkMult))<<16;
		//ftw = (((u64)freqHz<<30)/(AD9854_REFCLK_FREQ*Ad9854RefClkMult))<<18;
	}
	else
	{
		ftw = (((u64)freqHz<<32)/(AD9854_REFCLK_FREQ*Ad9854RefClkMult))<<16;
		//ftw = (((u64)freqHz<<20)/(AD9854_REFCLK_FREQ*Ad9854RefClkMult))<<28;
	}
	ad9854WriteFreq1(ftw);
}

void ad9854SetUpdateClock(u32 updclockHz)
{
	u32 updclock;
	
	if(updclockHz)
	{
		// use internal update clock
		// Update period = (N+1) × (SYSTEM CLOCK PERIOD × 2)
		// Update freq = REFCLK_FREQ/(2*(N+1))
		// N = ((REFCLK_FREQ*Ad9854RefClkMult)/(2*UpdateHz))-1
		updclock = ((AD9854_REFCLK_FREQ*Ad9854RefClkMult)/(2*updclockHz))-1;

		// turn off internal update
		ad9854ControlClear(AD9854_INT_IO_UPDATE);
		// write update counter register
		ad9854WriteUpdateClock(updclock);
		// turn on internal update
		ad9854ControlSet(AD9854_INT_IO_UPDATE);
		// manually clock update pin
		ad9854SetupIOUpdate(1);
		ad9854ClockIOUpdate();
		ad9854SetupIOUpdate(0);
	}
	else
	{
		// turn off internal update
		ad9854ControlClear(AD9854_INT_IO_UPDATE);
		// use external update clock
		ad9854SetupIOUpdate(0);
	}

}

void ad9854ShowRegisters(void)
{
	// read and print all registers

	rprintfStr("Phase1      :         0x");
	rprintfu08(ad9854Read(AD9854_REG_PHASE1H));
	rprintfu08(ad9854Read(AD9854_REG_PHASE1L));
	rprintfCRLF();

	rprintfStr("Phase2      :         0x");
	rprintfu08(ad9854Read(AD9854_REG_PHASE2H));
	rprintfu08(ad9854Read(AD9854_REG_PHASE2L));
	rprintfCRLF();

	rprintfStr("Freq1       : 0x");
	rprintfu08(ad9854Read(AD9854_REG_FREQ15));
	rprintfu08(ad9854Read(AD9854_REG_FREQ14));
	rprintfu08(ad9854Read(AD9854_REG_FREQ13));
	rprintfu08(ad9854Read(AD9854_REG_FREQ12));
	rprintfu08(ad9854Read(AD9854_REG_FREQ11));
	rprintfu08(ad9854Read(AD9854_REG_FREQ10));
	rprintfCRLF();

	rprintfStr("Freq2       : 0x");
	rprintfu08(ad9854Read(AD9854_REG_FREQ25));
	rprintfu08(ad9854Read(AD9854_REG_FREQ24));
	rprintfu08(ad9854Read(AD9854_REG_FREQ23));
	rprintfu08(ad9854Read(AD9854_REG_FREQ22));
	rprintfu08(ad9854Read(AD9854_REG_FREQ21));
	rprintfu08(ad9854Read(AD9854_REG_FREQ20));
	rprintfCRLF();

	rprintfStr("DeltaFreq   : 0x");
	rprintfu08(ad9854Read(AD9854_REG_DELTA5));
	rprintfu08(ad9854Read(AD9854_REG_DELTA4));
	rprintfu08(ad9854Read(AD9854_REG_DELTA3));
	rprintfu08(ad9854Read(AD9854_REG_DELTA2));
	rprintfu08(ad9854Read(AD9854_REG_DELTA1));
	rprintfu08(ad9854Read(AD9854_REG_DELTA0));
	rprintfCRLF();

	rprintfStr("Update Clock:     0x");
	rprintfu08(ad9854Read(AD9854_REG_UPDCLOCK3));
	rprintfu08(ad9854Read(AD9854_REG_UPDCLOCK2));
	rprintfu08(ad9854Read(AD9854_REG_UPDCLOCK1));
	rprintfu08(ad9854Read(AD9854_REG_UPDCLOCK0));
	rprintfCRLF();

	rprintfStr("Ramp Rate   :       0x");
	rprintfu08(ad9854Read(AD9854_REG_RAMPCLOCK2));
	rprintfu08(ad9854Read(AD9854_REG_RAMPCLOCK1));
	rprintfu08(ad9854Read(AD9854_REG_RAMPCLOCK0));
	rprintfCRLF();

	rprintfStr("Control     :     0x");
	rprintfu08(ad9854Read(AD9854_REG_CTRL3));
	rprintfu08(ad9854Read(AD9854_REG_CTRL2));
	rprintfu08(ad9854Read(AD9854_REG_CTRL1));
	rprintfu08(ad9854Read(AD9854_REG_CTRL0));
	rprintfCRLF();

	rprintfStr("Amplitude I :         0x");
	rprintfu08(ad9854Read(AD9854_REG_AMPLIH));
	rprintfu08(ad9854Read(AD9854_REG_AMPLIL));
	rprintfCRLF();

	rprintfStr("Amplitude Q :         0x");
	rprintfu08(ad9854Read(AD9854_REG_AMPLQH));
	rprintfu08(ad9854Read(AD9854_REG_AMPLQL));
	rprintfCRLF();

}
	
