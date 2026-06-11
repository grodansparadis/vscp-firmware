# vscp-ws1 — VSCP WebSocket Protocol WS1

**Date:** 2026-06-11  
**Module:** `common/vscp-ws1.h`, `common/vscp-ws1.c`  
**Spec:** [VSCP Websocket WS1](https://grodansparadis.github.io/vscp-doc-spec/#/./vscp_websocket?id=ws1-description)

---

## Overview

The WS1 module implements the VSCP text+binary WebSocket (WS1) protocol server side.
Each connected WebSocket client gets its own `vscp_ws1_ctx_t` context.

The module uses an **ops-table** pattern: all platform- and application-specific behaviour is
provided through a `vscp_ws1_ops_t` struct of function pointers. There are no link-time
callback symbols — the same object file can serve multiple connections with different
implementations simultaneously.

---

## Integration Flow

```
1. Allocate vscp_ws1_ctx_t (zeroed).
2. Set ctx.ops = &my_ops.
3. Call vscp_ws1_init(&ctx, pdata)
     → validates mandatory ops
     → generates SID via ops->generate_sid()
     → sends AUTH0 challenge via ops->reply()
     → calls ops->init() if set
4. On incoming text frame: vscp_ws1_handle_text_protocol_request(&ctx, frame, len)
5. On incoming binary frame: vscp_ws1_handle_binary_protocol_request(&ctx, frame, len)
6. On disconnect: vscp_ws1_clearup(&ctx, pdata)
```

---

## Context and Ops

### `vscp_ws1_ctx_t`

| Field           | Type                   | Description                                  |
|-----------------|------------------------|----------------------------------------------|
| `ops`           | `const vscp_ws1_ops_t*`| Ops table. Must be set before `vscp_ws1_init`. |
| `pdata`         | `void *`               | Application per-connection data.             |
| `bAuthenticated`| `bool`                 | Set `true` after successful `AUTH`.          |
| `bOpen`         | `bool`                 | Set `true` after `OPEN`, `false` after `CLOSE`. |
| `sid[16]`       | `uint8_t`              | Session ID / AES-128 IV.                     |
| `filter`        | `vscpEventFilter`      | Active event filter for this connection.     |
| `user`          | `vscp_ws_user_t`       | Authenticated user info (populated by `validate_user`). |
| `binary_ctx`    | `vscp_binary_ctx_t`    | Sub-context for binary protocol. Set `binary_ctx.ops` before using binary frames. |

---

## Ops Table (`vscp_ws1_ops_t`)

Mandatory ops — `vscp_ws1_init()` returns `VSCP_ERROR_INIT_MISSING` if NULL:

| Field           | Signature                                                      | Description                                 |
|-----------------|----------------------------------------------------------------|---------------------------------------------|
| `reply`         | `int (*)(ctx, const char *response)`                           | Send a text frame to the client.            |
| `generate_sid`  | `int (*)(ctx, uint8_t *sid, size_t size)`                      | Generate a 16-byte session ID.              |

Optional ops (set to `NULL` for default behaviour):

| Field               | Default when NULL                         | Description                                          |
|---------------------|-------------------------------------------|------------------------------------------------------|
| `init`              | no-op                                     | Called at end of `vscp_ws1_init()`.                 |
| `cleanup`           | no-op                                     | Called at start of `vscp_ws1_clearup()`.            |
| `get_primary_key`   | AUTH command fails                        | Return 16-byte AES key for credential decryption.   |
| `validate_user`     | AUTH command replies error                | Validate credentials; must populate `ctx->user`.    |
| `is_allowed_event`  | all events allowed                        | Per-event ACL check.                                |
| `is_allowed_connection` | all connections allowed             | Per-IP ACL check.                                   |
| `send_event`        | no-op                                     | Deliver an outbound event to the client.            |
| `copyright`         | sends `"-;COPYRIGHT;1,Not supported"`     | Handle `COPYRIGHT` command.                         |
| `open`              | sets `bOpen=true`, replies `"+;OPEN"`     | Handle `OPEN` command.                              |
| `close`             | sets `bOpen=false`, replies `"+;CLOSE"`   | Handle `CLOSE` command.                             |
| `setfilter`         | filter stored in `ctx->filter` only       | Persist or apply the parsed filter.                 |
| `clrqueue`          | replies `"+;CLRQ"` without action         | Clear the receive queue.                            |

---

## Minimal Integration Example

```c
#include "vscp-ws1.h"

/* Application state stored per-connection */
typedef struct { int sock_fd; } my_conn_t;

static int my_reply(vscp_ws1_ctx_t *pctx, const char *response)
{
    my_conn_t *c = pctx->pdata;
    send(c->sock_fd, response, strlen(response), 0);
    return VSCP_ERROR_SUCCESS;
}

static int my_generate_sid(vscp_ws1_ctx_t *pctx, uint8_t *sid, size_t size)
{
    getrandom(sid, size, 0);
    return VSCP_ERROR_SUCCESS;
}

static const vscp_ws1_ops_t my_ops = {
    .reply        = my_reply,
    .generate_sid = my_generate_sid,
    /* all other fields NULL */
};

/* Called when a new WebSocket connection is accepted */
void on_ws_connect(int sock_fd)
{
    vscp_ws1_ctx_t *ctx = calloc(1, sizeof(vscp_ws1_ctx_t));
    my_conn_t      *c   = malloc(sizeof(my_conn_t));
    c->sock_fd = sock_fd;
    ctx->ops   = &my_ops;
    vscp_ws1_init(ctx, c);
    /* ... store ctx for later use ... */
}
```

---

## Text Frame Format

| Type | Format |
|------|--------|
| Command (client→server) | `C;<COMMAND>[;<arg>]` |
| Positive reply (server→client) | `+;<COMMAND>[;<data>]` |
| Negative reply (server→client) | `-;<COMMAND>;<errcode>,<errstr>` |
| Event (client→server) | `E;<head>,<class>,<type>,<obid>,<datetime>,<timestamp>,<GUID>,<data>` |

### Commands Handled

| Command | Alias | Auth required | Description |
|---------|-------|:---:|-------------|
| `NOOP`  | — | No | No-op. Replies `+;NOOP`. |
| `VERSION` | — | No | Returns protocol version. |
| `CHALLENGE` | — | No | Re-sends AUTH0 SID challenge. |
| `AUTH` | — | — | Authenticates using AES-128 encrypted credentials. |
| `OPEN` | — | Yes | Opens event stream (`bOpen=true`). |
| `CLOSE` | — | Yes | Closes event stream (`bOpen=false`). |
| `SETFILTER` | `SF` | Yes | Sets event filter; stores in `ctx->filter`. |
| `CLRQUEUE` | `CLRQ` | — | Clears receive queue. |
| `COPYRIGHT` | — | No | Returns copyright string. |

---

## Binary Frame Support

Binary frames follow the [VSCP Binary Protocol](vscp-binary.md). The WS1 binary handler
decrypts the frame (if encrypted), then dispatches to either the command handler or the
event path.

To enable binary command handling, set `ctx.binary_ctx.ops = &my_binary_ops` before the
session starts. Binary events are delivered via the same `binary_ctx`.

---

## Platform (ESP-IDF)

`vscp-ws1.c` was originally written for ESP-IDF. Logging uses `WS1_LOGI` / `WS1_LOGE`
macros that expand to `ESP_LOGI` / `ESP_LOGE` when `VSCP_WS1_PLATFORM_ESP_IDF` is
`#define`d, and to no-ops otherwise.

For desktop unit tests the `tests/esp_log.h` and `tests/esp_log_buffer.h` stubs
provide the required symbols.

---

## Error Codes

The module returns standard VSCP error codes (`vscp.h`):

| Code | Meaning |
|------|---------|
| `VSCP_ERROR_SUCCESS` | OK |
| `VSCP_ERROR_INIT_MISSING` | Mandatory ops pointer is NULL |
| `VSCP_ERROR_INVALID_FRAME` | Malformed or oversize input frame |
| `VSCP_ERROR_INVALID_SYNTAX` | Command argument parsing failed |
| `VSCP_ERROR_INVALID_FORMAT` | Hex decoding failed |
| `VSCP_ERROR_MEMORY` | Memory allocation failure |
