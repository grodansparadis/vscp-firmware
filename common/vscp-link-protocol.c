// FILE: vscp-link-protocol.c

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol)
 * 	https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2023 Ake Hedman, Grodans Paradis AB
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
#include <vscp-firmware-helper.h>

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
  // We handle this the same way as a disconnect
  // from a user
  vscp_link_callback_quit(pdata);
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_idle_worker
//

int
vscp_link_idle_worker(const void* pdata)
{
  int rv;

  // Get receive loop status
  int bRcvLoop = 0;
  if (VSCP_ERROR_SUCCESS != (rv = vscp_link_callback_get_rcvloop_status(pdata, &bRcvLoop))) {
    return rv;
  }

  /*
    If we are in the rcvloop we do a callback to the application
    so it can send events or '+OK\r\n" each second.
    Note that we still handle all commands.
  */
  if (bRcvLoop) {

    int rv;
    char buf[THIS_FIRMWARE_TCPIP_LINK_MAX_BUFFER];
    vscpEvent* pev = NULL;

    rv = vscp_link_callback_rcvloop(pdata, &pev);
    if (VSCP_ERROR_SUCCESS == rv) {
      // We own the event from now on and must
      // delete it and it's data when we are done
      // with it
      int rv = vscp_fwhlp_eventToString(buf, sizeof(buf), pev);
      if (VSCP_ERROR_SUCCESS == rv) {
        // Write out the event
        strcat(buf, "\r\n+OK\r\n");
        vscp_link_callback_write_client(pdata, buf);
      }
      // Free event
      vscp_fwhlp_deleteEvent(&pev);
    }
    else if (VSCP_ERROR_TIMEOUT == rv) {
      // Timeout - Write out '+OK\r\n'
      vscp_link_callback_write_client(pdata, "+OK\r\n");
    }

    // No error on failure
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_parser
//

int
vscp_link_parser(const void* pdata, char* pbuf, char** pnext)
{
  char* p;
  int rv;

  // Check pointers
  if ((NULL == pdata) || (NULL == pnext)) {
    return VSCP_ERROR_INVALID_POINTER;
  }

  // If buf contains a carriage return, we have a command to handle
  // If not we are done and need to read more input data

  // printf("00000000 [%s]\n", pbuf);
  if (NULL == (p = strstr(pbuf, "\r\n"))) {
    // printf("missing\n");
    return VSCP_ERROR_MISSING;
  }

  // printf("1111111 [%s]\n", p);
  *p     = '\0';  // Terminate string at \r\n
  *pnext = p + 2; // Point beyond the \r\n
  // printf(">>rest str: [%s]\n", *pnext);

  // Just CRLF
  if (2 == strlen(pbuf)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK); // vscp_link_doCmdNoop(pdata, p);
  }

  // Get receive loop status
  int bRcvLoop = 0;
  if (VSCP_ERROR_SUCCESS != (rv = vscp_link_callback_get_rcvloop_status(pdata, &bRcvLoop))) {
    return rv;
  }

  // Remove leading whitespace from command
  const char* pcmd = pbuf;

  /*
   * THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD   bRcvLoop   =   THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD | !bRcvLoop
   *        0                 0      =   0 | 1 = 1
   *        0                 1      =   0 | 0 = 0
   *        1                 0      =   1 | 1 = 1
   *        1                 1      =   1 | 0 = 1
   */

  if (NULL != (p = vscp_fwhlp_stristr(pcmd, "help")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdHelp(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "noop")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdNoop(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "user")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdUser(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "pass")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdPassword(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "challenge")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 9;
    return vscp_link_doCmdChallenge(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "send")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdSend(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "retr")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdRetrieve(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "rcvloop")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdRcvLoop(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "quitloop"))) {
    p += 8;
    return vscp_link_doCmdQuitLoop(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "quit"))) {
    p += 4;
    return vscp_link_doCmdQuit(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "cdta")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdCheckData(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "chkdata")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdCheckData(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "clra")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdClearAll(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "clrall")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 6;
    return vscp_link_doCmdClearAll(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "stat")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdStatistics(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "info")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdInfo(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "chid")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdGetChannelId(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "getchid")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdGetChannelId(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "sgid")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdSetGUID(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "setguid")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdSetGUID(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "ggid")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdGetGUID(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "getguid")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdGetGUID(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "vers")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdGetVersion(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "version")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdGetVersion(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "sflt")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdSetFilter(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "setfilter")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 9;
    return vscp_link_doCmdSetFilter(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "smsk")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdSetMask(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "setmask")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdSetMask(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "test")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdTest(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "wcyd")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdWhatCanYouDo(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "whatcanyoudo")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 12;
    return vscp_link_doCmdWhatCanYouDo(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "+")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 1;
    return vscp_link_doCmdCommandAgain(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "interface")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 9;
    return vscp_link_doCmdInterface(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "shutdown")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 8;
    return vscp_link_doCmdShutdown(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "restart")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdRestart(pdata, p);
  }

  // Binary commands

  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "bretr")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdbRetr(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "bsend")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdbSend(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "brcvloop")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdbRcvLoop(pdata, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pcmd, "sec")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdSec(pdata, p);
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
  if (NULL != vscp_fwhlp_stristr(cmd, "noop")) {
    printf("----->");
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_NOOP);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "help")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_HELP);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "quit")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_QUIT);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "user")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_USER);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "pass")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_PASS);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "challenge")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_CHALLENGE);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "send")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_SEND);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "retr")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_RETR);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "rcvloop")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_RCVLOOP);
  }
  else if (NULL != strstr(cmd, "quitloop")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_QUITLOOP);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "cdta") || NULL != strstr(cmd, "chkdata")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_CDTA);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "clra") || NULL != strstr(cmd, "clrall")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_CLRA);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "stat")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_STAT);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "info")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_INFO);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "chid")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_CHID);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "sgid") || NULL != strstr(cmd, "setguid")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_SGID);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "ggid") || NULL != strstr(cmd, "getguid")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_GGID);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "vers") || NULL != strstr(cmd, "version")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_VERS);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "sflt") || NULL != strstr(cmd, "setfilter")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_SFLT);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "smsk") || NULL != strstr(cmd, "setmask")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_SMSK);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "test")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_TEST);
  }
  else if (NULL != vscp_fwhlp_stristr(cmd, "wcyd") || NULL != strstr(cmd, "whatcanyoudo")) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_WCYD);
  }
  else {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_STD_HELP_TEXT);
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
  vscpEvent* pev;

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 4)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  pev = vscp_fwhlp_newEvent();
  if (VSCP_ERROR_SUCCESS != (rv = vscp_fwhlp_parseEvent(pev, cmd))) {
    if (VSCP_ERROR_INVALID_POINTER == rv) {
      vscp_fwhlp_deleteEvent(&pev);
      return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
    }
    else if (VSCP_ERROR_MEMORY == rv) {
      vscp_fwhlp_deleteEvent(&pev);
      return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
    }
    else {
      vscp_fwhlp_deleteEvent(&pev);
      return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_PARAMETER_ERROR);
    }
  }

  if (VSCP_ERROR_SUCCESS != (rv = vscp_link_callback_send(pdata, pev))) {
    if (VSCP_ERROR_INVALID_POINTER == rv) {
      vscp_fwhlp_deleteEvent(&pev);
      return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_PARAMETER_ERROR);
    }
    else if (VSCP_ERROR_TRM_FULL == rv) {
      vscp_fwhlp_deleteEvent(&pev);
      return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_BUFFER_FULL);
    }
    else {
      vscp_fwhlp_deleteEvent(&pev);
      return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
    }
  }

  return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdRetrive
//

int
vscp_link_doCmdRetrieve(const void* pdata, const char* cmd)
{
  int rv;
  char buf[PRJDEF_VSCP_LINK_MAX_BUF];
  vscpEvent* pev = NULL;

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 2)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  int cnt = vscp_fwhlp_readStringValue(cmd);
  if (cnt <= 0) {
    cnt = 1;
  }

  while (VSCP_ERROR_SUCCESS == (rv = vscp_link_callback_retr(pdata, &pev))) {

    // We own the event from now on and must
    // delete it and it's data when we are done
    // with it

    if (VSCP_ERROR_SUCCESS == (rv = vscp_fwhlp_eventToString(buf, sizeof(buf), pev))) {
      strcat(buf, "\r\n");
      vscp_link_callback_write_client(pdata, buf);
    }
    else {
      return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
    }

    // Free event
    vscp_fwhlp_deleteEvent(&pev);

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
  if (VSCP_ERROR_SUCCESS != vscp_link_callback_chkData(pdata, &cnt)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }

  char buf[10];
  sprintf(buf, "%u\r\n", cnt);
  vscp_link_callback_write_client(pdata, buf);
  return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
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
  char* endptr;
  uint8_t guid[16];

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 6)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  if (VSCP_ERROR_SUCCESS != vscp_fwhlp_parseGuid(guid, cmd, &endptr)) {
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

  if (VSCP_ERROR_SUCCESS != vscp_fwhlp_writeGuidToString(strguid, guid)) {
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

  if (VSCP_ERROR_SUCCESS != vscp_fwhlp_parseFilter(&filter, cmd)) {
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

  if (VSCP_ERROR_SUCCESS != vscp_fwhlp_parseMask(&filter, cmd)) {
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
  char buf[130], wbuf[50];
  struct vscp_interface_info ifinfo;

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_authenticated(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_NOT_ACCREDITED);
  }

  if (VSCP_ERROR_SUCCESS != vscp_link_callback_check_privilege(pdata, 2)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }

  for (uint16_t i = 0; i < vscp_link_callback_get_interface_count(pdata); i++) {

    memset(buf, 0, sizeof(buf));
    if (VSCP_ERROR_SUCCESS != vscp_link_callback_get_interface(pdata, i, &ifinfo)) {
      return VSCP_ERROR_ERROR;
    }

    if (VSCP_ERROR_SUCCESS != vscp_fwhlp_writeGuidToString(wbuf, ifinfo.guid)) {
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

// ----------------------------------------------------------------------------
//                                   Binary
// ----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdbRetr
//

int
vscp_link_doCmdbRetr(const void* pdata, const char* cmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_callback_bretr(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }

  return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdbSend
//

int
vscp_link_doCmdbSend(const void* pdata, const char* cmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_callback_bsend(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }

  return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdbRcvLoop
//

int
vscp_link_doCmdbRcvLoop(const void* pdata, const char* cmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_callback_restart(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }

  return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
}

///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdSec
//

int
vscp_link_doCmdSec(const void* pdata, const char* cmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_callback_sec(pdata)) {
    return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_ERROR);
  }

  return vscp_link_callback_write_client(pdata, VSCP_LINK_MSG_OK);
}