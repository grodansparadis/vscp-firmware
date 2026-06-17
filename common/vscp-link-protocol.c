// FILE: vscp-link-protocol.c
/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol)
 * 	https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2000-2026 Ake Hedman, the VSCP Project
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
#include "vscp.h"
#include <vscp-firmware-helper.h>
#include "vscp-link-protocol.h"
// ---------------------------------------------------------------------------
// Internal helpers: validate context before use.
// ---------------------------------------------------------------------------
// Checks pctx, pctx->ops, and the mandatory write_client callback.
static inline int
vscp_link_check_ctx(const vscp_link_ctx_t *pctx)
{
  if (NULL == pctx || NULL == pctx->ops || NULL == pctx->ops->write_client) {
    return VSCP_ERROR_INIT_MISSING;
  }
  return VSCP_ERROR_SUCCESS;
}
// Also checks check_authenticated and check_privilege, required by most commands.
static inline int
vscp_link_check_ctx_auth(const vscp_link_ctx_t *pctx)
{
  if (NULL == pctx || NULL == pctx->ops || NULL == pctx->ops->write_client || NULL == pctx->ops->check_authenticated ||
      NULL == pctx->ops->check_privilege) {
    return VSCP_ERROR_INIT_MISSING;
  }
  return VSCP_ERROR_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////
//                          Command handlers
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdNoop
//
static int
vscp_link_doCmdNoop(vscp_link_ctx_t *pctx, const char *pcmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx(pctx)) {
    return VSCP_ERROR_INIT_MISSING;
  }
  pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
  return VSCP_ERROR_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdHelp
//
static int
vscp_link_doCmdHelp(vscp_link_ctx_t *pctx, const char *pcmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx(pctx)) {
    return VSCP_ERROR_INIT_MISSING;
  }
#ifdef VSCP_LINK_CUSTOM_HELP_TEXT
  // Need to be initialized
  if (NULL == pctx->ops->help_custom) {
    return VSCP_ERROR_INIT_MISSING;
  }
  return pctx->ops->help_custom(pctx, cmd);
#else
  if (NULL != vscp_fwhlp_stristr(pcmd, "noop")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_NOOP);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "help")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_HELP);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "quit")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_QUIT);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "user")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_USER);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "pass")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_PASS);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "challenge")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_CHALLENGE);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "send")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_SEND);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "retr")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_RETR);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "rcvloop")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_RCVLOOP);
  }
  else if (NULL != strstr(pcmd, "quitloop")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_QUITLOOP);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "cdta") || NULL != strstr(pcmd, "chkdata")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_CDTA);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "clra") || NULL != strstr(pcmd, "clrall")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_CLRA);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "stat")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_STAT);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "info")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_INFO);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "chid")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_CHID);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "sgid") || NULL != strstr(pcmd, "setguid")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_SGID);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "ggid") || NULL != strstr(pcmd, "getguid")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_GGID);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "vers") || NULL != strstr(pcmd, "version")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_VERS);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "sflt") || NULL != strstr(pcmd, "setfilter")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_SFLT);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "smsk") || NULL != strstr(pcmd, "setmask")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_SMSK);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "test")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_TEST);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "wcyd") || NULL != strstr(pcmd, "whatcanyoudo")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_WCYD);
  }
  else if (NULL != vscp_fwhlp_stristr(pcmd, "interface")) {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_INTERFACE);
  }
  else {
    return pctx->ops->write_client(pctx, VSCP_LINK_STD_HELP_TEXT);
  }
#endif
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdQuit
//
static int
vscp_link_doCmdQuit(vscp_link_ctx_t *pctx, const char *pcmd)
{
  // Need to be initialized
  if ((NULL == pctx) || (NULL == pctx->ops) || (NULL == pctx->ops->quit)) {
    return VSCP_ERROR_INIT_MISSING;
  }
  // VSCP_LINK_MSG_GOODBY
  return pctx->ops->quit(pctx);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdUser
//
static int
vscp_link_doCmdUser(vscp_link_ctx_t *pctx, const char *pcmd)
{
  // Need to be initialized
  if ((NULL == pctx) || (NULL == pctx->ops) || (NULL == pctx->ops->check_user)) {
    return VSCP_ERROR_INIT_MISSING;
  }

  // pcmd can be NULL and be accepted or not
  return pctx->ops->check_user(pctx, pcmd);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdPassword
//
static int
vscp_link_doCmdPassword(vscp_link_ctx_t *pctx, const char *pcmd)
{
  // Need to be initialized
  if ((NULL == pctx) || (NULL == pctx->ops) || (NULL == pctx->ops->check_password)) {
    return VSCP_ERROR_INIT_MISSING;
  }
  // ppwd can be NULL and be accepted or not
  return pctx->ops->check_password(pctx, pcmd);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdChallenge
//
static int
vscp_link_doCmdChallenge(vscp_link_ctx_t *pctx, const char *pcmd)
{
  // Need to be initialized
  if ((NULL == pctx) || (NULL == pctx->ops) || (NULL == pctx->ops->challenge)) {
    return VSCP_ERROR_INIT_MISSING;
  }
  return pctx->ops->challenge(pctx, pcmd);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdSend
//
static int
vscp_link_doCmdSend(vscp_link_ctx_t *pctx, const char *pcmd)
{
  int rv;
  vscpEvent *pev;
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->send) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_authenticated(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_privilege(pctx, 4)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  pev = vscp_fwhlp_newEvent();
  if (VSCP_ERROR_SUCCESS != (rv = vscp_fwhlp_parseStringToEvent(pev, pcmd))) {
    if (VSCP_ERROR_INVALID_POINTER == rv) {
      vscp_fwhlp_deleteEvent(&pev);
      return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
    }
    else if (VSCP_ERROR_MEMORY == rv) {
      vscp_fwhlp_deleteEvent(&pev);
      return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
    }
    else {
      vscp_fwhlp_deleteEvent(&pev);
      return pctx->ops->write_client(pctx, VSCP_LINK_MSG_PARAMETER_ERROR);
    }
  }
  if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->send(pctx, pev))) {
    if (VSCP_ERROR_INVALID_POINTER == rv) {
      vscp_fwhlp_deleteEvent(&pev);
      return pctx->ops->write_client(pctx, VSCP_LINK_MSG_PARAMETER_ERROR);
    }
    else if (VSCP_ERROR_TRM_FULL == rv) {
      vscp_fwhlp_deleteEvent(&pev);
      return pctx->ops->write_client(pctx, VSCP_LINK_MSG_BUFFER_FULL);
    }
    else {
      vscp_fwhlp_deleteEvent(&pev);
      return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
    }
  }
  return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdRetrive
//
static int
vscp_link_doCmdRetrieve(vscp_link_ctx_t *pctx, const char *pcmd)
{
  int rv;
  char buf[THIS_FIRMWARE_TCPIP_LINK_MAX_BUFFER];
  vscpEvent *pev = NULL;
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->retr) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_authenticated(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_privilege(pctx, 2)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  int cnt = vscp_fwhlp_readStringValue(pcmd);
  if (cnt <= 0) {
    cnt = 1;
  }
  while (VSCP_ERROR_SUCCESS == (rv = pctx->ops->retr(pctx, &pev))) {
    // We own the event from now on and must
    // delete it and it's data when we are done
    // with it
    if (VSCP_ERROR_SUCCESS == (rv = vscp_fwhlp_eventToString(buf, sizeof(buf), pev))) {
      strcat(buf, "\r\n");
      pctx->ops->write_client(pctx, buf);
    }
    else {
      return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
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
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
  }
  else if (VSCP_ERROR_RCV_EMPTY == rv) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NO_MSG);
  }
  else {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
  }
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdRcvLoop
//
static int
vscp_link_doCmdRcvLoop(vscp_link_ctx_t *pctx, const char *pcmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->enable_rcvloop) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_authenticated(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_privilege(pctx, 2)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  if (VSCP_ERROR_SUCCESS == pctx->ops->enable_rcvloop(pctx, 1)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
  }
  else {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
  }
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdQuitLoop
//
static int
vscp_link_doCmdQuitLoop(vscp_link_ctx_t *pctx, const char *pcmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->enable_rcvloop) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_authenticated(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_privilege(pctx, 2)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  if (VSCP_ERROR_SUCCESS == pctx->ops->enable_rcvloop(pctx, 0)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
  }
  else {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
  }
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdCmdCheckData
//
static int
vscp_link_doCmdCheckData(vscp_link_ctx_t *pctx, const char *pcmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->chkdata) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_authenticated(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_privilege(pctx, 1)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  uint16_t cnt;
  if (VSCP_ERROR_SUCCESS != pctx->ops->chkdata(pctx, &cnt)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
  }
  char buf[10];
  sprintf(buf, "%u\r\n", cnt);
  pctx->ops->write_client(pctx, buf);
  return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdClearAll
//
static int
vscp_link_doCmdClearAll(vscp_link_ctx_t *pctx, const char *pcmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->clrall) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_authenticated(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_privilege(pctx, 2)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  if (VSCP_ERROR_SUCCESS == pctx->ops->clrall(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
  }
  else {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
  }
  return VSCP_ERROR_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdStatistics
//
static int
vscp_link_doCmdStatistics(vscp_link_ctx_t *pctx, const char *pcmd)
{
  char buf[1000];
  vscp_statistics_t statistics;
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->statistics) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_authenticated(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_privilege(pctx, 2)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->statistics(pctx, &statistics)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
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
  return pctx->ops->write_client(pctx, buf);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdInfo
//
static int
vscp_link_doCmdInfo(vscp_link_ctx_t *pctx, const char *pcmd)
{
  char buf[1000];
  vscp_status_t status;
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->info) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_authenticated(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_privilege(pctx, 2)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->info(pctx, &status)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  snprintf(buf,
           sizeof(buf),
           "%lu,%lu,%lu,\"%s\"\r\nOK\r\n",
           status.channel_status,
           status.lasterrorcode,
           status.lasterrorsubcode,
           status.lasterrorstr);
  return pctx->ops->write_client(pctx, buf);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdGetChannelId
//
static int
vscp_link_doCmdGetChannelId(vscp_link_ctx_t *pctx, const char *pcmd)
{
  uint16_t chid;
  char buf[10];
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->get_channel_id) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_authenticated(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_privilege(pctx, 2)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->get_channel_id(pctx, &chid)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
  }
  sprintf(buf, "%u\r\n", chid);
  pctx->ops->write_client(pctx, buf);
  return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdSetGUID
//
static int
vscp_link_doCmdSetGUID(vscp_link_ctx_t *pctx, const char *pcmd)
{
  int rv;
  char *endptr;
  uint8_t guid[16];
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->set_guid) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_authenticated(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_privilege(pctx, 6)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  // Must be a parameter (guid)
  if (0 == *pcmd) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_PARAMETER_ERROR);
  }
  if (VSCP_ERROR_SUCCESS != vscp_fwhlp_parseGuid(guid, pcmd, &endptr)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
  }
  if (VSCP_ERROR_SUCCESS == (rv = pctx->ops->set_guid(pctx, guid))) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_PARAMETER_ERROR);
  }
  else if (VSCP_ERROR_NOT_SUPPORTED == rv) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_SUPPORTED);
  }
  else {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
  }
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdGetGUID
//
static int
vscp_link_doCmdGetGUID(vscp_link_ctx_t *pctx, const char *pcmd)
{
  uint8_t guid[16];
  char strguid[50];
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->get_guid) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_authenticated(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_privilege(pctx, 1)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->get_guid(pctx, guid)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
  }
  if (VSCP_ERROR_SUCCESS != vscp_fwhlp_writeGuidToString(strguid, guid)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
  }
  strcat(strguid, "\r\n");
  pctx->ops->write_client(pctx, strguid);
  return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdGetVersion
//
static int
vscp_link_doCmdGetVersion(vscp_link_ctx_t *pctx, const char *pcmd)
{
  uint8_t version[4];
  char strversion[50];
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->get_version) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->get_version(pctx, version)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
  }
  sprintf(strversion, "%d,%d,%d,%d\r\n", version[0], version[1], version[2], version[3]);
  pctx->ops->write_client(pctx, strversion);
  return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdSetFilter
//
static int
vscp_link_doCmdSetFilter(vscp_link_ctx_t *pctx, const char *pcmd)
{
  vscpEventFilter filter;
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->set_filter) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_authenticated(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_privilege(pctx, 6)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  // Must be a parameter (guid)
  if (0 == *pcmd) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_PARAMETER_ERROR);
  }
  if (VSCP_ERROR_SUCCESS != vscp_fwhlp_parseFilter(&filter, pcmd)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_PARAMETER_ERROR);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->set_filter(pctx, &filter)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
  }
  return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdSetMask
//
static int
vscp_link_doCmdSetMask(vscp_link_ctx_t *pctx, const char *pcmd)
{
  vscpEventFilter filter;
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->set_mask) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_authenticated(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_privilege(pctx, 6)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  // Must be a parameter (guid)
  if (0 == *pcmd) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_PARAMETER_ERROR);
  }
  if (VSCP_ERROR_SUCCESS != vscp_fwhlp_parseMask(&filter, pcmd)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_PARAMETER_ERROR);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->set_mask(pctx, &filter)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
  }
  return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdTest
//
static int
vscp_link_doCmdTest(vscp_link_ctx_t *pctx, const char *pcmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->test) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_authenticated(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_privilege(pctx, 15)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  return pctx->ops->test(pctx, pcmd);
}
///////////////////////////////////////////////////////////////////////////////
// doCmdInterface
//
// args
//   'list' - List all interfaces
//   'close <guid>' - Close interface <guid>
//
static int
vscp_link_doCmdInterface(vscp_link_ctx_t *pctx, const char *pcmd)
{
  int rv;
  char buf[130], wbuf[50];
  struct vscp_interface_info ifinfo;
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->close_interface ||
      NULL == pctx->ops->get_interface_count || NULL == pctx->ops->get_interface) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_authenticated(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_privilege(pctx, 2)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  // interface close <guid>
  const char *p = pcmd;
  size_t len    = strlen(pcmd);
  if (!len) {
    // continue "interface" command without arguments
  }
  else if (len && (NULL != (p = vscp_fwhlp_stristr(pcmd, "list")))) {
    p += 4;
    // Continue ."list interfaces"
  }
  else if (len && (NULL != (p = vscp_fwhlp_stristr(pcmd, "close")))) {
    uint8_t guid[16];
    memset(guid, 0, sizeof(guid));
    // Find argument (guid)
    p += 5;
    if (!(len = strlen(p))) {
      return pctx->ops->write_client(pctx, VSCP_LINK_MSG_PARAMETER_ERROR);
    }
    if (VSCP_ERROR_SUCCESS != vscp_fwhlp_parseGuid(guid, p, NULL)) {
      return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
    }
    if (VSCP_ERROR_SUCCESS == (rv = pctx->ops->close_interface(pctx, guid))) {
      return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
    }
    else if (VSCP_ERROR_NOT_SUPPORTED == rv) {
      return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_SUPPORTED);
    }
    else {
      return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
    }
  }
  else {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_PARAMETER_ERROR);
  }
  uint16_t rows = pctx->ops->get_interface_count(pctx);
  snprintf(buf, sizeof(buf), "%u rows\r\n", rows);
  pctx->ops->write_client(pctx, buf);
  for (uint16_t i = 0; i < rows; i++) {
    memset(buf, 0, sizeof(buf));
    if (VSCP_ERROR_SUCCESS != pctx->ops->get_interface(pctx, i, &ifinfo)) {
      return VSCP_ERROR_ERROR;
    }
    if (VSCP_ERROR_SUCCESS != vscp_fwhlp_writeGuidToString(wbuf, ifinfo.guid)) {
      return VSCP_ERROR_ERROR;
    }
    snprintf(buf, sizeof(buf), "%u,%u,%s,%s\r\n", ifinfo.idx, ifinfo.type, wbuf, ifinfo.description);
    pctx->ops->write_client(pctx, buf);
  }
  return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdWhatCanYouDo
//
static int
vscp_link_doCmdWhatCanYouDo(vscp_link_ctx_t *pctx, const char *pcmd)
{
  uint64_t wcyd;
  char buf[60];
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->wcyd) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->wcyd(pctx, &wcyd)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
  }
  memset(buf, '\0', sizeof(buf));
  // sprintf(buf, "%llu\r\n", wcyd);
  snprintf(buf,
           sizeof(buf),
           "%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X %llu\r\n",
           (uint8_t) ((wcyd >> 56) & 0xff),
           (uint8_t) ((wcyd >> 48) & 0xff),
           (uint8_t) ((wcyd >> 40) & 0xff),
           (uint8_t) ((wcyd >> 32) & 0xff),
           (uint8_t) ((wcyd >> 24) & 0xff),
           (uint8_t) ((wcyd >> 16) & 0xff),
           (uint8_t) ((wcyd >> 8) & 0xff),
           (uint8_t) (wcyd & 0xff),
           wcyd);
  pctx->ops->write_client(pctx, buf);
  return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdCommandAgain
//
static int
vscp_link_doCmdCommandAgain(vscp_link_ctx_t *pctx, const char *pcmd)
{
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx(pctx)) {
    return VSCP_ERROR_INIT_MISSING;
  }
  return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdCommandAgain
//
static int
vscp_link_doCmdShutdown(vscp_link_ctx_t *pctx, const char *pcmd)
{
  int rv;
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->shutdown) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_authenticated(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_privilege(pctx, 15)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->shutdown(pctx))) {
    if (VSCP_ERROR_NOT_SUPPORTED == rv) {
      return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_SUPPORTED);
    }
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
  }
  return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doCmdRestart
//
static int
vscp_link_doCmdRestart(vscp_link_ctx_t *pctx, const char *pcmd)
{
  int rv;

  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx_auth(pctx) || NULL == pctx->ops->restart) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_authenticated(pctx)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_ACCREDITED);
  }
  if (VSCP_ERROR_SUCCESS != pctx->ops->check_privilege(pctx, 15)) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR);
  }
  if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->restart(pctx))) {
    if (VSCP_ERROR_NOT_SUPPORTED == rv) {
      return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_SUPPORTED);
    }
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
  }
  return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
}
// ----------------------------------------------------------------------------
//                                   Binary
// ----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
// vscp_link_doBinary
//
static int
vscp_link_doBinary(vscp_link_ctx_t *pctx)
{
  int rv;

  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx(pctx)) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (NULL == pctx->ops->binary) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_BINARY_NACK);
  }
  if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->binary(pctx))) {
    if (VSCP_ERROR_NOT_SUPPORTED == rv) {
      return pctx->ops->write_client(pctx, VSCP_LINK_MSG_NOT_SUPPORTED);
    }
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_ERROR);
  }
  return pctx->ops->write_client(pctx, VSCP_LINK_MSG_BINARY_ACK);
}
// ----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
// vscp_link_connect
//
int
vscp_link_connect(vscp_link_ctx_t *pctx)
{
  if (NULL == pctx || NULL == pctx->ops || NULL == pctx->ops->welcome) {
    return VSCP_ERROR_INIT_MISSING;
  }
  return pctx->ops->welcome(pctx);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_disconnect
//
int
vscp_link_disconnect(vscp_link_ctx_t *pctx)
{
  if (NULL == pctx || NULL == pctx->ops || NULL == pctx->ops->disconnect) {
    return VSCP_ERROR_INIT_MISSING;
  }
  // We handle this the same way as a disconnect
  // from a user
  return pctx->ops->disconnect(pctx);
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_idle_worker
//
int
vscp_link_idle_worker(vscp_link_ctx_t *pctx)
{
  int rv;
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx(pctx) || NULL == pctx->ops->get_rcvloop_status ||
      NULL == pctx->ops->rcvloop) {
    return VSCP_ERROR_INIT_MISSING;
  }
  // Get receive loop status
  int bRcvLoop = 0;
  if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->get_rcvloop_status(pctx, &bRcvLoop))) {
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
    vscpEvent *pev = NULL;
    while (VSCP_ERROR_SUCCESS == (rv = pctx->ops->rcvloop(pctx, &pev))) {
      // We own the event from now on and must
      // delete it and it's data when we are done with it
      // Convert event to string
      rv = vscp_fwhlp_eventToString(buf, sizeof(buf), pev);
      // We are donw with the event, free it
      vscp_fwhlp_deleteEvent(&pev);
      // If the conversion was OK write out the event data
      if (VSCP_ERROR_SUCCESS == rv) {
        // Write out the event
        pctx->ops->write_client(pctx, buf);
        // Timeout - Write out '\r\n'
        pctx->ops->write_client(pctx, "\r\n");
      }
    }
    // Write out '+OK\r\n' periodically
    if (VSCP_ERROR_TIMEOUT == rv) {
      // Timeout - Write out '+OK\r\n'
      pctx->ops->write_client(pctx, "+OK\r\n");
    }
  } // rcvloop
  return VSCP_ERROR_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////
// vscp_link_parser
//

int
vscp_link_parser(vscp_link_ctx_t *pctx, char *pbuf)
{
  char *p;
  int rv;
  // Need to be initialized
  if (VSCP_ERROR_SUCCESS != vscp_link_check_ctx(pctx)) {
    return VSCP_ERROR_INIT_MISSING;
  }

  // Remove leading whitespace from command
  while (isspace((unsigned char) *pbuf)) {
    pbuf++;
  }

  // Remove trailing whitespace from command
  {
    size_t slen = strlen(pbuf);
    if (slen > 0) {
      char *end = pbuf + slen - 1;
      while (end > pbuf && isspace((unsigned char) *end)) {
        end--;
      }
      *(end + 1) = '\0';
    }
  }

  // Just CRLF (or all-whitespace line)
  if ('\0' == *pbuf) {
    return pctx->ops->write_client(pctx, VSCP_LINK_MSG_OK);
  }

  // Get receive loop status
  int bRcvLoop = 0;
  if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->get_rcvloop_status(pctx, &bRcvLoop))) {
    return rv;
  }

  size_t cmdlen = strlen(pbuf);
  if (NULL != (p = vscp_fwhlp_stristr(pbuf, "help")) && (4 == cmdlen) &&
      (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdHelp(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "noop")) && (4 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdNoop(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "user ")) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 5;
    return vscp_link_doCmdUser(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "pass ")) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 5;
    return vscp_link_doCmdPassword(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "challenge")) && (9 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 9;
    return vscp_link_doCmdChallenge(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "send ")) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 5;
    return vscp_link_doCmdSend(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "retr")) /*&& (4 == cmdlen)*/ &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdRetrieve(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "rcvloop")) && (7 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdRcvLoop(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "quitloop")) && (8 == cmdlen)) {
    p += 8;
    return vscp_link_doCmdQuitLoop(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "quit")) && (4 == cmdlen)) {
    p += 4;
    return vscp_link_doCmdQuit(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "cdta")) && (4 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdCheckData(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "chkdata")) && (7 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdCheckData(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "clra")) && (4 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdClearAll(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "clrall")) && (6 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 6;
    return vscp_link_doCmdClearAll(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "stat")) && (4 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdStatistics(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "info")) && (4 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdInfo(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "chid")) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdGetChannelId(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "getchid")) && (7 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdGetChannelId(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "sgid")) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdSetGUID(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "setguid")) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdSetGUID(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "ggid")) && (4 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdGetGUID(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "getguid")) && (7 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdGetGUID(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "vers")) && (4 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdGetVersion(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "version")) && (7 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdGetVersion(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "sflt")) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdSetFilter(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "setfilter")) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 9;
    return vscp_link_doCmdSetFilter(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "smsk")) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdSetMask(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "setmask")) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdSetMask(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "test")) && (4 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdTest(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "wcyd")) && (4 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 4;
    return vscp_link_doCmdWhatCanYouDo(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "whatcanyoudo")) && (12 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 12;
    return vscp_link_doCmdWhatCanYouDo(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "+")) && (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 1;
    return vscp_link_doCmdCommandAgain(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "interface")) /*&& (9 == cmdlen)*/ &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 9;
    return vscp_link_doCmdInterface(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "shutdown")) && (8 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 8;
    return vscp_link_doCmdShutdown(pctx, p);
  }
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "restart")) && (7 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 7;
    return vscp_link_doCmdRestart(pctx, p);
  }
  // Binary commands
  else if (NULL != (p = vscp_fwhlp_stristr(pbuf, "binary")) && (6 == cmdlen) &&
           (THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD || !bRcvLoop)) {
    p += 6;
    return vscp_link_doBinary(pctx);
  }

  // Unknown command
  pctx->ops->write_client(pctx, VSCP_LINK_MSG_UNKNOWN_COMMAND);
  return VSCP_ERROR_MISSING;
}
