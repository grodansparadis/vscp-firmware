/*
 * vidregs.h --
 *
 *	Declarations of macros for accessing the video (and sound)
 *	registers in the ARM7500.  All these registers are write only.
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

#ifndef _VIDREGS_H
#define _VIDREGS_H


#define VID(reg, val)	( *((volatile int *)0x03400000) = ((reg) | (val)))
#define SND(reg, val)	VID((reg), (val))

#define VideoPalette			0x00000000
#define VideoPaletteAddressRegister	0x10000000
#define LCDOffsetRegister0		0x30000000
#define LCDOffsetRegister1		0x31000000
#define BorderColourRegister		0x40000000
#define CursorPalette1			0x50000000
#define CursorPalette2			0x60000000
#define CursorPalette3			0x70000000
#define HorizontalCycleRegister		0x80000000
#define HorizontalSyncWidthRegister	0x81000000
#define HorizontalBorderStartRegister	0x82000000
#define HorizontalDisplayStartRegister	0x83000000
#define HorizontalDisplayEndRegister	0x84000000
#define HorizontalBorderEndRegister	0x85000000
#define HorizontalCursorStartRegister	0x86000000
#define VerticalCycleRegister		0x90000000
#define VerticalSyncWidthRegister	0x91000000
#define VerticalBorderStartRegister	0x92000000
#define VerticalDisplayStartRegister	0x93000000
#define VerticalDisplayEndRegister	0x94000000
#define VerticalBorderEndRegister	0x95000000
#define VerticalCursorStartRegister	0x96000000
#define VerticalCursorEndRegister	0x97000000
#define StereoImageRegister0		0xA0000000
#define StereoImageRegister1		0xA1000000
#define StereoImageRegister2		0xA2000000
#define StereoImageRegister3		0xA3000000
#define StereoImageRegister4		0xA4000000
#define StereoImageRegister5		0xA5000000
#define StereoImageRegister6		0xA6000000
#define StereoImageRegister7		0xA7000000
#define SoundFrequencyRegister		0xB0000000
#define SoundControlRegister		0xB1000000
#define ExternalRegister		0xC0000000
#define FrequencySynthesisRegister	0xD0000000
#define ControlRegister			0xE0000000
#define DataControlRegister		0xF0000000

#define HCR	HorizontalCycleRegister
#define HSWR	HorizontalSyncWidthRegister
#define HBSR	HorizontalBorderStartRegister
#define HDSR	HorizontalDisplayStartRegister
#define HDER	HorizontalDisplayEndRegister
#define HBER	HorizontalBorderEndRegister
#define HCSR	HorizontalCursorStartRegister
#define VCR	VerticalCycleRegister
#define VSWR	VerticalSyncWidthRegister
#define VBSR	VerticalBorderStartRegister
#define VDSR	VerticalDisplayStartRegister
#define VDER	VerticalDisplayEndRegister
#define VBER	VerticalBorderEndRegister
#define VCSR	VerticalCursorStartRegister
#define VCER	VerticalCursorEndRegister
#define EREG	ExternalRegister
#define FSYNREG	FrequencySynthesisRegister
#define CONREG	ControlRegister
#define DCTL	DataControlRegister

/*
 * Bit masks for external register ereg:
 */
#define EREG_ESEL_REDLUT	0x00000000
#define EREG_ESEL_GREENLUT	0x00000001
#define EREG_ESEL_LCD		0x00000001
#define EREG_ESEL_BLUELUT	0x00000002
#define EREG_ESEL_HIRES		0x00000002
#define EREG_ESEL_EXT_DC	0x00000003
#define EREG_ESEL_DELAY		0x00000003
#define EREG_ECLK_ON		0x00000004
#define EREG_PEDESTAL_ON	0x00000700
#define EREG_DACS_ON		0x00001000
#define EREG_LCDGREY_ON		0x00002000
#define EREG_HIRES_ON		0x00004000
#define EREG_HSYNC		0x00000000
#define EREG_NHSYNC		0x00010000
#define EREG_HSYNCNOR		0x00020000
#define EREG_NHSYNCNOR		0x00030000
#define EREG_VSYNC		0x00000000
#define EREG_NVSYNC		0x00040000
#define EREG_VSYNCXNOR		0x00080000
#define EREC_NVSYNCXNOR		0x000C0000
#define EREG_EREGVAL(eregval)  (0x000000F0 & ((eregval) << 4))

/*
 * Bit masks for control register conreg:
 */
#define CONREG_VCLK		0x00000000
#define CONREG_HCLK		0x00000001
#define CONREG_RCLK		0x00000002
#define CONREG_CK_DIV_1		0x00000000
#define CONREG_CK_DIV_2		0x00000004
#define CONREG_CK_DIV_3		0x00000008
#define CONREG_CK_DIV_4		0x0000000C
#define CONREG_CK_DIV_5		0x00000010
#define CONREG_CK_DIV_6		0x00000014
#define CONREG_CK_DIV_7		0x00000018
#define CONREG_CK_DIV_8		0x0000001C
#define CONREG_BPP_1		0x00000000
#define CONREG_BPP_2		0x00000020
#define CONREG_BPP_4		0x00000040
#define CONREG_BPP_8		0x00000060
#define CONREG_BPP_16		0x00000080
#define CONREG_BPP_32		0x000000C0
#define CONREG_FIFO_4		0x00000100
#define CONREG_FIFO_8		0x00000200
#define CONREG_FIFO_12		0x00000300
#define CONREG_FIFO_16		0x00000400
#define CONREG_FIFO_20		0x00000500
#define CONREG_FIFO_24		0x00000600
#define CONREG_FIFO_28		0x00000700
#define CONREG_DUP		0x00002000
#define CONREG_POWER_DOWN	0x00004000

/*
 * Bit masks for data control register DCTL:
 */
#define DCTL_NORMAL		0x00011000
#define DCTL_NO_HDIS		0x00013000
#define DCTL_HDWR(hdwr)	       (0x000003FF & (hdwr))


#endif /* _VIDREGS_H */

