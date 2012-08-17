/*! \file a2d.h \brief Analog-to-Digital Converte Driver for LPC2000. */
//*****************************************************************************
//
// File Name	: 'a2d.h'
// Title		: Analog-to-Digital Converte Driver for LPC2000
// Author		: Pascal Stang - Copyright (C) 2006
// Created		: 2006.04.09
// Revised		: 2006.04.15
// Version		: 0.1
// Target MCU	: LPC2000 ARM processors
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef A2D_H
#define A2D_H

#include "global.h"

// register bits
#define ADCR_SEL_MASK		(0xFF)
#define ADCR_CLKDIV_MASK	(0xFF<<8)
#define ADCR_BURST			(1<<16)

#define ADCR_CLKS_10BIT		(0<<17)
#define ADCR_CLKS_9BIT		(1<<17)
#define ADCR_CLKS_8BIT		(2<<17)
#define ADCR_CLKS_7BIT		(3<<17)
#define ADCR_CLKS_6BIT		(4<<17)
#define ADCR_CLKS_5BIT		(5<<17)
#define ADCR_CLKS_4BIT		(6<<17)
#define ADCR_CLKS_3BIT		(7<<17)
#define ADCR_CLKS_MASK		(7<<17)

#define ADCR_PDN			(1<<21)

#define ADCR_START_NOW		(1<<24)
#define ADCR_START_MASK		(7<<24)

#define ADCR_EDGE			(1<<27)

#define ADDR_DONE			(1<<31)
#define ADDR_OVERUN			(1<<31)
#define ADDR_CHN_MASK		(7<<24)
#define ADDR_VALUE_MASK		(0xFFFF)

void a2dInit(void);
int a2dConvert(int channel);

#endif
