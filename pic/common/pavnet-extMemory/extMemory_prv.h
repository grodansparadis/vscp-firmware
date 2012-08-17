/**************************************************************************
 *
 * FILE NAME:			extMemory_prv.h
 * FILE DESCRIPTION:	External memory private driver header
 *
 * FILE CREATION DATE:	3-12-2004
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 01a,3dec04 erd written
 *
 ***************************************************************************/

#ifndef __EXT_MEM_PRV_H_
#define __EXT_MEM_PRV_H_

#include "extMemory.h"

// ==========================================================================
// constants and types

//
// Flash memory definitions
//

// magic patterns for unlocking the flash
// note that the second address is 554 because the PIC only 
// outputs data on teh bus when a write to an odd address is
// performed. Therefore, to write to 555, we must write to 554
#define EXTMEM_ROM_UNLOCK_ADDR_1			(0x0AAA)
#define EXTMEM_ROM_UNLOCK_DATA_1			(0x00AA)
#define EXTMEM_ROM_UNLOCK_ADDR_2			(0x0554)
#define EXTMEM_ROM_UNLOCK_DATA_2			(0x0055)
		
// flash commands                   		
#define EXTMEM_ROM_CMD_RESET				(0x00F0)
#define EXTMEM_ROM_CMD_WRITE				(0x00A0)
#define EXTMEM_ROM_CMD_ERASE				(0x0080)
#define EXTMEM_ROM_AUTOSELECT				(0x0090)
#define EXTMEM_ROM_CMD_CHIP_ERASE			(0x0010)
#define EXTMEM_ROM_CMD_NONE					(0x0000)

// timeouts (all must be under 2^16)
#define EXTMEM_ROM_WRITE_TIMEOUT_MS 		(1000)
#define EXTMEM_ROM_CHIP_ERASE_TIMEOUT_MS 	(10000)

// ==========================================================================
// locals

// variable to write to external memory
far uint_16 *extMemPtr;

// extmem_romWriteCmd
//
// Write a command to the external ROM (unlocks and writes)
//

extern void extmem_romWriteCmd(const uint_16 cmd);

// extmem_romWriteDirect
//
// Write data directly out on the bus
//

extern void extmem_romWriteDirect(const uint_32 address, const uint_16 data);

// extmem_romReadDirect
//
// Write a command to the external ROM (unlocks and writes)
//

extern uint_16 extmem_romReadDirect(const uint_32 address);


#endif /* __PCA9554_PRV_H_ */
