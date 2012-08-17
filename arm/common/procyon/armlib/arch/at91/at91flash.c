/*! \file at91flash.c \brief Internal FLASH Write Routines for Atmel AT91. */
//*****************************************************************************
//
// File Name	: 'at91flash.c'
// Title		: Internal FLASH Write Routines for Atmel AT91
// Author		: Pascal Stang - Copyright (C) 2006
// Created		: 9/20/2006
// Revised		: 9/25/2006
// Version		: 0.1
// Target MCU	: Atmel AT91 series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

// system includes
#include "global.h"
#include "at91sam7s256.h"

// local includes
#include "at91flash.h"

void at91flashInit(void)
{
	// set cycles-per-microsecond
	//  as per datasheet,
	//	- NVM bits require a setting of F_CPU/1000
	//	- general flash write requires a setting of 1.5*F_CPU/1000
	// (here we're extra conservative setting clock cycles equal to 2us)
	AT91C_BASE_MC->MC_FMR = (((F_CPU*2/1000)<<16) & AT91C_MC_FMCN);
}

void at91flashWrite(uint32_t flashaddr, uint8_t* buffer, uint32_t len)
{
	int wrlen;

	// do automatic multi-page writes
	while(len)
	{
		// determine how many bytes to write
		wrlen = (len<AT91C_IFLASH_PAGE_SIZE)?(len):(AT91C_IFLASH_PAGE_SIZE);
		// write page
		at91flashWritePage(flashaddr, buffer, wrlen);
		// increment pointers
		flashaddr += wrlen;
		buffer += wrlen;
		// decrement remaining buffer size
		len -= wrlen;
	}
}

void at91flashWritePage(uint32_t flashaddr, uint8_t* buffer, uint32_t len)
{
	int pageword;
	unsigned long* wrptr = (unsigned long*)flashaddr;
	
	// do write cycle
	// copy data to flash location
	for(pageword=0; pageword<(len/4); pageword++)
	{
		// do the copy byte-wise because incoming buffer might not be word-aligned
		// NOTE: assuming little-endian source
		*wrptr++ = (buffer[3]<<24) | (buffer[2]<<16) | (buffer[1]<<8) | (buffer[0]<<0);
		buffer+=4;
	}
	// if the flash address does not begin on page boundary, then we do partial-page programming
	if( flashaddr & (AT91C_IFLASH_PAGE_SIZE-1) )
		AT91C_BASE_MC->MC_FMR |= AT91C_MC_NEBP;
	else
		AT91C_BASE_MC->MC_FMR &= ~AT91C_MC_NEBP;
	// write flash
	AT91C_BASE_MC->MC_FCR = (0x5A<<24) | (((flashaddr/AT91C_IFLASH_PAGE_SIZE)<<8)&AT91C_MC_PAGEN) | AT91C_MC_FCMD_START_PROG;
	// wait for flash done/ready
	while(!(AT91C_BASE_MC->MC_FSR & AT91C_MC_FRDY));
}

void at91flashErase(void)
{
	// erase flash
	AT91C_BASE_MC->MC_FCR = (0x5A<<24) | AT91C_MC_FCMD_ERASE_ALL;
	// wait for flash done/ready
	while(!(AT91C_BASE_MC->MC_FSR & AT91C_MC_FRDY));
}

int at91flashGetLock(uint32_t flashaddr)
{
	// mask flashaddr to size of flash
	flashaddr &= (AT91C_IFLASH_SIZE-1);
	// determine the lock state of a flash address/page
	if( AT91C_BASE_MC->MC_FSR & (1<<(16+(flashaddr/AT91C_IFLASH_LOCK_REGION_SIZE))) )
		return 1;	// region is locked
	else
		return 0;	// region is not locked
}

void at91flashSetLock(uint32_t flashaddr, int lockstate)
{
	// set the lock state of a flash address/page
	
	// mask flashaddr to size of flash
	flashaddr &= (AT91C_IFLASH_SIZE-1);
	// since lock bits have a small lifetime (100 programming cycles),
	// check to see if lock bit is already set to requested state
	if( at91flashGetLock(flashaddr) == lockstate)
		return;		// lock bit is already set as desired
	// program the lock bit
	if(lockstate)
		AT91C_BASE_MC->MC_FCR = (0x5A<<24) | (((flashaddr/AT91C_IFLASH_PAGE_SIZE)<<8)&AT91C_MC_PAGEN) | AT91C_MC_FCMD_LOCK;
	else
		AT91C_BASE_MC->MC_FCR = (0x5A<<24) | (((flashaddr/AT91C_IFLASH_PAGE_SIZE)<<8)&AT91C_MC_PAGEN) | AT91C_MC_FCMD_UNLOCK;
	// wait for flash done/ready
	while(!(AT91C_BASE_MC->MC_FSR & AT91C_MC_FRDY));
}
