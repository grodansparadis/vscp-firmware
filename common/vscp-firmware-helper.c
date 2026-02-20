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
 * Copyright (c) 2000-2026 Ake Hedman,
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

#include <vscp-class.h>
#include <vscp-type.h>
#include <vscp.h>
#include "vscp-firmware-helper.h"

#ifdef VSCP_FWHLP_UDP_FRAME_SUPPORT
#include <crc.h>
#endif

#ifdef VSCP_FWHLP_CRYPTO_SUPPORT
#include <vscp-aes.h>
#endif

#ifdef VSCP_FWHLP_JSON_SUPPORT
#include <cJSON.h>
#endif

#define UNUSED(expr)                                                                                                   \
  do {                                                                                                                 \
    (void) (expr);                                                                                                     \
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
  return (*(char *) &x);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_isBigEndian
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
vscp_fwhlp_a2ul(const char *src, uint8_t srclen, uint8_t base, uint32_t *resultp, char **endptr)
{
  const char *stop;
  static char hex[]   = "0123456789abcdef";
  static char uchex[] = "0123456789ABCDEF";
  int d;
  char c;
  char *p;
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
      return vscp_fwhlp_a2ul(src + 2, srclen - 2, 16, resultp, endptr);
    }

    if (srclen > 1 && *src == '0' && base != 13) {
      return vscp_fwhlp_a2ul(src + 1, srclen - 1, 8, resultp, endptr);
    }

    return vscp_fwhlp_a2ul(src, srclen, 10, resultp, endptr);
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
    dlimit = (int) (ULONG_MAX - rlimit * base);

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
  if (endptr) {
    *endptr = (char *) src;
  }

  return VSCP_ERROR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_dec2hex
//

void
vscp_fwhlp_dec2hex(uint8_t dec, char *pBuf, uint16_t len)
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
vscp_fwhlp_hex2dec(const char *pHex)
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

char *
vscp_fwhlp_strsubst(char *pNewStr, size_t len, const char *pStr, const char *pTarget, const char *pReplace)
{
  char *p     = (char *) pStr;
  char *pLast = (char *) pStr;

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
vscp_fwhlp_readStringValue(const char *pString)
{
  uint32_t ul;
  char *p;
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
    vscp_fwhlp_a2ul(buf, 0, 16, &ul, NULL);
    return ul;
  }
  if (NULL != (p = strstr(buf, "0"))) {
    p += 2; // Point at the data
    vscp_fwhlp_a2ul(buf, 0, 8, &ul, NULL);
    return ul;
  }
  if (NULL != (p = strstr(buf, "0b"))) {
    p += 2; // Point at the data
    vscp_fwhlp_a2ul(buf, 0, 2, &ul, NULL);
    return ul;
  }
  else {
    ul = (unsigned) atoi(buf);
    return ul;
  }

  return ul;
}

////////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_parseStringValue
//

uint32_t
vscp_fwhlp_parseStringValue(const char *pString, char **endptr)
{
  uint32_t ul = 0;
  char *p     = (char *) pString;
  char *pend  = NULL;

  if (NULL == pString) {
    if (endptr) {
      *endptr = NULL;
    }
    return 0;
  }

  while (*p && isspace((unsigned char) *p)) {
    p++;
  }

  if (('0' == p[0]) && ('b' == tolower((unsigned char) p[1]))) {
    char *pb = p + 2;
    ul       = 0;

    if ((*pb != '0') && (*pb != '1')) {
      pend = p;
    }
    else {
      while ((*pb == '0') || (*pb == '1')) {
        ul = (ul << 1) | (uint32_t) (*pb - '0');
        pb++;
      }
      pend = pb;
    }
  }
  else {
    unsigned long val = strtoul(p, &pend, 0);
    ul                = (uint32_t) val;
  }

  if (endptr) {
    *endptr = (NULL != pend) ? pend : p;
  }

  return ul;
}

////////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_stristr
//

char *
vscp_fwhlp_stristr(const char *haystack, const char *needle)
{
  int c = tolower((unsigned char) *needle);
  if (c == '\0')
    return (char *) haystack;
  for (; *haystack; haystack++) {
    if (tolower((unsigned char) *haystack) == c) {
      for (size_t i = 0;;) {
        if (needle[++i] == '\0')
          return (char *) haystack;
        if (tolower((unsigned char) haystack[i]) != tolower((unsigned char) needle[i]))
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
    return (unsigned char) (c - '0');
  }
  if ('A' <= c && c <= 'F') {
    return (unsigned char) (c - 'A' + 10);
  }
  if ('a' <= c && c <= 'f') {
    return (unsigned char) (c - 'a' + 10);
  }
  return 0xFF;
}

////////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_hex2bin
//

int
vscp_fwhlp_hex2bin(unsigned char *buf, size_t length, const char *s)
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
vscp_fwhlp_bin2hex(char *output, size_t outLength, const unsigned char *buf, size_t length)
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

//////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_parse_data
//

int
vscp_fwhlp_parse_data(uint8_t *data, size_t length, const char *datastr, const char **endptr)
{
  uint8_t *p      = data;
  const char *end = datastr + strlen(datastr);

  if (!datastr || !data || length <= 0) {
    return -1;
  }

  const char *s = datastr;
  while (*s && s < end) {
    while (*s == ' ' || *s == ',') {
      s++;
    }
    if (*s == '\0' || s >= end) {
      break;
    }

    char *next;
    unsigned long value = vscp_fwhlp_parseStringValue(s, &next);
    if (next == s) {
      return -1; // No conversion performed
    }
    if (value > 255) {
      return -1; // Value out of byte range
    }
    if (p - data >= length) {
      return -1; // Buffer overflow
    }

    *p++ = (uint8_t) value;
    s    = next;
  }

  if (endptr) {
    *endptr = s;
  }

  return p - data; // Return number of bytes read
}

//////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_make_string_from_data
//

char *
vscp_fwhlp_make_string_from_data(char *buf, size_t len, const uint8_t *data, size_t data_len)
{
  if (!buf || len <= 0 || !data || data_len <= 0) {
    return NULL;
  }

  size_t required_len = data_len * 5; // Max "255," per byte
  if (len < required_len) {
    return NULL; // Buffer too small
  }

  char *p = buf;
  for (size_t i = 0; i < data_len; ++i) {
    int written = snprintf(p, len, "%d", data[i]);
    if (written < 0 || (size_t) written >= len) {
      return NULL; // Encoding error or buffer overflow
    }
    p += written;
    len -= written;

    if (i < data_len - 1) {
      if (len < 2) {
        return NULL; // Buffer too small for comma and null terminator
      }
      *p++ = ',';
      *p   = '\0';
      len--;
    }
  }

  return buf;
}

//////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_get_datestr_from_event
//

char *
vscp_fwhlp_get_datestr_from_event(char *buf, size_t len, const vscpEvent *pev)
{
  if ((NULL == pev) || (NULL == buf) || (len < 21)) {
    return NULL;
  }

  snprintf(buf,
           len,
           "%04u-%02u-%02uT%02u:%02u:%02uZ",
           pev->year,
           pev->month,
           pev->day,
           pev->hour,
           pev->minute,
           pev->second);
  return buf;
}

//////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_get_datestr_from_eventex
//

char *
vscp_fwhlp_get_datestr_from_eventex(char *buf, size_t len, const vscpEventEx *pex)
{
  if ((NULL == pex) || (NULL == buf) || (len < 21)) {
    return NULL;
  }

  snprintf(buf,
           len,
           "%04u-%02u-%02uT%02u:%02u:%02uZ",
           pex->year,
           pex->month,
           pex->day,
           pex->hour,
           pex->minute,
           pex->second);
  return buf;
}

//////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_parse_event_datestr
//
// YYYY-MM-DDTHH:MM:SSZ
//

int
vscp_fwhlp_parse_event_datestr(vscpEvent *pev, const char *strdate, char **endptr)
{
  if ((NULL == pev) || (NULL == strdate)) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  char *p = (char *) strdate;

  // year
  pev->year = (uint16_t) strtol(p, &p, 10);
  if ('-' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond dash
  if (!(*p)) {
    return VSCP_ERROR_INVALID_FORMAT;
  }

  // month
  pev->month = (uint8_t) strtol(p, &p, 10);
  if ('-' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond dash
  if (!(*p)) {
    return VSCP_ERROR_INVALID_FORMAT;
  }

  // day
  pev->day = (uint8_t) strtol(p, &p, 10);
  // We accept both 'T' and space as separator between date and time
  // even if not fully ISO 8601 compliant
  if ('T' != *p && ' ' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond "T"
  if (!(*p)) {
    return VSCP_ERROR_INVALID_FORMAT;
  }

  // hour
  pev->hour = (uint8_t) strtol(p, &p, 10);
  if (':' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond colon
  if (!(*p)) {
    return VSCP_ERROR_INVALID_FORMAT;
  }

  // minute
  pev->minute = (uint8_t) strtol(p, &p, 10);
  if (':' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond colon
  if (!(*p)) {
    return VSCP_ERROR_INVALID_FORMAT;
  }

  // second
  pev->second = (uint8_t) strtol(p, &p, 10);
  p++; // point beyond second

  // If 'Z' is not preset we still accept
  if ('Z' != *p) {
    p++; // point beyond "Z" (for endptr
  }

  if (endptr) {
    *endptr = p;
  }

  return VSCP_ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_parse_eventex_datestr
//
// YYYY-MM-DDTHH:MM:SS
//

int
vscp_fwhlp_parse_eventex_datestr(vscpEventEx *pex, const char *strdate, char **endptr)
{
  if ((NULL == pex) || (NULL == strdate)) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  char *p = (char *) strdate;

  // year
  pex->year = (uint16_t) strtol(p, &p, 10);
  if ('-' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond dash
  if (!(*p)) {
    return VSCP_ERROR_INVALID_FORMAT;
  }

  // month
  pex->month = (uint16_t) strtol(p, &p, 10);
  if ('-' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond dash
  if (!(*p)) {
    return VSCP_ERROR_INVALID_FORMAT;
  }

  // day
  pex->day = (uint16_t) strtol(p, &p, 10);
  // We accept both 'T' and space as separator between date and time
  // even if not fully ISO 8601 compliant
  if ('T' != *p && ' ' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond "T"
  if (!(*p)) {
    return VSCP_ERROR_INVALID_FORMAT;
  }

  // hour
  pex->hour = (uint16_t) strtol(p, &p, 10);
  if (':' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond colon
  if (!(*p)) {
    return VSCP_ERROR_INVALID_FORMAT;
  }

  // minute
  pex->minute = (uint16_t) strtol(p, &p, 10);
  if (':' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond colon
  if (!(*p)) {
    return VSCP_ERROR_INVALID_FORMAT;
  }

  // second
  pex->second = (uint16_t) strtol(p, &p, 10);
  p++; // point beyond second

  // If 'Z' is not preset we still accept
  if ('Z' != *p) {
    p++; // point beyond "Z" (for endptr
  }

  if (endptr) {
    *endptr = p;
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_to_unix_ns
//

#define NS_PER_SEC   1000000000LL
#define SECS_PER_DAY 86400LL

// Convert civil date to days since 1970-01-01 (Unix epoch)
static int64_t
days_from_civil(int year, int month, int day)
{
  year -= (month <= 2);
  int era      = (year >= 0 ? year : year - 399) / 400;
  unsigned yoe = (unsigned) (year - era * 400);                            // [0, 399]
  unsigned doy = (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1; // [0, 365]
  unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;                    // [0,146096]
  return era * 146097LL + (int64_t) doe - 719468LL;
}

int64_t
vscp_fwhlp_to_unix_ns(int year, int month, int day, int hour, int minute, int second, uint32_t microsecond)
{
  // Convert date to days since epoch
  int64_t days = days_from_civil(year, month, day);

  // Convert everything to seconds
  int64_t total_seconds = days * SECS_PER_DAY + hour * 3600LL + minute * 60LL + second;

  // Convert to nanoseconds
  return total_seconds * NS_PER_SEC + (int64_t) microsecond * 1000LL;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_from_unix_ns
//

static void
civil_from_days(int64_t z, int *y, int *m, int *d)
{
  // z = days since 1970-01-01
  z += 719468; // shift to civil 0000-03-01 base

  int64_t era     = (z >= 0 ? z : z - 146096) / 146097;
  unsigned doe    = (unsigned) (z - era * 146097); // [0, 146096]
  unsigned yoe    = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  int64_t y_full  = (int64_t) yoe + era * 400;
  unsigned doy    = doe - (365 * yoe + yoe / 4 - yoe / 100);
  unsigned mp     = (5 * doy + 2) / 153;
  unsigned d_full = doy - (153 * mp + 2) / 5 + 1;
  unsigned m_full = mp + (mp < 10 ? 3 : -9);

  *y = (int) (y_full + (m_full <= 2));
  *m = (int) m_full;
  *d = (int) d_full;
}

void
vscp_fwhlp_from_unix_ns(int64_t unix_ns,
                        int *year,
                        int *month,
                        int *day,
                        int *hour,
                        int *minute,
                        int *second,
                        uint32_t *microsecond)
{
  // --- Split seconds and nanoseconds safely ---
  int64_t sec  = unix_ns / NS_PER_SEC;
  int64_t nsec = unix_ns % NS_PER_SEC;

  if (nsec < 0) {
    sec--;
    nsec += NS_PER_SEC;
  }

  // --- Split days and time-of-day ---
  int64_t days = sec / SECS_PER_DAY;
  int64_t rem  = sec % SECS_PER_DAY;

  if (rem < 0) {
    rem += SECS_PER_DAY;
    days--;
  }

  // --- Convert days → Y/M/D ---
  civil_from_days(days, year, month, day);

  // --- Convert remainder → H:M:S ---
  *hour = (int) (rem / 3600);
  rem %= 3600;
  *minute = (int) (rem / 60);
  *second = (int) (rem % 60);

  *microsecond = (uint32_t) (nsec / 1000);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_parseMac
//

int
vscp_fwhlp_parseMac(uint8_t *pmac, const char *strmac, char **endptr)
{
  int i;
  char *p = (char *) strmac;

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
    pmac[i] = (uint8_t) strtol(p, &p, 16);
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
vscp_fwhlp_getEventPriority(const vscpEvent *pev)
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
vscp_fwhlp_getEventPriorityEx(const vscpEventEx *pex)
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

vscpEvent *
vscp_fwhlp_newEvent(void)
{
  static vscpEvent *pev = NULL;
  pev                   = (vscpEvent *) calloc(1, sizeof(vscpEvent));
  return pev;
}

////////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_convertEventToEventEx

int
vscp_fwhlp_convertEventToEventEx(vscpEventEx *pEventEx, const vscpEvent *pEvent)
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
vscp_fwhlp_convertEventExToEvent(vscpEvent *pEvent, const vscpEventEx *pEventEx)
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

vscpEvent *
vscp_fwhlp_mkEventCopy(const vscpEvent *pev)
{
  // Must have event to work on
  if (NULL == pev) {
    return NULL;
  }

  vscpEvent *pnewEvent = vscp_fwhlp_newEvent();
  if (NULL == pnewEvent) {
    return NULL;
  }

  // Copy the event
  memcpy(pnewEvent, pev, sizeof(vscpEvent));

  if (pev->sizeData) {
    pnewEvent->pdata = (uint8_t *) malloc(pev->sizeData);
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

vscpEventEx *
vscp_fwhlp_mkEventExCopy(const vscpEventEx *pex)
{
  // Must have event to work on
  if (NULL == pex) {
    return NULL;
  }

  vscpEventEx *pnewEventEx = (vscpEventEx *) malloc(sizeof(vscpEventEx));
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
vscp_fwhlp_deleteEvent(vscpEvent **pev)
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
vscp_fwhlp_parseGuid(uint8_t *guid, const char *strguid, char **endptr)
{
  int i;
  char *p = (char *) strguid;

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
    guid[i] = (uint8_t) strtol(p, &p, 16);
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
vscp_fwhlp_writeGuidToString(char *strguid, const uint8_t *guid)
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
vscp_fwhlp_parseFilter(vscpEventFilter *pfilter, const char *strfilter)
{
  char *p = (char *) strfilter;

  // Check pointers
  if (NULL == strfilter) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (NULL == pfilter) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  // Priority
  pfilter->filter_priority = (uint8_t) strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_ERROR;
  }

  p++;
  if (p > (strfilter + strlen(strfilter))) {
    return VSCP_ERROR_ERROR;
  }

  // Class filter
  pfilter->filter_class = (uint16_t) strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_ERROR;
  }

  p++;
  if (p > (strfilter + strlen(strfilter))) {
    return VSCP_ERROR_ERROR;
  }

  // Type filter
  pfilter->filter_type = (uint16_t) strtol(p, &p, 0);
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
vscp_fwhlp_parseMask(vscpEventFilter *pfilter, const char *strmask)
{
  char *p = (char *) strmask;

  // Check pointers
  if (NULL == strmask) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (NULL == pfilter) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  // Priority
  pfilter->mask_priority = (uint8_t) strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_ERROR;
  }
  p++;
  if (p > (strmask + strlen(strmask))) {
    return VSCP_ERROR_ERROR;
  }

  // Class mask
  pfilter->mask_class = (uint16_t) strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_ERROR;
  }
  p++;
  if (p > (strmask + strlen(strmask))) {
    return VSCP_ERROR_ERROR;
  }

  // Type mask
  pfilter->mask_type = (uint16_t) strtol(p, &p, 0);
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
vscp_fwhlp_parseEvent(vscpEvent *pev, const char *buf)
{
  uint8_t wrkbuf[512];
  char *p = (char *) buf;

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
  pev->head = (uint16_t) strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond comma
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_PARAMETER;
  }

  // VSCP class
  pev->vscp_class = (uint16_t) strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond comma
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_PARAMETER;
  }

  // VSCP type
  pev->vscp_type = (uint16_t) strtol(p, &p, 0);
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
    pev->obid = (uint32_t) strtol(p, &p, 0);
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
    pev->year = (uint16_t) strtol(p, &p, 0);
    if ('-' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond dash
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // month
    pev->month = (uint16_t) strtol(p, &p, 0);
    if ('-' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond dash
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // day
    pev->day = (uint16_t) strtol(p, &p, 0);
    if ('T' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond dash
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // hour
    pev->hour = (uint16_t) strtol(p, &p, 0);
    if (':' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond colon
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // minute
    pev->minute = (uint16_t) strtol(p, &p, 0);
    if (':' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond colon
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // second
    pev->second = (uint16_t) strtol(p, &p, 0);
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
    pev->timestamp = (uint32_t) strtol(p, &p, 0);
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
    wrkbuf[pev->sizeData] = (uint8_t) strtol(p, &p, 0);
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
vscp_fwhlp_parseEventEx(vscpEventEx *pex, const char *buf)
{
  uint8_t wrkbuf[512];
  char *p = (char *) buf;

  // Check pointers
  if (NULL == buf) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (NULL == pex) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  memset(pex, 0, sizeof(vscpEventEx));

  // head
  pex->head = (uint16_t) strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond comma
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_PARAMETER;
  }

  // VSCP class
  pex->vscp_class = (uint16_t) strtol(p, &p, 0);
  if (',' != *p) {
    return VSCP_ERROR_PARAMETER;
  }
  p++; // point beyond comma
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_PARAMETER;
  }

  // VSCP type
  pex->vscp_type = (uint16_t) strtol(p, &p, 0);
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
    pex->obid = (uint32_t) strtol(p, &p, 0);
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
    pex->year = (uint16_t) strtol(p, &p, 0);
    if ('-' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond dash
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // month
    pex->month = (uint16_t) strtol(p, &p, 0);
    if ('-' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond dash
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // day
    pex->day = (uint16_t) strtol(p, &p, 0);
    if ('T' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond dash
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // hour
    pex->hour = (uint16_t) strtol(p, &p, 0);
    if (':' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond colon
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // minute
    pex->minute = (uint16_t) strtol(p, &p, 0);
    if (':' != *p) {
      return VSCP_ERROR_PARAMETER;
    }
    p++; // point beyond colon
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_PARAMETER;
    }

    // second
    pex->second = (uint16_t) strtol(p, &p, 0);
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
    pex->timestamp = (uint32_t) strtol(p, &p, 0);
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
    wrkbuf[pex->sizeData] = (uint8_t) strtol(p, &p, 0);
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
vscp_fwhlp_eventToString(char *buf, size_t size, const vscpEvent *pev)
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
  if (size < ((size_t) ((size_t) ((size_t) ((pev->sizeData * 5) + 110 + 1))))) {
    return VSCP_ERROR_BUFFER_TO_SMALL;
  }

  if (pev->year || pev->month || pev->day || pev->hour || pev->minute || pev->second) {
    sprintf(buf,
            "%u,%u,%u,%lu,%4d-%02d-%02dT%02d:%02d:%02dZ,%lu,",
            (unsigned) pev->head,
            (unsigned) pev->vscp_class,
            (unsigned) pev->vscp_type,
            (unsigned long) pev->obid,
            (int) pev->year,
            (int) pev->month,
            (int) pev->day,
            (int) pev->hour,
            (int) pev->minute,
            (int) pev->second,
            (unsigned long) pev->timestamp);
  }
  else {
    sprintf(buf,
            "%d,%u,%u,%lu,,%lu,",
            (unsigned) pev->head,
            (unsigned) pev->vscp_class,
            (unsigned) pev->vscp_type,
            (unsigned long) pev->obid,
            (unsigned long) pev->timestamp);
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
        sprintf(wrkbuf, "%d,", (int) pev->pdata[i]);
      }
      else {
        sprintf(wrkbuf, "%d", (int) pev->pdata[i]);
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
vscp_fwhlp_eventToStringEx(char *buf, size_t size, const vscpEventEx *pex)
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
  if (size < ((size_t) ((pex->sizeData * 5) + 110 + 1))) {
    return VSCP_ERROR_BUFFER_TO_SMALL;
  }

  if (pex->year || pex->month || pex->day || pex->hour || pex->minute || pex->second) {
    sprintf(buf,
            "%u,%u,%u,%lu,%4d-%02d-%02dT%02d:%02d:%02dZ,%lu,",
            (unsigned) pex->head,
            (unsigned) pex->vscp_class,
            (unsigned) pex->vscp_type,
            (unsigned long) pex->obid,
            (int) pex->year,
            (int) pex->month,
            (int) pex->day,
            (int) pex->hour,
            (int) pex->minute,
            (int) pex->second,
            (unsigned long) pex->timestamp);
  }
  else {
    sprintf(buf,
            "%d,%u,%u,%lu,,%lu,",
            (unsigned) pex->head,
            (unsigned) pex->vscp_class,
            (unsigned) pex->vscp_type,
            (unsigned long) pex->obid,
            (unsigned long) pex->timestamp);
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
        sprintf(wrkbuf, "%d,", (int) pex->data[i]);
      }
      else {
        sprintf(wrkbuf, "%d", (int) pex->data[i]);
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
vscp_fwhlp_doLevel2Filter(const vscpEvent *pev, const vscpEventFilter *pFilter)
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
  if (0xffff != (uint16_t) (~(pFilter->filter_class ^ pev->vscp_class) | ~pFilter->mask_class)) {
    return 0;
  }

  // Test vscp_type
  if (0xffff != (uint16_t) (~(pFilter->filter_type ^ pev->vscp_type) | ~pFilter->mask_type)) {
    return 0;
  }

  // GUID
  for (int i = 0; i < 16; i++) {
    if (0xff != (uint8_t) (~(pFilter->filter_GUID[i] ^ pev->GUID[i]) | ~pFilter->mask_GUID[i]))
      return 0;
  }

  // Test priority
  if (0xff != (uint8_t) (~(pFilter->filter_priority ^ vscp_fwhlp_getEventPriority(pev)) | ~pFilter->mask_priority)) {
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
vscp_fwhlp_doLevel2FilterEx(const vscpEventEx *pex, const vscpEventFilter *pFilter)
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
  if (0xffff != (uint16_t) (~(pFilter->filter_class ^ pex->vscp_class) | ~pFilter->mask_class)) {
    return 0;
  }

  // Test vscp_type
  if (0xffff != (uint16_t) (~(pFilter->filter_type ^ pex->vscp_type) | ~pFilter->mask_type)) {
    return 0;
  }

  // GUID
  for (int i = 0; i < 16; i++) {
    if (0xff != (uint8_t) (~(pFilter->filter_GUID[i] ^ pex->GUID[i]) | ~pFilter->mask_GUID[i]))
      return 0;
  }

  // Test priority
  if (0xff != (uint8_t) (~(pFilter->filter_priority ^ vscp_fwhlp_getEventPriorityEx(pex)) | ~pFilter->mask_priority)) {
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
vscp_fwhlp_getFrameSizeFromEvent(vscpEvent *pEvent)
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
vscp_fwhlp_getFrameSizeFromEventEx(vscpEventEx *pEventEx)
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
vscp_fwhlp_writeEventToFrame(uint8_t *frame, size_t len, uint8_t pkttype, const vscpEvent *pEvent)
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
  crc framecrc = crcFast((unsigned char const *) frame + 1, VSCP_MULTICAST_PACKET0_HEADER_LENGTH + pEvent->sizeData);

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
vscp_fwhlp_writeEventExToFrame(uint8_t *frame, size_t len, uint8_t pkttype, const vscpEventEx *pEventEx)
{
  int rv;
  vscpEvent *pEvent;

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
vscp_fwhlp_getEventFromFrame(vscpEvent *pEvent, const uint8_t *buf, size_t len)
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
                         ((uint16_t) buf[VSCP_MULTICAST_PACKET0_POS_VSCP_SIZE_MSB] << 8) +
                         buf[VSCP_MULTICAST_PACKET0_POS_VSCP_SIZE_LSB];

  // The buffer must hold a frame
  if (len < calcFrameSize)
    return VSCP_ERROR_BUFFER_TO_SMALL;

  crc crcFrame = ((uint16_t) buf[calcFrameSize - 2] << 8) + buf[calcFrameSize - 1];

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
    crcnew = crcFast((unsigned char const *) buf + 1, (int) calcFrameSize - 1);
    // CRC is zero if calculated over itself
    if (crcnew) {
      return VSCP_ERROR_INVALID_CHECKSUM;
    }
  }

  pEvent->sizeData =
    ((uint16_t) buf[VSCP_MULTICAST_PACKET0_POS_VSCP_SIZE_MSB] << 8) + buf[VSCP_MULTICAST_PACKET0_POS_VSCP_SIZE_LSB];

  // Allocate data
  if (NULL == (pEvent->pdata = (uint8_t *) malloc(pEvent->sizeData))) {
    return VSCP_ERROR_MEMORY;
  }

  // copy in data
  memcpy(pEvent->pdata, buf + VSCP_MULTICAST_PACKET0_POS_VSCP_DATA, pEvent->sizeData);

  // Head
  pEvent->head = ((uint16_t) buf[VSCP_MULTICAST_PACKET0_POS_HEAD_MSB] << 8) + buf[VSCP_MULTICAST_PACKET0_POS_HEAD_LSB];

  // Copy in GUID
  memcpy(pEvent->GUID, buf + VSCP_MULTICAST_PACKET0_POS_VSCP_GUID, pEvent->sizeData);

  // Set CRC
  pEvent->crc = crcFrame;

  // Set timestamp
  pEvent->timestamp = ((uint32_t) buf[VSCP_MULTICAST_PACKET0_POS_TIMESTAMP] << 24) +
                      ((uint32_t) buf[VSCP_MULTICAST_PACKET0_POS_TIMESTAMP + 1] << 16) +
                      ((uint32_t) buf[VSCP_MULTICAST_PACKET0_POS_TIMESTAMP + 2] << 8) +
                      buf[VSCP_MULTICAST_PACKET0_POS_TIMESTAMP + 3];

  // Date/time
  pEvent->year  = ((uint16_t) buf[VSCP_MULTICAST_PACKET0_POS_YEAR_MSB] << 8) + buf[VSCP_MULTICAST_PACKET0_POS_YEAR_LSB];
  pEvent->month = buf[VSCP_MULTICAST_PACKET0_POS_MONTH];
  pEvent->day   = buf[VSCP_MULTICAST_PACKET0_POS_DAY];
  pEvent->hour  = buf[VSCP_MULTICAST_PACKET0_POS_HOUR];
  pEvent->minute = buf[VSCP_MULTICAST_PACKET0_POS_MINUTE];
  pEvent->second = buf[VSCP_MULTICAST_PACKET0_POS_SECOND];

  // VSCP Class
  pEvent->vscp_class =
    ((uint16_t) buf[VSCP_MULTICAST_PACKET0_POS_VSCP_CLASS_MSB] << 8) + buf[VSCP_MULTICAST_PACKET0_POS_VSCP_CLASS_LSB];

  // VSCP Type
  pEvent->vscp_type =
    ((uint16_t) buf[VSCP_MULTICAST_PACKET0_POS_VSCP_TYPE_MSB] << 8) + buf[VSCP_MULTICAST_PACKET0_POS_VSCP_TYPE_LSB];

  // obid - set to zero so interface fill it in
  pEvent->obid = 0;

  return VSCP_ERROR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getEventExFromFrame
//

int
vscp_fwhlp_getEventExFromFrame(vscpEventEx *pEventEx, const uint8_t *frame, size_t len)
{
  int rv;
  vscpEvent *pEvent;

  pEvent = (vscpEvent *) calloc(1, sizeof(vscpEvent));
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
vscp_fwhlp_encryptFrame(uint8_t *output,
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
                             (uint32_t) padlen,
                             key,
                             (const uint8_t *) generated_iv);
      // Append iv
      memcpy(output + 1 + padlen, generated_iv, 16);
      padlen += 16; // length of iv
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
      padlen += 16; // length of iv
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
      padlen += 16; // length of iv
      break;

    case VSCP_ENCRYPTION_NONE:
      memcpy(output + 1, input + 1, padlen);
      break;

    default:
      // Not a valid encryption algorithm
      return VSCP_ERROR_PARAMETER;
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
vscp_fwhlp_decryptFrame(uint8_t *output,
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

    case VSCP_ENCRYPTION_AES128:
      AES_CBC_decrypt_buffer(AES128,
                             output + 1,
                             input + 1,
                             (uint32_t) real_len - 1,
                             key,
                             (const uint8_t *) appended_iv);
      break;

    default:
      // Not a valid encryption algorithm
      return VSCP_ERROR_PARAMETER;
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
vscp_fwhlp_parse_json(vscpEvent *pev, const char *jsonVscpEventObj)
{
  int rv;
  cJSON *root;

  // Check pointers
  if ((NULL == pev) || (NULL == jsonVscpEventObj)) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  root = cJSON_Parse(jsonVscpEventObj);
  if (NULL == root) {
    return VSCP_ERROR_INVALID_SYNTAX;
  }

  // Set unused path to known value
  memset(pev, 0, sizeof(vscpEvent));

  if (cJSON_GetObjectItem(root, "vscpHead")) {
    pev->head = (uint16_t) cJSON_GetObjectItem(root, "vscpHead")->valueint;
  }

  if (cJSON_GetObjectItem(root, "vscpObId")) {
    pev->obid = (uint32_t) cJSON_GetObjectItem(root, "vscpObId")->valuedouble;
  }

  // "2017-01-13T10:16:02",
  if (cJSON_GetObjectItem(root, "vscpDateTime")) {
    int year, month, day, hour, minute, second;
    const char *str = cJSON_GetObjectItem(root, "vscpDateTime")->valuestring;
    sscanf(str, "%d-%d-%dT%d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    pev->year   = year;
    pev->month  = month;
    pev->day    = day;
    pev->hour   = hour;
    pev->minute = minute;
    pev->second = second;
  }

  if (cJSON_GetObjectItem(root, "vscpTimeStamp")) {
    pev->timestamp = (uint32_t) cJSON_GetObjectItem(root, "vscpTimeStamp")->valuedouble;
  }

  if (cJSON_GetObjectItem(root, "vscpClass")) {
    pev->vscp_class = (uint16_t) cJSON_GetObjectItem(root, "vscpClass")->valueint;
    ;
  }

  if (cJSON_GetObjectItem(root, "vscpType")) {
    pev->vscp_type = (uint16_t) cJSON_GetObjectItem(root, "vscpType")->valueint;
  }

  if (cJSON_GetObjectItem(root, "vscpGuid")) {
    const char *str = cJSON_GetObjectItem(root, "vscpGuid")->valuestring;
    if (VSCP_ERROR_SUCCESS != (rv = vscp_fwhlp_parseGuid(pev->GUID, str, NULL))) {
      return rv;
    }
  }

  if (cJSON_GetObjectItem(root, "vscpData")) {

    cJSON *pdata  = cJSON_GetObjectItem(root, "vscpData");
    pev->sizeData = cJSON_GetArraySize(pdata);
    pev->pdata    = (uint8_t *) malloc(pev->sizeData);
    if (NULL == pev->pdata) {
      cJSON_Delete(root);
      return VSCP_ERROR_MEMORY;
    }

    for (int i = 0; i < pev->sizeData; i++) {
      cJSON *pitem = cJSON_GetArrayItem(pdata, i);
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
vscp_fwhlp_parse_json_ex(vscpEventEx *pex, const char *jsonVscpEventObj)
{
  int rv;
  cJSON *root;

  // Check pointers
  if ((NULL == pex) || (NULL == jsonVscpEventObj)) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  root = cJSON_Parse(jsonVscpEventObj);
  if (NULL == root) {
    return VSCP_ERROR_INVALID_SYNTAX;
  }

  // Set unused path to known value
  memset(pex, 0, sizeof(vscpEventEx));

  if (cJSON_GetObjectItem(root, "vscpHead")) {
    pex->head = (uint16_t) cJSON_GetObjectItem(root, "vscpHead")->valueint;
  }

  if (cJSON_GetObjectItem(root, "vscpObId")) {
    pex->obid = (uint32_t) cJSON_GetObjectItem(root, "vscpObId")->valuedouble;
  }

  // "2017-01-13T10:16:02",
  if (cJSON_GetObjectItem(root, "vscpDateTime")) {
    int year, month, day, hour, minute, second;
    const char *str = cJSON_GetObjectItem(root, "vscpDateTime")->valuestring;
    sscanf(str, "%d-%d-%dT%d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    pex->year   = year;
    pex->month  = month;
    pex->day    = day;
    pex->hour   = hour;
    pex->minute = minute;
    pex->second = second;
  }

  if (cJSON_GetObjectItem(root, "vscpTimeStamp")) {
    pex->timestamp = (uint32_t) cJSON_GetObjectItem(root, "vscpTimeStamp")->valuedouble;
  }

  if (cJSON_GetObjectItem(root, "vscpClass")) {
    pex->vscp_class = (uint16_t) cJSON_GetObjectItem(root, "vscpClass")->valueint;
    ;
  }

  if (cJSON_GetObjectItem(root, "vscpType")) {
    pex->vscp_type = (uint16_t) cJSON_GetObjectItem(root, "vscpType")->valueint;
  }

  if (cJSON_GetObjectItem(root, "vscpGuid")) {
    const char *str = cJSON_GetObjectItem(root, "vscpGuid")->valuestring;
    if (VSCP_ERROR_SUCCESS != (rv = vscp_fwhlp_parseGuid(pex->GUID, str, NULL))) {
      return rv;
    }
  }

  if (cJSON_GetObjectItem(root, "vscpData")) {

    cJSON *pdata  = cJSON_GetObjectItem(root, "vscpData");
    pex->sizeData = cJSON_GetArraySize(pdata);

    for (int i = 0; i < pex->sizeData; i++) {
      cJSON *pitem = cJSON_GetArrayItem(pdata, i);
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
vscp_fwhlp_create_json(char *strObj, size_t len, const vscpEvent *pev)
{
  char str[80];
  cJSON *root;

  // Check pointers
  if ((NULL == strObj) || (NULL == pev)) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  root = cJSON_CreateObject();
  if (NULL == root) {
    return VSCP_ERROR_INVALID_SYNTAX;
  }

  cJSON_AddNumberToObject(root, "vscpHead", pev->head);
  cJSON_AddNumberToObject(root, "vscpClass", pev->vscp_class);
  cJSON_AddNumberToObject(root, "vscpType", pev->vscp_type);
  cJSON_AddNumberToObject(root, "vscpObid", pev->obid);
  cJSON_AddNumberToObject(root, "vscpTimeStamp", pev->timestamp);
  vscp_fwhlp_writeGuidToString(str, pev->GUID);
  cJSON_AddStringToObject(root, "vscpGUID", str);
  sprintf(str, "%04d-%02d-%02dT%02d:%02d:%02d", pev->year, pev->month, pev->day, pev->hour, pev->minute, pev->second);
  cJSON_AddStringToObject(root, "vscpDateTime", str);
  cJSON *array;
  array = cJSON_AddArrayToObject(root, "vscpData");
  cJSON *element;
  for (int i = 0; i < pev->sizeData; i++) {
    element = cJSON_CreateNumber(pev->pdata[i]);
    cJSON_AddItemToArray(array, element);
  }
  char *json_string = cJSON_Print(root);
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
vscp_fwhlp_create_json_ex(char *strObj, size_t len, const vscpEventEx *pex)
{
  char str[80];
  cJSON *root;

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
  cJSON *array;
  array = cJSON_AddArrayToObject(root, "vscpData");
  cJSON *element;
  for (int i = 0; i < pex->sizeData; i++) {
    element = cJSON_CreateNumber(pex->data[i]);
    cJSON_AddItemToArray(array, element);
  }
  char *json_string = cJSON_Print(root);
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

/*
  XML support needs VSCP_FWHLP_XML_SUPPORT to be defined
  in the projdef file and sxml support linked in (can be found
  in vscp-firmware/third-party or at https://github.com/nopnop2002/esp-idf-xml
*/

#ifdef VSCP_FWHLP_XML_SUPPORT

#define MAX_ATTRIBUTES 20
#define MAX_NAME_LEN   16
#define MAX_VALUE_LEN  64

// Known attribute names for reference (not strictly necessary, but can be used
// for validation or indexing)
const char known_attributes[][MAX_NAME_LEN] = { "unknown",
                                                "head",
                                                "obid",
                                                "datetime",
                                                "timestamp",
                                                "class",
                                                "type",
                                                "guid",
                                                "sizedata",
                                                "data",
                                                // Optional extra attributes that may be present in the event string
                                                "value",
                                                "unit",
                                                "sensorindex",
                                                "zone",
                                                "subzone",
                                                // 64-bit Linux timestamp in nanoseconds (replace year, month, day,
                                                // hour, minute, second and 32-bit timestamp with 64-bit timestamps)
                                                "timestamps" };

// Structure to hold attribute information
typedef struct {
  char name[MAX_NAME_LEN];   // Attribute name
  char value[MAX_VALUE_LEN]; // Attribute value
  int id;                    // Index in the unique attribute name list
} attribute_t;

static int
_find_or_add_name(const char *name, char name_list[MAX_ATTRIBUTES][MAX_NAME_LEN], size_t *name_count)
{
  // Check if the name exists in the known_attributes list
  for (size_t i = 0; i < sizeof(known_attributes) / sizeof(known_attributes[0]); i++) {
    if (strcmp(name, known_attributes[i]) == 0) {
      return i; // Return the index in the known_attributes list
    }
  }

  // If the name is not in known_attributes, add it to the dynamic name_list
  for (size_t i = 0; i < *name_count; i++) {
    if (strcmp(name, name_list[i]) == 0) {
      return i + (sizeof(known_attributes) / sizeof(known_attributes[0])); // Offset by known_attributes size
    }
  }

  if (*name_count < MAX_ATTRIBUTES) {
    strncpy(name_list[*name_count], name, MAX_NAME_LEN - 1);
    name_list[*name_count][MAX_NAME_LEN - 1] = '\0';
    return (*name_count)++ +
           (sizeof(known_attributes) / sizeof(known_attributes[0])); // Add the name and return its new index
  }

  return -1; // Error: name list is full
}

static int
_validate_event_string(const char *eventstr)
{
  if (!eventstr) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  // Skip leading whitespace
  while (*eventstr && isspace((unsigned char) *eventstr)) {
    eventstr++;
  }

  // Ensure the string starts with "<event"
  // next character can be space, tab, newline or end of string,
  // all valid except for end of string.
  if (strncmp(eventstr, "<event ", 7) != 0 && strncmp(eventstr, "<event\t", 7) != 0 &&
      strncmp(eventstr, "<event\n", 7) != 0 && strncmp(eventstr, "<event\r", 7) != 0) {
    return VSCP_ERROR_INVALID_SYNTAX;
  }

  // Find the end of the string, skipping trailing whitespace
  const char *end = eventstr + strlen(eventstr) - 1;
  while (end > eventstr && isspace((unsigned char) *end)) {
    end--;
  }

  // Ensure the string ends with "/>"
  if (end - eventstr < 1 || strncmp(end - 1, "/>", 2) != 0) {
    return VSCP_ERROR_INVALID_SYNTAX;
  }

  return VSCP_ERROR_SUCCESS;
}

static int
_parse_event_string(attribute_t *attributes, size_t *attribute_count, const char *eventstr)
{
  *attribute_count = 0;
  char name_list[MAX_ATTRIBUTES][MAX_NAME_LEN];
  size_t name_count = 0;

  if (!eventstr || !attributes || !attribute_count) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (VSCP_ERROR_SUCCESS != _validate_event_string(eventstr)) {
    return VSCP_ERROR_INVALID_SYNTAX;
  }

  const char *ptr = eventstr + 7; // Skip the "<event " part
  while (*ptr && *attribute_count < MAX_ATTRIBUTES) {

    // Skip to the next attribute name
    while (*ptr && !isalpha((unsigned char) *ptr)) {

      // Also skip comments starting with <!-- and ending with -->
      if (*ptr == '<' && strncmp(ptr, "<!--", 4) == 0) {
        const char *comment_end = strstr(ptr, "-->");
        if (!comment_end) {
          return VSCP_ERROR_INVALID_SYNTAX; // Malformed comment, stop parsing
        }
        ptr = comment_end + 3; // Skip the -->
      }
      ptr++;
    }

    // Skip comments starting with <!-- and ending with -->
    const char *comment_start = strstr(ptr, "<!--");
    if (comment_start && (comment_start == ptr)) {
      ptr = comment_start + 4; // Skip the <!--
      printf("Skipping comment...\n");
      const char *comment_end = strstr(ptr, "-->");
      if (comment_end) {
        printf("Comment content: %.*s\n", (int) (comment_end - ptr), ptr);
        ptr = comment_end + 3; // Skip the -->
      }
      else {
        break; // Malformed comment, stop parsing
      }
      continue;
    }

    // Extract the attribute name
    const char *start = ptr;
    while (*ptr && *ptr != '=') {
      ptr++;
    }

    if (*ptr == '=') {
      size_t len = ptr - start;
      if (len >= MAX_NAME_LEN) {
        len = MAX_NAME_LEN - 1;
      }
      char name[MAX_NAME_LEN];
      strncpy(name, start, len);
      name[len] = '\0';

      // Skip the '=' and the opening quote
      ptr++;
      if (*ptr == '"') {
        ptr++;
      }

      // Extract the value
      start = ptr;
      while (*ptr && *ptr != '"') {
        ptr++;
      }

      len = ptr - start;
      if (len >= MAX_VALUE_LEN) {
        len = MAX_VALUE_LEN - 1;
      }
      strncpy(attributes[*attribute_count].value, start, len);
      attributes[*attribute_count].value[len] = '\0';

      // Find or add the name to the name list and store its index in id
      attributes[*attribute_count].id = _find_or_add_name(name, name_list, &name_count);

      // Copy the name to the attribute
      strncpy(attributes[*attribute_count].name, name, MAX_NAME_LEN - 1);
      attributes[*attribute_count].name[MAX_NAME_LEN - 1] = '\0';

      // Increment the attribute count
      (*attribute_count)++;

      // Skip the closing quote
      if (*ptr == '"') {
        ptr++;
      }
    }
  }
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_parse_xml_event
//
// <event
//     head="3"
//     obid="1234"
//     datetime="2017-01-13T10:16:02"
//     timestamp="50817"
//     class="10"
//     type="6"
//     guid="00:00:00:00:00:00:00:00:00:00:00:00:00:01:00:02"
//     sizedata="7"
//     data="0x48,0x34,0x35,0x2E,0x34,0x36,0x34"
// />

int
vscp_fwhlp_parse_xml_event(vscpEvent *pev, const char *eventstr)
{
  int rv;
  attribute_t attributes[MAX_ATTRIBUTES];
  size_t attribute_count = 0;

  if ((NULL == pev) || (NULL == eventstr)) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  if (VSCP_ERROR_SUCCESS != (rv = _parse_event_string(attributes, &attribute_count, eventstr))) {
    return rv;
  }

  memset(pev, 0, sizeof(vscpEvent)); // Clear the event structure

  for (int i = 0; i < attribute_count; i++) {

    switch (attributes[i].id) {

      case 1: // head
        pev->head = atoi(attributes[i].value);
        break;

      case 2: // obid
        pev->obid = atoi(attributes[i].value);
        break;

      case 3: // datetime
        // Parse datetime string and fill pev->year, pev->month, etc.
        if (VSCP_ERROR_SUCCESS != vscp_fwhlp_parse_event_datestr(pev, attributes[i].value, NULL)) {
          return VSCP_ERROR_PARAMETER;
        }
        break;

      case 4: // timestamp
        pev->timestamp = atoi(attributes[i].value);
        break;

      case 5: // class
        pev->vscp_class = atoi(attributes[i].value);
        break;

      case 6: // type
        pev->vscp_type = atoi(attributes[i].value);
        break;

      case 7: // guid
        if (VSCP_ERROR_SUCCESS != (rv = vscp_fwhlp_parseGuid(pev->GUID, attributes[i].value, NULL))) {
          return rv;
        }
        break;

      case 8: // sizedata
        pev->sizeData = atoi(attributes[i].value);
        break;

      case 9: // data
        // Parse data string and fill pev->pdata accordingly
        {
          pev->pdata = (uint8_t *) malloc(512); // Allocate maximum possible data size
          if (NULL == pev->pdata) {
            return VSCP_ERROR_MEMORY;
          }
          const char *endptr;
          rv = vscp_fwhlp_parse_data(pev->pdata, 512, attributes[i].value, &endptr);
          if (rv < 0) {
            free(pev->pdata);
            pev->pdata = NULL;
            return rv; // Return the error code from data parsing
          }
          pev->sizeData = rv;                                             // Set the actual size of data parsed
          pev->pdata    = (uint8_t *) realloc(pev->pdata, pev->sizeData); // Resize to actual data size
          if (NULL == pev->pdata) {
            return VSCP_ERROR_MEMORY;
          }
        }
        break;

      default:
        // Handle unknown attributes if necessary
        break;
    }
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_parse_xml_eventex
//

int
vscp_fwhlp_parse_xml_eventex(vscpEventEx *pex, const char *eventexstr)
{
  int rv;
  attribute_t attributes[MAX_ATTRIBUTES];
  size_t attribute_count = 0;

  if ((NULL == pex) || (NULL == eventexstr)) {
    return VSCP_ERROR_INVALID_POINTER;
  }
  printf("Parsing event string: %s\n", eventexstr);
  if (VSCP_ERROR_SUCCESS != (rv = _parse_event_string(attributes, &attribute_count, eventexstr))) {
    return rv;
  }

  memset(pex, 0, sizeof(vscpEventEx)); // Clear the event structure

  for (int i = 0; i < attribute_count; i++) {

    switch (attributes[i].id) {

      case 1: // head
        pex->head = atoi(attributes[i].value);
        break;

      case 2: // obid
        pex->obid = atoi(attributes[i].value);
        break;

      case 3: // datetime
        // Parse datetime string and fill pex->year, pex->month, etc.
        if (VSCP_ERROR_SUCCESS != vscp_fwhlp_parse_eventex_datestr(pex, attributes[i].value, NULL)) {
          return VSCP_ERROR_PARAMETER;
        }
        break;

      case 4: // timestamp
        pex->timestamp = atoi(attributes[i].value);
        break;

      case 5: // class
        pex->vscp_class = atoi(attributes[i].value);
        break;

      case 6: // type
        pex->vscp_type = atoi(attributes[i].value);
        break;

      case 7: // guid
        if (VSCP_ERROR_SUCCESS != (rv = vscp_fwhlp_parseGuid(pex->GUID, attributes[i].value, NULL))) {
          return rv;
        }
        break;

      case 8: // sizedata
        pex->sizeData = atoi(attributes[i].value);
        break;

      case 9: // data
        // Parse data string and fill pex->pdata accordingly
        {
          rv = vscp_fwhlp_parse_data(pex->data, 512, attributes[i].value, NULL);
          if (rv < 0) {
            return rv; // Return the error code from data parsing
          }
          pex->sizeData = rv; // Set the actual size of data parsed
        }
        break;

      default:
        // Handle unknown attributes if necessary
        break;
    }
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_event_to_xml
//

int
vscp_fwhlp_event_to_xml(char *eventstr, size_t len, const vscpEvent *pev)
{
  int rv;
  char strDateTime[64]  = { 0 };
  char strGuid[80]      = { 0 };
  char strData[512 * 5] = { 0 }; // 512 bytes of data can expand to 2048 characters in hex string format 0xff,0xff,...
                                 // (4 characters per byte + comma)
 
  // Check pointer
  if (NULL == pev || NULL == eventstr) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  // Check buffer length 
  if (len < (80 + 64 + 512 * 5 + 256)) {
    return VSCP_ERROR_BUFFER_TO_SMALL;
  }

  if (VSCP_ERROR_SUCCESS != (rv = vscp_fwhlp_writeGuidToString(strGuid, pev->GUID))) {
    return rv;
  }

  // NULL result is OK, it just means there is no data to convert and strData will be empty string
  vscp_fwhlp_make_string_from_data(strData, sizeof(strData), pev->pdata, pev->sizeData);
  
  // NULL result is OK, it just means there is no date information to convert and strDateTime will be empty string
  vscp_fwhlp_get_datestr_from_event(strDateTime, sizeof(strDateTime), pev);

  // datetime,head,obid,datetime,timestamp,class,type,guid,sizedata,data,note
  sprintf(eventstr,
          VSCP_XML_EVENT_TEMPLATE,
          (unsigned short int) pev->head,
          (unsigned long) pev->obid,
          (const char *) strDateTime,
          (unsigned long) pev->timestamp,
          (unsigned short int) pev->vscp_class,
          (unsigned short int) pev->vscp_type,
          (const char *) strGuid,
          (const char *) strData);

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_eventex_to_xml
//

int
vscp_fwhlp_eventex_to_xml(char *eventexstr, size_t len, const vscpEventEx *pex)
{
  int rv;
  char strGuid[80];
  char strDateTime[64];
  char strData[512 * 5]; // 512 bytes of data can expand to 2048 characters in hex string format 0xff,0xff,... (4
                         // characters per byte + comma)
  
  // Check pointer
  if (NULL == pex || NULL == eventexstr) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  // Check buffer length 
  if (len < (80 + 64 + 512 * 5 + 256)) {
    return VSCP_ERROR_BUFFER_TO_SMALL;
  }

  if (VSCP_ERROR_SUCCESS != (rv = vscp_fwhlp_writeGuidToString(strGuid, pex->GUID))) {
    return rv;
  }

  // NULL result is OK, it just means there is no data to convert and strData will be empty string
  vscp_fwhlp_make_string_from_data(strData, sizeof(strData), pex->data, pex->sizeData);

  // NULL result is OK, it just means there is no date information to convert and strDateTime will be empty string
  vscp_fwhlp_get_datestr_from_eventex(strDateTime, sizeof(strDateTime), pex);

  // datetime,head,obid,datetime,timestamp,class,type,guid,sizedata,data,note
  sprintf(eventexstr,
          VSCP_XML_EVENT_TEMPLATE,
          (unsigned short int) pex->head,
          (unsigned long) pex->obid,
          (const char *) strDateTime,
          (unsigned long) pex->timestamp,
          (unsigned short int) pex->vscp_class,
          (unsigned short int) pex->vscp_type,
          (const char *) strGuid,
          (const char *) strData);

  return VSCP_ERROR_SUCCESS;
}

#endif

// ----------------------------------------------------------------------------

// ***************************************************************************
//                              Data Coding Helpers
// ***************************************************************************

//////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getMeasurementDataCoding
//

uint8_t
vscp_fwhlp_getMeasurementDataCoding(const vscpEvent *pEvent)
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
vscp_fwhlp_getDataCodingBitArray(const uint8_t *pCode, const uint8_t length)
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
vscp_fwhlp_getDataCodingInteger(const uint8_t *pCode, uint8_t length)
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
    memset((uint8_t *) &value64, 0xff, 8);
  }
  else {
    memset((uint8_t *) &value64, 0, 8);
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
vscp_fwhlp_getDataCodingNormalizedInteger(const uint8_t *pCode, uint8_t length)
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
      memcpy((char *) &valarray, (pCode + 2), (length - 2));
      value = *((int8_t *) valarray);
      break;

    case 2: // 16-bit
      memcpy((char *) &valarray, (pCode + 2), (length - 2));
      value = VSCP_INT16_SWAP_ON_LE(*((uint16_t *) valarray));
      break;

    case 3: // 24-bit
      memcpy(((char *) &valarray + 1), (pCode + 2), (length - 2));
      if (bNegative)
        *valarray = 0xff; // First byte must be 0xff
      value = VSCP_INT32_SWAP_ON_LE(*((int32_t *) valarray));
      break;

    case 4: // 32-bit
      memcpy((char *) &valarray, (pCode + 2), (length - 2));
      value = VSCP_INT32_SWAP_ON_LE(*((int32_t *) valarray));
      break;

    case 5: // 40-bit
      memcpy(((char *) &valarray + 3), (pCode + 2), (length - 2));
      if (bNegative) {
        *valarray       = 0xff; // First byte must be 0xff
        *(valarray + 1) = 0xff;
        *(valarray + 2) = 0xff;
      }

#ifdef __BIG_ENDIAN__
      value = *((int64_t *) valarray);
#else
      value64 = Swap8Bytes(*((int64_t *) valarray));
      value   = (double) value64;
#endif
      break;

    case 6: // 48-bit
      memcpy(((char *) &valarray + 2), (pCode + 2), (length - 2));
      if (bNegative) {
        *valarray       = 0xff; // First byte must be 0xff
        *(valarray + 1) = 0xff;
      }

#ifdef __BIG_ENDIAN__
      value = *((int64_t *) valarray);
#else
      value64 = Swap8Bytes(*((int64_t *) valarray));
      value   = (double) value64;
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
vscp_fwhlp_setEventPriority(vscpEvent *pEvent, unsigned char priority)
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
vscp_fwhlp_getMeasurementUnit(const vscpEvent *pEvent)
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
vscp_fwhlp_getMeasurementSensorIndex(const vscpEvent *pEvent)
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
vscp_fwhlp_getMeasurementZone(const vscpEvent *pEvent)
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
vscp_fwhlp_getMeasurementSubZone(const vscpEvent *pEvent)
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
vscp_fwhlp_isMeasurement(const vscpEvent *pEvent)
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
  return (uint16_t) (0x1ff & (id >> 16));
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getVscpTypeFromCANALid
//

uint16_t
vscp_fwhlp_getVscpTypeFromCANALid(uint32_t id)
{
  return (uint16_t) (0xff & (id >> 8));
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
  return (((unsigned long) priority << 26) | ((unsigned long) vscp_class << 16) | ((unsigned long) vscp_type << 8) | 0);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fwhlp_getCANALidFromEvent
//

uint32_t
vscp_fwhlp_getCANALidFromEvent(const vscpEvent *pEvent)
{
  return (((unsigned long) vscp_fwhlp_getEventPriority(pEvent) << 26) | ((unsigned long) pEvent->vscp_class << 16) |
          ((unsigned long) pEvent->vscp_type << 8) | 0);
}
