# VSCP Link Protocol callback checklist

Use this checklist when bringing up a new VSCP Link protocol transport/integration.

## Minimum for a usable text session

- [ ] `vscp_link_callback_welcome`
- [ ] `vscp_link_callback_quit`
- [ ] `vscp_link_callback_write_client`
- [ ] `vscp_link_callback_get_rcvloop_status`

## Authentication and authorization

- [ ] `vscp_link_callback_check_user`
- [ ] `vscp_link_callback_check_password`
- [ ] `vscp_link_callback_challenge`
- [ ] `vscp_link_callback_check_authenticated`
- [ ] `vscp_link_callback_check_privilege`

## Event queue operations (text protocol)

- [ ] `vscp_link_callback_send`
- [ ] `vscp_link_callback_retr`
- [ ] `vscp_link_callback_enable_rcvloop`
- [ ] `vscp_link_callback_rcvloop`
- [ ] `vscp_link_callback_chkData`
- [ ] `vscp_link_callback_clrAll`

## Device and channel metadata

- [ ] `vscp_link_callback_get_channel_id`
- [ ] `vscp_link_callback_get_guid`
- [ ] `vscp_link_callback_set_guid`
- [ ] `vscp_link_callback_get_version`
- [ ] `vscp_link_callback_statistics`
- [ ] `vscp_link_callback_info`
- [ ] `vscp_link_callback_wcyd`

## Interface command support

- [ ] `vscp_link_callback_get_interface_count`
- [ ] `vscp_link_callback_get_interface`
- [ ] `vscp_link_callback_close_interface` (if `interface close` is supported)

## Optional binary and security commands

- [ ] `vscp_link_callback_bretr` (`bretr`)
- [ ] `vscp_link_callback_bsend` (`bsend`)
- [ ] `vscp_link_callback_brcvloop` (`brcvloop`)
- [ ] `vscp_link_callback_sec` (`sec`)

## Optional control hooks

- [ ] `vscp_link_callback_test` (`test`)
- [ ] `vscp_link_callback_shutdown` (`shutdown`)
- [ ] `vscp_link_callback_restart` (`restart`)
- [ ] `vscp_link_callback_help` (only if `VSCP_LINK_CUSTOM_HELP_TEXT` is enabled)

## Bring-up sanity checks

- [ ] `help`, `noop`, and empty line (`\r\n`) return expected `+OK` flow.
- [ ] Unauthenticated `send/retr/chkdata` return `VSCP_LINK_MSG_NOT_ACCREDITED`.
- [ ] Low-privilege account returns `VSCP_LINK_MSG_LOW_PRIVILEGE_ERROR` where expected.
- [ ] `retr 1` returns one event line followed by `+OK` when data exists.
- [ ] `rcvloop` emits periodic keepalive (`+OK`) on timeout path.
