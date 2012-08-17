/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  arm2k_ssp.h                                                     *
* Description : Headerfile for arm2k_ssp.c                                    *
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
*                                                    (c)2005 Michael De Nil   *
*                                                    (c)2005 Lennart Yseboodt *
*					             (c)2005 Joel Winarske    *
\*****************************************************************************/

#ifndef __ARM2K_SSP_H__
#define __ARM2K_SSP_H__

#include "debug.h"
#include "config.h"
#include "LPC214X.h"
#include "arm2k_macros.h"

/* Pin Function Select Block */
#define P0_17_SCK1          2
#define P0_18_MISO1         4
#define P0_19_MOSI1         6
#define P0_20_SSEL1         8

/* SSPCR0 bits */
#define DSS_MSK             0x0F
#define FRF_MSK             0x30
#define SSPCR0_BIT_CPOL     6
#define SSPCR0_BIT_CPHA     7

/* SSPCR1 bits */
#define SSPCR1_BIT_SSE      1
#define SSPCR1_BIT_MS       2

/* SSPSR bits */
#define SSPSR_BIT_TNF       1
#define SSPSR_BIT_RNE       2

#define SSPSR_TNF_MSK       0x02
#define SSPSR_RNE_MSK       0x04

#define SPI_MODE            0x00
#define EIGHT_BIT_TRANSFER  0x07

/*************************************************************\
              hwInterface
               ----------
* long		sectorCount		Number of sectors on the file.
*
* Remark: Configuration of the spi-port should become part of
* this configuration.
\*************************************************************/

typedef struct _arm2kSspInterface{
    euint8 CardPresent;         /* set in external interrupt */
    euint8 WriteProtect;        /* set in external interrupt */	
} arm2kSspInterface;

euint8 arm2k_ssp_init( arm2kSspInterface *iface );
euint8 arm2k_ssp_send( arm2kSspInterface *iface, euint8 data );

#endif
