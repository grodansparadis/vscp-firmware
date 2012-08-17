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
#ifndef _SD_CARD_SPI_HARDWARE_HEADER_FILE
#define _SD_CARD_SPI_HARDWARE_HEADER_FILE

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


        #define SD_CD                   (mPORTFReadBits(BIT_0))             // Card detect - Read Only
        #define SDSetCDDirection()      mPORTFSetPinsDigitalIn(BIT_0)


        #define SD_WE                   (mPORTFReadBits(BIT_1))               // Write enable - Read Only
        #define SDSetWEDirection()      mPORTFSetPinsDigitalIn(BIT_1)

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




#endif   /* _MEDIASD_H_ */

