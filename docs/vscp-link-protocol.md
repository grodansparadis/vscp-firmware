# vscp-link-protocol

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

- `vscp_link_connect(const void *pdata)`
- `vscp_link_disconnect(const void *pdata)`
- `vscp_link_idle_worker(const void *pdata)`
- `vscp_link_parser(const void *pdata, char *pbuf, char **pnext)`

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

## Text commands and handlers

The parser dispatches to these handlers:

- Session/auth: `noop`, `help`, `quit`, `user`, `pass`, `challenge`
- Event I/O: `send`, `retr`, `rcvloop`, `quitloop`, `cdta/chkdata`, `clra/clrall`
- Device/channel: `stat`, `info`, `chid/getchid`, `sgid/setguid`, `ggid/getguid`, `vers/version`
- Filtering/control: `sflt/setfilter`, `smsk/setmask`, `test`, `wcyd/whatcanyoudo`, `+`, `interface`, `shutdown`, `restart`
- Binary variants: `bretr`, `bsend`, `brcvloop`, `sec`

## Authentication and privilege gates

Several handlers enforce access checks through callbacks:

- Authentication: `vscp_link_callback_check_authenticated`
- Privilege level: `vscp_link_callback_check_privilege`

Observed privilege requirements in this implementation:

- Level 1: `getguid`, `chkdata`
- Level 2: `retr`, `rcvloop/quitloop`, `clrall`, `stat`, `info`, `chid`, `interface`
- Level 4: `send`
- Level 6: `setguid`, `setfilter`, `setmask`, `test`

Unauthorized operations typically return:

- `VSCP_LINK_MSG_NOT_ACCREDITED` (not logged in)
- `VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR` (insufficient rights)

## Callback contract

All platform-specific behavior is delegated. Important callback groups:

- Transport output/lifecycle: `vscp_link_callback_write_client`, `vscp_link_callback_disconnect_client`, `vscp_link_callback_quit`, `vscp_link_callback_welcome`
- Auth: `vscp_link_callback_check_user`, `vscp_link_callback_check_password`, `vscp_link_callback_challenge`, `vscp_link_callback_check_authenticated`, `vscp_link_callback_check_privilege`
- Events/queues: `vscp_link_callback_send`, `vscp_link_callback_retr`, `vscp_link_callback_rcvloop`, `vscp_link_callback_chkData`, `vscp_link_callback_clrAll`
- Channel/device metadata: `vscp_link_callback_get_channel_id`, `vscp_link_callback_set_guid`, `vscp_link_callback_get_guid`, `vscp_link_callback_get_version`, `vscp_link_callback_statistics`, `vscp_link_callback_info`, `vscp_link_callback_wcyd`
- Interface management: `vscp_link_callback_get_interface_count`, `vscp_link_callback_get_interface`, `vscp_link_callback_close_interface`
- Binary/security hooks: `vscp_link_callback_bretr`, `vscp_link_callback_bsend`, `vscp_link_callback_brcvloop`, `vscp_link_callback_sec`

The `pdata` pointer is opaque user context supplied by the integration layer and passed through all callbacks.

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
vscp_link_connect(ctx);

// In input loop
char *next = NULL;
int rv = vscp_link_parser(ctx, rxBuffer, &next);
if (VSCP_ERROR_SUCCESS == rv) {
  // consume up to `next`
}

// Periodic task
vscp_link_idle_worker(ctx);

// On close
vscp_link_disconnect(ctx);
```

## Quick command reference

| Command | Aliases | Access | Primary callbacks/path |
|---|---|---|---|
| `help` | — | none | `vscp_link_doCmdHelp` → `vscp_link_callback_write_client` (or custom help callback) |
| `noop` | — | none | `vscp_link_doCmdNoop` → `vscp_link_callback_write_client` |
| `quit` | — | none | `vscp_link_doCmdQuit` → `vscp_link_callback_quit` |
| `user <name>` | — | none | `vscp_link_doCmdUser` → `vscp_link_callback_check_user` |
| `pass <password>` | — | none | `vscp_link_doCmdPassword` → `vscp_link_callback_check_password` |
| `challenge` | — | none | `vscp_link_doCmdChallenge` → `vscp_link_callback_challenge` |
| `send <event>` | — | authenticated + privilege 4 | `check_authenticated`, `check_privilege`, parse event, `vscp_link_callback_send` |
| `retr [count]` | — | authenticated + privilege 2 | `check_authenticated`, `check_privilege`, `vscp_link_callback_retr` |
| `rcvloop` | — | authenticated + privilege 2 | `check_authenticated`, `check_privilege`, `vscp_link_callback_enable_rcvloop(1)` |
| `quitloop` | — | authenticated + privilege 2 | `check_authenticated`, `check_privilege`, `vscp_link_callback_enable_rcvloop(0)` |
| `chkdata` | `cdta` | authenticated + privilege 1 | `check_authenticated`, `check_privilege`, `vscp_link_callback_chkData` |
| `clrall` | `clra` | authenticated + privilege 2 | `check_authenticated`, `check_privilege`, `vscp_link_callback_clrAll` |
| `stat` | — | authenticated + privilege 2 | `check_authenticated`, `check_privilege`, `vscp_link_callback_statistics` |
| `info` | — | authenticated + privilege 2 | `check_authenticated`, `check_privilege`, `vscp_link_callback_info` |
| `chid` | `getchid` | authenticated + privilege 2 | `check_authenticated`, `check_privilege`, `vscp_link_callback_get_channel_id` |
| `setguid <guid>` | `sgid` | authenticated + privilege 6 | `check_authenticated`, `check_privilege`, parse GUID, `vscp_link_callback_set_guid` |
| `getguid` | `ggid` | authenticated + privilege 1 | `check_authenticated`, `check_privilege`, `vscp_link_callback_get_guid` |
| `version` | `vers` | none (in current code path) | `vscp_link_doCmdGetVersion` → `vscp_link_callback_get_version` |
| `setfilter <filter>` | `sflt` | authenticated + privilege 6 | `check_authenticated`, `check_privilege`, parse filter, `vscp_link_callback_setFilter` |
| `setmask <mask>` | `smsk` | authenticated + privilege 6 | `check_authenticated`, `check_privilege`, parse mask, `vscp_link_callback_setMask` |
| `test [arg]` | — | authenticated + privilege 6 | `check_authenticated`, `check_privilege`, `vscp_link_callback_test` |
| `whatcanyoudo` | `wcyd` | none (in current code path) | `vscp_link_doCmdWhatCanYouDo` → `vscp_link_callback_wcyd` |
| `+` | — | none | `vscp_link_doCmdCommandAgain` (currently responds `+OK`) |
| `interface [list\|close <guid>]` | — | authenticated + privilege 2 | interface list/close callbacks |
| `shutdown` | — | none (handler callback decides) | `vscp_link_doCmdShutdown` → `vscp_link_callback_shutdown` |
| `restart` | — | none (handler callback decides) | `vscp_link_doCmdRestart` → `vscp_link_callback_restart` |
| `bretr` | — | none (binary callback path) | `vscp_link_doCmdbRetr` → `vscp_link_callback_bretr` |
| `bsend` | — | none (binary callback path) | `vscp_link_doCmdbSend` → `vscp_link_callback_bsend` |
| `brcvloop` | — | none (binary callback path) | `vscp_link_doCmdbRcvLoop` (see behavior note below) |
| `sec` | — | none (binary/security callback path) | `vscp_link_doCmdSec` → `vscp_link_callback_sec` |

## Notes on current implementation behavior

This section documents behavior as implemented in `common/vscp-link-protocol.c` at the time of writing:

- `vscp_link_doCmdSetGUID` currently returns `VSCP_LINK_MSG_PARAMETER_ERROR` on callback success.
- `vscp_link_doCmdShutdown` currently returns `VSCP_LINK_MSG_NOT_SUPPORTED` when shutdown callback succeeds.
- `vscp_link_doCmdbRcvLoop` currently calls `vscp_link_callback_restart` (not `vscp_link_callback_brcvloop`).

If you rely on these commands, verify whether this is intended for your target branch before shipping.

## Callback checklist (copy/paste)

Use this as an integration checklist for your target.

A standalone reusable copy is available at [examples/vscp-link-protocol-callback-checklist.md](examples/vscp-link-protocol-callback-checklist.md).

### Minimum for a usable text session

- [ ] `vscp_link_callback_welcome`
- [ ] `vscp_link_callback_quit`
- [ ] `vscp_link_callback_write_client`
- [ ] `vscp_link_callback_get_rcvloop_status`

### Authentication and authorization

- [ ] `vscp_link_callback_check_user`
- [ ] `vscp_link_callback_check_password`
- [ ] `vscp_link_callback_challenge`
- [ ] `vscp_link_callback_check_authenticated`
- [ ] `vscp_link_callback_check_privilege`

### Event queue operations (text protocol)

- [ ] `vscp_link_callback_send`
- [ ] `vscp_link_callback_retr`
- [ ] `vscp_link_callback_enable_rcvloop`
- [ ] `vscp_link_callback_rcvloop`
- [ ] `vscp_link_callback_chkData`
- [ ] `vscp_link_callback_clrAll`

### Device/channel metadata

- [ ] `vscp_link_callback_get_channel_id`
- [ ] `vscp_link_callback_get_guid`
- [ ] `vscp_link_callback_set_guid`
- [ ] `vscp_link_callback_get_version`
- [ ] `vscp_link_callback_statistics`
- [ ] `vscp_link_callback_info`
- [ ] `vscp_link_callback_wcyd`

### Interface command support

- [ ] `vscp_link_callback_get_interface_count`
- [ ] `vscp_link_callback_get_interface`
- [ ] `vscp_link_callback_close_interface` (if `interface close` is supported)

### Optional binary/security commands

- [ ] `vscp_link_callback_bretr` (`bretr`)
- [ ] `vscp_link_callback_bsend` (`bsend`)
- [ ] `vscp_link_callback_brcvloop` (`brcvloop`)
- [ ] `vscp_link_callback_sec` (`sec`)

### Optional control hooks

- [ ] `vscp_link_callback_test` (`test`)
- [ ] `vscp_link_callback_shutdown` (`shutdown`)
- [ ] `vscp_link_callback_restart` (`restart`)
- [ ] `vscp_link_callback_help` (only if using `VSCP_LINK_CUSTOM_HELP_TEXT`)

### Bring-up sanity checklist

- [ ] `help`, `noop`, and empty line (`\r\n`) return `+OK` flow correctly.
- [ ] unauthenticated `send/retr/chkdata` return `VSCP_LINK_MSG_NOT_ACCREDITED`.
- [ ] low-privilege account returns `VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR` where expected.
- [ ] `retr 1` returns event line + `+OK` when queue has data.
- [ ] `rcvloop` emits periodic keepalive (`+OK`) on timeout path.
