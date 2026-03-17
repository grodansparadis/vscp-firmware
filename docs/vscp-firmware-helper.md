# vscp-firmware-helper

Last updated: 2026-02-20

This document describes the helper module implemented by:

- `common/vscp-firmware-helper.h`
- `common/vscp-firmware-helper.c`

The module provides utility functions used across VSCP firmware targets:

- VSCP Event and EventEx parsing/serialization (`vscpEvent`, `vscpEventEx`)
- GUID, MAC, date/time and data-string conversion
- Frame encoding/decoding for VSCP UDP framing
- Optional cryptography and JSON/XML support
- Measurement extraction helpers and CANAL ID helpers

---

## Include and Dependencies

```c
#include <vscp-firmware-helper.h>
```

Base dependencies are pulled in via:

- `vscp.h`
- `vscp-compiler.h`
- `vscp-projdefs.h`
- `vscp-aes.h` (for crypto-related declarations)

Optional dependencies:

- JSON support uses cJSON (`third-party/cJSON`) when `VSCP_FWHLP_JSON_SUPPORT` is enabled.

---

## Compile-Time Feature Flags

The helper API is split into always-available and feature-gated parts.

- `VSCP_FWHLP_BINARY_FRAME_SUPPORT`
  - Enables binary frame conversion APIs (event/command/reply framing).
- `VSCP_FWHLP_CRYPTO_SUPPORT`
  - Enables `vscp_fwhlp_encryptFrame` and `vscp_fwhlp_decryptFrame`.
- `VSCP_FWHLP_JSON_SUPPORT`
  - Enables JSON parse/create APIs.
- `VSCP_FWHLP_XML_SUPPORT`
  - Enables XML parse/create APIs.

Define these in project config (typically `vscp-projdefs.h`) according to footprint and dependencies.

---

## Error Handling Conventions

Most APIs return one of the VSCP error codes (for example `VSCP_ERROR_SUCCESS`, `VSCP_ERROR_INVALID_POINTER`, `VSCP_ERROR_BUFFER_TO_SMALL`, `VSCP_ERROR_INVALID_SYNTAX`, `VSCP_ERROR_PARSING`).

General pattern:

- Return value `VSCP_ERROR_SUCCESS` means success.
- Negative/error codes indicate failure.
- Caller-owned output buffers must be valid and large enough.
- Pointer arguments are expected to be non-null unless explicitly documented otherwise.

---

## API Overview by Category

### 1) Number/String Conversion and Parsing

- `vscp_fwhlp_a2ul`
- `vscp_fwhlp_dec2hex`
- `vscp_fwhlp_hex2dec`
- `vscp_fwhlp_hexchar`
- `vscp_fwhlp_bin2hex`
- `vscp_fwhlp_hex2bin`
- `vscp_fwhlp_readStringValue`
- `vscp_fwhlp_parseStringValue`
- `vscp_fwhlp_stristr`
- `vscp_fwhlp_strsubst`
- `vscp_fwhlp_parse_data`
- `vscp_fwhlp_make_string_from_data`

Use these for robust handling of decimal/hex/binary forms and VSCP data payload strings.

`vscp_fwhlp_parseStringValue` parsing rules:

- Leading whitespace is skipped.
- Prefix `0x`/`0X` parses hexadecimal.
- Leading `0` parses octal using C-style numeric parsing.
- Prefix `0b`/`0B` parses binary (`0`/`1` digits only).
- `endptr` points to the first character after the parsed token, which makes it suitable for token-by-token parsing in comma/space separated buffers.

### 2) Date/Time Helpers

- `vscp_fwhlp_get_datestr_from_event`
- `vscp_fwhlp_get_datestr_from_eventex`
- `vscp_fwhlp_parse_event_datestr`
- `vscp_fwhlp_parse_eventex_datestr`
- `vscp_fwhlp_to_unix_ns`
- `vscp_fwhlp_from_unix_ns`

Date strings follow ISO-style form such as `YYYY-MM-DDTHH:MM:SSZ`.

### 3) Identity and Addressing Helpers

- `vscp_fwhlp_parseMac`
- `vscp_fwhlp_parseGuid`
- `vscp_fwhlp_writeGuidToString`
- `vscp_fwhlp_writeGuidToStringCompact`
- `vscp_fwhlp_writeGuidToStringUUID`

#### vscp_fwhlp_parseGuid

Parse GUID string with extended format support.

```c
int vscp_fwhlp_parseGuid(uint8_t *guid, const char *strguid, char **endptr);
```

Supported formats:

| Format | Example | Description |
|--------|---------|-------------|
| Standard | `FF:EE:DD:CC:BB:AA:99:88:77:66:55:44:33:22:11:00` | Colon, dash, or comma separated |
| All zeros | `-` | Returns all 0x00 bytes |
| All 0xFF | `::` | Returns all 0xFF bytes |
| Leading 0xFF | `::01:02:03` | Fills leading bytes with 0xFF |
| Leading zeros | `-:01:02:03` | Fills leading bytes with 0x00 |
| UUID | `FFFFFFFF-FFFF-FFFF-0102-03AABB440130` | 8-4-4-4-12 format |
| Brace-enclosed | `{FF:EE:DD:CC:...}` | Any format inside `{}` |

#### vscp_fwhlp_writeGuidToStringCompact

Write GUID using `::` notation for leading 0xFF bytes.

```c
int vscp_fwhlp_writeGuidToStringCompact(char *strguid, const uint8_t *guid);
```

Output examples:
- `FF:FF:FF:FF:FF:FF:FF:FF:01:02:03:04:05:06:07:08` → `::01:02:03:04:05:06:07:08`
- `FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF` → `::`
- `01:02:03:04:...` → Standard format (no leading 0xFF)

Buffer must be at least 48 bytes.

#### vscp_fwhlp_writeGuidToStringUUID

Write GUID in UUID format (8-4-4-4-12).

```c
int vscp_fwhlp_writeGuidToStringUUID(char *strguid, const uint8_t *guid);
```

Output format: `FFFFFFFF-FFFF-FFFF-0102-03AABB440130`

Buffer must be at least 48 bytes.

### 4) Priority/Filter/Frame Version Helpers

- `vscp_fwhlp_getEventPriority`
- `vscp_fwhlp_getEventPriorityEx`
- `vscp_fwhlp_setEventPriority`
- `vscp_fwhlp_setEventExPriority`
- `setFrameVersion`
- `setFrameVersionEx`
- `vscp_fwhlp_doLevel2Filter`
- `vscp_fwhlp_doLevel2FilterEx`
- `vscp_fwhlp_parseFilter`
- `vscp_fwhlp_writeFilterToString`
- `vscp_fwhlp_parseMask`
- `vscp_fwhlp_writeMaskToString`

#### setFrameVersion

Set VSCP frame version in event structure.

```c
bool setFrameVersion(vscpEvent *pEvent, uint16_t version);
```

The frame version indicates which version of the VSCP frame format the event uses.
It is stored in bits 8-9 of the 16-bit `head` field.

**Parameters:**
- `pEvent` - Pointer to event structure.
- `version` - Frame version value. Use one of:
  - `VSCP_HEADER16_FRAME_VERSION_ORIGINAL` (0x0000) - Original frame format
  - `VSCP_HEADER16_FRAME_VERSION_UNIX_NS` (0x0100) - Frame with Unix nanosecond timestamp
  - `VSCP_HEADER16_FRAME_VERSION_2` (0x0200) - Reserved
  - `VSCP_HEADER16_FRAME_VERSION_3` (0x0300) - Reserved

**Returns:** `true` on success, `false` if `pEvent` is NULL.

**Example:**

```c
vscpEvent ev;
memset(&ev, 0, sizeof(ev));

// Set frame to use Unix nanosecond timestamp format
setFrameVersion(&ev, VSCP_HEADER16_FRAME_VERSION_UNIX_NS);
```

#### setFrameVersionEx

Set VSCP frame version in event ex structure.

```c
bool setFrameVersionEx(vscpEventEx *pEventEx, uint16_t version);
```

The frame version indicates which version of the VSCP frame format the event uses.
It is stored in bits 8-9 of the 16-bit `head` field.

**Parameters:**
- `pEventEx` - Pointer to event ex structure.
- `version` - Frame version value (same constants as `setFrameVersion`).

**Returns:** `true` on success, `false` if `pEventEx` is NULL.

**Example:**

```c
vscpEventEx ex;
memset(&ex, 0, sizeof(ex));

// Set frame to use Unix nanosecond timestamp format
setFrameVersionEx(&ex, VSCP_HEADER16_FRAME_VERSION_UNIX_NS);
```

### 5) Event and EventEx Parse/Format and Memory Helpers

- `vscp_fwhlp_parseStringToEvent`
- `vscp_fwhlp_parseStringToEventEx`
- `vscp_fwhlp_eventToString`
- `vscp_fwhlp_eventToStringEx`
- `vscp_fwhlp_newEvent`
- `vscp_fwhlp_deleteEvent`
- `vscp_fwhlp_mkEventCopy`
- `vscp_fwhlp_mkEventExCopy`
- `vscp_fwhlp_convertEventToEventEx`
- `vscp_fwhlp_convertEventExToEvent`

### 6) Binary Frame Helpers (Optional)

(Requires `VSCP_FWHLP_BINARY_FRAME_SUPPORT`)

- `vscp_fwhlp_getFrameSizeFromEvent`
- `vscp_fwhlp_getFrameSizeFromEventEx`
- `vscp_fwhlp_writeEventToFrame`
- `vscp_fwhlp_writeEventExToFrame`
- `vscp_fwhlp_getEventFromFrame`
- `vscp_fwhlp_getEventExFromFrame`
- `vscp_fwhlp_writeCommandToFrame`
- `vscp_fwhlp_writeReplyToFrame`

#### vscp_fwhlp_writeCommandToFrame

Write a command to a binary frame.

```c
int vscp_fwhlp_writeCommandToFrame(uint8_t *frame, size_t len, uint16_t command,
                                   const uint8_t *arg, size_t arg_len);
```

**Parameters:**
- `frame` - Buffer to receive the command frame.
- `len` - Size of the buffer.
- `command` - 16-bit command code.
- `arg` - Optional argument data (can be NULL if `arg_len` is 0).
- `arg_len` - Size of argument data.

**Returns:** `VSCP_ERROR_SUCCESS` on success, `VSCP_ERROR_PARAMETER` for invalid pointers,
`VSCP_ERROR_BUFFER_TO_SMALL` if the buffer is too small.

Frame layout: `[pkttype(1)] [command(2)] [arg(n)] [CRC(2)]`

Encryption is set to none. If encryption is needed, use `vscp_fwhlp_encryptFrame()` on the
resulting frame before sending.

**Example:**

```c
uint8_t frame[64];
uint8_t args[] = { 0x01, 0x02 };
int rv = vscp_fwhlp_writeCommandToFrame(frame, sizeof(frame), 0x0010, args, 2);
if (VSCP_ERROR_SUCCESS == rv) {
  // frame is ready to send
}
```

#### vscp_fwhlp_writeReplyToFrame

Write a reply to a binary frame.

```c
int vscp_fwhlp_writeReplyToFrame(uint8_t *frame, size_t len, uint16_t command,
                                 uint16_t error, const uint8_t *arg, size_t arg_len);
```

**Parameters:**
- `frame` - Buffer to receive the reply frame.
- `len` - Size of the buffer.
- `command` - 16-bit command code being replied to.
- `error` - 16-bit error code for the reply.
- `arg` - Optional argument data (can be NULL if `arg_len` is 0).
- `arg_len` - Size of argument data.

**Returns:** `VSCP_ERROR_SUCCESS` on success, `VSCP_ERROR_PARAMETER` for invalid pointers,
`VSCP_ERROR_BUFFER_TO_SMALL` if the buffer is too small.

Frame layout: `[pkttype(1)] [command(2)] [error(2)] [arg(n)] [CRC(2)]`

Encryption is set to none. If encryption is needed, use `vscp_fwhlp_encryptFrame()` on the
resulting frame before sending.

**Example:**

```c
uint8_t frame[64];
int rv = vscp_fwhlp_writeReplyToFrame(frame, sizeof(frame), 0x0010, 0x0000, NULL, 0);
if (VSCP_ERROR_SUCCESS == rv) {
  // frame contains success reply to command 0x0010
}
```

### 7) Crypto Helpers (Optional)

(Requires `VSCP_FWHLP_CRYPTO_SUPPORT`)

- `vscp_fwhlp_encryptFrame`
- `vscp_fwhlp_decryptFrame`

### 8) JSON Helpers (Optional)

(Requires `VSCP_FWHLP_JSON_SUPPORT`)

- `vscp_fwhlp_parse_json`
- `vscp_fwhlp_parse_json_ex`
- `vscp_fwhlp_create_json`
- `vscp_fwhlp_create_json_ex`

### 9) XML Helpers (Optional)

(Requires `VSCP_FWHLP_XML_SUPPORT`)

- `vscp_fwhlp_parse_xml_event`
- `vscp_fwhlp_parse_xml_eventex`
- `vscp_fwhlp_event_to_xml`
- `vscp_fwhlp_eventex_to_xml`
- `vscp_fwhlp_set_event_info_from_topic`
- `vscp_fwhlp_set_eventex_info_from_topic`

Topic format for the two helper functions:

- `vscp/guid/class/type`
- `guid` is `00:11:22:33:44:55:66:77:88:99:AA:BB:CC:DD:EE:FF`
- `class` and `type` are decimal strings

### 10) Measurement Helpers

- `vscp_fwhlp_getMeasurementDataCoding`
- `vscp_fwhlp_getDataCodingBitArray`
- `vscp_fwhlp_getDataCodingInteger`
- `vscp_fwhlp_getDataCodingNormalizedInteger`
- `vscp_fwhlp_getMeasurementUnit`
- `vscp_fwhlp_getMeasurementSensorIndex`
- `vscp_fwhlp_getMeasurementZone`
- `vscp_fwhlp_getMeasurementSubZone`
- `vscp_fwhlp_isMeasurement`
- `vscp_fwhlp_getMeasurementAsFloat`
- `vscp_fwhlp_getMeasurementAsString`
- `vscp_fwhlp_getMeasurementFloat64AsString`
- `vscp_fwhlp_getMeasurementAsDouble`
- `vscp_fwhlp_getMeasurementAsDoubleEx`
- `vscp_fwhlp_getMeasurementWithZoneAsString`

Measurement conversion helpers provide float/string/double extraction for common
measurement classes. `vscp_fwhlp_getMeasurementAsDoubleEx` accepts `vscpEventEx`
and internally converts to `vscpEvent` before extraction.

### 11) CANAL ID Helpers

- `vscp_fwhlp__getHeadFromCANALid`
- `vscp_fwhlp__getVscpClassFromCANALid`
- `vscp_fwhlp__getVscpTypeFromCANALid`
- `vscp_fwhlp__getNicknameFromCANALid`
- `vscp_fwhlp__getCANALidFromData`
- `vscp_fwhlp__getCANALidFromEvent`

---

## Usage Examples

### Parse Textual Event Data Bytes

```c
uint8_t data[16];
int n = vscp_fwhlp_parse_data(data,
                              sizeof(data),
                              "1,2,3,255,0x05,0b10101010",
                              NULL);
if (n < 0) {
  // handle parse error
}
```

### Parse Single Value with `endptr`

```c
const char *input = "  0b101101,0x2A,077,42";
char *next;

uint32_t a = vscp_fwhlp_parseStringValue(input, &next); // 45
if (next && *next == ',') {
  next++;
}

uint32_t b = vscp_fwhlp_parseStringValue(next, &next); // 42
```

### Convert Event to Compact CSV-Like String

```c
char out[256];
int rv = vscp_fwhlp_eventToString(out, sizeof(out), &event);
if (VSCP_ERROR_SUCCESS != rv) {
  // handle conversion error
}
```

### Convert Between `vscpEvent` and `vscpEventEx`

```c
vscpEventEx ex;
int rv = vscp_fwhlp_convertEventToEventEx(&ex, &event);
if (VSCP_ERROR_SUCCESS != rv) {
  // handle conversion error
}
```

### JSON Support (Feature Enabled)

```c
char json[1024];
int rv = vscp_fwhlp_create_json(json, sizeof(json), &event);
if (VSCP_ERROR_SUCCESS == rv) {
  // json now contains serialized event
}
```

### Filter/Mask String Serialization

Write filter and mask to string representation for storage or transmission:

```c
vscpEventFilter filter;
char strFilter[128];
char strMask[128];

// Setup filter
memset(&filter, 0, sizeof(filter));
filter.filter_priority = 3;
filter.filter_class    = 10;
filter.filter_type     = 6;
filter.mask_priority   = 7;
filter.mask_class      = 0xFFFF;
filter.mask_type       = 0xFFFF;

// Write filter to string: "priority,class,type,GUID"
int rv = vscp_fwhlp_writeFilterToString(strFilter, sizeof(strFilter), &filter);
if (VSCP_ERROR_SUCCESS == rv) {
  // strFilter contains "3,10,6,00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00"
}

// Write mask to string
rv = vscp_fwhlp_writeMaskToString(strMask, sizeof(strMask), &filter);
if (VSCP_ERROR_SUCCESS == rv) {
  // strMask contains "7,65535,65535,00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00"
}

// Parse strings back to filter structure
vscpEventFilter restored;
memset(&restored, 0, sizeof(restored));
vscp_fwhlp_parseFilter(&restored, strFilter);
vscp_fwhlp_parseMask(&restored, strMask);
```

The string format for both filter and mask is:
`priority,class,type,GUID` where GUID is colon-separated hex bytes.

Buffer size requirement: minimum 62 bytes for output string.

---

## Quickstart

Minimal end-to-end flow for common integration:

1. Parse incoming text/JSON/XML into `vscpEvent` or `vscpEventEx`.
2. Convert between Event and EventEx format if needed.
3. Serialize for transport/logging.
4. Free allocated dynamic data (`vscpEvent.pdata`) when done.

```c
vscpEvent ev;
vscpEventEx ex;
char out[256];

memset(&ev, 0, sizeof(ev));
memset(&ex, 0, sizeof(ex));

// Parse compact event string
if (VSCP_ERROR_SUCCESS != vscp_fwhlp_parseEvent(&ev,
    "3,10,6,1234,2024-06-30T23:59:58Z,50817,00:00:00:00:00:00:00:00:00:00:00:00:00:01:00:02,1,2,3")) {
  // handle parse error
}

// Convert to EventEx representation
if (VSCP_ERROR_SUCCESS != vscp_fwhlp_convertEventToEventEx(&ex, &ev)) {
  // handle conversion error
}

// Serialize to compact string for logging/transport
if (VSCP_ERROR_SUCCESS == vscp_fwhlp_eventToString(out, sizeof(out), &ev)) {
  // use out
}

// Cleanup dynamic payload (allocated by parseEvent)
if (ev.pdata) {
  free(ev.pdata);
  ev.pdata = NULL;
}
```

For JSON and XML workflows, use the same pattern with:

- `vscp_fwhlp_parse_json` / `vscp_fwhlp_create_json`
- `vscp_fwhlp_parse_xml_event` / `vscp_fwhlp_event_to_xml`

### Quickstart: `vscpEventEx` + Binary Frame Flow

Requires `VSCP_FWHLP_BINARY_FRAME_SUPPORT`.

```c
vscpEventEx ex_in;
vscpEventEx ex_out;
uint8_t frame[600];

memset(&ex_in, 0, sizeof(ex_in));
memset(&ex_out, 0, sizeof(ex_out));

ex_in.head = 3;
ex_in.vscp_class = 10;
ex_in.vscp_type = 6;
ex_in.sizeData = 3;
ex_in.data[0] = 1;
ex_in.data[1] = 2;
ex_in.data[2] = 3;

int rv = vscp_fwhlp_writeEventExToFrame(
  frame,
  sizeof(frame),
  VSCP_ENCRYPTION_NONE,
  &ex_in);

if (VSCP_ERROR_SUCCESS == rv) {
  rv = vscp_fwhlp_getEventExFromFrame(&ex_out, frame, sizeof(frame));
  if (VSCP_ERROR_SUCCESS == rv) {
    // ex_out now contains decoded event data
  }
}
```

### Quickstart: Encrypted Binary Frame Flow (AES128)

Requires both `VSCP_FWHLP_BINARY_FRAME_SUPPORT` and `VSCP_FWHLP_CRYPTO_SUPPORT`.

```c
uint8_t key[16] = {
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
  0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F
};

uint8_t frame[600];
uint8_t encrypted[640];
uint8_t decrypted[640];

vscpEventEx ex;
memset(&ex, 0, sizeof(ex));
ex.head = 3;
ex.vscp_class = 10;
ex.vscp_type = 6;
ex.sizeData = 2;
ex.data[0] = 0x11;
ex.data[1] = 0x22;

int rv = vscp_fwhlp_writeEventExToFrame(frame, sizeof(frame), VSCP_ENCRYPTION_NONE, &ex);
if (VSCP_ERROR_SUCCESS == rv) {
  size_t plain_len = vscp_fwhlp_getFrameSizeFromEventEx(&ex);

  size_t enc_len = vscp_fwhlp_encryptFrame(encrypted,
                                           frame,
                                           plain_len,
                                           key,
                                           NULL,  // auto-generated IV appended to output
                                           VSCP_ENCRYPTION_AES128);

  if (enc_len > 0) {
    rv = vscp_fwhlp_decryptFrame(decrypted,
                                 encrypted,
                                 enc_len,
                                 key,
                                 NULL,  // read IV from end of encrypted frame
                                 VSCP_ENCRYPTION_FROM_TYPE_BYTE);

    if (VSCP_ERROR_SUCCESS == rv) {
      // decrypted now contains original frame payload
    }
  }
}
```

Common Failures and Checks:

- Feature flags: ensure both `VSCP_FWHLP_BINARY_FRAME_SUPPORT` and `VSCP_FWHLP_CRYPTO_SUPPORT` are defined.
- Buffer size: `encrypted` and `decrypted` buffers must be larger than plain frame data (padding + IV).
- Key/algorithm mismatch: decrypt with the same key and algorithm used for encrypt.
- Type-byte decode mode: when using `VSCP_ENCRYPTION_FROM_TYPE_BYTE`, verify the frame encryption nibble is preserved.
- Frame length: use `vscp_fwhlp_getFrameSizeFromEventEx` before encryption.
- Pointer/return checks: validate all pointers and check for `VSCP_ERROR_SUCCESS` or positive encrypted length.

### Quickstart: Set Event Info from MQTT Topic

Requires `VSCP_FWHLP_XML_SUPPORT`.

```c
vscpEvent ev;
vscpEventEx ex;

memset(&ev, 0, sizeof(ev));
memset(&ex, 0, sizeof(ex));

const char *topic =
  "vscp/FF:EE:DD:CC:BB:AA:99:88:77:66:55:44:33:22:11:00/10/6";

int rv = vscp_fwhlp_set_event_info_from_topic(&ev, topic);
if (VSCP_ERROR_SUCCESS == rv) {
  // ev.GUID, ev.vscp_class, ev.vscp_type are now set from topic
}

rv = vscp_fwhlp_set_eventex_info_from_topic(&ex, topic);
if (VSCP_ERROR_SUCCESS == rv) {
  // ex.GUID, ex.vscp_class, ex.vscp_type are now set from topic
}
```

---

## Notes for Integrators

- Keep stack and buffer sizes in mind on constrained targets; many APIs expect caller-provided buffers.
- For dynamic-event APIs (`vscpEvent` with `pdata`), define ownership and free strategy clearly.
- For feature-gated APIs, ensure both compile flags and source dependencies are included.
- Validate all external input (JSON/XML/string data) before using decoded fields in application logic.

---

## Related Files

- `common/vscp-firmware-helper.h`
- `common/vscp-firmware-helper.c`
- `tests/unittest.cpp` (usage and behavior examples)
- `docs/README.md`
