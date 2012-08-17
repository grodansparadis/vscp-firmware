/*! \file armlibdefs.h \brief ARMlib global defines and macros. */
//*****************************************************************************
//
// File Name	: 'armlibdefs.h'
// Title		: ARMlib global defines and macros include file
// Author		: Pascal Stang
// Created		: 7/12/2001
// Revised		: 9/30/2002
// Version		: 1.1
// Target MCU	: ARM
// Editor Tabs	: 4
//
//	Description : This include file is designed to contain items useful to all
//					code files and projects, regardless of specific implementation.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************


#ifndef ARMLIBDEFS_H
#define ARMLIBDEFS_H

// MIN/MAX/ABS macros
#define MIN(a,b)			(((a)<(b))?(a):(b))
#define MAX(a,b)			(((a)>(b))?(a):(b))
#define ABS(x)				(((x)>0)?(x):(-x))

// constants
#define PI		3.14159265359

// some handy defines
#ifndef BIT
#define BIT(n)				(1 << (n))
#endif
#ifndef BV
#define BV(n)				(1 << (n))
#endif

#define sbi(num,bitloc)		(num|=(1<<bitloc))
#define cbi(num,bitloc)		(num&=~(1<<bitloc))
#define gbi(num,bitloc)		((num&=(1<<bitloc))>>bitloc)

// use this for packed structures
#define GNUC_PACKED __attribute__((packed)) 

// port bits
#define P00			BIT(0)		// P0.00
#define P01			BIT(1)		// P0.01
#define P02			BIT(2)		// P0.02
#define P03			BIT(3)		// P0.03
#define P04			BIT(4)		// P0.04
#define P05			BIT(5)		// P0.05
#define P06			BIT(6)		// P0.06
#define P07			BIT(7)		// P0.07
#define P08			BIT(8)		// P0.08
#define P09			BIT(9)		// P0.09
#define P10			BIT(10)		// P0.10
#define P11			BIT(11)		// P0.11
#define P12			BIT(12)		// P0.12
#define P13			BIT(13)		// P0.13
#define P14			BIT(14)		// P0.14
#define P15			BIT(15)		// P0.15
#define P16			BIT(16)		// P0.16
#define P17			BIT(17)		// P0.17
#define P18			BIT(18)		// P0.18
#define P19			BIT(19)		// P0.19
#define P20			BIT(20)		// P0.20
#define P21			BIT(21)		// P0.21
#define P22			BIT(22)		// P0.22
#define P23			BIT(23)		// P0.23
#define P24			BIT(24)		// P0.24
#define P25			BIT(25)		// P0.25
#define P26			BIT(26)		// P0.26
#define P27			BIT(27)		// P0.27
#define P28			BIT(28)		// P0.28
#define P29			BIT(29)		// P0.29
#define P30			BIT(30)		// P0.30
#define P31			BIT(31)		// P0.31

#endif
