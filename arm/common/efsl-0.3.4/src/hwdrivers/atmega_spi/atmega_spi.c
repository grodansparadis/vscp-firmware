/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : atmega_spi.c                                                     *
* Description : This file contains the functions needed to use efs for        *
*               accessing files on an SD-card connected to an ATMega's.       *
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
\*****************************************************************************/

/*****************************************************************************/
#include "include/atmega_spi.h"
/*****************************************************************************/

euint8 atmega_spi_init(atmegaSpiInterface *iface)
{
	euint8 i;
	
	/* Unselect card */
	PORTB |= iface->pinSelect;
	
	/* Set as master, clock and chip select output */
	DDR_SPI = (1<<DD_MOSI) | (1<<DD_SCK) | 1;

	/* Enable SPI, master, set clock rate to fck/2 */
	SPCR = (1<<SPE) | (1<<MSTR); /* fsck / 4 */
	SPSR = 1; /* fsck / 2 */

	/* Send 10 spi commands with card not selected */
	for(i=0;i<10;i++)
		atmega_spi_send(iface,0xff);

	/* Select card */
	PORTB &= ~(iface->pinSelect);

	return(0);
}
/*****************************************************************************/

euint8 atmega_spi_send(atmegaSpiInterface *iface, euint8 data)
{
	euint8 incoming=0;
	
	PORTB &= ~(iface->pinSelect);
	
	SPDR = data;
	while(!(SPSR & (1<<SPIF)))
		incoming = SPDR;

	PORTB |= iface->pinSelect;

	return(incoming);
}
/*****************************************************************************/

