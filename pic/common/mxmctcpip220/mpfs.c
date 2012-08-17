/*********************************************************************
 *
 *               Microchip File System Implementaion on PIC18
 *
 *********************************************************************
 * FileName:        MPFS.c
 * Dependencies:    stacktsk.H
 *                  mpfs.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Microchip Technology, Inc.
 *
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
 *
 * HiTech PICC18 Compiler Options excluding device selection:
 *                  -FAKELOCAL -G -E -C
 *
 *
 *
 *
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     8/14/01     Original (Rev. 1.0)
 * Nilesh Rajbharti     2/9/02      Cleanup
 * Nilesh Rajbharti     5/22/02     Rev 2.0 (See version.log for detail)
********************************************************************/
#define THIS_IS_MPFS

#include <string.h>
#include <stdlib.h>

#include "mpfs.h"

#if defined(MPFS_USE_EEPROM)
    #include "xeeprom.h"
#endif

/*
 * This file system supports short file names i.e. 8 + 3.
 */
#define MAX_FILE_NAME_LEN   (12)

#define MPFS_DATA          (0x00)
#define MPFS_DELETED       (0x01)
#define MPFS_DLE           (0x03)
#define MPFS_ETX           (0x04)

/*
 * MPFS Structure:
 *
 * MPFS_Start:
 *      <MPFS_DATA><Address1><FileName1>
 *      <MPFS_DATA><Address2><FileName2>
 *      ...
 *      <MPFS_ETX><MPFS_INVALID><FileNamen>
 * Address1:
 *      <Data1>[<Data2>...<Datan>]<MPFS_ETX><MPFS_INVALID>
 *      ...
 *
 * Note: - If File data contains either MPFS_DLE or MPFS_ETX extra MPFS_DLE is stuffed
 *         before that byte.
 *       - Last FAT entry contains "Flag=MPFS_EXT" and "address=MPFS_INVALID"
 *       - 
 */
typedef struct  _MPFS_ENTRY
{
    BYTE Flag;

    MPFS Address;
    BYTE Name[MAX_FILE_NAME_LEN];           // 8 + '.' + 3
} MPFS_ENTRY;

static union
{
    struct
    {
        unsigned int bNotAvailable : 1;
    } bits;
    BYTE Val;
} mpfsFlags;

BYTE mpfsOpenCount;

#if defined(MPFS_USE_PGRM)
    /*
     * An address where MPFS data starts in program memory.
     */
    extern MPFS MPFS_Start;

#else

#define MPFS_Start     MPFS_RESERVE_BLOCK

#endif

MPFS _currentHandle;    //Every open file has a handle. The handle is the address of the next byte
                        //to be read.
MPFS _currentFile;      //Start address of current file
BYTE _currentCount;     //Counts number of bytes written.


/*********************************************************************
 * Function:        BOOL MPFSInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE, if MPFS Storage access is initialized and
 *                          MPFS is is ready to be used.
 *                  FALSE otherwise
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
BOOL MPFSInit(void)
{
    mpfsOpenCount = 0;
    mpfsFlags.Val = 0;

#if defined(MPFS_USE_PGRM)
    return TRUE;
#else
    /*
     * Initialize the EEPROM access routines.
     * Use ~ 400 Khz.
     */
    XEEInit(EE_BAUD(CLOCK_FREQ, 400000));

    return TRUE;
#endif
}


/*********************************************************************
 * Function:        MPFS MPFSOpen(BYTE* file)
 *
 * PreCondition:    None
 *
 * Input:           file        - NULL terminated file name.
 *
 * Output:          A handle if file is found = address of first byte of requested file
 *                  MPFS_INVALID if file is not found.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
MPFS MPFSOpen(BYTE* file)
{
    MPFS_ENTRY entry;
    MPFS FAT;
    BYTE fileNameLen;

    if ( mpfsFlags.bits.bNotAvailable )
        return MPFS_NOT_AVAILABLE;

#if defined(MPFS_USE_PGRM)
    FAT = (MPFS)&MPFS_Start;
#else
    FAT = MPFS_Start;
#endif

    /*
     * If string is empty, do not attempt to find it in FAT.
     */
    if ( *file == '\0' )
        return MPFS_INVALID;

    file = (BYTE*)strupr((char*)file);

    while(1)
    {
        //Read a single MPFS_ENTRY from the file system. The MPFS_ENTRY to read is pointed to by
        //FAT. FAT will be incremented to next entry if it is not the file we are looking for.
#if defined(MPFS_USE_PGRM)
        // Bring current FAT entry into RAM.
        memcpypgm2ram(&entry, (ROM void*)FAT, sizeof(entry));
#else
        XEEReadArray(EEPROM_CONTROL, FAT, (unsigned char*)&entry, sizeof(entry));
#endif

        // Make sure that it is a valid entry.
        if ( entry.Flag == MPFS_DATA )
        {
            // Does the file name match ?
            fileNameLen = strlen((char*)file);
            if ( fileNameLen > MAX_FILE_NAME_LEN )
                fileNameLen = MAX_FILE_NAME_LEN;

            //Match!!! This is the file we are looking for.
            if( !strncmp((char*)file, (char*)entry.Name, fileNameLen) )
            {
                _currentFile = (MPFS)entry.Address; //Store address of current file
                mpfsOpenCount++;
                return entry.Address;
            }

            // File does not match.  Try next entry...
            FAT += sizeof(entry);
        }
        else if ( entry.Flag == MPFS_ETX )
        {
            if ( entry.Address != (MPFS)MPFS_INVALID )
                FAT = (MPFS)entry.Address;
            //This was the last FAT entry - break
            else
                break;
        }
        else
            return (MPFS)MPFS_INVALID;
    }
    return (MPFS)MPFS_INVALID;
}


/*********************************************************************
 * Function:        void MPFSClose(void)
 *
 * PreCondition:    None
 *
 * Input:           handle      - File handle to be closed
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
void MPFSClose(void)
{
    _currentCount = 0;
    mpfsFlags.bits.bNotAvailable = FALSE;
    if ( mpfsOpenCount )
        mpfsOpenCount--;
}

/*********************************************************************
 * Function:        BOOL MPFSGetBegin(MPFS handle)
 *
 * PreCondition:    MPFSOpen() != MPFS_INVALID
 *
 * Input:           handle      - handle of file that is to be read = address of next byte to be read
 *
 * Output:          TRUE if successful
 *                  !TRUE otherwise
 *
 * Side Effects:    None
 *
 * Overview:        Prepares MPFS storage media for subsequent reads.
 *
 * Note:            None
 ********************************************************************/
#if defined(MPFS_USE_EEPROM)
BOOL MPFSGetBegin(MPFS handle)
{
    _currentHandle = handle;    //The handle is the address of the next byte to be read.

    return (XEEBeginRead(EEPROM_CONTROL, handle) == XEE_SUCCESS);
}
#endif

/*********************************************************************
 * Function:        BYTE MPFSGet(void)
 *
 * PreCondition:    MPFSOpen() != MPFS_INVALID &&
 *                  MPFSGetBegin() == TRUE
 *
 * Input:           None
 *
 * Output:          Data byte from current address.
 *
 * Side Effects:    None
 *
 * Overview:        Reads a byte from current address.
 *
 * Note:            Caller must call MPFSIsEOF() to check for end of
 *                  file condition
 ********************************************************************/
BYTE MPFSGet(void)
{
    BYTE t;

    //_currentHandle contains the address of the next byte to be read.

#if defined(MPFS_USE_PGRM)
    t = (BYTE)*_currentHandle;
#else
    t = XEERead();
#endif
    _currentHandle++;   //Increment to next byte to be read

    // TODO!!! What if file contains multiple 
    if ( t == MPFS_DLE )
    {
#if defined(MPFS_USE_PGRM)
        t = (BYTE)*_currentHandle;
#else
        t = XEERead();
#endif
        _currentHandle++;
    }
    //End of file
    else if ( t == MPFS_ETX )
    {
        _currentHandle = MPFS_INVALID;
    }

    return t;
}


/*********************************************************************
 * Function:        MPFS MPFSGetEnd(void)
 *
 * PreCondition:    MPFSOpen() != MPFS_INVALID &&
 *                  MPFSGetBegin() = TRUE
 *
 * Input:           None
 *
 * Output:          Current mpfs handle = address of next byte that must be read, if any.
 *
 * Side Effects:    None
 *
 * Overview:        Ends on-going read cycle.
 *                  MPFS handle that is returned must be used
 *                  for subsequent begin gets..
 *
 * Note:            None
 ********************************************************************/
#if defined(MPFS_USE_EEPROM)
MPFS MPFSGetEnd(void)
{
    XEEEndRead();

    return _currentHandle;  //_currentHandle contains the address of the next byte to be read.
}
#endif


/*********************************************************************
 * Function:        MPFS MPFSFormat(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          A valid MPFS handle that can be used for MPFSPut
 *
 * Side Effects:    None
 *
 * Overview:        Prepares MPFS image to get re-written
 *                  Declares MPFS as in use.
 *
 * Note:            MPFS will be unaccessible until MPFSClose is
 *                  called.
 ********************************************************************/
MPFS MPFSFormat(void)
{
    mpfsFlags.bits.bNotAvailable = TRUE;
    return (MPFS)MPFS_RESERVE_BLOCK;
}


/*********************************************************************
 * Function:        BOOL MPFSPutBegin(MPFS handle)
 *
 * PreCondition:    MPFSInit() and MPFSFormat() are already called.
 *
 * Input:           handle  - handle to where put to begin = address of
 *                            the next byte to be written to.
 *
 * Output:          TRUE if successful
 *                  !TRUE otherwise
 *
 * Side Effects:    None
 *
 * Overview:        Prepares MPFS image to get re-written
 *
 * Note:            MPFS will be unaccessible until MPFSClose is
 *                  called.
 ********************************************************************/
#if defined(MPFS_USE_EEPROM)
BOOL MPFSPutBegin(MPFS handle)
{
    //_currentCount = 0;
    _currentHandle = handle;    //Address of next byte to be written to
    _currentCount = (BYTE)handle;
    _currentCount &= (MPFS_WRITE_PAGE_SIZE-1);
    return (XEEBeginWrite(EEPROM_CONTROL, handle) == XEE_SUCCESS);
}
#endif


/*********************************************************************
 * Function:        BOOL MPFSPut(BYTE b)
 *
 * PreCondition:    MPFSFormat() or MPFSCreate() must be called
 *                  MPFSPutBegin() is already called.
 *
 * Input:           b       - data to write.
 *
 * Output:          TRUE if successfull
 *                  !TRUE if failed.
 *
 * Side Effects:    Original MPFS handle is no longer valid.
 *                  Updated MPFS handle must be obtained by calling
 *                  MPFSPutEnd().
 *
 * Overview:        None
 *
 * Note:            Actual write may not get started until internal write page is full.
 *                  To ensure that previously data gets written, caller must call MPFSPutEnd()
 *                  after last call to MPFSPut().
 *                  !!!! This function does NOT put MPFS_DLE and MPFS_EXT character in
 *                  the file - they must all already be there !!!!
 ********************************************************************/
BOOL MPFSPut(BYTE b)
{
#if defined(MPFS_USE_EEPROM)
    if ( XEEWrite(b) )
        return FALSE;

    _currentCount++;
    _currentHandle++;   //_currentHandle contains the address of the next byte to be written to.
    if ( _currentCount >= MPFS_WRITE_PAGE_SIZE )
    {
        MPFSPutEnd();
        XEEBeginWrite(EEPROM_CONTROL, _currentHandle);
    }
#endif
    return TRUE;
}

/*********************************************************************
 * Function:        MPFS MPFSPutEnd(void)
 *
 * PreCondition:    MPFSPutBegin() is already called.
 *
 * Input:           None
 *
 * Output:          Up-to-date MPFS handle = address of next byte to be written to, if any.
 *
 * Side Effects:    Original MPFS handle is no longer valid.
 *                  Updated MPFS handle must be obtained by calling
 *                  MPFSPutEnd().
 *
 * Overview:        None
 *
 * Note:            Actual write may not get started until internal
 *                  write page is full.  To ensure that previously
 *                  data gets written, caller must call MPFSPutEnd()
 *                  after last call to MPFSPut().
 ********************************************************************/
MPFS MPFSPutEnd(void)
{
#if defined(MPFS_USE_EEPROM)
    _currentCount = 0;
    XEEEndWrite();
    while( XEEIsBusy(EEPROM_CONTROL) );
#endif

    return _currentHandle;
}

/*********************************************************************
 * Function:        MPFS MPFSSeek(MPFS_OFFSET offset)
 *
 * PreCondition:    MPFSGetBegin() is already called.
 *
 * Input:           offset      - Offset from current pointer
 *
 * Output:          New MPFS handle located to given offset
 *
 * Side Effects:    None.
 *
 * Overview:        Returns the handle for given offset into current open file.
 *
 * Note:            None.
 ********************************************************************/
MPFS MPFSSeek(MPFS_OFFSET offset)
{
    WORD i;

    //Configures reading from current file
    MPFSGetBegin(_currentFile);

    i = 0;
    while(i++ != offset)
        MPFSGet();

    MPFSGetEnd();

    return _currentHandle;
}
