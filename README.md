![License](https://img.shields.io/badge/license-MIT-blue.svg)
[![Tests](https://github.com/grodansparadis/vscp-firmware/actions/workflows/tests.yml/badge.svg)](https://github.com/grodansparadis/vscp-firmware/actions/workflows/tests.yml)
[![Project Status: Active – The project has reached a stable, usable state and is being actively developed.](https://www.repostatus.org/badges/latest/active.svg)](https://www.repostatus.org/#active)


# VCSP Firmware

![VSCP Logo](./docs/images/vscp-firmware.jpg)

VSCP firmware contains examples for different hardware architectures of how to
implement the Very Simple Control Protocol (VSCP, [http://www.vscp.org](http://www.vscp.org)).

Files are organized under hardware architecture with common file folders on
each level where common files for vscp (folder /common) and common
files for a specific architecture (folder /{arch}/common) reside. 

Most important files for implementers are:

  * /common/vscp_firmware.* (for Level I nodes) and
  * /common/vscp_firmware_level2.* (for Level II (and Level I) nodes)

which both implements the VSCP protocol. The first alternative is created for very space limited devices, the second is for devices with more normal resources. 

There are also some references to
vscp_class.h and vscp_type.h which reside in the vscp_software repository
in the path /src/vscp/common. It is preferred to include these files from
that second repository (and not to make redundant copies if possible).

Documentation on how to build your own nodes from this code is [here](https://grodansparadis.github.io/vscp-doc-firmware/#/) 

Local module documentation for the helper library is available in [docs/vscp-firmware-helper.md](docs/vscp-firmware-helper.md).
Local module documentation for the VSCP FIFO helper is available in [docs/vscp-fifo.md](docs/vscp-fifo.md).
Local module documentation for the Level I firmware core is available in [docs/vscp-firmware.md](docs/vscp-firmware.md).
Local module documentation for the AES module is available in [docs/vscp-aes.md](docs/vscp-aes.md).
Local module documentation for the Level II firmware framework is available in [docs/vscp-firmware-level2.md](docs/vscp-firmware-level2.md).
Local module documentation for the VSCP link protocol is available in [docs/vscp-link-protocol.md](docs/vscp-link-protocol.md).
Local module documentation for serial protocol definitions is available in [docs/vscp-serial.md](docs/vscp-serial.md).
Local module documentation for the bootloader is available in [docs/vscp-bootloader.md](docs/vscp-bootloader.md).
Host-side firmware update flow for bootloader integration is available in [docs/vscp-bootloader-host-sequence.md](docs/vscp-bootloader-host-sequence.md).
Reusable VSCP link protocol callback bring-up checklist is available in [docs/examples/vscp-link-protocol-callback-checklist.md](docs/examples/vscp-link-protocol-callback-checklist.md).
Reusable VSCP Level I firmware porting checklist is available in [docs/examples/vscp-firmware-porting-checklist.md](docs/examples/vscp-firmware-porting-checklist.md).

## Recent updates

- Added `vscp_fwhlp_parseStringValue` behavior notes in [docs/vscp-firmware-helper.md](docs/vscp-firmware-helper.md), including base-prefix handling (`0x`, `0`, `0b`) and `endptr` token parsing semantics.

## Version

The library version is `year.month.patch` (for example `26.11.3`) and is in
[common/vscp-firmware-version.h](common/vscp-firmware-version.h):

- `VSCP_FIRMWARE_LIB_VERSION_YEAR`, `_MONTH`, `_PATCH` and `_STR`
- `VSCP_FIRMWARE_LIB_VERSION` as one number for compile time checks, e.g.
  `#if VSCP_FIRMWARE_LIB_VERSION >= VSCP_FIRMWARE_LIB_VERSION_CODE(26, 11, 3)`

It is updated automatically on every push to `master` by the
[Update version](https://github.com/grodansparadis/vscp-firmware/actions/workflows/update-version.yml)
workflow: the patch number counts the pushes in a month and starts at 0 in a
new month. Do not edit the header by hand. Pushes whose commit message
contains `[skip ci]` (anywhere, also in the body) do not change the version;
the workflow can also be started by hand from the Actions tab.

This is the version of the library code, not the VSCP protocol version that
nodes report in registers 0x81/0x82 (`VSCP_MAJOR_VERSION` etc. in
`vscp-firmware.h`).

## CI

- Tests workflow: [GitHub Actions - Tests](https://github.com/grodansparadis/vscp-firmware/actions/workflows/tests.yml)
- Version workflow: [GitHub Actions - Update version](https://github.com/grodansparadis/vscp-firmware/actions/workflows/update-version.yml)
- Latest workflow runs: [GitHub Actions runs](https://github.com/grodansparadis/vscp-firmware/actions)

The main reference implementations for VSCP over CAN is the Paris and the Kelvin modules. Both can found [here](http://www.grodansparadis.com/products.html) along with other open source products. All modules are available as 
ready made modules, bare pcb's and software and hardware is and will always be fully open and free. All design documents are open and available.

## License

The whole source code is published under the MIT license. Consider the different licenses of possible third party libraries too!

## Contribution

Unless you explicitly state otherwise, any contribution intentionally submitted for inclusion in the work by you, shall be licensed as above, without any additional terms or conditions.

Copyright 2000-2026 Ake Hedman <akhe@grodansparadis.com>, <a href="http://www.grodansparadis.com">Grodans Paradis AB</a>
