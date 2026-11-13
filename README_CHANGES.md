# Changes in vscp-firmware

Every change that goes into a release is listed here, newest release first.
Add each change under **Unreleased** when it is made. When a release is
tagged, rename **Unreleased** to the version and date, and start a new
**Unreleased** section above it.

## Unreleased

- Fetch cJSON for the tests with CMake FetchContent instead of the third-party/cJSON submodule (same commit)
- No changes since the last release.

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
