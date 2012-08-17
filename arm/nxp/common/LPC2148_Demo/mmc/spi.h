//
//  $Id: spi.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/mmc/spi.h $
//

#ifndef _MMCLLSPI1_H_
#define _MMCLLSPI1_H_

#include "sysdefs.h"

//
//  Hardware depends definitions
//
#define IDENTIFICATIONMODECLOCK   400000l
#define SYSTEMPERIPHERIALCLOCK  48000000l
#define SPI_DLY_1MSEC                1000

//
//
//
inline int spiHardwareCardPresent (void);
inline int spiHardwareWriteProtected (void);
void spiChipSelect (BOOL select);
inline BOOL spiPresent (void);
inline BOOL spiWriteProtect (void);
U32 spiSetClockFreq (U32 frequency);
void spiInit (void);
U8 spiTransferByte (U8 c);
U8 spiWaitReady (void);
void spiSendBlock (const U8 *pData, U32 size);
void spiReceiveBlock (U8 *pData, U32 size);
void spiDelay1ms (U32 delay);

#endif
