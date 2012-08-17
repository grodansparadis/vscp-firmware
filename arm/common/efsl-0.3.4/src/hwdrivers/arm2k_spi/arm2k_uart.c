/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : arm2k_uart.c                                                     *
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
#include "arm2k_uart.h"

/*****************************************************************************/

int putchar(int ch)
{
#if (DEBUG_PORT == 0)
   while(!(U0LSR&ULSR_THRE_MSK)) {;}
//   while (BIT_IS_CLR(U0LSR,ULSR_BIT_THRE));  /* While Transmit Holding Register is Not Empty */
   U0THR = ch;
#elif (DEBUG_PORT == 1)
   while(!(U1LSR&ULSR_THRE_MSK)) {;}
//   while (BIT_IS_CLR(U1LSR,ULSR_BIT_THRE));  /* While Transmit Holding Register is Not Empty */
   U1THR = ch;
#endif
   return (ch);
}

/*****************************************************************************/

void UART0_Init( unsigned int baud )
{
    /* Line Control Register */
    U0LCR |= (ULCR_WLS_MSK & 0x3);  /* 8 bit */
    CLR_BIT(U0LCR,ULCR_BIT_SBS);    /* 1 stop bit */
    CLR_BIT(U0LCR,ULCR_BIT_PE);     /* no parity */
    SET_BIT(U0LCR,ULCR_BIT_DLAB);   /* enable DLAB */

    if(baud == 9600) {

        U0DLM =  0;
        U0DLL =  71;
        U0FDR = ((UFDR_MULVAL_MSK & 8)<<4)|(UFDR_DIVADDVAL_MSK & 3);

    } else if (115200) {

        U0DLM =  0;
        U0DLL =  6;
        U0FDR = ((UFDR_MULVAL_MSK & 14)<<4)|(UFDR_DIVADDVAL_MSK & 5);
    }

    U0LCR &= ~0x80;

    PINSEL0 |= (0x01 << P0_0_TX0);
    PINSEL0 |= (0x01 << P0_1_RXD0);
}

/*****************************************************************************/

void UART1_Init( unsigned int baud )
{
    /* Line Control Register */
    U1LCR |= (ULCR_WLS_MSK & 0x3);  /* 8 bit */
    CLR_BIT(U1LCR,ULCR_BIT_SBS);    /* 1 stop bit */
    CLR_BIT(U1LCR,ULCR_BIT_PE);     /* no parity */
    SET_BIT(U1LCR,ULCR_BIT_DLAB);   /* enable DLAB */

    if( baud == 9600 ) {

        U1DLM =  0;
        U1DLL =  71;
        U1FDR = ((UFDR_MULVAL_MSK & 8)<<4)|(UFDR_DIVADDVAL_MSK & 3);

    } else if ( baud == 115200 ) {

        U1DLM =  0;
        U1DLL =  6;
        U1FDR = ((UFDR_MULVAL_MSK & 14)<<4)|(UFDR_DIVADDVAL_MSK & 5);
    }

    U1LCR &= ~0x80;

    PINSEL0 |= (0x01 << P0_8_TX1);
    PINSEL0 |= (0x01 << P0_9_RXD1);
}

/*****************************************************************************/
