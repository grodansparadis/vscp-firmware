# VSCP Link Protocol callback checklist

Use this checklist when bringing up a new VSCP Link protocol transport/integration. All callbacks are fields of the `vscp_link_ops_t` table that `vscp_link_ctx_t.ops` points to (any function names can be used). See [vscp-link-protocol](../vscp-link-protocol.md).

## Required

- [ ] `write_client` (every command)
- [ ] `check_authenticated`, `check_privilege` (every command that needs a login)
- [ ] `get_rcvloop_status`, `rcvloop` (`vscp_link_idle_worker`)

## Session

- [ ] `welcome` (`vscp_link_connect`)
- [ ] `quit`
- [ ] `disconnect` (`vscp_link_disconnect`)

## Authentication

- [ ] `check_user`
- [ ] `check_password` (set `bValidated` and `privLevel` on success)
- [ ] `challenge`

## Event queue operations

- [ ] `send` — owns the event on success and frees it (or passes it on); return `VSCP_ERROR_TRM_FULL` when the queue is full (the library frees it then)
- [ ] `retr` — hand over a heap event (the library frees it); `VSCP_ERROR_RCV_EMPTY` when there is none
- [ ] `enable_rcvloop`
- [ ] `rcvloop` — like `retr`; return `VSCP_ERROR_TIMEOUT` about once a second without events (`+OK` keepalive)
- [ ] `chkdata`
- [ ] `clrall` — free the queued events (`vscp_fifo_clear()`)

## Device and channel metadata

- [ ] `get_channel_id`
- [ ] `get_guid`
- [ ] `set_guid`
- [ ] `get_version`
- [ ] `statistics`
- [ ] `info`
- [ ] `wcyd`

## Filter

- [ ] `set_filter`, `set_mask` (and apply `vscp_fwhlp_doLevel2Filter()` with the client's filter when events are queued for it)

## Interface command support

- [ ] `get_interface_count`
- [ ] `get_interface`
- [ ] `close_interface` (if `interface close` is supported)

## Optional

- [ ] `binary` (`binary`)
- [ ] `test` (`test`, privilege 15)
- [ ] `shutdown` (`shutdown`, privilege 15)
- [ ] `restart` (`restart`, privilege 15)
- [ ] `help_custom` (only if `VSCP_LINK_CUSTOM_HELP_TEXT` is enabled)

## Bring-up sanity checks

- [ ] `help`, `noop`, and empty line (`\r\n`) return expected `+OK` flow.
- [ ] Unauthenticated `send/retr/chkdata` return `VSCP_LINK_MSG_NOT_ACCREDITED`.
- [ ] Low-privilege account returns `VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR` where expected.
- [ ] `retr 1` returns one event line followed by `+OK` when data exists.
- [ ] `rcvloop` emits periodic keepalive (`+OK`) on timeout path.
- [ ] Thousands of `send` commands do not use up the memory (events are freed).
