/**
 * @brief           VSCP firmware helper functions
 * @file            vscp_firmware_helper.c
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
 * Copyright (c) 2000-2024 Ake Hedman,
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

#include "vscp-compiler.h"
#include "vscp-projdefs.h"

#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vscp-firmware-helper.h"
#include <vscp.h>

#ifdef VSCP_FWHLP_UDP_FRAME_SUPPORT
#include <crc.h>
#endif

#ifdef VSCP_FWHLP_CRYPTO_SUPPORT
#include <vscp-aes.h>
#endif

#ifdef VSCP_FWHLP_JSON_SUPPORT
#include <cJSON.h>
#endif

#define UNUSED(expr) \
  do {               \
    (void)(expr);    \
  } while (0)

#ifndef TRUE
#define TRUE -1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// case-independent ASCII character equality comparison
#define CIEQ(c1, c2) (((c1) & ~040) == ((c2) & ~040))

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_isLittleEndian
//

int
vscp_fwhlp_isLittleEndian(void)
{
  int x = 1;
  return (*(char*)&x);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_isBigEndian
//
//

int
vscp_fwhlp_isBigEndian(void)
{
  return ~vscp_fwhlp_isLittleEndian();
}

////////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_a2ul
//

int
vscp_fwhlp_a2ul(const char* src, uint8_t srclen, uint8_t base, uint32_t* resultp)
{
  const char* stop;
  static char hex[]   = "0123456789abcdef";
  static char uchex[] = "0123456789ABCDEF";
  int d;
  char c;
  char* p;
  unsigned long r;
  unsigned long rlimit;
  int dlimit;

  if (0 == srclen) {
    srclen = strlen(src);
  }

  if (0 == srclen) {
    return VSCP_ERROR_ERROR;
  }

  if (base == 0 || base == 13) {

    if (srclen > 2 && *src == '0' && CIEQ(*(src + 1), 'x')) {
      return vscp_fwhlp_a2ul(src + 2, srclen - 2, 16, resultp);
    }

    if (srclen > 1 && *src == '0' && base != 13) {
      return vscp_fwhlp_a2ul(src + 1, srclen - 1, 8, resultp);
    }

    return vscp_fwhlp_a2ul(src, srclen, 10, resultp);
  }

  if (base != 8 && base != 10 && base != 16) {
    return VSCP_ERROR_ERROR;
  }

  r    = 0;
  stop = src + srclen;
  if (base == 16) {
    while (src < stop) {
      c = *src++;
      p = strchr(hex, c);
      if (p != NULL) {
        d = p - hex;
      }
      else {
        p = strchr(uchex, c);
        if (p == NULL) {
          return VSCP_ERROR_ERROR;
        }
        d = p - uchex;
      }
      r = (r << 4) | d;
    }

    // defer length check to catch invalid digits first
    if (srclen > sizeof(unsigned long) * 2) {
      return VSCP_ERROR_ERROR;
    }
  }
  else {
    rlimit = ULONG_MAX / base;
    dlimit = (int)(ULONG_MAX - rlimit * base);

    while (src < stop) {

      c = *src++;
      d = c - '0';

      if (d < 0 || d >= base) {
        return VSCP_ERROR_ERROR;
      }

      if (r > rlimit || (r == rlimit && d > dlimit)) {
        return VSCP_ERROR_ERROR;
      }

      r = r * base + d;
    }
  }

  *resultp = r;
  return VSCP_ERROR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_dec2hex
//

void
vscp_fwhlp_dec2hex(uint8_t dec, char* pBuf, uint16_t len)
{
  uint32_t i;

  for (i = 0; i < len; i++) {
    uint32_t value = dec & 0x0f;
    if (value < 10) {
      pBuf[len - i - 1] = value + '0';
    }
    else {
      pBuf[len - i - 1] = value - 10 + 'A';
    }

    dec >>= 4;
  }
}

////////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_hex2dec
//

uint32_t
vscp_fwhlp_hex2dec(const char* pHex)
{
  int i;
  uint32_t val = 0;
  uint8_t len  = strlen(pHex);

  for (i = 0; i < len; i++) {
    if (0x20 == pHex[i])
      break;
    if (pHex[i] <= 57) {
      val += (pHex[i] - 48) * (1 << (4 * (len - 1 - i)));
    }
    else {
      val += (pHex[i] - 55) * (1 << (4 * (len - 1 - i)));
    }
  }
  return val;
}

//////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_strsubst
//
// Substitute string occurrences in string
//

char*
vscp_fwhlp_strsubst(char* pNewStr, size_t len, const char* pStr, const char* pTarget, const char* pReplace)
{
  char* p     = (char*)pStr;
  char* pLast = (char*)pStr;

  // Check pointers
  if ((NULL == pNewStr) || (NULL == pStr) || (NULL == pTarget) || (NULL == pReplace)) {
    return NULL;
  }

  memset(pNewStr, 0, len);
  while (*p && (NULL != (p = strstr(p, pTarget)))) {

    // Copy first part to string
    strncat(pNewStr, pLast, p - pLast);

    // Point beyond taget
    p += strlen(pTarget);

    // Target has to fit
    if (strlen(pNewStr) + strlen(pTarget) > (len - 1)) {
      return NULL;
    }
    // Copy in target
    strcat(pNewStr, pReplace);

    // Save last p
    pLast = p;
  }

  if (*pLast && (strlen(pLast) <= (len - 1))) {
    strcat(pNewStr, pLast);
  }

  return pNewStr;
}

////////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_readStringValue
//

uint32_t
vscp_fwhlp_readStringValue(const char* pString)
{
  uint32_t ul;
  char* p;
  char buf[128];

  memset(buf, 0, sizeof(buf));
  strncpy(buf, pString, sizeof(buf));

  // Convert string to lowercase
  uint16_t i = 0;
  while (buf[i]) {
    buf[i] = tolower(buf[i]);
    i++;
  }

  if (NULL != (p = strstr(buf, "0x"))) {
    p += 2; // Point at the data
    vscp_fwhlp_a2ul(buf, 0, 16, &ul);
    return ul;
  }
  if (NULL != (p = strstr(buf, "0"))) {
    p += 2; // Point at the data
    vscp_fwhlp_a2ul(buf, 0, 8, &ul);
    return ul;
  }
  if (NULL != (p = strstr(buf, "0b"))) {
    p += 2; // Point at the data
    vscp_fwhlp_a2ul(buf, 0, 2, &ul);
    return ul;
  }
  else {
    ul = (unsigned)atoi(buf);
    return ul;
  }

  return ul;
}

////////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_stristr
//

char*
vscp_fwhlp_stristr(const char* haystack, const char* needle)
{
  int c = tolower((unsigned char)*needle);
  if (c == '\0')
    return (char*)haystack;
  for (; *haystack; haystack++) {
    if (tolower((unsigned char)*haystack) == c) {
      for (size_t i = 0;;) {
        if (needle[++i] == '\0')
          return (char*)haystack;
        if (tolower((unsigned char)haystack[i]) != tolower((unsigned char)needle[i]))
          break;
      }
    }
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_hexChar
//

unsigned char
vscp_fwhlp_hexchar(char c)
{
  if ('0' <= c && c <= '9') {
    return (unsigned char)(c - '0');
  }
  if ('A' <= c && c <= 'F') {
    return (unsigned char)(c - 'A' + 10);
  }
  if ('a' <= c && c <= 'f') {
    return (unsigned char)(c - 'a' + 10);
  }
  return 0xFF;
}

////////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_hex2bin
//

int
vscp_fwhlp_hex2bin(unsigned char* buf, size_t length, const char* s)
{
  int result;
  if (!s || !buf || length <= 0) {
    return -1;
  }

  for (result = 0; *s; ++result) {
    unsigned char msn = vscp_fwhlp_hexchar(*s++);
    if (msn == 0xFF) {
      return -1;
    }
    unsigned char lsn = vscp_fwhlp_hexchar(*s++);
    if (lsn == 0xFF) {
      return -1;
    }
    unsigned char bin = (msn << 4) + lsn;

    if (length-- <= 0) {
      return -1;
    }
    *buf++ = bin;
  }

  return result;
}

////////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_bin2hex
//

void
vscp_fwhlp_bin2hex(char* output, size_t outLength, const unsigned char* buf, size_t length)
{
  char binHex[] = "0123456789ABCDEF";

  if (!output || outLength < 4) {
    return;
  }
  *output = '\0';

  if (!buf || length <= 0 || outLength <= 2 * length) {
    memcpy(output, "ERR", 4);
    return;
  }

  for (; length > 0; --length, outLength -= 2) {
    unsigned char byte = *buf++;

    *output++ = binHex[(byte >> 4) & 0x0F];
    *output++ = binHex[byte & 0x0F];
  }

  if (outLength-- <= 0) {
    return;
  }

  *output++ = '\0';
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_parseMac
//

int
vscp_fwhlp_parseMac(uint8_t* pmac, const char* strmac, char** endptr)
{
  int i;
  char* p = (char*)strmac;

  // Check pointers
  if (NULL == strmac) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (NULL == pmac) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  memset(pmac, 0, 6);

  // remove initial white space
  while (' ' == *p) {
    p++;
  }

  // Empty MAC or MAC  set to '-' means all nulls
  if (!*p || ('-' == *p)) {
    if (NULL != endptr) {
      p++;
      *endptr = p;
    }
    return VSCP_ERROR_SUCCESS;
  }

  for (i = 0; i < 6; i++) {
    pmac[i] = (uint8_t)strtol(p, &p, 16);
    if (!*p) {
      break; // at end?
    }
    if (i != 15) {
      if (':' != *p) {
        return VSCP_ERROR_ERROR;
      }
      p++; // Move beyond ':'
      if (p > (strmac + strlen(strmac))) {
        if (NULL != endptr) {
          *endptr = p;
        }
        return VSCP_ERROR_ERROR;
      }
    }
  }

  if (NULL != endptr) {
    *endptr = p;
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getEventPriority

unsigned char
vscp_fwhlp_getEventPriority(const vscpEvent* pev)
{
  // Must be a valid message pointer
  if (NULL == pev) {
    return 0;
  }

  return ((pev->head >> 5) & 0x07);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getEventPriorityEx

unsigned char
vscp_fwhlp_getEventPriorityEx(const vscpEventEx* pex)
{
  // Must be a valid message pointer
  if (NULL == pex) {
    return 0;
  }

  return ((pex->head >> 5) & 0x07);
}

// ----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_newEvent
//

vscpEvent*
vscp_fwhlp_newEvent(void)
{
  static vscpEvent* pev = NULL;
  pev                   = (vscpEvent*)calloc(1, sizeof(vscpEvent));
  return pev;
}

////////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_convertEventToEventEx

int
vscp_fwhlp_convertEventToEventEx(vscpEventEx* pEventEx, const vscpEvent* pEvent)
{
  // Check pointers
  if (NULL == pEvent) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (NULL == pEventEx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (pEvent->sizeData > VSCP_LEVEL2_MAXDATA) {
    return VSCP_ERROR_PARAMETER;
  }

  // Convert
  pEventEx->crc        = pEvent->crc;
  pEventEx->obid       = pEvent->obid;
  pEventEx->year       = pEvent->year;
  pEventEx->month      = pEvent->month;
  pEventEx->day        = pEvent->day;
  pEventEx->hour       = pEvent->hour;
  pEventEx->minute     = pEvent->minute;
  pEventEx->second     = pEvent->second;
  pEventEx->timestamp  = pEvent->timestamp;
  pEventEx->head       = pEvent->head;
  pEventEx->obid       = pEvent->obid;
  pEventEx->vscp_class = pEvent->vscp_class;
  pEventEx->vscp_type  = pEvent->vscp_type;
  pEventEx->sizeData   = pEvent->sizeData;

  memcpy(pEventEx->GUID, pEvent->GUID, 16);
  memcpy(pEventEx->data, pEvent->pdata, pEvent->sizeData);

  return VSCP_ERROR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_convertEventExToEvent
//

int
vscp_fwhlp_convertEventExToEvent(vscpEvent* pEvent, const vscpEventEx* pEventEx)
{
  // Check pointers
  if (NULL == pEvent) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (NULL == pEventEx) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (pEventEx->sizeData) {
    // Allocate memory for data
    if (NULL == (pEvent->pdata = malloc(pEventEx->sizeData))) {
      return VSCP_ERROR_MEMORY;
    }
    memcpy(pEvent->pdata, pEventEx->data, pEventEx->sizeData);
  }
  else {
    // No data
    pEvent->pdata = NULL;
  }

  // Convert
  pEvent->crc        = pEventEx->crc;
  pEvent->obid       = pEventEx->obid;
  pEvent->year       = pEventEx->year;
  pEvent->month      = pEventEx->month;
  pEvent->day        = pEventEx->day;
  pEvent->hour       = pEventEx->hour;
  pEvent->minute     = pEventEx->minute;
  pEvent->second     = pEventEx->second;
  pEvent->timestamp  = pEventEx->timestamp;
  pEvent->head       = pEventEx->head;
  pEvent->obid       = pEventEx->obid;
  pEvent->vscp_class = pEventEx->vscp_class;
  pEvent->vscp_type  = pEventEx->vscp_type;
  pEvent->sizeData   = pEventEx->sizeData;
  memcpy(pEvent->GUID, pEventEx->GUID, 16);

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_mkEventCopy
//

vscpEvent*
vscp_fwhlp_mkEventCopy(const vscpEvent* pev)
{
  // Must have event to work on
  if (NULL == pev) {
    return NULL;
  }

  vscpEvent* pnewEvent = vscp_fwhlp_newEvent();
  if (NULL == pnewEvent) {
    return NULL;
  }

  // Copy the event
  memcpy(pnewEvent, pev, sizeof(vscpEvent));

  if (pev->sizeData) {
    pnewEvent->pdata = (uint8_t*)malloc(pev->sizeData);
    if (NULL == pnewEvent->pdata) {
      free(pnewEvent);
      return NULL;
    }
    memcpy(pnewEvent->pdata, pev->pdata, pev->sizeData);
  }
  else {
    pnewEvent->pdata = NULL;
  }

  return pnewEvent;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_mkEventExCopy
//

vscpEventEx*
vscp_fwhlp_mkEventExCopy(const vscpEventEx* pex)
{
  // Must have event to work on
  if (NULL == pex) {
    return NULL;
  }

  vscpEventEx* pnewEventEx = (vscpEventEx*)malloc(sizeof(vscpEventEx));
  ;
  if (NULL == pnewEventEx) {
    return NULL;
  }

  // Copy the event
  memcpy(pnewEventEx, pex, sizeof(vscpEventEx));

  return pnewEventEx;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_deleteEvent
//

int
vscp_fwhlp_deleteEvent(vscpEvent** pev)
{
  if (NULL == *pev)
    return VSCP_ERROR_INVALID_POINTER;

  if (NULL != (*pev)->pdata) {
    free((*pev)->pdata);
  }

  free(*pev);
  *pev = NULL;

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_parseGuid
//

int
vscp_fwhlp_parseGuid(uint8_t* guid, const char* strguid, char** endptr)
{
  int i;
  char* p = (char*)strguid;

  // Check pointers
  if (NULL == strguid) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (NULL == guid) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  memset(guid, 0, 16);

  // remove initial white space
  while (' ' == *p) {
    p++;
  }

  // Empty GUID or GUID  set to '-' means all nulls
  if (!*p || ('-' == *p)) {
    if (NULL != endptr) {
      p++;
      *endptr = p;
    }
    return VSCP_ERROR_SUCCESS;
  }

  for (i = 0; i < 16; i++) {
    guid[i] = (uint8_t)strtol(p, &p, 16);
    if (!*p) {
      break; // at end?
    }
    if (i != 15) {
      if (':' != *p) {
        return VSCP_ERROR_ERROR;
      }
      p++; // Move beyond ':'
      if (p > (strguid + strlen(strguid))) {
        if (NULL != endptr) {
          *endptr = p;
        }
        return VSCP_ERROR_ERROR;
      }
    }
  }

  if (NULL != endptr) {
    *endptr = p;
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_writeGuidToString
//

int
vscp_fwhlp_writeGuidToString(char* strguid, const uint8_t* guid)
{
  // Check pointers
  if (NULL == strguid) {
    return VSCP_ERROR_INVALID_POINTER;
  }
  if (NULL == guid) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  sprintf(strguid,
          "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:"
          "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
          guid[0],
          guid[1],
          guid[2],
          guid[3],
          guid[4],
          guid[5],
          guid[6],
          guid[7],
          guid[8],
          guid[9],
          guid[10],
          guid[11],
          guid[12],
          guid[13],
          guid[14],
          guid[15]);

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_parseFilter
//
// filter-priority, filter-class, filter-type, filter-GUID
//

int
vscp_fwhlp_parseFilter(vscpEventFilter* pfilter, const char* strfilter)
{
  char* p = (char*)strfilter;

  // Check pointers
  if (NULL == strfilter) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (NULL == pfilter) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  // Priority
  pfilter->filter_priority = (uint8_t)strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_ERROR;
  }

  p++;
  if (p > (strfilter + strlen(strfilter))) {
    return VSCP_ERROR_ERROR;
  }

  // Class filter
  pfilter->filter_class = (uint16_t)strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_ERROR;
  }

  p++;
  if (p > (strfilter + strlen(strfilter))) {
    return VSCP_ERROR_ERROR;
  }

  // Type filter
  pfilter->filter_type = (uint16_t)strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_ERROR;
  }

  p++;
  if (p > (strfilter + strlen(strfilter))) {
    return VSCP_ERROR_ERROR;
  }

  // GUID filter
  return vscp_fwhlp_parseGuid(pfilter->filter_GUID, p, NULL);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_parseMask
//
// mask-priority, mask-class, mask-type, mask-GUID
//

int
vscp_fwhlp_parseMask(vscpEventFilter* pfilter, const char* strmask)
{
  char* p = (char*)strmask;

  // Check pointers
  if (NULL == strmask) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (NULL == pfilter) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  // Priority
  pfilter->mask_priority = (uint8_t)strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_ERROR;
  }
  p++;
  if (p > (strmask + strlen(strmask))) {
    return VSCP_ERROR_ERROR;
  }

  // Class mask
  pfilter->mask_class = (uint16_t)strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_ERROR;
  }
  p++;
  if (p > (strmask + strlen(strmask))) {
    return VSCP_ERROR_ERROR;
  }

  // Type mask
  pfilter->mask_type = (uint16_t)strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_ERROR;
  }
  p++;
  if (p > (strmask + strlen(strmask))) {
    return VSCP_ERROR_ERROR;
  }

  // GUID mask
  return vscp_fwhlp_parseGuid(pfilter->mask_GUID, p, NULL);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_parseEvent
//
// head,class,type,obid,datetime,timestamp,GUID,data1,data2,data3....
//

int
vscp_fwhlp_parseEvent(vscpEvent* pev, const char* buf)
{
  uint8_t wrkbuf[512];
  char* p = (char*)buf;

  // Check pointers
  if (NULL == buf) {
    return VSCP_ERROR_INVALID_POINTER;
  }
  if (NULL == pev) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  // Set all defaults
  memset(pev, 0, sizeof(vscpEvent));

  // head
  pev->head = (uint16_t)strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond comma
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_PARAMETER;
  }

  // VSCP class
  pev->vscp_class = (uint16_t)strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond comma
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_PARAMETER;
  }

  // VSCP type
  pev->vscp_type = (uint16_t)strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++;
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_PARAMETER;
  }

  // obid (may be left blank)
  if (',' == *p) {
    pev->obid = 0;
    p++; // point beyond comma
  }
  else {
    pev->obid = (uint32_t)strtol(p, &p, 0);
    if (',' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond comma
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }
  }

  // datetime YYYY-MM-DDTHH:MM:SS

  // Date may not be given (left blank) and in
  // this case it should be set to all nulls for
  // the interface to set it instead.

  if (',' == *p) {
    p++; // point beyond comma
    pev->second = 0;
    pev->minute = 0;
    pev->hour   = 0;
    pev->day    = 0;
    pev->month  = 0;
    pev->year   = 0;
  }
  else {
    // year
    pev->year = (uint16_t)strtol(p, &p, 0);
    if ('-' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond dash
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // month
    pev->month = (uint16_t)strtol(p, &p, 0);
    if ('-' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond dash
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // day
    pev->day = (uint16_t)strtol(p, &p, 0);
    if ('T' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond dash
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // hour
    pev->hour = (uint16_t)strtol(p, &p, 0);
    if (':' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond colon
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // minute
    pev->minute = (uint16_t)strtol(p, &p, 0);
    if (':' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond colon
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // second
    pev->second = (uint16_t)strtol(p, &p, 0);
    if (',' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond comma
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }
  }

  // timestamp (may be left blank)
  if (',' == *p) {
    pev->timestamp = 0;
    p++; // point beyond comma
  }
  else {
    pev->timestamp = (uint32_t)strtol(p, &p, 0);
    if (',' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond comma
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }
  }

  // Get GUID
  if (VSCP_ERROR_SUCCESS != vscp_fwhlp_parseGuid(pev->GUID, p, &p)) {
    return VSCP_ERROR_PARAMETER;
  }

  if (',' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond comma
  pev->sizeData = 0;

  // Get data (if any )
  while ((p <= (buf + strlen(buf))) && (pev->sizeData < 512)) {
    wrkbuf[pev->sizeData] = (uint8_t)strtol(p, &p, 0);
    pev->sizeData++;
    p++; // point beyond comma
  }

  // Copy in data (if any)
  if (pev->sizeData) {
    pev->pdata = malloc(pev->sizeData);
    if (NULL == pev->pdata) {
      return VSCP_ERROR_MEMORY;
    }
    memcpy(pev->pdata, wrkbuf, pev->sizeData);
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_parseEventEx
//

int
vscp_fwhlp_parseEventEx(vscpEventEx* pex, const char* buf)
{
  uint8_t wrkbuf[512];
  char* p = (char*)buf;

  // Check pointers
  if (NULL == buf) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (NULL == pex) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  memset(pex, 0, sizeof(vscpEventEx));

  // head
  pex->head = (uint16_t)strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond comma
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_PARAMETER;
  }

  // VSCP class
  pex->vscp_class = (uint16_t)strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond comma
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_PARAMETER;
  }

  // VSCP type
  pex->vscp_type = (uint16_t)strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++;
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_PARAMETER;
  }

  // obid (may be left blank)
  if (',' == *p) {
    pex->obid = 0;
    p++; // point beyond comma
  }
  else {
    pex->obid = (uint32_t)strtol(p, &p, 0);
    if (',' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond comma
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }
  }

  // datetime YYYY-MM-DDTHH:MM:SS

  // Date may not be given (left blank) and in
  // this case it should be set to all nulls for
  // the interface to set it instead.

  if (',' == *p) {
    p++; // point beyond comma
    pex->second = 0;
    pex->minute = 0;
    pex->hour   = 0;
    pex->day    = 0;
    pex->month  = 0;
    pex->year   = 0;
  }
  else {
    // year
    pex->year = (uint16_t)strtol(p, &p, 0);
    if ('-' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond dash
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // month
    pex->month = (uint16_t)strtol(p, &p, 0);
    if ('-' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond dash
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // day
    pex->day = (uint16_t)strtol(p, &p, 0);
    if ('T' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond dash
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // hour
    pex->hour = (uint16_t)strtol(p, &p, 0);
    if (':' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond colon
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // minute
    pex->minute = (uint16_t)strtol(p, &p, 0);
    if (':' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond colon
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // second
    pex->second = (uint16_t)strtol(p, &p, 0);
    if (',' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond comma
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }
  }

  // timestamp (may be left blank)
  if (',' == *p) {
    pex->timestamp = 0;
    p++; // point beyond comma
  }
  else {
    pex->timestamp = (uint32_t)strtol(p, &p, 0);
    if (',' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond comma
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }
  }

  // Get GUID
  if (VSCP_ERROR_SUCCESS != vscp_fwhlp_parseGuid(pex->GUID, p, &p)) {
    return VSCP_ERROR_PARAMETER;
  }

  if (',' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond comma
  pex->sizeData = 0;

  // Get data (if any )
  while ((p <= (buf + strlen(buf))) && (pex->sizeData < 512)) {
    wrkbuf[pex->sizeData] = (uint8_t)strtol(p, &p, 0);
    pex->sizeData++;
    p++; // point beyond comma
  }

  // Copy in data (if any)
  if (pex->sizeData) {
    memcpy(pex->data, wrkbuf, pex->sizeData);
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_eventToString
//
// head       6  - 0xffff or 255
// class      6  - 0xFFFF or 65535
// type       6  - 0xFFFF or 65535
// obid       10 - 0xFFFFFFFF
// time       20 - YYYY-MM-DDTHH:MM:SSZ
// timestamp  10 - 0xFFFFFFFF
// GUID       47 - 00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF
// 7 x comma
// ---------------------------------------------------------------
// Total:     103 + 7 (commas) + data = 110 + data
//

int
vscp_fwhlp_eventToString(char* buf, size_t size, const vscpEvent* pev)
{
  char wrkbuf[48]; // Can hold full GUID

  // Check pointers
  if (NULL == pev) {
    return VSCP_ERROR_INVALID_POINTER;
  }
  if (NULL == buf) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  memset(buf, 0, size);

  // Len must be able to hold content
  // data size for each byte is 5 as worst '0xff,'
  if (size < ((size_t)((size_t)((size_t)((pev->sizeData * 5) + 110 + 1))))) {
    return VSCP_ERROR_BUFFER_TO_SMALL;
  }

  if (pev->year || pev->month || pev->day || pev->hour || pev->minute || pev->second) {
    sprintf(buf, "%u,%u,%u,%lu,%4d-%02d-%02dT%02d:%02d:%02dZ,%lu,", (unsigned)pev->head, (unsigned)pev->vscp_class, (unsigned)pev->vscp_type, (unsigned long)pev->obid, (int)pev->year, (int)pev->month, (int)pev->day, (int)pev->hour, (int)pev->minute, (int)pev->second, (unsigned long)pev->timestamp);
  }
  else {
    sprintf(buf, "%d,%u,%u,%lu,,%lu,", (unsigned)pev->head, (unsigned)pev->vscp_class, (unsigned)pev->vscp_type, (unsigned long)pev->obid, (unsigned long)pev->timestamp);
  }

  // GUID
  memset(wrkbuf, 0, sizeof(wrkbuf));
  vscp_fwhlp_writeGuidToString(wrkbuf, pev->GUID);
  strcat(buf, wrkbuf);

  // Data
  if ((NULL != pev->pdata) && pev->sizeData) {

    if (size < (strlen(buf) + 1 + 1)) {
      return VSCP_ERROR_BUFFER_TO_SMALL;
    }

    strcat(buf, ",");

    for (int i = 0; i < pev->sizeData; i++) {

      if (i < (pev->sizeData - 1)) {
        sprintf(wrkbuf, "%d,", (int)pev->pdata[i]);
      }
      else {
        sprintf(wrkbuf, "%d", (int)pev->pdata[i]);
      }

      // Check that data fits
      if (size < (strlen(buf) + 1 + strlen(wrkbuf))) {
        return VSCP_ERROR_BUFFER_TO_SMALL;
      }

      // Copy in the data
      strcat(buf, wrkbuf);
    }
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_eventToStringEx
//
// head       6  - 0xffff or 255
// class      6  - 0xFFFF or 65535
// type       6  - 0xFFFF or 65535
// obid       10 - 0xFFFFFFFF
// time       20 - YYYY-MM-DDTHH:MM:SSZ
// timestamp  10 - 0xFFFFFFFF
// GUID       47 - 00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF
// 7 x comma
// ---------------------------------------------------------------
// Total:     103 + 7 (commas) + data = 110 + data
// data: ",255" => 4 * 512 = 2048

int
vscp_fwhlp_eventToStringEx(char* buf, size_t size, const vscpEventEx* pex)
{
  char wrkbuf[48]; // Can hold full GUID

  // Check pointers
  if (NULL == pex) {
    return VSCP_ERROR_INVALID_POINTER;
  }
  if (NULL == buf) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  memset(buf, 0, size);

  // Len must be able to hold content
  // data size for each byte is 5 as worst '0xff,'
  if (size < ((size_t)((pex->sizeData * 5) + 110 + 1))) {
    return VSCP_ERROR_BUFFER_TO_SMALL;
  }

  if (pex->year || pex->month || pex->day || pex->hour || pex->minute || pex->second) {
    sprintf(buf,
            "%u,%u,%u,%lu,%4d-%02d-%02dT%02d:%02d:%02dZ,%lu,",
            (unsigned)pex->head,
            (unsigned)pex->vscp_class,
            (unsigned)pex->vscp_type,
            (unsigned long)pex->obid,
            (int)pex->year,
            (int)pex->month,
            (int)pex->day,
            (int)pex->hour,
            (int)pex->minute,
            (int)pex->second,
            (unsigned long)pex->timestamp);
  }
  else {
    sprintf(buf,
            "%d,%u,%u,%lu,,%lu,",
            (unsigned)pex->head,
            (unsigned)pex->vscp_class,
            (unsigned)pex->vscp_type,
            (unsigned long)pex->obid,
            (unsigned long)pex->timestamp);
  }

  // GUID
  memset(wrkbuf, 0, sizeof(wrkbuf));
  vscp_fwhlp_writeGuidToString(wrkbuf, pex->GUID);
  strcat(buf, wrkbuf);

  // Data
  if (pex->sizeData) {

    if (size < (strlen(buf) + 1 + 1)) {
      return VSCP_ERROR_BUFFER_TO_SMALL;
    }

    strcat(buf, ",");

    for (int i = 0; i < pex->sizeData; i++) {

      if (i < (pex->sizeData - 1)) {
        sprintf(wrkbuf, "%d,", (int)pex->data[i]);
      }
      else {
        sprintf(wrkbuf, "%d", (int)pex->data[i]);
      }

      // Check that data fits
      if (size < (strlen(buf) + 1 + strlen(wrkbuf))) {
        return VSCP_ERROR_BUFFER_TO_SMALL;
      }

      // Copy in the data
      strcat(buf, wrkbuf);
    }
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_doLevel2Filter
//
//  filter ^ bit    mask    out
//  ============================
//        0          0       1    filter == bit, mask=don't care result = true
//        0          1       1    filter == bit, mask=valid, result = true
//        1          0       1    filter != bit, makse=don't care, result = true
//        1          1       0    filter != bit, mask=valid, result = false
//
// Mask tells *which* bits that are of interest means
// it always returns true if bit set to zero (0=don't care).
//
// Filter tells the value for valid bits. If filter bit is == 1 the bits
// must be equal to get a true filter return.
//
// So a nill mask will let everything through
//

int
vscp_fwhlp_doLevel2Filter(const vscpEvent* pev, const vscpEventFilter* pFilter)
{
  // A NULL filter is wildcard
  if (NULL == pFilter) {
    return 1;
  }

  // Must be a valid message
  if (NULL == pev) {
    return 0;
  }

  // Test vscp_class
  if (0xffff != (uint16_t)(~(pFilter->filter_class ^ pev->vscp_class) | ~pFilter->mask_class)) {
    return 0;
  }

  // Test vscp_type
  if (0xffff != (uint16_t)(~(pFilter->filter_type ^ pev->vscp_type) | ~pFilter->mask_type)) {
    return 0;
  }

  // GUID
  for (int i = 0; i < 16; i++) {
    if (0xff != (uint8_t)(~(pFilter->filter_GUID[i] ^ pev->GUID[i]) | ~pFilter->mask_GUID[i]))
      return 0;
  }

  // Test priority
  if (0xff != (uint8_t)(~(pFilter->filter_priority ^ vscp_fwhlp_getEventPriority(pev)) | ~pFilter->mask_priority)) {
    return 0;
  }

  return 1;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_doLevel2Filter
//
//  filter ^ bit    mask    out
//  ============================
//        0          0       1    filter == bit, mask=don't care result = true
//        0          1       1    filter == bit, mask=valid, result = true
//        1          0       1    filter != bit, makse=don't care, result = true
//        1          1       0    filter != bit, mask=valid, result = false
//
// Mask tells *which* bits that are of interest means
// it always returns true if bit set to zero (0=don't care).
//
// Filter tells the value for valid bits. If filter bit is == 1 the bits
// must be equal to get a true filter return.
//
// So a nill mask will let everything through
//

int
vscp_fwhlp_doLevel2FilterEx(const vscpEventEx* pex, const vscpEventFilter* pFilter)
{
  // A NULL filter is wildcard
  if (NULL == pFilter) {
    return 1;
  }

  // Must be a valid message
  if (NULL == pex) {
    return 0;
  }

  // Test vscp_class
  if (0xffff != (uint16_t)(~(pFilter->filter_class ^ pex->vscp_class) | ~pFilter->mask_class)) {
    return 0;
  }

  // Test vscp_type
  if (0xffff != (uint16_t)(~(pFilter->filter_type ^ pex->vscp_type) | ~pFilter->mask_type)) {
    return 0;
  }

  // GUID
  for (int i = 0; i < 16; i++) {
    if (0xff != (uint8_t)(~(pFilter->filter_GUID[i] ^ pex->GUID[i]) | ~pFilter->mask_GUID[i]))
      return 0;
  }

  // Test priority
  if (0xff != (uint8_t)(~(pFilter->filter_priority ^ vscp_fwhlp_getEventPriorityEx(pex)) | ~pFilter->mask_priority)) {
    return 0;
  }

  return 1;
}

/*!
  UDP frame handling support
*/
#ifdef VSCP_FWHLP_UDP_FRAME_SUPPORT

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getFrameSizeFromEvent
//

size_t
vscp_fwhlp_getFrameSizeFromEvent(vscpEvent* pEvent)
{
  // Check pointer
  if (NULL == pEvent) {
    return VSCP_ERROR_PARAMETER;
  }

  size_t size = 1 +                                                          // Packet type
                VSCP_MULTICAST_PACKET0_HEADER_LENGTH + pEvent->sizeData + 2; // CRC
  return size;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getFrameSizeFromEventEx
//

size_t
vscp_fwhlp_getFrameSizeFromEventEx(vscpEventEx* pEventEx)
{
  // Check pointer
  if (NULL == pEventEx) {
    return VSCP_ERROR_PARAMETER;
  }

  size_t size = 1 +                                                            // Packet type
                VSCP_MULTICAST_PACKET0_HEADER_LENGTH + pEventEx->sizeData + 2; // CRC
  return size;
}

////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_writeEventToFrame
//

int
vscp_fwhlp_writeEventToFrame(uint8_t* frame, size_t len, uint8_t pkttype, const vscpEvent* pEvent)
{
  // Check pointers
  if (NULL == frame) {
    return VSCP_ERROR_PARAMETER;
  }

  if (NULL == pEvent) {
    return VSCP_ERROR_PARAMETER;
  }

  // Can't have data size with invalid data pointer
  if (pEvent->sizeData && (NULL == pEvent->pdata)) {
    return VSCP_ERROR_PARAMETER;
  }

  size_t calcSize = 1 +                                                          // Packet type
                    VSCP_MULTICAST_PACKET0_HEADER_LENGTH + pEvent->sizeData + 2; // CRC

  if (len < calcSize) {
    return VSCP_ERROR_BUFFER_TO_SMALL;
  }

  // Frame type
  frame[VSCP_MULTICAST_PACKET0_POS_PKTTYPE] = pkttype;

  // Header
  frame[VSCP_MULTICAST_PACKET0_POS_HEAD_MSB] = (pEvent->head >> 8) & 0xff;
  frame[VSCP_MULTICAST_PACKET0_POS_HEAD_LSB] = pEvent->head & 0xff;

  // Timestamp
  frame[VSCP_MULTICAST_PACKET0_POS_TIMESTAMP]     = (pEvent->timestamp >> 24) & 0xff;
  frame[VSCP_MULTICAST_PACKET0_POS_TIMESTAMP + 1] = (pEvent->timestamp >> 16) & 0xff;
  frame[VSCP_MULTICAST_PACKET0_POS_TIMESTAMP + 2] = (pEvent->timestamp >> 8) & 0xff;
  frame[VSCP_MULTICAST_PACKET0_POS_TIMESTAMP + 3] = pEvent->timestamp & 0xff;

  // Date / time block GMT
  frame[VSCP_MULTICAST_PACKET0_POS_YEAR_MSB] = (pEvent->year >> 8) & 0xff;
  frame[VSCP_MULTICAST_PACKET0_POS_YEAR_LSB] = pEvent->year & 0xff;
  frame[VSCP_MULTICAST_PACKET0_POS_MONTH]    = pEvent->month;
  frame[VSCP_MULTICAST_PACKET0_POS_DAY]      = pEvent->day;
  frame[VSCP_MULTICAST_PACKET0_POS_HOUR]     = pEvent->hour;
  frame[VSCP_MULTICAST_PACKET0_POS_MINUTE]   = pEvent->minute;
  frame[VSCP_MULTICAST_PACKET0_POS_SECOND]   = pEvent->second;

  // Class
  frame[VSCP_MULTICAST_PACKET0_POS_VSCP_CLASS_MSB] = (pEvent->vscp_class >> 8) & 0xff;
  frame[VSCP_MULTICAST_PACKET0_POS_VSCP_CLASS_LSB] = pEvent->vscp_class & 0xff;

  // Type
  frame[VSCP_MULTICAST_PACKET0_POS_VSCP_TYPE_MSB] = (pEvent->vscp_type >> 8) & 0xff;
  frame[VSCP_MULTICAST_PACKET0_POS_VSCP_TYPE_LSB] = pEvent->vscp_type & 0xff;

  // GUID
  memcpy(frame + VSCP_MULTICAST_PACKET0_POS_VSCP_GUID, pEvent->GUID, 16);

  // Size
  frame[VSCP_MULTICAST_PACKET0_POS_VSCP_SIZE_MSB] = (pEvent->sizeData >> 8) & 0xff;
  frame[VSCP_MULTICAST_PACKET0_POS_VSCP_SIZE_LSB] = pEvent->sizeData & 0xff;

  // Data
  if (pEvent->sizeData) {
    memcpy(frame + VSCP_MULTICAST_PACKET0_POS_VSCP_DATA, pEvent->pdata, pEvent->sizeData);
  }

  // Calculate CRC
  crc framecrc = crcFast((unsigned char const*)frame + 1, VSCP_MULTICAST_PACKET0_HEADER_LENGTH + pEvent->sizeData);

  // CRC
  frame[1 + VSCP_MULTICAST_PACKET0_HEADER_LENGTH + pEvent->sizeData]     = (framecrc >> 8) & 0xff;
  frame[1 + VSCP_MULTICAST_PACKET0_HEADER_LENGTH + pEvent->sizeData + 1] = framecrc & 0xff;

#if 0
    printf("CRC1 %02X %02X\n",
                frame[ 1 + VSCP_MULTICAST_PACKET0_HEADER_LENGTH + pEvent->sizeData ],
                frame[ 1 + VSCP_MULTICAST_PACKET0_HEADER_LENGTH + pEvent->sizeData + 1 ] );
    printf("CRC2 %02X %02X\n",
                ( framecrc >> 8 ) & 0xff,
                framecrc & 0xff );
    crc nnnn = crcFast( frame+1,
                            VSCP_MULTICAST_PACKET0_HEADER_LENGTH +
                            pEvent->sizeData );
    printf("CRC3 %02X %02X\n",
                ( nnnn >> 8 ) & 0xff,
                nnnn & 0xff );
    printf("--------------------------------\n");
#endif

  return VSCP_ERROR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_writeEventExToFrame
//

int
vscp_fwhlp_writeEventExToFrame(uint8_t* frame, size_t len, uint8_t pkttype, const vscpEventEx* pEventEx)
{
  int rv;
  vscpEvent* pEvent;

  pEvent = vscp_fwhlp_newEvent();
  if (NULL == pEvent) {
    return VSCP_ERROR_PARAMETER;
  }

  pEvent->pdata = NULL;

  // Check pointer (rest is checked in vscp_convertEventExToEvent)
  if (NULL == pEventEx) {
    return VSCP_ERROR_PARAMETER;
  }

  // Convert eventEx to event
  if (VSCP_ERROR_SUCCESS != (rv = vscp_fwhlp_convertEventExToEvent(pEvent, pEventEx))) {
    return rv;
  }

  if (VSCP_ERROR_SUCCESS != (rv = vscp_fwhlp_writeEventToFrame(frame, len, pkttype, pEvent))) {
    return rv;
  }

  vscp_fwhlp_deleteEvent(&pEvent);

  return VSCP_ERROR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getEventFromFrame
//

int
vscp_fwhlp_getEventFromFrame(vscpEvent* pEvent, const uint8_t* buf, size_t len)
{
  // Check pointers
  if (NULL == pEvent) {
    return VSCP_ERROR_PARAMETER;
  }

  if (NULL == buf) {
    return VSCP_ERROR_PARAMETER;
  }

  //  0           Packet type & encryption settings
  //  1           HEAD MSB
  //  2           HEAD LSB
  //  3           Timestamp microseconds MSB
  //  4           Timestamp microseconds
  //  5           Timestamp microseconds
  //  6           Timestamp microseconds LSB
  //  7           Year MSB
  //  8           Year LSB
  //  9           Month
  //  10          Day
  //  11          Hour
  //  12          Minute
  //  13          Second
  //  14          CLASS MSB
  //  15          CLASS LSB
  //  16          TYPE MSB
  //  17          TYPE LSB
  //  18 - 33     ORIGINATING GUID
  //  34          DATA SIZE MSB
  //  35          DATA SIZE LSB
  //  36 - n 	    data limited to max 512 - 25 = 487 bytes
  //  len - 2     CRC MSB( Calculated on HEAD + CLASS + TYPE + ADDRESS + SIZE
  //  + DATA ) len - 1     CRC LSB
  // if encrypted with AES128/192/256 16.bytes IV here.

  size_t calcFrameSize = 1 +                                    // packet type & encryption
                         VSCP_MULTICAST_PACKET0_HEADER_LENGTH + // header
                         2 +                                    // CRC
                         ((uint16_t)buf[VSCP_MULTICAST_PACKET0_POS_VSCP_SIZE_MSB] << 8) +
                         buf[VSCP_MULTICAST_PACKET0_POS_VSCP_SIZE_LSB];

  // The buffer must hold a frame
  if (len < calcFrameSize)
    return VSCP_ERROR_BUFFER_TO_SMALL;

  crc crcFrame = ((uint16_t)buf[calcFrameSize - 2] << 8) + buf[calcFrameSize - 1];

  // CRC check (only if not disabled)
  crc crcnew;
  if (!((buf[VSCP_MULTICAST_PACKET0_POS_HEAD_LSB] & VSCP_HEADER_NO_CRC) && (VSCP_NOCRC_CALC_DUMMY_CRC == crcFrame))) {

#if 0
    int i;
    printf("DUMP = ");
    for ( i=0; i<calcFrameSize; i++ ) {
        printf("%02X ", buf[i] );
    }
    printf("\n");
#endif

    // Calculate & check CRC
    crcnew = crcFast((unsigned char const*)buf + 1, (int)calcFrameSize - 1);
    // CRC is zero if calculated over itself
    if (crcnew) {
      return VSCP_ERROR_INVALID_CHECKSUM;
    }
  }

  pEvent->sizeData =
    ((uint16_t)buf[VSCP_MULTICAST_PACKET0_POS_VSCP_SIZE_MSB] << 8) + buf[VSCP_MULTICAST_PACKET0_POS_VSCP_SIZE_LSB];

  // Allocate data
  if (NULL == (pEvent->pdata = (uint8_t*)malloc(pEvent->sizeData))) {
    return VSCP_ERROR_MEMORY;
  }

  // copy in data
  memcpy(pEvent->pdata, buf + VSCP_MULTICAST_PACKET0_POS_VSCP_DATA, pEvent->sizeData);

  // Head
  pEvent->head = ((uint16_t)buf[VSCP_MULTICAST_PACKET0_POS_HEAD_MSB] << 8) + buf[VSCP_MULTICAST_PACKET0_POS_HEAD_LSB];

  // Copy in GUID
  memcpy(pEvent->GUID, buf + VSCP_MULTICAST_PACKET0_POS_VSCP_GUID, pEvent->sizeData);

  // Set CRC
  pEvent->crc = crcFrame;

  // Set timestamp
  pEvent->timestamp = ((uint32_t)buf[VSCP_MULTICAST_PACKET0_POS_TIMESTAMP] << 24) +
                      ((uint32_t)buf[VSCP_MULTICAST_PACKET0_POS_TIMESTAMP + 1] << 16) +
                      ((uint32_t)buf[VSCP_MULTICAST_PACKET0_POS_TIMESTAMP + 2] << 8) +
                      buf[VSCP_MULTICAST_PACKET0_POS_TIMESTAMP + 3];

  // Date/time
  pEvent->year   = ((uint16_t)buf[VSCP_MULTICAST_PACKET0_POS_YEAR_MSB] << 8) + buf[VSCP_MULTICAST_PACKET0_POS_YEAR_LSB];
  pEvent->month  = buf[VSCP_MULTICAST_PACKET0_POS_MONTH];
  pEvent->day    = buf[VSCP_MULTICAST_PACKET0_POS_DAY];
  pEvent->hour   = buf[VSCP_MULTICAST_PACKET0_POS_HOUR];
  pEvent->minute = buf[VSCP_MULTICAST_PACKET0_POS_MINUTE];
  pEvent->second = buf[VSCP_MULTICAST_PACKET0_POS_SECOND];

  // VSCP Class
  pEvent->vscp_class =
    ((uint16_t)buf[VSCP_MULTICAST_PACKET0_POS_VSCP_CLASS_MSB] << 8) + buf[VSCP_MULTICAST_PACKET0_POS_VSCP_CLASS_LSB];

  // VSCP Type
  pEvent->vscp_type =
    ((uint16_t)buf[VSCP_MULTICAST_PACKET0_POS_VSCP_TYPE_MSB] << 8) + buf[VSCP_MULTICAST_PACKET0_POS_VSCP_TYPE_LSB];

  // obid - set to zero so interface fill it in
  pEvent->obid = 0;

  return VSCP_ERROR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getEventExFromFrame
//

int
vscp_fwhlp_getEventExFromFrame(vscpEventEx* pEventEx, const uint8_t* frame, size_t len)
{
  int rv;
  vscpEvent* pEvent;

  pEvent = (vscpEvent*)calloc(1, sizeof(vscpEvent));
  if (NULL == pEvent) {
    return VSCP_ERROR_PARAMETER;
  }

  // Check pointer (rest is checked in vscp_getVscpEventFromUdpFrame)
  if (NULL == pEventEx) {
    return VSCP_ERROR_PARAMETER;
  }

  if (VSCP_ERROR_SUCCESS != (rv = vscp_fwhlp_getEventFromFrame(pEvent, frame, len))) {
    vscp_fwhlp_deleteEvent(&pEvent); // Free event storage
    return rv;
  }

  // Convert eventEx to event
  if (VSCP_ERROR_SUCCESS != (rv = vscp_fwhlp_convertEventToEventEx(pEventEx, pEvent))) {
    vscp_fwhlp_deleteEvent(&pEvent); // Free event storage
    return VSCP_ERROR_ERROR;
  }

  vscp_fwhlp_deleteEvent(&pEvent); // Free event storage

  return VSCP_ERROR_SUCCESS;
}

#endif

/*!
  Add AES CBC crypto support
*/
#ifdef VSCP_FWHLP_CRYPTO_SUPPORT

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_encryptFrame
//

size_t
vscp_fwhlp_encryptFrame(uint8_t* output,
                        uint8_t* input,
                        size_t len,
                        const uint8_t* key,
                        const uint8_t* iv,
                        uint8_t nAlgorithm)
{
  uint8_t generated_iv[16];

  // Check pointers
  if (NULL == output) {
    return 0;
  }

  if (NULL == input) {
    return 0;
  }

  if (NULL == key) {
    return 0;
  }

  // If no encryption needed - return
  if (VSCP_ENCRYPTION_NONE == nAlgorithm) {
    memcpy(output, input, len);
    return len;
  }

  // Must be padded if needed (should have length 16).
  // Encrypted len is len-1 because of leading encryption byte
  size_t padlen = len - 1;
  padlen        = padlen + (16 - (padlen % 16));

  // The packet type is always unencrypted
  output[0] = input[0];

  // Should decryption algorithm be set by package
  if (VSCP_ENCRYPTION_FROM_TYPE_BYTE == (nAlgorithm & 0x0f)) {
    nAlgorithm = input[0] & 0x0f;
  }

  // If iv is not give it should be generated
  if (NULL == iv) {
    if (16 != getRandomIV(generated_iv, 16)) {
      return 0;
    }
  }
  else {
    memcpy(generated_iv, iv, 16);
  }

  switch (nAlgorithm) {

    case VSCP_ENCRYPTION_AES192:
      AES_CBC_encrypt_buffer(AES192,
                             output + 1,
                             input + 1, // Not Packet type byte
                             (uint32_t)padlen,
                             key,
                             (const uint8_t*)generated_iv);
      // Append iv
      memcpy(output + 1 + padlen, generated_iv, 16);
      padlen += 16; // length of iv
      break;

    case VSCP_ENCRYPTION_AES256:
      AES_CBC_encrypt_buffer(AES256,
                             output + 1,
                             input + 1, // Not Packet type byte
                             (uint32_t)padlen,
                             key,
                             (const uint8_t*)generated_iv);
      // Append iv
      memcpy(output + 1 + padlen, generated_iv, 16);
      padlen += 16; // length of iv
      break;

    case VSCP_ENCRYPTION_AES128:
      AES_CBC_encrypt_buffer(AES128,
                             output + 1,
                             input + 1, // Not Packet type byte
                             (uint32_t)padlen,
                             key,
                             (const uint8_t*)generated_iv);
      // Append iv
      memcpy(output + 1 + padlen, generated_iv, 16);
      padlen += 16; // length of iv
      break;

    default:
    case VSCP_ENCRYPTION_NONE:
      memcpy(output + 1, input + 1, padlen);
      break;
  }

  // This is needed as byte 0 is not counted and the encrypted data should be
  // a multiple of 16
  padlen++;

  return padlen;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_decryptFrame
//

int
vscp_fwhlp_decryptFrame(uint8_t* output,
                        uint8_t* input,
                        size_t len,
                        const uint8_t* key,
                        const uint8_t* iv,
                        uint8_t nAlgorithm)
{
  uint8_t appended_iv[16];
  size_t real_len = len;

  // Check pointers
  if (NULL == output) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (NULL == input) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (NULL == key) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (VSCP_ENCRYPTION_NONE == GET_VSCP_MULTICAST_PACKET_ENCRYPTION(nAlgorithm)) {
    memcpy(output, input, len);
    return VSCP_ERROR_SUCCESS;
  }

  // If iv is not given it should be fetched from the end of input (last 16
  // bytes)
  if (NULL == iv) {
    memcpy(appended_iv, (input + len - 16), 16);
    real_len -= 16; // Adjust frame length accordingly
  }
  else {
    memcpy(appended_iv, iv, 16);
  }

  // Preserve packet type which always is un-encrypted
  output[0] = input[0];

  // Should decryption algorithm be set by package
  if (VSCP_ENCRYPTION_FROM_TYPE_BYTE == (nAlgorithm & 0x0f)) {
    nAlgorithm = input[0] & 0x0f;
  }

  switch (nAlgorithm) {

    case VSCP_ENCRYPTION_AES256:
      AES_CBC_decrypt_buffer(AES256,
                             output + 1,
                             input + 1,
                             (uint32_t)real_len - 1,
                             key,
                             (const uint8_t*)appended_iv);
      break;

    case VSCP_ENCRYPTION_AES192:
      AES_CBC_decrypt_buffer(AES192,
                             output + 1,
                             input + 1,
                             (uint32_t)real_len - 1,
                             key,
                             (const uint8_t*)appended_iv);
      break;

    default:
    case VSCP_ENCRYPTION_AES128:
      AES_CBC_decrypt_buffer(AES128,
                             output + 1,
                             input + 1,
                             (uint32_t)real_len - 1,
                             key,
                             (const uint8_t*)appended_iv);
      break;
  }

  return VSCP_ERROR_SUCCESS;
}

#endif // VSCP_FWHLP_CRYPTO_SUPPORT

/*
  JSON support needs VSCP_FWHLP_JSON_SUPPORT to be defined
  in the projdef file and cJSON support linked in (can be found
  in vscp-firmware/third-party or at https://github.com/nopnop2002/esp-idf-json
*/

#ifdef VSCP_FWHLP_JSON_SUPPORT

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_parse_json
//
// // https://github.com/nopnop2002/esp-idf-json
/*
{
  "vscpHead":3,
  "vscpClass":10,
  "vscpType":6,
  "vscpGuid":"FF:FF:FF:FF:FF:FF:FF:FE:B8:27:EB:CF:3A:15:00:01",
  "vscpObId":11,
  "vscpTimeStamp":467530633,
  "vscpDateTime":"2022-12-16T16:41:02.000Z",
  "vscpData":[72,51,57,46,55,48,52],
  "measurement": {
    "value":39.704,
    "unit":1,
    "sensorindex":0,
    "index":0,
    "zone":0,
    "subzone":0
  }
}
*/

int
vscp_fwhlp_parse_json(vscpEvent* pev, const char* jsonVscpEventObj)
{
  int rv;
  cJSON* root = cJSON_Parse(jsonVscpEventObj);

  // Check pointers
  if ((NULL == pev) || (NULL == jsonVscpEventObj)) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  // Set unused path to known value
  memset(pev, 0, sizeof(vscpEventEx));

  if (cJSON_GetObjectItem(root, "vscpHead")) {
    pev->head = (uint16_t)cJSON_GetObjectItem(root, "vscpHead")->valueint;
  }

  if (cJSON_GetObjectItem(root, "vscpObId")) {
    pev->obid = (uint32_t)cJSON_GetObjectItem(root, "vscpObId")->valuedouble;
  }

  // "2017-01-13T10:16:02",
  if (cJSON_GetObjectItem(root, "vscpDateTime")) {
    int year, month, day, hour, minute, second;
    const char* str = cJSON_GetObjectItem(root, "vscpDateTime")->valuestring;
    sscanf(str, "%d-%d-%dT%d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    pev->year   = year;
    pev->month  = month;
    pev->day    = day;
    pev->hour   = hour;
    pev->minute = minute;
    pev->second = second;
  }

  if (cJSON_GetObjectItem(root, "vscpTimeStamp")) {
    pev->timestamp = (uint32_t)cJSON_GetObjectItem(root, "vscpTimeStamp")->valuedouble;
  }

  if (cJSON_GetObjectItem(root, "vscpClass")) {
    pev->vscp_class = (uint16_t)cJSON_GetObjectItem(root, "vscpClass")->valueint;
    ;
  }

  if (cJSON_GetObjectItem(root, "vscpType")) {
    pev->vscp_type = (uint16_t)cJSON_GetObjectItem(root, "vscpType")->valueint;
  }

  if (cJSON_GetObjectItem(root, "vscpGuid")) {
    const char* str = cJSON_GetObjectItem(root, "vscpGuid")->valuestring;
    if (VSCP_ERROR_SUCCESS != (rv = vscp_fwhlp_parseGuid(pev->GUID, str, NULL))) {
      return rv;
    }
  }

  if (cJSON_GetObjectItem(root, "vscpData")) {

    cJSON* pdata  = cJSON_GetObjectItem(root, "vscpData");
    pev->sizeData = cJSON_GetArraySize(pdata);

    for (int i = 0; i < pev->sizeData; i++) {
      cJSON* pitem = cJSON_GetArrayItem(pdata, i);
      if (pitem->type == cJSON_Number && i < 512) {
        pev->pdata[i] = pitem->valueint;
      }
    }
  }

  cJSON_Delete(root);
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_parse_json_ex
//
// // https://github.com/nopnop2002/esp-idf-json
/*
{
  "vscpHead":3,
  "vscpClass":10,
  "vscpType":6,
  "vscpGuid":"FF:FF:FF:FF:FF:FF:FF:FE:B8:27:EB:CF:3A:15:00:01",
  "vscpObId":11,
  "vscpTimeStamp":467530633,
  "vscpDateTime":"2022-12-16T16:41:02.000Z",
  "vscpData":[72,51,57,46,55,48,52],
  "measurement": {
    "value":39.704,
    "unit":1,
    "sensorindex":0,
    "index":0,
    "zone":0,
    "subzone":0
  }
}
*/

int
vscp_fwhlp_parse_json_ex(vscpEventEx* pex, const char* jsonVscpEventObj)
{
  int rv;
  cJSON* root = cJSON_Parse(jsonVscpEventObj);

  // Check pointers
  if ((NULL == pex) || (NULL == jsonVscpEventObj)) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  // Set unused path to known value
  memset(pex, 0, sizeof(vscpEventEx));

  if (cJSON_GetObjectItem(root, "vscpHead")) {
    pex->head = (uint16_t)cJSON_GetObjectItem(root, "vscpHead")->valueint;
  }

  if (cJSON_GetObjectItem(root, "vscpObId")) {
    pex->obid = (uint32_t)cJSON_GetObjectItem(root, "vscpObId")->valuedouble;
  }

  // "2017-01-13T10:16:02",
  if (cJSON_GetObjectItem(root, "vscpDateTime")) {
    int year, month, day, hour, minute, second;
    const char* str = cJSON_GetObjectItem(root, "vscpDateTime")->valuestring;
    sscanf(str, "%d-%d-%dT%d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    pex->year   = year;
    pex->month  = month;
    pex->day    = day;
    pex->hour   = hour;
    pex->minute = minute;
    pex->second = second;
  }

  if (cJSON_GetObjectItem(root, "vscpTimeStamp")) {
    pex->timestamp = (uint32_t)cJSON_GetObjectItem(root, "vscpTimeStamp")->valuedouble;
  }

  if (cJSON_GetObjectItem(root, "vscpClass")) {
    pex->vscp_class = (uint16_t)cJSON_GetObjectItem(root, "vscpClass")->valueint;
    ;
  }

  if (cJSON_GetObjectItem(root, "vscpType")) {
    pex->vscp_type = (uint16_t)cJSON_GetObjectItem(root, "vscpType")->valueint;
  }

  if (cJSON_GetObjectItem(root, "vscpGuid")) {
    const char* str = cJSON_GetObjectItem(root, "vscpGuid")->valuestring;
    if (VSCP_ERROR_SUCCESS != (rv = vscp_fwhlp_parseGuid(pex->GUID, str, NULL))) {
      return rv;
    }
  }

  if (cJSON_GetObjectItem(root, "vscpData")) {

    cJSON* pdata  = cJSON_GetObjectItem(root, "vscpData");
    pex->sizeData = cJSON_GetArraySize(pdata);

    for (int i = 0; i < pex->sizeData; i++) {
      cJSON* pitem = cJSON_GetArrayItem(pdata, i);
      if (pitem->type == cJSON_Number && i < 512) {
        pex->data[i] = pitem->valueint;
      }
    }
  }

  cJSON_Delete(root);
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_create_json
//
// https://github.com/nopnop2002/esp-idf-json
//

int
vscp_fwhlp_create_json(char* strObj, size_t len, const vscpEvent* pev)
{
  char str[80];
  cJSON* root;

  // Check pointers
  if ((NULL == strObj) || (NULL == pev)) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  root = cJSON_CreateObject();

  cJSON_AddNumberToObject(root, "vscpHead", pev->head);
  cJSON_AddNumberToObject(root, "vscpClass", pev->vscp_class);
  cJSON_AddNumberToObject(root, "vscpType", pev->vscp_type);
  cJSON_AddNumberToObject(root, "vscpObid", pev->obid);
  cJSON_AddNumberToObject(root, "vscpTimeStamp", pev->timestamp);
  vscp_fwhlp_writeGuidToString(str, pev->GUID);
  cJSON_AddStringToObject(root, "vscpGUID", str);
  sprintf(str, "%04d-%02d-%02dT%02d:%02d:%02d", pev->year, pev->month, pev->day, pev->hour, pev->minute, pev->second);
  cJSON_AddStringToObject(root, "vscpDateTime", str);
  cJSON* array;
  array = cJSON_AddArrayToObject(root, "vscpData");
  cJSON* element;
  for (int i = 0; i < pev->sizeData; i++) {
    element = cJSON_CreateNumber(pev->pdata[i]);
    cJSON_AddItemToArray(array, element);
  }
  char* json_string = cJSON_Print(root);
  if (NULL == json_string) {
    cJSON_Delete(root);
    return VSCP_ERROR_PARSING;
  }

  if (strlen(json_string) > (len - 1)) {
    cJSON_free(json_string);
    cJSON_Delete(root);
    return VSCP_ERROR_BUFFER_TO_SMALL;
  }

  strcpy(strObj, json_string);

  cJSON_free(json_string);
  cJSON_Delete(root);

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_create_json_ex
//
// https://github.com/nopnop2002/esp-idf-json
//

int
vscp_fwhlp_create_json_ex(char* strObj, size_t len, const vscpEventEx* pex)
{
  char str[80];
  cJSON* root;

  // Check pointers
  if ((NULL == strObj) || (NULL == pex)) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  root = cJSON_CreateObject();

  cJSON_AddNumberToObject(root, "vscpHead", pex->head);
  cJSON_AddNumberToObject(root, "vscpClass", pex->vscp_class);
  cJSON_AddNumberToObject(root, "vscpType", pex->vscp_type);
  cJSON_AddNumberToObject(root, "vscpObid", pex->obid);
  cJSON_AddNumberToObject(root, "vscpTimeStamp", pex->timestamp);
  vscp_fwhlp_writeGuidToString(str, pex->GUID);
  cJSON_AddStringToObject(root, "vscpGUID", str);
  sprintf(str, "%04d-%02d-%02dT%02d:%02d:%02d", pex->year, pex->month, pex->day, pex->hour, pex->minute, pex->second);
  cJSON_AddStringToObject(root, "vscpDateTime", str);
  cJSON* array;
  array = cJSON_AddArrayToObject(root, "vscpData");
  cJSON* element;
  for (int i = 0; i < pex->sizeData; i++) {
    element = cJSON_CreateNumber(pex->data[i]);
    cJSON_AddItemToArray(array, element);
  }
  char* json_string = cJSON_Print(root);
  if (NULL == json_string) {
    cJSON_Delete(root);
    return VSCP_ERROR_PARSING;
  }

  if (strlen(json_string) > (len - 1)) {
    cJSON_free(json_string);
    cJSON_Delete(root);
    return VSCP_ERROR_BUFFER_TO_SMALL;
  }

  strcpy(strObj, json_string);

  cJSON_free(json_string);
  cJSON_Delete(root);

  return VSCP_ERROR_SUCCESS;
}

#endif // JSON support

// ----------------------------------------------------------------------------

// ***************************************************************************
//                              Data Coding Helpers
// ***************************************************************************

//////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getMeasurementDataCoding
//

uint8_t
vscp_fwhlp_getMeasurementDataCoding(const vscpEvent* pEvent)
{
  uint8_t datacoding_byte = -1;

  // Check pointers
  if (NULL == pEvent) {
    return -1;
  }

  if (NULL == pEvent->pdata) {
    return -1;
  }

  // Check datasize
  if (pEvent->sizeData < 1) {
    return -1;
  }

  if (VSCP_CLASS1_MEASUREMENT == pEvent->vscp_class) {
    datacoding_byte = pEvent->pdata[0];
  }
  else if (VSCP_CLASS2_LEVEL1_MEASUREMENT == pEvent->vscp_class) {
    if (pEvent->sizeData >= 16) {
      datacoding_byte = pEvent->pdata[16];
    }
  }
  else if (VSCP_CLASS2_MEASUREMENT_STR == pEvent->vscp_class) {
    datacoding_byte = 0x40; // string
  }
  else if (VSCP_CLASS2_MEASUREMENT_FLOAT == pEvent->vscp_class) {
    datacoding_byte = 0xC0; // float
  }

  return datacoding_byte;
}

//////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getDataCodingBitArray
//

uint64_t
vscp_fwhlp_getDataCodingBitArray(const uint8_t* pCode, const uint8_t length)
{
  uint64_t bitArray = 0;

  // Check pointer
  if (NULL == pCode) {
    return 0;
  }

  if ((length > 7) || (length <= 1)) {
    return 0;
  }

  for (int i = 0; i < length - 1; i++) {
    bitArray = bitArray << 8;
    bitArray |= *(pCode + 1 + i);
  }

  return bitArray;
}

//////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getDataCodingInteger
//

int64_t
vscp_fwhlp_getDataCodingInteger(const uint8_t* pCode, uint8_t length)
{
  int64_t value64 = 0;

  // Check pointer
  if (NULL == pCode) {
    return 0;
  }

  if (length < 2) {
    return 0;
  }

  // Check if this is a negative number
  if ((*(pCode + 1)) & 0x80) {
    memset((uint8_t*)&value64, 0xff, 8);
  }
  else {
    memset((uint8_t*)&value64, 0, 8);
  }

  for (int i = 1; i < length; i++) {
    value64 = (value64 << 8) + pCode[i];
  }

  return value64;
}

//////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getDataCodingNormalizedInteger
//

double
vscp_fwhlp_getDataCodingNormalizedInteger(const uint8_t* pCode, uint8_t length)
{
  uint8_t valarray[8];
  uint8_t normbyte;
  uint8_t decibyte;
#ifndef __BIG_ENDIAN__
  int64_t value64;
#endif
  double value  = 0;
  int bNegative = FALSE; // set for negative number

  // Check pointer
  if (NULL == pCode) {
    return 0;
  }

  // Check character count
  if ((length > 8) || (length < 2)) {
    return 0;
  }

  memset(valarray, 0, sizeof(valarray));
  normbyte = *pCode;
  UNUSED(normbyte);
  decibyte = *(pCode + 1);

  // Check if this is a negative number
  if ((*(pCode + 2)) & 0x80) {
    bNegative = TRUE;
  }

  switch (length - 2) {

    case 1: // 8-bit
      memcpy((char*)&valarray, (pCode + 2), (length - 2));
      value = *((int8_t*)valarray);
      break;

    case 2: // 16-bit
      memcpy((char*)&valarray, (pCode + 2), (length - 2));
      value = VSCP_INT16_SWAP_ON_LE(*((uint16_t*)valarray));
      break;

    case 3: // 24-bit
      memcpy(((char*)&valarray + 1), (pCode + 2), (length - 2));
      if (bNegative)
        *valarray = 0xff; // First byte must be 0xff
      value = VSCP_INT32_SWAP_ON_LE(*((int32_t*)valarray));
      break;

    case 4: // 32-bit
      memcpy((char*)&valarray, (pCode + 2), (length - 2));
      value = VSCP_INT32_SWAP_ON_LE(*((int32_t*)valarray));
      break;

    case 5: // 40-bit
      memcpy(((char*)&valarray + 3), (pCode + 2), (length - 2));
      if (bNegative) {
        *valarray       = 0xff; // First byte must be 0xff
        *(valarray + 1) = 0xff;
        *(valarray + 2) = 0xff;
      }

#ifdef __BIG_ENDIAN__
      value = *((int64_t*)valarray);
#else
      value64 = Swap8Bytes(*((int64_t*)valarray));
      value   = (double)value64;
#endif
      break;

    case 6: // 48-bit
      memcpy(((char*)&valarray + 2), (pCode + 2), (length - 2));
      if (bNegative) {
        *valarray       = 0xff; // First byte must be 0xff
        *(valarray + 1) = 0xff;
      }

#ifdef __BIG_ENDIAN__
      value = *((int64_t*)valarray);
#else
      value64 = Swap8Bytes(*((int64_t*)valarray));
      value   = (double)value64;
#endif
      break;
  }

    // Bring back decimal points
#ifdef WIN32
  if (decibyte & 0x80) {
    decibyte &= 0x7f;
    value = value / (pow(10.0, decibyte));
  }
  else {
    decibyte &= 0x7f;
    value = value * (pow(10.0, decibyte));
  }
#else
  if (decibyte & 0x80) {
    decibyte &= 0x7f;
    value = value / (pow(10, decibyte));
  }
  else {
    decibyte &= 0x7f;
    value = value * (pow(10, decibyte));
  }
#endif
  return value;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getEventPriority

// unsigned char
// vscp_fwhlp_getEventPriority(const vscpEvent *pEvent)
// {
//   // Must be a valid message pointer
//   if (NULL == pEvent) {
//     return 0;
//   }

//   return ((pEvent->head >> 5) & 0x07);
// }

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_setEventPriority

void
vscp_fwhlp_setEventPriority(vscpEvent* pEvent, unsigned char priority)
{
  // Must be a valid message pointer
  if (NULL == pEvent) {
    return;
  }

  pEvent->head &= ~VSCP_HEADER_PRIORITY_MASK;
  pEvent->head |= (priority << 5);
}

//////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getMeasurementUnit
//

int
vscp_fwhlp_getMeasurementUnit(const vscpEvent* pEvent)
{
  int offset = 0;

  // If class >= 512 and class < 1024 we
  // have GUID in front of data.
  if ((pEvent->vscp_class >= VSCP_CLASS2_LEVEL1_PROTOCOL) && (pEvent->vscp_class < VSCP_CLASS2_PROTOCOL)) {
    offset = 16;
  }

  if ((VSCP_CLASS1_MEASUREMENT == pEvent->vscp_class) || (VSCP_CLASS2_LEVEL1_MEASUREMENT == pEvent->vscp_class) ||
      (VSCP_CLASS1_DATA == pEvent->vscp_class) || (VSCP_CLASS1_MEASUREZONE == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREZONE == pEvent->vscp_class) || (VSCP_CLASS1_SETVALUEZONE == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_SETVALUEZONE == pEvent->vscp_class)) {

    if ((NULL == pEvent->pdata) || (pEvent->sizeData < (offset + 1))) {
      return VSCP_ERROR_ERROR;
    }

    return VSCP_DATACODING_UNIT(pEvent->pdata[offset + 0]);
  }
  else if ((VSCP_CLASS1_MEASUREMENT32 == pEvent->vscp_class) ||
           (VSCP_CLASS2_LEVEL1_MEASUREMENT32 == pEvent->vscp_class)) {
    return 0; // Always default unit
  }
  else if ((VSCP_CLASS1_MEASUREMENT64 == pEvent->vscp_class) ||
           (VSCP_CLASS2_LEVEL1_MEASUREMENT64 == pEvent->vscp_class)) {
    return 0; // Always default unit
  }
  else if ((VSCP_CLASS2_MEASUREMENT_STR == pEvent->vscp_class)) {

    // Check if data length is valid
    if ((NULL == pEvent->pdata) || (pEvent->sizeData < 4)) {
      return VSCP_ERROR_ERROR;
    }

    return pEvent->pdata[3];
  }
  else if ((VSCP_CLASS2_MEASUREMENT_FLOAT == pEvent->vscp_class)) {

    // Check if data length is valid
    if ((NULL == pEvent->pdata) || (12 != pEvent->sizeData)) {
      return VSCP_ERROR_ERROR;
    }

    return pEvent->pdata[3];
  }

  return VSCP_ERROR_ERROR;
}

//////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getMeasurementSensorIndex
//

int
vscp_fwhlp_getMeasurementSensorIndex(const vscpEvent* pEvent)
{
  int offset = 0;

  // Must have valid event and data
  if ((NULL == pEvent) || (NULL == pEvent->pdata) || !pEvent->sizeData) {
    return -1;
  }

  // If class >= 512 and class < 1024 we
  // have GUID in front of data.
  if ((pEvent->vscp_class >= VSCP_CLASS2_LEVEL1_PROTOCOL) && (pEvent->vscp_class < VSCP_CLASS2_PROTOCOL)) {
    offset = 16;
  }

  if ((VSCP_CLASS1_MEASUREMENT == pEvent->vscp_class) || (VSCP_CLASS1_DATA == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENT == pEvent->vscp_class)) {

    return VSCP_DATACODING_INDEX(pEvent->pdata[offset + 0]);
  }
  else if ((VSCP_CLASS1_MEASUREZONE == pEvent->vscp_class) || (VSCP_CLASS2_LEVEL1_MEASUREZONE == pEvent->vscp_class) ||
           (VSCP_CLASS1_SETVALUEZONE == pEvent->vscp_class) ||
           (VSCP_CLASS2_LEVEL1_SETVALUEZONE == pEvent->vscp_class)) {

    if ((NULL == pEvent->pdata) || (pEvent->sizeData >= (offset + 3))) {
      return -1;
    }

    return pEvent->pdata[offset + 0];
  }
  else if ((VSCP_CLASS1_MEASUREMENT32 == pEvent->vscp_class) ||
           (VSCP_CLASS2_LEVEL1_MEASUREMENT32 == pEvent->vscp_class)) {
    return 0; // Sensor index is always zero
  }
  else if ((VSCP_CLASS1_MEASUREMENT64 == pEvent->vscp_class) ||
           (VSCP_CLASS2_LEVEL1_MEASUREMENT64 == pEvent->vscp_class)) {
    return 0; // Sensor index is always zero
  }
  else if ((VSCP_CLASS2_MEASUREMENT_STR == pEvent->vscp_class)) {

    // Check if data length is valid
    if ((NULL == pEvent->pdata) || (pEvent->sizeData < 4)) {
      return VSCP_ERROR_ERROR;
    }

    return pEvent->pdata[0];
  }
  else if ((VSCP_CLASS2_MEASUREMENT_FLOAT == pEvent->vscp_class)) {

    // Check if data length is valid
    if ((NULL == pEvent->pdata) || (12 != pEvent->sizeData)) {
      return VSCP_ERROR_ERROR;
    }

    return pEvent->pdata[0];
  }

  return VSCP_ERROR_ERROR;
}

//////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getMeasurementZone
//

int
vscp_fwhlp_getMeasurementZone(const vscpEvent* pEvent)
{
  int offset = 0;

  // Must have valid event and data
  if ((NULL == pEvent) || (NULL == pEvent->pdata) || !pEvent->sizeData) {
    return -1;
  }

  // If class >= 512 and class < 1024 we
  // have GUID in front of data.
  if ((pEvent->vscp_class >= VSCP_CLASS2_LEVEL1_PROTOCOL) && (pEvent->vscp_class < VSCP_CLASS2_PROTOCOL)) {
    offset = 16;
  }

  if ((VSCP_CLASS1_MEASUREMENT == pEvent->vscp_class) || (VSCP_CLASS1_DATA == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENT == pEvent->vscp_class)) {

    return 0; // Always zero
  }
  else if ((VSCP_CLASS1_MEASUREZONE == pEvent->vscp_class) || (VSCP_CLASS2_LEVEL1_MEASUREZONE == pEvent->vscp_class) ||
           (VSCP_CLASS1_SETVALUEZONE == pEvent->vscp_class) ||
           (VSCP_CLASS2_LEVEL1_SETVALUEZONE == pEvent->vscp_class)) {

    if ((NULL == pEvent->pdata) || (pEvent->sizeData >= (offset + 3))) {
      return 0;
    }

    return pEvent->pdata[offset + 1];
  }
  else if ((VSCP_CLASS1_MEASUREMENT32 == pEvent->vscp_class) ||
           (VSCP_CLASS2_LEVEL1_MEASUREMENT32 == pEvent->vscp_class)) {
    return 0; // Sensor index is always zero
  }
  else if ((VSCP_CLASS1_MEASUREMENT64 == pEvent->vscp_class) ||
           (VSCP_CLASS2_LEVEL1_MEASUREMENT64 == pEvent->vscp_class)) {
    return 0; // Sensor index is always zero
  }
  else if ((VSCP_CLASS2_MEASUREMENT_STR == pEvent->vscp_class)) {

    // Check if data length is valid
    if ((NULL == pEvent->pdata) || (pEvent->sizeData < 4)) {
      return VSCP_ERROR_ERROR;
    }

    return pEvent->pdata[2];
  }
  else if ((VSCP_CLASS2_MEASUREMENT_FLOAT == pEvent->vscp_class)) {

    // Check if data length is valid
    if ((NULL == pEvent->pdata) || (12 != pEvent->sizeData)) {
      return VSCP_ERROR_ERROR;
    }

    return pEvent->pdata[2];
  }

  return VSCP_ERROR_ERROR;
}

//////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getMeasurementSubZone
//

int
vscp_fwhlp_getMeasurementSubZone(const vscpEvent* pEvent)
{
  int offset = 0;

  // Must have valid event and data
  if ((NULL == pEvent) || (NULL == pEvent->pdata) || !pEvent->sizeData) {
    return -1;
  }

  // If class >= 512 and class < 1024 we
  // have GUID in front of data.
  if ((pEvent->vscp_class >= VSCP_CLASS2_LEVEL1_PROTOCOL) && (pEvent->vscp_class < VSCP_CLASS2_PROTOCOL)) {
    offset = 16;
  }

  if ((VSCP_CLASS1_MEASUREMENT == pEvent->vscp_class) || (VSCP_CLASS1_DATA == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENT == pEvent->vscp_class) || (VSCP_CLASS1_MEASUREZONE == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREZONE == pEvent->vscp_class) || (VSCP_CLASS1_SETVALUEZONE == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_SETVALUEZONE == pEvent->vscp_class)) {

    if ((NULL == pEvent->pdata) || (pEvent->sizeData >= (offset + 1))) {
      return 0;
    }

    return 0; // Always zero
  }
  else if ((VSCP_CLASS1_MEASUREMENT32 == pEvent->vscp_class) ||
           (VSCP_CLASS2_LEVEL1_MEASUREMENT32 == pEvent->vscp_class)) {
    return 0; // Sensor index is always zero
  }
  else if ((VSCP_CLASS1_MEASUREMENT64 == pEvent->vscp_class) ||
           (VSCP_CLASS2_LEVEL1_MEASUREMENT64 == pEvent->vscp_class)) {
    return 0; // Sensor index is always zero
  }
  else if ((VSCP_CLASS2_MEASUREMENT_STR == pEvent->vscp_class)) {

    // Check if data length is valid
    if ((NULL == pEvent->pdata) || (pEvent->sizeData < 4)) {
      return VSCP_ERROR_ERROR;
    }

    return pEvent->pdata[offset + 2];
  }
  else if ((VSCP_CLASS2_MEASUREMENT_FLOAT == pEvent->vscp_class)) {

    // Check if data length is valid
    if ((NULL == pEvent->pdata) || (12 != pEvent->sizeData)) {
      return VSCP_ERROR_ERROR;
    }

    return pEvent->pdata[2];
  }

  return VSCP_ERROR_ERROR;
}

//////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_isMeasurement
//

int
vscp_fwhlp_isMeasurement(const vscpEvent* pEvent)
{
  if (NULL == pEvent) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if ((VSCP_CLASS1_MEASUREMENT == pEvent->vscp_class) || (VSCP_CLASS1_MEASUREMENTX1 == pEvent->vscp_class) ||
      (VSCP_CLASS1_MEASUREMENTX2 == pEvent->vscp_class) || (VSCP_CLASS1_MEASUREMENTX3 == pEvent->vscp_class) ||
      (VSCP_CLASS1_MEASUREMENTX4 == pEvent->vscp_class) || (VSCP_CLASS1_DATA == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENT == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENTX1 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENTX2 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENTX3 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENTX4 == pEvent->vscp_class) || (VSCP_CLASS1_MEASUREZONE == pEvent->vscp_class) ||
      (VSCP_CLASS1_MEASUREZONEX1 == pEvent->vscp_class) || (VSCP_CLASS1_MEASUREZONEX2 == pEvent->vscp_class) ||
      (VSCP_CLASS1_MEASUREZONEX3 == pEvent->vscp_class) || (VSCP_CLASS1_MEASUREZONEX4 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREZONE == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREZONEX1 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREZONEX2 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREZONEX3 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREZONEX4 == pEvent->vscp_class) || (VSCP_CLASS1_SETVALUEZONE == pEvent->vscp_class) ||
      (VSCP_CLASS1_SETVALUEZONEX1 == pEvent->vscp_class) || (VSCP_CLASS1_SETVALUEZONEX2 == pEvent->vscp_class) ||
      (VSCP_CLASS1_SETVALUEZONEX3 == pEvent->vscp_class) || (VSCP_CLASS1_SETVALUEZONEX4 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_SETVALUEZONE == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_SETVALUEZONEX1 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_SETVALUEZONEX2 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_SETVALUEZONEX3 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_SETVALUEZONEX4 == pEvent->vscp_class) || (VSCP_CLASS1_MEASUREMENT32 == pEvent->vscp_class) ||
      (VSCP_CLASS1_MEASUREMENT32X1 == pEvent->vscp_class) || (VSCP_CLASS1_MEASUREMENT32X2 == pEvent->vscp_class) ||
      (VSCP_CLASS1_MEASUREMENT32X3 == pEvent->vscp_class) || (VSCP_CLASS1_MEASUREMENT32X4 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENT32 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENT32X1 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENT32X2 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENT32X3 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENT32X4 == pEvent->vscp_class) || (VSCP_CLASS1_MEASUREMENT64 == pEvent->vscp_class) ||
      (VSCP_CLASS1_MEASUREMENT64X1 == pEvent->vscp_class) || (VSCP_CLASS1_MEASUREMENT64X2 == pEvent->vscp_class) ||
      (VSCP_CLASS1_MEASUREMENT64X3 == pEvent->vscp_class) || (VSCP_CLASS1_MEASUREMENT64X4 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENT64 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENT64X1 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENT64X2 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENT64X3 == pEvent->vscp_class) ||
      (VSCP_CLASS2_LEVEL1_MEASUREMENT64X4 == pEvent->vscp_class) ||
      (VSCP_CLASS2_MEASUREMENT_STR == pEvent->vscp_class) || (VSCP_CLASS2_MEASUREMENT_FLOAT == pEvent->vscp_class)) {
    return VSCP_ERROR_SUCCESS;
  }

  return VSCP_ERROR_ERROR;
}

// ****************************
//            CANAL
// ****************************

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getHeadFromCANALid
//

unsigned char
vscp_fwhlp_getHeadFromCANALid(uint32_t id)
{
  uint8_t hardcoded = 0;
  uint8_t priority  = (0x07 & (id >> 26));
  if (id & (1 << 25)) {
    hardcoded = VSCP_HEADER_HARD_CODED;
  }

  return ((priority << 5) | hardcoded);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getVscpClassFromCANALid
//

uint16_t
vscp_fwhlp_getVscpClassFromCANALid(uint32_t id)
{
  return (uint16_t)(0x1ff & (id >> 16));
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getVscpTypeFromCANALid
//

uint16_t
vscp_fwhlp_getVscpTypeFromCANALid(uint32_t id)
{
  return (uint16_t)(0xff & (id >> 8));
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlpgetNicknameFromCANALid
//

uint8_t
vscp_fwhlp_getNicknameFromCANALid(uint32_t id)
{
  return (id & 0xff);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getCANALidFromData
//

uint32_t
vscp_fwhlp_getCANALidFromData(unsigned char priority, const uint16_t vscp_class, const uint16_t vscp_type)
{
  // unsigned long t1 = (unsigned long)priority << 20;
  // unsigned long t2 = (unsigned long)pvscpMsg->vscp_class << 16;
  // unsigned long t3 = (unsigned long)pvscpMsg->vscp_type << 8;
  return (((unsigned long)priority << 26) | ((unsigned long)vscp_class << 16) | ((unsigned long)vscp_type << 8) | 0);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getCANALidFromEvent
//

uint32_t
vscp_fwhlp_getCANALidFromEvent(const vscpEvent* pEvent)
{
  return (((unsigned long)vscp_fwhlp_getEventPriority(pEvent) << 26) | ((unsigned long)pEvent->vscp_class << 16) |
          ((unsigned long)pEvent->vscp_type << 8) | 0);
}
