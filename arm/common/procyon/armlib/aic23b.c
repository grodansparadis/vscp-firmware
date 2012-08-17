/*! \file aic23b.c \brief TI TLV320AIC23B Audio Codec Driver. */
//*****************************************************************************
//
// File Name	: 'aic23b.c'
// Title		: TI TLV320AIC23B Audio Codec Driver
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

#include "global.h"
#include "i2c.h"
#include "at91sam7s64.h"
#include "processor.h"
#include "aic23b.h"

// Sin wave table (256 steps, 16-bit output)
signed short SinTable256_16[] = {
     0,	   804,	  1608,	  2410,	  3212,	  4011,	  4808,	  5602,	
  6393,	  7179,	  7962,	  8739,	  9512,	 10278,	 11039,	 11793,	
 12539,	 13279,	 14010,	 14732,	 15446,	 16151,	 16846,	 17530,	
 18204,	 18868,	 19519,	 20159,	 20787,	 21403,	 22005,	 22594,	
 23170,	 23731,	 24279,	 24811,	 25329,	 25832,	 26319,	 26790,	
 27245,	 27683,	 28105,	 28510,	 28898,	 29268,	 29621,	 29956,	
 30273,	 30571,	 30852,	 31113,	 31356,	 31580,	 31785,	 31971,	
 32137,	 32285,	 32412,	 32521,	 32609,	 32678,	 32728,	 32757,	
 32767,	 32757,	 32728,	 32678,	 32609,	 32521,	 32412,	 32285,	
 32137,	 31971,	 31785,	 31580,	 31356,	 31113,	 30852,	 30571,	
 30273,	 29956,	 29621,	 29268,	 28898,	 28510,	 28105,	 27683,	
 27245,	 26790,	 26319,	 25832,	 25329,	 24811,	 24279,	 23731,	
 23170,	 22594,	 22005,	 21403,	 20787,	 20159,	 19519,	 18868,	
 18204,	 17530,	 16846,	 16151,	 15446,	 14732,	 14010,	 13279,	
 12539,	 11793,	 11039,	 10278,	  9512,	  8739,	  7962,	  7179,	
  6393,	  5602,	  4808,	  4011,	  3212,	  2410,	  1608,	   804,	
     0,	  -804,	 -1608,	 -2410,	 -3212,	 -4011,	 -4808,	 -5602,	
 -6393,	 -7179,	 -7962,	 -8739,	 -9512,	-10278,	-11039,	-11793,	
-12539,	-13279,	-14010,	-14732,	-15446,	-16151,	-16846,	-17530,	
-18204,	-18868,	-19519,	-20159,	-20787,	-21403,	-22005,	-22594,	
-23170,	-23731,	-24279,	-24811,	-25329,	-25832,	-26319,	-26790,	
-27245,	-27683,	-28105,	-28510,	-28898,	-29268,	-29621,	-29956,	
-30273,	-30571,	-30852,	-31113,	-31356,	-31580,	-31785,	-31971,	
-32137,	-32285,	-32412,	-32521,	-32609,	-32678,	-32728,	-32757,	
-32767,	-32757,	-32728,	-32678,	-32609,	-32521,	-32412,	-32285,	
-32137,	-31971,	-31785,	-31580,	-31356,	-31113,	-30852,	-30571,	
-30273,	-29956,	-29621,	-29268,	-28898,	-28510,	-28105,	-27683,	
-27245,	-26790,	-26319,	-25832,	-25329,	-24811,	-24279,	-23731,	
-23170,	-22594,	-22005,	-21403,	-20787,	-20159,	-19519,	-18868,	
-18204,	-17530,	-16846,	-16151,	-15446,	-14732,	-14010,	-13279,	
-12539,	-11793,	-11039,	-10278,	 -9512,	 -8739,	 -7962,	 -7179,	
 -6393,	 -5602,	 -4808,	 -4011,	 -3212,	 -2410,	 -1608,	  -804,	
};


void aicInit(void)
{
	i2cInit();

	aicWriteReg(AIC_REG_RESET);
	aicWriteReg(AIC_REG_POWERDOWN | BIT(AIC_PD_MIC));
	aicWriteReg(AIC_REG_ANALOG_PATH | BIT(AIC_AP_DAC));
	aicWriteReg(AIC_REG_DIGITAL_PATH | BIT(AIC_DP_DEEMP1));

	// preset volume
	aicSetVolumeHp(5);
}

int aicWriteReg(u16 regdata)
{
	u08 packet[2];
	packet[0] = regdata>>8;
	packet[1] = regdata;
	if(i2cMasterSend(AIC_I2C_ADDR>>1, 2, packet) == I2C_OK)
		return TRUE;
	else
		return FALSE;
}

void aicSetVolumeLineIn(u08 volume)
{
	// limit and bitmask
	volume = MIN(volume,31);
	volume &= 0x001F;
	// write to left and right channel
	aicWriteReg(AIC_REG_VOL_LINEIN_LEFT |
		volume | BIT(AIC_VLIL_LIM) | BIT(AIC_VLIL_LRS));
}

void aicSetVolumeHp(u08 volume)
{
	// add mute offset
	volume += 0x30;
	// limit and bitmask
	volume = MIN(volume,127);
	volume &= 0x007F;
	// write to left and right channel
	aicWriteReg(AIC_REG_VOL_HP_LEFT |
		volume | BIT(AIC_VHPL_LZC) | BIT(AIC_VHPL_LRS));
}

void aicDigitalEnable(void)
{
	// setup digital data format
	aicWriteReg(AIC_REG_INTERFACE_FORMAT|
		BIT(AIC_IF_MS)|
		AIC_IF_16BIT|
		AIC_IF_MSBL);

	// set sample rate
	aicWriteReg(AIC_REG_SAMPLE_RATE |
		BIT(AIC_SR_CLKIN) | BIT(AIC_SR_CLKOUT) | AIC_SR_18432_48KHZ);

	// activate digital interface
	aicWriteReg(AIC_REG_DIGITAL_IF_ACT | 0x01);
}
