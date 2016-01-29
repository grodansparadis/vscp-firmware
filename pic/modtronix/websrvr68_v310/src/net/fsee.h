/**
 * @brief           FSEE Module for Modtronix TCP/IP Stack
 * @file            fsee.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    stacktsk.h
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_fsee
 *
 * @section description Description
 **********************************
 * This module contains the Modtronix File System code.
 * <br>For a detailed description, see the @ref mod_sys_fsee section of this document - in 
 * [Modules] [System Modules].
 *
 * @section fsee_conf Configuration
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
//When FSEE_FILE_SIZE_16MB is defined, the file system can handle files with a size of up to 16 Mbytes.
//When not defined, the maximum size of a file is 64 Kbyte.
//When defined, the FSEE16M File System will be created.
//when NOT defined, the FSEE File System will be created.
#define FSEE_FILE_SIZE_16MB

//Specifies the maximum number of files that can be open at any one time. When defined as 1, the code
//will be much faster and smaller. This value should not be much less then the the number of HTTP
//Connections, seeing that each HTTP connection can have a open file. If most web page files are
//small (below 2 kbytes) then this is not so important.
#define FSEE_MAX_FILES 1ul

//When this define is present, the FSEE File System is used as the primary file system. All functions
//Will be remapped to general names, for example fseeOpen() will be mapped to fileOpen. This makes switching
//between different File System much simpler.
#define FSEE_IS_PRIMARY_FS

 @endcode
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on v2.20.04.01
 * of the Microchip TCP/IP stack. The original code base is owned by Microchip
 * Technology Incorporated (the Company), and is protected under applicable
 * copyright laws. The modifications are owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller based products manufactured
 * by Modtronix Engineering. The code may be modified and can be used free of charge
 * for non commercial and commercial applications. All rights are reserved. Any use
 * in violation of the foregoing restrictions may subject the user to criminal
 * sanctions under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 ********************************************************************
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * David Hosken         05/11/16     Original (Rev. 1.0)
********************************************************************/


/////////////////////////////////////////////////
// FSEE - documentation module
/**
@defgroup mod_sys_fsee File System FSEE
@ingroup mod_sys

@section mod_sys_mxfs_format Format
The Modtronix HTTP Server uses a simple file system (the Modtronix EEPROM File System, or "FSEE") to store
Web pages in the EEPROM. This documentation if for version 1 of the FSEE.
The FSEE image is stored in an external serial EEPROM.
FSEE follows a special format to store multiple files in the given storage media, which is summarized in
Figure 1.

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

The length of "Reserved Block" is defined by FSEE_RESERVE_BLOCK. The reserved block can be used by
the main application to store simple configuration values. FSEE storage begins with the "FSYS Header",
which contains information about the File System type, length and other info.
The format of the "FSYS Header" is shown in Figure 2.

<br>
<table class=compactTable cellpadding=2 cellspacing=0 border=1><tr>
<td>FSYS Header Length <i>(8 bits)</i></td>
<td>File System Identifier <i>(8 bits)</i></td>
<td>File System Version <i>(8 bits)</i></td>
<td>FAT Entries <i>(16 bits)</i></td>
</tr></table>
<i>Figure 2</i> - FSEE "FSYS Header" Format

- The "File System Identifier" of the FSEE (Modtronix EEPROM File System) with a maximum File System size
  of 16 Mbytes and file size of 64 Kbytes is 0x01. This is the default type for the FSEE File System.

- The "File System Identifier" of the FSEE (Modtronix EEPROM File System) with a maximum File System size
  of 16 Mbytes and file size of 16 Mbytes is 0x02. This is also know as the FSEE16M File System.

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
<i>Figure 3</i> - FSEE FAT Entry Format

The "Attributes" indicates whether the file is a normal file or a read only file.

Each FAT entry contains a 24-bit address value. This implies a maximum FSEE image size
of 16 Mbytes.

FSEE uses "short" file names of the "8 + 3" format (8 bytes for the actual file name and 3 bytes
for the extension, or NNNNNNNN.EEE). The 24-bit address gives the start of the first file data
block. All file names are stored in upper case to make file name comparisons easier. All FAT entries
are in alphabetical order to make finding a file faster.

The address in each FAT entry points in turn to a data block that contains the actual file data.
The data block format is shown in Figure 3. 

<br><br>
<table class=compactTable cellpadding=2 cellspacing=0 border=1><tr>
<td>Data <i>(variable length)</i></td>
</tr></table>
<i>Figure 3</i> - FSEE Data Block Format

*/

#ifndef FSEE_H
#define FSEE_H


/////////////////////////////////////////////////
//Do some check to see if all defines have been declared
#if !defined(FSEE_MAX_FILES)
#define FSEE_MAX_FILES 1ul
#endif


/////////////////////////////////////////////////
//If this is the primary File System, redefine all functions
#if defined(FSEE_IS_PRIMARY_FS)
#define fsysInit            fseeInit
#define fileOpen            fseeOpen
#define fileClose           fseeClose
#define fileRelease         fseeRelease
#define fileGetByte         fseeGetByte
#define fileIsEOF           fseeIsEOF
#define fileHasError        fseeHasError
#define fileIsOK            fseeIsOK
#define fileIsValidHandle   fseeIsValidHandle
#define fsysFormat          fseeFormat
#define filePutByte         fseePutByte
#define fileFlush           fseeFlush
#define fileSetPos          fseeSetPos
#define fileGetPos          fseeGetPos
#define fileGetFAT          fseeGetFAT
#define fileOpenFAT         fseeOpenFAT
#define fsysIsInUse         fseeIsInUse
#define fsysOpenImage       fseeOpenImage
#define fsysPutByteImage    fseePutByteImage
#define fsysCloseImage      fseeCloseImage

#define FILE                FSEE_FILE
#define FSYS_POS            FSEE_POS
#define FILE_POS            FSEE_FILE_POS
#define FILE_INVALID        FSEE_FILE_INVALID
#define FSYS_NOT_AVAILABLE  FSEE_NOT_AVAILABLE

#endif


/////////////////////////////////////////////////
//Data Type defines

/**
 * FILE handle. Each open files is assigned a file handle.
 * - Valid values are from 0 - 127
 * - FSEE_FILE_INVALID indicates an invalid file
 * - FSEE_NOT_AVAILABLE indicates that the File System is not available
 */
#define FSEE_FILE BYTE

/**
 * File System position pointer. A variable that can be used to give the offset anywhere in the File System.
 */
#define FSEE_POS SWORD      /**< File address in the File System */

/**
 * FILE position pointer. A variable that can be used to give the offset anywhere in a file.
 */
#ifdef FSEE_FILE_SIZE_16MB
#define FSEE_FILE_POS SWORD      /**< File length */
#else
#define FSEE_FILE_POS WORD       /**< File length */
#endif



/////////////////////////////////////////////////
// FILE_STRUCT defines
/**
 * FILE structure. Each file that is opened is assigned a FILE structure by the File System.
 */
typedef struct _FSEE_FILE_INFO {
    BYTE flags;         /**< Flags indicating current status of this file handle. Masked with FSEEFLAG_XX defines */

    /**
     * The current offset from the start of the file. Is incremented after each read and write.
     * This value will always point to the next byte that is to be read or written.
     */
    FSEE_FILE_POS offset;

    SWORD_VAL address;  /**< The absolute address of the start of this file */

    #if defined(FSEE_FILE_SIZE_16MB)
    SWORD_VAL length;   /**< The file length. */
    #else
    WORD_VAL length;    /**< The file length. */
    #endif
} FSEE_FILE_INFO;

/**
 * FILE flag. When set, indicates that this file is being used.
 */
#define FSEEFILE_USED       0x01ul

/**
 * FILE flag. Indicates that we are currently reading from this file. For I2C devices, this means that
 * the memory chip is in sequencial read mode and has control of the I2C bus! Before the bus can be used
 * by any other node, it has to be released!
 */
#define FSEEFILE_READING    0x02ul

/**
 * FILE flag. Indicates that we are currently writing to this file. For I2C devices, this means that
 * the memory chip is in sequencial write mode and has control of the I2C bus! Before the bus can be used
 * by any other node, it has to be released!
 */
#define FSEEFILE_WRITING    0x04ul

/**
 * FILE flag. When set, indicates that an error has occured with this file.
 */
#define FSEEFILE_ERROR      0x20ul

/**
 * FILE flag. When set, indicates that then end of file has been reached.
 */
#define FSEEFILE_EOF        0x40ul

/**
 * FILE flag. Reserve (don't use) signed bit, is implemented differently by different compilers
 */
#define FSEEFILE_RES        0x80ul


/////////////////////////////////////////////////
// FILE Handle defines
#define FSEE_FILE_INVALID       0xfful  /**< When FILE Handle has this value, it indicates an invalid file */
#define FSEE_NOT_AVAILABLE      0xfeul  /**< When FILE Handle has this value, it indicates that the File System is not available */

#define FSEE_WRITE_PAGE_SIZE    (64ul)


/////////////////////////////////////////////////
//File System Flags
/**
 * File System flag. When set, indicates that the File System is available.
 */
#define FSEEFLAG_AVAILABLE          0x01ul

/**
 * File System flag. When set, indicates that the File System is currently busy with a read or write
 * operation. This File System can only perform a single read or write operation at any time. This flag
 * is set as soon as a open file is read from or written to. It is cleared as soon as the open file is
 * released or closed.
 */
#define FSEEFLAG_READING_WRITING    0x02ul


/////////////////////////////////////////////////
//Global variables
#if !defined(THIS_IS_FSEE)
extern BYTE fseeOpenCount;
extern BYTE fseeFlags;
extern FSEE_FILE_INFO FCB[FSEE_MAX_FILES];
#endif


/**
 * Initializes the Modtronix File System
 *
 * @return      TRUE, if File System Storage access is initialized and File System is is ready to be used <br>
 *              FALSE otherwise
 */
BOOL fseeInit(void);


/**
 * Opens the given file for reading or writing, and returns a handle to the file.
 * The file pointer (where next read or write will occur) will be positioned at the beginning of
 * the file. To modify the file pointer use the fileSetPos() function.
 * There is no need to call fseeRelease() after calling this function, it does not reserve any resources!
 *
 * @preCondition    fsysInit() was successfully called.
 *
 * @param name      NULL terminate file name.
 * @param mode      Currently not used. All files are opened with read and write permission. When writing
 *                  to a file, no data can be appended to it!
 *
 * @return          - A FILE Handle (value 0 - 127) if the file is found
 *                  - FSEE_FILE_INVALID if file could not be opened
 *                  - FSEE_NOT_AVAILABLE if the File System is not available
 */
FSEE_FILE fseeOpen(BYTE* name, BYTE mode);


/**
 * Closes the given file. Seeing that the File System can only have a limited amount of files open
 * at any time (defined by FSEE_MAX_FILES), it is very important to call this function after finished
 * with a file!
 *
 * @param fhandle   FSEE_FILE handle of the file to be released
 */
void fseeClose(FSEE_FILE fhandle);


/**
 * Releases any resources that the given open file might be reserving.
 * On certian File Systems, like ones that use EEPROMs on a shared I2C bus for example, the File
 * System will take control of the bus once a file is opened. To release the bus so it can be
 * used by other modules, the fileRelease() function has to be called. At a later stage, when the
 * file has to be used again, the fileActive() function has to be called.
 *
 * This function should be called when a file has been opened, and we don't want to close it now,
 * but still want to use it at a later stage. In this case, we can call fileRelease() and suspend
 * operation to the system to perform other tasks. When at a later stage we want to use this file
 * again, fileActivate() will automatically be called by the file read and write functions.
 *
 * @preCondition    fsysInit() and fileOpen() were successfully called.
 *
 * @param fhandle   FSEE_FILE handle of the file to be released
 */
void fseeRelease(FSEE_FILE fhandle);


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
 * This function will place the EEPROM in sequencial read mode and take control of the I2C bus!
 * To allow other devices to use the I2C bus while the file is open, call fileRelease() when finished
 * reading some data. When calling fileRead() after calling fileRelease(), the EEPROM will automatically
 * be placed in sequencial read mode again and take control of the bus.
 *
 * To read multiple bytes, see ex_file_read.c example file.
 *
 * @preCondition    fsysInit() was successfully called<br>
 *                  fileOpen() != FSEE_FILE_INVALID and<br>
 *                  fileGetByteBegin() == TRUE
 *
 * @param fhandle   FSEE_FILE handle of the file to be released
 *
 * @return          Data byte from current address.
 */
BYTE fseeGetByte(FSEE_FILE fhandle);


/**
 * Tests if the given file has reached it's EOF. This will happen:
 * - After the last byte has been read from an open file.
 * - After the last byte has been written to an open file.
 *
 * @return          TRUE if given file has reached end of file. <br>
 *                  FALSE if otherwise.
 */
#if (FSEE_MAX_FILES == 1)
    #define fseeIsEOF(fhandle)     (FCB[0].flags & FSEEFILE_EOF)
#else
    #define fseeIsEOF(fhandle)     (FCB[fhandle].flags & FSEEFILE_EOF)
#endif

/**
 * Tests if the last operation on the given file generated an error.
 *
 * @return          TRUE if last operation on file generated and error. <br>
 *                  FALSE if otherwise.
 */
#if (FSEE_MAX_FILES == 1)
    #define fseeHasError(fhandle)     (FCB[0].flags & FSEEFILE_ERROR)
#else
    #define fseeHasError(fhandle)     (FCB[fhandle].flags & FSEEFILE_ERROR)
#endif

/**
 * Tests if the last operation on the given file completed without an EOF or Error. If this function
 * returns false, use the fileIsEOF() and fileIsOK() functions to determine exact condition.
 *
 * @return          TRUE if last operation on file was successfull <br>
 *                  FALSE if the last operation on the file generated and EOF or Error
 */
#if (FSEE_MAX_FILES == 1)
    #define fseeIsOK(fhandle)     (FCB[0].flags & (FSEEFILE_ERROR | FSEEFILE_EOF))
#else
    #define fseeIsOK(fhandle)     (FCB[fhandle].flags & (FSEEFILE_ERROR | FSEEFILE_EOF))
#endif


/**
 * Tests if the given FILE handle is a valid handle. A valid FILE handle is a value that
 * could be assigned to an open file. Possible invalid FILE handles are FILE_INVALID and FSYS_NOT_AVAILABLE
 *
 * @return          TRUE if given FILE handle value could be a valid FILE handle<br>
 *                  FALSE if it is not a possible FILE handle value
 */
#define fseeIsValidHandle(fhandle) (fhandle < FSEE_MAX_FILES)

/**
 * Deletes all files present on the File System
 *
 * @preCondition    fsysInit() was successfully called.
 *
 * @return          TRUE if successful <br>
 *                  FALSE otherwise
 */
BOOL fseeFormat(void);


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
 * @preCondition    fsysInit() and fileOpen() were successfully called.
 *
 * @param fhandle   FSEE_FILE handle to the file to be activated
 *
 * @param b         byte to be written
 *
 *
 * @return          TRUE if successful <br>
 *                  FALSE if otherwise
 */
BOOL fseePutByte(FSEE_FILE fhandle, BYTE b);


/**
 * Finishes writing any data that has not yet been written to the File System. When writing data to a
 * file via the filePutByte() function, it is not always written straight to the File System Media, but
 * some times to an intermediate buffer. This function will write all pending data from the buffer to the
 * File System Media.
 */
void fseeFlush(FSEE_FILE fhandle);


/**
 * Set the current file pointer for the given file. This is the offset in the given file that the
 * next read or write will be performed on.
 */
void fseeSetPos(FSEE_FILE fhandle, FSEE_FILE_POS put);

/**
 * Get the current file pointer for the given file. This is the offset in the given file that the
 * next read or write will be performed on. This value can be used as a parameter to the fileSetPos()
 * function at a later stage to restore the current file position. This is NOT the file address in
 * the file system.
 */
FSEE_FILE_POS fseeGetPos(FSEE_FILE fhandle);


/**
 * Gets the address in the File System of the requested file's FAT entry. This address can be used as a fast
 * way to open files in the future with the fileOpenFAT() function.
 *
 * !!! IMPORTANT !!!
 * The File System FAT entry address obtained with the fileGetFAT() function will only be valid as long as no
 * modifications are made to the File System! If after obtaining a address with the fileGetFAT() function the
 * File System is modified, this value might not be valid any more!
 */
FSEE_POS fseeGetFAT(BYTE* name);


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
 * @return          - A FSEE_FILE Handle (value 0 - 127) if the file is found
 *                  - FSEE_FILE_INVALID if file could not be opened
 *                  - FSEE_NOT_AVAILABLE if the File System is not available
 */
FSEE_FILE fseeOpenFAT(FSEE_POS fatPos);


/**
 * Indicates if the File System is currently in use.
 * 
 * @return      TRUE if it is currently being used
 *              FALSE if not
 */
#define fseeIsInUse()       ((fseeFlags & FSEEFLAG_AVAILABLE) == 0)


/**
 * Prepares the File System to receive a new Image via following calls to fseePutByteImage()
 *
 * !!! IMPORTANT !!!
 * This function will overwrite the entire File System! All data will be lost!
 *
 * @preCondition No files are allowed to be open! If there are any open files when calling this function,
 * it will return FALSE! fsysInit() must have been successfully called.
 *
 * @return          TRUE if successful <br>
 *                  FALSE otherwise
 */
BOOL fseeOpenImage(void);

/**
 * Writes a byte to the File System Image. The File System Image contains the following "FSYS Header",
 * "FAT Entries" and "File Data".
 *
 * !!! IMPORTANT !!!
 * This function will overwrite the entire File System! All data will be lost!
 *
 * @preCondition fseeOpenImage() must have been called.
 *
 * @return          TRUE if successful <br>
 *                  FALSE if otherwise
 */
BOOL fseePutByteImage(BYTE b);


/**
 * Finishes writing the File System Image
 *
 * @return          TRUE if successful <br>
 *                  FALSE otherwise
 */
BOOL fseeCloseImage(void);

#endif
