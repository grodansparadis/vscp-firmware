/***********************************************************************/
/*                                                                     */
/*  FILE        :resetprg.c                                            */
/*  DATE        :Tue, Jun 04, 2013                                     */
/*  DESCRIPTION :initialize for C language.                            */
/*  CPU GROUP   :Other                                                 */
/*                                                                     */
/*  This file is generated by Renesas Project Generator (Ver.4.18).    */
/*  NOTE:THIS IS A TYPICAL EXAMPLE.                                    */
/***********************************************************************/
/*********************************************************************
*
* Device     : R8C/Tiny
*
* File Name  : resetprg.c
*
* Abstract   : Initialize for C language.
*
* History    : 2.00  (2009-01-27)
*
* Copyright (C) 2009 (2010) Renesas Electronics Corporation.
* and Renesas Solutions Corporation. All rights reserved.
*
*********************************************************************/
#include "typedefine.h"
#include "resetprg.h"
#include "initsct.h"

////////////////////////////////////////////
// declare sfr register 
#pragma ADDRESS	protect	0AH
#pragma ADDRESS	pmode0	04H
_UBYTE protect,pmode0;
extern _UINT	_stack_top,_istack_top;
DEF_SBREGISTER;


#pragma entry start
void start(void);
extern void initsct(void);
extern void _init(void);
void exit(void);
void main(void);

#pragma section program interrupt 

void start(void)
{
	_isp_	= &_istack_top; 	// set interrupt stack pointer
	protect	= 0x02U;		// change protect mode register
	pmode0	= 0x00U;		// set processor mode register
	protect	= 0x00U;		// change protect mode register
	_flg_	= 0x0080U;	// set flag register
	_sp_	= &_stack_top; 	// set user stack pointer
	_sb_	= 0x400U;	// 400H fixation (Do not change)
	_intbh_ = 0x00U; 	// set variable vector's address
	_asm("	ldc	#(topof vector)&0FFFFh,INTBL");

	sclear("stack","data","noalign");
	initsct();		// initlalize each sections

#if __HEAPSIZE__ != 0
	heap_init();		// initialize heap 
#endif
#if __STANDARD_IO__ != 0
	_init();			// initialize standard I/O
#endif
	_fb_ = 0U;		// initialize FB registe for debugger
	main();			// call main routine

	exit();		// call exit
}

void exit(void)
{
	while(1)
	{
		;		//infinite loop
	}
}

