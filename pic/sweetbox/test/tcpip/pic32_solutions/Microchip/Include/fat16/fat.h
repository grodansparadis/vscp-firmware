/******************************************************************************
 *
 *       FAT16 FILESYSTEM HEADER FILE
 *
 ******************************************************************************
 * FileName:        FAT.h
 * Dependencies:    See include below
 * Processor:       Daytona
 * Compiler:        Daytona
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 * Microchip Technology Incorporated ("Microchip) and its licensors retain 
 * all ownership and intellectual property rights in the code accompanying 
 * this message and in all derivatives thereto.  The code and information 
 * accompanying this message is provided for your convenience and use with 
 * Microchip products only.  Your acceptance and/or use of this code 
 * constitute agreement to the terms and conditions of this notice.Microchip 
 * disclaims all liability arising from this information and its use.  It is 
 * your responsibility to ensure that your application meets with your 
 * specifications.Further, the implementation and use of the FAT file system 
 * accompanying this code, SD card specifications, MMC card specifications 
 * and other third party tools may require a license from various entities, 
 * including, but not limited to Microsoft® Corporation, SD Card Association 
 * and MMCA.  It is your responsibility to obtain more information regarding 
 * any applicable licensing obligations.
 * 
 * CODE AND INFORMATION ACCOMPANYING THIS MESSAGE IS PROVIDED “AS IS.”  
 * MICROCHIP AND ITS LICENSORS MAKES NO REPRESENTATION OR WARRANTIES OF ANY 
 * KIND WHETHER EXPRESS OR IMPLIED, WRITTEN OR ORAL, STATUTORY OR OTHERWISE, 
 * RELATED TO THE INFORMATION PROVIDED TO YOU, INCLUDING BUT NOT LIMITED TO 
 * ITS CONDITION, QUALITY, PERFORMANCE, MERCHANTABILITY, NON-INFRINGEMENT, 
 * OR FITNESS FOR PURPOSE.  MICROCHIP AND ITS LICENSORS ARE NOT LIABLE, UNDER 
 * ANY CIRCUMSTANCES, FOR SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES, 
 * FOR ANY REASON WHATSOEVER.  	 
 * ****************************************************************************
 *
 * $Id: FAT.h,v 1.10 2006/10/24 16:37:51 C12923 Exp $
 *
 * $Name:  $
 *
 *****************************************************************************/
#ifndef  _FAT_H_
#define  _FAT_H_

#include "fat.def"
#include "generic.h"
#include "FAT16\FileIO.h"
#include "FAT16\mediaSD.h"


// File I/O Error Codes
typedef enum _CETYPE
{
    CE_GOOD = 0,          // Everything is fine
    CE_ERASE_FAIL,        // Internal card erase failed
    CE_NOT_PRESENT,       // CARD not present
    CE_NOT_FORMATTED,     // The disk is of an unsupported format
    CE_BAD_PARTITION,     // The boot record is bad
    CE_INIT_ERROR,        // Initialization error has occurred
    CE_NOT_INIT,          // Card is not yet initialized because of some error
    CE_BAD_SECTOR_READ,   // A bad read occurred of a sector
    CE_WRITE_ERROR,       // Could not write to the sector
    CE_INVALID_CLUSTER,   // Invalid cluster value > maxcls
    CE_FILE_NOT_FOUND,    // Could not find the file on the card
    CE_DIR_NOT_FOUND,     // Could not find the directory
    CE_BAD_FILE,          // File is corrupted
    CE_DONE,              // No more files in this directory
    CE_FILENAME_2_LONG,   // The purposed file name is too long to use. Shorten the name and then resend.
    CE_FILENAME_EXISTS,   // This filename already exists
    CE_NO_MORE_TAILS,     // Long filename could not be created
    CE_DIR_FULL,          // All root dir entry are taken
    CE_DISK_FULL,         // All clusters in partition are taken
    CE_DIR_NOT_EMPTY,     // This directory is not empty yet, remove files b4 deleting
    CE_NONSUPPORTED_SIZE, // The disk is too big to format as FAT16
    CE_TEMP_MOUNT_FAILED, // A temporary mount failed (format)
    CE_WRITE_PROTECTED,   // Card is write protected
    CE_UNLOCK_FAILED,     // Card unlock failed
    CE_INVALIDCSUM,       // Invalid checksum
    CE_ENVLOADFAIL,       // The environment failed to load
    CE_TMPMEMFULL,        // No more external RAM
    CE_FILENOTOPENED,     // File not openned for the write
    CE_BADCACHEREAD,      // Bad cache read
    CE_CARDFAT32,         // FAT 32 - Card NOT supported
    CE_IMAGENOTAVAIL,     // The PC has tried to tell me to use a SQTP file without a part selected
    CE_READONLY,          // The File is readonly
    CE_CARDFAT12,         // FAT12 during intial testing we are not supporting FAT12
    CE_WRITEONLY          // The File is open in write only mode - a read was attempted
} CETYPE;

// BPB FAT32
typedef struct __BPB_FAT32 {
    WORD BPB_jmpBoot[3];     // Jump Command
    BYTE BPB_OEMName[8];     // OEM name
    WORD BPB_BytsPerSec;     // BYTEs per sector
    BYTE BPB_SecPerClus;     // Sectors per allocation unit
    WORD BPB_RsvdSecCnt;     // Number of reserved sectors after start
    BYTE BPB_NumFATs;        // Number of FATs
    WORD BPB_RootEntCnt;     // Number of root directory entries
    WORD BPB_TotSec16;       // Total number of sectors
    BYTE BPB_Media;          // Media descriptor
    WORD BPB_FATSz16;        // Number of sectors per FAT
    WORD BPB_SecPerTrk;      // Sectors per track
    WORD BPB_NumHeads;       // Number of heads
    DWORD BPB_HiddSec;       // Number of hidden sectors
    DWORD BPB_TotSec32;      // New 32bit total sec count
    DWORD BPB_FATSz32;       // Sectors occupied by one FAT
    WORD BPB_ExtFlags;       // Presently active FAT as defined by bits 0-3 if bit 7 is 1
    WORD BPB_FSVers;         // FAT32 filesystem version, should be 0:0
    DWORD BPB_RootClus;      // Start cluster of the root directory (should be 2)
    WORD BPB_FSInfo;         // Filesystem info structure sector
    WORD BPB_BkBootSec;      // Backup boot sector normally 6
    BYTE BPB_Reserved[12];   // Reserved
    BYTE BS_DrvNum;          // Int 13 drive number
    BYTE BS_Reserved1;       // Reserved
    BYTE BS_BootSig;         // 0x29
    BYTE BS_VolID[4];        // Volume Id
    BYTE BS_VolLab[11];      // Volume Label
    BYTE BS_FilSysType[8];   // File system type, not used for determination
} _BPB_FAT32;

typedef _BPB_FAT32 *BPB_FAT32;

// BPB FAT16
typedef struct __BPB_FAT16 {
    BYTE BS_jmpBoot[3];      // Jump Command
    BYTE BS_OEMName[8];      // OEM name
    WORD BPB_BytsPerSec;     // BYTEs per sector
    BYTE BPB_SecPerClus;     // Sectors per allocation unit
    WORD BPB_RsvdSecCnt;     // Number of reserved sectors after start
    BYTE BPB_NumFATs;        // Number of FATs
    WORD BPB_RootEntCnt;     // Number of root directory entries
    WORD BPB_TotSec16;       // Total number of sectors
    BYTE BPB_Media;          // Media descriptor
    WORD BPB_FATSz16;        // Number of sectors per FAT
    WORD BPB_SecPerTrk;      // Sectors per track
    WORD BPB_NumHeads;       // Number of heads
    DWORD BPB_HiddSec;       // Number of hidden sectors
    DWORD BPB_TotSec32;      // New 32bit total sec count
    BYTE BS_DrvNum;          // Int 13 drive number
    BYTE BS_Reserved1;       // Reserved
    BYTE BS_BootSig;         // 0x29
    BYTE BS_VolID[4];        // Volume Id
    BYTE BS_VolLab[11];      // Volume Label
    BYTE BS_FilSysType[8];   // File system type, not used for determination
} _BPB_FAT16;

typedef _BPB_FAT16 *BPB_FAT16;

// BPB FAT12
typedef struct __BPB_FAT12 {
    BYTE BS_jmpBoot[3];      // Jump Command
    BYTE BS_OEMName[8];      // OEM name
    WORD BPB_BytsPerSec;     // BYTEs per sector
    BYTE BPB_SecPerClus;     // Sectors per allocation unit
    WORD BPB_RsvdSecCnt;     // Number of reserved sectors after start
    BYTE BPB_NumFATs;        // Number of FATs
    WORD BPB_RootEntCnt;     // Number of root directory entries
    WORD BPB_TotSec16;       // Total number of sectors
    BYTE BPB_Media;          // Media descriptor
    WORD BPB_FATSz16;        // Number of sectors per FAT
    WORD BPB_SecPerTrk;      // Sectors per track
    WORD BPB_NumHeads;       // Number of heads
    DWORD BPB_HiddSec;       // Number of hidden sectors
    BYTE BS_Reserved0;       // Nothing
    BYTE BS_DrvNum;          // Int 13 drive number
    BYTE BS_Reserved1;       // Reserved
    BYTE BS_BootSig;         // 0x29
    BYTE BS_VolID[4];        // Volume Id
    BYTE BS_VolLab[11];      // Volume Label
    BYTE BS_FilSysType[8];   // File system type, not used for determination
} _BPB_FAT12;

typedef _BPB_FAT12 *BPB_FAT12;

// PTE_FAT - Partition Table Entry
#pragma pack(1)
typedef struct _PTE_FAT
{
    BYTE PTE_BootDes;        // Boot Descriptor, 0x80
    BYTE Pad1;
    WORD PTE_FrstPartSect;   // First Partion Sector
    BYTE PTE_FSDesc;         // File System Descriptor
    BYTE Pad2;
    WORD PTE_LstPartSect;    // Last Partion Sector
    DWORD PTE_FrstSect;      // First Sector Position
    DWORD PTE_NumSect;       // Number of Sectors in partion
} PTE_FAT;

// PT_FAT - Partition Table
typedef struct __PT_FAT
{
    BYTE ConsChkRtn[446];
    PTE_FAT Partition0;
    PTE_FAT Partition1;
    PTE_FAT Partition2;
    PTE_FAT Partition3;
    BYTE Signature0;     // 0x55
    BYTE Signature1;     // 0xAA
} _PT_FAT;

typedef _PT_FAT *PT_FAT;

typedef struct __BS
{
    union
    {
       _BPB_FAT32 FAT_32;
       _BPB_FAT16 FAT_16;
       _BPB_FAT12 FAT_12;
    } FAT;
    BYTE Reserved[MEDIA_SECTOR_SIZE-sizeof(_BPB_FAT32)-2];
    BYTE Signature0;     // 0x55
    BYTE Signature1;     // 0xAA
} _BS;

typedef _BS *BS;

// Master Boot Record offsets
#define FO_MBR         0L   // MBR sector LBA
#define FAT_EOF        0xfff   // Last cluster in a file or last entry in a DIR

#define FAT_GOOD_SIGN_0     0x55
#define FAT_GOOD_SIGN_1     0xAA
#define BOOTABLE            0x80
#define NON_BOOTABLE        0x00

#define BSI_BOOTJMP            0
#define BSI_OEM_NAME           3
#define BSI_BytesPerSec       11
#define BSI_SecPerClus        13
#define BSI_RsvdSecCount      14
#define BSI_NumFATs           16
#define BSI_RootEntCnt        17
#define BSI_TotSec16          19
#define BSI_Media             21
#define BSI_FATSz16           22
#define BSI_SecPerTrk         24
#define BSI_NumHeads          26
#define BSI_HiddSec           28
#define BSI_TotSec32          32
#define BSI_DrvNum            36
#define BSI_Reserved1         37
#define BSI_BootSig           38
#define BSI_VolID             39
#define BSI_VolLab            43
#define BSI_FilSysType        54

// RAM Access macros
#define RAMwrite( a, f, d ) *(a+f) = d
#define RAMread( a, f )  *(a+f)
#define RAMreadW( a, f ) *(WORD *)(a+f)
#define RAMreadD( a, f ) *(DWORD *)(a+f)

// Internal errors
#define CE_FAT_EOF  60   // FAT attempt to read beyond EOF
#define CE_EOF      61   // Reached the end of file

// Directory entry management
#define DIR_ESIZE   32      // Size of a directory entry (BYTEs)
#define DIR_NAME    0       // Offset of filename
#define DIR_EXT     8       // Offset of file extension
#define DIR_ATTRIB  11      // Offset of attribute (00ARSHDV) (BYTE)
#define DIR_TIME    22      // Offset of last use time (WORD)
#define DIR_DATE    24      // Offset of last use date (WORD)
#define DIR_CLST    26      // Offset of first cluster in FAT (WORD)
#define DIR_SIZE    28      // Offset of file size (DWORD)
#define DIR_DEL     0xE5    // Marker of a deleted entry
#define DIR_EMPTY   0       // Marker of last entry in directory

// Number of directory entries in one sector
#define DIRENTRIES_PER_SECTOR  0x10

#define LDIR_LAST  0x40   // Last entry of a long directory 0x40 | 0x00
#define LDIR_NAMESIZE  13   // 13 total UNICODE characters
#define LDIR_ORD_MASK  0x1f   // Long directory order mask

// Since we use an address generator, FILE is not actually the cast of what we pass
typedef FSFILE *FILEOBJ;

#define FS_FOUND          0   // Directory entry match
#define FS_NOT_FOUND      1   // Directory entry not found
#define FS_NO_MORE        2   // No more files found
#define FAIL             -1

#define DIR_NAMESIZE        8
#define DIR_EXTENSION       3
#define DIR_NAMECOMP        (DIR_NAMESIZE+DIR_EXTENSION)

typedef WORD UNICODE;

// DOS directory entry structure
typedef struct __DIRENTRY
{
    BYTE DIR_Name[DIR_NAMESIZE];       // Name
    BYTE DIR_Extension[DIR_EXTENSION]; // Extension
    BYTE DIR_Attr;                     // Attributes
    BYTE DIR_NTRes;                    // Reserved by NT
    BYTE DIR_CrtTimeTenth;             // Millisecond stamp
    WORD DIR_CrtTime;                  // Time created
    WORD DIR_CrtDate;                  // Date created
    WORD DIR_LstAccDate;               // Last access date
    WORD DIR_FstClusHI;                // High word of this enty's first cluster number
    WORD DIR_WrtTime;                  // Last update time
    WORD DIR_WrtDate;                  // Last update date
    WORD DIR_FstClusLO;                // Low word of this entry's first cluster number
    DWORD DIR_FileSize;                // File size in DWORD
} _DIRENTRY;

typedef _DIRENTRY *DIRENTRY;

typedef struct __LDIR
{
    BYTE LDIR_Ord;         // Order of this entry in the sequence of long dir entries
    UNICODE LDIR_Name1[5] __attribute__ ((packed)); // Characters 1-5
    BYTE LDIR_Attr;        // Attributes, must be ATTR_LONG_NAME
    BYTE LDIR_Type;        // 0, for a dir entry that is sub-component of a long name
    BYTE LDIR_Chksum;      // Checksum of name in short dir entry at end of the long dir set
    UNICODE LDIR_Name2[6]; // Characters 6-11
    WORD LDIR_FstClusLO;   // Must be 0
    UNICODE LDIR_Name3[2]; // Characters 12-13
} _LDIR;

typedef _LDIR *LDIR;

// Format 
#define defNUMOFFATS        2
#define defNUMOFROOTDIRS    512
#define defMEDIADESCRIPTOR  0xF8
#define defSECPERTRACK      0x3F
#define defNUMHEADS         0xFF
#define defBOOTSIG          0x29
#define defDRIVENUM         0x0
#define defRESERVEDSECS     0x01

#define FAT12   1   // Internal flags for FAT type 12 and 16
#define FAT16   2
#define FAT32   3

#define PT_FAT12            0x01
#define PT_FAT16            0x04   // For smaller than 32mb
#define PT_FAT16_32MB       0x06   // For greater than 32mb
#define PT_FAT16_LBA        0x0E   // For small cards that were formatted by win95 LBA
#define PT_FAT32_LBA        0x0B
#define PT_FAT32            0x0C

#define ATTR_READ_ONLY      0x01
#define ATTR_HIDDEN         0x02
#define ATTR_SYSTEM         0x04
#define ATTR_VOLUME_ID      0x08
#define ATTR_LONG_NAME      (ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)
#define ATTR_EXTEND         0x0f
#define ATTR_DIRECTORY      0x10
#define ATTR_ARCHIVE        0x20
#define ATTR_LONG_NAME_MASK (ATTR_LONG_NAME | ATTR_DIRECTORY | ATTR_ARCHIVE)

#define FILT_READ_ONLY      0x01
#define FILT_HIDDEN         0x02
#define FILT_SYSTEM         0x04
#define FILT_VOLUME         0x08
#define FILT_EXTND          (FILT_VOLUME | FILT_SYSTEM | FILT_HIDDEN | FILT_READ_ONLY)
#define FILT_DIRECTORY      0x10
#define FILT_NORMAL_FILE    0x20
#define FILT_MASK           (0x3F)

#define CLUSTER_EMPTY       0x0000
#define LAST_CLUSTER_FAT12  0xff8
#define LAST_CLUSTER_FAT16  0xfff8u
#define LAST_CLUSTER        0xfff8u
#define END_CLUSTER         0xFFFE
#define CLUSTER_FAIL        0xFFFF
#define WIN_LAST_CLUS       0xFFFF

#define FAT_ENTRY0          0xFFF8
#define FAT_ENTRY1          0x7FFF

#ifndef EOF
	#define EOF         (-1)
#endif

typedef struct
{
    BYTE *buffer;         // Address of sector buffer in ext RAM
    DWORD firsts;         // LBA of first sector in (first) partition
    DWORD fat;            // LBA of FAT
    DWORD root;           // LBA of root directory
    DWORD data;           // LBA of the data area
    WORD maxroot;         // Max number of entries in root dir
    DWORD maxcls;         // Max number of clusters in partition
    WORD fatsize;         // Number of sectors
    BYTE fatcopy;         // Number of copies
    BYTE SecPerClus;      // Number of sectors per cluster
    BYTE type;            // Type of FAT (FAT12, FAT16...)
    BYTE mount;           // Flag (TRUE=mounted, FALSE=invalid)
} DISK;

extern FSFILE *gBufferOwner;
#ifndef FAT16_DYNAMIC_MEM
    extern FSFILE gblFileArray[FAT16_MAX_FILES_OPEN];
    extern BYTE gblFileSlotOpen[FAT16_MAX_FILES_OPEN];
#endif
extern DISK gblDiskData;   // Used by FAT layer

PUBLIC BOOL FATMediaInit( void );
PUBLIC WORD FATWrite( DISK *dsk, WORD cls, WORD v );
PUBLIC WORD FATRead( DISK *dsk, WORD ccls );
PUBLIC WORD FATReadQueued( DISK *dsk, WORD ccls );
PUBLIC WORD FATFindEmptyCluster( FILEOBJ fo );
PUBLIC DWORD Cluster2Sector( DISK *dsk, WORD cluster );
PUBLIC BOOL FATEraseClusterChain( WORD cluster, DISK *dsk );
PUBLIC CETYPE EraseCluster( DISK *disk, WORD cluster );

#endif   /* _FAT_H_ */

