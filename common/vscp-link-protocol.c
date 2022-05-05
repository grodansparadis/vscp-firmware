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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vscp-link-protocol.h"
#include "vscp.h"

///////////////////////////////////////////////////////////////////////////////
// vscp_link_parser
//

int vscp_link_parser(const void *pdata, const char *cmd) {
  const char *p = cmd;
  const char *pcmd;

  // Check pointer
  if (NULL == cmd)
    return VSCP_ERROR_INVALID_POINTER;

  // Remove whitespace from command
  while (*p && (*p == ' ')) {
    p++;
  }

  pcmd = p;

  if (NULL != (p = strstr(pcmd, "noop"))) {
    p += 4;
    return vscp_link_doCmdNoop(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "help"))) {
    p += 4;
    return vscp_link_doCmdHelp(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "quit"))) {
    p += 4;
    return vscp_link_doCmdQuit(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "user"))) {
    p += 4;
    return vscp_link_doCmdUser(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "pass"))) {
    p += 4;
    return vscp_link_doCmdPassword(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "challenge"))) {
    p += 9;
    return vscp_link_doCmdChallenge(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "send"))) {
    p += 4;
    return vscp_link_doCmdSend(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "retr"))) {
    p += 4;
    return vscp_link_doCmdRetrive(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "rcvloop"))) {
    p += 7;
    return vscp_link_doCmdRcvLoop(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "quitloop"))) {
    p += 8;
    return vscp_link_doCmdQuitLoop(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "cdta"))) {
    p += 4;
    return vscp_link_doCmdCmdCheckData(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "chkdata"))) {
    p += 7;
    return vscp_link_doCmdCheckData(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "clra"))) {
    p += 4;
    return vscp_link_doCmdClearAll(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "clrall"))) {
    p += 6;
    return vscp_link_doCmdClearAll(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "stat"))) {
    p += 4;
    return vscp_link_doCmdStatistics(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "info"))) {
    p += 4;
    return vscp_link_doCmdInfo(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "chid"))) {
    p += 4;
    return vscp_link_doCmdGetChannelId(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "getchid"))) {
    p += 7;
    return vscp_link_doCmdGetChannelId(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "sgid"))) {
    p += 4;
    return vscp_link_doCmdSetChannelId(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "setguid"))) {
    p += 7;
    return vscp_link_doCmdSetChannelId(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "ggid"))) {
    p += 4;
    return vscp_link_doCmdGetGUID(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "getguid"))) {
    p += 7;
    return vscp_link_doCmdGetGUID(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "vers"))) {
    p += 4;
    return vscp_link_doCmdGetVersion(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "version"))) {
    p += 7;
    return vscp_link_doCmdGetVersion(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "sflt"))) {
    p += 4;
    return vscp_link_doCmdSetFilter(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "setfilter"))) {
    p += 9;
    return vscp_link_doCmdSetFilter(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "smsk"))) {
    p += 4;
    return vscp_link_doCmdSetMask(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "setmask"))) {
    p += 7;
    return vscp_link_doCmdSetMask(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "wcyd"))) {
    p += 4;
    return vscp_link_doCmdWhatCanYouDo(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "whatcanyoudo"))) {
    p += 12;
    return vscp_link_doCmdWhatCanYouDo(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "+"))) {
    p += 1;
    return vscp_link_doCmdCommandAgain(pdata, p);
  } else if (NULL != (p = strstr(pcmd, "interface"))) {
    p += 9;
    return vscp_link_doCmdInterface(pdata, p);
  }

  // Unknown command
  vscp_link_callback_writeClient(pdata, "-OK - Unknown command\r\n");
  return VSCP_ERROR_MISSING;
}

///////////////////////////////////////////////////////////////////////////////
// doCmdInterface
//
// list
// close
//

int vscp_link_doCmdInterface(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_reply
//

int vscp_link_reply(const void *pdata, const char *msg) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdNoop
//

int vscp_link_doCmdNoop(const void *pdata, const char *cmd) {
  vscp_link_callback_writeClient(pdata, "+OK\r\n");
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdHelp
//

int vscp_link_doCmdHelp(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdQuit
//

int vscp_link_doCmdQuit(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdUser
//

int vscp_link_doCmdUser(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdPassword
//

int vscp_link_doCmdPassword(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdChallenge
//

int vscp_link_doCmdChallenge(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdSend
//

int vscp_link_doCmdSend(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdRetrive
//

int vscp_link_doCmdRetrive(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdRcvLoop
//

int vscp_link_doCmdRcvLoop(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// VSCP_ERROR_SUCCESS
//

int vscp_link_doCmdQuitLoop(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// VSCP_ERROR_SUCCESS
//

int vscp_link_doCmdCmdCheckData(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// VSCP_ERROR_SUCCESS
//

int vscp_link_doCmdCheckData(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// VSCP_ERROR_SUCCESS
//

int vscp_link_doCmdClearAll(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdStatistics
//

int vscp_link_doCmdStatistics(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdInfo
//

int vscp_link_doCmdInfo(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdGetChannelId
//

int vscp_link_doCmdGetChannelId(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdSetChannelId
//

int vscp_link_doCmdSetChannelId(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdGetGUID
//

int vscp_link_doCmdGetGUID(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdGetVersion
//

int vscp_link_doCmdGetVersion(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdSetFilter
//

int vscp_link_doCmdSetFilter(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdSetMask
//

int vscp_link_doCmdSetMask(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdWhatCanYouDo
//

int vscp_link_doCmdWhatCanYouDo(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdCommandAgain
//

int vscp_link_doCmdCommandAgain(const void *pdata, const char *cmd) {
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// newEvent
//

vscpEvent *newEvent(void) {
  vscpEvent *pev = NULL;

  pev = (vscpEvent *)VSCP_LINK_MALLOC(sizeof(vscpEvent));
  if (NULL != pev)
    pev->pdata = NULL;

  return pev;
}

///////////////////////////////////////////////////////////////////////////////
// deleteEvent
//

int deleteEvent(vscpEvent **pev) {
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

int parseGuid(uint8_t *guid, const char *strguid, char **endptr) {
  int i;
  char *p = (char *)strguid;

  // Check pointers
  if (NULL == strguid)
    return VSCP_ERROR_INVALID_POINTER;
  if (NULL == guid)
    return VSCP_ERROR_INVALID_POINTER;

  memset(guid, 0, 16);

  // remove initial white space
  while (' ' == *p)
    p++;

  // Empty GUID or GUID  set to '-' means all nulls
  if (!*p || ('-' == *p)) {
    if (NULL != endptr)
      *endptr = p;
    return VSCP_ERROR_SUCCESS;
  }

  for (i = 0; i < 16; i++) {
    guid[i] = (uint8_t)strtol(p, &p, 16);
    if (!*p)
      break; // at end?
    if (i != 15) {
      if (':' != *p)
        return VSCP_ERROR_ERROR;
      p++; // Move beyond ':'
      if (p > (strguid + strlen(strguid))) {
        if (NULL != endptr)
          *endptr = p;
        return VSCP_ERROR_ERROR;
      }
    }
  }

  if (NULL != endptr)
    *endptr = p;
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// writeGuidToString
//

int writeGuidToString(char *strguid, const uint8_t *guid) {
  // Check pointers
  if (NULL == strguid)
    return VSCP_ERROR_INVALID_POINTER;
  if (NULL == guid)
    return VSCP_ERROR_INVALID_POINTER;

  sprintf(strguid,
          "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:"
          "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
          guid[0], guid[1], guid[2], guid[3], guid[4], guid[5], guid[6],
          guid[7], guid[8], guid[9], guid[10], guid[11], guid[12], guid[13],
          guid[14], guid[15]);

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// parseFilter
//
// filter-priority, filter-class, filter-type, filter-GUID
//

int parseFilter(vscpEventFilter *pfilter, const char *strfilter) {
  char *p = (char *)strfilter;

  // Check pointers
  if (NULL == strfilter)
    return VSCP_ERROR_INVALID_POINTER;
  if (NULL == pfilter)
    return VSCP_ERROR_INVALID_POINTER;

  // Priority
  pfilter->filter_priority = (uint8_t)strtol(p, &p, 0);
  if (',' != *p)
    return VSCP_ERROR_ERROR;
  p++;
  if (p > (strfilter + strlen(strfilter))) {
    return VSCP_ERROR_ERROR;
  }

  // Class filter
  pfilter->filter_class = (uint16_t)strtol(p, &p, 0);
  if (',' != *p)
    return VSCP_ERROR_ERROR;
  p++;
  if (p > (strfilter + strlen(strfilter))) {
    return VSCP_ERROR_ERROR;
  }

  // Type filter
  pfilter->filter_type = (uint16_t)strtol(p, &p, 0);
  if (',' != *p)
    return VSCP_ERROR_ERROR;
  p++;
  if (p > (strfilter + strlen(strfilter))) {
    return VSCP_ERROR_ERROR;
  }

  // GUID filter
  return parseGuid(pfilter->filter_GUID, p, NULL);
}

///////////////////////////////////////////////////////////////////////////////
// parseMask
//
// mask-priority, mask-class, mask-type, mask-GUID
//

int parseMask(vscpEventFilter *pfilter, const char *strmask) {
  char *p = (char *)strmask;

  // Check pointers
  if (NULL == strmask)
    return VSCP_ERROR_INVALID_POINTER;
  if (NULL == pfilter)
    return VSCP_ERROR_INVALID_POINTER;

  // Priority
  pfilter->mask_priority = (uint8_t)strtol(p, &p, 0);
  if (',' != *p)
    return VSCP_ERROR_ERROR;
  p++;
  if (p > (strmask + strlen(strmask))) {
    return VSCP_ERROR_ERROR;
  }

  // Class mask
  pfilter->mask_class = (uint16_t)strtol(p, &p, 0);
  if (',' != *p)
    return VSCP_ERROR_ERROR;
  p++;
  if (p > (strmask + strlen(strmask))) {
    return VSCP_ERROR_ERROR;
  }

  // Type mask
  pfilter->mask_type = (uint16_t)strtol(p, &p, 0);
  if (',' != *p)
    return VSCP_ERROR_ERROR;
  p++;
  if (p > (strmask + strlen(strmask))) {
    return VSCP_ERROR_ERROR;
  }

  // GUID mask
  return parseGuid(pfilter->mask_GUID, p, NULL);
}

///////////////////////////////////////////////////////////////////////////////
// parseEvent
//
// head,class,type,obid,datetime,timestamp,GUID,data1,data2,data3....
//

int parseEvent(vscpEvent *pev, const char *buf) 
{
  uint8_t wrkbuf[512];
  char *p = (char *)buf;

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
    return VSCP_ERROR_ERROR;
  }
  p++; // point beyond comma
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_ERROR;
  }

  // VSCP class
  pev->vscp_class = (uint16_t)strtol(p, &p, 0);
  if (',' != *p)
    return VSCP_ERROR_ERROR;
  p++; // point beyond comma
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_ERROR;
  }

  // VSCP type
  pev->vscp_type = (uint16_t)strtol(p, &p, 0);
  if (',' != *p)
    return VSCP_ERROR_ERROR;
  p++;
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_ERROR;
  }

  // obid
  pev->obid = (uint32_t)strtol(p, &p, 0);
  if (',' != *p)
    return VSCP_ERROR_ERROR;
  p++; // point beyond comma
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_ERROR;
  }

  // datetime YYYY-MM-DDTHH:MM:SS

  // Date may not be given (left blank) and in
  // this case it should be set to all nulls for
  // the interface to set it instead.

  if (',' == *p) {
    p++; // point beyond comma
  } else {
    // year
    pev->year = (uint16_t)strtol(p, &p, 0);
    if ('-' != *p)
      return VSCP_ERROR_ERROR;
    p++; // point beyond dash
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_ERROR;
    }

    // month
    pev->month = (uint16_t)strtol(p, &p, 0);
    if ('-' != *p)
      return VSCP_ERROR_ERROR;
    p++; // point beyond dash
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_ERROR;
    }

    // day
    pev->day = (uint16_t)strtol(p, &p, 0);
    if ('T' != *p)
      return VSCP_ERROR_ERROR;
    p++; // point beyond dash
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_ERROR;
    }

    // hour
    pev->hour = (uint16_t)strtol(p, &p, 0);
    if (':' != *p)
      return VSCP_ERROR_ERROR;
    p++; // point beyond colon
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_ERROR;
    }

    // minute
    pev->minute = (uint16_t)strtol(p, &p, 0);
    if (':' != *p)
      return VSCP_ERROR_ERROR;
    p++; // point beyond colon
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_ERROR;
    }

    // second
    pev->second = (uint16_t)strtol(p, &p, 0);
    if (',' != *p)
      return VSCP_ERROR_ERROR;
    p++; // point beyond comma
    if (p > (buf + strlen(buf))) {
      return VSCP_ERROR_ERROR;
    }
  }

  // timestamp
  pev->timestamp = (uint32_t)strtol(p, &p, 0);
  if (',' != *p)
    return VSCP_ERROR_ERROR;
  p++; // point beyond comma
  if (p > (buf + strlen(buf))) {
    return VSCP_ERROR_ERROR;
  }

  // Get GUID
  if (VSCP_ERROR_SUCCESS != parseGuid(pev->GUID, p, &p)) {
    return VSCP_ERROR_ERROR;
  }

  if (',' != *p)
    return VSCP_ERROR_ERROR;
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
    if (NULL == pev->pdata)
      return VSCP_ERROR_ERROR;
    memcpy(pev->pdata, wrkbuf, pev->sizeData);
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// parseEventEx
//

int parseEventEx(vscpEventEx *pex, const char *buf) 
{
  char *p = (char *)buf;

  // Check pointers
  if (NULL == buf) {
    return VSCP_ERROR_INVALID_POINTER;
  }
  if (NULL == pex) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  memset(pex, 0, sizeof(vscpEventEx));

  vscpEvent *pev = newEvent();
  if (NULL == pev)
    return VSCP_ERROR_ERROR;

  if (VSCP_ERROR_SUCCESS != parseEvent(pev, buf)) {
    deleteEvent(&pev);
    return VSCP_ERROR_ERROR;
  }

  // Copy in data
  pex->head = pev->head;
  pex->obid = pev->obid;
  pex->year = pev->year;
  pex->month = pev->month;
  pex->day = pev->day;
  pex->hour = pev->hour;
  pex->minute = pev->minute;
  pex->second = pev->second;
  pex->timestamp = pev->timestamp;
  pex->vscp_class = pev->vscp_class;
  pex->vscp_type = pev->vscp_type;
  memcpy(pex->GUID, pev->GUID, 16);
  pex->sizeData = pev->sizeData;
  if (pex->sizeData) {
    memcpy(pex->data, pev->pdata, pev->sizeData);
  }

  deleteEvent(&pev);

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// eventToString
//
// head       4  - 0xff or 255
// class      6  - 0xFFFF or 65535
// type       6  - 0xFFFF or 65535
// obid       10 - 0xFFFFFFFF
// time       20 - YYYY-MM-DDTHH:MM:SSZ
// timestamp  10 - 0xFFFFFFFF
// GUID       47 - 00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF
// ---------------------------------------------------------------
// Total:     103 + 7 (commas) + data = 110 + data
//

int eventToString(char *buf, size_t size, const vscpEvent *pev) 
{
  char wrkbuf[48]; // Can hold full GUID

  // Check pointers
  if (NULL == pev) {
    return VSCP_ERROR_INVALID_POINTER;
  }
  if (NULL == buf) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  // Len must be able to hold content
  if (size < 110) {
    return VSCP_ERROR_BUFFER_TO_SMALL;
  }

  if (pev->year || pev->month || pev->day || pev->hour || pev->minute ||
      pev->second) {
    sprintf(buf, "%u,%u,%u,%lu,%4d-%02d-%02dT%02d:%02d:%02dZ,%lu,",
            (unsigned)pev->head, (unsigned)pev->vscp_class,
            (unsigned)pev->vscp_type, (unsigned long)pev->obid, (int)pev->year,
            (int)pev->month, (int)pev->day, (int)pev->hour, (int)pev->minute,
            (int)pev->second, (unsigned long)pev->timestamp);
  } 
  else {
    sprintf(buf, "%d,%u,%u,%lu,,%lu,", (unsigned)pev->head,
            (unsigned)pev->vscp_class, (unsigned)pev->vscp_type,
            (unsigned long)pev->obid, (unsigned long)pev->timestamp);
  }

  // GUID
  writeGuidToString(buf, pev->GUID);
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
      } else {
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
