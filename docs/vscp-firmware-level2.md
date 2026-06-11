# vscp-firmware-level2

Last updated: 2026-06-11

This document describes the VSCP Level II firmware framework implemented by:

- `common/vscp-firmware-level2.h`
- `common/vscp-firmware-level2.c`

The module provides protocol handling, register abstraction, node state management, and decision-matrix execution for VSCP Level II nodes (and Level I compatibility paths where configured).

---

## Purpose

`vscp_frmw2_work()` is the main periodic protocol worker.

At runtime, the framework:

1. Handles node startup and state transitions.
2. Performs nickname discovery/wait logic for Level I compatibility mode.
3. Processes incoming protocol events.
4. Maintains standard register behavior.
5. Runs decision-matrix filtering and actions.
6. Sends periodic heartbeat/capabilities events when configured.

---

## Include and Dependencies

```c
#include <vscp-firmware-level2.h>
```

Primary dependencies:

- `vscp.h`
- `vscp-class.h`
- `vscp-type.h`
- `vscp-firmware-helper.h`
- `vscp-projdefs.h`

---

## Version and Constants

Framework version constants:

- `FRMW2_MAJOR_VERSION`
- `FRMW2_MINOR_VERSION`

Selected timing and probe constants:

- `VSCP_PROBE_TIMEOUT`
- `VSCP_PROBE_TIMEOUT_COUNT`
- `VSCP_SEGCTRL_PROBE_TIMEOUT`
- `VSCP_SEGCTRL_RESPONSE_TIMEOUT`

State constants:

- `FRMW2_STATE_NONE`
- `FRMW2_STATE_PROBE`
- `FRMW2_STATE_PREACTIVE`
- `FRMW2_STATE_ACTIVE`
- `FRMW2_STATE_ERROR`

Substate constants:

- `FRMW2_SUBSTATE_NONE`
- `FRMW2_SUBSTATE_SEGCTRL_PROBE_WAITING`
- `FRMW2_SUBSTATE_PROBE_WAITING`

---

## Core Configuration Structure

`vscp_frmw2_firmware_context_t` is the central runtime and configuration container.
(`vscp_frmw2_firmware_config_t` is a typedef alias provided for backward compatibility.)

Major groups inside the structure:

- Runtime state and timers (`m_state`, `m_substate`, `m_timer1`)
- Node identity (`m_nickname`, `m_guid`, level selection)
- Probe/discovery behavior (`m_probe_*`)
- Optional behavior switches (`m_bEnableErrorReporting`, `m_bEnableLogging`, etc.)
- Event scheduling (`m_interval_heartbeat`, `m_interval_caps`)
- Decision matrix storage (`m_pDm`, `m_nDmRows`, `m_sizeDmRow`)
- Standard registers and persistent fields (`m_userId`, `m_manufacturerId`, `m_mdfurl`, etc.)
- `ops` — pointer to a `vscp_frmw2_ops_t` operations table (must be set before calling `vscp_frmw2_init()`)

The context pointer `pctx` is passed as the first argument to every public function and to every callback.

---

## Public Framework API

### Initialization and Main Work

- `vscp_frmw2_init`
- `vscp_frmw2_init_persistent_storage`
- `vscp_frmw2_work`
- `vscp_frmw2_setup_event_ex`

### Nickname and Protocol Handling

- `vscp_frmw2_send_probe`
- `vscp_frmw2_nickname_discovery`
- `vscp_frmw2_nickname_wait`
- `vscp_frmw2_handle_protocol_event`

### Register and Page Operations

- `vscp_frmw2_read_reg`
- `vscp_frmw2_write_reg`
- `vscp_frmw2_page_read`
- `vscp_frmw2_page_write`
- `vscp_frmw2_vscp2_register_read`
- `vscp_frmw2_vscp2_register_write`
- `vscp_frmw2_extended_page_read`
- `vscp_frmw2_extended_page_write`

### Periodic and Discovery Events

- `vscp_frmw2_send_heartbeat`
- `vscp_frmw2_send_caps`
- `vscp_frmw2_send_high_end_server_probe`
- `vscp_frmw2_whois_response`

### Decision Matrix

- `vscp_frmw2_feed_level1_dm`
- `vscp_frmw2_feed_leve2_dm`  *(note: "leve2" is a known typo in the source — it should read `level2`; kept as-is for ABI compatibility)*
- `vscp_frmw2_feed_dm`
- `vscp_frmw2_report_dmatrix`

### Diagnostics and Logging

- `vscp_frmw2_send_error_event`
- `vscp_frmw2_send_log_event`

---

## Callback Contract (Implemented by Application/Port)

All platform-specific behavior is provided through a `vscp_frmw2_ops_t` operations table.
Populate the struct and assign it to `ctx.ops` before calling `vscp_frmw2_init()`.
Every callback receives `pctx` as its first argument.

Time and lifecycle callbacks:

- `get_milliseconds`
- `get_timestamp`
- `enter_bootloader`
- `restore_defaults`
- `reset`
- `feed_watchdog`

Transport and event callbacks:

- `send_event_ex`
- `set_event_time`
- `report_events_of_interest`
- `segment_ctrl_heartbeat`

Decision matrix callback:

- `dm_action`

Register and network callbacks:

- `read_reg`
- `write_reg`
- `stdreg_change`
- `get_ip_addr`

---

## Runtime State Machine Overview

Typical flow:

1. `vscp_frmw2_init(&ctx)` sets defaults and initial state.
2. Node enters active state immediately in Level II mode.
3. In Level I compatibility mode, framework may run probe/nickname phases first.
4. `vscp_frmw2_work(&ctx, pex)` is called periodically:
   - with `pex != NULL` when an event is available
   - with `pex == NULL` for periodic/state processing
5. Framework updates heartbeats/caps/protocol responses and applies decision matrix actions.

## Level II Startup/Runtime Diagram

```mermaid
stateDiagram-v2
  [*] --> INIT
  INIT: vscp_frmw2_init()

  INIT --> ACTIVE: Level II mode
  INIT --> PROBE: Level I compat + nickname unassigned
  INIT --> ACTIVE: Level I compat + nickname already set

  PROBE --> PREACTIVE: Segment controller responds
  PROBE --> PROBE: Probe timeout/retry nickname
  PROBE --> ERROR: Nickname space exhausted
  PREACTIVE --> ACTIVE: Nickname assigned/accepted
  PREACTIVE --> PROBE: Segment controller response timeout

  ACTIVE --> ACTIVE: vscp_frmw2_work(&ctx, NULL)
  ACTIVE --> ACTIVE: vscp_frmw2_work(&ctx, event)
  ACTIVE --> ACTIVE: Heartbeat/caps/protocol/DM handling

  ERROR --> PROBE: Reset/recovery path
```

## Nickname Discovery Timing Diagram

```mermaid
stateDiagram-v2
  [*] --> NONE
  NONE --> SEND_PROBE

  SEND_PROBE --> WAIT_SEGCTRL: probe nickname 0
  SEND_PROBE --> WAIT_PROBE: probe nickname > 0

  WAIT_SEGCTRL --> PREACTIVE: PROBE_ACK received
  WAIT_SEGCTRL --> WAIT_PROBE: timeout VSCP_SEGCTRL_PROBE_TIMEOUT / start nickname=1

  WAIT_PROBE --> NEXT_NICK: PROBE_ACK for tested nickname
  WAIT_PROBE --> RETRY_SAME: timeout and retry_count < VSCP_PROBE_TIMEOUT_COUNT
  WAIT_PROBE --> ASSIGN_NICK: timeout and retry_count >= VSCP_PROBE_TIMEOUT_COUNT

  RETRY_SAME --> SEND_PROBE
  NEXT_NICK --> SEND_PROBE

  ASSIGN_NICK --> ACTIVE: send NEW_NODE_ONLINE

  PREACTIVE --> ACTIVE: SET_NICKNAME accepted
  PREACTIVE --> NONE: timeout VSCP_SEGCTRL_RESPONSE_TIMEOUT
```

---

## Register Model Notes

The framework supports standard register handling and app-defined register callbacks.

Key points:

- `vscp_frmw2_read_reg()` / `vscp_frmw2_write_reg()` operate across standard and user register spaces.
- Page-based operations are provided for protocol page reads/writes.
- Extended page read/write supports larger access patterns.
- `ops->stdreg_change` is called to persist changes to standard register-backed data.

---

## Decision Matrix Notes

Decision matrix support is configurable through fields in `vscp_frmw2_firmware_context_t`:

- `m_pDm`
- `m_nDmRows`
- `m_sizeDmRow`
- `m_regOffsetDm`
- `m_pageDm`

Framework helpers feed events into Level I/Level II DM paths and invoke `ops->dm_action` when rules match.

---

## Minimal Integration Pattern

```c
// Define operations table
static const vscp_frmw2_ops_t my_ops = {
  .get_milliseconds = my_get_ms,
  .get_timestamp    = my_get_ts,
  .send_event_ex    = my_send_event_ex,
  .set_event_time   = my_set_event_time,
  .read_reg         = my_read_reg,
  .write_reg        = my_write_reg,
  .stdreg_change    = my_stdreg_change,
  // ... other callbacks
};

vscp_frmw2_firmware_context_t ctx;
memset(&ctx, 0, sizeof(ctx));

ctx.m_level = VSCP_LEVEL2;
ctx.ops     = &my_ops;

int rv = vscp_frmw2_init(&ctx);
if (VSCP_ERROR_SUCCESS == rv) {
  for (;;) {
    // If no incoming event:
    vscp_frmw2_work(&ctx, NULL);

    // If an incoming event is available:
    // vscp_event_ex_t ex; fill from transport...
    // vscp_frmw2_work(&ctx, &ex);
  }
}
```

---

## Common Integration Pitfalls

- Not calling `vscp_frmw2_work()` frequently enough for timers/probe/heartbeat handling.
- Missing callback implementations (especially send-event, register read/write, and time callbacks).
- Inconsistent register persistence handling after standard-register changes.
- Misconfigured decision matrix row size or storage pointer.
- Mixing Level I/Level II assumptions without setting `m_level` and transport behavior accordingly.

---

## Related Files

- `common/vscp-firmware-level2.h`
- `common/vscp-firmware-level2.c`
- `common/vscp-firmware-helper.h`
- `common/vscp-firmware.h`
- `docs/README.md`
