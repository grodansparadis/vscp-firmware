/* ******************************************************************************
 * VSCP (Very Simple Control Protocol)
 * http://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (C) 2000-2026 Ake Hedman,
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vscp-compiler.h"
#include "vscp-projdefs.h"

#include "vscp.h"

#include "vscp-ws2-protocol.h"

///////////////////////////////////////////////////////////////////////////////
// vscp_ws2_check_ctx
//

static inline int
vscp_ws2_check_ctx(const vscp_ws2_ctx_t *pctx)
{
  if ((NULL == pctx) || (NULL == pctx->ops) || (NULL == pctx->ops->reply)) {
    return VSCP_ERROR_INIT_MISSING;
  }
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_ws2_init
//

int
vscp_ws2_init(vscp_ws2_ctx_t *pctx, void *pdata)
{
  if ((NULL == pctx) || (NULL == pctx->ops) ||
      (NULL == pctx->ops->reply) || (NULL == pctx->ops->generate_sid)) {
    return VSCP_ERROR_INIT_MISSING;
  }

  const vscp_ws2_ops_t *saved_ops = pctx->ops;
  int rv;
  char buf[256];

  memset(pctx, 0, sizeof(vscp_ws2_ctx_t));
  pctx->ops   = saved_ops;
  pctx->pdata = pdata;

  if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->generate_sid(pctx, pctx->sid, sizeof(pctx->sid)))) {
    return rv;
  }

  /* Send AUTH0 challenge as JSON */
  snprintf(buf,
           sizeof(buf),
           "{\"type\":\"AUTH0\",\"sid\":\"%02X%02X%02X%02X%02X%02X%02X%02X"
           "%02X%02X%02X%02X%02X%02X%02X%02X\"}",
           pctx->sid[0], pctx->sid[1], pctx->sid[2],  pctx->sid[3],
           pctx->sid[4], pctx->sid[5], pctx->sid[6],  pctx->sid[7],
           pctx->sid[8], pctx->sid[9], pctx->sid[10], pctx->sid[11],
           pctx->sid[12], pctx->sid[13], pctx->sid[14], pctx->sid[15]);
  if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->reply(pctx, buf))) {
    return rv;
  }

  if ((NULL != pctx->ops->init) &&
      (VSCP_ERROR_SUCCESS != (rv = pctx->ops->init(pctx)))) {
    return rv;
  }

  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_ws2_clearup
//

int
vscp_ws2_clearup(vscp_ws2_ctx_t *pctx, void *pdata)
{
  if (VSCP_ERROR_SUCCESS != vscp_ws2_check_ctx(pctx)) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if (NULL != pctx->ops->cleanup) {
    int rv;
    if (VSCP_ERROR_SUCCESS != (rv = pctx->ops->cleanup(pctx))) {
      return rv;
    }
  }
  return VSCP_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_ws2_handle_request
//
// WS2 frames are JSON objects with a mandatory "type" field, e.g.:
//   {"type":"COMMAND","command":"NOOP"}
//   {"type":"COMMAND","command":"AUTH","data":{"sid":"...","crypto":"..."}}
//   {"type":"EVENT","event":{...}}
//

int
vscp_ws2_handle_request(vscp_ws2_ctx_t *pctx, const char *packet, size_t len)
{
  if (VSCP_ERROR_SUCCESS != vscp_ws2_check_ctx(pctx)) {
    return VSCP_ERROR_INIT_MISSING;
  }
  if ((NULL == packet) || (0 == len)) {
    return VSCP_ERROR_INVALID_FRAME;
  }

  /* Stub: full JSON parsing requires a JSON library (e.g. cJSON).
   * Return NOT_SUPPORTED until the JSON parsing layer is implemented. */
  (void) len;
  return pctx->ops->reply(pctx, "{\"type\":\"ERROR\",\"code\":1,\"msg\":\"Not implemented\"}");
}
