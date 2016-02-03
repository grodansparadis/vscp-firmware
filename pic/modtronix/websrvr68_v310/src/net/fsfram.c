/*********************************************************************
 *
 *               Modtronix File System Implementation for FRAMs
 *
 *********************************************************************
 * FileName:        fsFRAM.c
 * Dependencies:    FRAM.H
 *                  fsfram.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.35PL3 or higher
 * Company:         Microchip Technology, Inc.
 *
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
 *
 *
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 2006-07-15, David Lemley
 *      - original version, derived from fsee.h

//derived from work bearing this history
 * 2006-01-14, David Hosken (DH):
 *      - made getFCB() a local function in stead of a global function
 *      - speed optimized
 * 2005-11-15, David Hosken (DH):
 *      - Original version
********************************************************************/
#define THIS_IS_FSFRAM

#include <string.h>
#include <stdlib.h>

#include "projdefs.h"
#include "net\checkcfg.h" 
#include "fsfram.h"
#include "fram.h"
#include "debug.h"

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xD3, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xD3, msgCode, msgStr)

/////////////////////////////////////////////////
// Global variables
FSFRAM_FILE_INFO fsframFCB[FSFRAM_MAX_FILES];

BYTE fsframOpenCount;     /**< Current number of open files */
BYTE fsframFlags;         /**< Current number of open files */


/*
 * This file system supports short file names i.e. 8 + 3.
 */
#define MAX_FILE_NAME_LEN   (12ul)


/** FSFRAM FAT Entry */
typedef struct _FSFRAM_ENTRY
{
    BYTE attr;              /**< Contains file attributes */

    BYTE name[MAX_FILE_NAME_LEN];   /**< File name, has the format 8 + '.' + 3 */

    SWORD_VAL address;  /**< Address in File System of this file. This address has already taken the reserve block into account */

    #if defined(FSFRAM_FILE_SIZE_16MB)
    SWORD_VAL length;   /**< File length. LSB will be first in memory */
    #else
    WORD_VAL length;    /**< File length. LSB will be first in memory */
    #endif
} FSFRAM_ENTRY;


/** FSFRAM FSYS Header */
typedef struct _FSFRAM_HEADER
{
    BYTE length;        /**< File System Header length */

    BYTE id;            /**< File System identifier */
    
    BYTE version;       /**< File System version */

    /**
     * Number of FAT entries that follow the "FSYS Header" = number of files - 1. The
     * last FAT entry does not represent a file, and has the FATFLAG_END flag set.
     */
    WORD fatEntries;
} FSFRAM_HEADER;


/**
 * FILE Attribute. When set, indicates that this is the last FAT entry
 */
#define FATFLAG_END         0x80ul

/**
 * FILE Attribute. When set, indicates that this file is read only
 */
#define FATFLAG_READ_ONLY   0x01ul


/**
 * Get a reference to the framFCB for the given FSFRAM_FILE handle
 *
 * @param fhandle FSFRAM_FILE handle of desired framFCB
 *
 * @return The requested framFCB
 */
#if (FSFRAM_MAX_FILES == 1)
    #define getFCB(fhandle)     fsframFCB[0]
#else
    #define getFCB(fhandle)     fsframFCB[fhandle]
#endif


/**
 * Initializes the Modtronix File System
 *
 * @return      TRUE, if File System Storage access is initialized and File System is is ready to be used <br>
 *              FALSE otherwise
 */
BOOL fsframInit(void) {
    #if (FSFRAM_MAX_FILES > 1)
    BYTE i;
    #endif
    fsframOpenCount = 0;
    fsframFlags = FSFRAMFLAG_AVAILABLE;

    //Initialize the FRAM access routines.
    FRAMInit();
    
    //Set all FILE object as free
    #if (FSFRAM_MAX_FILES == 1)
        getFCB(0).flags = 0;
    #else
        for (i = 0; i < FSFRAM_MAX_FILES; i++) {
            getFCB(i).flags = 0;
        }
    #endif
    
    //Read File System header, and varify that it is the same type as this File System
    // TODO
    

    return TRUE;
}


/**
 * Opens the given file for reading or writing, and returns a handle to the file.
 * The file pointer (where next read or write will occur) will be positioned at the beginning of
 * the file. To modify the file pointer use the fileSetPos() function.
 *
 * @param filename  NULL terminate file name.
 * @param mode      Currently not used. All files are opened with read and write permission. When writing
 *                  to a file, no data can be appended to it!
 *
 * @return          - A FSFRAM_FILE Handle (value 0 - 127) if the file is found
 *                  - FSFRAM_FILE_INVALID if file could not be opened
 *                  - FSFRAM_NOT_AVAILABLE if the File System is not available
 */
FSFRAM_FILE fsframOpen(BYTE* filename, BYTE mode) {
    BYTE c;
    BYTE i;
    
    FSFRAM_FILE fhandle = FSFRAM_NOT_AVAILABLE;

    FSFRAM_POS fatAdr;    //Address of current FAT entry
    
    FSFRAM_FILE_INFO * pFileInfo;
    
    #if (DEBUG_FSFRAM >= LOG_DEBUG)
    debugPutMsg(1); //fileOpen called for %s
    debugPutString(filename);
    #endif

    //Check if File System is available
    if (IS_BIT_CLEAR(fsframFlags, FSFRAMFLAG_AVAILABLE)) {
        #if (DEBUG_FSFRAM >= LOG_ERROR)
        debugPutMsg(2);    //File System not available for fseeOpen() function
        #endif
    
        return FSFRAM_NOT_AVAILABLE;
    }
        
    //Check if there are any FSFRAM_FILE handles available
    #if (FSFRAM_MAX_FILES == 1)
        if (getFCB(0).flags & FSFRAMFILE_USED) {
            #if (DEBUG_FSFRAM >= LOG_ERROR)
            debugPutMsg(3); //No File handles available for fseeOpen() function
            #endif

            return FSFRAM_NOT_AVAILABLE;
        }
        fhandle = 0;
    #else
        for (i = 0; i < FSFRAM_MAX_FILES; i++) {
            if ( !(getFCB(i).flags & FSFRAMFILE_USED)) {
                fhandle = i;    //Found available handle, use it!
                break;
            }
        }
        
        //All handles are used, return FSFRAM_NOT_AVAILABLE
        if (fhandle == FSFRAM_NOT_AVAILABLE) {
            #if (DEBUG_FSFRAM >= LOG_ERROR)
            debugPutMsg(3); //No File handles available for fseeOpen() function
            #endif

            return FSFRAM_NOT_AVAILABLE;
        }
    #endif

    //Set to first FAT entry, follows after "Reserve Block" and "File System Header"
    fatAdr = FSFRAM_RESERVE_BLOCK + sizeof(FSFRAM_HEADER);

    //If string is empty, do not attempt to find it in FAT.
    if ( *filename == '\0' )
        return FSFRAM_FILE_INVALID;

    filename = (BYTE*)strupr((char*)filename);

    while(1)
    {
        //Prepare for reading FAT
    	FRAMBeginRead ( fatAdr );
            
        //Read first byte from FAT = Attribute
        c = FRAMRead();
        #if (DEBUG_FSFRAM >= LOG_DEBUG)
        debugPutMsg(4);     //Read FAT Attributes during fseeOpen = %x
        debugPutByteHex(c);
        #endif
        
        //End of FAT reached, file was not found
        if (c & FATFLAG_END) {
            fhandle = FSFRAM_FILE_INVALID;
            break;
        }

        //Read Filename from FAT, and compare to given filename
        for (i = 0; i < MAX_FILE_NAME_LEN; i++) {
            c = FRAMRead();

            //No match, increment to next FAT entry
            if (c != filename[i]) {
                fatAdr += sizeof(FSFRAM_ENTRY);
                break;
            }
            
            //Match!!
            if (c == '\0') {
                //Increment past filename.
                while (++i < MAX_FILE_NAME_LEN) FRAMRead();
                break;
            }
        }
        
        //Match was found!
        if (i == MAX_FILE_NAME_LEN) {
            #if (DEBUG_FSFRAM >= LOG_INFO)
            debugPutMsg(5);     //fseeOpen() Found file, assigned to handle %d
            debugPutByte(fhandle);
            #endif
        
            pFileInfo = &getFCB(fhandle);
            
            pFileInfo->flags = FSFRAMFILE_USED;  //Indicate this FSFRAM_FILE handle is used
            pFileInfo->offset = 0;             //After opening file, offset is 0
            
            //Read address from FAT, is always 3 bytes long!
            pFileInfo->address.v[0] = FRAMRead();
            pFileInfo->address.v[1] = FRAMRead();
            pFileInfo->address.v[2] = FRAMRead();

            //Read length from FAT, is 2 or 3 bytes long
            pFileInfo->length.v[0] = FRAMRead();
            pFileInfo->length.v[1] = FRAMRead();
            #if defined(FSFRAM_FILE_SIZE_16MB)
            pFileInfo->length.v[2] = FRAMRead();
            #endif

            fsframOpenCount++;
            break;
        }

        //End current sequencial read. At top of this loop a new sequencial read FRAMBeginRead() will be initiated
        FRAMEndRead();
    }
    
    FRAMEndRead();

//XXX whiz through FAT finding end address
pFileInfo->nMaxLen = pFileInfo->length;    //for now, it's just what we started with

    return fhandle;
}


/**
 * Closes the given file. Seeing that the File System can only have a limited amount of files open
 * at any time (defined by FSFRAM_MAX_FILES), it is very important to call this function after finished
 * with a file!
 *
 * @param fhandle   FSFRAM_FILE handle of the file to be released
 */
void fsframClose(FSFRAM_FILE fhandle) {
    #if (DEBUG_FSFRAM >= LOG_ERROR)
    if (fhandle >= FSFRAM_MAX_FILES) {
        debugPutMsg(13);     //fseeClose() called with invalid FILE handle %d
        debugPutByte(fhandle);
        return;
    }
    #endif

    #if (DEBUG_FSFRAM >= LOG_INFO)
    debugPutMsg(6);     //fseeClose() called for FILE handle %d
    debugPutByte(fhandle);
    #endif

    // Release any resources held by this file
    fsframRelease(fhandle);

    getFCB(fhandle).flags = 0;      //Indicate this FSFRAM_FILE handle is free to be used
}


/**
 * Releases any resources that the given open file might be reserving. Ends any sequencial
 * page write cycle that might be in progress, and waits for internal write operation to complete.
 * On certian File Systems, like ones that use FRAMs on a shared SPI bus for example, the File
 * System will take control of the bus once a file is opened and a read or write operation is
 * performed (it goes into sequencial read or write mode). To release the bus so it can be
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
void fsframRelease(FSFRAM_FILE fhandle) {
    FSFRAM_FILE_INFO * pFileInfo;

    #if (DEBUG_FSFRAM >= LOG_ERROR)
    if (fhandle >= FSFRAM_MAX_FILES) {
        debugPutMsg(14);     //fseeRelease() called with invalid FILE handle %d
        debugPutByte(fhandle);
        return;
    }
    #endif

    pFileInfo = &getFCB(fhandle);
    
    //If currently reading (FRAM Sequencial read), send SPI stop condition and terminate sequencial read
    if (pFileInfo->flags & FSFRAMFILE_READING) {
        FRAMEndRead();
    }
    //If currently writing (FRAM Sequencial write), send SPI stop condition and terminate sequencial write
    else if (pFileInfo->flags & FSFRAMFILE_WRITING) {
        FRAMEndWrite();                      //End current sequencial write
    }
    
    //Clear READING and WRITING flags
    pFileInfo->flags &= ~(FSFRAMFILE_WRITING | FSFRAMFILE_READING);
    
    //Indicate that the File System is currently NOT Writing or Reading from any open file
    //Only needed when more then one FILE. When only one file, use FSFRAMFILE_READING and FSFRAMFILE_WRITING of framFCB[0].flags
    #if (FSFRAM_MAX_FILES > 1)
    fsframFlags &= (~FSFRAMFLAG_READING_WRITING);
    #endif
    
    #if (DEBUG_FSFRAM >= LOG_INFO)
    debugPutMsg(7);     //fseeRelease() called for FILE handle %d
    debugPutByte(fhandle);
    #endif
}


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
 * reading some data. When calling fileRead() after calling fileRelease(), the FRAM will automatically
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
BYTE fsframGetByte(FSFRAM_FILE fhandle) {
    BYTE c;

    FSFRAM_FILE_INFO * pFileInfo;

    #if (DEBUG_FSFRAM >= LOG_ERROR)
    if (fhandle >= FSFRAM_MAX_FILES) {
        debugPutMsg(15);     //fseeGetByte() called with invalid FILE handle %d
        debugPutByte(fhandle);
        return 0;
    }
    #endif

    pFileInfo = &getFCB(fhandle);

    //Clear all error flags
    pFileInfo->flags &= ~(FSFRAMFILE_EOF | FSFRAMFILE_ERROR);
    
    //If FSFRAM_FILE handle is not in use, return with error
    if ( (pFileInfo->flags & FSFRAMFILE_USED) == 0) {
        pFileInfo->flags |= FSFRAMFILE_ERROR;    //Set error condition
        #if (DEBUG_FSFRAM >= LOG_ERROR)
        debugPutMsg(8);     //FILE handle (%d) passed to fseeGetByte() is not used
        debugPutByte(fhandle);
        #endif

        return 0;
    }

    //If currently NOT reading (FRAM Sequencial read), initiate sequencial read sequence
    if ( (pFileInfo->flags & FSFRAMFILE_READING) == 0) {
        //Check if the File System is not currently busy reading or writing from another open file
        //Only needed when more then one FILE. When only one file, use FSFRAMFILE_READING and FSFRAMFILE_WRITING of framFCB[0].flags
        #if (FSFRAM_MAX_FILES > 1)
        if (fsframFlags & FSFRAMFLAG_READING_WRITING) {
            pFileInfo->flags |= FSFRAMFILE_ERROR;    //Set error condition
            #if (DEBUG_FSFRAM >= LOG_ERROR)
            debugPutMsg(12);     //Currently busy reading or writing from another open file, can not open file
            #endif
            return 0;
        }
        #endif
        
        //Successfully placed FRAM into sequencial read mode
    	FRAMBeginRead ( (FRAM_ADDR)(pFileInfo->address.Val + pFileInfo->offset) );
    	pFileInfo->flags |= FSFRAMFILE_READING;  //Indicate that this FILE is currently in Reading mode
        
        //Indicate that the File System is currently Reading or Writing an open file
        //Only needed when more then one FILE. When only one file, use FSFRAMFILE_READING and FSFRAMFILE_WRITING of framFCB[0].flags
        #if (FSFRAM_MAX_FILES > 1)
    	fsframFlags |= FSFRAMFLAG_READING_WRITING;
        #endif
    }

    //Read next byte
    c = FRAMRead();
    
    //Increment offset and check for end of file
    if (++(pFileInfo->offset) == pFileInfo->length.Val) {
        pFileInfo->flags |= FSFRAMFILE_EOF;
    }
    
    /*
    if ((pFileInfo->flags & 0xf0) != 0) {
        debugPutGenMsg(2);     //@mxd:2:%s
        debugPutRomStringXNull("fsframGetByte flags = 0x");
        debugPutByteHex(pFileInfo->flags);
        debugPutByte(0);    //Null terminate string
    }
    */

//HHH must _always_ relinquish SPI in the interest of safety/general utility
//(the 8019 shares the IO port).  This is hardly optimal, but the FRAM is
//greatly faster than the EE anyway.
    FRAMEndRead();
	pFileInfo->flags &= ~FSFRAMFILE_READING;  //not reading any more
    #if (FSFRAM_MAX_FILES > 1)
    fsframFlags &= (~FSFRAMFLAG_READING_WRITING);
    #endif

    return c;
}

/**
 * Deletes all files present on the File System
 *
 * @return          TRUE if successful <br>
 *                  FALSE otherwise
 */
BOOL fsframFormat(void) {
    
    return TRUE;
}


/**
 * Writes a byte to the given file.
 */
BOOL fsframPutByte ( FSFRAM_FILE fhandle, BYTE b )
{
FSFRAM_FILE_INFO * pFileInfo;

#if (DEBUG_FSFRAM >= LOG_ERROR)
if (fhandle >= FSFRAM_MAX_FILES)
    {
	debugPutMsg(15);     //fseeGetByte() called with invalid FILE handle %d
	debugPutByte(fhandle);
	return 0;
    }
#endif

pFileInfo = &getFCB(fhandle);

//Clear all error flags
pFileInfo->flags &= ~(FSFRAMFILE_EOF | FSFRAMFILE_ERROR);

//If FSFRAM_FILE handle is not in use, or position is at end, return with error
if ( (pFileInfo->flags & FSFRAMFILE_USED) == 0 ||
    	pFileInfo->offset >= pFileInfo->nMaxLen.Val )
    {
	pFileInfo->flags |= FSFRAMFILE_ERROR;    //Set error condition
#if (DEBUG_FSFRAM >= LOG_ERROR)
	debugPutMsg(8);     //FILE handle (%d) passed to fseeGetByte() is not used
	debugPutByte(fhandle);
#endif
	return 0;
    }

FRAMBeginWrite ( (FRAM_ADDR)(pFileInfo->address.Val + pFileInfo->offset) );
//Write next byte
FRAMWrite ( b );
//Increment offset and check for end of file
if ( ++(pFileInfo->offset) == pFileInfo->nMaxLen.Val )
    { pFileInfo->flags |= FSFRAMFILE_EOF; }
FRAMEndWrite();

return TRUE;
}


/**
 * Finishes writing any data that has not yet been written to the File System. When writing data to a
 * file via the filePutByte() function, it is not always written straight to the File System Media, but
 * some times to an intermediate buffer. This function will write all pending data from the buffer to the
 * File System Media.
 */
void fsframFlush(FSFRAM_FILE fhandle) {
//all our writes are immediate, so there is no flushing
}


/**
 * Set the current file pointer for the given file. This is the offset in the given file that the
 * next read or write will be performed on.
 */
void fsframSetPos ( FSFRAM_FILE fhandle, FSFRAM_FILE_POS put )
{
FSFRAM_FILE_INFO * pFileInfo;

#if (DEBUG_FSFRAM >= LOG_ERROR)
if (fhandle >= FSFRAM_MAX_FILES)
    {
	debugPutMsg(15);     //fseeGetByte() called with invalid FILE handle %d
	debugPutByte(fhandle);
	return 0;
    }
#endif

pFileInfo = &getFCB(fhandle);

//If FSFRAM_FILE handle is not in use, or the position is greater that maximum, return with error
if ( (pFileInfo->flags & FSFRAMFILE_USED) == 0 ||
    	put >= pFileInfo->nMaxLen.Val )
    {
	pFileInfo->flags |= FSFRAMFILE_ERROR;    //Set error condition
#if (DEBUG_FSFRAM >= LOG_ERROR)
	debugPutMsg(8);     //FILE handle (%d) passed to fseeGetPos() is not used
	debugPutByte(fhandle);
#endif
	return;    //fail
    }

//set the current file position; hope it won't overwrite the next file or
//extend beyond the file system image!
pFileInfo->offset = put;
}


/**
 * Get the current file pointer for the given file. This is the offset in the given file that the
 * next read or write will be performed on. This value can be used as a parameter to the fileSetPos()
 * function at a later stage to restore the current file position. This is NOT the file address in
 * the file system.
 */
FSFRAM_FILE_POS fsframGetPos ( FSFRAM_FILE fhandle )
{
FSFRAM_FILE_INFO * pFileInfo;

#if (DEBUG_FSFRAM >= LOG_ERROR)
if (fhandle >= FSFRAM_MAX_FILES)
    {
	debugPutMsg(15);     //fseeGetPos() called with invalid FILE handle %d
	debugPutByte(fhandle);
	return 0;
    }
#endif

pFileInfo = &getFCB(fhandle);

//If FSFRAM_FILE handle is not in use, return with error
if ( (pFileInfo->flags & FSFRAMFILE_USED) == 0)
    {
	pFileInfo->flags |= FSFRAMFILE_ERROR;    //Set error condition
#if (DEBUG_FSFRAM >= LOG_ERROR)
	debugPutMsg(8);     //FILE handle (%d) passed to fseeGetPos() is not used
	debugPutByte(fhandle);
#endif
	return 0;
    }

return pFileInfo->offset;    //the offset now
}


/**
 * Gets the address in the File System of the requested file's FAT entry. This address can be used as a fast
 * way to open files in the future with the fileOpenFAT() function.
 *
 * !!! IMPORTANT !!!
 * The File System FAT entry address obtained with the fileGetFAT() function will only be valid as long as no
 * modifications are made to the File System! If after obtaining a address with the fileGetFAT() function the
 * File System is modified, this value might not be valid any more!
 */
FSFRAM_POS fsframGetFAT ( BYTE* filename )
{
FSFRAM_POS fatAdrNow, fatAdrRet;    //Address of current FAT entry
BYTE c, i;

#if (DEBUG_FSFRAM >= LOG_DEBUG)
debugPutMsg(1); //fseeOpen called for %s
debugPutString(filename);
#endif

//Check if File System is available
if (IS_BIT_CLEAR(fsframFlags, FSFRAMFLAG_AVAILABLE))
    {
#if (DEBUG_FSFRAM >= LOG_ERROR)
	debugPutMsg(2);    //File System not available for fseeOpen() function
#endif
	return FSFRAM_NOT_AVAILABLE;
    }

//Set to first FAT entry, follows after "Reserve Block" and "File System Header"
fatAdrRet = 0;
fatAdrNow = FSFRAM_RESERVE_BLOCK + sizeof(FSFRAM_HEADER);

//If string is empty, do not attempt to find it in FAT.
if ( *filename == '\0' )
	return FSFRAM_FILE_INVALID;

filename = (BYTE*)strupr((char*)filename);

while(1)
    {
    //Prepare for reading FAT
	FRAMBeginRead ( fatAdrNow );
    
    //Read first byte from FAT = Attribute
	c = FRAMRead();
#if (DEBUG_FSFRAM >= LOG_DEBUG)
	debugPutMsg(4);     //Read FAT Attributes during fseeOpen = %x
	debugPutByteHex(c);
#endif

    //End of FAT reached, file was not found
	if (c & FATFLAG_END)
        {
    	break;
        }
    
    //Read Filename from FAT, and compare to given filename
	for (i = 0; i < MAX_FILE_NAME_LEN; i++)
        {
    	c = FRAMRead();
        //No match, increment to next FAT entry
    	if (c != filename[i])
            {
        	fatAdrNow += sizeof(FSFRAM_ENTRY);
        	break;
            }
        
        //Match!!
    	if (c == '\0')
            {
            //Increment past filename.
        	while (++i < MAX_FILE_NAME_LEN) FRAMRead();
        	break;
            }
        }

    //Match was found!
	if (i == MAX_FILE_NAME_LEN)
        {
    	fatAdrRet = fatAdrNow;
    	break;
        }

    //End current sequencial read. At top of this loop a new sequencial read FRAMBeginRead() will be initiated
	FRAMEndRead();
    }

FRAMEndRead();    //if any...

return fatAdrRet;
}


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
FSFRAM_FILE fsframOpenFAT ( FSFRAM_POS fatPos )
{
FSFRAM_FILE fhandle = FSFRAM_NOT_AVAILABLE;
FSFRAM_FILE_INFO * pFileInfo;
BYTE i;

//Check if there are any FSFRAM_FILE handles available
#if (FSFRAM_MAX_FILES == 1)    //system with just one file handle to use

if (getFCB(0).flags & FSFRAMFILE_USED)
    {
#if (DEBUG_FSFRAM >= LOG_ERROR)
	debugPutMsg(3); //No File handles available for fseeOpen() function
#endif
	return FSFRAM_NOT_AVAILABLE;
    }
fhandle = 0;

#else	//system with more than one file handle to use

for (i = 0; i < FSFRAM_MAX_FILES; i++)
    {
	if ( !(getFCB(i).flags & FSFRAMFILE_USED))
        {
    	fhandle = i;    //Found available handle, use it!
    	break;
        }
    }
if (fhandle == FSFRAM_NOT_AVAILABLE)    //All handles are used, return FSFRAM_NOT_AVAILABLE
    {
#if (DEBUG_FSFRAM >= LOG_ERROR)
	debugPutMsg(3); //No File handles available for fseeOpen() function
#endif
	return FSFRAM_NOT_AVAILABLE;
    }

#endif

//prepare to read FAT entry
FRAMBeginRead ( fatPos );

#if (DEBUG_FSFRAM >= LOG_INFO)
debugPutMsg(5);     //fseeOpen() Found file, assigned to handle %d
debugPutByte(fhandle);
#endif

pFileInfo = &getFCB(fhandle);

pFileInfo->flags = FSFRAMFILE_USED;  //Indicate this FSFRAM_FILE handle is used
pFileInfo->offset = 0;             //After opening file, offset is 0

//Read address from FAT, is always 3 bytes long!
pFileInfo->address.v[0] = FRAMRead();
pFileInfo->address.v[1] = FRAMRead();
pFileInfo->address.v[2] = FRAMRead();

//Read length from FAT, is 2 or 3 bytes long
pFileInfo->length.v[0] = FRAMRead();
pFileInfo->length.v[1] = FRAMRead();
#if defined(FSFRAM_FILE_SIZE_16MB)
pFileInfo->length.v[2] = FRAMRead();
#endif

FRAMEndRead();    //finished reading FAT


//XXX whiz through FAT finding end address
pFileInfo->nMaxLen = pFileInfo->length;    //for now, it's just what we started with

fsframOpenCount++;

return fhandle;
}


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
BOOL fsframOpenImage(void) {
    BYTE i;
    
    //Check if File System is available
    if (IS_BIT_CLEAR(fsframFlags, FSFRAMFLAG_AVAILABLE))
        return FALSE;
        
    //Ensure there are NO open files!
    #if (FSFRAM_MAX_FILES == 1)
        if (getFCB(0).flags & FSFRAMFILE_USED) {
            return FALSE;
        }
    #else
        for (i = 0; i < FSFRAM_MAX_FILES; i++) {
            if (getFCB(i).flags & FSFRAMFILE_USED) {
                return FALSE;       //Found open file, return error!
            }
        }
    #endif

    #if (DEBUG_FSFRAM >= LOG_INFO)
    debugPutMsg(10);    //fseeOpenImage() Successfully executed
    #endif
    
    getFCB(0).flags = FSFRAMFILE_USED;    //Indicate this FSFRAM_FILE handle is used
    
    //Use "address" member in FILE handle 0 to indicate next byte that will be written to
    //The first byte written to is the one following the "Reserved Block"
    getFCB(0).address.Val = FSFRAM_RESERVE_BLOCK;

    return TRUE;
}

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
BOOL fsframPutByteImage(BYTE b) {
    FSFRAM_FILE_INFO * pFileInfo;
    
    pFileInfo = &getFCB(0);
    
    //We currently are NOT writing (FRAM Sequencial write) - initiate sequencial write sequence
    if ( (pFileInfo->flags & FSFRAMFILE_WRITING) == 0) {
        //Check if the File System is not currently busy reading or writing from another open file
        //Only needed when more then one FILE. When only one file, use FSFRAMFILE_READING and FSFRAMFILE_WRITING of framFCB[0].flags
        #if (FSFRAM_MAX_FILES > 1)
        if (fsframFlags & FSFRAMFLAG_READING_WRITING) {
            pFileInfo->flags |= FSFRAMFILE_ERROR;    //Set error condition
            #if (DEBUG_FSFRAM >= LOG_ERROR)
            debugPutMsg(12);     //Currently busy reading or writing from another open file, can not open file
            #endif
            return FALSE;
        }
        #endif
        
        //Successfully placed FRAM into sequencial write mode
    	FRAMBeginWrite( (FRAM_ADDR)(pFileInfo->address.Val) );
    	pFileInfo->flags |= FSFRAMFILE_WRITING;    //Indicate that this FILE is currently in Reading mode
        
        //Indicate that the File System is currently Reading or Writing an open file
        //Only needed when more then one FILE. When only one file, use FSFRAMFILE_READING and FSFRAMFILE_WRITING of framFCB[0].flags
        #if (FSFRAM_MAX_FILES > 1)
    	fsframFlags |= FSFRAMFLAG_READING_WRITING;
        #endif
    }

    //Write given byte
    FRAMWrite(b);
    
    //Increment address
    pFileInfo->address.Val++;

//HHH must _always_ relinquish SPI in the interest of safety/general utility
//(the 8019 shares the IO port).  This is hardly optimal, but the FRAM is
//greatly faster than the EE anyway.
    FRAMEndWrite();
	pFileInfo->flags &= ~FSFRAMFILE_WRITING;  //not writing any more
    #if (FSFRAM_MAX_FILES > 1)
    fsframFlags &= (~FSFRAMFLAG_READING_WRITING);
    #endif

    return TRUE;
}


/**
 * Finishes writing the File System Image
 *
 * @return          TRUE if successful <br>
 *                  FALSE otherwise
 */
BOOL fsframCloseImage(void) {
    // Release any resources held by this file
    fsframRelease(0);

    getFCB(0).flags = 0;      //Indicate this FSFRAM_FILE handle is free to be used

    return TRUE;
}
