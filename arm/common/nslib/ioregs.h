/*
 * ioregs.h --
 *
 *	Declarations of macros for accessing IO registers in the ARM7500.
 *
 * Copyright (c) 1996 Ben Williamson.
 * All rights reserved.
 *
 * This file is part of nslib, a library used by programs
 * running on the Netslate Development Board.
 *
 * This software is released under the GNU Public License.
 * See the file COPYING for more information.
 */

#ifndef _IOREGS_H
#define _IOREGS_H

#define IO(a)		*((volatile unsigned int *)(0x03200000 + (a)))

#define IOCR		IO(0x00)
#define KBDDAT		IO(0x04)
#define KBDCR		IO(0x08)
#define IOLINES		IO(0x0C)
#define IRQSTA		IO(0x10)
#define IRQRQA		IO(0x14)
#define IRQMSKA		IO(0x18)
#define SUSMODE		IO(0x1C)
#define IRQSTB		IO(0x20)
#define IRQRQB		IO(0x24)
#define IRQMSKB		IO(0x28)
#define STOPMODE	IO(0x2C)
#define FIQST		IO(0x30)
#define FIQRQ		IO(0x34)
#define FIQMSK		IO(0x38)
#define CLKCTL		IO(0x3C)
#define T0LOW		IO(0x40)
#define T0HIGH		IO(0x44)
#define T0GO		IO(0x48)
#define T0LAT		IO(0x4C)
#define T1LOW		IO(0x50)
#define T1HIGH		IO(0x54)
#define T1GO		IO(0x58)
#define T1LAT		IO(0x5C)
#define IRQSTC		IO(0x60)
#define IRQRQC		IO(0x64)
#define IRQMSKC		IO(0x68)
#define VIDMUX		IO(0x6C)
#define IRQSTD		IO(0x70)
#define IRQRQD		IO(0x74)
#define IRQMSKD		IO(0x78)
#define ROMCR0		IO(0x80)
#define ROMCR1		IO(0x84)
#define REFCR		IO(0x8C)
#define ID0		IO(0x94)
#define ID1		IO(0x98)
#define VERSION		IO(0x9C)
#define MSEDAT		IO(0xA8)
#define MSECR		IO(0xAC)
#define IOTCR		IO(0xC4)
#define ECTCR		IO(0xC8)
#define ASTCR		IO(0xCC)
#define DRAMWID		IO(0xD0)
#define SELFREF		IO(0xD4)
#define ATODICR		IO(0xE0)
#define ATODSR		IO(0xE4)
#define ATODCC		IO(0xE8)
#define ATODCNT1	IO(0xEC)
#define ATODCNT2	IO(0xF0)
#define ATODCNT3	IO(0xF4)
#define ATODCNT4	IO(0xF8)
#define SDCURA		IO(0x180)
#define SDENDA		IO(0x184)
#define SDCURB		IO(0x188)
#define SDENDB		IO(0x18C)
#define SDCR		IO(0x190)
#define SDST		IO(0x194)
#define CURSCUR		IO(0x1C0)
#define CURSINIT	IO(0x1C4)
#define VIDCURB		IO(0x1C8)
#define VIDCURA		IO(0x1D0)
#define VIDEND		IO(0x1D4)
#define VIDSTART	IO(0x1D8)
#define VIDINITA	IO(0x1DC)
#define VIDCR		IO(0x1E0)
#define VIDINITB	IO(0x1E8)
#define DMAST		IO(0x1F0)
#define DMARQ		IO(0x1F4)
#define DMAMSK		IO(0x1F8)


#endif /* _IOREGS_H */
