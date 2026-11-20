# vscp-link-protocol

Last updated: 2026-11-20

This module implements the VSCP Link text command protocol dispatcher for firmware targets. It parses command lines, maps commands to handlers, and delegates all transport/application work through callbacks.

- Header: `src/vscp-link-protocol.h`
- Implementation: `src/vscp-link-protocol.c`

## Scope and model

`vscp-link-protocol` does not own sockets/UART/TCP sessions, user databases, event queues, or persistent state. It is a protocol layer that:

1. Accepts command input as text lines.
2. Validates basic syntax and command access conditions.
3. Calls callback hooks for authentication, privilege checks, queue access, filtering, and output.
4. Emits standard `+OK`/`-OK` responses and command payload lines.

Core entry points:

- `vscp_link_connect(vscp_link_ctx_t *pctx)` — new client: sends the welcome (`ops->welcome`)
- `vscp_link_disconnect(vscp_link_ctx_t *pctx)` — calls `ops->disconnect`
- `vscp_link_idle_worker(vscp_link_ctx_t *pctx)` — call often; sends events to a client in a receive loop
- `vscp_link_parser(vscp_link_ctx_t *pctx, char *pbuf)` — handles one command line (modified in place)

## Typical integration flow

```mermaid
flowchart TD
  A[Transport accepts client] --> B[vscp_link_connect]
  B --> C[Read bytes into line buffer]
  C --> D[vscp_link_parser]
  D -->|command complete| E[Command handler]
  E --> F[Application callback(s)]
  F --> G[Write response via callback]
  C --> H[vscp_link_idle_worker periodic]
  H --> I[rcvloop push events or heartbeat]
  A --> J[Disconnect]
  J --> K[vscp_link_disconnect]
```

## Command parsing details

`vscp_link_parser` behavior:

- Takes one complete command line. Splitting the input into lines (at `\n`) is done by the transport; the line ending may be left in the buffer.
- Trims leading/trailing whitespace (including `\r\n`) around the command text.
- Empty line (`"\r\n"`) responds with `VSCP_LINK_MSG_OK`.
- Uses case-insensitive matching (`vscp_fwhlp_stristr`).
- Supports aliases like `cdta/chkdata`, `clra/clrall`, `vers/version`, `wcyd/whatcanyoudo`.
- Writes `VSCP_LINK_MSG_UNKNOWN_COMMAND` and returns `VSCP_ERROR_MISSING` for unknown commands.

While receive loop mode is active, command acceptance is controlled by `THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD` and loop state (`ops->get_rcvloop_status`).

> **Note:** Individual command handler functions (`vscp_link_doCmdNoop`, `vscp_link_doCmdSend`, etc.) are `static` implementation details of `vscp-link-protocol.c`. They are not part of the public API and are not declared in the header. All interaction happens through `vscp_link_parser`.

## Text commands and handlers

The parser dispatches to these handlers:

- Session/auth: `noop`, `help`, `quit`, `user`, `pass`, `challenge`
- Event I/O: `send`, `retr`, `rcvloop`, `quitloop`, `cdta/chkdata`, `clra/clrall`
- Device/channel: `stat`, `info`, `chid/getchid`, `sgid/setguid`, `ggid/getguid`, `vers/version`
- Filtering/control: `sflt/setfilter`, `smsk/setmask`, `test`, `wcyd/whatcanyoudo`, `+`, `interface`, `shutdown`, `restart`
- Binary variants: `binary`

## Authentication and privilege gates

Several handlers enforce access checks through callbacks:

- Authentication: `ops->check_authenticated`
- Privilege level: `ops->check_privilege`

Observed privilege requirements in this implementation:

- Level 1: `getguid`, `chkdata`
- Level 2: `retr`, `rcvloop/quitloop`, `clrall`, `stat`, `info`, `chid`, `interface`
- Level 4: `send`
- Level 6: `setguid`, `setfilter`, `setmask`
- Level 15: `test`, `shutdown`, `restart`

Unauthorized operations typically return:

- `VSCP_LINK_MSG_NOT_ACCREDITED` (not logged in)
- `VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR` (insufficient rights)

## Callback contract

All platform-specific behavior is delegated through the `vscp_link_ops_t` operations table, which is set on `vscp_link_ctx_t.ops` at integration time. Callbacks are invoked as `pctx->ops->xxx(pctx, ...)`. `write_client` is required by every command; `check_authenticated` and `check_privilege` by every command that needs a login. A command whose callback is NULL returns `VSCP_ERROR_INIT_MISSING` instead of calling it. `vscp_link_idle_worker` needs `get_rcvloop_status` and `rcvloop`.

Callback groups in `vscp_link_ops_t`:

- Transport output/lifecycle: `write_client`, `disconnect`, `quit`, `welcome`
- Auth: `check_user`, `check_password`, `challenge`, `check_authenticated`, `check_privilege`
- Events/queues: `send`, `retr`, `rcvloop`, `enable_rcvloop`, `get_rcvloop_status`, `chkdata`, `clrall`
- Channel/device metadata: `get_channel_id`, `set_guid`, `get_guid`, `get_version`, `statistics`, `info`, `wcyd`
- Filter/mask: `set_filter`, `set_mask`
- Interface management: `get_interface_count`, `get_interface`, `close_interface`
- Binary: `binary`
- Misc: `help_custom`, `test`, `shutdown`, `restart`

### Event ownership

- `ops->send(pctx, pev)` — an event sent by the client. On `VSCP_ERROR_SUCCESS` the callback owns `pev` and must free it (`vscp_fwhlp_deleteEvent()`) or pass it on to code that does. On an error the library frees it; return `VSCP_ERROR_TRM_FULL` for a full queue (the client gets `VSCP_LINK_MSG_BUFFER_FULL`)
- `ops->retr(pctx, &pev)` / `ops->rcvloop(pctx, &pev)` — the callback gives the library an event (from `vscp_fwhlp_newEvent()`/`vscp_fwhlp_mkEventCopy()`), which the library writes to the client and frees. Return `VSCP_ERROR_RCV_EMPTY` when there is none
- `ops->clrall` — free the queued events (`vscp_fifo_clear()` does)

The context struct (`vscp_link_ctx_t`) carries per-connection state including the ops pointer, socket, GUID, FIFO queues, authentication state, privilege level, filter, and a `void *user_data` field for transport-specific data.

### Internal context validation helpers

Two `static inline` helpers guard all public functions:

- `vscp_link_check_ctx(pctx)` — validates `pctx`, `pctx->ops`, and `ops->write_client`.
- `vscp_link_check_ctx_auth(pctx)` — above plus `ops->check_authenticated` and `ops->check_privilege`.

Every command handler calls one of these on entry, returning `VSCP_ERROR_INIT_MISSING` on any NULL before touching any pointer.

## Receive-loop idle behavior

`vscp_link_idle_worker` checks loop state and, when active:

1. Calls `ops->rcvloop` until it returns something else than `VSCP_ERROR_SUCCESS`.
2. Converts returned events with `vscp_fwhlp_eventToString`.
3. Writes event lines and CRLF to the client.
4. Sends `+OK\r\n` when `ops->rcvloop` returns `VSCP_ERROR_TIMEOUT` (normally once a second when there are no events).

This allows non-blocking command processing plus asynchronous event push/heartbeat.

## Message constants

`src/vscp-link-protocol.h` defines reusable protocol strings, including:

- Success/flow: `VSCP_LINK_MSG_OK`, `VSCP_LINK_MSG_GOODBY`, `VSCP_LINK_MSG_QUEUE_CLEARED`, `VSCP_LINK_MSG_RECEIVE_LOOP`
- Generic errors: `VSCP_LINK_MSG_ERROR`, `VSCP_LINK_MSG_UNKNOWN_COMMAND`, `VSCP_LINK_MSG_PARAMETER_ERROR`, `VSCP_LINK_MSG_NOT_SUPPORTED`
- Auth/errors: `VSCP_LINK_MSG_PASSWORD_ERROR`, `VSCP_LINK_MSG_NOT_ACCREDITED`, `VSCP_LINK_MSG_NEED_USERNAME`, `VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR`

The same header also includes standard help text macros (`VSCP_LINK_STD_HELP_*`) used when `VSCP_LINK_CUSTOM_HELP_TEXT` is not enabled.

## Minimal usage skeleton

```c
// On new client/session
vscp_fifo_init(&pctx->fifoEventsOut, 32);
vscp_link_connect(pctx);

// In input loop: one call per complete line
char *eol;
while (NULL != (eol = strchr(rxBuffer, '\n'))) {
  *eol = '\0';
  vscp_link_parser(pctx, rxBuffer);
  // remove the line from rxBuffer
}

// Often (for example after every receive with a short timeout)
vscp_link_idle_worker(pctx);

// On close
vscp_link_disconnect(pctx);
vscp_fifo_deinit(&pctx->fifoEventsOut);
```

Complete servers: `main/link_server.c` in [vscp-demo-esp32-wifi-blinky](https://github.com/grodansparadis/vscp-demo-esp32-wifi-blinky) (several clients, one task each) and `firmware/callbacks-link.c` in [vscp-demo-link-stm32f401-wiz-ip20-blinky](https://github.com/grodansparadis/vscp-demo-link-stm32f401-wiz-ip20-blinky).

## Quick command reference

| Command | Aliases | Access | Notes |
|---|---|---|---|
| `help` | — | `write_client` present | Writes help text; `ops->help_custom` used if `VSCP_LINK_CUSTOM_HELP_TEXT` |
| `noop` | — | `write_client` present | Returns `+OK` |
| `quit` | — | `ops->quit` present | Calls `ops->quit`, returns goodbye |
| `user <name>` | — | `ops->check_user` present | Calls `ops->check_user` |
| `pass <password>` | — | `ops->check_password` present | Calls `ops->check_password` |
| `challenge` | — | `ops->challenge` present | Calls `ops->challenge` |
| `send <event>` | — | authenticated + privilege 4 | `check_authenticated`, `check_privilege`, parse event, `ops->send` |
| `retr [count]` | — | authenticated + privilege 2 | `check_authenticated`, `check_privilege`, `ops->retr` |
| `rcvloop` | — | authenticated + privilege 2 | `check_authenticated`, `check_privilege`, `ops->enable_rcvloop(1)` |
| `quitloop` | — | authenticated + privilege 2 | `check_authenticated`, `check_privilege`, `ops->enable_rcvloop(0)` |
| `chkdata` | `cdta` | authenticated + privilege 1 | `check_authenticated`, `check_privilege`, `ops->chkdata` |
| `clrall` | `clra` | authenticated + privilege 2 | `check_authenticated`, `check_privilege`, `ops->clrall` |
| `stat` | — | authenticated + privilege 2 | `check_authenticated`, `check_privilege`, `ops->statistics` |
| `info` | — | authenticated + privilege 2 | `check_authenticated`, `check_privilege`, `ops->info` |
| `chid` | `getchid` | authenticated + privilege 2 | `check_authenticated`, `check_privilege`, `ops->get_channel_id` |
| `setguid <guid>` | `sgid` | authenticated + privilege 6 | `check_authenticated`, `check_privilege`, parse GUID, `ops->set_guid` |
| `getguid` | `ggid` | authenticated + privilege 1 | `check_authenticated`, `check_privilege`, `ops->get_guid` |
| `version` | `vers` | ops present (no auth gate) | `ops->get_version` |
| `setfilter <filter>` | `sflt` | authenticated + privilege 6 | `check_authenticated`, `check_privilege`, parse filter, `ops->set_filter` |
| `setmask <mask>` | `smsk` | authenticated + privilege 6 | `check_authenticated`, `check_privilege`, parse mask, `ops->set_mask` |
| `test [arg]` | — | authenticated + privilege 15 | `check_authenticated`, `check_privilege`, `ops->test` |
| `whatcanyoudo` | `wcyd` | ops present (no auth gate) | `ops->wcyd` |
| `+` | — | `write_client` present | Responds `+OK` |
| `interface [list\|close <guid>]` | — | authenticated + privilege 2 | `ops->get_interface_count`, `ops->get_interface`, `ops->close_interface` |
| `shutdown` | — | authenticated + privilege 15 | `ops->shutdown` |
| `restart` | — | authenticated + privilege 15 | `ops->restart` |
| `binary` | — | `write_client` present | `ops->binary` |

## Notes on command results

- `setguid` answers `+OK` when `ops->set_guid` succeeds and `VSCP_LINK_MSG_NOT_SUPPORTED` when it returns `VSCP_ERROR_NOT_SUPPORTED`.
- `shutdown` answers `+OK` on success and `VSCP_LINK_MSG_NOT_SUPPORTED` when `ops->shutdown` returns `VSCP_ERROR_NOT_SUPPORTED`.

## Ops table checklist (copy/paste)

Use this as an integration checklist for your target. All fields are in `vscp_link_ops_t`.

### Minimum for a usable text session

- [ ] `ops->welcome`
- [ ] `ops->quit`
- [ ] `ops->write_client`
- [ ] `ops->get_rcvloop_status`

### Authentication and authorization

- [ ] `ops->check_user`
- [ ] `ops->check_password`
- [ ] `ops->challenge`
- [ ] `ops->check_authenticated`
- [ ] `ops->check_privilege`

### Event queue operations (text protocol)

- [ ] `ops->send`
- [ ] `ops->retr`
- [ ] `ops->enable_rcvloop`
- [ ] `ops->rcvloop`
- [ ] `ops->chkdata`
- [ ] `ops->clrall`

### Device/channel metadata

- [ ] `ops->get_channel_id`
- [ ] `ops->get_guid`
- [ ] `ops->set_guid`
- [ ] `ops->get_version`
- [ ] `ops->statistics`
- [ ] `ops->info`
- [ ] `ops->wcyd`

### Filter/mask

- [ ] `ops->set_filter`
- [ ] `ops->set_mask`

### Interface command support

- [ ] `ops->get_interface_count`
- [ ] `ops->get_interface`
- [ ] `ops->close_interface` (if `interface close` is supported)

### Optional binary commands

- [ ] `ops->binary` (`binary`)


### Optional control hooks

- [ ] `ops->test` (`test`, privilege 15)
- [ ] `ops->shutdown` (`shutdown`, privilege 15)
- [ ] `ops->restart` (`restart`, privilege 15)
- [ ] `ops->help_custom` (only if using `VSCP_LINK_CUSTOM_HELP_TEXT`)

### Bring-up sanity checklist

- [ ] `help`, `noop`, and empty line (`\r\n`) return `+OK` flow correctly.
- [ ] unauthenticated `send/retr/chkdata` return `VSCP_LINK_MSG_NOT_ACCREDITED`.
- [ ] low-privilege account returns `VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR` where expected.
- [ ] `retr 1` returns event line + `+OK` when queue has data.
- [ ] `rcvloop` emits periodic keepalive (`+OK`) on timeout path.
