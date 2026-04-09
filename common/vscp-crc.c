/**********************************************************************
 *
 * Filename:    vscp-crc.c
 *
 * Description: Slow and fast CRC-CCITT implementations.
 *
 * Notes:       This shared VSCP CRC implementation is fixed to the
 *              CRC-CCITT standard defined in crc.h.
 *
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 *
 **********************************************************************/

#include "vscp-crc.h"

#define WIDTH  (8 * sizeof(crc))
#define TOPBIT (1 << (WIDTH - 1))

#define REFLECT_DATA(X)      (X)
#define REFLECT_REMAINDER(X) (X)

static crc crcTable[256];

crc
crcSlow(unsigned char const message[], int nBytes)
{
  crc remainder = INITIAL_REMAINDER;
  int byte;
  unsigned char bit;

  for (byte = 0; byte < nBytes; ++byte) {
    remainder ^= (REFLECT_DATA(message[byte]) << (WIDTH - 8));

    for (bit = 8; bit > 0; --bit) {
      if (remainder & TOPBIT) {
        remainder = (remainder << 1) ^ POLYNOMIAL;
      }
      else {
        remainder = (remainder << 1);
      }
    }
  }

  return (REFLECT_REMAINDER(remainder) ^ FINAL_XOR_VALUE);
}

void
crcInit(void)
{
  crc remainder;
  int dividend;
  unsigned char bit;

  for (dividend = 0; dividend < 256; ++dividend) {
    remainder = dividend << (WIDTH - 8);

    for (bit = 8; bit > 0; --bit) {
      if (remainder & TOPBIT) {
        remainder = (remainder << 1) ^ POLYNOMIAL;
      }
      else {
        remainder = (remainder << 1);
      }
    }

    crcTable[dividend] = remainder;
  }
}

crc
crcFast(unsigned char const message[], int nBytes)
{
  crc remainder = INITIAL_REMAINDER;
  unsigned char data;
  int byte;

  for (byte = 0; byte < nBytes; ++byte) {
    data = REFLECT_DATA(message[byte]) ^ (remainder >> (WIDTH - 8));
    remainder = crcTable[data] ^ (remainder << 8);
  }

  return (REFLECT_REMAINDER(remainder) ^ FINAL_XOR_VALUE);
}