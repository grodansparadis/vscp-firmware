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
#ifndef _SD_CARD_SPI_HEADER_FILE
#define _SD_CARD_SPI_HEADER_FILE

#include "GenericTypeDefs.h"
#include <fat16/sdcard_spi_hw.h>
#include <fat16\fat.h>
#include <fat16/media.h>

#define SD_CMD_IDLE             0
#define SD_CMD_SEND_OP_COND     1
#define SD_CMD_CSD              9
#define SD_CMD_SET_BLOCK_LEN   16
#define SD_CMD_READ_BLOCK      17
#define SD_CMD_WRITE_BLOCK     24

#define MediaIsPresent() (!SD_CD)
#define MediaIsWriteProtected()	(SD_WE)




/******************************************************************************
 * Function:        BOOL MediaInit( CARDSTATUS *cs )
 *
 * PreCondition:    None
 *
 * Input:           cs - Pointer to CARDSTATUS structure
 *
 * Output:          TRUE - Media initialized successfully
 *                  FALSE - Media failed to initialize
 *
 * Side Effects:    None
 *
 * Overview:        User-called function to initialize the SD Media. 
 *
 * Note:            None
 *****************************************************************************/
PUBLIC BOOL SDInit(unsigned long pb_clk, unsigned long flags );

/******************************************************************************
 * Function:        void MediaShutdown( void )
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        User-called function to read a specific sector on the media.
 *
 * Note:            None
 *****************************************************************************/
PUBLIC BOOL SDShutDown( unsigned long flags );


/******************************************************************************
 * Function:        BOOL MediaReadSector( DWORD Sector, BYTE *pDat )
 *
 * PreCondition:    None
 *
 * Input:           Sector - Sector location to perform the read
 *                  pDat - Buffer containing data read from the
 *                         sector location on the media. 
 *
 * Output:          TRUE - Write to media was successful
 *                  FALSE - Failure to write to media
 *
 * Side Effects:    None
 *
 * Overview:        User-called function to read a specific sector on the media.
 *
 * Note:            None
 *****************************************************************************/
PUBLIC BOOL SDReadSector( DWORD Sector, BYTE *pDat );


/******************************************************************************
 * Function:        BOOL MediaWriteSector( DWORD Sector, BYTE *pDat )
 *
 * PreCondition:    None
 *
 * Input:           Sector - Sector location to perform the write
 *                  pDat - Buffer containing data to be written to 
 *                         to the sector location on the media.
 *
 * Output:          TRUE - Write to media was successful
 *                  FALSE - Failure to write to media
 *
 * Side Effects:    None
 *
 * Overview:        User-called function to write a specific sector on
 *                  the media.
 *
 * Note:            None
 *****************************************************************************/
PUBLIC BOOL SDWriteSector( DWORD Sector, BYTE *pDat );

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
PUBLIC BOOL SDCallBack(unsigned long cmd, void *passed_data, void *return_data);


#endif
