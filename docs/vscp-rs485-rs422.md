# vscp-rs485-rs422

Last updated: 2026-07-03

This document describes the RS-485/RS-422 protocol definition header:

- `common/vscp-rs485-protocol.h`

The header defines a VSCP-oriented multi-drop transport profile where:

- one master polls up to 254 clients for events
- every client has a preset 8-bit nickname (`0x01..0xFE`)
- event payloads use VSCP binary event format
- encryption is optional (default no encryption, AES-128 selectable)

---

## Purpose

`vscp-rs485-protocol.h` provides transport constants and frame-layout metadata for
implementing a polling RS-485/RS-422 driver in firmware.

It includes:

- framing control bytes
- parser state constants
- frame field offsets
- frame types
- command and error codes
- encryption mode IDs
- capability structure for node transport support

---

## Include

```c
#include <vscp-rs485-protocol.h>
```

---

## Bus model

- `VSCP_RS485_MAX_CLIENTS` = `254`
- `VSCP_RS485_MASTER_ID` = `0x00`
- `VSCP_RS485_BROADCAST_ID` = `0xFF`
- `VSCP_RS485_NICKNAME_SIZE` = `1` (8-bit preset nickname)

Master polling flow:

1. Master transmits `POLL_REQUEST` to a nickname.
2. Client replies with `POLL_RESPONSE`.
3. Client includes `VSCP_EVENT` frame when event data is available, otherwise `NO_EVENT`.

---

## Framing and parser states

Control bytes:

- `VSCP_RS485_DLE`
- `VSCP_RS485_STX`
- `VSCP_RS485_ETX`

Parser states:

- `STATE_VSCP_RS485_WAIT_FOR_FRAME_START`
- `STATE_VSCP_RS485_WAIT_FOR_FRAME_END`
- `STATE_VSCP_RS485_FRAME_RECEIVED`

---

## Frame layout

Decoded frame field offsets:

- `VSCP_RS485_POS_FRAME_TYPE`
- `VSCP_RS485_POS_DESTINATION_NICKNAME`
- `VSCP_RS485_POS_SOURCE_NICKNAME`
- `VSCP_RS485_POS_SEQUENCE`
- `VSCP_RS485_POS_FLAGS`
- `VSCP_RS485_POS_FRAME_SIZE_PAYLOAD_MSB`
- `VSCP_RS485_POS_FRAME_SIZE_PAYLOAD_LSB`
- `VSCP_RS485_POS_FRAME_PAYLOAD`

Flags in `VSCP_RS485_POS_FLAGS`:

- `VSCP_RS485_FLAG_ENCRYPTED`
- `VSCP_RS485_FLAG_ACK_REQUEST`

`VSCP_RS485_FLAG_ENCRYPTED` indicates that the payload is encrypted according to
the selected transport mode; if clear, payload is plain VSCP binary format.

---

## Frame types and commands

Frame types:

- `VSCP_RS485_FRAME_TYPE_NOOP`
- `VSCP_RS485_FRAME_TYPE_POLL_REQUEST`
- `VSCP_RS485_FRAME_TYPE_POLL_RESPONSE`
- `VSCP_RS485_FRAME_TYPE_VSCP_EVENT`
- `VSCP_RS485_FRAME_TYPE_NO_EVENT`
- `VSCP_RS485_FRAME_TYPE_COMMAND`
- `VSCP_RS485_FRAME_TYPE_COMMAND_REPLY`
- `VSCP_RS485_FRAME_TYPE_ACK`
- `VSCP_RS485_FRAME_TYPE_NACK`
- `VSCP_RS485_FRAME_TYPE_ERROR`

Transport commands:

- `VSCP_RS485_COMMAND_NOOP`
- `VSCP_RS485_COMMAND_POLL`
- `VSCP_RS485_COMMAND_SET_ENCRYPTION_NONE`
- `VSCP_RS485_COMMAND_SET_ENCRYPTION_AES128`

---

## Encryption modes

- `VSCP_RS485_ENCRYPTION_NONE` (default/open mode)
- `VSCP_RS485_ENCRYPTION_AES128` (optional mode)

Error codes include:

- `VSCP_RS485_ERROR_CHECKSUM`
- `VSCP_RS485_ERROR_UNKNOWN_OPERATION`
- `VSCP_RS485_ERROR_INVALID_NICKNAME`
- `VSCP_RS485_ERROR_ENCRYPTION_FAILURE`

---

## Capability structure

```c
typedef struct {
  uint8_t maxClients;
  uint8_t encryptionSupported;
  uint8_t defaultEncryption;
  uint8_t reserved;
} vscp_rs485_caps;
```

Packed size macro:

- `VSCP_RS485_CAPS_SIZE`

---

## Related files

- `common/vscp-rs485-protocol.h`
- `common/vscp-serial.h`
- `common/vscp-binary-protocol.h`
- `docs/vscp-serial.md`
- `docs/vscp-binary.md`
