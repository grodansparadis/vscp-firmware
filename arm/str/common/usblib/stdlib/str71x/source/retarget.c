/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : retarget.c
* Author             : MCD Application Team
* Date First Issued  : 06/08/2003
* Description        : C Library retargeting.
********************************************************************************
* History:
*  13/01/2006 : V3.1
*  24/05/2005 : V3.0
*  30/11/2004 : V2.0
*  14/07/2004 : V1.3
*  01/01/2004 : V1.2
********************************************************************************
 THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
 CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
 OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#include <stdio.h>
#include "71x_conf.h"

/* #define USE_SERIAL_PORT */

#ifdef __thumb
/* Thumb Semihosting SWI */
#define SemiSWI 0xAB
#else
/* ARM Semihosting SWI */
#define SemiSWI 0x123456
#endif

/* Write a character */ 
__swi(SemiSWI) void _WriteC(unsigned op, char *c);
#define WriteC(c) _WriteC (0x3,c)

/* Exit */
__swi(SemiSWI) void _Exit(unsigned op, unsigned except);
#define Exit() _Exit (0x18,0x20026)


struct __FILE { int handle;   /* Add whatever you need here */};
FILE __stdout;


extern void sendchar( char *ch );    /* in serial.c */


int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to a UART, or to the debugger console with SWI WriteC */

    char tempch = ch;
#ifdef USE_SERIAL_PORT
    sendchar( &tempch );
#else
    WriteC( &tempch );
#endif
    return ch;
}


int ferror(FILE *f)
{   /* Your implementation of ferror */
    return EOF;
}


void _sys_exit(int return_code)
{
    Exit();         /* for debugging */

label:  goto label; /* endless loop */
}


void _ttywrch(int ch)
{
    char tempch = ch;
#ifdef USE_SERIAL_PORT
    sendchar( &tempch );
#else
    WriteC( &tempch );
#endif
}


__value_in_regs struct R0_R3 {unsigned heap_base, stack_base, heap_limit, stack_limit;} 
    __user_initial_stackheap(unsigned int R0, unsigned int SP, unsigned int R2, unsigned int SL)
{
    struct R0_R3 config;

    config.heap_base = 0x00030000;
    config.stack_base = SP;

/*
To place heap_base directly above the ZI area, use:
    extern unsigned int Image$$ZI$$Limit;
    config.heap_base = (unsigned int)&Image$$ZI$$Limit;
(or &Image$$region_name$$ZI$$Limit for scatterloaded images)

To specify the limits for the heap & stack, use e.g:
    config.heap_limit = SL;
    config.stack_limit = SL;
*/

    return config;
}


