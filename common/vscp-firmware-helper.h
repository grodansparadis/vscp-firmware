/**
 * @brief           VSCP firmware helper functions
 * @file            vscp_firmware_helper.h
 * @author          Ake Hedman, The VSCP Project, www.vscp.org
 *
 * @section description Description
 **********************************
 * This module contains the definitions for the
 * available VSCP class id's
 *********************************************************************/

/* ******************************************************************************
 * VSCP (Very Simple Control Protocol)
 * http://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2025 Ake Hedman,
 * The VSCP Project <info@grodansparadis.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *  This file is part of VSCP - Very Simple Control Protocol
 *  http://www.vscp.org
 *
 * ******************************************************************************
 */

#ifndef __VSCP_FIRMWARE_HELPER_H__
#define __VSCP_FIRMWARE_HELPER_H__

#include "vscp-compiler.h"
#include "vscp-projdefs.h"

#include <vscp-aes.h>
#include <vscp.h>

/* Macros */

/* This macro construct a signed integer from two unsigned chars in a safe way */
#if !defined(construct_signed16)
#define construct_signed16(msb, lsb) ((int16_t)((((uint16_t)msb) << 8) + \
                                                (uint16_t)lsb))
#endif

/* This macro construct a unsigned integer from two unsigned chars in a safe way */
#if !defined(construct_unsigned16)
#define construct_unsigned16(msb, lsb) ((uint16_t)((((uint16_t)msb) << 8) + \
                                                   (uint16_t)lsb))
#endif

/* This macro construct a signed long from four unsigned chars in a safe way */
#if !defined(construct_signed32)
#define construct_signed32(b0, b1, b2, b3) ((int32_t)((((uint32_t)b0) << 24) + \
                                                      (((uint32_t)b1) << 16) + \
                                                      (((uint32_t)b2) << 8) +  \
                                                      (uint32_t)b3))
#endif

/* This macro construct a unsigned long from four unsigned chars in a safe way */
#if !defined(construct_unsigned32)
#define construct_unsigned32(b0, b1, b2, b3) ((uint32_t)((((uint32_t)b0) << 24) + \
                                                         (((uint32_t)b1) << 16) + \
                                                         (((uint32_t)b2) << 8) +  \
                                                         (uint32_t)b3))
#endif

/*  byte swapping */

#if !defined(VSCP_UINT16_SWAP_ALWAYS)
#define VSCP_UINT16_SWAP_ALWAYS(val) \
  ((uint16_t)((((uint16_t)(val) & (uint16_t)0x00ffU) << 8) | (((uint16_t)(val) & (uint16_t)0xff00U) >> 8)))
#endif

#if !defined(VSCP_INT16_SWAP_ALWAYS)
#define VSCP_INT16_SWAP_ALWAYS(val) \
  ((int16_t)((((uint16_t)(val) & (uint16_t)0x00ffU) << 8) | (((uint16_t)(val) & (uint16_t)0xff00U) >> 8)))
#endif

#if !defined(VSCP_UINT32_SWAP_ALWAYS)
#define VSCP_UINT32_SWAP_ALWAYS(val)                              \
  ((uint32_t)((((uint32_t)(val) & (uint32_t)0x000000ffU) << 24) | \
              (((uint32_t)(val) & (uint32_t)0x0000ff00U) << 8) |  \
              (((uint32_t)(val) & (uint32_t)0x00ff0000U) >> 8) |  \
              (((uint32_t)(val) & (uint32_t)0xff000000U) >> 24)))
#endif

#if !defined(VSCP_INT32_SWAP_ALWAYS)
#define VSCP_INT32_SWAP_ALWAYS(val)                              \
  ((int32_t)((((uint32_t)(val) & (uint32_t)0x000000ffU) << 24) | \
             (((uint32_t)(val) & (uint32_t)0x0000ff00U) << 8) |  \
             (((uint32_t)(val) & (uint32_t)0x00ff0000U) >> 8) |  \
             (((uint32_t)(val) & (uint32_t)0xff000000U) >> 24)))
#endif

/*  machine specific byte swapping */

#if !defined(VSCP_UINT64_SWAP_ALWAYS)
#define VSCP_UINT64_SWAP_ALWAYS(val)                                       \
  ((uint64_t)((((uint64_t)(val) & (uint64_t)(0x00000000000000ff)) << 56) | \
              (((uint64_t)(val) & (uint64_t)(0x000000000000ff00)) << 40) | \
              (((uint64_t)(val) & (uint64_t)(0x0000000000ff0000)) << 24) | \
              (((uint64_t)(val) & (uint64_t)(0x00000000ff000000)) << 8) |  \
              (((uint64_t)(val) & (uint64_t)(0x000000ff00000000)) >> 8) |  \
              (((uint64_t)(val) & (uint64_t)(0x0000ff0000000000)) >> 24) | \
              (((uint64_t)(val) & (uint64_t)(0x00ff000000000000)) >> 40) | \
              (((uint64_t)(val) & (uint64_t)(0xff00000000000000)) >> 56)))
#endif

#if !defined(VSCP_INT64_SWAP_ALWAYS)
#define VSCP_INT64_SWAP_ALWAYS(val)                                       \
  ((int64_t)((((uint64_t)(val) & (uint64_t)(0x00000000000000ff)) << 56) | \
             (((uint64_t)(val) & (uint64_t)(0x000000000000ff00)) << 40) | \
             (((uint64_t)(val) & (uint64_t)(0x0000000000ff0000)) << 24) | \
             (((uint64_t)(val) & (uint64_t)(0x00000000ff000000)) << 8) |  \
             (((uint64_t)(val) & (uint64_t)(0x000000ff00000000)) >> 8) |  \
             (((uint64_t)(val) & (uint64_t)(0x0000ff0000000000)) >> 24) | \
             (((uint64_t)(val) & (uint64_t)(0x00ff000000000000)) >> 40) | \
             (((uint64_t)(val) & (uint64_t)(0xff00000000000000)) >> 56)))
#endif

#ifdef __BIG_ENDIAN__
#if !defined(VSCP_UINT16_SWAP_ON_BE)
#define VSCP_UINT16_SWAP_ON_BE(val) VSCP_UINT16_SWAP_ALWAYS(val)
#define VSCP_INT16_SWAP_ON_BE(val)  VSCP_INT16_SWAP_ALWAYS(val)
#define VSCP_UINT16_SWAP_ON_LE(val) (val)
#define VSCP_INT16_SWAP_ON_LE(val)  (val)
#define VSCP_UINT32_SWAP_ON_BE(val) VSCP_UINT32_SWAP_ALWAYS(val)
#define VSCP_INT32_SWAP_ON_BE(val)  VSCP_INT32_SWAP_ALWAYS(val)
#define VSCP_UINT32_SWAP_ON_LE(val) (val)
#define VSCP_INT32_SWAP_ON_LE(val)  (val)
#define VSCP_UINT64_SWAP_ON_BE(val) VSCP_UINT64_SWAP_ALWAYS(val)
#define VSCP_UINT64_SWAP_ON_LE(val) (val)
#define VSCP_INT64_SWAP_ON_BE(val)  VSCP_INT64_SWAP_ALWAYS(val)
#define VSCP_INT64_SWAP_ON_LE(val)  (val)
#endif
#else
#if !defined(VSCP_UINT16_SWAP_ON_LE)
#define VSCP_UINT16_SWAP_ON_LE(val) VSCP_UINT16_SWAP_ALWAYS(val)
#define VSCP_INT16_SWAP_ON_LE(val)  VSCP_INT16_SWAP_ALWAYS(val)
#define VSCP_UINT16_SWAP_ON_BE(val) (val)
#define VSCP_INT16_SWAP_ON_BE(val)  (val)
#define VSCP_UINT32_SWAP_ON_LE(val) VSCP_UINT32_SWAP_ALWAYS(val)
#define VSCP_INT32_SWAP_ON_LE(val)  VSCP_INT32_SWAP_ALWAYS(val)
#define VSCP_UINT32_SWAP_ON_BE(val) (val)
#define VSCP_INT32_SWAP_ON_BE(val)  (val)
#define VSCP_UINT64_SWAP_ON_LE(val) VSCP_UINT64_SWAP_ALWAYS(val)
#define VSCP_UINT64_SWAP_ON_BE(val) (val)
#define VSCP_INT64_SWAP_ON_LE(val)  VSCP_INT64_SWAP_ALWAYS(val)
#define VSCP_INT64_SWAP_ON_BE(val)  (val)
#endif
#endif

#if !defined(Swap8Bytes)
#define Swap8Bytes(val)                                                                                               \
  ((((val) >> 56) & 0x00000000000000FF) | (((val) >> 40) & 0x000000000000FF00) |                                      \
   (((val) >> 24) & 0x0000000000FF0000) | (((val) >> 8) & 0x00000000FF000000) | (((val) << 8) & 0x000000FF00000000) | \
   (((val) << 24) & 0x0000FF0000000000) | (((val) << 40) & 0x00FF000000000000) | (((val) << 56) & 0xFF00000000000000))
#endif

/*!
 * @name Min/max macros
 * @{
 */
#if !defined(MIN)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#if !defined(MAX)
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
/* @} */

/**
 * @brief VSCP TCP/IP link interface description
 * Describes one interface
 */
typedef struct vscp_interface_info {
  uint16_t idx;
  uint16_t type;
  uint8_t guid[16];
  char description[64];
} vscp_interface_info_t;

/**
 * @fn vscp_fwhlp_isLittleEndian
 * @brief Check if little endian system
 * @return Non zero fo rlittle endian system.
 */
int
vscp_fwhlp_isLittleEndian(void);

/**
 * @fn vscp_fwhlp_isBigEndian
 * @brief Check if big endian system
 * @return int Return Non zero for big endian system.
 */
int
vscp_fwhlp_isBigEndian(void);

/**
    Convert ASCII substring to unsigned long number
    Copyright (C) 1998, 1999  Henry Spencer.
    http://www.koders.com/c/fid83F5660A86069B2E2D29B9D3FC9013F76A9BCEB7.aspx

    @param src NULL terminated string to convert.
    @param srclen length of string, 0 means strlen( src )
    @param base The base for the conversion, 0 means figure it out.
    @param resultp Pointer that holds unsigned long result after conversion.
    @return  VSCP_ERROR_SUCCESS for success, else VSCP_ERROR_ERROR.
*/
int
vscp_fwhlp_a2ul(const char* src, uint8_t srclen, uint8_t base, uint32_t* resultp);

/**
    Convert decimal byte to hex string
    @param d Decimal byte to convert
    @param pBuf Pointer to string buffer that will hold the result.
    @param len Number of digits to convert.
*/
void
vscp_fwhlp_dec2hex(uint8_t d, char* pBuf, uint16_t len);

/**
    Convert hexadecimal integer to a decimal value
    @param pHex Pointer to hexadecimal string.
    @return Converted value in decimal form.
*/
uint32_t
vscp_fwhlp_hex2dec(const char* pHex);

/**
 * @brief Convert one hex character
 *
 * @param c Hex character to convert (0-f)
 * @return unsigned char Hex value for character (0-15)
 */
unsigned char
vscp_fwhlp_hexchar(char c);

/**
 * @brief Create string with hex values from binary values in buffer
 *
 * @param output Result buffer for output string
 * @param outLength Size of result buffer
 * @param buf Buf with binary values
 * @param length Size of buffer
 */

void
vscp_fwhlp_bin2hex(char* output, size_t outLength, const unsigned char* buf, size_t length);

/**
 * @brief Convert a string of hex values to binary values in buffer
 *
 * @param buf Buffer to fill
 * @param length Size of buffer
 * @param s String to convert
 * @return int Resulting length
 */

int
vscp_fwhlp_hex2bin(unsigned char* buf, size_t length, const char* s);

/**
 * @fn vscp_fwhlp_strsubst
 * @brief Substitute all occurrences of a string in a string
 *
 * @param pNewStr The result string
 * @param len The len of the buffer for the result string
 * @param pStr The string to replace strings in.
 * @param pTarget This is the substring that should be replaced.
 * @param pReplace This is the string that should replace the substring.
 * @return char* A pointer to the result string or NULL if an error such as
 * to small result buffer len for the result.
 */

char*
vscp_fwhlp_strsubst(char* pNewStr, size_t len, const char* pStr, const char* pTarget, const char* pReplace);

/**
    Read a value (hex or decimal)
    @param pString Pointe to value. A hex value should be
    be preceded by "0x" or "0X"
    @return Converted value in decimal form.
*/
uint32_t
vscp_fwhlp_readStringValue(const char* pString);

/**
  Find substring of other string and return pointer to it
  @param haystack String to search in.
  @param needle String to search for.
  @return Pointer to substring or NULL if not found.
*/
char*
vscp_fwhlp_stristr(const char* haystack, const char* needle);

/**
 * @brief Parse MAC address on string form to binary array
 *
 * @param pmac Six byte binary array for MAC address.
 * @param strmac Null terminated string with MAC address.
 * @param endptr Points to first character after MAC string. Can be NULL.
 * @return int Returns VSCP_ERROR_SUCCESS if OK, else error code.
 */
int
vscp_fwhlp_parseMac(uint8_t* pmac, const char* strmac, char** endptr);

/**
    Get VSCP priority
    @param pEvent Pointer to VSCP event to set priority for.
    @return Priority (0-7) for event.
*/
unsigned char
vscp_fwhlp_getEventPriority(const vscpEvent* pev);

/**
    Get VSCP EventEx priority
    @param pEvent Pointer to VSCP event ex to set priority for.
    @return Priority (0-7) for event.
*/
unsigned char
vscp_fwhlp_getEventPriorityEx(const vscpEventEx* pex);

/**
    Check filter/mask to check if filter should be delivered

    filter ^ bit    mask    out
    ============================
        0           0       1    filter == bit, mask = don't care result = true
        0           1       1    filter == bit, mask = valid, result = true
        1           0       1    filter != bit, mask = don't care, result = true
        1           1       0    filter != bit, mask = valid, result = false

    Mask tells *which* bits that are of interest means
    it always returns true if bit set to zero (0=don't care).

    Filter tells the value for valid bits. If filter bit is == 1 the bits
    must be equal to get a true filter return.

    So a nill mask will let everything through

    A filter pointer set to NULL will let every event through.

    @return true if message should be delivered false if not.
    */
int
vscp_fwhlp_doLevel2Filter(const vscpEvent* pev, const vscpEventFilter* pFilter);

/**
    Check filter/mask to check if filter should be delivered

    filter ^ bit    mask    out
    ============================
        0           0       1    filter == bit, mask = don't care result = true
        0           1       1    filter == bit, mask = valid, result = true
        1           0       1    filter != bit, mask = don't care, result = true
        1           1       0    filter != bit, mask = valid, result = false

    Mask tells *which* bits that are of interest means
    it always returns true if bit set to zero (0=don't care).

    Filter tells the value for valid bits. If filter bit is == 1 the bits
    must be equal to get a true filter return.

    So a nill mask will let everything through

    A filter pointer set to NULL will let every event through.

    @return true if message should be delivered false if not.
    */
int
vscp_fwhlp_doLevel2FilterEx(const vscpEventEx* pex, const vscpEventFilter* pFilter);

/**
    Called by the system when a new connection is made.
    @param pdata  Pointer to user data
    @return VSCP_ERROR_SUCCESS if command was executed correctly,
*/

int
vscp_fwhlp_connect(const void* pdata);

/**
    @brief Called by the system when a connection is closed.
    @param pdata  Pointer to user data
    @return VSCP_ERROR_SUCCESS if command was executed correctly,

    Can be used to clean up any resources used by the connection.
*/

int
vscp_fwhlp_disconnect(const void* pdata);

/**
    Called by the system when no command is available
    @param pdata  Pointer to user data
    @return VSCP_ERROR_SUCCESS if command was executed correctly,
*/
int
vscp_fwhlp_idle_worker(const void* pdata);

/**
  @brief Parse a command from the client
  @param pdata  Pointer to user data
  @param cmd    Pointer to string that holds command to parse.
  @return VSCP_ERROR_SUCCESS if command was executed correctly,
    VSCP_ERROR_MISSING if command is not recognized, possible other error
    return if command was not executed correctly.
*/

int
vscp_fwhlp_parser(const void* pdata, const char* cmd);

/**
  @brief Parse GUID string into GUID array
  @param guid     Pointer to GUID array of 16 bytes to fill.
  @param strguid  Pointer to string that holds GUID to parse.
  @param endptr   Pointer to character after last parsed character.
                    Can be NULL.
  @return VSCP_ERROR_SUCCESS if GUID was parsed correctly,
*/

int
vscp_fwhlp_parseGuid(uint8_t* guid, const char* strguid, char** endptr);

/**
  @brief Write GUID to string
  @param strguid Buffer that the GUID string will be written to. This buffer
    must be at least 48 bytes long to hold GUID string on the form
    "FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF" including a terminating
    null character.
  @param guid Pointer to GUID array of 16 bytes to write.
  @return VSCP_ERROR_SUCCESS if GUID was written correctly,
*/

int
vscp_fwhlp_writeGuidToString(char* strguid, const uint8_t* guid);

/**
  @brief Parse a filter string and write data to a filter structure
  @param pfilter Pointer to filter structure to fill.
  @param strfilter Pointer to string that holds filter to parse.
  @return VSCP_ERROR_SUCCESS if filter was parsed correctly.
*/
int
vscp_fwhlp_parseFilter(vscpEventFilter* pfilter, const char* strfilter);

/**
  @brief Parse a filter mask and write data to a filter structure
  @param pfilter Pointer to filter structure to fill.
  @param strmask Pointer to string that holds mask to parse.
  @return VSCP_ERROR_SUCCESS if mask was parsed correctly.
*/
int
vscp_fwhlp_parseMask(vscpEventFilter* pfilter, const char* strmask);

/**
  @brief Parse an event on string form and write data to an event structure
  @param pev Pointer to event structure to fill.
  @param buf Pointer to string that holds event to parse.
  @return VSCP_ERROR_SUCCESS if event was parsed correctly.
*/
int
vscp_fwhlp_parseEvent(vscpEvent* pev, const char* buf);

/**
  @brief Parse an event on string form and write data to an event ex structure
  @param pev Pointer to event ex structure to fill.
  @param buf Pointer to string that holds event to parse.
  @return VSCP_ERROR_SUCCESS if event was parsed correctly.
*/
int
vscp_fwhlp_parseEventEx(vscpEventEx* pex, const char* streventex);

/**
  @brief Write event to string
  @param buf String buffer that will get event on string form
  @param size Size of string buffer
  @param pev Pointer to event
  @return VSCP_ERROR_SUCCESS if event was written correctly.
*/
int
vscp_fwhlp_eventToString(char* buf, size_t len, const vscpEvent* pev);

/**
  @brief Write event ex to string
  @param buf String buffer that will get event on string form
  @param size Size of string buffer
  @param pev Pointer to event
  @return VSCP_ERROR_SUCCESS if event was written correctly.
*/
int
vscp_fwhlp_eventToStringEx(char* buf, size_t len, const vscpEventEx* pex);

/**
  @brief Allocate a new event with zero data.
  @return Pointer to the new event if successful, NULL if not.
*/
vscpEvent*
vscp_fwhlp_newEvent(void);

/**
 * @brief Convert event to eventex
 *
 * @param pEventEx Pointer to eventex that will get result of conversion.
 * @param pEvent Pointer to event that should be converted
 * @return int VSCP_ERROR_SUCCESS is returned on success, otherwise an error code is returned.
 */

int
vscp_fwhlp_convertEventToEventEx(vscpEventEx* pEventEx, const vscpEvent* pEvent);

/**
 * @brief Convert eventex to event
 *
 * @param pEvent  Pointer to event that will get result of conversion.
 * @param pEventEx  Pointer to eventex that should be converted
 * @return int VSCP_ERROR_SUCCESS is returned on success, otherwise an error code is returned.
 */

int
vscp_fwhlp_convertEventExToEvent(vscpEvent* pEvent, const vscpEventEx* pEventEx);

/*!
    @brief Make a copy of an event
    @param pev Pointer to event structure to make copy of.
    @return Pointer to the new event if successful, NULL if not.
*/

vscpEvent*
vscp_fwhlp_mkEventCopy(const vscpEvent* pev);

/*!
    @brief Make a copy of an eventex
    @param pex Pointer to event structure to make copy of.
    @return Pointer to the new event if successful, NULL if not.
*/

vscpEventEx*
vscp_fwhlp_mkEventExCopy(const vscpEventEx* pex);

/**
  @brief Delete an event and it's data.
  @param pev Address of event to delete.
  @return VSCP_ERROR_SUCCESS if event was deleted correctly,
*/
int
vscp_fwhlp_deleteEvent(vscpEvent** pev);

/*
  AES crypto support  requires the vscp-aes.c lib to be
  linked in (from the vscp-firmware common folder).

  Define VSCP_FWHLP_CRYPTO_SUPPORT in vscp-projefs.h to
  compile.
*/
#ifdef VSCP_FWHLP_CRYPTO_SUPPORT

/*!
 * Encrypt VSCP frame using the selected encryption algorithm. The iv
 * initialization vector) is appended to the end of the encrypted data.
 *
 * @param output Buffer that will receive the encrypted result. The buffer
 *          should be at least 16 bytes larger than the frame. This means
 *          the size must be original size adjusted to 16 upper byte block and one
 *          added to this (encryption code). So if the data that should be encrypted
 *          is 13 byte in size and the first byte is the encryption code byte
 *          (which should not be encrypted) then 16 is the encryption block and 16 + 1
 *          bytes will be the minimum needed output buffer size.
 * @param input This is the frame that should be encrypted. The first
 *          byte in the frame is the packet type which is not encrypted.
 * @param len This is the length of the frame to be encrypted. This
 *          length includes the frame encryption type in the first byte.
 *          NOTE:   Length must be evenly divisible by 16 bytes (len % 16 == 0)
            You should pad the end of the string with zeros if this is not the case.
 * @param key This is a pointer to the secret encryption key. This key
 *          should be 128 bits for AES128, 192 bits for AES192, 256 bits
 *          for AES256.
 * @param iv Pointer to the initialization vector. Should always point to a
 *           128 bit content. If NULL the iv will be created from random
 *            system data. In both cases the end result will have the iv
 *            appended to the encrypted block.
 * @param nAlgorithm The VSCP defined algorithm (0-15) to encrypt the frame
 * with. If set to 15 (VSCP_ENCRYPTION_FROM_TYPE_BYTE) the algorithm will be
 * set from the four lower bits of the buffer to decrypt.
 * @return Packet length on success, zero on failure.
 *
 */
size_t
vscp_fwhlp_encryptFrame(uint8_t* output,
                        uint8_t* input,
                        size_t len,
                        const uint8_t* key,
                        const uint8_t* iv,
                        uint8_t nAlgorithm);

/*!
 * Decrypt VSCP frame using the selected encryption algorithm. The iv
 * initialization vector) is appended to the end of the encrypted data.
 *
 * @param output Buffer that will receive the decrypted result. The buffer
 *          should have a size of at lest equal to the encrypted block.
 * @param input This is the frame that should be decrypted.
 * @param len This is the length of the frame to be decrypted.
 * @param key This is a pointer to the secret encryption key. This key
 *            should be 128 bits for AES128, 192 bits for AES192,
 *            256 bits for AES256.
 * @param iv Pointer to the initialization vector. Should always point to a
 *           128 bit content. If NULL the iv is expected to be the last
 *           16 bytes of the encrypted data.
 * @param nAlgorithm The VSCP defined algorithm (0-15) to decrypt the frame
 * with. (vscp.h) If set to 15 (VSCP_ENCRYPTION_FROM_TYPE_BYTE) the algorithm
 * will be set from the four lower bits of the buffer to decrypt.
 * @return VSCP_ERROR_SUCCESS, errorcode on failure on failure.
 */
int
vscp_fwhlp_decryptFrame(uint8_t* output,
                        uint8_t* input,
                        size_t len,
                        const uint8_t* key,
                        const uint8_t* iv,
                        uint8_t nAlgorithm);

#endif

/*
  JSON support needs VSCP_FWHLP_JSON_SUPPORT to be defined
  in the projdef file and cJSON support linked in (can be found
  in vscp-firmware/third-party or at https://github.com/nopnop2002/esp-idf-json
*/

#ifdef VSCP_FWHLP_JSON_SUPPORT

/**
 * @fn vscp_fwhlp_parse_json
 * @brief Convert JSON string to VSCP event
 *
 * @param jsonVscpEventObj1
 * @param pev Pointer to event that will get data
 * @return int Returns VSCP_ERROR_SUCCESS on OK, error code else.
 */

int
vscp_fwhlp_parse_json(vscpEvent* pev, const char* jsonVscpEventObj);

/**
 * @fn vscp_fwhlp_parse_json_ex
 * @brief Convert JSON string to VSCP event ex
 *
 * @param jsonVscpEventObj1
 * @param pex Pointer to event ex that will get data
 * @return int Returns VSCP_ERROR_SUCCESS on OK, error code else.
 */

int
vscp_fwhlp_parse_json_ex(vscpEventEx* pex, const char* jsonVscpEventObj);

/**
 * @fn vscp_fwhlp_create_json
 * @brief Convert pointer to VSCP event to VSCP JSON string
 *
 * @param strObj String buffer that will get result
 * @param len Size of string buffer
 * @param pev Pointer to event
 * @return int Returns VSCP_ERROR_SUCCESS on OK, error code else.
 */
int
vscp_fwhlp_create_json(char* strObj, size_t len, const vscpEvent* pev);

/**
 * @fn vscp_fwhlp_create_json_ex
 * @brief Convert pointer to VSCP event to VSCP JSON string
 *
 * @param strObj String buffer that will get result
 * @param len Size of string buffer
 * @param pex Pointer to event ex
 * @return int Returns VSCP_ERROR_SUCCESS on OK, error code else.
 */
int
vscp_fwhlp_create_json_ex(char* strObj, size_t len, const vscpEventEx* pex);

#endif // JSON support

// ----------------------------------------------------------------------------

uint8_t
vscp_fwhlp_getMeasurementDataCoding(const vscpEvent* pEvent);

uint64_t
vscp_fwhlp_getDataCodingBitArray(const uint8_t* pCode, const uint8_t length);

int64_t
vscp_fwhlp_getDataCodingInteger(const uint8_t* pCode, uint8_t length);

double
vscp_fwhlp_getDataCodingNormalizedInteger(const uint8_t* pCode, uint8_t length);

// unsigned char
// vscp_fwhlp_getEventPriority(const vscpEvent *pEvent);

void
vscp_fwhlp_setEventPriority(vscpEvent* pEvent, unsigned char priority);

int
vscp_fwhlp_getMeasurementUnit(const vscpEvent* pEvent);

int
vscp_fwhlp_getMeasurementSensorIndex(const vscpEvent* pEvent);

int
vscp_fwhlp_getMeasurementZone(const vscpEvent* pEvent);

int
vscp_fwhlp_getMeasurementSubZone(const vscpEvent* pEvent);

int
vscp_fwhlp_isMeasurement(const vscpEvent* pEvent);

// ****************************
//            CANAL
// ****************************

unsigned char
vscp_fwhlp__getHeadFromCANALid(uint32_t id);

uint16_t
vscp_fwhlp__getVscpClassFromCANALid(uint32_t id);

uint16_t
vscp_fwhlp__getVscpTypeFromCANALid(uint32_t id);

uint8_t
vscp_fwhlp__getNicknameFromCANALid(uint32_t id);

uint32_t
vscp_fwhlp__getCANALidFromData(unsigned char priority, const uint16_t vscp_class, const uint16_t vscp_type);

uint32_t
vscp_fwhlp__getCANALidFromEvent(const vscpEvent* pEvent);

#endif
