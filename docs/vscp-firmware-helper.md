# vscp-firmware-helper

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

- `VSCP_FWHLP_UDP_FRAME_SUPPORT`
  - Enables UDP frame conversion APIs.
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
- `vscp_fwhlp_stristr`
- `vscp_fwhlp_strsubst`
- `vscp_fwhlp_parse_data`
- `vscp_fwhlp_make_string_from_data`

Use these for robust handling of decimal/hex/binary forms and VSCP data payload strings.

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

### 4) Priority/Filter Helpers

- `vscp_fwhlp_getEventPriority`
- `vscp_fwhlp_getEventPriorityEx`
- `vscp_fwhlp_setEventPriority`
- `vscp_fwhlp_doLevel2Filter`
- `vscp_fwhlp_doLevel2FilterEx`
- `vscp_fwhlp_parseFilter`
- `vscp_fwhlp_parseMask`

### 5) Event and EventEx Parse/Format and Memory Helpers

- `vscp_fwhlp_parseEvent`
- `vscp_fwhlp_parseEventEx`
- `vscp_fwhlp_eventToString`
- `vscp_fwhlp_eventToStringEx`
- `vscp_fwhlp_newEvent`
- `vscp_fwhlp_deleteEvent`
- `vscp_fwhlp_mkEventCopy`
- `vscp_fwhlp_mkEventExCopy`
- `vscp_fwhlp_convertEventToEventEx`
- `vscp_fwhlp_convertEventExToEvent`

### 6) UDP Frame Helpers (Optional)

(Requires `VSCP_FWHLP_UDP_FRAME_SUPPORT`)

- `vscp_fwhlp_getFrameSizeFromEventEx`
- `vscp_fwhlp_writeEventToFrame`
- `vscp_fwhlp_writeEventExToFrame`
- `vscp_fwhlp_getEventFromFrame`
- `vscp_fwhlp_getEventExFromFrame`

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

### Quickstart: `vscpEventEx` + UDP Frame Flow

Requires `VSCP_FWHLP_UDP_FRAME_SUPPORT`.

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

### Quickstart: Encrypted UDP Frame Flow (AES128)

Requires both `VSCP_FWHLP_UDP_FRAME_SUPPORT` and `VSCP_FWHLP_CRYPTO_SUPPORT`.

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

- Feature flags: ensure both `VSCP_FWHLP_UDP_FRAME_SUPPORT` and `VSCP_FWHLP_CRYPTO_SUPPORT` are defined.
- Buffer size: `encrypted` and `decrypted` buffers must be larger than plain frame data (padding + IV).
- Key/algorithm mismatch: decrypt with the same key and algorithm used for encrypt.
- Type-byte decode mode: when using `VSCP_ENCRYPTION_FROM_TYPE_BYTE`, verify the frame encryption nibble is preserved.
- Frame length: use `vscp_fwhlp_getFrameSizeFromEventEx` before encryption.
- Pointer/return checks: validate all pointers and check for `VSCP_ERROR_SUCCESS` or positive encrypted length.

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
