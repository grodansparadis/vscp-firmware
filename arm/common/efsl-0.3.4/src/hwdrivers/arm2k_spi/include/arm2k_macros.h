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

#ifndef __ARM2K_MACROS_H__
#define __ARM2K_MACROS_H__

#define	SET_BIT(val,bit)	      ((val) |=   (1<<bit))
#define	CLR_BIT(val,bit)	      ((val) &= (~(1<<bit)))
#define	BIT_IS_SET(val,bit)       (((val) &    (1<<bit))!=0)
#define	BIT_IS_CLR(val,bit)       (((val) &    (1<<bit))==0)

#endif
