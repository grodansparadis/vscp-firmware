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
 * $Id: MediaSD.c,v 1.4 2006/10/24 16:37:51 C12923 Exp $
 *
 * $Name:  $
 *
 *****************************************************************************/
#define _MEDIA_CONFIGURATION
#include "fat.def"
#include <fat16\media.h>


/****************************************************************************
  Section:
    Inline Functions
  ***************************************************************************/
static inline __attribute__((always_inline)) MEDIA_IDX _GetMediaName(BYTE *name)
{
    UINT i;
    
    for(i = 0; i < MEDIA_IDX_NUM; i++)
    {
        if(!strncmp(name, _media[i].name, strlen(_media[i].name)))
        {
            break;
        }
    }
    
    return i;
}
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
BOOL MediaStripRootName(BYTE *name)
{
    UINT    idx;
    UINT    i;
    UINT    length;

    idx = _GetMediaName(name);

    if(idx >= MEDIA_IDX_NUM)
        return FALSE;

    length = strlen(_media[idx].name);

    strcpy(name, &name[length]);

    return TRUE;

}
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
BOOL MediaInitName(BYTE *name, UINT pb_clock)
{
    
    UINT idx;

    idx = _GetMediaName(name);

    return MediaInitIdx(idx, pb_clock);

}
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
BOOL MediaInitIdx(UINT idx, UINT pb_clock)
{
    if(idx >= MEDIA_IDX_NUM)
        return FALSE;

    return _media[idx].init(pb_clock, _media[idx].init_flags);
}
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
BOOL MediaInitAll(UINT pb_clock)
{
    UINT i;
    BOOL result = FALSE;
    
    for(i = 0; i < MEDIA_IDX_NUM; i++)
    {
        result |=  _media[i].init(pb_clock, _media[i].init_flags);
    }

    return result;

}
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
CETYPE MediaDiskMountName(BYTE *name)
{
    
    UINT idx;

    idx = _GetMediaName(name);

    return MediaDiskMountIdx(idx);
   
}
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
CETYPE MediaDiskMountIdx(UINT idx)
{
    UINT        buffer_addr;
    UINT        disk_addr;
    CETYPE  error = CE_GOOD;
    
    if(idx >= MEDIA_IDX_NUM)
        return CE_NOT_INIT;

    if(!_media[idx].callback(MEDIA_CMD_DISK, NULL, &disk_addr))
        return CE_NO_DISK_BUF;

    if(!_media[idx].callback(MEDIA_CMD_SECTOR, NULL, &buffer_addr))
        return CE_NO_SECTOR_BUF;

    ((DISK *)disk_addr)->name = (BYTE *)_media[idx].name;

    if(_media[idx].disk_mount_init)
    {
        if(!_media[idx].disk_mount_init((DISK *)disk_addr))
            return CE_NOT_PRESENT;
    }

    return DiskMount( idx, (DISK *)disk_addr, (BYTE *)buffer_addr);
   
}
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
CETYPE MediaDiskMountAll(void)
{
    UINT        buffer_addr;
    UINT        disk_addr;
    UINT i;
    CETYPE  error = CE_GOOD;
    
    for(i = 0; i < MEDIA_IDX_NUM; i++)
    {
        CETYPE err;

        if(!_media[i].callback(MEDIA_CMD_DISK, NULL, &disk_addr))
            continue;

        if(!_media[i].callback(MEDIA_CMD_SECTOR, NULL, &buffer_addr))
            continue;

        ((DISK *)disk_addr)->name = (BYTE *)_media[i].name;
        
        if(err = DiskMount( i, (DISK *)disk_addr, (BYTE *)buffer_addr) != CE_GOOD)
            error = err;
    }

    return error;   
}
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
BOOL MediaCallbackName(BYTE *name, UINT cmd, void *pass_data, void *return_data)
{
    
    UINT idx;

    idx = _GetMediaName(name);

    return MediaCallbackIdx(idx, cmd, pass_data, return_data);

}
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
BOOL MediaCallbackIdx(UINT idx, UINT cmd, void *pass_data, void *return_data)
{
    if(idx >= MEDIA_IDX_NUM)
        return FALSE;

    return _media[idx].callback(cmd, pass_data, return_data);
}
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
BOOL MediaReadSectorName(BYTE *name, DWORD addr, BYTE *buffer)
{
    UINT idx;

    idx = _GetMediaName(name);

    return MediaReadSectorIdx(idx, addr, buffer);
}
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
BOOL MediaReadSectorIdx(UINT idx, DWORD addr, BYTE *buffer)
{
    if(idx >= MEDIA_IDX_NUM)
        return FALSE;

    return _media[idx].read(addr, buffer);
}
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
BOOL MediaWriteSectorName(BYTE *name, DWORD addr, BYTE *buffer)
{
    UINT idx;

    idx = _GetMediaName(name);

    return MediaWriteSectorIdx(idx, addr, buffer);
}
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
BOOL MediaWriteSectorIdx(UINT idx, DWORD addr, BYTE *buffer)
{

    if(idx >= MEDIA_IDX_NUM)
        return FALSE;

    return _media[idx].write(addr, buffer);
}
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
BOOL MediaCloseName(BYTE *name)
{
    UINT idx;

    idx = _GetMediaName(name);

    return MediaCloseIdx(idx);
}
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
BOOL MediaCloseIdx(UINT idx)
{

    if(idx >= MEDIA_IDX_NUM)
        return FALSE;

    return _media[idx].close(_media[idx].close_flags);
}
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
BOOL MediaCloseAll(void)
{
    UINT i;
    BOOL result = FALSE;
    
    for(i = 0; i < MEDIA_IDX_NUM; i++)
    {
        result |=  _media[i].close(_media[i].close_flags);
    }

    return result;

}
