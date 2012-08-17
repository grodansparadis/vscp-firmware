/*! \file membus.c \brief Memory Bus Driver for ADuC7026. */
//*****************************************************************************
//
// File Name	: 'membus.c'
// Title		: Memory Bus Driver for ADuC7026
// Author		: Pascal Stang - Copyright (C) 2005
// Created		: 2005.02.14
// Revised		: 2005.02.14
// Version		: 0.1
// Target MCU	: Analog Devices ADuC7026 ARM Series
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

#include "aduc7026.h"
#include "global.h"
#include "membus.h"

void membusInit(void)
{
	// enable external memory bus
	XMCFG = 0x01;
	// setup I/O lines
	// setup control lines
	GP2CON |= 0x22222220;
	// setup AD0-15
	GP3CON |= 0x22222222;
	GP4CON |= 0x22222222;

	// setup all memory banks as 16-bit
	XM0CON = XMCON_16BIT | XMCON_ENABLE;
	XM1CON = XMCON_16BIT | XMCON_ENABLE;
	XM2CON = XMCON_16BIT | XMCON_ENABLE;
	XM3CON = XMCON_16BIT | XMCON_ENABLE;

	XM0PAR = XMPAR_16BITADDR | XMPAR_EXTRAHOLD;
	XM1PAR = XMPAR_16BITADDR | XMPAR_EXTRAHOLD;
	XM2PAR = XMPAR_16BITADDR | XMPAR_EXTRAHOLD;
	XM3PAR = XMPAR_16BITADDR | XMPAR_EXTRAHOLD;
}

uint16_t membusRead(uint32_t addr)
{
	return *((unsigned short*)addr);
}

void membusWrite(uint32_t addr, uint16_t data)
{
	*((unsigned short*)addr) = data;
}
