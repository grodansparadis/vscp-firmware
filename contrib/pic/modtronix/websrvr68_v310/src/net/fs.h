/**
 * @brief           Primary File System
 * @file            fs.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_fs
 *
 * @section description Description
 **********************************
 * This module only contains defines for functions that are mapped to the Primary File System.
 * There can only be one Primary File System. For example, to make the FSEE module the primary
 * File System, place the following line in the "projdefs.h" file:
 * @code
 * #define FSEE_IS_PRIMARY_FS
 * @endcode
 *
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
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
// FS - documentation module
/**
@defgroup mod_sys_fs File System
@ingroup mod_sys

The Modtronix TCP/IP Stack can be configured to use a couple of different storage mediums for its
file system. The chosen medium is called the "Primary File System". The Primary File System is
configured by defining one of the X_IS_PRIMARY_FS defines.

For example, to make the FSEE module (uses a
I2C EEPROM as storage medium) the primary File System, the following define must be added to the code (It
is normally added to the "projdefs.h" file):
@code
#define FSEE_IS_PRIMARY_FS
@endcode
To make the FSFRAM module (uses a SPI FRAM as storage medium) the primary File System, the following define
must be added to the code (It is normally added to the "projdefs.h" file):
@code
#define FSFRAM_IS_PRIMARY_FS
@endcode

The Modtronix TCP/IP stack has been designed so other mediums can easily be used as the file system. For
example, to use a SD Card as the file system, the "fsee.c" and "fsee.h" files can be used as a starting point,
and modified for a SD Card. Future versions of this stack might contain implementations for using Atmel DataFlash
chips and SD Cards as the primary file system.
*/

#ifndef FS_H
#define FS_H


/////////////////////////////////////////////////
//Data Type defines

/**
 * FILE handle. Each open files is assigned a file handle.
 * - Valid values are from 0 - 127
 * - FILE_INVALID indicates an invalid file
 * - FSYS_NOT_AVAILABLE indicates that the File System is not available
 */
#define FILE BYTE

/**
 * File System position pointer. A variable that can be used to give the offset anywhere in the File System.
 */
#define FSYS_POS

/**
 * FILE position pointer. A variable that can be used to give the offset anywhere in a file.
 */
#define FILE_POS


/////////////////////////////////////////////////
// FILE_STRUCT defines
/**
 * FILE structure. Each file that is opened is assigned a FILE structure by the File System.
 */
typedef struct _FILE_INFO {
    BYTE flags;         /**< Flags indicating current status of this file handle. Masked with FSEEFLAG_XX defines */

    /**
     * The current offset from the start of the file. Is incremented after each read and write.
     * This value will always point to the next byte that is to be read or written.
     */
    FILE_POS offset;

    SWORD_VAL address;  /**< The absolute address of the start of this file */

    FILE_POS length;   /**< The file length. */
} FSEE_FILE_INFO;

/**
 * FILE flag. When set, indicates that this file is being used.
 */
#define FILEFLAG_USED       0x01ul

/**
 * FILE flag. Indicates that we are currently reading from this file. For I2C devices, this means that
 * the memory chip is in sequencial read mode and has control of the I2C bus! Before the bus can be used
 * by any other node, it has to be released!
 */
#define FILEFLAG_READING    0x02ul

/**
 * FILE flag. Indicates that we are currently writing to this file. For I2C devices, this means that
 * the memory chip is in sequencial write mode and has control of the I2C bus! Before the bus can be used
 * by any other node, it has to be released!
 */
#define FILEFLAG_WRITING    0x04ul

/**
 * FILE flag. When set, indicates that an error has occured with this file.
 */
#define FILEFLAG_ERROR      0x20ul

/**
 * FILE flag. When set, indicates that then end of file has been reached.
 */
#define FILEFLAG_EOF        0x40ul

/**
 * FILE flag. Reserve (don't use) signed bit, is implemented differently by different compilers
 */
#define FILEFLAG_RES        0x80ul


/////////////////////////////////////////////////
// FILE Handle defines
#define FILE_INVALID        /**< When FILE Handle has this value, it indicates an invalid file */
#define FSYS_NOT_AVAILABLE  /**< When FILE Handle has this value, it indicates that the File System is not available */

/////////////////////////////////////////////////
//File System Flags
/**
 * File System flag. When set, indicates that the File System is available.
 */
#define FSYSFLAG_AVAILABLE       0x01ul


/**
 * Initializes the Modtronix File System
 *
 * @return      TRUE, if File System Storage access is initialized and File System is is ready to be used <br>
 *              FALSE otherwise
 */
BOOL fsysInit(void);


/**
 * Opens the given file for reading or writing, and returns a handle to the file.
 * The file pointer (where next read or write will occur) will be positioned at the beginning of
 * the file. To modify the file pointer use the fileSetPos() function.
 * There is no need to call fileRelease() after calling this function, it does not reserve any resources!
 *
 * @preCondition    fsysInit() was successfully called.
 *
 * @param name      NULL terminate file name.
 * @param mode      Currently not used. All files are opened with read and write permission. When writing
 *                  to a file, no data can be appended to it!
 *
 * @return          - A FILE Handle (value 0 - 127) if the file is found
 *                  - FILE_INVALID if file could not be opened
 *                  - FSYS_NOT_AVAILABLE if the File System is not available
 */
FILE fileOpen(BYTE* name, BYTE mode);


/**
 * Closes the given file. Seeing that the File System can only have a limited amount of files open
 * at any time (defined by FSEE_MAX_FILES), it is very important to call this function after finished
 * with a file!
 *
 * @param fhandle   FILE handle of the file to be released
 */
void fileClose(FILE fhandle);


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
 * @param fhandle   FILE handle of the file to be released
 */
void fileRelease(FILE fhandle);


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
 * @preCondition    fsysInit() was successfully called.
 *
 * @preCondition    fileOpen() != FILE_INVALID and<br>
 *                  fileGetByteBegin() == TRUE
 *
 * @param fhandle   FILE handle of the file to be released
 *
 * @return          Data byte from current address.
 */
BYTE fileGetByte(FILE fhandle);


/**
 * Tests if the given file has reached it's EOF. This will happen:
 * - After the last byte has been read from an open file.
 * - After the last byte has been written to an open file.
 *
 * @return          TRUE if given file has reached end of file. <br>
 *                  FALSE if otherwise.
 */
#define fileIsEOF(fhandle)


/**
 * Tests if the last operation on the given file generated an error.
 *
 * @return          TRUE if last operation on file generated and error. <br>
 *                  FALSE if otherwise.
 */
#define fileHasError(fhandle)


/**
 * Tests if the last operation on the given file completed without an EOF or Error. If this function
 * returns false, use the fileIsEOF() and fileIsOK() functions to determine exact condition.
 *
 * @return          TRUE if last operation on file was successfull <br>
 *                  FALSE if the last operation on the file generated and EOF or Error
 */
#define fileIsOK(fhandle)

/**
 * Tests if the given FILE handle is a valid handle. A valid FILE handle is a value that
 * could be assigned to an open file. Possible invalid FILE handles are FILE_INVALID and FSYS_NOT_AVAILABLE
 *
 * @return          TRUE if given FILE handle value could be a valid FILE handle<br>
 *                  FALSE if it is not a possible FILE handle value
 */
#define fileIsValidHandle(fhandle)


/**
 * Deletes all files present on the File System
 *
 * @preCondition    fsysInit() was successfully called.
 *
 * @return          TRUE if successful <br>
 *                  FALSE otherwise
 */
BOOL fsysFormat(void);


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
 * @param fhandle   FILE handle to the file to be activated
 *
 * @param b         byte to be written
 *
 *
 * @return          TRUE if successful <br>
 *                  FALSE if otherwise
 */
BOOL filePutByte(FILE fhandle, BYTE b);


/**
 * Finishes writing any data that has not yet been written to the File System. When writing data to a
 * file via the filePutByte() function, it is not always written straight to the File System Media, but
 * some times to an intermediate buffer. This function will write all pending data from the buffer to the
 * File System Media.
 */
void fileFlush(FILE fhandle);


/**
 * Set the current file pointer for the given file. This is the offset in the given file that the
 * next read or write will be performed on.
 */
void fileSetPos(FILE fhandle, FILE_POS put);

/**
 * Get the current file pointer for the given file. This is the offset in the given file that the
 * next read or write will be performed on. This value can be used as a parameter to the fileSetPos()
 * function at a later stage to restore the current file position. This is NOT the file address in
 * the file system.
 */
FILE_POS fileGetPos(FILE fhandle);


/**
 * Prepares the File System to receive a new Image via following calls to filePutByte()!
 * The given image must contain the FAT and all file data.
 *
 * !!! IMPORTANT !!!
 * This function will overwrite the entire File System! All data will be lost!
 *
 * @return           TRUE if successful <br>
 *                  FALSE if otherwise
 */
FILE fileOpenImage();


/**
 * Gets the address in the File System of the requested file's FAT entry. This address can be used as a fast
 * way to open files in the future with the fileOpenFAT() function.
 *
 * !!! IMPORTANT !!!
 * The File System FAT entry address obtained with the fileGetFAT() function will only be valid as long as no
 * modifications are made to the File System! If after obtaining a address with the fileGetFAT() function the
 * File System is modified, this value might not be valid any more!
 */
FSEE_POS fileGetFAT(BYTE* name);


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
 * @return          - A FILE Handle (value 0 - 127) if the file is found
 *                  - FILE_INVALID if file could not be opened
 *                  - FSYS_NOT_AVAILABLE if the File System is not available
 */
FILE fileOpenFAT(FSYS_POS fatPos);


/**
 * Indicates if the File System is currently in use.
 * 
 * @return      TRUE if it is currently being used
 *              FALSE if not
 */
#define fsysIsInUse()

#endif
