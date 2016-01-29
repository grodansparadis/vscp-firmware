/**
 * @brief           FSFRAM Module for Modtronix TCP/IP Stack
 * @file            fsfram.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    stacktsk.h
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_fsfram
 *
 * @section description Description
 **********************************
 * This module contains the code for integrating a FRAM into the Modtronix File System.
 * <br>For a detailed description, see the @ref mod_sys_fsfram section of this document - in 
 * [Modules] [System Modules].
 *
 * @section fsfram_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required defines should be uncommended, and the rest commented out.
 @code
//*********************************************************************
//--------------------  File System Configuration --------------------
//*********************************************************************
//Defines the maximum size of a file used in the file system.
//When FSFRAM_FILE_SIZE_16MB is defined, the file system can handle files with a size of up to 16 Mbytes.
//When not defined, the maximum size of a file is 64 Kbyte.
//When defined, the FSFRAM16M File System will be created.
//when NOT defined, the FSFRAM File System will be created.
#define FSFRAM_FILE_SIZE_16MB

//Specifies the maximum number of files that can be open at any one time. When defined as 1, the code
//will be much faster and smaller. This value should not be much less then the the number of HTTP
//Connections, seeing that each HTTP connection can have a open file. If most web page files are
//small (below 2 kbytes) then this is not so important.
#define FSFRAM_MAX_FILES 1

//When this define is present, the FSFRAM File System is used as the primary file system. All functions
//Will be remapped to general names, for example fsframOpen() will be mapped to fileOpen. This makes switching
//between different File System much simpler.
//#define FSFRAM_IS_PRIMARY_FS

 @endcode
 *********************************************************************/

/*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on code written
 * by Ziggurat29. For details, see www.ziggurat29.com
 * The code may be modified and can be used free of charge for non commercial and
 * commercial applications. All rights are reserved. 
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2005-11-16, David G Lemley:
 *    - Initial version (ver 1.0.0)
 * 2007-04-23, David Hosken (DH):
 *    - Integrated into Modtronix TCP/IP stack
********************************************************************/

/////////////////////////////////////////////////
// FSFRAM - documentation module
/**
@defgroup mod_sys_fsfram File System FSFRAM
@ingroup mod_sys

@section mod_sys_mxfs_format Format
The Modtronix HTTP Server can use a simple file system (the Modtronix FRAM File System, or "FSFRAM") to store
Web pages in the FRAM. This documentation is for version 1 of the FSFRAM.
The FSFRAM image is stored in an external serial FRAM.
FSFRAM follows a special format to store multiple files in the given storage media, which is summarized in
Figure 1.

NOTE:  This file system is essential the same as the FSEE file system, with
changes in implementation having been made to accomodate FRAM IO.

<br>
<table class=compactTable cellpadding=2 cellspacing=0 border=1>
<tr><td>Reserved Block&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td></tr>
<tr><td>FSYS Header</td></tr>
<tr><td>FAT Entry 1</td></tr>
<tr><td>.............</td></tr>
<tr><td>FAT Entry n</td></tr>
<tr><td>File 1</td></tr>
<tr><td>.............</td></tr>
<tr><td>File n</td></tr>
</table>
<i>Figure 1</i> - File System Image Format

The length of "Reserved Block" is defined by FSFRAM_RESERVE_BLOCK. The reserved block can be used by
the main application to store simple configuration values. FSFRAM storage begins with the "FSYS Header",
which contains information about the File System type, length and other info.
The format of the "FSYS Header" is shown in Figure 2.

<br>
<table class=compactTable cellpadding=2 cellspacing=0 border=1><tr>
<td>FSYS Header Length <i>(8 bits)</i></td>
<td>File System Identifier <i>(8 bits)</i></td>
<td>File System Version <i>(8 bits)</i></td>
<td>FAT Entries <i>(16 bits)</i></td>
</tr></table>
<i>Figure 2</i> - FSFRAM "FSYS Header" Format
c
- The "File System Identifier" of the FSFRAM (Modtronix EEPROM File System) with a maximum File System size
  of 16 Mbytes and file size of 64 Kbytes is 0x01. This is the default type for the FSFRAM File System.

- The "File System Identifier" of the FSFRAM (Modtronix EEPROM File System) with a maximum File System size
  of 16 Mbytes and file size of 16 Mbytes is 0x02. This is also know as the FSFRAM16M File System.

Following the "FSYS Header" is one or more "FAT Entries" (File Allocation Table), followed by the file
data of each file. The "FAT entries" are always in alphabetic order, and all file names are in capitals.
The "FAT entry" describes the file name, location and it's attributes.
The format of the "FAT Entry" is shown in Figure 3.

<br>
<table class=compactTable cellpadding=2 cellspacing=0 border=1><tr>
<td>Attributes <i>(8 bits)</i></td>
<td>File Name <i>(8-byte + 3-byte format)</i></td>
<td>Address <i>(24 bits)</i></td>
<td>File Length <i>(16 or 24 bits)</i></td>
</tr></table>
<i>Figure 3</i> - FSFRAM FAT Entry Format

The "Attributes" indicates whether the file is a normal file or a read only file.

Each FAT entry contains a 24-bit address value. This implies a maximum FSFRAM image size
of 16 Mbytes.

FSFRAM uses "short" file names of the "8 + 3" format (8 bytes for the actual file name and 3 bytes
for the extension, or NNNNNNNN.EEE). The 24-bit address gives the start of the first file data
block. All file names are stored in upper case to make file name comparisons easier. All FAT entries
are in alphabetical order to make finding a file faster.

The address in each FAT entry points in turn to a data block that contains the actual file data.
The data block format is shown in Figure 3. 

<br><br>
<table class=compactTable cellpadding=2 cellspacing=0 border=1><tr>
<td>Data <i>(variable length)</i></td>
</tr></table>
<i>Figure 3</i> - FSFRAM Data Block Format

*/

#ifndef FSFRAM_H
#define FSFRAM_H


/////////////////////////////////////////////////
//Do some check to see if all defines have been declared
#if !defined(FSFRAM_MAX_FILES)
#define FSFRAM_MAX_FILES 1
#endif


/////////////////////////////////////////////////
//If this is the primary File System, redefine all functions
#if defined(FSFRAM_IS_PRIMARY_FS)
#define fsysInit            fsframInit
#define fileOpen            fsframOpen
#define fileClose           fsframClose
#define fileRelease         fsframRelease
#define fileGetByte         fsframGetByte
#define fileIsEOF           fsframIsEOF
#define fileHasError        fsframHasError
#define fileIsOK            fsframIsOK
#define fileIsValidHandle   fsframIsValidHandle
#define fsysFormat          fsframFormat
#define filePutByte         fsframPutByte
#define fileFlush           fsframFlush
#define fileSetPos          fsframSetPos
#define fileGetPos          fsframGetPos
#define fileGetFAT          fsframGetFAT
#define fileOpenFAT         fsframOpenFAT
#define fsysIsInUse         fsframIsInUse
#define fsysOpenImage       fsframOpenImage
#define fsysPutByteImage    fsframPutByteImage
#define fsysCloseImage      fsframCloseImage

#define FILE                FSFRAM_FILE
#define FSYS_POS            FSFRAM_POS
#define FILE_POS            FSFRAM_FILE_POS
#define FILE_INVALID        FSFRAM_FILE_INVALID
#define FSYS_NOT_AVAILABLE  FSFRAM_NOT_AVAILABLE

#endif


/////////////////////////////////////////////////
//Data Type defines

/**
 * FILE handle. Each open files is assigned a file handle.
 * - Valid values are from 0 - 127
 * - FSFRAM_FILE_INVALID indicates an invalid file
 * - FSFRAM_NOT_AVAILABLE indicates that the File System is not available
 */
#define FSFRAM_FILE BYTE

/**
 * File System position pointer. A variable that can be used to give the offset anywhere in the File System.
 */
#define FSFRAM_POS SWORD      /**< File address in the File System */

/**
 * FILE position pointer. A variable that can be used to give the offset anywhere in a file.
 */
#ifdef FSFRAM_FILE_SIZE_16MB
#define FSFRAM_FILE_POS SWORD      /**< File length */
#else
#define FSFRAM_FILE_POS WORD       /**< File length */
#endif



/////////////////////////////////////////////////
// FILE_STRUCT defines
/**
 * FILE structure. Each file that is opened is assigned a FILE structure by the File System.
 */
typedef struct _FSFRAM_FILE_INFO {
    BYTE flags;         /**< Flags indicating current status of this file handle. Masked with FSFRAMFLAG_XX defines */

    /**
     * The current offset from the start of the file. Is incremented after each read and write.
     * This value will always point to the next byte that is to be read or written.
     */
    FSFRAM_FILE_POS offset;

    SWORD_VAL address;  /**< The absolute address of the start of this file */

    #if defined(FSFRAM_FILE_SIZE_16MB)
    SWORD_VAL length;   /**< The file length. */
    SWORD_VAL		nMaxLen;    //no bigger than this
    #else
    WORD_VAL length;    /**< The file length. */
    WORD_VAL		nMaxLen;    //no bigger than this
    #endif
} FSFRAM_FILE_INFO;

/**
 * FILE flag. When set, indicates that this file is being used.
 */
#define FSFRAMFILE_USED       0x01ul

/**
 * FILE flag. Indicates that we are currently reading from this file. For SPI devices, this means that
 * the memory chip is in sequencial read mode and has control of the SPI bus! Before the bus can be used
 * by any other node, it has to be released!
 */
#define FSFRAMFILE_READING    0x02ul

/**
 * FILE flag. Indicates that we are currently writing to this file. For SPI devices, this means that
 * the memory chip is in sequencial write mode and has control of the SPI bus! Before the bus can be used
 * by any other node, it has to be released!
 */
#define FSFRAMFILE_WRITING    0x04ul

/**
 * FILE flag. When set, indicates that an error has occured with this file.
 */
#define FSFRAMFILE_ERROR      0x20ul

/**
 * FILE flag. When set, indicates that then end of file has been reached.
 */
#define FSFRAMFILE_EOF        0x40ul

/**
 * FILE flag. Reserve (don't use) signed bit, is implemented differently by different compilers
 */
#define FSFRAMFILE_RES        0x80ul


/////////////////////////////////////////////////
// FILE Handle defines
#define FSFRAM_FILE_INVALID       0xfful  /**< When FILE Handle has this value, it indicates an invalid file */
#define FSFRAM_NOT_AVAILABLE      0xfeul  /**< When FILE Handle has this value, it indicates that the File System is not available */



/////////////////////////////////////////////////
//File System Flags
/**
 * File System flag. When set, indicates that the File System is available.
 */
#define FSFRAMFLAG_AVAILABLE          0x01ul

/**
 * File System flag. When set, indicates that the File System is currently busy with a read or write
 * operation. This File System can only perform a single read or write operation at any time. This flag
 * is set as soon as a open file is read from or written to. It is cleared as soon as the open file is
 * released or closed.
 */
#define FSFRAMFLAG_READING_WRITING    0x02ul


/////////////////////////////////////////////////
//Global variables
#if !defined(THIS_IS_FSFRAM)
extern BYTE fsframOpenCount;
extern BYTE fsframFlags;
extern FSFRAM_FILE_INFO fsframFCB[FSFRAM_MAX_FILES];
#endif


/**
 * Initializes the Modtronix File System
 *
 * @return      TRUE, if File System Storage access is initialized and File System is is ready to be used <br>
 *              FALSE otherwise
 */
BOOL fsframInit(void);


/**
 * Opens the given file for reading or writing, and returns a handle to the file.
 * The file pointer (where next read or write will occur) will be positioned at the beginning of
 * the file. To modify the file pointer use the fileSetPos() function.
 *
 * @param name      NULL terminate file name.
 * @param mode      Currently not used. All files are opened with read and write permission. When writing
 *                  to a file, no data can be appended to it!
 *
 * @return          - A FILE Handle (value 0 - 127) if the file is found
 *                  - FSFRAM_FILE_INVALID if file could not be opened
 *                  - FSFRAM_NOT_AVAILABLE if the File System is not available
 */
FSFRAM_FILE fsframOpen(BYTE* name, BYTE mode);


/**
 * Closes the given file. Seeing that the File System can only have a limited amount of files open
 * at any time (defined by FSFRAM_MAX_FILES), it is very important to call this function after finished
 * with a file!
 *
 * @param fhandle   FSFRAM_FILE handle of the file to be released
 */
void fsframClose(FSFRAM_FILE fhandle);


/**
 * Releases any resources that the given open file might be reserving.
 * On certian File Systems, like ones that use FRAMs on a shared SPI bus for example, the File
 * System will take control of the bus once a file is opened. To release the bus so it can be
 * used by other modules, the fileRelease() function has to be called. At a later stage, when the
 * file has to be used again, the fileActive() function has to be called.
 *
 * This function should be called when a file has been opened, and we don't want to close it now,
 * but still want to use it at a later stage. In this case, we can call fileRelease() and suspend
 * operation to the system to perform other tasks. When at a later stage we want to use this file
 * again, fileActivate() will automatically be called by the file read and write functions.
 *
 * @preCondition    fileOpen() was successfully called.
 *
 * @param fhandle   FSFRAM_FILE handle of the file to be released
 */
void fsframRelease(FSFRAM_FILE fhandle);


/**
 * Reads the next byte from current open file.
 *
 * Caller must call fileIsEOF() to check for end of file condition before calling this function
 * to make sure the file has not reached it's end. If the fileIsEOF() returns true, then this
 * function will have no affect!
 *
 * Caller must call fileHasError() function after calling this function to ensure byte was read
 * without error!
 *
 * This function will place the FRAM in sequencial read mode and take control of the SPI bus!
 * To allow other devices to use the SPI bus while the file is open, call fileRelease() when finished
 * reading some data. When calling fileRead() after calling fileRelease(), the EEPROM will automatically
 * be placed in sequencial read mode again and take control of the bus.
 *
 * To read multiple bytes, see ex_file_read.c example file.
 *
 * @preCondition    fileOpen() != FSFRAM_FILE_INVALID and<br>
 *                  fileGetByteBegin() == TRUE
 *
 * @param fhandle   FSFRAM_FILE handle of the file to be released
 *
 * @return          Data byte from current address.
 */
BYTE fsframGetByte(FSFRAM_FILE fhandle);


/**
 * Tests if the given file has reached it's EOF. This will happen:
 * - After the last byte has been read from an open file.
 * - After the last byte has been written to an open file.
 *
 * @return          TRUE if given file has reached end of file. <br>
 *                  FALSE if otherwise.
 */
#if (FSFRAM_MAX_FILES == 1)
    #define fsframIsEOF(fhandle)     (fsframFCB[0].flags & FSFRAMFILE_EOF)
#else
    #define fsframIsEOF(fhandle)     (fsframFCB[fhandle].flags & FSFRAMFILE_EOF)
#endif

/**
 * Tests if the last operation on the given file generated an error.
 *
 * @return          TRUE if last operation on file generated and error. <br>
 *                  FALSE if otherwise.
 */
#if (FSFRAM_MAX_FILES == 1)
    #define fsframHasError(fhandle)     (fsframFCB[0].flags & FSFRAMFILE_ERROR)
#else
    #define fsframHasError(fhandle)     (fsframFCB[fhandle].flags & FSFRAMFILE_ERROR)
#endif

/**
 * Tests if the last operation on the given file completed without an EOF or Error. If this function
 * returns false, use the fileIsEOF() and fileIsOK() functions to determine exact condition.
 *
 * @return          TRUE if last operation on file was successfull <br>
 *                  FALSE if the last operation on the file generated and EOF or Error
 */
#if (FSFRAM_MAX_FILES == 1)
    #define fsframIsOK(fhandle)     (fsframFCB[0].flags & (FSFRAMFILE_ERROR | FSFRAMFILE_EOF))
#else
    #define fsframIsOK(fhandle)     (fsframFCB[fhandle].flags & (FSFRAMFILE_ERROR | FSFRAMFILE_EOF))
#endif


/**
 * Tests if the given FILE handle is a valid handle. A valid FILE handle is a value that
 * could be assigned to an open file. Possible invalid FILE handles are FILE_INVALID and FSYS_NOT_AVAILABLE
 *
 * @return          TRUE if given FILE handle value could be a valid FILE handle<br>
 *                  FALSE if it is not a possible FILE handle value
 */
#define fsframIsValidHandle(fhandle) (fhandle < FSFRAM_MAX_FILES)

/**
 * Deletes all files present on the File System
 *
 * @return          TRUE if successful <br>
 *                  FALSE otherwise
 */
BOOL fsframFormat(void);


/**
 * Writes a byte to the given file.
 *
 * Caller must call fileIsEOF() to check for end of file condition before calling this function to
 * make sure the file has not reached it's end. If the fileIsEOF() returns true, then this function
 * will have no affect!
 *
 * Writes a byte to the current output. Actual write may not get started until internal
 * write page is full. To ensure that previously data gets written, caller must
 * call fileFlush() after last call to filePutByte().
 *
 * @preCondition    fileOpen() was successfully called.
 *
 * @param fhandle   FSFRAM_FILE handle to the file to be activated
 *
 * @param b         byte to be written
 *
 *
 * @return          TRUE if successful <br>
 *                  FALSE if otherwise
 */
BOOL fsframPutByte(FSFRAM_FILE fhandle, BYTE b);


/**
 * Finishes writing any data that has not yet been written to the File System. When writing data to a
 * file via the filePutByte() function, it is not always written straight to the File System Media, but
 * some times to an intermediate buffer. This function will write all pending data from the buffer to the
 * File System Media.
 */
void fsframFlush(FSFRAM_FILE fhandle);


/**
 * Set the current file pointer for the given file. This is the offset in the given file that the
 * next read or write will be performed on.
 */
void fsframSetPos(FSFRAM_FILE fhandle, FSFRAM_FILE_POS put);

/**
 * Get the current file pointer for the given file. This is the offset in the given file that the
 * next read or write will be performed on. This value can be used as a parameter to the fileSetPos()
 * function at a later stage to restore the current file position. This is NOT the file address in
 * the file system.
 */
FSFRAM_FILE_POS fsframGetPos(FSFRAM_FILE fhandle);


/**
 * Gets the address in the File System of the requested file's FAT entry. This address can be used as a fast
 * way to open files in the future with the fileOpenFAT() function.
 *
 * !!! IMPORTANT !!!
 * The File System FAT entry address obtained with the fileGetFAT() function will only be valid as long as no
 * modifications are made to the File System! If after obtaining a address with the fileGetFAT() function the
 * File System is modified, this value might not be valid any more!
 */
FSFRAM_POS fsframGetFAT(BYTE* name);


/**
 * Opens the given file for reading or writing, and returns a handle to the file.
 * The file pointer (where next read or write will occur) will be positioned at the beginning of
 * the file. To modify the file pointer use the fileSetPos() function.
 *
 * !!! IMPORTANT !!!
 * The File System FAT entry address obtained with the fileGetFAT() function will only be valid as long as no
 * modifications are made to the File System! If after obtaining a address with the fileGetFAT() function the
 * File System is modified, this value might not be valid any more!
 *
 * @param fatPos    The requested File's FAT address in the File System. This value has to be obtained
 *                  from a fileGetFAT() function.
 *
 * @return          - A FSFRAM_FILE Handle (value 0 - 127) if the file is found
 *                  - FSFRAM_FILE_INVALID if file could not be opened
 *                  - FSFRAM_NOT_AVAILABLE if the File System is not available
 */
FSFRAM_FILE fsframOpenFAT(FSFRAM_POS fatPos);


/**
 * Indicates if the File System is currently in use.
 * 
 * @return      TRUE if it is currently being used
 *              FALSE if not
 */
#define fsframIsInUse()       ((fsframFlags & FSFRAMFLAG_AVAILABLE) == 0)


/**
 * Prepares the File System to receive a new Image via following calls to fsframPutByteImage()
 *
 * !!! IMPORTANT !!!
 * This function will overwrite the entire File System! All data will be lost!
 *
 * @preCondition No files are allowed to be open! If there are any open files when calling this function,
 * it will return FALSE!
 *
 * @return          TRUE if successful <br>
 *                  FALSE otherwise
 */
BOOL fsframOpenImage(void);

/**
 * Writes a byte to the File System Image. The File System Image contains the following "FSYS Header",
 * "FAT Entries" and "File Data".
 *
 * !!! IMPORTANT !!!
 * This function will overwrite the entire File System! All data will be lost!
 *
 * @preCondition fsframOpenImage() must have been called.
 *
 * @return          TRUE if successful <br>
 *                  FALSE if otherwise
 */
BOOL fsframPutByteImage(BYTE b);


/**
 * Finishes writing the File System Image
 *
 * @return          TRUE if successful <br>
 *                  FALSE otherwise
 */
BOOL fsframCloseImage(void);

#endif
