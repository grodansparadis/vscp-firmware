/* 
 * mmu.c --
 *
 *	Code to set up the Memory Management Unit and write the L1
 *	translation table.
 *
 * Copyright (c) 1996 Ben Williamson.
 * All rights reserved.
 *
 * This file is part of nslib, a library used by programs
 * running on the Netslate Development Board.
 *
 * This software is released under the GNU Public License.
 * See the file COPYING for more information.
 */

#include <nslib.h>

#define INVALID_SECT	0x000
#define MEMORY_SECT	0xC1E		/* memory is cached & buffered */
#define IO_SECT		0xC12		/* IO space is not */

#define ENTRIES		0x1000

static int *table = &_l1TableStartPhysical;


/*
 *----------------------------------------------------------------------
 *
 * _MmuInitTable --
 *
 *	Write the L1 translation table ready to start the MMU.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The L1 translation table is filled with section mappings for
 *	RAM, ROM and IO blocks in the memory map, defined in memmap.S.
 *
 *----------------------------------------------------------------------
 */

void
_MmuInitTable(void)
{
	int i;
	
	for (i = 0; i < ENTRIES; i++) {
		table[i] = INVALID_SECT;
	}
	
	table[(int)&_ram0StartLogical >> 20]  
				= (int)&_ram0StartPhysical  | MEMORY_SECT;
	table[(int)&_ram1StartLogical >> 20]  
				= (int)&_ram1StartPhysical  | MEMORY_SECT;
	table[(int)&_ram2StartLogical >> 20]  
				= (int)&_ram2StartPhysical  | MEMORY_SECT;
	table[(int)&_ram3StartLogical >> 20]  
				= (int)&_ram3StartPhysical  | MEMORY_SECT;

	table[(int)&_pcioStartLogical >> 20]  
				= (int)&_pcioStartPhysical  | IO_SECT;
	table[(int)&_armioStartLogical >> 20] 
				= (int)&_armioStartPhysical | IO_SECT;
	table[(int)&_sioStartLogical >> 20]   
				= (int)&_sioStartPhysical   | IO_SECT;
	table[(int)&_vidStartLogical >> 20]   
				= (int)&_vidStartPhysical   | IO_SECT;
	
	table[(int)&_romStartLogical >> 20]   
				= (int)&_romStartPhysical   | MEMORY_SECT;
}
