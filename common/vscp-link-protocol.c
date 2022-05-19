// FILE: vscp-link-protocol.c

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol)
 * 	https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2022 Ake Hedman, Grodans Paradis AB
 *<info@grodansparadis.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 *all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *	This file is part of VSCP - Very Simple Control Protocol
 *	https://www.vscp.org
 *
 * ******************************************************************************
 */

#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vscp-compiler.h"
#include "vscp-projdefs.h"

#include "vscp-link-protocol.h"
#include "vscp.h"

// case-independent ASCII character equality comparison
#define CIEQ(c1, c2) (((c1) & ~040) == ((c2) & ~040))

////////////////////////////////////////////////////////////////////////////////////
// vscp_link_a2ul
//

int
vscp_link_a2ul(const char* src, uint8_t srclen, uint8_t base, uint32_t* resultp)
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
      return vscp_link_a2ul(src + 2, srclen - 2, 16, resultp);
    }

    if (srclen > 1 && *src == '0' && base != 13) {
      return vscp_link_a2ul(src + 1, srclen - 1, 8, resultp);
    }

    return vscp_link_a2ul(src, srclen, 10, resultp);
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
// vscp_link_dec2hex
//

void
vscp_link_dec2hex(uint8_t dec, char* pBuf, uint16_t len)
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
// vscp_link_hex2dec
//

uint32_t
vscp_link_hex2dec(const char* pHex)
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
// vscp_link_readStringValue
//

uint32_t
vscp_link_readStringValue(const char* pString)
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
    vscp_link_a2ul(buf, 0, 16, &ul);
    return ul;
  }
  if (NULL != (p = strstr(buf, "0"))) {
    p += 2; // Point at the data
    vscp_link_a2ul(buf, 0, 8, &ul);
    return ul;
  }
  if (NULL != (p = strstr(buf, "0b"))) {
    p += 2; // Point at the data
    vscp_link_a2ul(buf, 0, 2, &ul);
    return ul;
  }
  else {
    ul = (unsigned)atoi(buf);
    return ul;
  }

  return ul;
}

////////////////////////////////////////////////////////////////////////////////////
// vscp_link_stristr
//

char*
vscp_link_stristr(const char* haystack, const char* needle)
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
// vscp_link_getEventPriority

unsigned char
vscp_link_getEventPriority(const vscpEvent *pEvent)
{
  // Must be a valid message pointer
  if (NULL == pEvent) {
    return 0;
  }

  return ((pEvent->head >> 5) & 0x07);
}

// ----------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////
// newEvent
//

vscpEvent*
vscp_link_newEvent(void)
{
  vscpEvent* pev = NULL;

  pev = (vscpEvent*)VSCP_LINK_MALLOC(sizeof(vscpEvent));
  if (NULL != pev) {
    pev->pdata = NULL;
  }

  return pev;
}

///////////////////////////////////////////////////////////////////////////////
// deleteEvent
//

int
vscp_link_deleteEvent(vscpEvent** pev)
{
  if (NULL == *pev)
    return VSCP_ERROR_INVALID_POINTER;

  if (NULL != (*pev)->pdata) {
    VSCP_LINK_FREE((*pev)->pdata);
  }

  VSCP_LINK_FREE(*pev);
  *pev = NULL;

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// parseGuid
//

int
vscp_link_parseGuid(uint8_t* guid, const char* strguid, char** endptr)
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
// writeGuidToString
//

int
vscp_link_writeGuidToString(char* strguid, const uint8_t* guid)
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
// parseFilter
//
// filter-priority, filter-class, filter-type, filter-GUID
//

int
vscp_link_parseFilter(vscpEventFilter* pfilter, const char* strfilter)
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
  return vscp_link_parseGuid(pfilter->filter_GUID, p, NULL);
}

///////////////////////////////////////////////////////////////////////////////
// parseMask
//
// mask-priority, mask-class, mask-type, mask-GUID
//

int
vscp_link_parseMask(vscpEventFilter* pfilter, const char* strmask)
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
  return vscp_link_parseGuid(pfilter->mask_GUID, p, NULL);
}

///////////////////////////////////////////////////////////////////////////////
// parseEvent
//
// head,class,type,obid,datetime,timestamp,GUID,data1,data2,data3....
//

int
vscp_link_parseEvent(vscpEvent* pev, const char* buf)
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
  if (VSCP_ERROR_SUCCESS != vscp_link_parseGuid(pev->GUID, p, &p)) {
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
    pev->pdata = VSCP_LINK_MALLOC(pev->sizeData);
    if (NULL == pev->pdata) {
      return VSCP_ERROR_MEMORY;
    }
    memcpy(pev->pdata, wrkbuf, pev->sizeData);
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// parseEventEx
//

int
vscp_link_parseEventEx(vscpEventEx* pex, const char* buf)
{
  char* p = (char*)buf;

  // Check pointers
  if (NULL == buf) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (NULL == pex) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  memset(pex, 0, sizeof(vscpEventEx));

  vscpEvent* pev = vscp_link_newEvent();
  if (NULL == pev) {
    return VSCP_ERROR_MEMORY;
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_parseEvent(pev, buf)) {
    vscp_link_deleteEvent(&pev);
    return VSCP_ERROR_PARAMETER;
  }

  // Copy in data
  pex->head       = pev->head;
  pex->obid       = pev->obid;
  pex->year       = pev->year;
  pex->month      = pev->month;
  pex->day        = pev->day;
  pex->hour       = pev->hour;
  pex->minute     = pev->minute;
  pex->second     = pev->second;
  pex->timestamp  = pev->timestamp;
  pex->vscp_class = pev->vscp_class;
  pex->vscp_type  = pev->vscp_type;
  memcpy(pex->GUID, pev->GUID, 16);
  pex->sizeData = pev->sizeData;
  if (pex->sizeData) {
    memcpy(pex->data, pev->pdata, pev->sizeData);
  }

  vscp_link_deleteEvent(&pev);

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// eventToString
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
vscp_link_eventToString(char* buf, size_t size, const vscpEvent* pev)
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
  vscp_link_writeGuidToString(wrkbuf, pev->GUID);
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
// doLevel2Filter
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
vscp_link_doLevel2Filter(const vscpEvent *pEvent, const vscpEventFilter *pFilter)
{
  // A nullptr filter is wildcard
  if (NULL == pFilter) {
    return 1;
  }

  // Must be a valid message
  if (NULL == pEvent) {
    return 0;
  }

  // Test vscp_class
  if (0xffff != (uint16_t) (~(pFilter->filter_class ^ pEvent->vscp_class) | ~pFilter->mask_class)) {
    return 0;
  }

  // Test vscp_type
  if (0xffff != (uint16_t) (~(pFilter->filter_type ^ pEvent->vscp_type) | ~pFilter->mask_type)) {
    return 0;
  }

  // GUID
  for (int i = 0; i < 16; i++) {
    if (0xff != (uint8_t) (~(pFilter->filter_GUID[i] ^ pEvent->GUID[i]) | ~pFilter->mask_GUID[i]))
      return 0;
  }

  // Test priority
  if (0xff != (uint8_t) (~(pFilter->filter_priority ^ vscp_link_getEventPriority(pEvent)) | ~pFilter->mask_priority)) {
    return 0;
  }

  return 1;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_connect
//

int
vscp_link_connect(const void* pdata)
{
  return vscp_link_callback_welcome(pdata);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_disconnect
//

int
vscp_link_disconnect(const void* pdata)
{
  // We currently do nothing here
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_idle_worker
//

int 
vscp_link_idle_worker(const void* pdata)
{
  // Get receive loop status
  int bRcvLoop = vscp_link_callback_get_rcvloop_status(pdata);

  /*
    If we are in the rcvloop we do a callback to the application
    so it can send events or '+OK\r\n" each second.
    Note that we still handle all commands.
  */
  if (bRcvLoop) {

    int rv;
    char buf[VSCP_LINK_MAX_BUF];
    vscpEvent *pev;
    pev->pdata = NULL;

    if (rv = vscp_link_callback_rcvloop(pdata, pev)) {
      if (VSCP_ERROR_SUCCESS == rv) {
        // We own the event from now on and must
        // delete it and it's data when we are done
        // with it
        int rv = vscp_link_eventToString(buf, sizeof(buf), pev);
        vscp_link_deleteEvent(&pev);
        if (VSCP_ERROR_SUCCESS == rv) {
          // Write out the event
          vscp_link_callback_write_client(pdata, buf);      
        }
      }
      else if (VSCP_ERROR_TIMEOUT == rv) {
        // Timeout - Write out '+OK\r\n'
        vscp_link_callback_write_client(pdata, "+OK\r\n");
      }

    }

    // No error on failure

  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_parser
//

int
vscp_link_parser(const void* pdata, const char* cmd)
{
  const char* p = cmd;
  const char* pcmd;
  static lastcmd[128] = {0};

  // Check pointers
  if ((NULL == pdata) || (NULL == cmd)) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  // Get receive loop status
  int bRcvLoop = vscp_link_callback_get_rcvloop_status(pdata);
  
  // Remove whitespace from command
  while (*p && (*p == ' ')) {
    p++;
  }

  // Check for repeat command
  if ('+' == *p) {
    pcmd = lastcmd;
    // print out the repeat command for convinience to user
    vscp_link_callback_writeClient(pdata, lastcmd);
    vscp_link_callback_writeClient(pdata, "\r\n");
  }
  else {
    // Save command
    memset(lastcmd, 0, sizeof(lastcmd));
    strncpy(lastcmd, p, sizeof(lastcmd) - 1);
    pcmd = p;
  }

  /*
  * ENABLE_RCVLOOP_CMD   bRcvLoop   =   ENABLE_RCVLOOP_CMD | !bRcvLoop
  *        0                 0      =   0 | 1 = 1
  *        0                 1      =   0 | 0 = 0
  *        1                 0      =   1 | 1 = 1 
  *        1                 1      =   1 | 0 = 1
  */

  if (NULL != (p = vscp_link_stristr(pcmd, "help")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdHelp(pdata, p); 
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "noop")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdNoop(pdata, p);
  }  
  else if (NULL != (p = vscp_link_stristr(pcmd, "user")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdUser(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "pass")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdPassword(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "challenge")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 9;
    return vscp_link_doCmdChallenge(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "send")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdSend(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "retr")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdRetrieve(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "rcvloop")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdRcvLoop(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "quitloop"))) {
    p += 8;
    return vscp_link_doCmdQuitLoop(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "quit"))) {
    p += 4;
    return vscp_link_doCmdQuit(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "cdta")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdCheckData(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "chkdata")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdCheckData(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "clra")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdClearAll(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "clrall")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 6;
    return vscp_link_doCmdClearAll(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "stat")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdStatistics(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "info")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdInfo(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "chid")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdGetChannelId(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "getchid")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdGetChannelId(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "sgid")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdSetGUID(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "setguid")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdSetGUID(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "ggid")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdGetGUID(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "getguid")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdGetGUID(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "vers")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdGetVersion(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "version")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdGetVersion(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "sflt")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdSetFilter(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "setfilter")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 9;
    return vscp_link_doCmdSetFilter(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "smsk")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdSetMask(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "setmask")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdSetMask(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "test")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdTest(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "wcyd")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdWhatCanYouDo(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "whatcanyoudo")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 12;
    return vscp_link_doCmdWhatCanYouDo(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "+")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 1;
    return vscp_link_doCmdCommandAgain(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "interface")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 9;
    return vscp_link_doCmdInterface(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "shutdown")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 8;
    return vscp_link_doCmdShutdown(pdata, p);
  }
  else if (NULL != (p = vscp_link_stristr(pcmd, "restart")) && (ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdRestart(pdata, p);
  }

  // Unknown command
  vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_UNKNOWN_COMMAND);
  return VSCP_ERROR_MISSING;
}



///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdNoop
//

int
vscp_link_doCmdNoop(const void* pdata, const char* cmd)
{
  vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdHelp
//

int
vscp_link_doCmdHelp(const void* pdata, const char* cmd)
{
#ifdef VSCP_LINK_CUSTOM_HELP_TEXT 
  return vscp_link_callback_help(pdata, cmd);
#else
  if (NULL != vscp_link_stristr(cmd, "noop")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_NOOP);
  }
  else if (NULL != vscp_link_stristr(cmd, "help")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_HELP);
  }
  else if (NULL != vscp_link_stristr(cmd, "quit")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_QUIT);
  }
  else if (NULL != vscp_link_stristr(cmd, "user")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_USER);
  }
  else if (NULL != vscp_link_stristr(cmd, "pass")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_PASS);
  }
  else if (NULL != vscp_link_stristr(cmd, "challenge")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_CHALLENGE);
  }
  else if (NULL != vscp_link_stristr(cmd, "send")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_SEND);
  }
  else if (NULL != vscp_link_stristr(cmd, "retr")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_RETR);
  }
  else if (NULL != vscp_link_stristr(cmd, "rcvloop")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_RCVLOOP);
  }
  else if (NULL != strstr(cmd, "quitloop")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_QUITLOOP);
  }
  else if (NULL != vscp_link_stristr(cmd, "cdta") || NULL != strstr(cmd, "chkdata")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_CDTA);
  }
  else if (NULL != vscp_link_stristr(cmd, "clra") || NULL != strstr(cmd, "clrall")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_CLRA);
  }
  else if (NULL != vscp_link_stristr(cmd, "stat")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_STAT);
  }
  else if (NULL != vscp_link_stristr(cmd, "info")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_INFO);
  }  
  else if (NULL != vscp_link_stristr(cmd, "chid")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_CHID);
  }
  else if (NULL != vscp_link_stristr(cmd, "sgid") || NULL != strstr(cmd, "setguid")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_SGID);
  }
  else if (NULL != vscp_link_stristr(cmd, "ggid") || NULL != strstr(cmd, "getguid")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_GGID);
  }
  else if (NULL != vscp_link_stristr(cmd, "vers") || NULL != strstr(cmd, "version")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_VERS);
  }
  else if (NULL != vscp_link_stristr(cmd, "sflt") || NULL != strstr(cmd, "setfilter")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_SFLT);
  }
  else if (NULL != vscp_link_stristr(cmd, "smsk") || NULL != strstr(cmd, "setmask")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_SMSK);
  }
  else if (NULL != vscp_link_stristr(cmd, "test")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_TEST);
  }
  else if (NULL != vscp_link_stristr(cmd, "wcyd") || NULL != strstr(cmd, "whatcanyoudo")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_WCYD);
  }
  else {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP);
  }
#endif
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdQuit
//

int
vscp_link_doCmdQuit(const void* pdata, const char* cmd)
{
  // VSCP_LINK_MSG_GOODBY
  return vscp_link_callback_quit(pdata);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdUser
//

int
vscp_link_doCmdUser(const void* pdata, const char* cmd)
{
  return vscp_link_callback_check_user(pdata, cmd);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdPassword
//

int
vscp_link_doCmdPassword(const void* pdata, const char* cmd)
{
  return vscp_link_callback_check_password(pdata, cmd);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdChallenge
//

int
vscp_link_doCmdChallenge(const void* pdata, const char* cmd)
{
  return vscp_link_callback_challenge(pdata, cmd);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdSend
//

int
vscp_link_doCmdSend(const void* pdata, const char* cmd)
{
  int rv;
  vscpEvent *pev;

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 4)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  pev = vscp_link_newEvent();
  if (VSCP_ERROR_SUCCESS != (rv = vscp_link_parseEvent(pev, cmd))) {
    if (VSCP_ERROR_INVALID_POINTER == rv) {
      vscp_link_deleteEvent(&pev);
      return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
    }
    else if (VSCP_ERROR_MEMORY == rv) {
      vscp_link_deleteEvent(&pev);
      return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
    }
    else {
      vscp_link_deleteEvent(&pev);
      return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_PARAMETER_ERROR);
    }
  }

  rv = vscp_link_callback_send(pdata, pev);
  if (VSCP_ERROR_SUCCESS == rv) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
  }
  else if (VSCP_ERROR_INVALID_HANDLE == rv) {
    vscp_link_deleteEvent(&pev);
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  else if (VSCP_ERROR_INVALID_PERMISSION == rv) {
    vscp_link_deleteEvent(&pev);
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  else if (VSCP_ERROR_TRM_FULL == rv) {
    vscp_link_deleteEvent(&pev);
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_BUFFER_FULL);
  }
  else {
    vscp_link_deleteEvent(&pev);
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdRetrive
//

int
vscp_link_doCmdRetrieve(const void* pdata, const char* cmd)
{
  int rv;
  char buf[VSCP_LINK_MAX_BUF];
  vscpEvent *pev = vscp_link_newEvent();

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 2)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  int cnt = vscp_link_readStringValue(cmd);

  while (VSCP_ERROR_SUCCESS == (rv = vscp_link_callback_retr(pdata, &pev))) {
    
    // We own the event from now on and must
    // delete it and it's data when we are done
    // with it
    rv = vscp_link_eventToString(buf, sizeof(buf), pev);
    vscp_link_deleteEvent(&pev);
    if (VSCP_ERROR_SUCCESS == rv) {
      strcat(buf,"\r\n");
      vscp_link_callback_write_client(pdata, buf);
    }
    else {
      return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
    }

    cnt--;
    if (cnt <= 0) {
      rv = VSCP_ERROR_SUCCESS;
      break;
    }
  }

  if (VSCP_ERROR_SUCCESS == rv) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
  }
  else if (VSCP_ERROR_RCV_EMPTY == rv) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NO_MSG);
  }
  else {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdRcvLoop
//

int
vscp_link_doCmdRcvLoop(const void* pdata, const char* cmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 2)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  if (VSCP_ERROR_SUCCESS == vscp_link_callback_enable_rcvloop(pdata, 1)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
  }
  else {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdQuitLoop
//

int
vscp_link_doCmdQuitLoop(const void* pdata, const char* cmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 2)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  if (VSCP_ERROR_SUCCESS == vscp_link_callback_enable_rcvloop(pdata, 0)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
  }
  else {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdCmdCheckData
//

int
vscp_link_doCmdCheckData(const void* pdata, const char* cmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 1)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  uint16_t cnt;
  if (VSCP_ERROR_SUCCESS == vscp_link_callback_chkData(pdata, &cnt)) {
    char buf[10];
    sprintf(buf, "%u\r\n", cnt);
    vscp_link_callback_write_client(pdata, buf);
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
  }
  else {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }
}


///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdClearAll
//

int
vscp_link_doCmdClearAll(const void* pdata, const char* cmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 2)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  if (VSCP_ERROR_SUCCESS == vscp_link_callback_clrAll(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
  }
  else {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdStatistics
//

int
vscp_link_doCmdStatistics(const void* pdata, const char* cmd)
{
  char buf[1000];
  VSCPStatistics statistics;

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 2)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_statistics(pdata, &statistics)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  snprintf(buf,
           sizeof(buf),
           "%lu,%lu,%lu,%lu,%lu,%lu,%lu\r\n+OK\r\n",
           statistics.x,
           statistics.y,
           statistics.cntOverruns,
           statistics.cntReceiveData,
           statistics.cntReceiveFrames,
           statistics.cntTransmitData,
           statistics.cntTransmitFrames);

  return vscp_link_callback_write_client(pdata, buf);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdInfo
//

int
vscp_link_doCmdInfo(const void* pdata, const char* cmd)
{
  char buf[1000];
  VSCPStatus status;

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 2)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_info(pdata, &status)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  snprintf(buf,
           sizeof(buf),
           "%lu,%lu,%lu,\"%s\"\r\nOK\r\n",
           status.channel_status,
           status.lasterrorcode,
           status.lasterrorsubcode,
           status.lasterrorstr);

  return vscp_link_callback_write_client(pdata, buf);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdGetChannelId
//

int
vscp_link_doCmdGetChannelId(const void* pdata, const char* cmd)
{
  uint16_t chid;
  char buf[10];

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 2)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_get_channel_id(pdata, &chid)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);    
  }
  
  sprintf(buf, "%u\r\n", chid);
  vscp_link_callback_write_client(pdata, buf);

  return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdSetGUID
//

int
vscp_link_doCmdSetGUID(const void* pdata, const char* cmd)
{
  char *endptr;
  uint8_t guid[16];

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 6)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_parseGuid(guid, cmd, &endptr)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }

  if (VSCP_ERROR_SUCCESS == vscp_link_callback_set_guid(pdata, guid)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
  }
  else {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdGetGUID
//

int
vscp_link_doCmdGetGUID(const void* pdata, const char* cmd)
{
  uint8_t guid[16];
  char strguid[50];

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 1)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_get_guid(pdata, guid)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_writeGuidToString(strguid, guid)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }

  strcat(strguid, "\r\n");
  vscp_link_callback_write_client(pdata, strguid);

  return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdGetVersion
//

int
vscp_link_doCmdGetVersion(const void* pdata, const char* cmd)
{
  uint8_t version[4];
  char strversion[50];

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_get_version(pdata, version)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }

  sprintf(strversion, "%d,%d,%d,%d\r\n", version[0], version[1], version[2], version[3]);
  vscp_link_callback_write_client(pdata, strversion);

  return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdSetFilter
//

int
vscp_link_doCmdSetFilter(const void* pdata, const char* cmd)
{
  vscpEventFilter filter;

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 6)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_parseFilter(&filter, cmd)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_setFilter(pdata, &filter)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }

  return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdSetMask
//

int
vscp_link_doCmdSetMask(const void* pdata, const char* cmd)
{
  vscpEventFilter filter;

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 6)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_parseMask(&filter, cmd)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_setMask(pdata, &filter)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }

  return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdTest
//

int
vscp_link_doCmdTest(const void* pdata, const char* cmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 6)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  return vscp_link_callback_test(pdata, cmd);
}

///////////////////////////////////////////////////////////////////////////////
// doCmdInterface
//

int
vscp_link_doCmdInterface(const void* pdata, const char* cmd)
{
  char buf[120], wbuf[50];
  struct vscp_interface_info ifinfo;

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 2)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  for (uint16_t i=0; i<vscp_link_callback_get_interface_count(pdata); i++) {
    
    memset(buf, 0, sizeof(buf));
    if (VSCP_ERROR_SUCCESS != vscp_link_callback_get_interface(pdata, i, &ifinfo)) {
      return VSCP_ERROR_ERROR;
    }

    if (VSCP_ERROR_SUCCESS != vscp_link_writeGuidToString(wbuf, ifinfo.guid)) {
      return VSCP_ERROR_ERROR;
    }

    snprintf(buf, sizeof(buf), "%u,%u,%s,%s\r\n", ifinfo.idx, ifinfo.type, wbuf, ifinfo.description);
    vscp_link_callback_write_client(pdata, buf);
  }

  return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdWhatCanYouDo
//

int
vscp_link_doCmdWhatCanYouDo(const void* pdata, const char* cmd)
{
  uint64_t wcyd;
  char buf[20];

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_wcyd(pdata, &wcyd)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }

  memset(buf, '\0', sizeof(buf));
  sprintf(buf, "%llu\r\n", wcyd);
  vscp_link_callback_write_client(pdata, buf);

  return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdCommandAgain
//

int
vscp_link_doCmdCommandAgain(const void* pdata, const char* cmd)
{
  return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK); 
}


///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdCommandAgain
//

int
vscp_link_doCmdShutdown(const void* pdata, const char* cmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_callback_shutdown(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }

  return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
}


///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdRestart
//

int
vscp_link_doCmdRestart(const void* pdata, const char* cmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_callback_restart(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }

  return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
}

