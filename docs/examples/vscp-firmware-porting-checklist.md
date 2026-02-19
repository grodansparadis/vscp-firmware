# VSCP Level I firmware porting checklist

Use this checklist when porting `common/vscp-firmware.*` to a new target.

## Transport callbacks

- [ ] Implement `getVSCPFrame(...)`.
- [ ] Implement `sendVSCPFrame(...)`.
- [ ] Verify bus arbitration/error handling when `sendVSCPFrame` fails.

## Persistent storage and identity

- [ ] Implement `vscp_readNicknamePermanent` / `vscp_writeNicknamePermanent`.
- [ ] Implement `vscp_getGUID` (and `vscp_setGUID` if protected writes are enabled).
- [ ] Implement user/manufacturer getters/setters.
- [ ] Implement `vscp_getControlByte` / `vscp_setControlByte` markers (`0x55`, `0xAA`).
- [ ] Implement `vscp_init_pstorage`.

## Register and page access

- [ ] Implement `vscp_readAppReg` / `vscp_writeAppReg`.
- [ ] Implement page-select support used by your register backend.
- [ ] Verify page restore behavior after extended page read/write operations.

## Device metadata callbacks

- [ ] Implement version callbacks (`vscp_getMajorVersion`, `vscp_getMinorVersion`, `vscp_getSubMinorVersion`).
- [ ] Implement `vscp_getBootLoaderAlgorithm`, `vscp_getBufferSize`, `vscp_getRegisterPagesUsed`.
- [ ] Implement `vscp_getMDF_URL`.
- [ ] Implement `vscp_getZone` / `vscp_getSubzone`.
- [ ] Implement family/type/firmware code callbacks.

## Control and protocol support callbacks

- [ ] Implement `vscp_getMatrixInfo`.
- [ ] Implement `vscp_goBootloaderMode`.
- [ ] Implement `vscp_restoreDefaults` (0x55/0xAA handshake window).
- [ ] If `EMBEDDED_MDF` is used, implement `vscp_getEmbeddedMdfInfo`.

## Timebase integration

- [ ] Increment `vscp_timer` every 1 ms.
- [ ] Maintain `vscp_configtimer` with the same timebase.
- [ ] Maintain `vscp_initbtncnt` / `vscp_statuscnt` if used by board logic.
- [ ] Call `vscp_doOneSecondWork()` once per second.

## Startup and state-machine validation

- [ ] Call `vscp_init()` during boot.
- [ ] Call `vscp_check_pstorage()` after init.
- [ ] Verify startup path: stored nickname vs probe-based assignment.
- [ ] Verify transition to `VSCP_STATE_ACTIVE` and `NEW_NODE_ONLINE` emission.

## Protocol compliance smoke tests

- [ ] `READ_REGISTER` and `WRITE_REGISTER` work for app (`<0x80`) and standard (`>=0x80`) ranges.
- [ ] `PAGE_READ` / `PAGE_WRITE` return expected `RW_PAGE_RESPONSE` payloads.
- [ ] `WHO_IS_THERE` response includes GUID and MDF URL fragments.
- [ ] `SET_NICKNAME` and `DROP_NICKNAME` flows behave as expected.
- [ ] `ENTER_BOOT_LOADER` condition checks and callback execution verified.

## Optional feature checks

- [ ] If `VSCP_FIRMWARE_ENABLE_ERROR_REPORTING`: validate `vscp_sendErrorEvent` frame format.
- [ ] If `VSCP_FIRMWARE_ENABLE_LOGGING`: validate `vscp_sendLogEvent` frame format.
- [ ] If `DROP_NICKNAME_EXTENDED_FEATURES`: validate hard reset/wait behavior.
