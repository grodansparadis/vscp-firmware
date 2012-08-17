/*! \file ata.h \brief IDE-ATA hard disk interface driver. */
//*****************************************************************************
//
// File Name	: 'ata.h'
// Title		: IDE-ATA interface driver for hard disks
// Author		: Pascal Stang
// Date			: 11/22/2000
// Revised		: 4/2/2004
// Version		: 0.1
// Target MCU	: any
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


#ifndef ATA_H
#define ATA_H

#include "global.h"
#include "atadev.h"
#include "ataconf.h"

// constants
#define DRIVE0		0

#define STANDBY		0
#define SLEEP		1
#define IDLE		2

// ATA status register bits
#define ATA_SR_BSY		0x80
#define ATA_SR_DRDY		0x40
#define ATA_SR_DF		0x20
#define ATA_SR_DSC		0x10
#define ATA_SR_DRQ		0x08
#define ATA_SR_CORR		0x04
#define ATA_SR_IDX		0x02
#define ATA_SR_ERR		0x01

// ATA error register bits
#define ATA_ER_UNC		0x40
#define ATA_ER_MC		0x20
#define ATA_ER_IDNF		0x10
#define ATA_ER_MCR		0x08
#define ATA_ER_ABRT		0x04
#define ATA_ER_TK0NF	0x02
#define ATA_ER_AMNF		0x01

// ATA head register bits
#define ATA_HEAD_USE_LBA	0x40

// ATA commands
#define ATA_CMD_READ			0x20
#define ATA_CMD_READNR			0x21
#define ATA_CMD_WRITE			0x30
#define ATA_CMD_WRITENR			0x31
#define ATA_CMD_IDENTIFY		0xEC
#define ATA_CMD_RECALIBRATE		0x10
#define ATA_CMD_SPINDOWN		0xE0	// spin down disk immediately
#define ATA_CMD_SPINUP			0xE1	// spin up disk immediately
#define ATA_CMD_STANDBY_5SU		0xE2	// spin down disk and set auto-power-down timer (sectorcount*5sec)
#define ATA_CMD_IDLE_5SU		0xE3	// keep disk spinning and set auto-power-down timer (sectorcount*5sec)
#define ATA_CMD_SLEEP			0xE6	// sleep disk (wakeup only on HW or SW reset)
#define ATA_CMD_STANDBY_01SU	0xF2	// spin down disk and set auto-power-down timer (sectorcount*0.1sec)
#define ATA_CMD_IDLE_01SU		0xF3	// keep disk spinning and set auto-power-down timer (sectorcount*0.1sec)


// ATA CHS disk parameters (examples, now we autodetect)
#define ATA_DISKPARM_CLYS		0x03A6	// number of cylinders per platter
#define ATA_DISKPARM_HEADS		0x10	// number of heads (usable plater sides)
#define ATA_DISKPARM_SECTORS	0x11	// number of sectors per head per cylinder

// ATA Identity fields
// all offsets refer to word offset (2 byte increments)
#define ATA_IDENT_DEVICETYPE	0		// specifies ATA/ATAPI, removable/non-removable
#define ATA_IDENT_CYLINDERS		1		// number of logical cylinders
#define ATA_IDENT_HEADS			3		// number of logical heads
#define ATA_IDENT_SECTORS		6		// number of sectors per track
#define ATA_IDENT_SERIAL		10		// drive serial number (20 characters)
#define ATA_IDENT_FIRMWAREREV	23		// drive firmware revision (8 characters)
#define ATA_IDENT_MODEL			27		// drive model name (40 characters)
#define ATA_IDENT_FIELDVALID	53		// indicates field validity of higher words (bit0: words54-58, bit1: words 64-70)
#define ATA_IDENT_LBASECTORS	60		// number of sectors in LBA translation mode

// drive mode defines (for ataSetDrivePowerMode() )
#define ATA_DISKMODE_SPINDOWN	0
#define ATA_DISKMODE_SPINUP		1
#define ATA_DISKMODE_SETTIMEOUT	2
#define ATA_DISKMODE_SLEEP		3

// typedefs
// drive info structure
struct DiskInfo
{
	DevBlock_t ataif;
	unsigned long sizeinsectors;
	unsigned short cylinders;
	unsigned char heads;
	unsigned char sectors;
	unsigned char LBAsupport;
	unsigned char driveno;
	char model[41];
} GNUC_PACKED;

typedef struct DiskInfo DiskInfo_t;

// Prototypes
void ataInit(DiskInfo_t* disk, DevBlock_t ataif, unsigned char driveno);
void ataDriveInit(DiskInfo_t* disk);

void ataDriveSelect(DiskInfo_t* disk);
void ataSetDrivePowerMode(DiskInfo_t* disk, u08 mode, u08 timeout);

void ataShowRegisters(DiskInfo_t* disk);
u08  ataSWReset(DiskInfo_t* disk);
void ataDiskErr(DiskInfo_t* disk);
u08 ataStatusWait(DiskInfo_t* disk, u08 mask, u08 waitStatus);

// read and write routines for CHS based drives
unsigned char ataReadSectorsCHS(	DiskInfo_t* disk,
									unsigned char Head, 
									unsigned int Track,
									unsigned char Sector,
									unsigned int numsectors,
									unsigned char *Buffer);

unsigned char ataWriteSectorsCHS(	DiskInfo_t* disk,
									unsigned char Head, 
									unsigned int Track,
									unsigned char Sector,
									unsigned int numsectors,
									unsigned char *Buffer);

// read and write routines for LBA based drives
unsigned char ataReadSectorsLBA(	DiskInfo_t* disk,
									unsigned long lba,
									unsigned int numsectors,
                            		unsigned char *Buffer);

unsigned char ataWriteSectorsLBA(	DiskInfo_t* disk,
									unsigned long lba,
									unsigned int numsectors,
                            		unsigned char *Buffer);

// generic read and write routines using LBA
//   uses native or translated LBA addressing
//   given autodetected drive type
unsigned char ataReadSectors(	DiskInfo_t* disk, 
								unsigned long lba,
								unsigned int numsectors,
                            	unsigned char *Buffer);

unsigned char ataWriteSectors(	DiskInfo_t* disk, 
								unsigned long lba,
								unsigned int numsectors,
                            	unsigned char *Buffer);

#endif
