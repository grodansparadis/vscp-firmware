# vscp-guid-parser

Last updated: 2026-03-12

This document describes the GUID parser module implemented by:

- `common/vscp-guid-parser.h`
- `common/vscp-guid-parser.c`

The module provides extended GUID parsing with support for multiple string formats, including compact notations, UUID format, and brace-enclosed GUIDs.

---

## Include and Dependencies

```c
#include <vscp-guid-parser.h>
```

Dependencies:

- `vscp.h` (for error codes)
- Standard C headers: `<stdint.h>`, `<string.h>`, `<ctype.h>`

---

## Supported GUID String Formats

The parser accepts multiple GUID string formats for flexibility:

### 1. Standard Colon-Separated Format

16 single-byte hex values separated by colons, dashes, or commas:

```
FF:FF:FF:FF:FF:FF:FF:FF:01:02:03:AA:BB:44:01:30
FF-FF-FF-FF-FF-FF-FF-FF-01-02-03-AA-BB-44-01-30
FF,FF,FF,FF,FF,FF,FF,FF,01,02,03,AA,BB,44,01,30
```

### 2. Double-Colon Prefix (Leading 0xFF Bytes)

`::` followed by trailing values fills leading bytes with `0xFF`:

```
::01:02:03:AA:BB:44:01:30
```

Equivalent to:

```
FF:FF:FF:FF:FF:FF:FF:FF:01:02:03:AA:BB:44:01:30
```

### 3. Hyphen-Colon Prefix (Leading Zero Bytes)

`-:` followed by trailing values fills leading bytes with `0x00`:

```
-:01:02:03:AA:BB:44:01:30
```

Equivalent to:

```
00:00:00:00:00:00:00:00:01:02:03:AA:BB:44:01:30
```

Comma separators are also supported:

```
-:1,2,3
```

Equivalent to:

```
00:00:00:00:00:00:00:00:00:00:00:00:00:01:02:03
```

### 4. Grouped Hex Values

Multiple hex digits can be grouped (2-8 digits per group):

```
::0102:03AA:BB44:0130
::0102:03aa:440130
```

### 5. UUID Format (8-4-4-4-12)

Standard UUID format with dashes:

```
FFFFFFFF-FFFF-FFFF-0102-03AABB440130
```

### 6. UUID-like Format with Colons

```
FFFFFFFF:FFFF:FFFF:0102:03AABB440130
```

### 7. Special Values

| String | Result |
|--------|--------|
| `-` | All zeros (`00:00:...:00`) |
| `::` | All 0xFF (`FF:FF:...:FF`) |

### 8. Brace-Enclosed Formats

Any of the above formats can be enclosed in curly braces:

```
{FF:FF:FF:FF:FF:FF:FF:FF:01:02:03:AA:BB:44:01:30}
{FFFFFFFF-FFFF-FFFF-0102-03AABB440130}
{::01:02:03:AA:BB:44:01:30}
{-:1,2,3}
{-}
{::}
```

Whitespace inside braces is allowed:

```
{ FF:FF:FF:FF:FF:FF:FF:FF:01:02:03:AA:BB:44:01:30 }
```

---

## API Reference

### vscp_guid_parse

Parse a GUID string into a 16-byte buffer.

```c
int vscp_guid_parse(uint8_t *guid, const char *strguid, char **endptr);
```

**Parameters:**

| Parameter | Description |
|-----------|-------------|
| `guid` | Pointer to 16-byte buffer to receive the parsed GUID |
| `strguid` | Input string containing GUID in any supported format |
| `endptr` | Optional pointer to receive position after parsed GUID (can be NULL) |

**Returns:**

- `VSCP_ERROR_SUCCESS` on success
- `VSCP_ERROR_INVALID_POINTER` if `guid` or `strguid` is NULL
- `VSCP_ERROR_ERROR` on parse error

**Example:**

```c
uint8_t guid[16];
char *end;

// Standard format
vscp_guid_parse(guid, "FF:EE:DD:CC:BB:AA:99:88:77:66:55:44:33:22:11:00", NULL);

// Compact format with leading 0xFF
vscp_guid_parse(guid, "::01:02:03", NULL);

// Compact format with leading zeros
vscp_guid_parse(guid, "-:1,2,3", NULL);

// Brace-enclosed UUID format
vscp_guid_parse(guid, "{FFFFFFFF-FFFF-FFFF-0102-03AABB440130}", &end);
```

---

### vscp_guid_to_string

Write GUID to string in standard colon-separated format.

```c
int vscp_guid_to_string(char *strguid, const uint8_t *guid);
```

**Parameters:**

| Parameter | Description |
|-----------|-------------|
| `strguid` | Buffer to receive string (must be at least 48 bytes) |
| `guid` | Pointer to 16-byte GUID |

**Returns:**

- `VSCP_ERROR_SUCCESS` on success
- `VSCP_ERROR_INVALID_POINTER` if `strguid` or `guid` is NULL

**Output format:**

```
FF:FF:FF:FF:FF:FF:FF:FF:01:02:03:AA:BB:44:01:30
```

---

### vscp_guid_to_string_compact

Write GUID to string using `::` notation for leading 0xFF bytes.

```c
int vscp_guid_to_string_compact(char *strguid, const uint8_t *guid);
```

**Parameters:**

| Parameter | Description |
|-----------|-------------|
| `strguid` | Buffer to receive string (must be at least 48 bytes) |
| `guid` | Pointer to 16-byte GUID |

**Returns:**

- `VSCP_ERROR_SUCCESS` on success
- `VSCP_ERROR_INVALID_POINTER` if `strguid` or `guid` is NULL

**Output examples:**

| Input GUID | Output String |
|------------|---------------|
| `FF:FF:FF:FF:FF:FF:FF:FF:01:02:03:04:05:06:07:08` | `::01:02:03:04:05:06:07:08` |
| `FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF` | `::` |
| `01:02:03:04:05:06:07:08:09:0A:0B:0C:0D:0E:0F:10` | `01:02:03:04:05:06:07:08:09:0A:0B:0C:0D:0E:0F:10` |

---

### vscp_guid_to_string_uuid

Write GUID to string in UUID format (8-4-4-4-12).

```c
int vscp_guid_to_string_uuid(char *strguid, const uint8_t *guid);
```

**Parameters:**

| Parameter | Description |
|-----------|-------------|
| `strguid` | Buffer to receive string (must be at least 48 bytes) |
| `guid` | Pointer to 16-byte GUID |

**Returns:**

- `VSCP_ERROR_SUCCESS` on success
- `VSCP_ERROR_INVALID_POINTER` if `strguid` or `guid` is NULL

**Output format:**

```
FFFFFFFF-FFFF-FFFF-0102-03AABB440130
```

---

## Usage Examples

### Parse Various Formats

```c
#include <vscp-guid-parser.h>

uint8_t guid[16];

// All these produce the same GUID:
vscp_guid_parse(guid, "FF:FF:FF:FF:FF:FF:FF:FF:01:02:03:04:05:06:07:08", NULL);
vscp_guid_parse(guid, "::01:02:03:04:05:06:07:08", NULL);
vscp_guid_parse(guid, "FFFFFFFF-FFFF-FFFF-0102-030405060708", NULL);
vscp_guid_parse(guid, "{::01:02:03:04:05:06:07:08}", NULL);

// Leading zeros format
vscp_guid_parse(guid, "-:01:02:03", NULL);  // 13 leading zeros + 01:02:03
vscp_guid_parse(guid, "-:1,2,3", NULL);     // Same result with comma separators
```

### Convert to Different Output Formats

```c
uint8_t guid[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
char str[48];

vscp_guid_to_string(str, guid);
// Result: "FF:FF:FF:FF:FF:FF:FF:FF:01:02:03:04:05:06:07:08"

vscp_guid_to_string_compact(str, guid);
// Result: "::01:02:03:04:05:06:07:08"

vscp_guid_to_string_uuid(str, guid);
// Result: "FFFFFFFF-FFFF-FFFF-0102-030405060708"
```

### Parse with End Pointer

```c
const char *input = "{FF:00:00:00:00:00:00:00:00:00:00:00:00:00:00:01} extra data";
uint8_t guid[16];
char *end;

vscp_guid_parse(guid, input, &end);
// end now points to " extra data"
```

---

## Error Handling

All functions return VSCP error codes:

| Code | Meaning |
|------|---------|
| `VSCP_ERROR_SUCCESS` | Operation completed successfully |
| `VSCP_ERROR_INVALID_POINTER` | NULL pointer passed for required parameter |
| `VSCP_ERROR_ERROR` | Parse error (malformed input) |

---

## Buffer Size Requirements

| Function | Minimum Buffer Size |
|----------|-------------------|
| `vscp_guid_to_string` | 48 bytes |
| `vscp_guid_to_string_compact` | 48 bytes |
| `vscp_guid_to_string_uuid` | 48 bytes |
