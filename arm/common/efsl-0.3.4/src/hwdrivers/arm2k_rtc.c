/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : arm2k_spi.c                                                      *
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
*                                                    (c)2005 Michael De Nil   *
*                                                    (c)2005 Lennart Yseboodt *
*                                                    (c)2005 Joel Winarske    *
\*****************************************************************************/

/*****************************************************************************/
#include "arm2k_rtc.h"

/*****************************************************************************/

euint8 arm2k_spi_init( arm2kSpiInterface *iface )
{
        PINSEL1 |= P0_17_SCK1;      /* SCK1 */
        PINSEL1 |= P0_18_MISO1;     /* MISO1 */
        PINSEL1 |= P0_19_MOSI1;     /* MOSI1 */
        PINSEL1 |= P0_20_SSEL1;     /* SSEL1 */

        SSPCR0 |= (DSS_MSK & EIGHT_BIT_TRANSFER); /* 8 bits data size */
        SSPCR0 |= (FRF_MSK & (SPI_MODE<<4));     /* Select SPI mode */

        CLR_BIT(SSPCR0,SSPCR0_BIT_CPOL);      /* ClockPlarity */
        CLR_BIT(SSPCR0,SSPCR0_BIT_CPHA);      /* ClockPhase (active) */
        CLR_BIT(SSPCR1,SSPCR1_BIT_MS);        /* Master mode */
        SET_BIT(SSPCR1,SSPCR1_BIT_SSE);       /* Enable SPI */

        SSPCPSR = 2;  /* 1/2 pclk */

	return(0);
}
/*****************************************************************************/

euint8 arm2k_spi_send( arm2kSpiInterface *iface, euint8 data )
{
	euint8 incoming=0;
        while(BIT_IS_CLR(SSPSR,SSPSR_BIT_TNF)) {;}    /* wait for slot in fifo */
        SSPDR = data;                              /* Send the data */

        while(BIT_IS_CLR(SSPSR,SSPSR_BIT_RNE)) {;}    /* Wait for data in rx fifo */
        incoming = SSPDR;                          /* read the data received */
        return (incoming);
}
/*****************************************************************************/

