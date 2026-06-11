# vscp-ws2 — VSCP WebSocket Protocol WS2

**Date:** 2026-06-11  
**Module:** `common/vscp-ws2.h`, `common/vscp-ws2.c`  
**Spec:** [VSCP Websocket WS2](https://grodansparadis.github.io/vscp-doc-spec/#/./vscp_websocket?id=ws2-description)

---

## Overview

The WS2 module implements the VSCP JSON-based WebSocket (WS2) protocol server side.
WS2 is the second-generation VSCP WebSocket protocol; unlike WS1 (semicolon-delimited
text), every WS2 frame is a JSON object with a mandatory `"type"` field.

The module follows the same **ops-table** pattern as `vscp-ws1`, `vscp-link-protocol`,
`vscp-firmware-level2`, and `vscp-binary`: all application callbacks are provided through
a `vscp_ws2_ops_t` struct of function pointers attached to a `vscp_ws2_ctx_t` context.

> **Implementation status:** The `vscp_ws2_handle_request()` function is currently a
> stub — it replies with a JSON error until a full JSON-parsing layer is wired in
> (e.g. via cJSON).

---

## Integration Flow

```
1. Allocate vscp_ws2_ctx_t (zeroed).
2. Set ctx.ops = &my_ops.
3. Call vscp_ws2_init(&ctx, pdata)
     → validates mandatory ops
     → generates SID via ops->generate_sid()
     → sends AUTH0 JSON challenge via ops->reply()
     → calls ops->init() if set
4. On incoming WebSocket text frame: vscp_ws2_handle_request(&ctx, json, len)
5. On disconnect: vscp_ws2_clearup(&ctx, pdata)
```

---

## Context and Ops

### `vscp_ws2_ctx_t`

| Field           | Type                   | Description                                  |
|-----------------|------------------------|----------------------------------------------|
| `ops`           | `const vscp_ws2_ops_t*`| Ops table. Must be set before `vscp_ws2_init`. |
| `pdata`         | `void *`               | Application per-connection data.             |
| `bAuthenticated`| `bool`                 | Set `true` after successful `AUTH`.          |
| `bOpen`         | `bool`                 | Set `true` after `OPEN`, `false` after `CLOSE`. |
| `sid[16]`       | `uint8_t`              | Session ID / AES-128 IV.                     |
| `filter`        | `vscpEventFilter`      | Active event filter for this connection.     |
| `user`          | `vscp_ws_user_t`       | Authenticated user info (populated by `validate_user`). |

---

## Ops Table (`vscp_ws2_ops_t`)

Mandatory ops — `vscp_ws2_init()` returns `VSCP_ERROR_INIT_MISSING` if NULL:

| Field           | Signature                                          | Description                    |
|-----------------|----------------------------------------------------|--------------------------------|
| `reply`         | `int (*)(ctx, const char *json_response)`          | Send a JSON text frame.        |
| `generate_sid`  | `int (*)(ctx, uint8_t *sid, size_t size)`          | Generate a 16-byte session ID. |

Optional ops (set to `NULL` for default behaviour):

| Field                   | Default when NULL                         | Description                                        |
|-------------------------|-------------------------------------------|----------------------------------------------------|
| `init`                  | no-op                                     | Called at end of `vscp_ws2_init()`.               |
| `cleanup`               | no-op                                     | Called at start of `vscp_ws2_clearup()`.          |
| `get_primary_key`       | AUTH fails                                | Return 16-byte AES key.                           |
| `validate_user`         | AUTH replies error                        | Validate credentials; must populate `ctx->user`.  |
| `is_allowed_event`      | all events allowed                        | Per-event ACL check.                              |
| `is_allowed_connection` | all connections allowed                   | Per-IP ACL check.                                 |
| `send_event`            | no-op                                     | Deliver an outbound event to the client.          |
| `open`                  | sets `bOpen=true`                         | Handle `OPEN` command.                            |
| `close`                 | sets `bOpen=false`                        | Handle `CLOSE` command.                           |
| `setfilter`             | filter stored in `ctx->filter` only       | Persist or apply the parsed filter.               |
| `clrqueue`              | no queue action                           | Clear the receive queue.                          |

---

## Minimal Integration Example

```c
#include "vscp-ws2.h"

static int my_reply(vscp_ws2_ctx_t *pctx, const char *json)
{
    /* send json over WebSocket */
    return VSCP_ERROR_SUCCESS;
}

static int my_generate_sid(vscp_ws2_ctx_t *pctx, uint8_t *sid, size_t size)
{
    getrandom(sid, size, 0);
    return VSCP_ERROR_SUCCESS;
}

static const vscp_ws2_ops_t my_ops = {
    .reply        = my_reply,
    .generate_sid = my_generate_sid,
};

void on_ws2_connect(void)
{
    static vscp_ws2_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.ops = &my_ops;
    vscp_ws2_init(&ctx, NULL);
}
```

---

## AUTH0 Challenge Frame

`vscp_ws2_init()` sends the following JSON frame to the client immediately after
generating the session ID:

```json
{"type":"AUTH0","sid":"0102030405060708090A0B0C0D0E0F10"}
```

The `sid` value is 32 hex characters (16 bytes). The client uses this as the AES-128
IV when encrypting credentials for the `AUTH` command.

---

## Shared Types (`vscp-ws-common.h`)

Both WS1 and WS2 use `vscp_ws_user_t` from `vscp-ws-common.h` to represent the
authenticated user:

| Field             | Type         | Description                                |
|-------------------|--------------|--------------------------------------------|
| `username[64]`    | `char`       | Login name.                                |
| `fullname[64]`    | `char`       | Display name.                              |
| `rights`          | `uint64_t`   | 63-bit permission bitmask.                 |
| `allowed_remotes` | `uint8_t[16][16]` | Up to 16 IPv6 allowed remote addresses. |
| `events[16][2]`   | `uint8_t`    | Allowed (class, type) pairs.               |
| `note[256]`       | `char`       | Free-text note.                            |

---

## Error Codes

| Code | Meaning |
|------|---------|
| `VSCP_ERROR_SUCCESS` | OK |
| `VSCP_ERROR_INIT_MISSING` | Mandatory ops pointer is NULL |
| `VSCP_ERROR_INVALID_FRAME` | NULL or empty input |
