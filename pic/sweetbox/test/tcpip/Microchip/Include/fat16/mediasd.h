/******************************************************************************
 *
 *       SECURE DIGITAL MEDIA INTERFACE HEADER FILE
 *
 ******************************************************************************
 * FileName:        MediaSD.h
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
 * ****************************************************************************
 *
 * $Id: MediaSD.h,v 1.4 2006/10/24 16:37:51 C12923 Exp $
 *
 * $Name:  $
 *
 *****************************************************************************/
#ifndef _MEDIASD_H_
#define _MEDIASD_H_

#include "generic.h"
#include "fat.def"
#include <plib.h>

#if defined(MEDIASD_IF_SPI1)
#elif defined(MEDIASD_IF_SPI2)
#else
   #error Media SPI port is not defined. See "MEDIASD_IF_XXX" define in fat.def
#endif

#if defined (EXPLORER16_REV_5)
#define SetSPILatch()           {   mPORTBSetPinsDigitalOut(BIT_12 | BIT_13); mPORTBSetBits(BIT_12 | BIT_13);   }
#else
#define SetSPILatch()
#endif

// Hardware interface to SD Card. Modify it as per your hardware.
#if defined(EXPLORER16_REV_1) | defined(EXPLORER16_REV_5)

    #if defined(MEDIASD_IF_SPI1)


        #define _SetChipSelect(on)      {   SetSPILatch();\
                                            mPORTBSetPinsDigitalOut(BIT_1);\
                                            if(on)\
                                                mPORTBClearBits(BIT_1);\
                                            else\
                                                mPORTBSetBits(BIT_1);\
                                        }


        #define SD_CD                   (mPORTFReadBit(BIT_0))             // Card detect - Read Only
        #define SDSetCDDirection()      mPORTFInputConfig(BIT_0)


        #define SD_WE                   (mPORTFReadBit(BIT_1))               // Write enable - Read Only
        #define SDSetWEDirection()      mPORTFInputConfig(BIT_1)

    #elif defined(MEDIASD_IF_SPI2)

        #define _SetChipSelect(on)      {\
                                            mPORTBSetPinsDigitalOut(BIT_9);\
                                            if(on)\
                                                mPORTBClearBits(BIT_9);\
                                            else\
                                                mPORTBSetBits(BIT_9);\
                                        }


        #define SD_CD                   (mPORTGReadBits(BIT_0))               // Card detect - Read Only
        #define SDSetCDDirection()      mPORTGSetPinsDigitalIn(BIT_0)

        #define SD_WE                   (mPORTGReadBits(BIT_1))               // Write enable - Read Only
        #define SDSetWEDirection()      mPORTGSetPinsDigitalIn(BIT_1)

    #else
       #error Media SPI port is not defined. See "MEDIASD_IF_XXX" define in fat.def
    #endif

#elif defined(SHOW_N_TELL_BOARD)
        
#error "Please place these defines in fat.def, get the latest version of fat.tmpl for an example"

        #define _SetChipSelect(on)      {\
                                            mPORTBSetPinsDigitalOut(BIT_1);\
                                            if(on)\
                                                mPORTBClearBits(BIT_1);\
                                            else\
                                                mPORTBSetBits(BIT_1);\
                                        }

        #define SD_CD                   (mPORTDReadBits(BIT_3))               // Card detect - Read Only
        #define SDSetCDDirection()      mPORTDSetPinsDigitalIn(BIT_3)

        #define SD_WE                   (mPORTGReadBits(BIT_13))               // Write enable - Read Only
        #define SDSetWEDirection()      mPORTGSetPinsDigitalIn(BIT_13)

#elif defined(FSE_DEMO_BOARD)

#error "Please place these defines in fat.def, get the latest version of fat.tmpl for an example"

        #define _SetChipSelect(on)      {\
                                            mPORTGSetPinsDigitalOut(BIT_9);\
                                            if(on)\
                                                mPORTGClearBits(BIT_9);\
                                            else\
                                                mPORTGSetBits(BIT_9);\
                                        }

        #define SD_CD                   (mPORTDReadBits(BIT_3))               // Card detect - Read Only
        #define SDSetCDDirection()      mPORTDSetPinsDigitalIn(BIT_3)

        #define SD_WE                   (mPORTDReadBits(BIT_1))               // Write enable - Read Only
        #define SDSetWEDirection()      mPORTDSetPinsDigitalIn(BIT_1)


#elif defined (STARTER_KIT_NETWORK_BOARD)

        #define mProvidePower()         {\
	                                        mPORTGSetPinsDigitalOut(BIT_15);\
	                                        mPORTGClearBits(BIT_15);\
                                        }

        #define _SetChipSelect(on)      {\
                                            mPORTBSetPinsDigitalOut(BIT_9);\
                                            if(on)\
                                                mPORTBClearBits(BIT_9);\
                                            else\
                                                mPORTBSetBits(BIT_9);\
                                        }


        #define SD_CD                   (mPORTGReadBits(BIT_0))               // Card detect - Read Only
        #define SDSetCDDirection()      mPORTGSetPinsDigitalIn(BIT_0)

        #define SD_WE                   (mPORTGReadBits(BIT_1))               // Write enable - Read Only
        #define SDSetWEDirection()      mPORTGSetPinsDigitalIn(BIT_1)

#endif


#define SD_CMD_IDLE             0
#define SD_CMD_SEND_OP_COND     1
#define SD_CMD_CSD              9
#define SD_CMD_SET_BLOCK_LEN   16
#define SD_CMD_READ_BLOCK      17
#define SD_CMD_WRITE_BLOCK     24

typedef union _MMCstate
{
    struct {
       BYTE isSDMMC : 1;   // Set if it is a SDMMC
       BYTE isWP    : 1;   // Set if it is write protected
    };
    BYTE  _byte;
} MMCSTATE;

typedef union _CARDSTATUS
{
    struct
    {
       BYTE  CS_Inserted:1;  // Card is inserted and initialized
       BYTE  CS_Reset:1;     // Card has just been reset
       BYTE  CS_SDMMC:1;     // Card is of the SDMMC type
       BYTE  CS_WP:1;        // Card is write Protected
       BYTE  CS_Failure:1;   // There was a failure initializing the card
    };
    WORD _word;
} CARDSTATUS;

PUBLIC BOOL MediaInit( CARDSTATUS *cs );
#define MediaIsPresent() (!SD_CD)
#define MediaIsWriteProtected()	(SD_WE)
PUBLIC BOOL MediaReadSector( DWORD Sector, BYTE *pDat );
PUBLIC BOOL MediaWriteSector( DWORD Sector, BYTE *pDat );
PUBLIC void MediaShutDown( void );
PUBLIC BYTE ReadByte( BYTE *pBuffer, WORD index );
PUBLIC WORD ReadWord( BYTE *pBuffer, WORD index );
PUBLIC DWORD ReadDWord( BYTE *pBuffer, WORD index );

#define MEDIA_SECTOR_SIZE 512


#endif   /* _MEDIASD_H_ */

