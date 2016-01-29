/**********************************************************************
 *
 * Filename:    crc.h
 * 
 * Description: A header file describing the various CRC standards.
 *
 * Notes:       
 **********************************************************************/

#ifndef _crc_h_
#define _crc_h_

#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif

typedef unsigned short  crc;

#define CRC_NAME			"CRC-CCITT"
#define POLYNOMIAL			0x1021
#define INITIAL_REMAINDER	0x1D0F
#define FINAL_XOR_VALUE		0x0000

crc	crcInit(void);
crc	crcAccumulate(crc remainder, unsigned char byte);
crc crcFinal(crc remainder);

crc   crcMedium( unsigned char const message[], int nBytes );

#endif /* _crc_h */

