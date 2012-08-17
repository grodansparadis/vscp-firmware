/*! \file po1030.h \brief PO1030 Imager Interface Driver. */
//*****************************************************************************
//
// File Name	: 'po1030.h'
// Title		: PO1030 Imager Interface Driver
// Author		: Pascal Stang - Copyright (C) 2006
// Created		: 2006.02.01
// Revised		: 2006.02.01
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

#ifndef PO1030_H
#define PO1030_H

// device base address
#define PO1030_I2C_BASE_ADDR	0x6E

// if addr bit7 set, indicates word register (16-bit)
#define PO1030_REG_MASK				0x7F
#define PO1030_REG_16BIT			0x80

// register defines
#define PO1030_REG_DEVID			(0x00|PO1030_REG_16BIT)

#define PO1030_REG_FRAMEWIDTH		(0x04|PO1030_REG_16BIT)
#define PO1030_REG_FRAMEHEIGHT		(0x06|PO1030_REG_16BIT)
#define PO1030_REG_WINDOWX			(0x08|PO1030_REG_16BIT)
#define PO1030_REG_WINDOWY			(0x0A|PO1030_REG_16BIT)
#define PO1030_REG_WINDOWWIDTH		(0x0C|PO1030_REG_16BIT)
#define PO1030_REG_WINDOWHIEGHT		(0x0E|PO1030_REG_16BIT)

#define PO1030_REG_GLOBALIBIAS		(0x12)
#define PO1030_REG_PIXELIBIAS		(0x13)
#define PO1030_REG_GLOBALGAIN		(0x15)

#define PO1030_REG_RGAIN			(0x16)
#define PO1030_REG_G1GAIN			(0x17)
#define PO1030_REG_BGAIN			(0x18)
#define PO1030_REG_G2GAIN			(0x19)

#define PO1030_REG_INTEGLINESH		(0x1A)
#define PO1030_REG_INTEGLINESM		(0x1B)
#define PO1030_REG_INTEGLINESL		(0x1C)

#define PO1030_REG_CONTROL1			(0x1D)
#define PO1030_REG_CONTROL2			(0x1E)
#define PO1030_REG_CONTROL3			(0x1F)
#define PO1030_REG_CONTROL4			(0x20)

#define PO1030_REG_PERIOD50			(0x23|PO1030_REG_16BIT)
#define PO1030_REG_PERIOD60			(0x25|PO1030_REG_16BIT)
#define PO1030_REG_REGCLK167		(0x27)
#define PO1030_REG_DELTA50			(0x28)
#define PO1030_REG_DELTA60			(0x29)

#define PO1030_REG_ADCOFFSET		(0x2C)
// Gamma Correction Coeffs
#define PO1030_REG_GC0				(0x2D)
#define PO1030_REG_GC1				(0x2E)
#define PO1030_REG_GC2				(0x2F)
#define PO1030_REG_GC3				(0x30)
#define PO1030_REG_GC4				(0x31)
#define PO1030_REG_GC5				(0x32)
#define PO1030_REG_GC6				(0x33)
#define PO1030_REG_GC7				(0x34)
// Color Transform Matrix
#define PO1030_REG_CT0				(0x35)
#define PO1030_REG_CT1				(0x36)
#define PO1030_REG_CT2				(0x37)
#define PO1030_REG_CT3				(0x38)
#define PO1030_REG_CT4				(0x39)
#define PO1030_REG_CT5				(0x3A)
#define PO1030_REG_CT6				(0x3B)
#define PO1030_REG_CT7				(0x3C)
#define PO1030_REG_CT8				(0x3D)

#define PO1030_REG_AUTOCTRL1		(0x3E)
#define PO1030_REG_AUTOCTRL2		(0x3F)

#define PO1030_REG_YTARGET			(0x40)
#define PO1030_REG_GLOBALGAINMIN	(0x41)
#define PO1030_REG_GLOBALGAINMAX	(0x42)
// Cutput format control
#define PO1030_REG_OUTFORMCTRL1		(0x5A)
#define PO1030_REG_OUTFORMCTRL2		(0x5B)
#define PO1030_REG_OUTFORMCTRL3		(0x5C)
#define PO1030_REG_OUTFORMCTRL4		(0x5D)
#define PO1030_REG_OUTFORMCTRL5		(0x5E)
// Imaging coefficients
#define PO1030_REG_YBRIGHT			(0x73)
#define PO1030_REG_YCONTRAST		(0x74)
#define PO1030_REG_YSATURATION		(0x75)


// functions

// PO1030 interface initializer
void po1030Init(void);

u16 po1030GetID(void);

// read/write register
u16 po1030RegRead(u08 regaddr);
void po1030RegWrite(u08 regaddr, u16 value);

#endif
