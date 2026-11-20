# Changes in vscp-firmware

Every change that goes into a release is listed here, newest release first.
Add each change under **Unreleased** when it is made. When a release is
tagged, rename **Unreleased** to the version and date, and start a new
**Unreleased** section above it.

## Unreleased

- helper: the JSON parsers also read `vscpObId` (the obid was only read from `obid`). JSON is written with the short keys (`head`, `class`, `type`, `obid`, `guid`, `data`, ...) and read with either those or the `vscpHead`, `vscpClass`, ... keys
- fifo: `vscp_fifo_clear()` now deletes the queued events; before it only dropped the pointers (a leak), and its check of the event data was reversed so the data was never freed
- helper: `vscp_fwhlp_create_json()`/`_ex()` wrote the timestamp with `%llx`, which newlib-nano prints as "lx"; it is now written as two 32-bit halves. For Unix ns events the date/time is taken from the timestamp (it was printed from the unset 0xffff/0xff fields)
- level2: Level II decision matrix fixes: rows now match the event class and type (they triggered on every event); the 32-bit control word is read from byte 0 (it was read from byte 1, so no flag was seen right); the origin check uses VSCP_LEVEL2_DM_FLAG_CHECK_OADDR (it used the Level I value); the index is matched against data byte 0 (it used byte 1); a NULL event is refused
- level2: new `bDmLevel1Rows` in the context: a Level II device can use Level I (8-byte) decision matrix rows, as the Blinky MDF describes
- demos/blinky MDF: the status register is 1=ON, 0=OFF (said 0=ON, 1=OFF); removed the leftover "test" alarm at bit 6
- Arduino: new VscpFirmware Arduino library (`arduino/VscpFirmware`) that makes an Arduino a VSCP Level I node on a CAN bus, built on vscp-firmware-level2. `VscpNode` does nickname discovery, heartbeats, standard registers, register read/write, the decision matrix and saves the configuration (`VscpEepromStorage`); transports for the MCP2515 (`VscpCanMcp2515`, mcp_can library) and the ESP32 CAN controller (`VscpCanTwai`). Examples: MinimalNode, Blinky (follows the Blinky MDF), TemperatureSensor, Esp32Twai. Documentation in docs/vscp-arduino.md
- Arduino: `tests/arduino/run.sh` runs the library and the Blinky example against scripted CAN traffic on the PC; CI runs it and builds the examples with PlatformIO (Uno, Mega, ESP32). `tools/arduino-sync-library.sh` keeps the library's copies of the framework files in step with `src/` (`--check`, `--zip`)
- level2: the Level I "who is there" response read one byte before the GUID and one byte after the 32-byte MDF URL (found with the address sanitizer); the last byte of frame 6 is now always 0 as the specification says
- Fix the CANAL id helper declarations in vscp-firmware-helper.h: they were declared as `vscp_fwhlp__get...` (two underscores) but implemented as `vscp_fwhlp_get...`, so they could not be linked
- Fetch cJSON for the tests with CMake FetchContent instead of the third-party/cJSON submodule (same commit)
- level2: Level I compatibility fixes. Nickname probes now carry the probed nickname (they sent 0xff), the "all nicknames taken" check works, and a nickname won by probing is reported through `stdreg_change`
- level2: Level I decision matrix reads flags from row byte 1 (was byte 0, the originating address) and puts class bit 8 at 0x100
- level2: `vscp_frmw2_work()` runs the decision matrix and heartbeat/caps timers also when an event is passed in ACTIVE state
- level2: `vscp_frmw2_write_reg()` passes the selected page to `ops->write_reg` (was always 0); page select registers are writable
- level2: EXTENDED_PAGE_WRITE is only applied by the addressed node
- level2: the 100 us wait in extended page read no longer hangs when a 32-bit `get_timestamp()` wraps
- level2: standard registers user id, manufacturer id/sub id and device family/type read correctly (MSB first), Level II standard register addresses are handled, reserved ones read as zero
- helper: `vscp_fwhlp_getHeadFromCANALid` hard coded bit check no longer overflows a 16-bit int
- demos/blinky: blinky0.json is valid JSON again, its `files` section uses the object form the MDF parser reads, and the Level I heartbeat (CLASS1.INFORMATION) and Turn OFF (type 6) event codes are corrected; blinky0.mdf and blinky0.xml include the START/STOP (CLASS1.INFORMATION 25/24) correction from master
- demos/blinky: blinky0.mdf and blinky0.xml are regenerated from blinky0.json (the XML was from an older register layout); `tools/mdf-json2xml.py` does the conversion
- demos/blinky: module `level` in blinky0.json is 1 (Level I), not 0; the specification uses 1 and 2

## v26.11.4 - 2026-11-12

- Fixed invalid former reference (7f1d60c)
- Fix gtest source paths (3b5099e)
- Fix version bump header path (6cecdbe)
- Initial plan (6e3ca09)
- Fixed readme image (78327ba)
- Added images (ed5a51a)
- common folder renamed to src (0eaa9f5)
- Add TCP record layer with authenticated encryption to the binary protocol (790ac44)
- Allow starting the version update by hand (3616198)
- Add automatic library version year.month.patch (b7ed125)
- Add small footprint crypto switch for 8-bit targets (8327cfd)
- Add authenticated binary frames (AES-CBC + HMAC-SHA-256) (bdbd4d7)
- Decode CLASS2.MEASUREMENT_FLOAT MSB first and fix measurement bounds (1e5c6c2)
- tests: free parser allocated event data with free() (bdee7be)
- level2: fix out-of-bounds read and stack overflow in protocol events (52971f3)
- Clamp vscp_fwhlp_to_unix_ns() instead of overflowing (2391ea8)
- link: bound RETR line and free event on format error (0c4ffee)
- ws1/binary: require login, check AUTH sid, fix buffer overflows (fa320ce)
- Fix read past end of string in datestr parsers (3523f93)
- Bound and type check JSON event parsing (eb414c3)
- Updated shared files (94a4d13)
- Use PKCS#7 padding for AES frames (1e37de0)
- fixed logo path (a293933)
- Changed path to logo (97d4002)
- Fix VSCP_RS485_MAX_CLIENTS to 254 and update docs/tests accordingly (4995e8e)
- Initial plan (f1f8969)
- Add RS-485/RS-422 protocol support (1a86fe3)
