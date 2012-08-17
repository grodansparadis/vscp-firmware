//
//  $Id: iap.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/iap/iap.h $
//

#ifndef _IAP_H_
#define _IAP_H_

//
//
//
#define IAP_RESULT_X_NOTSAFEREGION      (IAP_RESULT_LAST + 1)
#define IAP_RESULT_X_NOSAFEREGIONAVAIL  (IAP_RESULT_LAST + 2)

//
//
//
void iapInit (void);
int iapSectorToAddress (int sectorNumber, unsigned long *address, int *sectorSize);
int iapAddressToSector (unsigned long address);
int iapIsSafeAddress (unsigned long address);;
int iapIsSafeSector (int sector);
int iapFindSafeSector (void);
int iapIsValidSector (int sector);
int iapGetErrno (void);
const char *iapStrerror (int err);
int iapPrepareSectors (int startingSector, int endingSector);
int iapFillSectors (int startingSector, int endingSector, int byte);
int iapWriteSectors (unsigned int address, unsigned char *buffer, int bufferLen);
int iapEraseSectors (int startingSector, int endingSector);
int iapBlankCheckSectors (int startingSector, int endingSector);
unsigned int iapReadPartID (void);
unsigned int iapReadBootCodeVersion (void);
int iapCompare (unsigned int address, unsigned char *buffer, int bufferLen);
void iapISP (void);

#endif
