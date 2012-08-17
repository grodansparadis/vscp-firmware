/*! \file membus.h \brief Memory Bus Driver for ADuC7026. */
//*****************************************************************************
//
// File Name	: 'membus.h'
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

#ifndef MEMBUS_H
#define MEMBUS_H

#include "global.h"

// defines
#define XMCON_16BIT			(1<<1)
#define XMCON_ENABLE		(1<<0)

#define XMPAR_BWS			(1<<15)
#define XMPAR_AE_WS2		(1<<14)
#define XMPAR_AE_WS1		(1<<13)
#define XMPAR_AE_WS0		(1<<12)
#define XMPAR_16BITADDR		(1<<11)
#define XMPAR_EXTRAHOLD		(1<<10)
#define XMPAR_EXTRAREAD		(1<<9)
#define XMPAR_EXTRAWRITE	(1<<8)
#define XMPAR_WR_WS3		(1<<7)
#define XMPAR_WR_WS2		(1<<6)
#define XMPAR_WR_WS1		(1<<5)
#define XMPAR_WR_WS0		(1<<4)
#define XMPAR_RD_WS3		(1<<3)
#define XMPAR_RD_WS2		(1<<2)
#define XMPAR_RD_WS1		(1<<1)
#define XMPAR_RD_WS0		(1<<0)

// memory bank addresses
#define XM_ADDR_MEMORY0		0x10000000l
#define XM_ADDR_MEMORY1		0x20000000l
#define XM_ADDR_MEMORY2		0x30000000l
#define XM_ADDR_MEMORY3		0x40000000l

// functions
void membusInit(void);
uint16_t membusRead(uint32_t addr);
void membusWrite(uint32_t addr, uint16_t data);

#endif
