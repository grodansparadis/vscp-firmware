/*********************************************************************
 *
 *               Modtronix File System Implementation for EEPROMs
 *
 *********************************************************************
 * FileName:        fsee.c
 * Dependencies:    stacktsk.H
 *                  fsee.h
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
 * 2006-01-14, David Hosken (DH):
 *      - made getFCB() a local function in stead of a global function
 *      - speed optimized
 * 2005-11-15, David Hosken (DH):
 *      - Original version
********************************************************************/
#define THIS_IS_FSEE

#include <string.h>
#include <stdlib.h>

#include "projdefs.h"
#include "net\checkcfg.h" 
#include "net\fsee.h"
#include "net\xeeprom.h"
#include "debug.h"

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xE3, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xE3, msgCode, msgStr)

/////////////////////////////////////////////////
// Global variables
FSEE_FILE_INFO FCB[FSEE_MAX_FILES];

BYTE fseeOpenCount;     /**< Current number of open files */
BYTE fseeFlags;         /**< Current number of open files */
BYTE pageWrite;         /**< Counts how many bytes have been written in current page write mode */


/*
 * This file system supports short file names i.e. 8 + 3.
 */
#define MAX_FILE_NAME_LEN   (12ul)


/** FSEE FAT Entry */
typedef struct _FSEE_ENTRY
{
    BYTE attr;              /**< Contains file attributes */

    BYTE name[MAX_FILE_NAME_LEN];   /**< File name, has the format 8 + '.' + 3 */

    SWORD_VAL address;  /**< Address in File System of this file. This address has already taken the reserve block into account */

    #if defined(FSEE_FILE_SIZE_16MB)
    SWORD_VAL length;   /**< File length. LSB will be first in memory */
    #else
    WORD_VAL length;    /**< File length. LSB will be first in memory */
    #endif
} FSEE_ENTRY;


/** FSEE FSYS Header */
typedef struct _FSEE_HEADER
{
    BYTE length;        /**< File System Header length */

    BYTE id;            /**< File System identifier */
    
    BYTE version;       /**< File System version */

    /**
     * Number of FAT entries that follow the "FSYS Header" = number of files - 1. The
     * last FAT entry does not represent a file, and has the FATFLAG_END flag set.
     */
    WORD fatEntries;
} FSEE_HEADER;


/**
 * FILE Attribute. When set, indicates that this is the last FAT entry
 */
#define FATFLAG_END         0x80ul

/**
 * FILE Attribute. When set, indicates that this file is read only
 */
#define FATFLAG_READ_ONLY   0x01ul


/**
 * Get a reference to the FCB for the given FSEE_FILE handle
 *
 * @param fhandle FSEE_FILE handle of desired FCB
 *
 * @return The requested FCB
 */
#if (FSEE_MAX_FILES == 1)
    #define getFCB(fhandle)     FCB[0]
#else
    #define getFCB(fhandle)     FCB[fhandle]
#endif


/**
 * Initializes the Modtronix File System
 *
 * @return      TRUE, if File System Storage access is initialized and File System is is ready to be used <br>
 *              FALSE otherwise
 */
BOOL fseeInit(void) {
    #if (FSEE_MAX_FILES > 1)
    BYTE i;
    #endif
    fseeOpenCount = 0;
    fseeFlags = FSEEFLAG_AVAILABLE;

    //Initialize the EEPROM access routines. Use ~ 400 Khz.
    XEEInit(EE_BAUD(CLOCK_FREQ, 400000));
    
    //Set all FILE object as free
    #if (FSEE_MAX_FILES == 1)
        FCB[0].flags = 0;
    #else
        for (i = 0; i < FSEE_MAX_FILES; i++) {
            FCB[i].flags = 0;
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
 * @return          - A FSEE_FILE Handle (value 0 - 127) if the file is found
 *                  - FSEE_FILE_INVALID if file could not be opened
 *                  - FSEE_NOT_AVAILABLE if the File System is not available
 */
FSEE_FILE fseeOpen(BYTE* filename, BYTE mode) {
    BYTE c;
    BYTE i;
    
    FSEE_FILE fhandle = FSEE_NOT_AVAILABLE;

    FSEE_POS fatAdr;    //Address of current FAT entry
    
    FSEE_FILE_INFO * pFileInfo;
    
    #if (DEBUG_FSEE >= LOG_DEBUG)
    debugPutMsg(1); //fseeOpen called for %s
    debugPutString(filename);
    #endif

    //Check if File System is available
    if (IS_BIT_CLEAR(fseeFlags, FSEEFLAG_AVAILABLE)) {
        #if (DEBUG_FSEE >= LOG_ERROR)
        debugPutMsg(2);    //File System not available for fseeOpen() function
        #endif
    
        return FSEE_NOT_AVAILABLE;
    }
        
    //Check if there are any FSEE_FILE handles available
    #if (FSEE_MAX_FILES == 1)
        if (FCB[0].flags & FSEEFILE_USED) {
            #if (DEBUG_FSEE >= LOG_ERROR)
            debugPutMsg(3); //No File handles available for fseeOpen() function
            #endif

            return FSEE_NOT_AVAILABLE;
        }
        fhandle = 0;
    #else
        for (i = 0; i < FSEE_MAX_FILES; i++) {
            if ( !(FCB[i].flags & FSEEFILE_USED)) {
                fhandle = i;    //Found available handle, use it!
                break;
            }
        }
        
        //All handles are used, return FSEE_NOT_AVAILABLE
        if (fhandle == FSEE_NOT_AVAILABLE) {
            #if (DEBUG_FSEE >= LOG_ERROR)
            debugPutMsg(3); //No File handles available for fseeOpen() function
            #endif

            return FSEE_NOT_AVAILABLE;
        }
    #endif

    //Set to first FAT entry, follows after "Reserve Block" and "File System Header"
    fatAdr = FSEE_RESERVE_BLOCK + sizeof(FSEE_HEADER);

    //If string is empty, do not attempt to find it in FAT.
    if ( *filename == '\0' )
        return FSEE_FILE_INVALID;

    filename = (BYTE*)strupr((char*)filename);

    while(1)
    {
        //Prepare for reading FAT
        if (XEEBeginRead(EEPROM_CONTROL, fatAdr) != XEE_SUCCESS ) {
            fhandle = FSEE_NOT_AVAILABLE;
            break;
        }
            
        //Read first byte from FAT = Attribute
        c = XEERead();
        #if (DEBUG_FSEE >= LOG_DEBUG)
        debugPutMsg(4);     //Read FAT Attributes during fseeOpen = %x
        debugPutByteHex(c);
        #endif
        
        //End of FAT reached, file was not found
        if (c & FATFLAG_END) {
            fhandle = FSEE_FILE_INVALID;
            break;
        }

        //Read Filename from FAT, and compare to given filename
        for (i = 0; i < MAX_FILE_NAME_LEN; i++) {
            c = XEERead();

            //No match, increment to next FAT entry
            if (c != filename[i]) {
                fatAdr += sizeof(FSEE_ENTRY);
                break;
            }
            
            //Match!!
            if (c == '\0') {
                //Increment past filename.
                while (++i < MAX_FILE_NAME_LEN) XEERead();
                break;
            }
        }
        
        //Match was found!
        if (i == MAX_FILE_NAME_LEN) {
            #if (DEBUG_FSEE >= LOG_INFO)
            debugPutMsg(5);     //fseeOpen() Found file, assigned to handle %d
            debugPutByte(fhandle);
            #endif
        
            pFileInfo = &getFCB(fhandle);
            
            pFileInfo->flags = FSEEFILE_USED;  //Indicate this FSEE_FILE handle is used
            pFileInfo->offset = 0;             //After opening file, offset is 0
            
            //Read address from FAT, is always 3 bytes long!
            pFileInfo->address.v[0] = XEERead();
            pFileInfo->address.v[1] = XEERead();
            pFileInfo->address.v[2] = XEERead();

            //Read length from FAT, is 2 or 3 bytes long
            pFileInfo->length.v[0] = XEERead();
            pFileInfo->length.v[1] = XEERead();
            #if defined(FSEE_FILE_SIZE_16MB)
            pFileInfo->length.v[2] = XEERead();
            #endif

            fseeOpenCount++;
            break;
        }

        //End current sequencial read. At top of this loop a new sequencial read XEEBeginRead() will be initiated
        XEEEndRead();
        
        FAST_USER_PROCESS();
    }
    
    //End current sequencial read.
    XEEEndRead();
    
    return fhandle;
}


/**
 * Closes the given file. Seeing that the File System can only have a limited amount of files open
 * at any time (defined by FSEE_MAX_FILES), it is very important to call this function after finished
 * with a file!
 *
 * @param fhandle   FSEE_FILE handle of the file to be released
 */
void fseeClose(FSEE_FILE fhandle) {
    #if (DEBUG_FSEE >= LOG_ERROR)
    if (fhandle >= FSEE_MAX_FILES) {
        debugPutMsg(13);     //fseeClose() called with invalid FILE handle %d
        debugPutByte(fhandle);
        return;
    }
    #endif

    #if (DEBUG_FSEE >= LOG_INFO)
    debugPutMsg(6);     //fseeClose() called for FILE handle %d
    debugPutByte(fhandle);
    #endif

    // Release any resources held by this file
    fileRelease(fhandle);

    getFCB(fhandle).flags = 0;      //Indicate this FSEE_FILE handle is free to be used
}


/**
 * Releases any resources that the given open file might be reserving. Ends any sequencial
 * page write cycle that might be in progress, and waits for internal write operation to complete.
 * On certian File Systems, like ones that use EEPROMs on a shared I2C bus for example, the File
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
 * @param fhandle   FSEE_FILE handle of the file to be released
 */
void fseeRelease(FSEE_FILE fhandle) {
    FSEE_FILE_INFO * pFileInfo;

    #if (DEBUG_FSEE >= LOG_ERROR)
    if (fhandle >= FSEE_MAX_FILES) {
        debugPutMsg(14);     //fseeRelease() called with invalid FILE handle %d
        debugPutByte(fhandle);
        return;
    }
    #endif

    pFileInfo = &getFCB(fhandle);
    
    //If currently reading (EEPROM Sequencial read), send I2C stop condition and terminate sequencial read
    if (pFileInfo->flags & FSEEFILE_READING) {
        XEEEndRead();
    }
    //If currently writing (EEPROM Sequencial write), send I2C stop condition and terminate sequencial write
    else if (pFileInfo->flags & FSEEFILE_WRITING) {
        XEEEndWrite();                                          //End current sequencial write
        while( XEEIsBusy(EEPROM_CONTROL) ) FAST_USER_PROCESS(); //Wait until write delay is finished - takes up to 5ms
    }
    
    //Clear READING and WRITING flags
    pFileInfo->flags &= ~(FSEEFILE_WRITING | FSEEFILE_READING);
    
    //Indicate that the File System is currently NOT Writing or Reading from any open file
    //Only needed when more then one FILE. When only one file, use FSEEFILE_READING and FSEEFILE_WRITING of FCB[0].flags
    #if (FSEE_MAX_FILES > 1)
    fseeFlags &= (~FSEEFLAG_READING_WRITING);
    #endif

    /*
    #if (DEBUG_FSEE >= LOG_INFO)
    debugPutMsg(7);     //fseeRelease() called for FILE handle %d
    debugPutByte(fhandle);
    #endif
    */
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
 * This function will place the EEPROM in sequencial read mode and take control of the I2C bus!
 * To allow other devices to use the I2C bus while the file is open, call fileRelease() when finished
 * reading some data. When calling fileRead() after calling fileRelease(), the EEPROM will automatically
 * be placed in sequencial read mode again and take control of the bus.
 *
 * To read multiple bytes, see ex_file_read.c example file.
 *
 * @preCondition    fileOpen() != FSEE_FILE_INVALID and<br>
 *                  fileGetByteBegin() == TRUE
 *
 * @param fhandle   FSEE_FILE handle of the file to be released
 *
 * @return          Data byte from current address.
 */
BYTE fseeGetByte(FSEE_FILE fhandle) {
    BYTE c;

    FSEE_FILE_INFO * pFileInfo;

    #if (DEBUG_FSEE >= LOG_ERROR)
    if (fhandle >= FSEE_MAX_FILES) {
        debugPutMsg(15);     //fseeGetByte() called with invalid FILE handle %d
        debugPutByte(fhandle);
        return 0;
    }
    #endif

    pFileInfo = &getFCB(fhandle);

    //Clear all error flags
    pFileInfo->flags &= ~(FSEEFILE_EOF | FSEEFILE_ERROR);
    
    //If FSEE_FILE handle is not in use, return with error
    if ( (pFileInfo->flags & FSEEFILE_USED) == 0) {
        pFileInfo->flags |= FSEEFILE_ERROR;    //Set error condition
        #if (DEBUG_FSEE >= LOG_ERROR)
        debugPutMsg(8);     //FILE handle (%d) passed to fseeGetByte() is not used
        debugPutByte(fhandle);
        #endif

        return 0;
    }

    //If currently NOT reading (EEPROM Sequencial read), initiate sequencial read sequence
    if ( (pFileInfo->flags & FSEEFILE_READING) == 0) {
        //Check if the File System is not currently busy reading or writing from another open file
        //Only needed when more then one FILE. When only one file, use FSEEFILE_READING and FSEEFILE_WRITING of FCB[0].flags
        #if (FSEE_MAX_FILES > 1)
        if (fseeFlags & FSEEFLAG_READING_WRITING) {
            pFileInfo->flags |= FSEEFILE_ERROR;    //Set error condition
            #if (DEBUG_FSEE >= LOG_ERROR)
            debugPutMsg(12);     //Currently busy reading or writing from another open file, can not open file
            #endif
            return 0;
        }
        #endif
        
        //Successfully placed EEPROM into sequencial read mode
        if (XEEBeginRead(EEPROM_CONTROL,
                (XEE_ADDR)(pFileInfo->address.Val + pFileInfo->offset)) == XEE_SUCCESS)
        {
            pFileInfo->flags |= FSEEFILE_READING;  //Indicate that this FILE is currently in Reading mode

            //Indicate that the File System is currently Reading or Writing an open file
            //Only needed when more then one FILE. When only one file, use FSEEFILE_READING and FSEEFILE_WRITING of FCB[0].flags
            #if (FSEE_MAX_FILES > 1)
            fseeFlags |= FSEEFLAG_READING_WRITING;
            #endif
        }
        //Error!
        else {
            pFileInfo->flags |= FSEEFILE_ERROR;    //Set error condition
            #if (DEBUG_FSEE >= LOG_ERROR)
            debugPutMsg(9);     //Err in fseeGetByte() while trying to place EEPROM in seq read mode
            #endif
            return 0;
        }
    }

    //Read next byte
    c = XEERead();
    
    //Increment offset and check for end of file
    if (++(pFileInfo->offset) == pFileInfo->length.Val) {
        pFileInfo->flags |= FSEEFILE_EOF;
    }
    
    /*
    if ((pFileInfo->flags & 0xf0) != 0) {
        debugPutGenMsg(2);     //@mxd:2:%s
        debugPutRomStringXNull("fseeGetByte flags = 0x");
        debugPutByteHex(pFileInfo->flags);
        debugPutByte(0);    //Null terminate string
    }
    */

    return c;
}

/**
 * Deletes all files present on the File System
 *
 * @return          TRUE if successful <br>
 *                  FALSE otherwise
 */
BOOL fseeFormat(void) {
    
    return TRUE;
}


/**
 * Writes a byte to the given file.
 *
 * This implementation was written by OmarZ, see http://forum.modtronix.com/index.php?topic=242.0
 * It has not been checked by us yet!
 */
BOOL fseePutByte(FSEE_FILE fhandle, BYTE b) {
    FSEE_FILE_INFO * pFileInfo;
    
    pFileInfo = &getFCB(fhandle);

    //We currently ARE writing (in EEPROM Sequencial write mode)
    if (pFileInfo->flags & FSEEFILE_WRITING) {
        //If we have reached our page limit with the current sequencial write, end it and start new sequencial write
        if (pageWrite >= FSEE_WRITE_PAGE_SIZE) {
            //Removes this FILE from "currently writing" mode. Ends any sequencial page write cycle that might
            //be in progress, and waits for internal write operation to complete.
            fseeRelease(0);
        }
    }

    //We currently are NOT writing (EEPROM Sequencial write) - initiate sequencial write sequence
    if ( (pFileInfo->flags & FSEEFILE_WRITING) == 0) {
        //Check if the File System is not currently busy reading or writing from another open file
        //Only needed when more then one FILE. When only one file, use FSEEFILE_READING and FSEEFILE_WRITING of FCB[0].flags
        #if (FSEE_MAX_FILES > 1)
        if (fseeFlags & FSEEFLAG_READING_WRITING) {
            pFileInfo->flags |= FSEEFILE_ERROR;    //Set error condition
            #if (DEBUG_FSEE >= LOG_ERROR)
            debugPutMsg(12);     //Currently busy reading or writing from another open file, can not open file
            #endif
            return FALSE;
        }
        #endif

        //Successfully placed EEPROM into sequencial write mode
        if ( XEEBeginWrite(EEPROM_CONTROL, (XEE_ADDR)(pFileInfo->address.Val+ pFileInfo->offset)) == XEE_SUCCESS)
        {
            pFileInfo->flags |= FSEEFILE_WRITING;    //Indicate that this FILE is currently in Writing mode

            //Indicate that the File System is currently Writing an open file
            //Only needed when more then one FILE. When only one file, use FSEEFILE_READING and FSEEFILE_WRITING of FCB[0].flags
            #if (FSEE_MAX_FILES > 1)
            fseeFlags |= FSEEFLAG_READING_WRITING;
            #endif
        }
        //Error!
        else {
            #if (DEBUG_FSEE >= LOG_ERROR)
            debugPutMsg(11);    //Err in fseePutByte() while trying to place EEPROM in seq write mode
            #endif
            return FALSE;
        }
    }

    //Write given byte
    XEEWrite(b);

    //Keep track of how many bytes have been written in sequencial page write
    //pageWrite++;

    //Increment offset and check for end of file
    if (++(pFileInfo->offset) == pFileInfo->length.Val) {
        pFileInfo->flags |= FSEEFILE_EOF;
    }

    return TRUE;
}


/**
 * Finishes writing any data that has not yet been written to the File System. When writing data to a
 * file via the filePutByte() function, it is not always written straight to the File System Media, but
 * some times to an intermediate buffer. This function will write all pending data from the buffer to the
 * File System Media.
 */
void fseeFlush(FSEE_FILE fhandle) {

}


/**
 * Set the current file pointer for the given file. This is the offset in the given file that the
 * next read or write will be performed on.
 */
void fseeSetPos(FSEE_FILE fhandle, FSEE_FILE_POS put) {

}


/**
 * Get the current file pointer for the given file. This is the offset in the given file that the
 * next read or write will be performed on. This value can be used as a parameter to the fileSetPos()
 * function at a later stage to restore the current file position. This is NOT the file address in
 * the file system.
 */
FSEE_FILE_POS fseeGetPos(FSEE_FILE fhandle) {
    return 0;
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
FSEE_POS fseeGetFAT(BYTE* name) {
    return 0;
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
 * @return          - A FSEE_FILE Handle (value 0 - 127) if the file is found
 *                  - FSEE_FILE_INVALID if file could not be opened
 *                  - FSEE_NOT_AVAILABLE if the File System is not available
 */
FSEE_FILE fseeOpenFAT(FSEE_POS fatPos) {
    return 0;
}


/**
 * Prepares the File System to receive a new Image via following calls to fseePutByteImage()
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
BOOL fseeOpenImage(void) {
    BYTE i;
    
    //Check if File System is available
    if (IS_BIT_CLEAR(fseeFlags, FSEEFLAG_AVAILABLE))
        return FALSE;
        
    //Ensure there are NO open files!
    #if (FSEE_MAX_FILES == 1)
        if (FCB[0].flags & FSEEFILE_USED) {
            return FALSE;
        }
    #else
        for (i = 0; i < FSEE_MAX_FILES; i++) {
            if (FCB[i].flags & FSEEFILE_USED) {
                return FALSE;       //Found open file, return error!
            }
        }
    #endif

    #if (DEBUG_FSEE >= LOG_INFO)
    debugPutMsg(10);    //fseeOpenImage() Successfully executed
    #endif
    
    getFCB(0).flags = FSEEFILE_USED;    //Indicate this FSEE_FILE handle is used
    
    //Use "address" member in FILE handle 0 to indicate next byte that will be written to
    //The first byte written to is the one following the "Reserved Block"
    getFCB(0).address.Val = FSEE_RESERVE_BLOCK;

    return TRUE;
}

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
BOOL fseePutByteImage(BYTE b) {
    FSEE_FILE_INFO * pFileInfo;
    
    pFileInfo = &getFCB(0);
    
    //We currently ARE writing (in EEPROM Sequencial write mode)
    if (getFCB(0).flags & FSEEFILE_WRITING) {
        //If we have reached our page limit with the current sequencial write, end it and start new sequencial write
        if (pageWrite >= FSEE_WRITE_PAGE_SIZE) {
            //Removes this FILE from "currently writing" mode. Ends any sequencial page write cycle that might
            //be in progress, and waits for internal write operation to complete.
            fseeRelease(0);
        }
    }

    //We currently are NOT writing (EEPROM Sequencial write) - initiate sequencial write sequence
    if ( (getFCB(0).flags & FSEEFILE_WRITING) == 0) {
        //Check if the File System is not currently busy reading or writing from another open file
        //Only needed when more then one FILE. When only one file, use FSEEFILE_READING and FSEEFILE_WRITING of FCB[0].flags
        #if (FSEE_MAX_FILES > 1)
        if (fseeFlags & FSEEFLAG_READING_WRITING) {
            getFCB(0).flags |= FSEEFILE_ERROR;    //Set error condition
            #if (DEBUG_FSEE >= LOG_ERROR)
            debugPutMsg(12);     //Currently busy reading or writing from another open file, can not open file
            #endif
            return FALSE;
        }
        #endif
        
        //Successfully placed EEPROM into sequencial write mode
        if ( XEEBeginWrite(EEPROM_CONTROL, (XEE_ADDR)(getFCB(0).address.Val)) == XEE_SUCCESS)
        {
            getFCB(0).flags |= FSEEFILE_WRITING;    //Indicate that this FILE is currently in Reading mode
            
            //Indicate that the File System is currently Reading or Writing an open file
            //Only needed when more then one FILE. When only one file, use FSEEFILE_READING and FSEEFILE_WRITING of FCB[0].flags
            #if (FSEE_MAX_FILES > 1)
            fseeFlags |= FSEEFLAG_READING_WRITING;
            #endif
            
            pageWrite = 0;  //Keep track of how many bytes have been written in sequencial page write mode
        }
        //Error!
        else {
            #if (DEBUG_FSEE >= LOG_ERROR)
            debugPutMsg(11);    //Err in fseePutByte() while trying to place EEPROM in seq write mode
            #endif
            return FALSE;
        }
    }

    //Write given byte
    XEEWrite(b);
    
    //Keep track of how many bytes have been written in sequencial page write
    pageWrite++;
    
    //Increment address
    getFCB(0).address.Val++;

    return TRUE;
}


/**
 * Finishes writing the File System Image
 *
 * @return          TRUE if successful <br>
 *                  FALSE otherwise
 */
BOOL fseeCloseImage(void) {
    // Release any resources held by this file
    fileRelease(0);

    getFCB(0).flags = 0;      //Indicate this FSEE_FILE handle is free to be used

    return TRUE;
}
