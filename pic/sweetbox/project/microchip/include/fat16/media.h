/******************************************************************************
 *
 *              SECURE DIGITAL MEDIA INTERFACE
 *
 ******************************************************************************
 * FileName:        MediaSD.c
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
 *****************************************************************************
 *
 * $Id: media.h 161 2008-10-15 19:30:28Z akhe $
 *
 * $Name:  $
 *
 *****************************************************************************/
#ifndef _MEDIA_HEADER_FILE
#define _MEDIA_HEADER_FILE

/****************************************************************************
  Section: Includes
  ***************************************************************************/
#include <fat16\fat.h>
#include "GenericTypeDefs.h"
/****************************************************************************
  Section:  Media Structure

    You will need to provide an index in the table.

    You will need to provide a drive name
        ie "C:", "USB", etc


    For each media source, you will need to provide the following funtions:
    
    Initialization:
        syntax:     BOOL <init_media>(unsigned long flags)
        parameters  flags - 32 bit flag parameter
        return      TRUE if success, else FALSE
        overview:   initialize media

    Read Sector:
        syntax:     BOOL <read_sector>(unsigned long address, unsigned char *buffer)
        parameters  address - 32 bit address to read
                    buffer - pointer to buffer the hold sector
        return      TRUE if success, else FALSE
        overview    Read a sector from the media

    Write Sector:   
        syntax:     BOOL <write_sector>(unsigned long address, unsigned char *buffer)
        parameters  address - 32 bit address to read
                    buffer - pointer to buffer the hold sector
        return      TRUE if success, else FALSE
        overview    Write a sector from the media

    Close:
        syntax      BOOL <close_media>(unsigned long flags)
        parameters  flags - 32 bit flag parameter
        return      TRUE if success, else FALSE
        overview:   initialize media

    Callback:
        syntax      BOOL<callback>(unsigned long cmd, void *passed_date, void *return_data)
        paramters   cmd - command 
                    passed_data - pointer data to be passed
                    return_data - pointer to data to be returned
        return      TRUE if success, else FALSE
        overview:   This is an optional function, can be NULL
  ***************************************************************************/

typedef BOOL (*MEDIA_INIT_FUNC) ( unsigned long clock, unsigned long flags );
typedef BOOL (*MEDIA_READ_SECTOR_FUNC) ( unsigned long address, unsigned char *buffer );
typedef BOOL (*MEDIA_WRITE_SECTOR_FUNC) ( unsigned long address, unsigned char *buffer );
typedef BOOL (*MEDIA_CLOSE_FUNC) ( unsigned long flags );
typedef BOOL (*MEDIA_CALLBACK_FUNC) ( unsigned long cmd, void *passed_data, void *return_data);
typedef BOOL (*MEDIA_DISKMOUNT_INIT)(DISK *dsk);

typedef struct __MEDIA_DATA
{
    const unsigned char     *name;
    MEDIA_INIT_FUNC         init;
    unsigned long           init_flags;
    MEDIA_READ_SECTOR_FUNC  read;
    MEDIA_WRITE_SECTOR_FUNC write;
    MEDIA_CLOSE_FUNC        close;
    unsigned long           close_flags;
    MEDIA_DISKMOUNT_INIT    disk_mount_init;
    MEDIA_CALLBACK_FUNC     callback;
}MEDIA_DATA;




/****************************************************************************
  Function:
    yy <xx>(zz)

  Summary:

  Description:

  Precondition:

  Parameters:

  Returns:

  Remarks:
  ***************************************************************************/
BOOL MediaStripRootName(BYTE *name);
/****************************************************************************
  Function:
    yy <xx>(zz)

  Summary:

  Description:

  Precondition:

  Parameters:

  Returns:

  Remarks:
  ***************************************************************************/
BOOL MediaInitName(BYTE *name, UINT pb_clock);
/****************************************************************************
  Function:
    yy <xx>(zz)

  Summary:

  Description:

  Precondition:

  Parameters:

  Returns:

  Remarks:
  ***************************************************************************/
BOOL MediaInitIdx(UINT idx, UINT pb_clock);
/****************************************************************************
  Function:
    yy <xx>(zz)

  Summary:

  Description:

  Precondition:

  Parameters:

  Returns:

  Remarks:
  ***************************************************************************/
BOOL MediaInitAll(UINT pb_clock);
/****************************************************************************
  Function:
    yy <xx>(zz)

  Summary:

  Description:

  Precondition:

  Parameters:

  Returns:

  Remarks:
  ***************************************************************************/
CETYPE MediaDiskMountName(BYTE *name);
/****************************************************************************
  Function:
    yy <xx>(zz)

  Summary:

  Description:

  Precondition:

  Parameters:

  Returns:

  Remarks:
  ***************************************************************************/
CETYPE MediaDiskMountIdx(UINT idx);
/****************************************************************************
  Function:
    yy <xx>(zz)

  Summary:

  Description:

  Precondition:

  Parameters:

  Returns:

  Remarks:
  ***************************************************************************/
CETYPE MediaDiskMountAll( void);
/****************************************************************************
  Function:
    yy <xx>(zz)

  Summary:

  Description:

  Precondition:

  Parameters:

  Returns:

  Remarks:
  ***************************************************************************/
BOOL MediaCallbackName(BYTE *name, UINT cmd, void *pass_data, void *return_data);
/****************************************************************************
  Function:
    yy <xx>(zz)

  Summary:

  Description:

  Precondition:

  Parameters:

  Returns:

  Remarks:
  ***************************************************************************/
BOOL MediaCallbackIdx(UINT idx, UINT cmd, void *pass_data, void *return_data);
/****************************************************************************
  Function:
    yy <xx>(zz)

  Summary:

  Description:

  Precondition:

  Parameters:

  Returns:

  Remarks:
  ***************************************************************************/
BOOL MediaReadSectorName(BYTE *name, DWORD addr, BYTE *buffer);
/****************************************************************************
  Function:
    yy <xx>(zz)

  Summary:

  Description:

  Precondition:

  Parameters:

  Returns:

  Remarks:
  ***************************************************************************/
BOOL MediaReadSectorIdx(UINT idx, DWORD addr, BYTE *buffer);
/****************************************************************************
  Function:
    yy <xx>(zz)

  Summary:

  Description:

  Precondition:

  Parameters:

  Returns:

  Remarks:
  ***************************************************************************/
BOOL MediaWriteSectorName(BYTE *name, DWORD addr, BYTE *buffer);
/****************************************************************************
  Function:
    yy <xx>(zz)

  Summary:

  Description:

  Precondition:

  Parameters:

  Returns:

  Remarks:
  ***************************************************************************/
BOOL MediaWriteSectorIdx(UINT idx, DWORD addr, BYTE *buffer);
/****************************************************************************
  Function:
    yy <xx>(zz)

  Summary:

  Description:

  Precondition:

  Parameters:

  Returns:

  Remarks:
  ***************************************************************************/
BOOL MediaCloseName(BYTE *name);
/****************************************************************************
  Function:
    yy <xx>(zz)

  Summary:

  Description:

  Precondition:

  Parameters:

  Returns:

  Remarks:
  ***************************************************************************/
BOOL MediaCloseIdx(UINT idx);
/****************************************************************************
  Function:
    yy <xx>(zz)

  Summary:

  Description:

  Precondition:

  Parameters:

  Returns:

  Remarks:
  ***************************************************************************/
BOOL MediaCloseAll(void);

#endif
