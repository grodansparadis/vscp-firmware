# vscp-aes

Last updated: 2026-02-19

This document describes the AES helper module implemented by:

- `common/vscp-aes.h`
- `common/vscp-aes.c`

The module provides AES-128/192/256 primitives for ECB and CBC operation modes plus IV generation support.

---

## Purpose

`vscp-aes` is a compact crypto utility used by firmware and frame-protection code.

Supported algorithms:

- `AES128`
- `AES192`
- `AES256`

Supported operation modes:

- ECB (compile-time enabled)
- CBC (compile-time enabled)

---

## Include and Dependencies

```c
#include <vscp-aes.h>
```

Primary dependency:

- `stdint.h`

Platform dependency for random IV generation:

- Windows: `rand_s`
- Non-Windows: `/dev/urandom`

---

## Compile-Time Configuration

Mode toggles in `vscp-aes.h`:

- `CBC` (default `1`)
- `ECB` (default `1`)

You can override these before include or by compiler definitions.

Example:

```c
#define CBC 1
#define ECB 0
#include <vscp-aes.h>
```

---

## Public API

### ECB mode

(Available when `ECB == 1`)

- `AES_ECB_encrypt`
- `AES_ECB_decrypt`

### CBC mode

(Available when `CBC == 1`)

- `AES_CBC_encrypt_buffer`
- `AES_CBC_decrypt_buffer`

### IV generation

- `getRandomIV`

---

## Buffer and Length Requirements

AES block size is always 16 bytes.

Important constraints:

- Input/output buffers must have enough space for `length` bytes.
- For standard operation, use lengths divisible by 16 (`length % 16 == 0`).
- If payload is not block-aligned, pad before encryption and unpad after decryption.

Key size must match algorithm type:

- `AES128` → 16-byte key
- `AES192` → 24-byte key
- `AES256` → 32-byte key

IV size for CBC is always 16 bytes.

---

## Usage Examples

### ECB encryption/decryption

```c
uint8_t key[16] = { /* 16 bytes */ };
uint8_t plain[32] = { /* 32 bytes */ };
uint8_t enc[32];
uint8_t dec[32];

AES_ECB_encrypt(AES128, plain, key, enc, sizeof(plain));
AES_ECB_decrypt(AES128, enc, key, dec, sizeof(enc));
```

### CBC encryption/decryption

```c
uint8_t key[32] = { /* 32 bytes for AES256 */ };
uint8_t iv[16];
uint8_t plain[64] = { /* must be block aligned */ };
uint8_t enc[64];
uint8_t dec[64];

getRandomIV(iv, sizeof(iv));

AES_CBC_encrypt_buffer(AES256, enc, plain, sizeof(plain), key, iv);
AES_CBC_decrypt_buffer(AES256, dec, enc, sizeof(enc), key, iv);
```

---

## Compatibility with Frame Encryption Helpers

`vscp-aes` is the low-level crypto primitive layer.

For VSCP frame encryption/decryption in firmware flows, use the higher-level helpers in `vscp-firmware-helper`:

- `vscp_fwhlp_encryptFrame`
- `vscp_fwhlp_decryptFrame`

Layering guidance:

- Use `AES_CBC_encrypt_buffer` / `AES_CBC_decrypt_buffer` when you need raw block crypto on buffers you fully manage.
- Use `vscp_fwhlp_encryptFrame` / `vscp_fwhlp_decryptFrame` when working with VSCP transport frames and VSCP encryption type semantics.

Algorithm mapping:

- `AES128` ↔ `VSCP_ENCRYPTION_AES128`
- `AES192` ↔ `VSCP_ENCRYPTION_AES192`
- `AES256` ↔ `VSCP_ENCRYPTION_AES256`

The frame helpers also handle VSCP-specific framing details (encryption type byte and IV placement conventions) and are preferred for protocol-level use.

---

## Security Notes

- Never reuse the same IV with the same key for different CBC messages.
- Always validate that random IV generation succeeded (`getRandomIV` return value).
- Keep secret keys out of logs and persistent plaintext storage.
- Apply authenticated protection at protocol level where possible (encryption alone does not provide integrity).

---

## Common Integration Pitfalls

- Passing incorrect key length for selected AES type.
- Encrypting non-block-aligned data without padding strategy.
- Reusing static IV values in CBC mode.
- Assuming ECB provides semantic security for repeated data patterns.
- Not checking IV generation failures on constrained platforms.

---

## Related Files

- `common/vscp-aes.h`
- `common/vscp-aes.c`
- `common/vscp-firmware-helper.h`
- `docs/README.md`
