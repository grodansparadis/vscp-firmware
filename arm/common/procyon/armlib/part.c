/*! \file part.c \brief Disk Partition Interface. */
//*****************************************************************************
//
// File Name	: 'part.c'
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


#include <string.h>

#include "device.h"
#include "ata.h"
#include "rprintf.h"
#include "debug.h"
#include "ataconf.h"

#include "part.h"

#define DEBUG_PART

// globals
unsigned char PartBuffer[0x200];

// filesystem constants/metrics
PartInfo_t PartInfo;

/*************************************************************************/
/*************************************************************************/

int partInit(DiskInfo_t* disk)
{
	//unsigned char* buffer = (unsigned char*) SECTOR_BUFFER_ADDR;
	unsigned char* buffer = PartBuffer;

	// local drivers
	PartInfo.disk = *disk;

	// read partition table
	// TODO.... error checking
	ataReadSectors(&PartInfo.disk, 0, 1, buffer);
	// map first partition record	
	// save partition information to global PartRecord
	//PartRecord = ((struct partsector *) SectorBuffer)->psPartition[0]);
	
	// setup global disk constants
	PartInfo.start =			((struct partsector *) buffer)->psPartition[0].prStartLBA;
	PartInfo.sizeinsectors =	((struct partsector *) buffer)->psPartition[0].prSize;

	// do debug
#ifdef DEBUG_PART
	partPrintEntry( &((struct partsector *) buffer)->psPartition[0] );
#endif

	// setup access methods
	PartInfo.dev.bytespersector = 512;
	PartInfo.dev.numsectors = PartInfo.sizeinsectors;
	PartInfo.dev.ReadSector = partReadSector;
	PartInfo.dev.WriteSector = partWriteSector;

	return 0;	
}

void partPrintEntry(struct partrecord *partition)
{
	rprintfProgStrM("----- Partition -----\r\n");
	rprintfProgStrM("Active        : ");	rprintfu08(partition->prIsActive);		rprintfCRLF();
	rprintfProgStrM("StartHead     : ");	rprintfu08(partition->prStartHead);		rprintfCRLF();
	rprintfProgStrM("StartCylSect  : ");	rprintfu16(partition->prStartCylSect);	rprintfCRLF();
	rprintfProgStrM("Partition Type: ");
	switch(partition->prPartType)
	{
		case PART_TYPE_DOSFAT16:
				rprintfProgStrM("Found: DOSFAT 16\r\n");
				break;
		case PART_TYPE_FAT16:
				rprintfProgStrM("Found: FAT16\r\n");
				break;
		case PART_TYPE_FAT16LBA:
				rprintfProgStrM("Found: FAT16 LBA\r\n");
				break;
		case PART_TYPE_FAT32LBA:
				rprintfProgStrM("Found: FAT32 LBA\r\n");
				break;
		case PART_TYPE_FAT32:
				rprintfProgStrM("Found: FAT32\r\n");
				break;
		case PART_TYPE_UNKNOWN:
				rprintfProgStrM("No Partition\r\n");
				break;
		default:
				rprintf("Type: 0x%x\r\n", partition->prPartType);
				break;
	}
	rprintfProgStrM("EndHead       : ");	rprintfu08(partition->prEndHead);		rprintfCRLF();
	rprintfProgStrM("EndCylSect    : ");	rprintfu16(partition->prEndCylSect);	rprintfCRLF();
	rprintfProgStrM("StartLBA      : ");	rprintfu32(partition->prStartLBA);		rprintfCRLF();
	rprintfProgStrM("Size          : ");	rprintfu32(partition->prSize);			rprintfCRLF();
}

void partPrintTable(struct partsector *buffer)
{
	ataReadSectors(&PartInfo.disk, 0, 1, PartBuffer);
	buffer = (struct partsector*)PartBuffer;

	if( (buffer->psBootSectSig0 == PART_BOOTSIG0) &&
		(buffer->psBootSectSig1 == PART_BOOTSIG1) )
	{
		rprintfProgStrM("Partition Sector Valid.\r\n");
	}

	partPrintEntry( &buffer->psPartition[0] );
	partPrintEntry( &buffer->psPartition[1] );
	partPrintEntry( &buffer->psPartition[2] );
	partPrintEntry( &buffer->psPartition[3] );
}


int partReadSector(unsigned long sector, int numsectors, u08* buffer)
{
	if(sector < PartInfo.sizeinsectors)
	{
		ataReadSectors(&PartInfo.disk, sector+PartInfo.start, numsectors, buffer);
		return 0;
	}
	else
		return 1;
}

int partWriteSector(unsigned long sector, int numsectors, u08* buffer)
{
	if(sector < PartInfo.sizeinsectors)
	{
		ataWriteSectors(&PartInfo.disk, sector+PartInfo.start, numsectors, buffer);
		return 0;
	}
	else
		return 1;
}
