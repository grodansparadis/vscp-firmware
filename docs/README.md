# Documentation

Last updated: 2026-11-20

[![Tests](https://github.com/grodansparadis/vscp-firmware/actions/workflows/tests.yml/badge.svg)](https://github.com/grodansparadis/vscp-firmware/actions/workflows/tests.yml)

The documentation is [here](https://grodansparadis.github.io/vscp-doc-firmware/#/)

## Where to start

- **A Level I node** (CAN4VSCP, RS-485, nickname): [vscp-firmware](vscp-firmware.md)
- **A Level II node** (TCP/IP, UDP, MQTT, ...): [vscp-firmware-level2](vscp-firmware-level2.md), plus [vscp-link-protocol](vscp-link-protocol.md) for a VSCP link protocol (TCP/IP) server and [vscp-firmware-helper](vscp-firmware-helper.md) for events, frames, JSON and GUIDs
- **Porting to a new platform**: [vscp-firmware-porting-checklist.md](examples/vscp-firmware-porting-checklist.md) and [vscp-link-protocol-callback-checklist.md](examples/vscp-link-protocol-callback-checklist.md)
- **An Arduino**: [vscp-arduino](vscp-arduino.md)

Complete nodes to copy from (the Blinky demo):

- [vscp-demo-esp32-wifi-blinky](https://github.com/grodansparadis/vscp-demo-esp32-wifi-blinky) — ESP32 Level II node: link protocol, UDP, multicast and MQTT
- [vscp-demo-link-stm32f401-wiz-ip20-blinky](https://github.com/grodansparadis/vscp-demo-link-stm32f401-wiz-ip20-blinky) — STM32F401 Level II node with a WIZ-IP20 Ethernet module

## Modules


- [vscp-aes](vscp-aes.md)
- [vscp-firmware-helper](vscp-firmware-helper.md)
- [vscp-fifo](vscp-fifo.md)
- [vscp-firmware](vscp-firmware.md)
- [vscp-firmware-level2](vscp-firmware-level2.md)
- [vscp-link-protocol](vscp-link-protocol.md)
- [vscp-serial](vscp-serial.md)
- [vscp-rs485-rs422](vscp-rs485-rs422.md)
- [vscp-bootloader](vscp-bootloader.md)
- [vscp-bootloader-host-sequence](vscp-bootloader-host-sequence.md)
- [vscp-binary](vscp-binary.md)
- [vscp-guid-parser](vscp-guid-parser.md)
- [vscp-ws1](vscp-ws1.md) / [vscp-ws2](vscp-ws2.md)
- [vscp-arduino](vscp-arduino.md) - the VscpFirmware Arduino library

Examples:

- [vscp-bootloader-host-updater-skeleton.c](examples/vscp-bootloader-host-updater-skeleton.c)
- [vscp-bootloader-host-updater-fsm-skeleton.c](examples/vscp-bootloader-host-updater-fsm-skeleton.c)
- [vscp-fifo-usage-skeleton.c](examples/vscp-fifo-usage-skeleton.c)
- [vscp-link-protocol-callback-checklist.md](examples/vscp-link-protocol-callback-checklist.md)
- [vscp-firmware-porting-checklist.md](examples/vscp-firmware-porting-checklist.md)

## Recent updates

- 2026-11-20: [vscp-firmware-level2](vscp-firmware-level2.md) uses the current context field names, describes the Level I/Level II decision matrix rows (`bDmLevel1Rows`), `vscp_frmw2_setup_event()` and who frees events. [vscp-link-protocol](vscp-link-protocol.md) describes `vscp_link_parser()` as it is (one line per call), the `ops` callbacks and event ownership. [vscp-fifo](vscp-fifo.md): `vscp_fifo_clear()` deletes the queued events.
- Added `vscp_fwhlp_parseStringValue` behavior notes in [vscp-firmware-helper](vscp-firmware-helper.md), including base-prefix handling (`0x`, `0`, `0b`) and `endptr` token parsing semantics.

## CI

- Tests workflow: [GitHub Actions - Tests](https://github.com/grodansparadis/vscp-firmware/actions/workflows/tests.yml)
- Latest workflow runs: [GitHub Actions runs](https://github.com/grodansparadis/vscp-firmware/actions)
