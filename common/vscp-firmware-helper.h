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
 * Copyright (C) 2000-2022 Ake Hedman, 
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

#include "vscp.h"

#ifndef __VSCP_FIRMWARE_HELPER_H__
#define __VSCP_FIRMWARE_HELPER_H__

/* Macros */

/* This macro construct a signed integer from two unsigned chars in a safe way */
#define construct_signed16(msb, lsb) ((int16_t)((((uint16_t)msb) << 8) + \
                                                (uint16_t)lsb))

/* This macro construct a unsigned integer from two unsigned chars in a safe way */
#define construct_unsigned16(msb, lsb) ((uint16_t)((((uint16_t)msb) << 8) + \
                                                   (uint16_t)lsb))

/* This macro construct a signed long from four unsigned chars in a safe way */
#define construct_signed32(b0, b1, b2, b3) ((int32_t)((((uint32_t)b0) << 24) + \
                                                      (((uint32_t)b0) << 16) + \
                                                      (((uint32_t)b0) << 8) +  \
                                                      (uint32_t)b0))

/* This macro construct a unsigned long from four unsigned chars in a safe way */
#define construct_unsigned32(b0, b1, b2, b3) ((uint32_t)((((uint32_t)b0) << 24) + \
                                                         (((uint32_t)b0) << 16) + \
                                                         (((uint32_t)b0) << 8) +  \
                                                         (uint32_t)b0))

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
    Convert ASCII substring to unsigned long number
    Copyright (C) 1998, 1999  Henry Spencer.
    http://www.koders.com/c/fid83F5660A86069B2E2D29B9D3FC9013F76A9BCEB7.aspx
    
    @param src NULL terminated string to convert.
    @param srclen length of string, 0 means strlen( src )
    @param base The base for the conversion, 0 means figure it out.
    @param resultp Pointer that holds unsigned long result after conversion.
    @return  VSCP_ERROR_SUCCESS for success, else VSCP_ERROR_ERROR.    
*/
int vscp_fwhlp_a2ul(const char *src, uint8_t srclen, uint8_t base, uint32_t *resultp);

/**
    Convert decimal byte to hex string
    @param d Decimal byte to convert
    @param pBuf Pointer to string buffer that will hold the result.
    @param len Number of digits to convert.
*/
void vscp_fwhlp_dec2hex(uint8_t d, char * pBuf, uint16_t len);


/**
    Convert hexadecimal integer to a decimal value
    @param pHex Pointer to hexadecimal string.
    @return Converted value in decimal form.
*/
uint32_t vscp_fwhlp_hex2dec(const char *pHex);

/**
    Read a value (hex or decimal)
    @param pString Pointe to value. A hex value should be
    be preceded by "0x" or "0X"
    @return Converted value in decimal form.  
*/
uint32_t vscp_fwhlp_readStringValue(const char *pString);

/**
  Find substring other string and rteturn pointer to it
  @param haystack String to search in.
  @param needle String to search for.
  @return Pointer to substring or NULL if not found.
*/
char *vscp_fwhlp_stristr(const char *haystack, const char *needle);

/**
    Get VSCP priority
    @param pEvent Pointer to VSCP event to set priority for.
    @return Priority (0-7) for event.
*/
unsigned char
vscp_fwhlp_getEventPriority(const vscpEvent *pev);

/**
    Get VSCP EventEx priority
    @param pEvent Pointer to VSCP event ex to set priority for.
    @return Priority (0-7) for event.
*/
unsigned char
vscp_fwhlp_getEventPriorityEx(const vscpEventEx *pex);


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
vscp_fwhlp_doLevel2Filter(const vscpEvent *pev, const vscpEventFilter *pFilter);

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
vscp_fwhlp_doLevel2FilterEx(const vscpEventEx *pex, const vscpEventFilter *pFilter);

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

/*!
    @brief Make a copy of an event
    @param pev Pointer to event structure to make copy of.
    @return Pointer to the new event if successful, NULL if not.
*/

vscpEvent* vscp_fwhlp_mkEventCopy(vscpEvent* pev);

/**
  @brief Delete an event and it's data.
  @param pev Address of event to delete.
  @return VSCP_ERROR_SUCCESS if event was deleted correctly,
*/
int
vscp_fwhlp_deleteEvent(vscpEvent** pev);


/*!
 * Encrypt VSCP frame using the selected encryption algorithm. The iv
 * initialization vector) is appended to the end of the encrypted data.
 *
 * @param output Buffer that will receive the encrypted result. The buffer
 *          should be at least 16 bytes larger than the frame.
 * @param input This is the frame that should be encrypted. The first
 *          byte in the frame is the packet type which is not encrypted.
 * @param len This is the length of the frame to be encrypted. This
 *          length includes the packet tye in the first byte.
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
vscp_encryptFrame(uint8_t *output,
                  uint8_t *input,
                  size_t len,
                  const uint8_t *key,
                  const uint8_t *iv,
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
 * @return True on success, false on failure.
 *
 * NOTE: Note that VSCP packet type (first byte in UDP frame) is not
 * recognised here.
 *
 */
int
vscp_decryptFrame(uint8_t *output,
                  uint8_t *input,
                  size_t len,
                  const uint8_t *key,
                  const uint8_t *iv,
                  uint8_t nAlgorithm);

#endif