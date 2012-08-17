/*! \file lpc2000.h \brief Philips ARM LPC2000 Definitions. */
//*****************************************************************************
//
// File Name	: 'ata.h'
// Title		: Philips ARM LPC2000 Definitions
// Author		: Pascal Stang
// Date			: 2/20/2006
// Revised		: 2/20/2006
// Version		: 0.1
// Target MCU	: any
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

//#ifndef LPC2000_H
//#define LPC2000_H

#include "global.h"

#define LPC210x		1
#define LPC21xx		2
#define LPC22xx		3

#if(MMCU==LPC210x)
	#include "lpc210x.h"
#elif(MMCU==LPC21xx)
	#include "lpc21xx.h"
#elif(MMCU==LPC22xx)
	#include "lpc22xx.h"
#else
	#warning **Assuming LPC210x-series processor!
	#include "lpc210x.h"
#endif

//#endif
