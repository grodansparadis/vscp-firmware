/******************************************************************************
 *
 *                      FSFILE I/O OPERATION API
 *
 ******************************************************************************
 * FileName:        FileIO.c
 * Dependencies:    See include below
 * Processor:       PIC24/Daytona
 * Compiler:        MPLAB C30 Compiler
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
 * $Id: fileio.c 161 2008-10-15 19:30:28Z akhe $
 *
 * $Name:  $
 *
 *****************************************************************************/
#include <fat16\fat.h>
#include <fat16\fileio.h>
#include <fat16\media.h>

FSFILE *gBufferOwner;
#ifndef FAT16_DYNAMIC_MEM
    FSFILE gblFileArray[FAT16_MAX_FILES_OPEN];
    BYTE gblFileSlotOpen[FAT16_MAX_FILES_OPEN];
#endif

// Globals
BYTE BSR;

PRIVATE char ShortAllowedChars[] = 
    { '$','%',0x27,'-','_','@','~','!','(',')','{','}','^','#','&' };

PRIVATE char defDOTDIR[]      =   ".          ";   // Keep spaces!
PRIVATE char defDOTDOTDIR[]   =   "..         ";


/******************************************************************************
 * Function:        BYTE CalcShortFNameCsum( char *name )
 *
 * PreCondition:    None   
 *
 * Input:           name - character string containing the filename
 *                  
 * Output:          One byte checksum
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to calculate the checksum
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE BYTE CalcShortFNameCsum( char *name )
{
    BYTE len, sum = 0;

    for( len = 11; len != 0; len-- )
       sum = ((sum & 1) ? 0x80: 0) + (sum >> 1) + *name++;

    return( sum );

}   // CalcShortFNameCsum()


/******************************************************************************
 * Function:        FILEOBJ GetFilePtr( void )
 *
 * PreCondition:    Either dynamic or static memory allocation is used
 *
 * Input:           None
 *                  
 * Output:          FILEOBJ - Pointer to file object or NULL if
 *                            unavailable 
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to obtain a file object
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE FILEOBJ GetFilePtr( void )
{
    FSFILE *pFile;
#ifndef FAT16_DYNAMIC_MEM
    int fIndex;
#endif

#ifdef FAT16_DYNAMIC_MEM
    pFile = (FSFILE *)FAT16_malloc( sizeof( FSFILE ) );
#else
    pFile = NULL;

    // Pick first available file structure
    for( fIndex = 0; fIndex < FAT16_MAX_FILES_OPEN; fIndex++ ) {
       if( gblFileSlotOpen[fIndex] ) {   // This slot available ?
          gblFileSlotOpen[fIndex] = FALSE;
          pFile = &gblFileArray[fIndex];
          break;
       }
    }
#endif

    if( !pFile )
       return NULL;   // No file structure slot available or FAT16_malloc() failed

    memset( pFile, 0, sizeof( FSFILE ) );

    return( pFile );

}   // GetFilePtr()


/******************************************************************************
 * Function:        void FreeFilePtr( FILEOBJ fo )
 *
 * PreCondition:    Either dynamic or static memory allocation is used
 *
 * Input:           fo - Pointer to file object to be freed (obtained
 *                       from GetFilePtr())
 *                  
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to free up a file object
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE void FreeFilePtr( FILEOBJ fo )
{
#ifndef FAT16_DYNAMIC_MEM
    WORD fIndex;
#endif

#ifdef FAT16_DYNAMIC_MEM
    FAT16_free( fo );
#else
    for( fIndex = 0; fIndex < FAT16_MAX_FILES_OPEN; fIndex++ ) {
       if( fo == &gblFileArray[fIndex] ) {
          // Put this slot back to the pool
          gblFileSlotOpen[fIndex] = TRUE;
          break;
       }
    }
#endif

}   // FreeFilePtr()


/******************************************************************************
 * Function:        void reverse( char *s )
 *
 * PreCondition:    None
 *
 * Input:           s - Pointer to character string 
 *                  
 * Output:          s  - Pointer to character string
 *
 * Side Effects:    None
 *
 * Overview:        Reverse string s in place 
 *
 * Note:            None
 *****************************************************************************/
PRIVATE void reverse( char *s )
{
    int c, i, j;
    
    for( i=0, j=(strlen( s )- 1); i<j; i++, j-- ) {
       c = s[i];
       s[i] = s[j];
       s[j] = c;
    }

}   // reverse()


/******************************************************************************
 * Function:        void itoa( int n, char *s )
 *
 * PreCondition:    None
 *
 * Input:           n - Integer value to be converted
 *                  
 * Output:          s - Character string representation of n
 *
 * Side Effects:    None
 *
 * Overview:        Convert integer n to character s
 *
 * Note:            None
 *****************************************************************************/
PRIVATE void itoa( int n, char *s )
{
    int i, sign;

    if( (sign = n) < 0 )
       n = -n;

    i = 0;
    do {
       s[i++] = n % 10 + '0';
    } while( ( n /= 10) > 0 );

    if( sign < 0 )
       s[i++] = '-';

    s[i] = '\0';

    reverse( s );

}   // itoa()


/******************************************************************************
 * Function:        BOOL InitFileObject( FILEOBJ fo, const char *name )
 *
 * PreCondition:    None
 *
 * Input:           fo - Pointer to file object
 *                  name - Character string containing the name
 *                  
 * Output:          TRUE - Operation successful
 *                  FALSE - Operation failed due to length of name is NULL
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to initialize the file object
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE BOOL InitFileObject( FILEOBJ fo, const char *name )
{
    WORD index, len;

    len = strlen( name );
    
    if( !len )
       return( FALSE );

    strcpy(fo->name, name);
    /***
    for( index = 0; index < len; index++ )
       fo->name[index] = name[index];
	***/
	
    if(!MediaStripRootName(fo->name))
        return FALSE;

    if(!MediaCallbackName((BYTE *)name, MEDIA_CMD_DISK, NULL, (void *)&fo->dsk))
        return FALSE;

    fo->cluster = 0;
    fo->ccls = 0;

    // Start at the root directory
    fo->dirclus = 0;
    fo->dirccls = 0;

    return( TRUE );

}   // InitFileObject()


/******************************************************************************
 * Function:        DIRENTRY CacheFileEntry( FILEOBJ fo, WORD *curEntry,
 *                                           BYTE ForceRead )
 *
 * PreCondition:    Disk mounted
 *
 * Input:           fo - File information
 *                  curEntry - Location of the sector
 *                  ForceRead - Forces loading of a new sector of the root
 *
 * Output:          DIRENTRY - Sector loaded
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to load the sector based on the 
 *                  file handle
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE DIRENTRY CacheFileEntry( FILEOBJ fo, WORD *curEntry, BYTE ForceRead )
{
    DIRENTRY dir;
    DISK *dsk;
    DWORD sector;
    WORD cluster;
    WORD ccls;
    BYTE offset2;
    BYTE numofclus;

    dsk = fo->dsk;

    // Get the base sector of this directory
    cluster = fo->dirclus;
    ccls = fo->dirccls;

    // Figure out the offset from the base sector
    offset2 = *curEntry >> 4;
    offset2 = offset2;   // Emulator issue

    // If it's the root, it's not cluster-based
    if( cluster )
       offset2 = offset2 % (dsk->SecPerClus);   // Figure out the offset

    // Check if a new sector of the root must be loaded
    if( ForceRead || !(*curEntry & 0xf) ) {   // Only 16 entries per sector

       // See if we have to load a new cluster
       if( (!offset2 && (*curEntry) > DIRENTRIES_PER_SECTOR) || ForceRead ) {
          if( !cluster ) {
             ccls = 0;
          } else {
             if( ForceRead )
                numofclus = ((WORD)(*curEntry) /
                            (WORD)(((WORD)DIRENTRIES_PER_SECTOR) *
                                    (WORD)dsk->SecPerClus));
             else
                numofclus = 1;

             // Move to the correct cluster
             while( numofclus ) {
                ccls = FATRead( dsk, ccls );

                if( ccls >= LAST_CLUSTER )
                   break;
                else
                   numofclus--;
             }   // while
          }
       }

       // See if we have a valid cluster number
       if( ccls < LAST_CLUSTER ) {
          fo->dirccls = ccls;   // Write it back

          sector = Cluster2Sector( dsk, ccls );

          // See if we are root and about to go pass our boundaries
          if( !ccls && ((sector + offset2) >= dsk->data) ) {
             dir = (DIRENTRY)NULL;   // Reached the end of the root!
          } else {
             gBufferOwner = NULL;

             if( !MediaReadSectorIdx(dsk->idx, sector + offset2, dsk->buffer ) )
                dir = (DIRENTRY)NULL;
             else {
                // Now figure out the pointer if its a forced read
                if( ForceRead )
                   dir = (DIRENTRY)((DIRENTRY)dsk->buffer) + ((*curEntry)%DIRENTRIES_PER_SECTOR);
                else
                   dir = (DIRENTRY)dsk->buffer;
             }
          }

       } else
          dir = (DIRENTRY)NULL;
    } else
       dir = (DIRENTRY)(((DIRENTRY)dsk->buffer) + (*curEntry)%DIRENTRIES_PER_SECTOR);

    return( dir );

}   // CacheFileEntry()


/******************************************************************************
 * Function:        DIRENTRY FillLongDir( FILEOBJ fo, DIRENTRY dir,
 *                                        WORD *fHandle )
 *
 * PreCondition:    
 *
 * Input:           
 *                  
 * Output:          
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to fill the long directory entry
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE DIRENTRY FillLongDir( FILEOBJ fo, DIRENTRY dir, WORD *fHandle )
{
    BOOL LastWasLong = FALSE;
    WORD LgNameIndex; 
    WORD index;
    char a;
    LDIR ldir;   // Long directory
    DISK *disk;
    
    disk = fo->dsk;
    a = dir->DIR_Attr; 
     
    while( a == ATTR_LONG_NAME ) {

       // Overcast the long directory structure
       ldir = (LDIR)dir;

       // Figure out where this long entry text is actually supposed to go, 
       // the count will tell us how many entries
       LgNameIndex = LDIR_NAMESIZE * ((ldir->LDIR_Ord-1) & LDIR_ORD_MASK);
        
       // See if we already have data
       if( !LastWasLong ) {

          // Clear it out first
          for( index = 0; index < FILE_NAME_SPACE_SIZE; index++ )
             fo->name[index] = (BYTE)'\0';	 				
        		
          LastWasLong = TRUE;     
        	
          // Now store the identifier, use the first entry
          fo->entry = *fHandle;

       }

       for( index = 0; index < 5; index++ )
          fo->name[LgNameIndex++] = ldir->LDIR_Name1[index];
        
       for( index = 0; index < 6; index++ )
          fo->name[LgNameIndex++] = ldir->LDIR_Name2[index];
            
       for( index = 0; index < 2; index++ )
          fo->name[LgNameIndex++] = ldir->LDIR_Name3[index];   
            
       // Get the next entry    
       (*fHandle)++;
       dir = CacheFileEntry( fo, fHandle, FALSE );     
        
       a = dir->DIR_Attr; 

    }   // while loop

    return( dir );

}   // FillLongDir()


/******************************************************************************
 * Function:        BYTE FillFileObject( FILEOBJ fo, WORD *fHandle )
 *
 * PreCondition:    Disk mounted
 *
 * Input:           fo - Pointer to file structure
 *                  fHandle - Passed member's location
 *
 * Output:          FS_FOUND - Operation successful
 *                  FS_NOT_FOUND - Operation failed
 *
 * Side Effects:    None
 *
 * Overview:        Fill the passed file object with the passed member's
 *                  (fHandle) information
 *
 * Note:            Should NOT be called by user
 *****************************************************************************/
PRIVATE BYTE FillFileObject( FILEOBJ fo, WORD *fHandle )
{
    DIRENTRY dir;
    BYTE index, a, i = 0;
    BYTE character, status;
    DWORD temp;

    // Get the entry
    dir = CacheFileEntry( fo, fHandle, FALSE );

    // Read the first char of the filename
    a = dir->DIR_Name[0];

    // Make sure there is a directory left
    if( (dir == (DIRENTRY)NULL) || (a == DIR_EMPTY) )
       status = FS_NO_MORE;

    else {

       // Check for empty or deleted directory
       if( a == DIR_DEL )
          status = FS_NOT_FOUND;

       else {
          // Get the attributes
          a = dir->DIR_Attr;

          // Long file name
          if( a == ATTR_LONG_NAME )
             dir = FillLongDir( fo, dir, fHandle );

          else {   // Short file name

             // 2.1.1  Print the file name and extension
             for( index = 0; index < DIR_NAMESIZE; index++ ) {
                character = (BYTE)toupper( dir->DIR_Name[index] );
                BSR = 0;

                // Remove spaces
                if( character != ' ' )
                   fo->name[i++] = character;
             }

             // Get the attributes
             a = dir->DIR_Attr;

             // It's possible to have an extension in a directory
             character = dir->DIR_Extension[0];
             BSR = 0;

             if( character != ' ' ) {
                fo->name[i++] = (BYTE)'.';

                // Get the file extension if it's there
                for( index = 0; index < DIR_EXTENSION; index++ ) {
                   character = (BYTE)toupper(dir->DIR_Extension[index] );
                   BSR = 0;

                   // Remove spaces
                   if( character != ' ' )
                      fo->name[i++] = character;
                }
             }

             // Done and done with the name
             fo->name[i] = (BYTE)'\0';

             // Now store the identifier
             fo->entry = *fHandle;

          }   // Short file name

          // See if we are still a good file
          // (long file names can erase just the main entry)
          a = dir->DIR_Name[0];
          if( a == DIR_DEL )
             status = FS_NOT_FOUND;
          else
             status = FS_FOUND;

          // Now store the size
          fo->size = dir->DIR_FileSize;

          // Now store the cluster
          temp = (dir->DIR_FstClusHI * 0x10000) | dir->DIR_FstClusLO;
          fo->cluster = temp;

          // Get the date and time
          fo->time = dir->DIR_WrtTime;
          fo->date = dir->DIR_WrtDate;

          // Get and store the attributes
          a = dir->DIR_Attr;
          fo->attributes = a;

       }   // Deleted directory

    }   // Ensure we are still good

    return( status );

}   // FillFileObject()


/******************************************************************************
 * Function:        CETYPE FileFind( FILEOBJ foDest, FILEOBJ foCompareTo,
 *                                   BYTE cmd )
 *
 * PreCondition:    Disk mounted function has been executed, foCompareTo 
 *                  loaded with filename
 *
 * Input:           foDest - FILEOBJ containing information of file found
 *                  foCompareTo	- FILEOBJ containing name of file to be found
 *                  cmd	-
 *                        1 - Search for a matching entry
 *                        2 - Search for an empty entry
 *
 * Output:          CE_GOOD - File found
 *                  CE_BADCACHEREAD - Operational failed due to bad
 *                                    cache read
 *                  CE_FILE_NOT_FOUND - File not found
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to find a file given a name as
 *                  passed via foCompareTo and place it in foDest
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE CETYPE FileFind( FILEOBJ foDest, FILEOBJ foCompareTo, BYTE cmd )
{
    WORD attrib, fHandle = 0;   // Current entry counter
    BYTE state, index;   // State of the current object
    CETYPE statusB = CE_FILE_NOT_FOUND;
    BYTE character, test;

    // Reset the cluster
    foDest->dirccls = foDest->dirclus;

    if(	CacheFileEntry( foDest, &fHandle, TRUE ) == NULL )
       statusB = CE_BADCACHEREAD;

    else {
       // Loop until you reach the end or find the file
       while( 1 ) {

          if( statusB != CE_GOOD ) {
             state = FillFileObject( foDest, &fHandle );
             if( state == FS_NO_MORE )
                break;

          } else
             break;

          if( state == FS_FOUND ) {   // We found something

             // Get the attributes
             attrib = foDest->attributes & ATTR_LONG_NAME_MASK;

             // See if we are a volume id or hidden, ignore
             if( (attrib != ATTR_VOLUME_ID) && ((attrib & ATTR_HIDDEN) != ATTR_HIDDEN) ) {
                statusB = CE_GOOD;
                character = (BYTE)'m';   // Random value

                // Search for one, if status=TRUE then we found one
                for( index = 0; (statusB == CE_GOOD) &&
                                (foDest->name[index] != '\0');
                     index++ ) {
                   // Get the source character
                   character = foDest->name[index];

                   // Get the destination character
                   test = foCompareTo->name[index];

                   if( tolower( character ) != tolower( test ) )
                      statusB = CE_FILE_NOT_FOUND;   // No it's not a match

                   BSR = 0;
                }   // for-loop

             }   // Not dir nor vol

          } else {   // Not found

             // Looking for an empty/re-usable entry
            if( cmd == 2 )
               statusB = CE_GOOD;

          }   // Found or not

          // Just increment it no matter what happened
          fHandle++;

       }   // while-loop
    }

    return( statusB );

}   // FileFind()


/******************************************************************************
 * Function:        void IncrementTimeStamp( DIRENTRY dir )
 *
 * PreCondition:    File opened
 *
 * Input:           dir - Pointer to directory structure
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Touch the time so we can keep track of the files
 *
 * Note:            None
 *****************************************************************************/
PRIVATE void IncrementTimeStamp( DIRENTRY dir )
{
    BYTE seconds;
    BYTE minutes;
    BYTE hours;
    BYTE day;
    BYTE month;
    BYTE year;

    seconds = (dir->DIR_WrtTime & 0x1f);
    minutes = ((dir->DIR_WrtTime & 0x07E0) >> 5);
    hours = ((dir->DIR_WrtTime & 0xF800) >> 11);

    day = (dir->DIR_WrtDate & 0x1f);
    month = ((dir->DIR_WrtDate & 0x01E0) >> 5);
    year = ((dir->DIR_WrtDate & 0xFE00) >> 9);

    if( seconds < 29 )
       seconds++;

    else {
       seconds = 0x00;

        if( minutes < 59 )
           minutes++;

        else {
            minutes = 0;

            if( hours < 23 )
                hours++;

            else {
                hours = 0;
                // New day!... only 28 in a month
                if( day < 28 )
                    day++;

                else {
                    day = 1;

                    if( month < 12 )
                        month++;

                    else {
                        month = 1;
                        // New year
                        year++;
                        // HOPEFULLY we never go past 2107
                    }
                }
            }
        }
    }

    dir->DIR_WrtTime = (WORD)(seconds);
    dir->DIR_WrtTime |= ((WORD)(minutes) << 5);
    dir->DIR_WrtTime |= ((WORD)(hours) << 11);

    dir->DIR_WrtDate = (WORD)(day);
    dir->DIR_WrtDate |= ((WORD)(month) << 5);
    dir->DIR_WrtDate |= ((WORD)(year) << 9);

}   // IncrementTimeStamp()


/******************************************************************************
 * Function:        DIRENTRY LoadDirAttrib( FILEOBJ fo, WORD *fHandle )
 *
 * PreCondition:    Disk mounted
 *
 * Input:           fo - Pointer to file structure
 *                  fHandle - Information location
 *
 * Output:          DIRENTRY - The directory entry
 *
 * Side Effects:    None
 *
 * Overview:        Load the current sector for the filename base and return
 *                  the direntry
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE DIRENTRY LoadDirAttrib( FILEOBJ fo, WORD *fHandle )
{
    DIRENTRY dir;
    BYTE a;

    // Get the entry
    dir = CacheFileEntry( fo, fHandle, TRUE );

    // Read the first char of the file name
    a = dir->DIR_Name[0];

    // Make sure there is a directory left
    // Check for empty or deleted directory
    if( (a == DIR_EMPTY) || (a == DIR_DEL) )
       dir = (DIRENTRY)NULL;

    else {

       // Get the attributes
       a = dir->DIR_Attr;

       // Scan through all the long dir entries
       while( a == ATTR_LONG_NAME ) {
          (*fHandle)++;
          dir = CacheFileEntry( fo, fHandle, FALSE );
          a = dir->DIR_Attr;
       }   // Long file name while loop


    }

    return( dir );

}   // LoadDirAttrib()


/******************************************************************************
 * Function:        CETYPE FileAllocateNewCluster( FILEOBJ fo )
 *
 * PreCondition:    Disk mounted
 *
 * Input:           fo - Pointer to file structure
 *
 * Output:          CE_GOOD - Cluster allocated successfully
 *                  CE_DISK_FULL - Operation failed due to media is full
 *                  CE_WRITE_ERROR - Operation failed due to media write
 *                                   failed
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to allocate new cluster
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE CETYPE FileAllocateNewCluster( FILEOBJ fo )
{
    DISK *dsk;
    WORD c, curcls;

    dsk = fo->dsk;
    c = fo->ccls;

    // Find the next empty cluster
    c = FATFindEmptyCluster( fo );
    if( !c )
       return CE_DISK_FULL;

    // Mark the cluster as taken, and last in chain
    if( dsk->type == FAT12 )
       FATWrite( dsk, c, LAST_CLUSTER_FAT12 );
    else
       FATWrite( dsk, c, LAST_CLUSTER_FAT16 );

    // Link current cluster to the new one
    curcls = fo->ccls;

    FATWrite( dsk, curcls, c );

    // Update the FSFILE structure
    fo->ccls = c;

    return CE_GOOD;

}   // FileAllocateNewCluster()


/******************************************************************************
 * Function:        BOOL FindEmptyEntries( FILEOBJ fo, WORD *fHandle,
 *                                         BYTE numEntries )
 *
 * PreCondition:    Disk mounted, CreateFileEntry() called
 *
 * Input:           fo - Pointer to file structure
 *                  fHandle - Location of the file
 *                  numEntries - Number of directory entries
 *
 * Output:          TRUE - One found
 *                  FALSE - None found
 *
 * Side Effects:    None
 *
 * Overview:        Find the passed number of contiguous empty entries
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE BOOL FindEmptyEntries( FILEOBJ fo, WORD *fHandle, BYTE numEntries )
{
    BYTE status = FS_NOT_FOUND;
    BYTE amountfound;
    BYTE a;
    WORD bHandle;
    DIRENTRY dir;

    if( (dir = CacheFileEntry( fo, fHandle, TRUE )) == NULL ) {
       status = CE_BADCACHEREAD;

    } else {

       // while its still not found
       while( status == FS_NOT_FOUND ) {
          amountfound = 0;
          bHandle = *fHandle;

          // Find (numEntries) continuous entries
          do {
             // Get the entry
             dir = CacheFileEntry( fo, fHandle, FALSE );

             // Read the first char of the file name
             a = dir->DIR_Name[0];

             // Increase number
             (*fHandle)++;
             BSR=0;
          } while( ((a == DIR_DEL) || (a == DIR_EMPTY)) &&
                   (dir != (DIRENTRY)NULL) &&
                   (++amountfound < numEntries) );

          // --- Now why did we exit?
          if( !dir ) {   // Last entry of the cluster

             // Setup the current cluster
             a = fo->dirccls;   // write it back

             // Make sure we are not the root directory
             if( !a )
                status = FS_NO_MORE;
             else {
                fo->ccls = a;

                if( FileAllocateNewCluster( fo ) == CE_DISK_FULL )
                   status = FS_NO_MORE;
                else {
                   *fHandle = bHandle;
                   status = FS_FOUND;   // A new cluster will surely hold a new file name
                }
             }

          } else {

                if( amountfound == numEntries ) {
                   status = FS_FOUND;
                   *fHandle = bHandle;
                }

          }

       }   // while

    	// Copy the base handle over
    	*fHandle = bHandle;

    }

    if( status == FS_FOUND )
       return( TRUE );
    else
       return( FALSE );

}   // FindEmptyEntries()


/******************************************************************************
 * Function:        BOOL FindShortName( FILEOBJ fo, char *name )
 *
 * PreCondition:    
 *
 * Input:           fo - Pointer to file object
 *                  name - Character string containing the name
 *                  
 * Output:          TRUE - Operation successful
 *                  FALSE - Operation failed
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to find the short name representation
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE BOOL FindShortName( FILEOBJ fo, char *name )
{
    BYTE status = FS_NOT_FOUND;
    WORD fHandle;
    BYTE a, index, character;
    DIRENTRY dir;

    dir = CacheFileEntry( fo, &fHandle, TRUE );

    while( (status != FS_NO_MORE) && (status != FS_NOT_FOUND) ) {

       // Get the entry
       dir = CacheFileEntry( fo, &fHandle, FALSE );

       // Read the first character of the filename
       a = dir->DIR_Name[0];

       // Make sure there is a directory available
       if( (dir == (DIRENTRY)NULL) || (a == DIR_EMPTY) ) {
          status = FS_NO_MORE;
       } else {
          // Check for empty or deleted directory
          if( a == DIR_DEL )
             status = FS_NOT_FOUND;
          else {
             // Get the attributes
             a = dir->DIR_Attr;
	     
             // Skip long entry names
             if( a != ATTR_LONG_NAME ) {
                for( index = 0; index < DIR_NAMECOMP; index++ ) {
                   character = (BYTE)toupper( dir->DIR_Name[index] );
                   BSR = 0;
                   if( character != toupper( name[index] ) )
                      break;
                }

                if( index == DIR_NAMECOMP )
                   status = FS_FOUND;

             }   // Not long name
          }   // Not deleted
       }  // Not no more

       fHandle++;

    }   // while loop

    if( status == FS_FOUND )
       return( TRUE );
    else
       return( FALSE );

}   // FindShortName()


/******************************************************************************
 * Function:        BOOL WriteFileEntry( FILEOBJ fo, WORD *curEntry )
 *
 * PreCondition:    Disk mounted, fo contains valid file data
 *
 * Input:           fo - File structure
 *                  curEntry - Write destination
 *
 * Output:          TRUE - Operation successful
 *                  FALSE - Operation failed
 *
 * Side Effects:    None
 *
 * Overview:        Write the buffer into the current entry
 *
 * Note:            None
 *****************************************************************************/
PRIVATE BOOL WriteFileEntry( FILEOBJ fo, WORD *curEntry )
{
    DISK *dsk;
    BYTE status;
    BYTE offset2;
    DWORD sector;
    WORD ccls;

    dsk = fo->dsk;

    // Get the cluster of this entry
    ccls = fo->dirccls;

    offset2 = (*curEntry >> 4);

    // If it's not the root, it works cluster based
    if( ccls )
       offset2 = offset2 % (dsk->SecPerClus);

    sector = Cluster2Sector( dsk, ccls );

    // Now write it
    if( !MediaWriteSectorIdx(dsk->idx, sector + offset2, dsk->buffer ) )
       status = FALSE;
    else
       status = TRUE;

    return( status );

}   // WriteFileEntry()


/******************************************************************************
 * Function:        CETYPE PopulateEntries( FILEOBJ fo, char *name, WORD *fHandle,
 *                                          BYTE numDirEntries, BYTE size )
 *
 * PreCondition:    Disk mounted, CreateFileEntry() called
 *
 * Input:           fo - Pointer to file structure
 *                  name - Name of the file
 *                  fHandle - Location of the file
 *                  numDirEntries - Number of directory entries
 *                  size - Size of filename
 *
 * Output:          CE_GOOD - Populate successful
 *
 * Side Effects:    None
 *
 * Overview:        Fill in the directory entry with the data at hand and write it
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE CETYPE PopulateEntries( FILEOBJ fo, char *name, WORD *fHandle,
                                BYTE numDirEntries, BYTE size )
{
    BYTE error = CE_GOOD;
    BYTE temp, index, csum;
    DIRENTRY dir;
    LDIR ldir;   // Long directory

    dir = CacheFileEntry( fo, fHandle, TRUE );

    ldir = (LDIR)dir;

    // Start from the top
    size--;

    // Figure out how many spaces to add
    temp = LDIR_NAMESIZE - (size % LDIR_NAMESIZE) - 1;

    // Figure out the csum
    csum = CalcShortFNameCsum( name );

    numDirEntries--;

    // Setup the first directory
    if( numDirEntries ) {
       ldir->LDIR_Ord = LDIR_LAST | numDirEntries;
    }

    while( numDirEntries ) {

       for( index = 2; index > 0; index-- ) {
           if( temp ) {
              ldir->LDIR_Name3[index-1] = ((temp == 1) ? (UNICODE)'\0' : 0xFFFF);
              temp--;
           } else {
              ldir->LDIR_Name3[index-1] = fo->name[size--];
           }
       }

       for( index = 6; index > 0; index-- ) {
           if( temp ) {
              ldir->LDIR_Name2[index-1] = ((temp == 1) ? (UNICODE)'\0' : 0xFFFF);
              temp--;
           } else {
              ldir->LDIR_Name2[index-1] = fo->name[size--];
           }
       }

       for( index = 5; index > 0; index-- ) {
           if( temp ) {
              ldir->LDIR_Name1[index-1] = ((temp == 1) ? (UNICODE)'\0' : 0xFFFF);
              temp--;
           } else {
              ldir->LDIR_Name1[index-1] = fo->name[size--];
           }
       }

       // Setup everything else
       ldir->LDIR_Attr = ATTR_LONG_NAME;
       ldir->LDIR_Type = 0;
       ldir->LDIR_Chksum = csum;
       ldir->LDIR_FstClusLO = 0;

       // Now write that entry in
       if( !WriteFileEntry( fo, fHandle ) )
          return( CE_WRITE_ERROR );

       (*fHandle)++;
       ldir = (LDIR)CacheFileEntry( fo, fHandle, FALSE );

       // Setup the order
       if( numDirEntries > 1 )
          ldir->LDIR_Ord = --numDirEntries;
       else {
          numDirEntries--;
          dir = (DIRENTRY)ldir;
       }

    }   // while loop

    // Copy the contents over
    strncpy( dir->DIR_Name, name, DIR_NAMECOMP );

    // Setup no attributes
    dir->DIR_Attr = ATTR_ARCHIVE;
    dir->DIR_NTRes = 0x00; // NT reserved

    // We don't have a real time clock
    dir->DIR_CrtTimeTenth = 0x00;  // Millisecond stamp
    dir->DIR_CrtTime = 0x2280;     // Time created
    dir->DIR_CrtDate = 0x3094;     // Date created (1/1/2004)
    dir->DIR_LstAccDate = 0x3094;  // Last Access date
    dir->DIR_FstClusHI = 0x00;     // High word of this enty's first cluster number
    dir->DIR_WrtTime = 0x2280;     // Last update time
    dir->DIR_WrtDate = 0x3094;     // Last update date
    dir->DIR_FstClusLO = 0x0000;   // Low word of this entry's first cluster number
    dir->DIR_FileSize = 0x0;       // File size in DWORD

    fo->size = dir->DIR_FileSize;
    fo->time = dir->DIR_CrtTime;
    fo->date = dir->DIR_CrtDate;
    fo->attributes = dir->DIR_Attr;
    fo->entry = *fHandle;

    // Just write the last entry in
    if( !WriteFileEntry( fo, fHandle ) )
       error = CE_WRITE_ERROR;

    return( error );

}   // PopulateEntries()


/******************************************************************************
 * Function:        void FileObjectCopy( FILEOBJ foDest, FILEOBJ foSource )
 *
 *
 * Input:           foDest - Destination buffer
 *                  foSource - Source buffer
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to make a replica of an existing
 *                  file object
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE void FileObjectCopy( FILEOBJ foDest, FILEOBJ foSource )
{
    WORD i, size;
    BYTE *dest, *source;

    dest = (BYTE *)foDest;
    source = (BYTE *)foSource;

    size = sizeof( FSFILE );

    for( i=0; i<size; i++ )
       dest[i] = source[i];

}   // FileObjectCopy()


/******************************************************************************
 * Function:        BOOL IsShortNameAcceptable( char character )
 *
 * PreCondition:    Called from Format8_3Char()
 *
 * Input:           character - Character to verify
 *                  
 * Output:          TRUE - Operation successful
 *                  FALSE - Operation failed
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to verify that a character is valid
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE BOOL IsShortNameAcceptable( char character )
{
    BYTE index;
    BOOL status = FALSE;
    
    for( index = 0; (index < sizeof( ShortAllowedChars )) && (status == FALSE);
         index++ ) {
       if( ShortAllowedChars[index] == character )
          status = TRUE;
    }
     
    return( status );   

}   // IsShortNameAcceptable()


/******************************************************************************
 * Function:        BOOL Format8_3Char( char character, char *strName,
 *                                      BYTE *bn )
 *
 * PreCondition:    Called from ConstructShortName()    
 *
 * Input:           
 *                  
 * Output:          TRUE - Operation successful
 *                  FALSE - Operation failed
 *
 * Side Effects:    None
 *
 * Overview:        
 *
 * Note:            Should not be called by user
*****************************************************************************/
PRIVATE BOOL Format8_3Char( char character, char *strName, BYTE *bn )
{
    BOOL isLong = FALSE;

    if( isalnum( character ) )
       strName[(*bn)++] = toupper( character );
    else {

       if( IsShortNameAcceptable( character ) )
          strName[(*bn)++] = character;
       else {

          // Transpond all extended characters except . to underscores
          if( character != '.' && character != ' ' ) {
             // Convert to '_'
             strName[(*bn)++] = (char)'_';
          }

          isLong = TRUE;   // Cannot be a short anymore

       }

    }
	
    return( isLong );

}   // Format8_3Char()


/******************************************************************************
 * Function:        BOOL ConstructShortName( FILEOBJ fo, char *strName,
 *                                           BYTE *nameSize )
 *
 * PreCondition:    Called from Create8Dot3Name()    
 *
 * Input:           fo - Pointer to file object
 *                  strName - Character string of the filename
 *                  nameSize - Pointer to size of the filename
 *                  
 * Output:          TRUE - Operation successful
 *                  FALSE - Operation failed
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to construct a short name for passed
 *                  in name
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE BOOL ConstructShortName( FILEOBJ fo, char *strName, BYTE *nameSize )
{
    BYTE character;
    BYTE index, bn = 0;
    BYTE loc = 0;
    BOOL possibleShort = TRUE;
        
    // Find the dot and the size
    for( index = 0; index < FILE_NAME_SIZE; index++ ) {
       character = fo->name[index];
        
       // Find the last dot
       if( character == '.' )
          loc = index;
                
       // If it's the last entry
       if( character == '\0' )    
          break;        
    }
    
    *nameSize = index;

    // If we have an extension, reset the pointer
    if( loc ) {
       // Make sure the dot is not at the end
       if( loc == *nameSize )
          loc = 0;
    }

    // Now get the extension        
    for( index = 0; index < DIR_EXTENSION; index++ ) {
       if( loc ) {
          character = fo->name[loc+1+index];
          bn = index + DIR_NAMESIZE;

          // Make sure we are not at the end of it
          if( (loc + index + 1) < *nameSize ) {
             if( Format8_3Char( (char)character, strName, &bn ) )
                possibleShort = FALSE;
                // Don't worry, the only place bn does not inc is
                // for a '.' and we are passed it			
          } else
             strName[DIR_NAMESIZE + index] = ' ';

       } else   // loc == 0
            strName[DIR_NAMESIZE + index] = ' ';
    }
     
    // Create the name basis
    bn = 0;
	
    // If there was no extension
    if( !loc ) {

       loc = *nameSize;
       if( *nameSize > DIR_NAMESIZE )
          possibleShort = FALSE;

    } else {   // loc != 0

          // See if its a long extension (3 ext + dot)
          if( loc < (*nameSize - 4) )
             possibleShort = FALSE;			
          else {
             // Check if base name is too big
             if( loc > DIR_NAMESIZE )
                possibleShort = FALSE;
          }

    }

    for( index = 0; (index < loc) && (bn < DIR_NAMESIZE); index++ ) {
       character = fo->name[index];   

       // Format to 8.3 spec
       if( Format8_3Char( (char)character, strName, &bn ) )
          possibleShort = FALSE;
    }
    
    // Fill in the rest with blanks
    while( bn < DIR_NAMESIZE )
       strName[bn++] = ' ';

    return( possibleShort );

}   // ConstructShortName()


/******************************************************************************
 * Function:        CETYPE Create8Dot3Name( FILEOBJ fo, char *name, BYTE *size,
 *                                          BOOL *isShort )
 *
 * PreCondition:    Called from CreateFileEntry()    
 *
 * Input:           fo - Pointer to file object
 *                  name - Character string containing the filename
 *                  size - Pointer to size of the filename
 *                  isShort - Pointer to flag which indicates if the
 *                            filename is considered short or long
 *                  
 * Output:          CE_GOOD - 8.3 filename representation created
 *                            successfully
 *                  CE_FILENAME_EXISTS - Operation failed due to
 *                                       filename already exists
 *                                       for short name only
 *                  CE_NO_MORE_TAILS - Operation failed due to
 *                                     inability to create tail
 *                                     for long name only
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to create an 8.3 representation of the
 *                  passed in name for the file     
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE CETYPE Create8Dot3Name( FILEOBJ fo, char *name, BYTE *size,
                                BOOL *isShort )
{
    CETYPE error = CE_GOOD;
    BOOL found = TRUE;
    BYTE index, l, seek = 0, tmp;
    char tail[4];

    // Figure out the 8.3 namebase for this file, update the size of the
    // filename, and return CE_GOOD, if the name is truly short.
    *isShort = ConstructShortName( fo, name, size );
    if( *isShort == FALSE ) {

       for( index = 1; index && (found == TRUE); index++ ) {

          itoa( (int)index, tail );
          l = strlen( tail );

          if( !seek ) {

             for( seek = 0; seek < DIR_NAMESIZE; seek++ ) {
                if( name[seek] == ' ' ) 
                   break;
             }

          }

          tmp = DIR_NAMESIZE - (l+1);
          if( seek > tmp )
             seek = tmp;

          name[seek] = '~';
          strncpy( &name[seek+1], tail, l );

          found = FindShortName( fo, name );
	  
       }   // for loop

       if( !index )
          error = CE_NO_MORE_TAILS;

    } else {   // *isShort == TRUE

       found = FindShortName( fo, name );
       if( found == TRUE )
          error = CE_FILENAME_EXISTS;
    }

    return( error );

}   // Create8Dot3Name()


/******************************************************************************
 * Function:        CETYPE FileErase( FILEOBJ fo, WORD *fHandle,
 *                                    BYTE EraseClusters )
 *
 * PreCondition:    File not opened, File exists
 *
 * Input:           fo - Pointer to file structure
 *                  fHandle - Location of file information
 *                  EraseClusters - Remove cluster allocation from FAT?
 *
 * Output:          CE_GOOD - File erased successfully
 *                  CE_FILE_NOT_FOUND - Could not find the file on the card
 *                  CE_ERASE_FAIL - Internal Card erase failed
 *
 * Side Effects:    None
 *
 * Overview:        Erase file
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE CETYPE FileErase( FILEOBJ fo, WORD *fHandle, BYTE EraseClusters )
{
    DIRENTRY dir;
    BYTE a;
    CETYPE status = CE_GOOD;
    WORD clus;
    DISK *disk;

    disk = fo->dsk;

    // Reset the cluster
    fo->dirccls = fo->dirclus;

    // Load the sector
    dir = CacheFileEntry( fo, fHandle, TRUE );

    // Fill up the File Object with the information pointed to by fHandle
    a = dir->DIR_Name[0];

    // See if there is something in the dir
    if( !dir || (a == DIR_EMPTY) ) {
       status = CE_FILE_NOT_FOUND;

     } else {
       // Check for empty or deleted directory
       if( a == DIR_DEL)
          status = CE_FILE_NOT_FOUND;

       else {
          // Get the attributes
          a = dir->DIR_Attr;

          // Long File Name entry
          while( (a == ATTR_LONG_NAME) && (status == CE_GOOD) ) {
             dir->DIR_Name[0] = DIR_DEL;   // Mark as deleted

             if( !WriteFileEntry( fo, fHandle ) )
                status = CE_ERASE_FAIL;

             // Get the next entry
             (*fHandle)++;
             dir = CacheFileEntry( fo, fHandle, FALSE );

             a = dir->DIR_Attr;
          }

          // Short File Name entry
          dir->DIR_Name[0] = DIR_DEL;   // Mark as deleted

          // Get the starting cluster
          clus = dir->DIR_FstClusLO;

          // Now write it
          if( (status != CE_GOOD) || !(WriteFileEntry( fo, fHandle )) )
             status = CE_ERASE_FAIL;

          else {
             if( EraseClusters ) {
                // Now remove the cluster allocation from the FAT
                status = ((FATEraseClusterChain( clus, disk )) ? CE_GOOD : CE_ERASE_FAIL);
             }

          }

       }   // Not deleted

    }   // Not empty

    return( status );

}   // FileErase()


/******************************************************************************
 * Function:        CETYPE FileCreateHeadCluster( FILEOBJ fo, WORD *cluster )
 *
 * PreCondition:    Disk mounted
 *
 * Input:           fo - Pointer to file structure
 *                  cluster - Cluster location
 *
 * Output:          CE_GOOD - Success
 *                  CE_WRITE_ERROR - Sector write failed
 *                  CE_DISK_FULL - All clusters in partition are taken
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to create the head cluster (1st one)
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE CETYPE FileCreateHeadCluster( FILEOBJ fo, WORD *cluster )
{
    DISK *disk;
    CETYPE error = CE_GOOD;

    disk = fo->dsk;

    // Find the next empty cluster
    *cluster = FATFindEmptyCluster( fo );

    if( !(*cluster) )
       error = CE_DISK_FULL;

    else {

       // Mark the cluster as taken, and last in chain
       if( disk->type == FAT12 ) {
          if( FATWrite( disk, *cluster, LAST_CLUSTER_FAT12 ) == (WORD)FAIL )
             error = CE_WRITE_ERROR;
       } else {
          if( FATWrite( disk, *cluster, LAST_CLUSTER_FAT16 ) == (WORD)FAIL )
             error = CE_WRITE_ERROR;
       }

        // Let's erase this cluster
       if( error == CE_GOOD ) {
          error = EraseCluster( disk, *cluster );
       }

    }

    return( error );

}   // FILECreateHeadCluster()


/******************************************************************************
 * Function:        CETYPE CreateFirstCluster( FILEOBJ fo )
 *
 * PreCondition:    Called from CreateFileEntry()
 *
 * Input:           fo	- The file that contains the first cluster
 *
 * Output:          CE_GOOD - First cluster created
 *                  CE_WRITE_ERROR - Cluster creation failed
 *
 * Side Effects:    None
 *
 * Overview:        Find an open cluster and link it in
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE CETYPE CreateFirstCluster( FILEOBJ fo )
{
    CETYPE    error;
    WORD      cluster;
    WORD      fHandle;
    DIRENTRY  dir;

    fHandle = fo->entry;

    // Now create the first cluster (head cluster)
    error = FileCreateHeadCluster( fo, &cluster );
    if( error == CE_GOOD )
    {
       // Load it so I can link in the new cluster
       dir = LoadDirAttrib( fo, &fHandle );

       // Now update the new cluster
       dir->DIR_FstClusLO = cluster;

       // now write it
       if( WriteFileEntry( fo, &fHandle ) != TRUE )
          error = CE_WRITE_ERROR;
    }

    return( error );

}   // CreateFirstCluster()


/******************************************************************************
 * Function:        CETYPE FileGetNextCluster( FILEOBJ fo, WORD n )
 *
 * PreCondition:    Disk mounted, fo contains valid file data
 *
 * Input:           fo - File structure
 *                  n - number of links in the FAT cluster chain to jump
 *                      through; n == 1 - next cluster in the chain
 *
 * Output:          CE_GOOD - Operation successful
 *                  CE_BAD_SECTOR_READ - A bad sector read occurred
 *                  CE_INVALID_CLUSTER - Invalid cluster value > maxcls
 *                  CE_FAT_EOF - FAT attempt to read beyond EOF
 *
 *
 * Side Effects:    None
 *
 * Overview:        Steps through a chain of clusters
 *
 * Note:            None 
 *****************************************************************************/
PRIVATE CETYPE FileGetNextCluster( FILEOBJ fo, WORD n )
{
    WORD c, c2;
    BYTE error = CE_GOOD;
    DISK *disk;
 
    disk = fo->dsk;
 
    // Loop n times
    do {
       // Get the next cluster link from FAT
       c2 = fo->ccls;

       if( (c = FATRead( disk, c2 )) == CLUSTER_FAIL )
          error = CE_BAD_SECTOR_READ;

       else {
          // Check if cluster value is valid
          if( c >= disk->maxcls )
             error = CE_INVALID_CLUSTER;

             // Compare against max value of a cluster in FAT table
             c2 = LAST_CLUSTER;

             // Return if EOF
             if( c >= c2 )   // Check against EOF
                error = CE_FAT_EOF;
       }
 
       // Update the FSFILE structure
       fo->ccls = c;
 
    } while( (--n > 0) && (error == CE_GOOD) );   // Loop end
 
 
    return( error );
 
}   // FileGetNextCluster()


/******************************************************************************
 * Function:        CETYPE CreateFileEntry( FILEOBJ fo, WORD *fHandle )
 *
 * PreCondition:    File opened in WRITE or APPEND (if file does not
 *                  exist) mode
 *
 * Input:           fo - Pointer to file structure
 *                  fHandle - Location to create file
 *
 * Output:          CE_GOOD - File creation successful
 *                  CE_DIR_FULL - All root dir entry are taken/full
 *
 * Side Effects:    None
 *
 * Overview:        With the data passed within fo, create a new file
 *                  entry in the current directory
 *
 * Note:            None
 *****************************************************************************/
PRIVATE CETYPE CreateFileEntry( FILEOBJ fo, WORD *fHandle )
{
    BYTE size;
    BOOL isShort;
    CETYPE error = CE_GOOD;
    BYTE numDirEntries;
    char name[12];

    // Come up with 8.3 representation
    error = Create8Dot3Name( fo, name, &size, &isShort );

    // Make sure we are good
    if( error == CE_GOOD ) {

       // Always at least 1 entry
       if( isShort )
          numDirEntries = 1;
       else
          numDirEntries = 1 + ((size+(LDIR_NAMESIZE-1)) / LDIR_NAMESIZE);

       *fHandle = 0;

       // Figure out where to put this file in the directory stucture
       error = FindEmptyEntries( fo, fHandle, numDirEntries );
       if( error ) {

          // Found an empty entry now populate it
          error = PopulateEntries( fo, name, fHandle, numDirEntries, size );
          if( error == CE_GOOD )
             // If everything is a go, create a first cluster
             error = CreateFirstCluster( fo );

       } else
          // Directory is full
          error = CE_DIR_FULL;

    }

    return( error );

}   // CreateFileEntry()


/******************************************************************************
 * Function:        CETYPE __fread( FILEOBJ fo, void *pDest, WORD count,
 *                                  WORD *pBytesRead )
 *
 * PreCondition:    Called from fread()
 *
 * Input:           fo - Pointer to file object
 *                  pDest - Pointer to destination buffer
 *                  count - Number of bytes to read
 *                  pBytesRead - Pointer to number of bytes read
 *
 * Output:          CE_GOOD - Operation successful
 *                  CE_EOF - Operation successful
 *                  CE_NOT_PRESENT - Operation failed due to media
 *                                   not detected
 *                  CE_WRITEONLY - Operation failed due to file
 *                                 in "w" mode of operation
 *                  CE_BAD_SECTOR_READ - Operation failed due to
 *                                 failure in reading the media
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to read data from a file into a
 *                  destination buffer
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE CETYPE __fread( FSFILE *fo, void *pDest, WORD count, WORD *pBytesRead )
{
    DISK *dsk;   // Disk structure
    DWORD seek, sec_sel;
    WORD pos;   // Position within sector
    CETYPE error = CE_GOOD;

    dsk = (DISK *)fo->dsk;
    pos = fo->pos;
    seek = fo->seek;

    *pBytesRead = 0;

    if( fo->Flags.write )
       return( CE_WRITEONLY );

    // If it's not my buffer, then get it from the disk.
    if( (gBufferOwner != fo) && (pos != MEDIA_SECTOR_SIZE) ) {
       gBufferOwner = fo;
       sec_sel = Cluster2Sector( dsk, fo->ccls );
       sec_sel += (WORD)fo->sec;   // Add the sector number to it

       if( !MediaReadSectorIdx(dsk->idx,  sec_sel, dsk->buffer ) )
          return( CE_BAD_SECTOR_READ );
    }else
    {
        sec_sel = Cluster2Sector( dsk, fo->ccls );
        sec_sel += (WORD)fo->sec;   // Add the sector number to it
    }
    
    // Loop reading (count) bytes
    while( count ) {

       if( seek >= fo->size ) {
          error = CE_EOF;
          break;
       }

       // In fopen, pos is init to 0 and the sect is loaded
       if( pos == MEDIA_SECTOR_SIZE ) {

            // Reset position
            pos = 0;

            // Point to the next sector
            fo->sec++;
            sec_sel++;

            // Get a new cluster if necessary
            if( fo->sec == dsk->SecPerClus ) 
            {
               fo->sec = 0;
               if( (error = FileGetNextCluster( fo, 1 )) != CE_GOOD )
                  break;

               sec_sel = Cluster2Sector( dsk, fo->ccls );
            }


            gBufferOwner = fo;

            if((count >= MEDIA_SECTOR_SIZE) && (fo->size > (seek + MEDIA_SECTOR_SIZE)))
            {
                if( !MediaReadSectorIdx(dsk->idx, sec_sel, pDest ) ) 
                {
                   error = CE_BAD_SECTOR_READ;
                   break;
                }
                
                pDest           += MEDIA_SECTOR_SIZE;
                seek            += MEDIA_SECTOR_SIZE;
                (*pBytesRead)   += MEDIA_SECTOR_SIZE;
                count           -= MEDIA_SECTOR_SIZE;
                pos             += MEDIA_SECTOR_SIZE;
                continue;
            }

            if( !MediaReadSectorIdx(dsk->idx, sec_sel, dsk->buffer ) ) 
            {
               error = CE_BAD_SECTOR_READ;
               break;
            }

        }

        {
            DWORD temp;

            if((seek + (MEDIA_SECTOR_SIZE - pos)) > fo->size)
               temp = fo->size - (seek + (MEDIA_SECTOR_SIZE - pos));
            else
               temp = (MEDIA_SECTOR_SIZE - pos);

            if(temp > count)
                temp = count;

            memcpy(pDest, &dsk->buffer[pos], temp);

            pDest += temp;
            seek += temp;
            (*pBytesRead) += temp;
            count -= temp;
            pos += temp;

        }

    }   // while loop

    // Save off the position
    fo->pos = pos;

    // Save off the seek
    fo->seek = seek;

    return( error );

}   // __fread()


/******************************************************************************
 * Function:        CETYPE __fclose( FILEOBJ fo )
 *
 * PreCondition:    Called from fclose()
 *
 * Input:           fo - Pointer to file object 
 *                  
 * Output:          CE_GOOD - Operation successful
 *                  CE_WRITE_ERROR - Operation failed due to media write
 *                                   error
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to close a file 
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE CETYPE __fclose( FILEOBJ fo )
{
    WORD fHandle;
    DIRENTRY dir;
    CETYPE error = CE_GOOD;

    fHandle = fo->entry;

    if( fo->Flags.write ) {
       // Get the entry
       dir = LoadDirAttrib( fo, &fHandle );

       // Update the time
       IncrementTimeStamp( dir );

       dir->DIR_FileSize = fo->size;

       // Just write the last entry in
       if( !WriteFileEntry( fo, &fHandle ) )
          error = CE_WRITE_ERROR;

        // It's now closed!
        fo->Flags.write = FALSE;
    }

    return( error );

}   // __fclose()


/******************************************************************************
 * Function:        CETYPE __fwrite( FSFILE *fo, void *src, WORD count,
 *                                   WORD *pBytesWritten )
 *
 * PreCondition:    Called from fwrite()
 *
 * Input:           fo - Pointer to file object
 *                  src - Pointer to source buffer for data
 *                  count - Number of bytes to write
 *                  pBytesWritten - Pointer to number of bytes written
 *                  
 * Output:          CE_GOOD - Operation successful
 *                  CE_WRITE_ERROR - Operation failed due to file
 *                                   not in "w" mode of operation
 *                                   or failure in writing the media
 *                  CE_NOT_PRESENT - Operation failed due to media
 *                                   being not detected
 *                  CE_WRITE_PROTECTED - Operation failed due to
 *                                       media is write-protected
 *                  CE_BAD_SECTOR_READ - Operation failed due to
 *                                       failure in reading the media
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to write data from source buffer to
 *                  a file 
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE CETYPE __fwrite( FSFILE *fo, void *src, WORD count, WORD *pBytesWritten )
{
    DISK    *dsk;   // Pointer to disk structure
    CETYPE  error = CE_GOOD;
    BYTE    sectorLoaded = FALSE;
    WORD    pos;
    DWORD   l;   // Absolute LBA of sector to load
    DWORD   seek, size;
    BYTE    *buffer, *source;

    dsk = fo->dsk;

    if(!MediaCallbackIdx(dsk->idx, MEDIA_CMD_PRESENT, NULL, NULL))
       return( CE_NOT_PRESENT );

    // See if the file was opened in a write mode
    if( !fo->Flags.write )
       return( CE_WRITE_ERROR );

    if(MediaCallbackIdx(dsk->idx, MEDIA_CMD_WRITE_PROTECTED, NULL, NULL))
       return( CE_WRITE_PROTECTED );

    dsk = fo->dsk;

    // Get the stated position
    pos = fo->pos;
    seek = fo->seek;

    l = Cluster2Sector( dsk, fo->ccls );
    l += (WORD)fo->sec;   // Add the sector number to it

    gBufferOwner = fo;
    if( !MediaReadSectorIdx(dsk->idx, l, dsk->buffer ) )
       return( CE_BAD_SECTOR_READ );

    sectorLoaded = TRUE;

    // Exit loop if EOF reached
    size = fo->size;
    
    buffer = dsk->buffer;
    buffer += pos;
    source = (BYTE *)src;

    // 2. Loop reading count bytes
    while( (error == CE_GOOD) && count > 0 ) {

       if( seek >= size )
          fo->Flags.FileWriteEOF = TRUE;

       // Load a new sector if necessary, multiples of sector
       if( pos == MEDIA_SECTOR_SIZE ) {

          // Close the currently loaded sector if we need to
          if( sectorLoaded ) {
             if( !MediaWriteSectorIdx(dsk->idx, l, dsk->buffer ) ) {
                error = CE_WRITE_ERROR;
                goto _fwrite_exit;
             }
          }

          // Reset position
          pos = 0;
          buffer = dsk->buffer;
          
          // Point to the next sector
          fo->sec++;

          // Get a new cluster if necessary
          if( fo->sec == dsk->SecPerClus ) {
             fo->sec = 0;

             if( fo->Flags.FileWriteEOF )
                error = FileAllocateNewCluster( fo );   // Add new cluster to the file
             else
                error = FileGetNextCluster( fo, 1 );
          }

          if( error == CE_GOOD ) {
             l = Cluster2Sector( dsk, fo->ccls );
             l += (WORD)fo->sec;   // Add the sector number to it
             gBufferOwner = fo;

             if( !MediaReadSectorIdx(dsk->idx, l, dsk->buffer ) ) {
                error = CE_BAD_SECTOR_READ;
                goto _fwrite_exit;
             }

          } else {
             goto _fwrite_exit;
	  }
             

       }   // Load new sector

       if( error == CE_GOOD ) 
       {
            unsigned int wr_size;
            
            wr_size = MEDIA_SECTOR_SIZE - pos;

            if(wr_size > count)
                wr_size = count;

            memcpy(buffer, source, wr_size);

            count   -= (WORD)wr_size;
            pos     += (WORD)wr_size;
            source  += wr_size;
            buffer  += wr_size;
            seek    += wr_size;
            (*pBytesWritten) += wr_size;
       }

       // Now increment the size of the part
       if( fo->Flags.FileWriteEOF )
          size = seek;

    }   // while loop

    if( error == CE_GOOD ) {
       // Figure out the LBA
       l = Cluster2Sector( dsk, fo->ccls );

       l += (WORD)fo->sec;   // Add the sector number to it

       if( !MediaWriteSectorIdx(dsk->idx, l, dsk->buffer ) )
          error = CE_WRITE_ERROR;
    }

_fwrite_exit:

   // Save off the positon
   fo->pos = pos;

   // Save off the seek
   fo->seek = seek;

   // Now the new size
   fo->size = size;

    return( error );

}   // __fwrite()


/******************************************************************************
 * Function:        CETYPE __fseek( FILEOBJ fo, DWORD offset, BYTE origin )
 *
 * PreCondition:    Called from fseek()    
 *
 * Input:           fo - Pointer to file object
 *                  offset - Offset from origin location
 *                  origin -
 *                         SEEK_SET - Seek from start of file
 *                         SEEK_CUR - Seek from current location
 *                         SEEK_END - Seek from end of file
 *                  
 * Output:          CE_GOOD - Operation successful
 *                  CE_INVALID_CLUSTER - Invalid cluster value > maxcls
 *                  CE_BAD_SECTOR_READ - A bad sector read occurred
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to set the starting position of the
 *                  file object to the offset
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE CETYPE __fseek( FILEOBJ fo, DWORD offset, BYTE origin )
{
    DWORD numSector, temp;   // LBA of first sector of first cluster
    DISK *dsk;   // Pointer to disk structure

    dsk = fo->dsk;

    switch( origin ) {
       case SEEK_CUR:
          // Apply the offset to the current position
          offset += fo->seek;
          break;

       case SEEK_END:
          // Apply the offset to the end of the file
          offset = fo->size - offset;
          break;

       case SEEK_SET:
          // Automatically there

       default:
          break;
    }

    // Start from the beginning
    fo->ccls = fo->cluster;

    temp = fo->size;

    if( offset > temp )
       return( -1 );   // Past the limits

    // If we are writing, we are no longer at the end
    fo->Flags.FileWriteEOF = FALSE;

    // Set the new postion
    fo->seek = offset;

    // Figure out how many sectors
    numSector = offset / MEDIA_SECTOR_SIZE;

    // Figure out how many bytes off of the offset
    offset = offset - (numSector * MEDIA_SECTOR_SIZE);
    fo->pos = offset;

    // Figure out how many clusters
    temp = numSector / (dsk->SecPerClus);

    // Figure out the stranded sectors
    numSector -= ((dsk->SecPerClus) * temp);
    fo->sec = numSector;

    // If we are in the current cluster stay there
    if( temp ) 
	{
       CETYPE status;
       
       status = FileGetNextCluster( fo, temp );
       
       if(status == CE_FAT_EOF)
       {
	       status = FileAllocateNewCluster( fo );   // Add new cluster to the file
	   } 
       	
       if(status != CE_GOOD)
          return( CE_INVALID_CLUSTER );   // Past the limits
    }

    // Determine the LBA of the selected sector and load
    temp = Cluster2Sector( dsk, fo->ccls );

    // Now the extra sectors
    numSector = fo->sec;
    temp += numSector;

    gBufferOwner = fo;

    if( !MediaReadSectorIdx(dsk->idx, temp, dsk->buffer ) )
       return( CE_BAD_SECTOR_READ );   // Bad read

    return( CE_GOOD );

}   // __fseek()


/******************************************************************************
 * Function:        CETYPE FileOpen( FILEOBJ fo, WORD *fHandle, char type )
 *
 * PreCondition:    FileFind() returns true for read or append, fo contains
 *                  file data
 *
 * Input:           fo - File to be opened
 *                  fHandle - Location of file
 *                  type - 
 *                         WRITE - Create a new file or replace an existing file
 *                         READ - Read data from an existing file
 *                         APPEND - Append data to an existing file
 *
 * Output:          CE_GOOD - File opened successful
 *                  CE_NOT_INIT	- Card is not yet initialized because of some error
 *                  CE_FILE_NOT_FOUND - Could not find the file on the card
 *                  CE_BAD_SECTOR_READ - A bad read occurred on a sector
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to open a file to perform a specific
 *                  operation
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE CETYPE FileOpen( FILEOBJ fo, WORD *fHandle, char type )
{
    DISK *dsk;   // Disk structure
    BYTE r;   // Result of search for file
    DWORD l;   // LBA of first sector of first cluster
    CETYPE error = CE_GOOD;

    dsk = (DISK *)(fo->dsk);

    if( dsk->mount == FALSE )   // Unmounted
       error = CE_NOT_INIT;

    else {

       // Reset the cluster
       fo->dirccls = fo->dirclus;

       // Load the sector
       CacheFileEntry( fo, fHandle, TRUE );

       // Fill up the file object with the information pointed to by fHandle
       r = FillFileObject( fo, fHandle );
       if( r != FS_FOUND )
          error = CE_FILE_NOT_FOUND;
       else {
          fo->seek = 0;   // First byte in file
          fo->ccls = fo->cluster;   // First cluster
          fo->sec = 0;   // First sector in the cluster
          fo->pos = 0;   // First byte in sector/cluster

          if( r == FS_NOT_FOUND ) {
    	     error = CE_FILE_NOT_FOUND;
          } else {
             // Determine the LBA of the selected sector and load
             l = Cluster2Sector( dsk,fo->ccls );

             gBufferOwner = fo;
             if( !MediaReadSectorIdx(dsk->idx, l, dsk->buffer ) )
                error = CE_BAD_SECTOR_READ;
          }   // Found

          fo->Flags.FileWriteEOF = FALSE;

          // Set flag for operation type
          if( (type == 'w') || (type == 'a') ) {
             fo->Flags.write = 1;   // Write or Append
          } else {
             fo->Flags.write = 0;   // Read
          }   // Flags

       }   // Found

    }   // Mounted

    return( error );

}   // FileOpen()


/******************************************************************************
 * Function:        FILEOBJ __fopen( const char *filename, const char *mode,
 *                                   FILEOBJ filePtr )
 *
 * PreCondition:    Called from fopen()    
 *
 * Input:           filename - The name of the file to open
 *                  mode -
 *                       w - Create a new file or replace an existing file
 *                       r - Read data from an existing file
 *                       a - Append data to an existing file or create a
 *                           file if it does not exist
 *                  filePtr - Pointer to an already allocated file object
 *
 * Output:          FILEOBJ - Pointer to the file object that has been
 *                            created or NULL if cannot be created
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to open a file
 *
 * Note:            Should not be called by user; Only supports "w", "r",
 *                  and "a"
 *****************************************************************************/
PRIVATE FILEOBJ __fopen( const char *filename, const char *mode, FSFILE *filePtr )
{
    WORD fHandle;
    CETYPE fileFound;
    FSFILE tempFile;

    if(!MediaCallbackName((BYTE *)filename, MEDIA_CMD_PRESENT, NULL, NULL))
       return NULL;

    if( !InitFileObject( filePtr, filename ) )
       return NULL;

    // Start at the root directory
    filePtr->dirclus = 0;
    filePtr->dirccls = 0;

    // Copy file object over
    FileObjectCopy( &tempFile, filePtr );

    // See if the file is found
    fileFound = FileFind( filePtr, &tempFile, 1 );

    switch( mode[0] ) {

       case 'w':

          if( fileFound == CE_GOOD ) {   // File found
             // File exists, we want to create a new one, remove it first
             fHandle = filePtr->entry;
             if( FileErase( filePtr, &fHandle, TRUE ) != CE_GOOD )
                return NULL;

          } else {   // File NOT found
             // Copy original file object back
             FileObjectCopy( filePtr, &tempFile );

             // Use the user requested name
             fHandle = 0;
          } 

          // Now create a new entry, if possible
          if( CreateFileEntry( filePtr, &fHandle ) != CE_GOOD )
             return NULL;

          if( FileOpen( filePtr, &fHandle, 'w' ) != CE_GOOD )
             return NULL;

          if( __fseek( filePtr, 0, SEEK_END ) != CE_GOOD )
             filePtr = NULL;

          break;   // case 'w'

       case 'a':

          if( fileFound != CE_GOOD ) {   // File NOT found

             // Copy original file object back
             FileObjectCopy( filePtr, &tempFile );

             // Use the user requested name
             fHandle = 0;

             // Now create a new entry, if possible
             if( CreateFileEntry( filePtr, &fHandle ) != CE_GOOD )
                return NULL;

          } else {   // File found
             fHandle = filePtr->entry;

             if( !filePtr->size ) {   // filePtr->size is 0
                if( FileErase( filePtr, &fHandle, TRUE ) != CE_GOOD )
                   return NULL;

                // Now create a new entry, if possible
                if( CreateFileEntry( filePtr, &fHandle ) != CE_GOOD )
                   return NULL;
             }

          }

          if( FileOpen( filePtr, &fHandle, 'w' ) != CE_GOOD )
             return NULL;

          if( __fseek( filePtr, 0, SEEK_END ) != CE_GOOD )
             filePtr = NULL;

            break;   // case 'a'

          case 'r':

	    if( fileFound == CE_GOOD ) {
               fHandle = filePtr->entry;
               if( FileOpen( filePtr, &fHandle, 'r' ) != CE_GOOD )
                  filePtr = NULL;
	    } else
               filePtr = NULL;

            break;   // case 'r'

          default:
            filePtr = NULL;   // Indicate error condition
            break;

    }   // End switch

    return( filePtr );

}   // __fopen()


/******************************************************************************
 * Function:        BOOL FAT16Init( void )
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE - Operation successful
 *                  FALSE - Operation failed
 *
 * Side Effects:    None
 *
 * Overview:        Initialize the static memory slots for holding
 *                  file structures (only used when FAT16_DYNAMIC_MEM is
 *                  not defined) and the interface to the media.
 *
 * Note:            User-called function to initialize the FAT16 library
 *****************************************************************************/
BOOL FSInit( UINT pb_clk )
{

#ifdef STARTER_KIT_NETWORK_BOARD
    mPORTGSetPinsDigitalOut(BIT_15);
    mPORTGClearBits(BIT_15);
#endif

#ifndef FAT16_DYNAMIC_MEM
    int fIndex;

    for( fIndex = 0; fIndex < FAT16_MAX_FILES_OPEN; fIndex++ )
        gblFileSlotOpen[fIndex] = TRUE;
#endif

    return( FATMediaInit(pb_clk) );

}   // FAT16Init()


/******************************************************************************
 * Function:        FSFILE *fopen( char *fileName, const char *mode )
 *
 * PreCondition:    Only supports "w", "r", and "a"
 *
 * Input:           fileName - The name of the file to open
 *                  mode -
 *                       w - Create a new file or replace an existing file
 *                       r - Read data from an existing file
 *                       a - Append data to an existing file
 *
 * Output:          FSFILE * - The pointer to the file object
 *
 * Side Effects:    None
 *
 * Overview:        User called file open function
 *
 * Note:            None
 *****************************************************************************/
PUBLIC FSFILE *FSfopen( const char *fileName, const char *mode )
{
    FSFILE *filePtr;
    FSFILE *pFile;

    filePtr = GetFilePtr();
    if( !filePtr )
       return NULL;

    // Try to open the file
    pFile = __fopen( fileName, mode, filePtr );
    if( !pFile ) {
       FreeFilePtr( filePtr );
    }

    return( pFile );

}   // fopen()


/******************************************************************************
 * Function:        int fseek( FILEOBJ fo, long offset, int origin )
 *
 * PreCondition:    File exists
 *
 * Input:           fo - Pointer to file structure
 *                  offset - Offset from origin location
 *                  origin - SEEK_SET - Seek from start of file
 *                           SEEK_CUR - Seek from current location
 *                           SEEK_END - Seek from end of file
 *
 * Output:          CE_GOOD - Operation successful
 *                  CE_INVALID_CLUSTER - Invalid cluster value > maxcls
 *                  CE_BAD_SECTOR_READ - A bad sector read occurred
 *
 * Side Effects:    None
 *
 * Overview:        User called function to set the starting position of
 *                  the stream to the offset
 *
 * Note:            None
 *****************************************************************************/
PUBLIC int FSfseek( FSFILE *stream, long offset, int origin )
{
    CETYPE err;

    err = __fseek( stream, offset, (BYTE)origin );
    if( err == CE_GOOD )
       return 0;
    else
       return -1;

}   // fseek()


/******************************************************************************
 * Function:        void rewind( FSFILE *stream )
 *
 * PreCondition:    File opened and file exists
 *
 * Input:           stream - Pointer to file structure
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        User called function to reset the position of the file
 *
 * Note:            None
 *****************************************************************************/
PUBLIC void FSrewind( FSFILE *stream )
{
    stream->seek = 0;
    stream->pos = 0;
    stream->sec = 0;
    stream->ccls = stream->cluster;

}   // rewind()


/******************************************************************************
 * Function:        int feof( FSFILE *stream )
 *
 * PreCondition:    File is open is read mode
 *
 * Input:           stream - Pointer to file structure
 *
 * Output:          Non-zero - EOF reached
 *                  0 - Not at end of File
 *
 * Side Effects:    None
 *
 * Overview:        User called function to find if position of the file is EOF
 *
 * Note:            None
 *****************************************************************************/
PUBLIC int FSfeof( FSFILE *stream )
{
    return( stream->seek == stream->size );

}   // feof()


/******************************************************************************
 * Function:        size_t fread( void *ptr, size_t size, size_t n,
 *                                FSFILE *stream )
 *
 * PreCondition:    File opened in the READ mode
 *
 * Input:           ptr - Destination buffer for read data
 *                  size - Size of object
 *                  n - Number of objects
 *                  stream - Source buffer (via file ptr) from where to
 *                           perform read
 *
 * Output:          Returns the number of objects read
 *
 *
 * Side Effects:    None
 *
 * Overview:        Read data from a file into a destination buffer
 *
 * Note:            None
 *****************************************************************************/
PUBLIC size_t FSfread( void *ptr, size_t size, size_t n, FSFILE *stream )
{
    WORD readCount = 0;
    CETYPE result;

    if( !size || !n )
       return 0;

    result = __fread( stream, ptr, size*n, &readCount ); 
    if( (result == CE_GOOD) || (result == CE_EOF) )
       return( readCount/size );   // Return number of objects read
    else
       return 0;

}   // fread()


/******************************************************************************
 * Function:        size_t fwrite( const void *ptr, size_t size, size_t n,
 *                                 FSFILE *stream )
 *
 * PreCondition:    File opened in WRITE mode
 *
 * Input:           ptr - Source buffer for write data
 *                  size - Size of data
 *                  n - Number of data
 *                  stream - Destination buffer (via file ptr) from where to
 *                           perform write 
 *
 * Output:          Returns the number of objects written
 *
 * Side Effects:    None
 *
 * Overview:        User-called function to write data from source buffer
 *                  to a file 
 *
 * Note:            None
 *****************************************************************************/
PUBLIC size_t FSfwrite( const void *ptr, size_t size, size_t n, FSFILE *stream )
{
    WORD writeCount = 0;

    if( !size || !n )
       return 0;

    if( __fwrite( stream, (void *)ptr, size*n, &writeCount ) == CE_GOOD )
       return( writeCount/size );   // Return number of objects written
    else
       return 0;

}   // fwrite()


/******************************************************************************
 * Function:        int fclose( FSFILE *stream )
 *
 * PreCondition:    File opened
 *
 * Input:           stream - Pointer to file structure
 *
 * Output:          0 - Operation successful
 *                  EOF - Operation failed due to either a NULL
 *                        stream or media failure 
 *
 * Side Effects:    None
 *
 * Overview:        User-called function to close a file
 *
 * Note:            None
 *****************************************************************************/
PUBLIC int FSfclose( FSFILE *stream )
{
    int status;

    if( !stream )
       return EOF;

    if( __fclose( stream ) == CE_GOOD )
       status = 0;
    else
       status = EOF;

    FreeFilePtr( stream );

    return( status );

}   // fclose()


/******************************************************************************
 * Function:        ftell( FSFILE *stream )
 *
 * PreCondition:    File opened
 *
 * Input:           stream - Pointer to file structure
 *
 * Output:          Current file position for stream
 *
 * Side Effects:    None
 *
 * Overview:        User-called function to determine the current position
 *                  of a file
 *
 * Note:            None
 *****************************************************************************/
PUBLIC long FSftell( FSFILE *stream )
{
    return( stream->seek );

}   // ftell()


/******************************************************************************
 * Function:        int remove( const char *filename )
 *
 * PreCondition:    File not opened, file exists
 *
 * Input:           compare - Pointer to the name of file to be removed
 *
 * Output:          0 - Operation successful
 *                  -1 - Operation failed
 *
 * Side Effects:    None
 *
 * Overview:        User-called function to remove the named file
 *
 * Note:            None
 *****************************************************************************/
PUBLIC int FSremove( const char *filename )
{
    FSFILE f, tempFile;
    FILEOBJ fo = &f;
    CETYPE result;

    if(!MediaCallbackName((BYTE *)filename, MEDIA_CMD_PRESENT, NULL, NULL))
        return -1;

    if(MediaCallbackName((BYTE *)filename, MEDIA_CMD_WRITE_PROTECTED, NULL, NULL))
       return -1;

    if( !InitFileObject( fo, filename ) )
       return -1;

    // Copy file object over
    FileObjectCopy( &tempFile, fo );

    // See if the file is found
    result = FileFind( fo, &tempFile, 1 );
    if( result != CE_GOOD )
       return -1;

    result = FileErase( fo, &fo->entry, TRUE );
    if( result != CE_GOOD )
       return -1;
    
    return 0;

}   // remove()


/******************************************************************************
 * Function:        CETYPE CreateShortFileEntry( FILEOBJ fo, WORD *fHandle,
 *                                               char *name )
 *
 * PreCondition:    
 *
 * Input:           
 *                  
 * Output:          
 *
 * Side Effects:    None
 *
 * Overview:        
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE CETYPE CreateShortFileEntry( FILEOBJ fo, WORD *fHandle, char *name )
{
    *fHandle = 0;
    
    // Figure out where to put this file in the directory stucture 
    if( FindEmptyEntries( fo, fHandle, 1 ) ) {

       // Found the entry now populate it 
       return( PopulateEntries( fo, name, fHandle, 1, DIR_NAMECOMP ) );

    } else

       return( CE_DIR_FULL );

}   // CreateShortFileEntry()


/******************************************************************************
 * Function:        CETYPE __CreateDirectory( FILEOBJ fo, WORD *fHandle )
 *
 * PreCondition:    
 *
 * Input:           
 *                  
 * Output:          
 *
 * Side Effects:    None
 *
 * Overview:        
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE CETYPE __CreateDirectory( FILEOBJ fo, WORD *fHandle )
{
    DIRENTRY dir;
    CETYPE error;
    WORD mainCluster, directClus;
    char name[DIR_NAMECOMP];

    mainCluster = fo->dirclus;

    // Create the main file
    error = CreateFileEntry( fo, fHandle );
    if( error == CE_GOOD ) {

       // Find an empty cluster to use
       error = FileCreateHeadCluster( fo, &directClus );
       if( error == CE_GOOD ) {

          // Add the directory attributes
          dir = LoadDirAttrib( fo, fHandle );
          dir->DIR_Attr = ATTR_DIRECTORY;
          dir->DIR_FstClusLO = directClus;

          // Now it's a directory
          if( !WriteFileEntry( fo, fHandle ) ) {
             FileErase( fo, fHandle, TRUE );
             return( CE_WRITE_ERROR );
          }

          // Point to the new cluster
          fo->dirccls = fo->dirclus = directClus;
           
          // Create the "." entry
          strcpy( name, defDOTDIR );

          error = CreateShortFileEntry( fo, fHandle, name );
          if( error != CE_GOOD ) {
             FileErase( fo, fHandle, TRUE );
             return( error );
          }

          // Make it a directory pointing to itself
          *fHandle = 0;   // First entry
          dir = LoadDirAttrib( fo, fHandle );
          dir->DIR_Attr = ATTR_DIRECTORY;       
          dir->DIR_FstClusLO = directClus;
                
          // Now it's a directory
          if( !WriteFileEntry( fo, fHandle ) ) {
             FileErase( fo, fHandle, TRUE );
             return( CE_WRITE_ERROR );
          }

          // Create the ".." entry
          strcpy( name, defDOTDOTDIR );

          error = CreateShortFileEntry( fo, fHandle, name );
          if( error != CE_GOOD ) {
             FileErase( fo, fHandle, TRUE );
             return( error );
          }
  
          // Make it a directory pointing to itself
          *fHandle = 1;   // Second entry
          dir = LoadDirAttrib( fo, fHandle );
          dir->DIR_Attr = ATTR_DIRECTORY;       
          dir->DIR_FstClusLO = mainCluster;
                
          // Now it's a directory
          if( !WriteFileEntry( fo, fHandle ) ) {
             FileErase( fo, fHandle, TRUE );
             return( CE_WRITE_ERROR );
          }
           
       } else {

          // If there is no room, remove the file
          FileErase( fo, fHandle, TRUE );

       }   // Create head cluster

    }   // Create directory entry

    return( error );

}   // __CreateDirectory()


/******************************************************************************
 * Function:        FSFILE *CreateDirectory( const char *dirName )
 *
 * PreCondition:    
 *
 * Input:           
 *                  
 * Output:          TRUE - Operation successful
 *                  FALSE - Operation failed
 *
 * Side Effects:    None
 *
 * Overview:        User called function to create a directory.        
 *
 * Note:            None           
 *****************************************************************************/
PUBLIC FSFILE *CreateDirectory( const char *dirName )
{
    WORD fHandle = 0;
    FSFILE tempFile;
    FILEOBJ fo;
    CETYPE fileFound;
    //BOOL error = FALSE;

/**
    if( MediaIsWriteProtected() || !MediaIsPresent() )
       return FALSE;
**/
    fo = GetFilePtr();
    if( !fo )
       return NULL;

    if( !InitFileObject( fo, dirName ) ) {
       FreeFilePtr( fo );
       return NULL;
    }

    // Copy file object over
    FileObjectCopy( &tempFile, fo );

    // See if the file is found
    fileFound = FileFind( fo, &tempFile, 1 );

    if( fileFound == CE_FILE_NOT_FOUND ) { 

       // Copy original file object back
       FileObjectCopy( fo, &tempFile );

       // Use the user requested name
       fHandle = 0;

       // Try to create the directory
       if(  __CreateDirectory( fo, &fHandle ) == CE_GOOD )
          return( fo );

    }

    return( NULL );

}   // CreateDirectory()


/******************************************************************************
 * Function:        BOOL __ChangeDirectory( FILEOBJ fo, WORD *fHandle )
 *
 * PreCondition:    Called from ChangeDirectory()
 *
 * Input:           
 *                  
 * Output:          TRUE - Operation successful
 *                  FALSE - Operation failed
 *
 * Side Effects:    None
 *
 * Overview:        
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE BOOL __ChangeDirectory( FILEOBJ fo, WORD *fHandle )
{
    BOOL status = FALSE;
   
    // Reset the cluster
    fo->dirccls = fo->dirclus;
    
    CacheFileEntry( fo, fHandle, TRUE );
   
    // Fill fo with the information pointed to by fHandle
    if( FillFileObject( fo, fHandle ) == FS_FOUND ) {

       // Found a file
       // Is it an directory?
       if( (fo->attributes & ATTR_DIRECTORY) == ATTR_DIRECTORY ) {
           // Found the directory
           // Now figure out the directory cluster and write the cluster
           fo->dirclus = fo->dirccls = fo->cluster;
           status = TRUE;
        }

    }
   
    return( status );

}   // __ChangeDirectory()


/******************************************************************************
 * Function:        BOOL ChangeDirectory( FSFILE *stream, const char *dirName )
 *
 * PreCondition:    
 *
 * Input:           
 *                  
 * Output:          TRUE - Operation successful
 *                  FALSE - Operation failed
 *
 * Side Effects:    None
 *
 * Overview:        
 *
 * Note:           None 
 *****************************************************************************/
PUBLIC BOOL ChangeDirectory( FSFILE *stream, const char *dirName )
{
    WORD fHandle;

    if(!MediaCallbackIdx(((DISK *)stream->dsk)->idx, MEDIA_CMD_PRESENT, NULL, NULL))
       return FALSE;

    if( !stream )
       return FALSE;

    fHandle = stream->entry;

    // Try to change the directory
    return( __ChangeDirectory( stream, &fHandle ) );

}   // ChangeDirectory()


/******************************************************************************
 * Function:        BOOL __GetDirectoryName( FILEOBJ fo, WORD *fHandleReturn )
 *
 * PreCondition:    
 *
 * Input:           
 *                  
 * Output:          TRUE - Operation successful
 *                  FALSE - Operation failed
 *
 * Side Effects:    None
 *
 * Overview:        Internal function function to obtain the name of the
 *                  current directory
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE BOOL __GetDirectoryName( FILEOBJ fo, WORD *fHandleReturn )
{
    WORD currentDir, fHandle;
    BOOL status = FALSE, dirFound = FALSE;
   
    // Reset the cluster
    currentDir = fo->dirccls = fo->dirclus;
    
    // Make sure we are not in the root directory 
    if( !currentDir )
        status = FALSE;

    else {
       // -- 1. Go up a level to the parent directory
       fHandle = 0x0001; 
        
       CacheFileEntry( fo, &fHandle, TRUE );
        
       // Fill up fo with the information pointed to by fHandle
       if( FillFileObject( fo, &fHandle ) == FS_FOUND ) {

          // Found a file
          // Is it a directory?
          if( (fo->attributes & ATTR_DIRECTORY) == ATTR_DIRECTORY ) {

             // Found the directory
             // Now figure out the directory cluster
             // -- 2. Now move to that parent directory
             fo->dirclus = fo->dirccls = fo->cluster;

             // Reset file handle
             fHandle = 0;

             // -- 3. Now find the entry that points to the directory
             //       we are trying to find the name for
             while( (dirFound == FALSE) &&
                    ((status = FillFileObject( fo, &fHandle )) != FS_NO_MORE) ) {  

                if( status == FS_FOUND ) {

	           // Check if the cluster it points to match
	           if( fo->cluster == currentDir )
	              dirFound = TRUE;
                }

                fHandle++;

             }   // while loop

             if( dirFound )
                status = TRUE;
                                    
             // -- Now move back up to the main level
             fo->dirclus = fo->dirccls = currentDir;
                
             // Now re-cache
             CacheFileEntry( fo, fHandleReturn, TRUE );
                
          }   // Found a directory
            
       } else   // Did not find the upper directory
          fo->dirclus = fo->dirccls = currentDir;

    }   // Not root directory

    return( status );

}   // __GetDirectoryName()


/******************************************************************************
 * Function:        BOOL GetDirectoryName( FSFILE *stream, char *dirName )
 *
 * PreCondition:    None
 *
 * Input:           dirName - Character string containing the directory
 *                            name
 *                  
 * Output:          TRUE - Operation successful
 *                  FALSE - Operation failed
 *
 * Side Effects:    None
 *
 * Overview:        User called function to obtain the name of the current
 *                  directory
 *
 * Note:            None
 *****************************************************************************/
PUBLIC BOOL GetDirectoryName( FSFILE *stream, char *dirName )
{
    WORD fHandle = 0, index = 0;
    BOOL status;

    if( !stream )
       return FALSE;
    
    if(!stream->dsk)
   		return FALSE;
   		
    if(!MediaCallbackIdx(((DISK *)stream->dsk)->idx, MEDIA_CMD_PRESENT, NULL, NULL))
       return FALSE;


    // Try to get the directory name
    status = __GetDirectoryName( stream,  &fHandle );

    if( status == TRUE )
    {
        while(stream->name[index] != '\0')
        {
            dirName[index] = stream->name[index];
            index++;
        }
        
        dirName[index] = '\0';
    }

    return( status );
    
}   // GetDirectoryName()
/*********************************************************************
 * Function:       
 *
 * PreCondition:    
 *
 * Input:           
 *
 * Output:          
 *
 * Side Effects:    
 *
 * Overview:        
 *
 * Note:           
 ********************************************************************/
PUBLIC FSFILE *GetRootDirectory(const char *root_name)
{
    FSFILE *filePtr;
    FSFILE *pFile;

    filePtr = GetFilePtr();

    if( !filePtr )
       return NULL;
    
    
    if( !InitFileObject( filePtr, root_name ) )
    {
        FreeFilePtr( filePtr );
        return NULL;
    }

    if(!MediaCallbackIdx(((DISK *)filePtr->dsk)->idx, MEDIA_CMD_PRESENT, NULL, NULL))
    {
        FreeFilePtr( filePtr );
        return NULL;
    }

    // Start at the root directory
    filePtr->dirclus = 0;
    filePtr->dirccls = 0;

    return( filePtr );

}

/******************************************************************************
 * Function:        BOOL EnsureDirEmpty( FILEOBJ fo )
 *
 * PreCondition:    Called from __EraseDirectory()
 *
 * Input:           fo - Pointer to file object
 *                  
 * Output:          TRUE - Operation successful
 *                  FALSE - Operation failed
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to check whether the current directory
 *                  is empty
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE BOOL EnsureDirEmpty( FILEOBJ fo )
{
    WORD fHandle;
    BYTE a;
    DIRENTRY dir;

    // Start with the entry passed the ".."
    fHandle = 2;

    dir = CacheFileEntry( fo, &fHandle, TRUE );

    // Search the whole dir until a file or the empty entry is found
    do {
       // Get the entry
       dir = CacheFileEntry( fo, &fHandle, FALSE );
        
       // Read the first char of the file name 
       a = dir->DIR_Name[0];  
        
       fHandle++;   
    } while( a == DIR_DEL );  

    // Directory is empty?
    if( a == DIR_EMPTY )
       return( TRUE );
    else
       return( FALSE );        

}   // EnsureDirEmpty()


/******************************************************************************
 * Function:        CETYPE __EraseDirectory( FILEOBJ fo, WORD *fHandle )
 *
 * PreCondition:    Called from EraseDirectory()
 *
 * Input:           fo - File
 *                  
 * Output:          CE_GOOD - Operation successful
 *                  CE_DIR_NOT_EMPTY - Operation failed due to directory
 *                                     not being empty
 *                  CE_DIR_NOT_FOUND - Operation failed due to directory
 *                                     cannot be found
 *
 * Side Effects:    None
 *
 * Overview:        Internal function to remove a directory
 *
 * Note:            Should not be called by user
 *****************************************************************************/
PRIVATE CETYPE __EraseDirectory( FILEOBJ fo, WORD *fHandle )
{
    WORD ccls, baseDir, fHandleCur;
    CETYPE status;    
    
    // Reset the cluster
    baseDir = fo->dirccls = fo->dirclus;
    
    CacheFileEntry( fo, fHandle, TRUE );
    
    // Fill file object updates the handle
    fHandleCur = *fHandle;

    // Fill up fo with the information pointed to by fHandle
    if( FillFileObject( fo, &fHandleCur ) == FS_FOUND ) {

       // Found a file
       // Is it a directory?
       if( (fo->attributes & ATTR_DIRECTORY) == ATTR_DIRECTORY ) {

          // Found the directory
          // Now figure out the directory cluster
          ccls = fo->cluster;
            
          // Move to the directory
          fo->dirclus = fo->dirccls = ccls;
                        
          // No need to erase the "." & ".." since they are the only entries
          if( EnsureDirEmpty( fo ) ) {

             // Move to the base directory
             fo->dirclus = fo->dirccls = baseDir;

             // Now erase the directory entry and the corresponding clusters
             status = FileErase( fo, fHandle, TRUE );
                
          } else {   // Directory not empty

             status = CE_DIR_NOT_EMPTY;

             // Move to the base directory
             fo->dirccls = fo->dirclus = baseDir;

          }   // Ensure directory is empty

       } else   // Not a directory

          status = CE_DIR_NOT_FOUND;

    }
    
    return( status );

}   // __EraseDirectory()


/******************************************************************************
 * Function:        BOOL EraseDirectory( FSFILE *stream, const char *dirName )
 *
 * PreCondition:    Directory must be empty prior to being removed
 *
 * Input:           dirName 
 *                  
 * Output:          TRUE - Operation successful
 *                  FALSE - Operation failed
 *
 * Side Effects:    None
 *
 * Overview:        User called function to remove a directory        
 *
 * Note:            None
 *****************************************************************************/
PUBLIC BOOL EraseDirectory( FSFILE *stream, const char *dirName )
{
    CETYPE status;
    FSFILE f, tempFile;
    FILEOBJ fo = &f;

    if( !stream )
       return FALSE;

    if(!MediaCallbackIdx(((DISK *)stream->dsk)->idx, MEDIA_CMD_PRESENT, NULL, NULL))
        return FALSE;

    if(MediaCallbackIdx(((DISK *)stream->dsk)->idx, MEDIA_CMD_WRITE_PROTECTED, NULL, NULL))
        return FALSE;



    memset( fo, 0, sizeof( f ) );

    if( !InitFileObject( fo, dirName ) )
       return FALSE;

    // Copy file object over
    FileObjectCopy( &tempFile, fo );

    // See if the file is found
    status = FileFind( fo, &tempFile, 1 );
    if( status == CE_FILE_NOT_FOUND )
       return FALSE;

    status = __EraseDirectory( fo, &fo->entry );
    if( status != CE_GOOD )
       return FALSE;

    FreeFilePtr( stream );

    return TRUE;

}   // EraseDirectory()
/*********************************************************************
 * Function:       
 *
 * PreCondition:    
 *
 * Input:           
 *
 * Output:          
 *
 * Side Effects:    
 *
 * Overview:        
 *
 * Note:           
 ********************************************************************/
BOOL GetFileName(FSFILE *file, WORD *handle, BOOL *dir)
{
    WORD    temp_handle;
    BYTE    state;
    BOOL    found = FALSE;

    *dir = FALSE;

    if((!file) || (!handle))
        return FALSE;

    temp_handle = *handle;

    if(!temp_handle)
        if(!CacheFileEntry(file, &temp_handle, TRUE))
            return FALSE;
        
    while(!found)
    {
        state = FillFileObject(file, &temp_handle);

        if(state == FS_NO_MORE)
        {
            *handle = temp_handle;
            return FALSE;
        }

        if(state == FS_FOUND)
        {
            WORD    attribute;

            attribute = file->attributes & ATTR_LONG_NAME_MASK;

            if( (attribute != ATTR_VOLUME_ID) && ((attribute & ATTR_HIDDEN) != ATTR_HIDDEN) )
            {
                found = TRUE;

                if(attribute & ATTR_DIRECTORY)
                    *dir = TRUE;
            }

        }

        temp_handle++;
    }

    *handle = temp_handle;
    return TRUE;
}
/*********************************************************************
 * Function:       
 *
 * PreCondition:    
 *
 * Input:           
 *
 * Output:          
 *
 * Side Effects:    
 *
 * Overview:        
 *
 * Note:           
 ********************************************************************/
BOOL GetFileNameIdx(FSFILE *file, WORD idx, BOOL *dir)
{
    WORD    i;
    WORD    handle = 0;

    idx++;

    for(i = 0; i < idx; i++)
    {
        if(!GetFileName(file, &handle, dir))
            break;
    }

    if(i != idx)
        return FALSE;

    return TRUE;
}
