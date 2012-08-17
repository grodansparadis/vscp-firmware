/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  arm2k_pll.h                                                     *
* Description : Headerfile for arm2k_pll.c                                    *
*                                                                             *
* This library is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU Lesser General Public                  *
* License as published by the Free Software Foundation; either                *
* version 2.1 of the License, or (at your option) any later version.          *
*                                                                             *
* This library is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           *
* Lesser General Public License for more details.                             *
*                                                                             *
*                                                                             *
*                                                    (c)2005 Joel Winarske    *
\*****************************************************************************/

#ifndef __ARM2K_PLL_H__
#define __ARM2K_PLL_H__

#include "debug.h"
#include "config.h"
#include "LPC214X.h"
#include "arm2k_macros.h"


#define PLLCFG_MSEL_MSK     0x1F
#define PLLCFG_PSEL_MSK     0x03
#define PLLFEED_FEED_MSK    0xFF

#define PLLCFG_BIT_MSEL     0
#define PLLCFG_BIT_PSEL     5

#define PLLCON_BIT_PLLE     0
#define PLLCON_BIT_PLLC     1
#define PLLSTAT_BIT_PLOCK   10

void PLL0_Init(void);

#endif
