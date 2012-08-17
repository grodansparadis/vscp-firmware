/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : arm2k_pll.c                                                      *
* Description : This file contains the functions needed to use efs for        *
*               accessing files on an SD-card connected to a Phillips         *
*               LPC2000 series SSP port                                       *
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
*                                                    (c)2005 Joel Winarske    *
\*****************************************************************************/

/*****************************************************************************/
#include "arm2k_pll.h"

/*****************************************************************************/

void PLL0_Init(void)
{
  PLLCFG |= (PLLCFG_MSEL_MSK & 0x4);                       /* PLL multiplier */
  PLLCFG |= ((PLLCFG_PSEL_MSK & 0x1)<<PLLCFG_BIT_PSEL);    /* PLL divider */

  PLLFEED |= (PLLFEED_FEED_MSK & 0xAA);
  PLLFEED |= (PLLFEED_FEED_MSK & 0x55);

  PLLCON |= (1 << PLLCON_BIT_PLLE);                        /* enable PLL */

  PLLFEED |= (PLLFEED_FEED_MSK & 0xAA);
  PLLFEED |= (PLLFEED_FEED_MSK & 0x55);

  while(BIT_IS_CLR(PLLSTAT,PLLSTAT_BIT_PLOCK));            /* wait for lock */

  PLLCON |= (1 << PLLCON_BIT_PLLC);                        /* connect PLL */

  PLLFEED |= (PLLFEED_FEED_MSK & 0xAA);
  PLLFEED |= (PLLFEED_FEED_MSK & 0x55);
}

/*****************************************************************************/
