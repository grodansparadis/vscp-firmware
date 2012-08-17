/*! \file part.h \brief Disk Partition Interface. */
//*****************************************************************************
//
// File Name	: 'part.h'
// Title		: Disk Partition Interface
// Author		: Pascal Stang (c) 2004
// Date			: 8/13/2004
// Revised		: 8/13/2004
// Version		: 0.3
// Target MCU	: any?
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

#ifndef PART_H
#define PART_H

#include "global.h"

// Partition Type used in the partition record
#define PART_TYPE_UNKNOWN		0x00
#define PART_TYPE_FAT12			0x01
#define PART_TYPE_XENIX			0x02
#define PART_TYPE_DOSFAT16		0x04
#define PART_TYPE_EXTDOS		0x05
#define PART_TYPE_FAT16			0x06
#define PART_TYPE_NTFS			0x07
#define PART_TYPE_FAT32			0x0B
#define PART_TYPE_FAT32LBA		0x0C
#define PART_TYPE_FAT16LBA		0x0E
#define PART_TYPE_EXTDOSLBA		0x0F
#define PART_TYPE_ONTRACK		0x33
#define PART_TYPE_NOVELL		0x40
#define PART_TYPE_PCIX			0x4B
#define PART_TYPE_PHOENIXSAVE	0xA0
#define PART_TYPE_CPM			0xDB
#define PART_TYPE_DBFS			0xE0
#define PART_TYPE_BBT			0xFF

// partition record, length 16 bytes
struct partrecord
{			
	BYTE	prIsActive;					// 0x80 indicates active partition
	BYTE	prStartHead;				// starting head for partition
	WORD	prStartCylSect;				// starting cylinder and sector
	BYTE	prPartType;					// partition type (see above)
	BYTE	prEndHead;					// ending head for this partition
	WORD	prEndCylSect;				// ending cylinder and sector
	DWORD	prStartLBA;					// first LBA sector for this partition
	DWORD	prSize;						// size of this partition (bytes or sectors ?)
} GNUC_PACKED;

struct partsector
{
	CHAR	psPartCode[512-64-2];		// pad so struct is 512b
	struct partrecord psPartition[4];	// four partition records (64 bytes)
	BYTE	psBootSectSig0;				// first signature byte: 0x55
	BYTE	psBootSectSig1;				// second signature byte: 0xAA
} GNUC_PACKED;
        
struct partsector_byte
{
	CHAR	psPartCode[512-64-2];		// pad so struct is 512b
	BYTE	psPart[64];					// four partition records (64 bytes)
	BYTE	psBootSectSig0;				// first signature byte: 0x55
	BYTE	psBootSectSig1;				// second signature byte: 0xAA
} GNUC_PACKED;

#define PART_BOOTSIG0        0x55
#define PART_BOOTSIG1        0xaa

// interface
struct PartInfo
{
	DiskInfo_t disk;
	DevDisk_t dev;
	unsigned long start;
	unsigned long sizeinsectors;
//};
} GNUC_PACKED;

typedef struct PartInfo PartInfo_t;

extern PartInfo_t PartInfo;

// Prototypes
int partInit(DiskInfo_t* disk);

void partPrintEntry(struct partrecord *partition);
void partPrintTable(struct partsector *buffer);

int partReadSector(unsigned long sector, int numsectors, u08* buffer);
int partWriteSector(unsigned long sector, int numsectors, u08* buffer);

#endif
