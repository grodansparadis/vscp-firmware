# vscp-link-protocol

Last updated: 2026-06-11

This module implements the VSCP Link text command protocol dispatcher for firmware targets. It parses CRLF-terminated lines, maps commands to handlers, and delegates all transport/application work through callbacks.

- Header: `common/vscp-link-protocol.h`
- Implementation: `common/vscp-link-protocol.c`

## Scope and model

`vscp-link-protocol` does not own sockets/UART/TCP sessions, user databases, event queues, or persistent state. It is a protocol layer that:

1. Accepts command input as text lines.
2. Validates basic syntax and command access conditions.
3. Calls callback hooks for authentication, privilege checks, queue access, filtering, and output.
4. Emits standard `+OK`/`-OK` responses and command payload lines.

Core entry points:

- `vscp_link_connect(const vscp_link_ctx_t *pctx)`
- `vscp_link_disconnect(const vscp_link_ctx_t *pctx)`
- `vscp_link_idle_worker(const vscp_link_ctx_t *pctx)`
- `vscp_link_parser(const vscp_link_ctx_t *pctx, char *pbuf, char **pnext)`

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

- Expects complete commands terminated by `"\r\n"`.
- Returns `VSCP_ERROR_MISSING` when no full command is available yet.
- Trims leading/trailing whitespace around command text.
- Empty line (`"\r\n"`) responds with `VSCP_LINK_MSG_OK`.
- Uses case-insensitive matching (`vscp_fwhlp_stristr`).
- Supports aliases like `cdta/chkdata`, `clra/clrall`, `vers/version`, `wcyd/whatcanyoudo`.
- Returns `VSCP_LINK_MSG_UNKNOWN_COMMAND` for unknown commands.

While receive loop mode is active, command acceptance is controlled by `THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD` and loop state (`vscp_link_callback_get_rcvloop_status`).

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

- Authentication: `vscp_link_callback_check_authenticated`
- Privilege level: `vscp_link_callback_check_privilege`

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

All platform-specific behavior is delegated through the `vscp_link_ops_t` operations table, which is set on `vscp_link_ctx_t.ops` at integration time. Callbacks are invoked as `pctx->ops->xxx(pctx, ...)`. Function pointer fields left NULL cause the library to return `VSCP_ERROR_INIT_MISSING` rather than crash.

Callback groups in `vscp_link_ops_t`:

- Transport output/lifecycle: `write_client`, `disconnect`, `quit`, `welcome`
- Auth: `check_user`, `check_password`, `challenge`, `check_authenticated`, `check_privilege`
- Events/queues: `send`, `retr`, `rcvloop`, `enable_rcvloop`, `get_rcvloop_status`, `chkdata`, `clrall`
- Channel/device metadata: `get_channel_id`, `set_guid`, `get_guid`, `get_version`, `statistics`, `info`, `wcyd`
- Filter/mask: `set_filter`, `set_mask`
- Interface management: `get_interface_count`, `get_interface`, `close_interface`
- Binary: `binary_`
- Misc: `help_custom`, `test`, `shutdown`, `restart`

The context struct (`vscp_link_ctx_t`) carries per-connection state including the ops pointer, socket, GUID, FIFO queues, authentication state, privilege level, filter, and a `void *user_data` field for transport-specific data.

### Internal context validation helpers

Two `static inline` helpers guard all public functions:

- `vscp_link_check_ctx(pctx)` — validates `pctx`, `pctx->ops`, and `ops->write_client`.
- `vscp_link_check_ctx_auth(pctx)` — above plus `ops->check_authenticated` and `ops->check_privilege`.

Every command handler calls one of these on entry, returning `VSCP_ERROR_INIT_MISSING` on any NULL before touching any pointer.

## Receive-loop idle behavior

`vscp_link_idle_worker` checks loop state and, when active:

1. Calls `vscp_link_callback_rcvloop` repeatedly.
2. Converts returned events with `vscp_fwhlp_eventToString`.
3. Writes event lines and CRLF to the client.
4. Sends periodic `+OK\r\n` on timeout (`VSCP_ERROR_TIMEOUT`).

This allows non-blocking command processing plus asynchronous event push/heartbeat.

## Message constants

`common/vscp-link-protocol.h` defines reusable protocol strings, including:

- Success/flow: `VSCP_LINK_MSG_OK`, `VSCP_LINK_MSG_GOODBY`, `VSCP_LINK_MSG_QUEUE_CLEARED`, `VSCP_LINK_MSG_RECEIVE_LOOP`
- Generic errors: `VSCP_LINK_MSG_ERROR`, `VSCP_LINK_MSG_UNKNOWN_COMMAND`, `VSCP_LINK_MSG_PARAMETER_ERROR`, `VSCP_LINK_MSG_NOT_SUPPORTED`
- Auth/errors: `VSCP_LINK_MSG_PASSWORD_ERROR`, `VSCP_LINK_MSG_NOT_ACCREDITED`, `VSCP_LINK_MSG_NEED_USERNAME`, `VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR`

The same header also includes standard help text macros (`VSCP_LINK_STD_HELP_*`) used when `VSCP_LINK_CUSTOM_HELP_TEXT` is not enabled.

## Minimal usage skeleton

```c
// On new client/session
vscp_link_connect(pctx);

// In input loop
char *next = NULL;
int rv = vscp_link_parser(pctx, rxBuffer, &next);
if (VSCP_ERROR_SUCCESS == rv) {
  // consume up to `next`
}

// Periodic task
vscp_link_idle_worker(pctx);

// On close
vscp_link_disconnect(pctx);
```

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

## Notes on current implementation behavior

This section documents behavior as implemented in `common/vscp-link-protocol.c` at the time of writing:

- `vscp_link_doCmdSetGUID` currently returns `VSCP_LINK_MSG_PARAMETER_ERROR` on callback success.
- `vscp_link_doCmdShutdown` currently returns `VSCP_LINK_MSG_NOT_SUPPORTED` when shutdown callback succeeds.
- `vscp_link_doCmdbRcvLoop` currently calls `vscp_link_callback_restart` (not `vscp_link_callback_brcvloop`).

If you rely on these commands, verify whether this is intended for your target branch before shipping.

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
