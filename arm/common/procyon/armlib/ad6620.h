/*! \file ad6620.h \brief Analog Devices AD6620 Digital Receiver Driver. */
//*****************************************************************************
//
// File Name	: 'ad6620.h'
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

#ifndef AD6620_H
#define AD6620_H

#include "global.h"

// defines

// hardware
#define AD6620_REFCLK_FREQ		50000000l
/*#define AD6620_IO_UPDATE		P04
#define AD6620_RESET			P05
*/

// base address
#define AD6620_REG_BASE			0x81000200

// external interface registers
#define AD6620_REG_DR0			0x00
#define AD6620_REG_DR1			0x01
#define AD6620_REG_DR2			0x02
#define AD6620_REG_DR3			0x03
#define AD6620_REG_DR4			0x04
#define AD6620_REG_LAR			0x06
#define AD6620_REG_AMR			0x07

// internal registers and RAM
#define AD6620_REG_RCF_COEFF	0x000
#define AD6620_REG_RCF_DATA		0x100
#define AD6620_REG_MODE_CTRL	0x300
#define AD6620_REG_NCO_CTRL		0x301
#define AD6620_REG_NCO_SYNC		0x302
#define AD6620_REG_NCO_FREQ		0x303
#define AD6620_REG_NCO_PHASE	0x304

#define AD6620_REG_CIC2_SCALE	0x305
#define AD6620_REG_CIC2_DECIM	0x306

#define AD6620_REG_CIC5_SCALE	0x307
#define AD6620_REG_CIC5_DECIM	0x308

#define AD6620_REG_RCF_CTRL		0x309
#define AD6620_REG_RCF_DECIM	0x30A
#define AD6620_REG_RCFADDR_OFS	0x30B
#define AD6620_REG_RCF_TAPS		0x30C

#define AD6620_REG_RESERVED		0x30D

// register values
#define AD6620_NCO_BYPASS		0x01
#define AD6620_NCO_PHASE_DITHER	0x02
#define AD6620_NCO_AMPL_DITHER	0x04

void ad6620Init(void);

u32 ad6620ReadReg(u16 reg);
void ad6620WriteReg(u16 reg, u32 data);
void ad6620SetNCOMode(u08 mode);
void ad6620SetNCOFreq(u32 freqHz);
void ad6620SetNCOPhase(u16 phase);
void ad6620LoadRCFilter(int start, int taps, s32* coeffs);

void ad6620ShowRegisters(void);


#endif
