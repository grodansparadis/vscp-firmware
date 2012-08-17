//
//  $Id: eeprom.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/i2c/eeprom.h $
//

#ifndef _EEPROM_H_
#define _EEPROM_H_

//
//
//
#define EEPROM_ADDRESS  (0xa0)
#define EEPROM_SIZE     (131072)
#define EEPROM_PAGESIZE (256)

//
//
//
void eepromInit (void);
int eepromSetAddress (U32 address);
U32 eepromGetAddress (void);
int eepromRead (U8 *buffer, U32 bufferLength);
int eepromReadAddress (U32 address, U8 *buffer, U32 bufferLength);
int eepromWrite (U8 *buffer, U32 bufferLength);
int eepromWriteAddress (U32 address, U8 *buffer, U32 bufferLength);
int eepromFillAddress (U32 address, U32 length, U8 fillValue);

#endif
