/**********************************************************************
 *
 * Filename:    vscp-crc.h
 *
 * Description: Shared VSCP CRC-CCITT definitions.
 *
 * Notes:
 *
 *
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/

#ifndef _vscp_crc_h
#define _vscp_crc_h

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/*
 * The shared VSCP CRC implementation is fixed to CRC-CCITT.
 * Keep CRC_CCITT defined for compatibility with existing includes.
 */
#ifndef CRC_CCITT
#define CRC_CCITT
#endif

typedef unsigned short crc;

#define CRC_NAME          "CRC-CCITT"
#define POLYNOMIAL        0x1021
#define INITIAL_REMAINDER 0xFFFF
#define FINAL_XOR_VALUE   0x0000
#define REFLECT_DATA      FALSE
#define REFLECT_REMAINDER FALSE
#define CHECK_VALUE       0x29B1

void
crcInit(void);
crc
crcSlow(unsigned char const message[], int nBytes);
crc
crcFast(unsigned char const message[], int nBytes);

#ifdef __cplusplus
}
#endif

#endif /* _vscp_crc_h */