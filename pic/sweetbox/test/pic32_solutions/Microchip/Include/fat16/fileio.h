/******************************************************************************
 *
 *                  FILE I/O OPERATION API HEADER FILE
 *
 ******************************************************************************
 * FileName:        FileIO.h
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
 *
 * ****************************************************************************
 *
 * $Id: FileIO.h,v 1.1 2006/08/15 23:27:21 C12923 Exp $
 *
 * $Name:  $
 *
 *****************************************************************************/
#ifndef _FILEIO_H_
#define _FILEIO_H_

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "peripheral\ports.h"

#define SEEK_SET  0
#define SEEK_CUR  1
#define SEEK_END  2

#define APPEND  "a"
#define WRITE   "w"
#define READ    "r"

typedef struct
{
    unsigned write :1;         // Set if the file was opened in a write mode
    unsigned FileWriteEOF :1;  // Set if we are writing and have reached the EOF
} FileFlags;

#ifdef MS_FILENAME_SPECIFICATION
    #define FILE_NAME_SIZE  255   // This is not modifiable option for FAT16.
    #define FILE_NAME_SPACE_SIZE  (FILE_NAME_SIZE+5)   // MS-Windows allocates 260 bytes

#else
    #ifdef CUSTOM_FILE_NAME_SIZE   
       #define FILE_NAME_SIZE  CUSTOM_FILE_NAME_SIZE_VAL
       #define FILE_NAME_SPACE_SIZE  (FILE_NAME_SIZE+5)

    #else
       #define FILE_NAME_SIZE  25
       #define FILE_NAME_SPACE_SIZE  (FILE_NAME_SIZE+5)
   
    #endif

#endif


typedef struct
{
    DWORD       seek;                           // Position in the file
    DWORD       size;                           // File size
    void        *dsk;                           // Disk structure
    FileFlags   Flags;                          // Flags
    WORD        cluster;                        // First cluster
    WORD        ccls;                           // Current cluster in file
    WORD        sec;                            // Sector in current cluster
    WORD        pos;                            // Position in current sector
    WORD        time;                           // Last update time
    WORD        date;                           // Last update date
    WORD        entry;                          // Entry position in current directory
    WORD        chk;                            // FILE structure checksum = ~(entry + name[0])
    WORD        attributes;                     // The bare bones attributes
    WORD        dirclus;                        // Base cluster of directory
    WORD        dirccls;                        // Current cluster
    char        name[FILE_NAME_SPACE_SIZE];     // Need to make it WORD wide for external mem
} FSFILE;

#define MEDIA_SECTOR_SIZE          512

// FAT16 Filesystem Initialization Function
BOOL FSInit( UINT pb_clk );

// File Operation Functions
FSFILE *FSfopen( const char *fileName, const char *mode );
int FSfclose( FSFILE *stream );
int FSremove( const char *fileName );

// Direct I/O Functions
size_t FSfread( void *ptr, size_t size, size_t n, FSFILE *stream );
size_t FSfwrite( const void *ptr, size_t size, size_t n, FSFILE *stream );

// File Positioning Functions
int FSfseek( FSFILE *stream, long offset, int origin );
long FSftell( FSFILE *stream  );
void FSrewind( FSFILE *stream );

// Directory Support Functions
FSFILE *CreateDirectory( const char *dirName );
BOOL ChangeDirectory( FSFILE *stream, const char *dirName );
BOOL GetDirectoryName( FSFILE *stream, char *dirName );
BOOL EraseDirectory( FSFILE *stream, const char *dirName );

// Error Functions
int FSfeof( FSFILE *stream );
PUBLIC FSFILE *GetRootDirectory(const char *root_name);
BOOL GetFileName(FSFILE *file, WORD *handle, BOOL *dir);
BOOL GetFileNameIdx(FSFILE *file, WORD idx, BOOL *dir);

#ifdef _USE_STANDARD_LIB_API

#ifdef FILE
#undef FILE
#endif

typedef FSFILE  FILE;

extern inline FSFILE * __attribute__ ((always_inline)) fopen(const char *fileName, const char *mode)
{
    return FSfopen(fileName, mode);
}

extern inline int __attribute__ ((always_inline)) fclose( FSFILE *stream )
{
    return FSfclose(stream);
}

extern inline int __attribute__ ((always_inline)) remove( const char *fileName )
{
    return FSremove(fileName);
}

extern inline size_t __attribute__ ((always_inline)) fread( void *ptr, size_t size, size_t n, FSFILE *stream )
{
    return FSfread(ptr, size, n, stream);
}

extern inline size_t __attribute__ ((always_inline)) fwrite( const void *ptr, size_t size, size_t n, FSFILE *stream )
{
    return FSfwrite(ptr, size, n, stream);
}

extern inline int __attribute__ ((always_inline)) fseek( FSFILE *stream, long offset, int origin )
{
    return FSfseek(stream, offset, origin);
}

extern inline long __attribute__ ((always_inline)) ftell( FSFILE *stream  )
{
    return FSftell(stream);
}

extern inline void __attribute__ ((always_inline)) rewind( FSFILE *stream )
{
    FSrewind(stream);
}

extern inline int __attribute__ ((always_inline)) feof( FSFILE *stream )
{
    return FSfeof(stream);
}
#endif

#endif   /* _FILEIO_H_ */
