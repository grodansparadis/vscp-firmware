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

#include "vscp-compiler.h"
#include "vscp-projdefs.h"

#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vscp_aes.h>
#include <vscp.h>
#include "vscp-firmware-helper.h"


// case-independent ASCII character equality comparison
#define CIEQ(c1, c2) (((c1) & ~040) == ((c2) & ~040))

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
  vscpEvent* pev = NULL;

  pev = (vscpEvent*)VSCP_MALLOC(sizeof(vscpEvent));
  if (NULL != pev) {
    memset(pev, 0, sizeof(vscpEvent));
  }

  return pev;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_mkEventCopy
//

vscpEvent*
vscp_fwhlp_mkEventCopy(vscpEvent* pev)
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
    pnewEvent->pdata = (uint8_t*)VSCP_MALLOC(pev->sizeData);
    if (NULL == pnewEvent->pdata) {
      VSCP_FREE(pnewEvent);
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
// vscp_fwhlp_deleteEvent
//

int
vscp_fwhlp_deleteEvent(vscpEvent** pev)
{
  if (NULL == *pev)
    return VSCP_ERROR_INVALID_POINTER;

  if (NULL != (*pev)->pdata) {
    VSCP_FREE((*pev)->pdata);
  }

  VSCP_FREE(*pev);
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

  // obid
  pev->obid = (uint32_t)strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond comma
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_PARAMETER;
  }

  // datetime YYYY-MM-DDTHH:MM:SS

  // Date may not be given (left blank) and in
  // this case it should be set to all nulls for
  // the interface to set it instead.

  if (',' == *p) {
    p++; // point beyond comma
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

  // timestamp
  pev->timestamp = (uint32_t)strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond comma
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_PARAMETER;
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
    pev->pdata = VSCP_MALLOC(pev->sizeData);
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

  // obid
  pex->obid = (uint32_t)strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond comma
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_PARAMETER;
  }

  // datetime YYYY-MM-DDTHH:MM:SS

  // Date may not be given (left blank) and in
  // this case it should be set to all nulls for
  // the interface to set it instead.

  if (',' == *p) {
    p++; // point beyond comma
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

  // timestamp
  pex->timestamp = (uint32_t)strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond comma
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_PARAMETER;
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
  if (size < ((pev->sizeData * 5) + 110 + 1)) {
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
  if (size < ((pex->sizeData * 5) + 110 + 1)) {
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
  // A nullptr filter is wildcard
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
  // A nullptr filter is wildcard
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

///////////////////////////////////////////////////////////////////////////////
// vscp_encryptFrame
//

size_t
vscp_encryptFrame(uint8_t *output,
                  uint8_t *input,
                  size_t len,
                  const uint8_t *key,
                  const uint8_t *iv,
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

  // Must pad if needed
  size_t padlen = len - 1; // Without packet type
  padlen        = len + (16 - (len % 16));

  // The packet type s always un encrypted
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
                             (uint32_t) padlen,
                             key,
                             (const uint8_t *) generated_iv);
      // Append iv
      memcpy(output + 1 + padlen, generated_iv, 16);
      padlen += 16;
      break;

    case VSCP_ENCRYPTION_AES256:
      AES_CBC_encrypt_buffer(AES256,
                             output + 1,
                             input + 1, // Not Packet type byte
                             (uint32_t) padlen,
                             key,
                             (const uint8_t *) generated_iv);
      // Append iv
      memcpy(output + 1 + padlen, generated_iv, 16);
      padlen += 16;
      break;

    case VSCP_ENCRYPTION_AES128:
      AES_CBC_encrypt_buffer(AES128,
                             output + 1,
                             input + 1, // Not Packet type byte
                             (uint32_t) padlen,
                             key,
                             (const uint8_t *) generated_iv);
      // Append iv
      memcpy(output + 1 + padlen, generated_iv, 16);
      padlen += 16;
      break;

    default:
    case VSCP_ENCRYPTION_NONE:
      memcpy(output + 1, input + 1, padlen);
      break;
  }

  padlen++; // Count packet type byte

  return padlen;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_encryptFrame
//

int
vscp_decryptFrame(uint8_t *output,
                  uint8_t *input,
                  size_t len,
                  const uint8_t *key,
                  const uint8_t *iv,
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
                             (uint32_t) real_len - 1,
                             key,
                             (const uint8_t *) appended_iv);
      break;

    case VSCP_ENCRYPTION_AES192:
      AES_CBC_decrypt_buffer(AES192,
                             output + 1,
                             input + 1,
                             (uint32_t) real_len - 1,
                             key,
                             (const uint8_t *) appended_iv);
      break;

    default:
    case VSCP_ENCRYPTION_AES128:
      AES_CBC_decrypt_buffer(AES128,
                             output + 1,
                             input + 1,
                             (uint32_t) real_len - 1,
                             key,
                             (const uint8_t *) appended_iv);
      break;
  }

  return VSCP_ERROR_SUCCESS;
}