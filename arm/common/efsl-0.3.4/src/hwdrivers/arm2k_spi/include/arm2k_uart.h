/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  arm2k_uart.h                                                    *
* Description : Headerfile for arm2k_uart.c                                   *
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

#ifndef __ARM2K_UART_H__
#define __ARM2K_UART_H__

#include "debug.h"
#include "config.h"
#include "LPC214X.h"
#include "arm2k_macros.h"


#define ULSR_BIT_THRE       5
#define ULSR_THRE_MSK       0x20

#define ULCR_WLS_MSK        0x03
#define ULCR_BIT_SBS        2
#define ULCR_BIT_PE         3
#define ULCR_BIT_DLAB       7

#define UFDR_DIVADDVAL_MSK  0xF
#define UFDR_MULVAL_MSK     0xF

#define P0_0_TX0            0
#define P0_1_RXD0           2

#define P0_8_TX1            16
#define P0_9_RXD1           18


void UART0_Init( unsigned int baud );
void UART1_Init( unsigned int baud );

#endif
