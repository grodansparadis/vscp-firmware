/*--------------------------------------------------------------------------/
/  FatFs - FAT file system module include file  R0.02a       (C)ChaN, 2006
/---------------------------------------------------------------------------/
/ FatFs module is an experimenal project to implement FAT file system to
/ cheap microcontrollers. This is opened for education, reserch and
/ development. You can use it for non-profit or profit use without any
/ limitation under your responsibility.
/---------------------------------------------------------------------------*/

#ifndef _FATFS

#define _BYTE_ACC
/* This enables byte-by-byte access for multi-byte variables. It must be defined
on the big-endian processor, or to prevent address error. */

//#define _FS_READONLY
/* Read only configuration. This removes writing code for read-only applications. */

//#define _FS_MINIMUM
/* Minimum configuration. This removes some functions to reduce module size. */

#define	_USE_SJIS
/* This enables Shift-JIS code transparency, or only US-ASCII file name can be accepted. */


//#include "integer.h"
//#include "typedefs.h"
//#include <global.h>
//#include <sd/mmc/includes.h>

/* Result type for fatfs application interface */
typedef unsigned char	FRESULT;


/* File system object structure */
typedef struct _FATFS {
	BYTE	fs_type;		/* FAT type */
	BYTE	files;			/* Number of files currently opend */
	BYTE	sects_clust;            /* Sectors per cluster */
	BYTE	n_fats;			/* Number of FAT copies */
	WORD	n_rootdir;		/* Number of root directory entry */
	BYTE	winflag;		/* win[] dirty flag (1:must be written back) */
	BYTE	pad1;
	DWORD	sects_fat;		/* Sectors per fat */
	DWORD	max_clust;		/* Maximum cluster# + 1 */
	DWORD	fatbase;		/* FAT start sector */
	DWORD	dirbase;		/* Root directory start sector (cluster# for FAT32) */
	DWORD	database;		/* Data start sector */
	DWORD	winsect;		/* Current sector appearing in the win[] */
	BYTE	win[512];		/* Disk access window for Directory/FAT area */
} FATFS;


/* Directory object structure */
typedef struct _DIR {
	DWORD	sclust;		/* Start cluster */
	DWORD	clust;		/* Current cluster */
	DWORD	sect;		/* Current sector */
	WORD	index;		/* Current index */
} DIR;


/* File object structure */
typedef struct _FIL {
	DWORD	fptr;			/* File R/W pointer */
	DWORD	fsize;			/* File size */
	DWORD	org_clust;		/* File start cluster */
	DWORD	curr_clust;		/* Current cluster */
	DWORD	curr_sect;		/* Current sector */
#ifndef _FS_READONLY
	DWORD	dir_sect;		/* Sector containing the directory entry */
	BYTE*	dir_ptr;		/* Ponter to the directory entry in the window */
#endif
	BYTE	flag;			/* File status flags */
	BYTE	sect_clust;		/* Left sectors in cluster */
	BYTE	buffer[512];            /* File R/W buffer */
} FIL;


/* File status structure */
typedef struct _FILINFO {
	DWORD fsize;			/* Size */
	WORD fdate;			/* Date */
	WORD ftime;			/* Time */
	BYTE fattrib;			/* Attribute */
	char fname[8+1+3+1];            /* Name (8.3 format) */
} FILINFO;



/*-----------------------------------------------------*/
/* FatFs module application interface                  */

extern FATFS *FatFs;	/* Pointer to active file system object */

FRESULT f_open (FIL*, const char*, BYTE);			/* Open or create a file */
FRESULT f_read (FIL*, void*, WORD, WORD*);			/* Read file */
FRESULT f_close (FIL*);						/* Close file */
FRESULT f_lseek (FIL*, DWORD);					/* Seek file pointer */
FRESULT f_opendir (DIR*, const char*);				/* Initialize to read a directory */
FRESULT f_readdir (DIR*, FILINFO*);				/* Read a directory item */
FRESULT f_stat (const char*, FILINFO*);				/* Get file status */
FRESULT f_getfree (DWORD*);					/* Get number of free clusters */
FRESULT f_mountdrv ();						/* Force initialized the file system */
#ifndef _FS_READONLY
FRESULT f_write (FIL*, const void*, WORD, WORD*);               /* Write file */
FRESULT f_sync (FIL*);						/* Flush cached information of a writing file */
FRESULT f_unlink (const char*);					/* Delete a file or directory */
FRESULT	f_mkdir (const char*);					/* Create a directory */
FRESULT f_chmod (const char*, BYTE, BYTE);			/* Change file attriburte */
#endif


/* User defined function to give a current time to fatfs module */

#ifndef _FS_READONLY
DWORD get_fattime();	/* 31-25: Year(0-127 +1980), 24-21: Month(1-12), 20-16: Day(1-31) */
                        /* 15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */
#endif

/* File function return code */

#define	FR_OK					0
#define	FR_NOT_READY				1
#define	FR_NO_FILE				2
#define	FR_NO_PATH				3
#define	FR_INVALID_NAME				4
#define	FR_DENIED				5
#define	FR_DISK_FULL				6
#define	FR_RW_ERROR				7
#define	FR_INCORRECT_DISK_CHANGE        	9
#define	FR_WRITE_PROTECTED			10
#define	FR_NOT_ENABLED				11
#define	FR_NO_FILESYSTEM			12


/* File access control and file status flags */

#define	FA_READ			0x01
#define	FA_OPEN_EXISTING	0x00
#ifndef _FS_READONLY
#define	FA_WRITE		0x02
#define	FA_CREATE_ALWAYS	0x08
#define	FA_OPEN_ALWAYS		0x10
#define FA__WRITTEN		0x20
#define FA__DIRTY		0x40
#endif
#define FA__ERROR		0x80


/* FAT type signature (fs_type) */

#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3


/* File attribute mask for directory entry */

#define	AM_RDO	0x01	/* Read Only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define	AM_VOL	0x08	/* Volume Label */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */



/* Multi-byte word access macros  */

#ifdef _BYTE_ACC
#define	LD_WORD(ptr)		(((WORD)*(BYTE*)(ptr+1)<<8)|*(ptr))
#define	LD_DWORD(ptr)		(((DWORD)*(BYTE*)(ptr+3)<<24)|((DWORD)*(BYTE*)(ptr+2)<<16)|((WORD)*(BYTE*)(ptr+1)<<8)|*(BYTE*)(ptr))
#define	ST_WORD(ptr,val)	*(BYTE*)(ptr)=val; *(BYTE*)(ptr+1)=val>>8
#define	ST_DWORD(ptr,val)	*(BYTE*)(ptr)=val; *(BYTE*)(ptr+1)=val>>8; *(BYTE*)(ptr+2)=val>>16; *(BYTE*)(ptr+3)=val>>24
#else
#define	LD_WORD(ptr)		(*(WORD*)(BYTE*)(ptr))
#define	LD_DWORD(ptr)		(*(DWORD*)(BYTE*)(ptr))
#define	ST_WORD(ptr,val)	*(WORD*)(BYTE*)(ptr)=(val)
#define	ST_DWORD(ptr,val)	*(DWORD*)(BYTE*)(ptr)=(val)
#endif


#define _FATFS

static bool remove_chain (DWORD clust);
static bool move_window (DWORD sector);
static bool put_cluster (DWORD clust,DWORD val);
static bool next_dir_entry (DIR *scan);
#endif
