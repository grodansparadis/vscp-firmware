/*! \file ata.c \brief IDE-ATA hard disk interface driver. */
//*****************************************************************************
//
// File Name	: 'ata.c'
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

#include "global.h"
#include "timer.h"
#include "rprintf.h"
#include "debug.h"

#include "atadev.h"
#include "ata.h"

// global variables
unsigned char AtaBuffer[0x200];

// functions
void ataInit(DiskInfo_t* disk, DevBlock_t ataif, unsigned char driveno)
{
	// set drive interface 
	disk->ataif = ataif;	
	// set drive number (0=master, 1=slave)
	disk->driveno = driveno;
	// initialize interface
	disk->ataif.Init();
}

void ataDriveInit(DiskInfo_t* disk)
{
	u08 i;
	//unsigned char* buffer = (unsigned char*)SECTOR_BUFFER_ADDR;
	unsigned char* buffer = AtaBuffer;

//	rprintfProgStrM("Init      :"); rprintfu32((unsigned int)disk->ataif.Init); rprintfCRLF();
//	rprintfProgStrM("ReadReg   :"); rprintfu32((unsigned int)disk->ataif.ReadReg); rprintfCRLF();
//	rprintfProgStrM("WriteReg  :"); rprintfu32((unsigned int)disk->ataif.WriteReg); rprintfCRLF();
//	rprintfProgStrM("ReadBlock :"); rprintfu32((unsigned int)disk->ataif.ReadBlock); rprintfCRLF();
//	rprintfProgStrM("WriteBlock:"); rprintfu32((unsigned int)disk->ataif.WriteBlock); rprintfCRLF();

	// read drive identity
	rprintfProgStrM("\r\nScanning IDE interface...\r\n");
	// Wait for drive to be ready
	ataStatusWait(disk, ATA_SR_BSY, ATA_SR_BSY);
	// issue identify command
	disk->ataif.WriteReg(ATA_REG_CMDSTATUS1, 0xEC);
	// wait for drive to request data transfer
	ataStatusWait(disk, ATA_SR_DRQ, ATA_SR_DRQ);
	timerPause(20);
	// read in the data
	disk->ataif.ReadBlock(buffer, 512);

	// set local drive info parameters
	disk->cylinders =		*( ((unsigned short*) buffer) + ATA_IDENT_CYLINDERS );
	disk->heads =			*( ((unsigned short*) buffer) + ATA_IDENT_HEADS );
	disk->sectors =			*( ((unsigned short*) buffer) + ATA_IDENT_SECTORS );
	disk->LBAsupport =		*( ((unsigned short*) buffer) + ATA_IDENT_FIELDVALID );
	disk->sizeinsectors =	*( ((unsigned long*) (buffer + ATA_IDENT_LBASECTORS*2)) );

	// copy model string
	for(i=0; i<40; i+=2)
	{
		// correct for byte order
		disk->model[i  ] = buffer[(ATA_IDENT_MODEL*2) + i + 1];
		disk->model[i+1] = buffer[(ATA_IDENT_MODEL*2) + i    ];
	}
	// terminate string
	disk->model[40] = 0;

	// process and print info
	if(disk->LBAsupport)
	{
		// LBA support
		rprintf("Drive 0: %dMB ", disk->sizeinsectors/(1000000/512) );
		rprintfProgStrM("LBA mode -- MODEL: ");
	}
	else
	{
		// CHS, no LBA support
		// calculate drive size
		disk->sizeinsectors = (unsigned long) disk->cylinders*disk->heads*disk->sectors;
		rprintf("Drive 0: %dMB ", disk->sizeinsectors/(1000000/512) );
		rprintf("CHS mode C=%d H=%d S=%d -- MODEL: ", disk->cylinders, disk->heads, disk->sectors );
	}
	// print model information	
	rprintfStr(disk->model); rprintfCRLF();

	// initialize local disk parameters
	//ataDriveInfo.cylinders = ATA_DISKPARM_CLYS;
	//ataDriveInfo.heads = ATA_DISKPARM_HEADS;
	//ataDriveInfo.sectors = ATA_DISKPARM_SECTORS;
}

void ataDiskErr(DiskInfo_t* disk)
{
	unsigned char b;

	b = disk->ataif.ReadReg(ATA_REG_ERROR);	
	rprintfProgStrM("ATA Error: "); 
	rprintfu08(b); 
	rprintfCRLF();
}

void ataSetDrivePowerMode(DiskInfo_t* disk, u08 mode, u08 timeout)
{
	// select drive
	ataDriveSelect(disk);
	// Wait for drive to be ready
	ataStatusWait(disk, ATA_SR_BSY, ATA_SR_BSY);

	// set mode
	switch(mode)
	{
	case ATA_DISKMODE_SPINDOWN:
		disk->ataif.WriteReg(ATA_REG_CMDSTATUS1, ATA_CMD_SPINDOWN);
		break;
	case ATA_DISKMODE_SPINUP:
		disk->ataif.WriteReg(ATA_REG_CMDSTATUS1, ATA_CMD_SPINUP);
		break;
	case ATA_DISKMODE_SETTIMEOUT:
		disk->ataif.WriteReg(ATA_REG_SECCOUNT, timeout);
		disk->ataif.WriteReg(ATA_REG_CMDSTATUS1, ATA_CMD_IDLE_5SU);
		break;
	case ATA_DISKMODE_SLEEP:
		disk->ataif.WriteReg(ATA_REG_CMDSTATUS1, ATA_CMD_SLEEP);
		break;
	default:
		break;
	}
}

u08 ataStatusWait(DiskInfo_t* disk, u08 mask, u08 waitStatus)
{
	register u08 status;

	delay(100);

	// wait for desired status
	while( ((status = disk->ataif.ReadReg(ATA_REG_CMDSTATUS1)) & mask) == waitStatus );

	return status;
}


unsigned char ataReadSectorsCHS(	DiskInfo_t* disk, 
									unsigned char Head, 
									unsigned int Track,
									unsigned char Sector,
									unsigned int numsectors,
									unsigned char *Buffer)
{
  	unsigned char temp;

	// Wait for drive to be ready
	temp = ataStatusWait(disk, ATA_SR_BSY, ATA_SR_BSY);

  	// Prepare parameters...
	disk->ataif.WriteReg(ATA_REG_HDDEVSEL, 0xA0+(disk->driveno ? 0x10:00)+Head); // CHS mode/Drive/Head
	disk->ataif.WriteReg(ATA_REG_CYLHI, Track>>8);  		// MSB of track
	disk->ataif.WriteReg(ATA_REG_CYLLO, Track);     		// LSB of track
  	disk->ataif.WriteReg(ATA_REG_STARTSEC, Sector);    	// sector
	disk->ataif.WriteReg(ATA_REG_SECCOUNT, numsectors);	// # of sectors

	// Issue read sector command...
  	disk->ataif.WriteReg(ATA_REG_CMDSTATUS1, 0x21);

	// Wait for drive to be ready
	temp = ataStatusWait(disk, ATA_SR_BSY, ATA_SR_BSY);

	if (temp & ATA_SR_ERR)
	{
		rprintfProgStrM("RD ERR\r\n");
		return 1;
	}

	// Wait for drive to request data transfer
	ataStatusWait(disk, ATA_SR_DRQ, 0);

	// read data from drive
	disk->ataif.ReadBlock(Buffer, 512*numsectors);

	// Return the error bit from the status register...
	temp = disk->ataif.ReadReg(ATA_REG_CMDSTATUS1);	// read status register

	return (temp & ATA_SR_ERR) ? 1:0;
}


unsigned char ataWriteSectorsCHS(	DiskInfo_t* disk,
									unsigned char Head, 
									unsigned int Track,
									unsigned char Sector,
									unsigned int numsectors,
									unsigned char *Buffer)
{
	unsigned char temp;

	// Wait for drive to be ready
	temp = ataStatusWait(disk, ATA_SR_BSY, ATA_SR_BSY);

  	// Prepare parameters...
	disk->ataif.WriteReg(ATA_REG_HDDEVSEL, 0xA0+(disk->driveno ? 0x10:00)+Head); // CHS mode/Drive/Head
	disk->ataif.WriteReg(ATA_REG_CYLHI, Track>>8);  		// MSB of track
	disk->ataif.WriteReg(ATA_REG_CYLLO, Track);     		// LSB of track
  	disk->ataif.WriteReg(ATA_REG_STARTSEC, Sector);    	// sector
	disk->ataif.WriteReg(ATA_REG_SECCOUNT, numsectors);	// # of sectors

	// Issue write sector command
  	disk->ataif.WriteReg(ATA_REG_CMDSTATUS1, 0x31);

	//delay(100);

	// Wait for drive to request data transfer
	ataStatusWait(disk, ATA_SR_DRQ, 0);

	// write data to drive
	disk->ataif.WriteBlock(Buffer, 512*numsectors);
	
	// Wait for drive to finish write
	temp = ataStatusWait(disk, ATA_SR_BSY, ATA_SR_BSY);

	// check for errors
	if (temp & ATA_SR_ERR)
	{
		rprintfProgStrM("WR ERR\r\n");
		return 1;
	}

	// Return the error bit from the status register...
	return (temp & ATA_SR_ERR) ? 1:0;
}

unsigned char ataReadSectorsLBA(	DiskInfo_t* disk,
									unsigned long lba,
									unsigned int numsectors,
                            		unsigned char *Buffer)
{
  	unsigned int cyl, head, sect;
  	unsigned char temp;

#ifdef DEBUG_ATA
	rprintfProgStrM("ATA LBA read ");
	rprintfu32(lba); rprintfChar(' ');
	rprintfu16(numsectors); rprintfChar(' ');
	rprintfu16((unsigned int)Buffer); 
	rprintfCRLF();
#endif

	sect = (int) ( lba & 0x000000ffL );
	lba = lba >> 8;
	cyl = (int) ( lba & 0x0000ffff );
	lba = lba >> 16;
	head = ( (int) ( lba & 0x0fL ) ) | ATA_HEAD_USE_LBA;

	temp = ataReadSectorsCHS( disk, head, cyl, sect, numsectors, Buffer );

	if(temp)
		ataDiskErr(disk);
	return temp;
}

unsigned char ataWriteSectorsLBA(	DiskInfo_t* disk,
									unsigned long lba,
									unsigned int numsectors,
                            		unsigned char *Buffer)
{
	unsigned int cyl, head, sect;
	unsigned char temp;

#ifdef DEBUG_ATA
	rprintfProgStrM("ATA LBA write ");
	rprintfu32(lba); rprintfChar(' ');
	rprintfu16(numsectors); rprintfChar(' ');
	rprintfu16((unsigned int)Buffer); 
	rprintfCRLF();
#endif

	sect = (int) ( lba & 0x000000ffL );
	lba = lba >> 8;
	cyl = (int) ( lba & 0x0000ffff );
	lba = lba >> 16;
	head = ( (int) ( lba & 0x0fL ) ) | ATA_HEAD_USE_LBA;

	temp = ataWriteSectorsCHS( disk, head, cyl, sect, numsectors, Buffer );

	if(temp)
		ataDiskErr(disk);
	return temp;
}                            		


unsigned char ataReadSectors(	DiskInfo_t* disk,
								unsigned long lba,
								unsigned int numsectors,
                            	unsigned char *Buffer)
{
  	unsigned int cyl, head, sect;
  	unsigned char temp;

	// check if drive supports native LBA mode
	if(disk->LBAsupport)
	{
		// drive supports using native LBA
		temp = ataReadSectorsLBA(disk, lba, numsectors, Buffer);
	}
	else
	{
		// drive required CHS access
		#ifdef DEBUG_ATA
			// do this defore destroying lba
			rprintfProgStrM("ATA LBA for CHS read: ");
			rprintfProgStrM("LBA="); rprintfu32(lba); rprintfChar(' ');
		#endif

		// convert LBA to pseudo CHS
		// remember to offset the sector count by one
		sect = (u08) (lba % disk->sectors)+1;
		lba = lba / disk->sectors;
		head = (u08) (lba % disk->heads);
		lba = lba / disk->heads;
		cyl = (u16) lba;

		#ifdef DEBUG_ATA
			rprintfProgStrM("C:H:S=");
			rprintfu16(cyl); rprintfChar(':');
			rprintfu08(head); rprintfChar(':');
			rprintfu08(sect); rprintfCRLF();
		#endif

		temp = ataReadSectorsCHS( disk, head, cyl, sect, numsectors, Buffer );
	}

	if(temp)
		ataDiskErr(disk);
	return temp;
}


unsigned char ataWriteSectors(	DiskInfo_t* disk,
								unsigned long lba,
								unsigned int numsectors,
                            	unsigned char *Buffer)
{
  	unsigned int cyl, head, sect;
  	unsigned char temp;

	// check if drive supports native LBA mode
	if(disk->LBAsupport)
	{
		// drive supports using native LBA
		temp = ataWriteSectorsLBA(disk, lba, numsectors, Buffer);
	}
	else
	{
		// drive required CHS access
		#ifdef DEBUG_ATA
			// do this defore destroying lba
			rprintfProgStrM("ATA LBA for CHS write: ");
			rprintfProgStrM("LBA="); rprintfu32(lba); rprintfChar(' ');
		#endif

		// convert LBA to pseudo CHS
		// remember to offset the sector count by one
		sect = (u08) (lba % disk->sectors)+1;
		lba = lba / disk->sectors;
		head = (u08) (lba % disk->heads);
		lba = lba / disk->heads;
		cyl = (u16) lba;

		#ifdef DEBUG_ATA
			rprintfProgStrM("C:H:S=");
			rprintfu16(cyl); rprintfChar(' ');
			rprintfu08(head); rprintfChar(' ');
			rprintfu08(sect); rprintfCRLF();
		#endif

		temp = ataWriteSectorsCHS( disk, head, cyl, sect, numsectors, Buffer );
	}

	if(temp)
		ataDiskErr(disk);
	return temp;
}                            		

void ataDriveSelect(DiskInfo_t* disk)
{
	disk->ataif.WriteReg(ATA_REG_HDDEVSEL, 0xA0+(disk->driveno ? 0x10:00)); // Drive selection
}

void ataShowRegisters(DiskInfo_t* disk) 
{ 
	disk->ataif.WriteReg(ATA_REG_HDDEVSEL, 0xA0 + (disk->driveno ? 0x10:0x00)); // Select drive
	
	rprintfProgStrM("R0: DATA     = 0x");	rprintfu08(disk->ataif.ReadReg(ATA_REG_DATA		));		rprintfProgStrM(" \r\n");
	rprintfProgStrM("R1: ERROR    = 0x");	rprintfu08(disk->ataif.ReadReg(ATA_REG_ERROR	));		rprintfProgStrM(" \r\n");
	rprintfProgStrM("R2: SECT CNT = 0x");	rprintfu08(disk->ataif.ReadReg(ATA_REG_SECCOUNT));		rprintfProgStrM(" \r\n");
	rprintfProgStrM("R3: SECT NUM = 0x");	rprintfu08(disk->ataif.ReadReg(ATA_REG_STARTSEC));		rprintfProgStrM(" \r\n");
	rprintfProgStrM("R4: CYL LOW  = 0x");	rprintfu08(disk->ataif.ReadReg(ATA_REG_CYLLO	));		rprintfProgStrM(" \r\n");
	rprintfProgStrM("R5: CYL HIGH = 0x");	rprintfu08(disk->ataif.ReadReg(ATA_REG_CYLHI	));		rprintfProgStrM(" \r\n");
	rprintfProgStrM("R6: HEAD/DEV = 0x");	rprintfu08(disk->ataif.ReadReg(ATA_REG_HDDEVSEL));		rprintfProgStrM(" \r\n");
	rprintfProgStrM("R7: CMD/STA  = 0x");	rprintfu08(disk->ataif.ReadReg(ATA_REG_CMDSTATUS1));	rprintfProgStrM("\r\n");
}

unsigned char ataSWReset(DiskInfo_t* disk)
{
	disk->ataif.WriteReg(ATA_REG_HDDEVSEL, 0x06);	// SRST and nIEN bits
	delay(10);	// 10uS delay
	disk->ataif.WriteReg(ATA_REG_HDDEVSEL, 0x02);	// nIEN bits
	delay(10);	// 10uS delay
   
	while( (disk->ataif.ReadReg(ATA_REG_CMDSTATUS1) & 0xC0) != 0x40 ); // Wait for DRDY and not BSY
    
	return disk->ataif.ReadReg(ATA_REG_CMDSTATUS1) + disk->ataif.ReadReg(ATA_REG_ERROR);
}

/*
unsigned char ATA_Idle(unsigned char Drive)
{

  WriteBYTE(CMD, 6, 0xA0 + (Drive ? 0x10:0x00)); // Select drive
  WriteBYTE(CMD,7, 0xE1);

  while ((ReadBYTE(CMD,7) & 0xC0)!=0x40); // Wait for DRDY & NOT BUSY 

  // Return the error register...
  return ReadBYTE(CMD, 1);
}
*/
//----------------------------------------------------------------------------
// Set drive mode (STANDBY, IDLE)
//----------------------------------------------------------------------------
/*#define STANDBY 0
#define IDLE    1
#define SLEEP   2 
*/ 

/*
unsigned char SetMode(unsigned char DriveNo, unsigned char Mode, unsigned char PwrDown) 
{
  WriteBYTE(CMD, 6, 0xA0 + (DriveNo ? 0x10:0x00)); // Select drive
  WriteBYTE(CMD, 2, (PwrDown ? 0x01:0x00)); // Enable automatic power down
  switch (Mode) 
  {
    case STANDBY: WriteBYTE(CMD,7, 0xE2); break;
    case IDLE:    WriteBYTE(CMD,7, 0xE3); break;
    // NOTE: To recover from sleep, either issue a soft or hardware reset !
    // (But not on all drives, f.ex seagate ST3655A it's not nessecary to reset
    // but only to go in Idle mode, But on a Conner CFA170A it's nessecary with
    // a reset)
    case SLEEP:   WriteBYTE(CMD,7, 0xE6); break;
  }
  Timer10mSec=10000;
  while ((ReadBYTE(CMD,7) & 0xC0)!=0x40 && Timer10mSec); // Wait for DRDY & NOT BUSY 
  if (Timer10mSec==0) return 0xFF;                       //   or timeout
 
  // Return the error register...
  return ReadBYTE(CMD, 1);
}

*/
