# vscp-bootloader

Last updated: 2026-02-19

This document describes the VSCP bootloader module implemented by:

- `common/vscp-bootloader.h`
- `common/vscp-bootloader.c`

The module provides a protocol-driven firmware update state machine for VSCP nodes.

---

## Purpose

`vscpboot_loader()` is the boot entry state machine.

At boot:

1. Initialize bootloader hardware.
2. Read boot flag from persistent storage.
3. Jump to application if boot flag indicates normal run.
4. Otherwise enter VSCP boot protocol mode and handle firmware transfer events.

The implementation supports Level I and Level II usage patterns via configuration.

---

## Include and Dependencies

```c
#include <vscp-bootloader.h>
```

Primary dependencies:

- `vscp.h`
- `vscp-class.h`
- `vscp-type.h`
- `crc.h`
- `vscp-projdefs.h`

---

## Core Types and Constants

### Bootloader state

- `STATE_BLOCKSTART`
- `STATE_BLOCKPRG`
- `STATE_BLOCKDATA`

### Configuration

`vscpboot_config_t`:

- `vscpLevel` (`0` = Level I, `1` = Level II)
- `blockSize`
- `blockCount`

### Important constants

- `VSCPBOOT_FLAG_BOOT` (`0xFF`): value used to force bootloader mode.
- `VSCPBOOT_NICKNAME_BOOT` (`0xFE`): reserved nickname for booting node.

---

## Public Bootloader API

### Entry and runtime

- `vscpboot_initEventEx`
- `vscpboot_init`
- `vscpboot_loader`

### Platform/port hooks (implemented by target)

- `vscpboot_goApplication`
- `vscpboot_run`
- `vscpboot_init_hardware`
- `vscpboot_release_hardware`
- `vscpboot_getBootFlag`
- `vscpboot_setBootFlag`
- `vscpboot_reboot`
- `vscpboot_getConfig`
- `vscpboot_getGUID`
- `vscpboot_isMemTypeValid`
- `vscpboot_isMemBankValid`
- `vscpboot_programBlock`
- `vscpboot_calcPrgCrc`
- `vscpboot_sendEventEx`
- `vscpboot_getEventEx`

---

## Protocol Flow

### 1) Enter boot mode

On entering boot mode, the node sends `VSCP_TYPE_PROTOCOL_ACK_BOOT_LOADER` with:

- block size
- block count

### 2) Start block

Host sends `VSCP_TYPE_PROTOCOL_START_BLOCK` with block index and optional memory type/bank.

Bootloader validates request and responds:

- `VSCP_TYPE_PROTOCOL_START_BLOCK_ACK` on success
- `VSCP_TYPE_PROTOCOL_START_BLOCK_NACK` on failure

### 3) Transfer block data

Host sends `VSCP_TYPE_PROTOCOL_BLOCK_DATA` frames.

Bootloader appends payload into an internal block buffer until full and responds with ACK/NACK events.

### 4) Program block

Host sends `VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA` for the active block.

Bootloader calls `vscpboot_programBlock()` and returns:

- `VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA_ACK` on success
- `VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA_NACK` on failure

### 5) Activate image

Host sends `VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE` with expected CRC.

Bootloader compares against `vscpboot_calcPrgCrc()` and responds with:

- `VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE_ACK` when CRC matches
- `VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE_NACK` when CRC differs

---

## Implementor Contract

These functions are mandatory integration points:

- Hardware lifecycle (`init`/`release`)
- Boot flag storage (`get`/`set`)
- Application jump (`goApplication`)
- Transport receive/send (`getEventEx`/`sendEventEx`)
- Memory validation/programming (`isMemTypeValid`, `isMemBankValid`, `programBlock`)
- Program CRC calculation (`calcPrgCrc`)

### Transport translation note

`vscpboot_getEventEx()` must normalize protocol events so bootloader logic receives expected class/type and payload layout. In particular, Level II class-wrapped Level I protocol events should be mapped to the Level I protocol format expected by this module.

---

## Minimal Integration Example

```c
void boot_entry(void)
{
  vscpboot_init();
  vscpboot_loader();
}
```

Typical target-specific implementations include:

- `vscpboot_getBootFlag()` reading EEPROM/flash/NVM marker.
- `vscpboot_setBootFlag()` updating marker before reboot.
- `vscpboot_goApplication()` jumping to reset vector of application image.

---

## Common Integration Pitfalls

- Boot flag logic inverted: nonzero should enter bootloader, zero should run app.
- Block configuration mismatch: host and node must agree on `blockSize` and `blockCount`.
- Missing transport mapping: protocol class conversion not applied in `vscpboot_getEventEx()`.
- Incomplete hardware teardown: communication/peripherals not reset before jumping to app.
- CRC mismatch at activation: CRC algorithm or programmed range not aligned with host.

---

## Related Files

- `common/vscp-bootloader.h`
- `common/vscp-bootloader.c`
- `common/vscp-firmware-helper.h`
- `common/vscp-firmware.h`
- `docs/README.md`
