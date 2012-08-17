/*! \file fat.h \brief FAT16/32 file system driver configuration. */
//*****************************************************************************
//
// File Name	: 'fatconf.h'
// Title		: FAT16/32 file system driver configuration
// Author		: Pascal Stang
// Date			: 4/19/2003
// Revised		: 5/18/2003
// Version		: 0.3
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef FATCONF_H
#define FATCONF_H

// debug on/off
#define DEBUG_FAT

#define FAT_SECTOR_BUFFER_ADDR		0x8000+0x40000000
#define FAT_SECTOR_BUFFER_SIZE		0x0200

#define FAT_FILENAME_BUFFER_ADDR	0x8200+0x40000000
#define FAT_FILENAME_BUFFER_SIZE	0x0100

#define FAT_PATHNAME_BUFFER_ADDR	0x8300+0x40000000
#define FAT_PATHNAME_BUFFER_SIZE	0x0100

#define FAT_CACHE_ADDR				0x8400+0x40000000
#define FAT_CACHE_SIZE				0x0200

#endif
