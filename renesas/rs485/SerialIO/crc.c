/**********************************************************************
 *
 * Filename:    crc.c
 * 
 * Description: Medium fast implementations of the CRC standards.
 *
 **********************************************************************/
 
#include "crc.h"

crc crcInit(void)
{
	return (crc)INITIAL_REMAINDER;
}

crc crcAccumulate(crc remainder, unsigned char byte)
{
	crc		new;

	new = (unsigned char)(remainder >> 8) | (remainder << 8);
	new ^= byte;
	new ^= (unsigned char)(new & 0xff) >> 4;
	new ^= new << 12;
	new ^= (new & 0xff) << 5;
	
	return new;
}

crc crcFinal(crc remainder)
{
	return remainder ^ FINAL_XOR_VALUE;
}

crc crcMedium(unsigned char message[], int nBytes)
{
	crc		remainder = crcInit();
	int		byte;
	
	for (byte = 0; byte < nBytes; byte++)
	{
		remainder = crcAccumulate(remainder, message[byte]);
	}
	
	return crcFinal(remainder);
}

