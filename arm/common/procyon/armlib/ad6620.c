/*! \file ad6620.c \brief Analog Devices AD6620 Digital Receiver Driver. */
//*****************************************************************************
//
// File Name	: 'ad6620.c'
// Title		: Analog Devices AD6620 Digital Receiver Driver
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 2004.07.08
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
#include "ad6620.h"

// global variables

void ad6620Write(unsigned int addr, unsigned char data)
{
	((volatile u16*)AD6620_REG_BASE)[addr] = data;
}

unsigned char ad6620Read(unsigned int addr)
{
	return ((volatile u16*)AD6620_REG_BASE)[addr];
}

void ad6620Init(void)
{
	//membusInit();

	// reset device
	//IOSET = AD9854_RESET;
	//IODIR |= AD9854_RESET;
	//delay(10000);
	//IOCLR = AD9854_RESET;

	// initialize registers

	// initialize receiver - set soft reset
	ad6620WriteReg(AD6620_REG_MODE_CTRL, 0x01);
	// initialize RCF filter coeffs
	ad6620WriteReg(AD6620_REG_RCF_COEFF, 0x20000);
	// set NCO
	ad6620WriteReg(AD6620_REG_NCO_CTRL, 0);
	ad6620WriteReg(AD6620_REG_NCO_SYNC, 0);
	ad6620WriteReg(AD6620_REG_NCO_FREQ, 0);
	ad6620WriteReg(AD6620_REG_NCO_PHASE, 0);
	// set CIC2 filter
	ad6620WriteReg(AD6620_REG_CIC2_SCALE, 6);
	ad6620WriteReg(AD6620_REG_CIC2_DECIM, 16-1);
//	ad6620WriteReg(AD6620_REG_CIC2_SCALE, 2);
//	ad6620WriteReg(AD6620_REG_CIC2_DECIM, 4-1);
	// set CIC5 filter
	ad6620WriteReg(AD6620_REG_CIC5_SCALE, 20);
	ad6620WriteReg(AD6620_REG_CIC5_DECIM, 32-1);
//	ad6620WriteReg(AD6620_REG_CIC5_SCALE, 12);
//	ad6620WriteReg(AD6620_REG_CIC5_DECIM, 10-1);
	// set RCF filter
	ad6620WriteReg(AD6620_REG_RCF_CTRL, 1);
	ad6620WriteReg(AD6620_REG_RCF_DECIM, 32-1);
	ad6620WriteReg(AD6620_REG_RCFADDR_OFS, 0);
	ad6620WriteReg(AD6620_REG_RCF_TAPS, 0);
//	ad6620WriteReg(AD6620_REG_RCF_TAPS, 32-1);
	// set reserved register to zero
	ad6620WriteReg(AD6620_REG_RESERVED, 0x00);
	// initialize receiver - set operation mode
	ad6620WriteReg(AD6620_REG_MODE_CTRL, 0x08);

}

void ad6620WriteReg(u16 reg, u32 data)
{
	// write address
	ad6620Write(AD6620_REG_AMR, reg>>8);
	ad6620Write(AD6620_REG_LAR, reg);
	// write value
	ad6620Write(AD6620_REG_DR3, data>>24);
	ad6620Write(AD6620_REG_DR2, data>>16);
	ad6620Write(AD6620_REG_DR1, data>>8);
	ad6620Write(AD6620_REG_DR0, data);
}


u32 ad6620ReadReg(u16 reg)
{
	u32 data;
	// write address
	ad6620Write(AD6620_REG_AMR, reg>>8);
	ad6620Write(AD6620_REG_LAR, reg);
	// read value
	data  = (ad6620Read(AD6620_REG_DR0) & 0x000000FF);
	data |= (ad6620Read(AD6620_REG_DR1) & 0x000000FF)<<8;
	data |= (ad6620Read(AD6620_REG_DR2) & 0x000000FF)<<16;
	data |= (ad6620Read(AD6620_REG_DR3) & 0x000000FF)<<24;
	return data;
}

void ad6620SetNCOMode(u08 mode)
{
	ad6620WriteReg(AD6620_REG_NCO_CTRL, mode);
}

void ad6620SetNCOFreq(u32 freqHz)
{
	u32 nco_freq;
	nco_freq = ((u64)freqHz<<32)/(AD6620_REFCLK_FREQ);
	ad6620WriteReg(AD6620_REG_NCO_FREQ, nco_freq);
}

void ad6620SetNCOPhase(u16 phase)
{
	ad6620WriteReg(AD6620_REG_NCO_PHASE, phase);
}

void ad6620LoadRCFilter(int start, int taps, s32* coeffs)
{
	int i;
	// load RCF filter taps into receiver
	for(i=0;i<taps;i++)
	{
		ad6620WriteReg(AD6620_REG_RCF_COEFF+start+i, coeffs[i]>>12);
	}
}


void ad6620ShowRegisters(void)
{
	// read and print all registers

	rprintfStr("Mode Control      :       0x");
	rprintfu08(ad6620ReadReg(AD6620_REG_MODE_CTRL));
	rprintfCRLF();

	rprintfStr("NCO Control       :       0x");
	rprintfu08(ad6620ReadReg(AD6620_REG_NCO_CTRL));
	rprintfCRLF();

	rprintfStr("NCO SYNC Control  : 0x");
	rprintfu32(ad6620ReadReg(AD6620_REG_NCO_SYNC));
	rprintfCRLF();

	rprintfStr("NCO Frequency     : 0x");
	rprintfu32(ad6620ReadReg(AD6620_REG_NCO_FREQ));
	rprintfCRLF();

	rprintfStr("NCO Phase Offset  :     0x");
	rprintfu16(ad6620ReadReg(AD6620_REG_NCO_PHASE));
	rprintfCRLF();

	rprintfStr("INPUT/CIC2 Scale  :       0x");
	rprintfu08(ad6620ReadReg(AD6620_REG_CIC2_SCALE));
	rprintfCRLF();

	rprintfStr("CIC2 Decimation   :       0x");
	rprintfu08(ad6620ReadReg(AD6620_REG_CIC2_DECIM));
	rprintfCRLF();

	rprintfStr("CIC5 Scale        :       0x");
	rprintfu08(ad6620ReadReg(AD6620_REG_CIC5_SCALE));
	rprintfCRLF();

	rprintfStr("CIC5 Decimation   :       0x");
	rprintfu08(ad6620ReadReg(AD6620_REG_CIC5_DECIM));
	rprintfCRLF();

	rprintfStr("OUTPUT/RCF Control:       0x");
	rprintfu08(ad6620ReadReg(AD6620_REG_RCF_CTRL));
	rprintfCRLF();

	rprintfStr("RCF Decimation    :       0x");
	rprintfu08(ad6620ReadReg(AD6620_REG_RCF_DECIM));
	rprintfCRLF();

	rprintfStr("RCF Address Offset:       0x");
	rprintfu08(ad6620ReadReg(AD6620_REG_RCFADDR_OFS));
	rprintfCRLF();

	rprintfStr("RCF Taps          :       0x");
	rprintfu08(ad6620ReadReg(AD6620_REG_RCF_TAPS));
	rprintfCRLF();
}
	
