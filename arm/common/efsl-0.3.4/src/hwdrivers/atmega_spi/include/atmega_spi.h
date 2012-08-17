/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  atmega_spi.h                                                    *
* Description : Headerfile for atmega_spi.c                                   *
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
\*****************************************************************************/

#ifndef __ATMEGA_SPI_H__ 
#define __ATMEGA_SPI_H__ 

#define __AVR_ATmega128__ 1
#define FALSE	0X00
#define TRUE	0x01

#define DDR_SPI DDRB
#define DD_MOSI DDB2
#define DD_SCK  DDB1

#include <avr/io.h>
#include <compat/ina90.h>
#include "debug.h"
#include "config.h"


/*************************************************************\
              hwInterface
               ----------
* long		sectorCount		Number of sectors on the file.
* 
* Remark: Configuration of the spi-port should become part of 
* this configuration.
\*************************************************************/
struct atmegaSpiInterface{
	/*euint8 portSelect;*/ /* TODO */
	euint8 pinSelect;	
};
typedef struct atmegaSpiInterface atmegaSpiInterface;

euint8 atmega_spi_init(atmegaSpiInterface *iface);
euint8 atmega_spi_send(atmegaSpiInterface *iface, euint8 data);

#endif
