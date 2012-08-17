/*! \file fat.c \brief FAT16/32 file system driver. */
//*****************************************************************************
//
// File Name	: 'fat.c'
// Title		: FAT16/32 file system driver
// Author		: Pascal Stang
// Date			: 11/07/2000
// Revised		: 12/12/2000
// Version		: 0.3
// Target MCU	: ATmega103 (should work for Atmel AVR Series)
// Editor Tabs	: 4
//
// This code is based in part on work done by Jesper Hansen for his
//		YAMPP MP3 player project.
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
#include "rprintf.h"
#include "debug.h"

#include "fat.h"
#include "fatconf.h"

// globals
// buffers
//unsigned char *SectorBuffer  =		(unsigned char *) FAT_SECTOR_BUFFER_ADDR;
//unsigned char *FileNameBuffer =		(unsigned char *) FAT_FILENAME_BUFFER_ADDR;
//unsigned char *PathNameBuffer =		(unsigned char *) FAT_PATHNAME_BUFFER_ADDR;
//unsigned char *FatCacheBuffer =		(unsigned char *) FAT_CACHE_BUFFER_ADDR;
unsigned char SectorBuffer[0x200];
unsigned char FileNameBuffer[0x100];
unsigned char PathNameBuffer[0x100];
unsigned char FatCacheBuffer[0x200];

struct fatFsInfo
{
	unsigned long FirstDataSector;
	unsigned short BytesPerSector;
	unsigned short SectorsPerCluster;
	unsigned long FirstFATSector;
	unsigned long SectorsPerFat;
	unsigned long RootDirStartCluster;
	unsigned long FatInCache;
	unsigned long FatMask;
	unsigned char Fat32Enabled;
	unsigned char NumberOfFats;

	DevDisk_t devdisk;
//};
} GNUC_PACKED;

typedef struct FatFsInfo FatFsInfo_t;

// filesystem constants/metrics
struct fatFsInfo FatFsInfo;

// operating variables
unsigned long CurrentDirStartCluster;	//< current directory starting cluster
struct FileInfo_s FileInfo;				//< file information for last file accessed


/*************************************************************************/
/*************************************************************************/

unsigned char fatInit(DevDisk_t* devdisk)
{
	//bios parameter block
	struct bpb710 *bpb;

	// initialize disk access interface
	FatFsInfo.devdisk = *devdisk;

	rprintfProgStrM("DevDisk pointers\r\n");
	rprintfu32((unsigned long)devdisk->ReadSector);
	rprintfu32((unsigned long)devdisk->WriteSector);
	rprintfCRLF();

	FatFsInfo.FatInCache = 0;

	// Read the BootSector
	FatFsInfo.devdisk.ReadSector(0, 1, SectorBuffer);
	bpb = (struct bpb710 *) ((struct bootsector710 *) SectorBuffer)->bsBPB;

	// setup global disk constants
	if(bpb->bpbFATsecs)
	{
		// bpbFATsecs (16-bit) is non-zero and is therefore valid
		FatFsInfo.SectorsPerFat = bpb->bpbFATsecs;
		// this volume should be FAT16
		// first directory cluster is 2 by default (clusters range 2->big)
		FatFsInfo.RootDirStartCluster = CLUST_FIRST;
		// push data sector pointer to end of root directory area
		//FirstDataSector += (bpb->bpbRootDirEnts)/DIRENTRIES_PER_SECTOR;
		FatFsInfo.Fat32Enabled = FALSE;
		// set FAT mask for 16-bit
		FatFsInfo.FatMask = FAT16_MASK;
	}
	else
	{
		// bpbFATsecs is zero, real value is in bpbBigFATsecs (32-bit)
		FatFsInfo.SectorsPerFat = bpb->bpbBigFATsecs;
		// this volume must be FAT32
		// bpbRootClust field exists in FAT32 bpb710, but not in lesser bpb's
		FatFsInfo.RootDirStartCluster = bpb->bpbRootClust;
		// push data sector pointer to end of root directory area
		// need this? FirstDataSector += (bpb->bpbRootDirEnts)/DIRENTRIES_PER_SECTOR;
		FatFsInfo.Fat32Enabled = TRUE;
		// set FAT mask for 32-bit
		FatFsInfo.FatMask = FAT32_MASK;
	}
	FatFsInfo.SectorsPerCluster		= bpb->bpbSecPerClust;
	FatFsInfo.BytesPerSector		= bpb->bpbBytesPerSec;
	FatFsInfo.FirstFATSector		= bpb->bpbResSectors;
	FatFsInfo.NumberOfFats			= bpb->bpbFATs;
	FatFsInfo.FirstDataSector		= bpb->bpbResSectors + FatFsInfo.NumberOfFats * FatFsInfo.SectorsPerFat;

	// set current directory to root (\)
	CurrentDirStartCluster = FatFsInfo.RootDirStartCluster;
	PathNameBuffer[0] = '\\';
	PathNameBuffer[1] = 0;

	// do debug
#ifdef DEBUG_FAT
	rprintfProgStrM("---------- Disk/Partition Information ----------\r\n");
	rprintfProgStrM("Volume/Part Size: ");	rprintfu32(FatFsInfo.devdisk.numsectors);		rprintfCRLF();
	rprintfProgStrM("---------- FAT Volume Information --------------\r\n");
	rprintfProgStrM("Reserved Sectors: ");	rprintfu16(bpb->bpbResSectors);				rprintfCRLF();
	rprintfProgStrM("FatSectors      : ");	rprintfu16(bpb->bpbFATsecs);				rprintfCRLF();
	rprintfProgStrM("BigFatSectors   : ");	rprintfu32(bpb->bpbBigFATsecs);				rprintfCRLF();
	rprintfProgStrM("Bytes/Sector    : ");	rprintfu16(FatFsInfo.BytesPerSector);		rprintfCRLF();
	rprintfProgStrM("First Fat Sector: ");	rprintfu32(FatFsInfo.FirstFATSector);		rprintfCRLF();
	rprintfProgStrM("Number of Fats  : ");	rprintfu08(FatFsInfo.NumberOfFats);			rprintfCRLF();
	rprintfProgStrM("Sectors/FAT     : ");  rprintfu32(FatFsInfo.SectorsPerFat);		rprintfCRLF();
	rprintfProgStrM("First Data Sect : ");	rprintfu32(FatFsInfo.FirstDataSector);		rprintfCRLF();
	rprintfProgStrM("Sectors/Cluster : ");	rprintfu08(FatFsInfo.SectorsPerCluster);	rprintfCRLF();
	rprintfProgStrM("RootDirStartClus: ");	rprintfu32(FatFsInfo.RootDirStartCluster);	rprintfCRLF();
#endif

	return 0;	
}


//////////////////////////////////////////////////////////////

unsigned char fatGetDirEntry(unsigned short entry)
{
	unsigned long sector;
	struct direntry *de = 0;	// avoid compiler warning by initializing
	struct winentry *we;
	unsigned char haveLongNameEntry;
	unsigned char gotEntry;
	unsigned short b;
	int i,index;
	char *fnbPtr;
	unsigned short entrycount = 0;

	// read dir data

	sector = fatClusterToSector(CurrentDirStartCluster);

	haveLongNameEntry = 0;
	gotEntry = 0;

	index = 16;	// crank it up
	
	//while(entrycount < entry) 
	while(1)
	{
		if(index == 16)	// time for next sector ?
		{
			FatFsInfo.devdisk.ReadSector(sector++, 1, SectorBuffer);
			de = (struct direntry *) SectorBuffer;
			index = 0;
		}
		
		// check the status of this directory entry slot
		if(de->deName[0] == 0x00)
		{
			// slot is empty and this is the end of directory
			gotEntry = 0;
			break;
		}
		else if(de->deName[0] == 0xE5)
		{
			// this is an empty slot
			// do nothing and move to the next one
		}
		else
		{
			// this is a valid and occupied entry
			// is it a part of a long file/dir name?
			if(de->deAttributes == ATTR_LONG_FILENAME)
			{
				// we have a long name entry
				// cast this directory entry as a "windows" (LFN: LongFileName) entry
				we = (struct winentry *) de;
				
				b = WIN_ENTRY_CHARS*( (we->weCnt-1) & 0x0f);		// index into string
				fnbPtr = &FileNameBuffer[b];
				for (i=0;i<5;i++)	*fnbPtr++ = we->wePart1[i*2];	// copy first part
				for (i=0;i<6;i++)	*fnbPtr++ = we->wePart2[i*2];	// second part
				for (i=0;i<2;i++)	*fnbPtr++ = we->wePart3[i*2];	// and third part
				if (we->weCnt & WIN_LAST) *fnbPtr = 0;				// in case dirnamelength is multiple of 13, add termination
				if ((we->weCnt & 0x0f) == 1) haveLongNameEntry = 1;	// flag that we have a complete long name entry set
			}
			else
			{
				// we have a short name entry
				
				// check if this is the short name entry corresponding
				// to the end of a multi-part long name entry
				if(haveLongNameEntry)
				{
					// a long entry name has been collected
					if(entrycount == entry)		
					{
						// desired entry has been found, break out
						gotEntry = 1;
						break;
					}
					// otherwise
					haveLongNameEntry = 0;	// clear long name flag
					entrycount++;			// increment entry counter		
				}
				else
				{
					// entry is a short name (8.3 format) without a
					// corresponding multi-part long name entry
					fnbPtr = FileNameBuffer;
					for (i=0;i<8;i++)	*fnbPtr++ = de->deName[i];		// copy name
					*fnbPtr++ = '.';									// insert '.'
					for (i=0;i<3;i++)	*fnbPtr++ = de->deExtension[i];	// copy extension
					*fnbPtr = 0;										// null-terminate

					if(entrycount == entry)		
					{
						// desired entry has been found, break out
						gotEntry = 1;
						break;
					}
					// otherwise
					entrycount++;			// increment entry counter		
				}
			}
		}
		// next directory entry
		de++;
		// next index
		index++;
	}
	
	// we have a file/dir to return
	// store file/dir starting cluster (start of data)
	FileInfo.StartCluster = (unsigned long) ((unsigned long)de->deHighClust << 16) + de->deStartCluster;
	// store file/dir size
	// (note: size field for subdirectory entries is always zero)
	FileInfo.Size = de->deFileSize;
	// store file/dir attributes
	FileInfo.Attr = de->deAttributes;
	// store file/dir creation time
	FileInfo.CreateTime = de->deCTime[0] | de->deCTime[1]<<8;
	// store file/dir creation date
	FileInfo.CreateTime = de->deCDate[0] | de->deCDate[1]<<8;

	return gotEntry;
}

int fatCreateFile(unsigned char* filename, FileInfo_t* fileInfo)
{
	unsigned long sector;
	struct direntry *de = 0;	// avoid compiler warning by initializing
	unsigned int index;
	unsigned char* strptr;
	unsigned char i;
	
	// find empty directory entry
	sector = fatClusterToSector(CurrentDirStartCluster);

	index = DIRENTRIES_PER_SECTOR;

	while(1)
	{
		if(index == DIRENTRIES_PER_SECTOR)	// time for next sector ?
		{
			FatFsInfo.devdisk.ReadSector(sector++, 1, SectorBuffer);
			de = (struct direntry *)SectorBuffer;
			index = 0;
		}
		
		// check the status of this directory entry slot
		if(de->deName[0] == SLOT_EMPTY)
		{
			// slot is empty and this is the end of the directory
			// need to make sure the next slot is written with a zero
			break;
		}
		else if(de->deName[0] == SLOT_DELETED)
		{
			// this is an empty slot, the entry was previously deleted
			break;
		}
		// empty/unused directory entry not yet found
		// move to the next one
		de++;
		index++;
	}

	// populate directory entry
	strptr = filename;
	// parse and fill filename
	for(i=0; i<8; i++)
	{
		if( (*strptr != '.') && (*strptr != 0) )
			de->deName[i] = *strptr++;
		else
			de->deName[i] = ' ';
	}
	// fill extension
	//strchr(filename, '.')
	if(*strptr == '.')
	{
		strptr++;
		de->deExtension[0] = *strptr++;
		de->deExtension[1] = *strptr++;
		de->deExtension[2] = *strptr++;
	}
	else
	{
		de->deExtension[0] = ' ';
		de->deExtension[1] = ' ';
		de->deExtension[2] = ' ';
	}
	// attributes
	de->deAttributes = fileInfo->Attr;
	// stuff
	de->deLowerCase = 0;
	// dates
	de->deCHundredth = 0;
	de->deCTime[0] = fileInfo->CreateTime;
	de->deCTime[1] = fileInfo->CreateTime>>8;
	de->deCDate[0] = fileInfo->CreateDate;
	de->deCDate[1] = fileInfo->CreateDate>>8;
	de->deADate[0] = 0;
	de->deADate[1] = 0;
	de->deMTime[0] = 0;
	de->deMTime[1] = 0;
	de->deMDate[0] = 0;
	de->deMDate[1] = 0;
	// file size
	de->deFileSize = fileInfo->Size;
	// set file's start cluster
	de->deStartCluster = fileInfo->StartCluster;
	de->deHighClust = fileInfo->StartCluster>>16;

	// write directory entry back to disk
	FatFsInfo.devdisk.WriteSector(--sector, 1, SectorBuffer);

	return 0;
}

int fatFormat(unsigned long volsize)
{
	unsigned long i,j,fatsecs;
	unsigned long sector;

	struct bootsector710 *bs;
	struct bpb710* bpb;

	// FIRST STAGE
	rprintfProgStrM("Calculating filesystem parameters\r\n");
	// clear sector buffer
	memset(SectorBuffer, 0, 512);
	// create boot sector and bios parameter block
	bs = (struct bootsector710 *)SectorBuffer;

	strcpy(&bs->bsOEMName[0], "MSDOS5.0");
	
	bpb = (struct bpb710*)bs->bsBPB;

	// BPB for FAT16
	bpb->bpbBytesPerSec = 0x200;	// standard 512Kbyte sectors
	bpb->bpbSecPerClust = 8;		// standard 4K clusters
	bpb->bpbResSectors = 4;			// number of reserved sectors before FAT starts
	bpb->bpbFATs = 2;				// standard 2 FATs
	bpb->bpbRootDirEnts = 0x200;	// 512 entries
	if(volsize < 0x10000)
	{
		// can be FAT16 only
		bpb->bpbSectors = volsize;	// size of the volume in sectors
		bpb->bpbHugeSectors = 0;	
	}
	else
	{
		// can be FAT16 or FAT32
		bpb->bpbSectors = 0;
		bpb->bpbHugeSectors = volsize;	// size of the volume in sectors
	}
	// -----------------------------------------------------------------
	// figure out how to split the drive into FAT(s) and data clusters
	// -determine the number of sectors usable for FAT and clusters
	i = volsize - (bpb->bpbResSectors + (bpb->bpbRootDirEnts+(DIRENTRIES_PER_SECTOR-1))/DIRENTRIES_PER_SECTOR);
	// -each cluster occupies bpbSecPerClust + 1 FAT entry (2/4 bytes) per fat
	j = ((unsigned short)bpb->bpbSecPerClust<<8) + bpb->bpbFATs;
	// if fat32 then j/=2;
	fatsecs = (i+(j-1))/j;
	/*
	rprintf("rootentr = "); rprintfu32(bpb->bpbRootDirEnts); rprintfCRLF();
	rprintf("i = "); rprintfu32(i); rprintfCRLF();
	rprintf("j = "); rprintfu32(j); rprintfCRLF();
	rprintf("fatsecs = %d\r\n", fatsecs);
	*/
	// -----------------------------------------------------------------	
	if(fatsecs < 65525)
	{
		// FAT16
		bpb->bpbFATsecs = fatsecs;	// number of sectors per FAT
	}
	else
	{
		// FAT32
		bpb->bpbFATsecs = 0;			// real value is in bigFatSecs
		bpb->bpbBigFATsecs = fatsecs;	// number of sectors per FAT
	}
	bpb->bpbMedia = 0xF8;			// 0xF8 = standard fixed disk
	bpb->bpbSecPerTrack = 63;	
	bpb->bpbHeads = 255;
	bpb->bpbHiddenSecs = 0;
	// BPB for FAT32
	bpb->bpbExtFlags = 0;			// FATs are mirrored
	bpb->bpbFSVers = FSVERS;

	// apply bootsector signature
	bs->bsBootSectSig0 = 0x55;
	bs->bsBootSectSig1 = 0xAA;

	// write bootsector
	rprintfProgStrM("Writing boot sector\r\n");
	FatFsInfo.devdisk.WriteSector(0, 1, SectorBuffer);

	// SECOND STAGE
	// initialize the FAT(s) and create the root directory

	// "remount" the filesystem
	rprintfProgStrM("Remounting Filesystem...\r\n");
	fatInit(&FatFsInfo.devdisk);
	// clear sector buffer
	memset(SectorBuffer, 0, 512);
	// initialize the FATs
	rprintfProgStrM("Initializing FAT sectors...\r\n");
	for(i=0; i<(FatFsInfo.NumberOfFats*FatFsInfo.SectorsPerFat); i++)
	{
		rprintfProgStrM("Writing FAT sector: "); rprintfu32(FatFsInfo.FirstFATSector+i); rprintfChar('\r');
		FatFsInfo.devdisk.WriteSector(FatFsInfo.FirstFATSector+i, 1, SectorBuffer);
	}
	rprintfCRLF();

	// create the root directory
	rprintfProgStrM("Creating Root Directory...\r\n");
	// tag root directory cluster as used in FAT
	fatWriteClusterValue(FatFsInfo.RootDirStartCluster, CLUST_EOFE);
	// find the starting sector of the root directory
	sector = fatClusterToSector(FatFsInfo.RootDirStartCluster);
	// clear sector buffer
	memset(SectorBuffer, 0, 512);
	// clear the cluster where the root directory will reside
	for(i=0; i<FatFsInfo.SectorsPerCluster; i++)
	{
		rprintfProgStrM("Writing Root Dir sector: "); rprintfu32(sector+i); rprintfChar('\r');
		FatFsInfo.devdisk.WriteSector(sector+i, 1, SectorBuffer);
	}
	rprintfCRLF();

	// create volume label entry
	rprintfProgStrM("Creating Volume Label...\r\n");
	
	FileInfo.Attr = ATTR_VOLUME;
	FileInfo.Size = 0;
	FileInfo.StartCluster = 0;
	FileInfo.CreateDate = 0;
	FileInfo.CreateTime = 0;

	fatCreateFile("MYDISK", &FileInfo);

	return 0;
}

// change directory into 
unsigned char fatChangeDirectory(unsigned short entry)
{
	// get the requested directory entry
	if(	fatGetDirEntry(entry) )
	{
		// make sure the entry is a directory
		if(FileInfo.Attr & ATTR_DIRECTORY)
		{
			// change directories into this directory
			// check to see if we are changing back to root directory
			if(FileInfo.StartCluster)
			{
				// standard change directory
				CurrentDirStartCluster = FileInfo.StartCluster;
			}
			else
			{
				// if startCluster pointer is zero,
				// a change to the root directory is intended
				// change directory to root
				CurrentDirStartCluster = FatFsInfo.RootDirStartCluster;
			}
			// TODO: handle pathname properly for going up a directory
			// set path string
			strcat(PathNameBuffer, FileNameBuffer);
			strcat(PathNameBuffer, "\\");
			// return success
			return TRUE;
		}
		else
		{
			// not a directory, cannot CD into a file!
			return FALSE;
		}
	}
	else
	{
		// not a valid entry, cannot CD!
		return FALSE;
	}
}

void fatPrintDirEntry(void)
{
	// print a formatted dir-style output for most recent file
	// print date
	rprintfNum(10, 2, FALSE, '0', (FileInfo.CreateDate&DD_MONTH_MASK)>>DD_MONTH_SHIFT );	// month
	rprintfChar('/');
	rprintfNum(10, 2, FALSE, '0', (FileInfo.CreateDate&DD_DAY_MASK)>>DD_DAY_SHIFT );		// day
	rprintfChar('/');
	rprintfNum(10, 4, FALSE, '0', (FileInfo.CreateDate&DD_YEAR_MASK)>>DD_YEAR_SHIFT );	// year
	rprintfChar(' ');

	// print time
	rprintfNum(10, 2, FALSE, '0', (FileInfo.CreateTime&DT_HOURS_MASK)>>DT_HOURS_SHIFT );	// month
	rprintfChar(':');
	rprintfNum(10, 2, FALSE, '0', (FileInfo.CreateTime&DT_MINUTES_MASK)>>DT_MINUTES_SHIFT );		// day
	rprintfChar(':');
	rprintfNum(10, 2, FALSE, '0', 2*(FileInfo.CreateTime&DT_2SECONDS_MASK)>>DT_2SECONDS_SHIFT );	// seconds
	rprintfChar(' ');

	// print attributes
	if(FileInfo.Attr & ATTR_VOLUME)		rprintfChar('V'); else rprintfChar('-');
	if(FileInfo.Attr & ATTR_DIRECTORY)	rprintfChar('D'); else rprintfChar('-');
	if(FileInfo.Attr & ATTR_READONLY)	rprintfChar('R'); else rprintfChar('-');
	if(FileInfo.Attr & ATTR_HIDDEN)		rprintfChar('H'); else rprintfChar('-');
	if(FileInfo.Attr & ATTR_SYSTEM)		rprintfChar('S'); else rprintfChar('-');
	if(FileInfo.Attr & ATTR_ARCHIVE)	rprintfChar('A'); else rprintfChar('-');
	rprintfChar(' ');

	// print filesize
	rprintfNum(10, 8, FALSE, ' ', FileInfo.Size);	// filesize
	rprintfChar(' ');

	// print filename
	rprintfStr(FileNameBuffer);
}

void fatDumpDirSlot(unsigned short slot)
{
	unsigned long sector;
	
	// load correct sector
	sector = fatClusterToSector(CurrentDirStartCluster);
	sector += slot/DIRENTRIES_PER_SECTOR;

	rprintf("CurrentDirStartCluster:");
	rprintfu32(CurrentDirStartCluster);
	rprintfCRLF();
	rprintf("Sector access         :");
	rprintfu32(sector);
	rprintfCRLF();
		
	// print the entry as a hex table
	debugPrintHexTable(32, SectorBuffer+(slot<<5) );
}

FileInfo_t* fatGetFileInfo(void)
{
	return &FileInfo;
}

// return the size of the last directory entry
unsigned long fatGetFilesize(void)
{
	return FileInfo.Size;
}

// return the long name of the last directory entry
char* fatGetFilename(void)
{	
	return FileNameBuffer;
}

// return the directory of the last directory entry
char* fatGetDirname(void)
{	
	return PathNameBuffer;
}

// load a cluster-full of data
void fatLoadCluster(unsigned long cluster, unsigned char *buffer)
{
	register unsigned char i;
	// read cluster
	//while ( ataReadSectors( FatFsInfo.disk, clust2sect(cluster), SectorsPerCluster, buffer) != 0);
	for(i=0; i<FatFsInfo.SectorsPerCluster; i++)
	{
		FatFsInfo.devdisk.ReadSector(fatClusterToSector(cluster)+i, 1, buffer+(i<<9) );
		// temporary fix for wierd misaligned cluster problem
		// (only when using FAT16?)
//		ataReadSectors(FatFsInfo.disk, fatClustToSect(cluster+8)+i, 1, buffer+(i<<9) );
	}
}


// find next cluster in the FAT chain
unsigned long fatNextCluster(unsigned long cluster)
{
	unsigned long nextCluster;

	// read the nextCluster value
	nextCluster = fatClusterValue(cluster);

	// check to see if we're at the end of the chain
	if (nextCluster == (CLUST_EOFE & FatFsInfo.FatMask))
		nextCluster = 0;

#ifdef DEBUG_FAT
	rprintfProgStrM(">");
	rprintfu32(nextCluster);
	rprintfCRLF();
#endif
	
	return nextCluster;
}

unsigned long fatNextFreeCluster(void)
{
	unsigned long cluster;

	// start at beginning of fat
	cluster = FatFsInfo.RootDirStartCluster;

	while( fatClusterValue(cluster) != CLUST_FREE)
	{
		// increment to next cluster
		cluster++;
	}
	return cluster;
}

unsigned long fatClusterValue(unsigned long cluster)
{
	// return the value of the FAT entry for the requested cluster
	unsigned char* buffer;
	unsigned long fatOffset;
	unsigned long sector;
	unsigned int offset;
	
	// get fat offset in bytes
	if(FatFsInfo.Fat32Enabled)
	{
		// four FAT bytes (32 bits) for every cluster
		fatOffset = cluster << 2;
	}
	else
	{
		// two FAT bytes (16 bits) for every cluster
		fatOffset = cluster << 1;
	}
	
	// calculate the FAT sector that we're interested in
	sector = (fatOffset / FatFsInfo.BytesPerSector);
	// calculate offset of the our entry within that FAT sector
	offset = fatOffset % FatFsInfo.BytesPerSector;
	// get the fat sector
	buffer = fatGetFatSector( sector );
	// return the fat value of the cluster
	// NOTE: don't be fooled by the UNSIGNED LONG,
	// this should nominally work for both FAT16 and FAT32
	// FAT16 -> offset will be on 2-byte boundary
	// FAT32 -> offset will be on 4-byte boundary
	return (*((unsigned long*) &buffer[offset])) & FatFsInfo.FatMask;
}

int fatWriteClusterValue(unsigned long cluster, unsigned value)
{
	// write the value of the FAT entry for the requested cluster
	unsigned char* buffer;
	unsigned long fatOffset;
	unsigned long fatSector;
	unsigned int offset;
	
	// get fat offset in bytes
	if(FatFsInfo.Fat32Enabled)
	{
		// four FAT bytes (32 bits) for every cluster
		fatOffset = cluster << 2;
	}
	else
	{
		// two FAT bytes (16 bits) for every cluster
		fatOffset = cluster << 1;
	}
	
	// calculate the FAT sector that we're interested in
	fatSector = (fatOffset / FatFsInfo.BytesPerSector);
	// calculate offset of the our entry within that FAT sector
	offset = fatOffset % FatFsInfo.BytesPerSector;
	// get the fat sector
	buffer = fatGetFatSector( fatSector );
	// write the fat value of the cluster
	// NOTE: don't be fooled by the UNSIGNED LONG,
	// this should nominally work for both FAT16 and FAT32
	// FAT16 -> offset will be on 2-byte boundary
	// FAT32 -> offset will be on 4-byte boundary
	*((unsigned long*) &buffer[offset]) = (value & FatFsInfo.FatMask);

	// flush to disk
	FatFsInfo.devdisk.WriteSector(FatFsInfo.FirstFATSector+fatSector, 1, buffer);

	return 0;
}

unsigned char* fatGetFatSector(unsigned long fatsector)
{
	// NOTE: fatsector is referenced to the beginning of the fat
	// eg. fatsector=0 is the first sector of the first FAT

	// the idea here is to make a generic FAT loading function that
	// can later be used to cache not one but multiple FAT sectors that
	// have been recently used

	// if we don't already have this FAT chunk loaded, go get it
	if(fatsector != FatFsInfo.FatInCache)
	{
		// read sector of FAT table
		while (FatFsInfo.devdisk.ReadSector(FatFsInfo.FirstFATSector+fatsector, 1, FatCacheBuffer) != 0);
		FatFsInfo.FatInCache = fatsector;
	}
	return FatCacheBuffer;
}

unsigned long fatClusterToSector(unsigned long cluster)
{
	return ((cluster-2) * FatFsInfo.SectorsPerCluster) + FatFsInfo.FirstDataSector;
}

unsigned int fatClusterSize(void)
{
	// return the number of sectors in a disk cluster
	return FatFsInfo.SectorsPerCluster;
}
