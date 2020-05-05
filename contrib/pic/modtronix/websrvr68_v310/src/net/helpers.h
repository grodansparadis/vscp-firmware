/**
 * @brief           Helper functions for Modtronix TCP/IP Stack
 * @file            helpers.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    stacktsk.h
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 *
 *
 * @section description Description
 **********************************
 * This module contains general helper functions.
 *
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on v2.20.04.01
 * of the Microchip TCP/IP stack. The original code base is owned by Microchip
 * Technology Incorporated (the Company), and is protected under applicable
 * copyright laws. The modifications are owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller based products manufactured
 * by Modtronix Engineering. The code may be modified and can be used free of charge
 * for non commercial and commercial applications. All rights are reserved. Any use
 * in violation of the foregoing restrictions may subject the user to criminal
 * sanctions under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 ********************************************************************
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     5/17/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 ********************************************************************/

#ifndef HELPERS_H
#define HELPERS_H

#include "net\compiler.h"


WORD    swaps(WORD v);
DWORD   swapl(DWORD v);

WORD    CalcIPChecksum(BYTE *buffer, WORD len);

/**
 * This function performs checksum calculation in MAC buffer itself.
 *
 * @preCondition TCPInit() is already called  AND  MAC buffer pointer set to start of buffer
 *
 * @param len Total number of bytes to calculate checksum for
 *
 * @return 16-bit checksum as defined by rfc 793
 */
WORD    CalcIPBufferChecksum(WORD len);

#if defined(__C30__) || defined(HI_TECH_C)

/**
 * The function converts an unsigned integer (16 bits) into a null terminated decimal string.
 *
 * @preCondition TCPInit() is already called  AND  MAC buffer pointer set to start of buffer
 *
 * @param Value Unsigned integer to be converted
 *
 * @param Buffer Pointer to a location to write the string
 *
 * @return *Buffer: Receives the resulting string
 */
void itoa(unsigned int Value, char *Buffer);

/**
 * Convert all lower case characters in a string to upper case.
 * This function converts all lower case characters in str to upper case characters.
 * All characters that are not lower case (a to z) are not affected.
 *
 * @param s Pointer to NULL terminated string
 *
 * @return This function returns the given string, containing the converted string
 */
char *strupr (char *s);
#endif

/**
 * Converts the given 2 character hex string to a byte value. Only the first two character of the given
 * string is checked. No NULL terminating character is required.
 * 
 * @param s Input string. First two characters of string must contain hex string. For example "8F" or "8f"
 * @param c Will contain the byte representation of the given string on return.
 *
 * @return 1 if a valid hex value was found, or 0 if s did NOT contain a valid hex value
 */
BYTE hextob(char * s, BYTE * c);

/**
 * 
 * @param b An ascii byte (always uppercase) between '0'-'9' or 'A'-'F' that
 * corresponds to the lower 4 bits of the input byte.
 * ex: b = 0xAE, btohexa_low() returns 'E'
 *
 * @return the resulting ASCII character
 */
BYTE btohexa_low(BYTE b);


/**
 * Convert the upper nibble (upper 4 bits) to an ASCII character.
 * @param b An ascii byte (always uppercase) between '0'-'9' or 'A'-'F' that
 * corresponds to the upper 4 bits of the input byte.
 * ex: b = 0xAE, btohexa_high() returns 'A'
 *
 * @return the resulting ASCII character
 */
BYTE btohexa_high(BYTE b);


/**
 * Compare two strings.
 *
 * @param strRam Pointer to null terminated string contained in RAM
 * @param strEe Address (offset) in EEPROM of null terminated string
 *
 * @return Returns 0 if both strings are the same, else non 0
 */
BYTE strcmpee2ram(char * strRam, WORD strEe);


/**
 * Copies the source string (in EEPROM) to the given destination in buffer in RAM.
 * Characters in strSrc are copied up to, and including, the terminating null character
 * strSrc.
 *
 * @param strDst Pointer to RAM buffer where EEPROM string will be copied to
 * @param strSrc Source address (offset) in the EEPROM
 * @param len The maximum string length that is allowed to be written (excluding NULL terminating char).
 *            If the source string is longer then this value, the destination will be truncated
 *            with a NULL!
 *            For example, if len = 8, then that string can have a maximum of 8 characters = 8 + NULL.
 *            The destination must always have len + 1 positions available!
 *
 * @return Returns the number of bytes written, excluding the terminating null. For example, if
 * 8 is returned, then 8 characters + NULL terminating char were written.
 */
BYTE strcpyee2ram(char * strDst, WORD strSrc, BYTE len);

/**
 * Test if given string in RAM begins with given ROM string, and Ignores Case.
 * The ROM string has to be in upper case. 
 *
 * @param strRam Pointer to string contained in RAM. This tring does not have to be NULL
 *               terminated.
 * @param strRom Pointer to null terminated string contained in ROM
 *
 * @return Returns TRUE if RAM string begins with ROM string
 */
BOOL strBeginsWithIC(char * strRam, ROM char * strRom);


/**
 * Copies the source string (in RAM) to the given destination in EEPROM. 
 * Characters in strSrc are copied up to, and including, the terminating null character
 * strSrc.
 *
 * @param strSrc Pointer to null terminated Source string contained in RAM.
 * @param strDst Destination address (offset) in the EEPROM
 * @param len The maximum string length that is allowed to be written (excluding NULL terminating char).
 *            If the source string is longer then this value, the destination will be truncated
 *            with a NULL! To disable length checking, set this value to 255.
 *            For example, if len = 8, then that string can have a maximum of 8 characters = 8 + NULL.
 *            The destination must always have len + 1 positions available!
 *
 * @return Returns the number of bytes written, excluding the terminating null. For example, if
 * 8 is returned, then 8 characters + NULL terminating char were written.
 */
BYTE strcpyram2ee(WORD strDst, char * strSrc, BYTE len);
#endif

/**
 * Fills the given number of bytes in the given array with 0
 *
 * @param dest Pointer to memory area in RAM that has to be set to 0
 * @param size Number of consecutive bytes to set to 0
 */
void memclr(void * dest, WORD size);

/**
 * Encode given ascii characters to base64 6-bit characters, adding padding characters where required
 *
 * @param out   Buffer where output is written to
 * @param in    Input ascii string
 * @param inlen Length of input ascii string
 */
void base64Encode(char *out, char *in, BYTE inlen);

/**
 * Get the hex value for the given alpha numrical character. The returned value will be from
 * 0 - 15. If the given byte is not a alphe numerical value, 0xff is returned!
 */
BYTE getHexNibble(BYTE c);

