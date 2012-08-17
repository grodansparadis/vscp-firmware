//
//  $Id: eeprom.c 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/i2c/eeprom.c $
//

#include "FreeRTOS.h"

#include <string.h>

#include "i2c.h"
#include "eeprom.h"

//
//
//
static U32 rwAddress;
static U8 deviceAddress = EEPROM_ADDRESS;

//
//
//
void eepromInit (void)
{
}

int eepromSetAddress (U32 address)
{
  int r = 0;

  if (address >= EEPROM_SIZE)
    r = -1;

  address %= EEPROM_SIZE;

  if ((rwAddress = address) >= 65536)
    deviceAddress |= 0x02;
  else
    deviceAddress &= ~0x02;

  return r;
}

static int eepromSetAddressEx (U32 address, U8 *buffer)
{
  int r;

  if ((r = eepromSetAddress (address)))
    return r;

  buffer [0] = rwAddress >> 8;
  buffer [1] = rwAddress;

  return 0;
}

U32 eepromGetAddress (void)
{
  return rwAddress;
}

//
//
//
int eepromRead (U8 *buffer, U32 bufferLength)
{
  int r;

  buffer [0] = rwAddress >> 8;
  buffer [1] = rwAddress;

  if (!(r = i2cWriteReadBufferPoll (deviceAddress, buffer, sizeof (U16), bufferLength)))
    eepromSetAddress (eepromGetAddress () + bufferLength);

  return r;
}

int eepromReadAddress (U32 address, U8 *buffer, U32 bufferLength)
{
  int r;

  if ((r = eepromSetAddress (address)))
    return r;

  return eepromRead (buffer, bufferLength);
}

//
//
//
int eepromWrite (U8 *buffer, U32 bufferLength)
{
  int r;

  buffer [0] = rwAddress >> 8;
  buffer [1] = rwAddress;

  if (!(r = i2cWriteBufferPoll (deviceAddress, buffer, bufferLength + 2)))
    eepromSetAddress (eepromGetAddress () + bufferLength);

  return r;
}

int eepromWriteAddress (U32 address, U8 *buffer, U32 bufferLength)
{
  int r;

  if ((r = eepromSetAddress (address)))
    return r;

  return eepromWrite (buffer, bufferLength);
}

//
//
//
static int eepromFillAddressCommon (U32 address, U8 *buffer, U32 bufferLength)
{
  int r;

  if (!(r = eepromSetAddressEx (address, buffer)))
    r = i2cWriteBufferPoll (deviceAddress, buffer, bufferLength + 2);

  return r;
}

int eepromFillAddress (U32 address, U32 bufferLength, U8 fillValue)
{
  int r;
  U32 l;
  U32 i;
  U8 buffer [EEPROM_PAGESIZE + 2];

  memset (buffer, fillValue, sizeof (buffer));

  l = (EEPROM_PAGESIZE - ((address + EEPROM_PAGESIZE) % EEPROM_PAGESIZE)) % EEPROM_PAGESIZE;
  l = MIN (l, bufferLength);

  if (l && (r = eepromFillAddressCommon (address, buffer, l)))
    return r;

  address += l;
  bufferLength -= l;
  l = bufferLength - (bufferLength % EEPROM_PAGESIZE);

  for (i = 0; i < l; i += EEPROM_PAGESIZE, address += (sizeof (buffer) - 2), bufferLength -= (sizeof (buffer) - 2))
    if ((r = eepromFillAddressCommon (address, buffer, sizeof (buffer) - 2)))
      return r;

  if (bufferLength && (r = eepromFillAddressCommon (address, buffer, bufferLength)))
    return r;

  return 0;
}

