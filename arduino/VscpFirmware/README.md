# VSCP Firmware for Arduino

Make an Arduino a [VSCP](https://www.vscp.org) node on a CAN bus (CAN4VSCP,
VSCP Level I): nickname discovery, heartbeats, standard registers, register
read/write, the decision matrix and a saved configuration, built on
vscp-firmware-level2.

```cpp
#include <VscpFirmware.h>
#include <VscpEepromStorage.h>
#include <VscpCanMcp2515.h>

VscpCanMcp2515 can(10, MCP_8MHZ);   // MCP2515 module: CS pin, crystal
VscpEepromStorage storage;
VscpNode node(can, &storage);
uint8_t regs[2];                    // zone, subzone

void setup() {
  node.setRegisters(regs, sizeof(regs));
  node.setZoneRegisters(0, 1);
  node.begin();
}

void loop() {
  node.loop();
}
```

- CAN with an MCP2515 (needs the **mcp_can** library by coryjfowler) or the
  CAN controller of the ESP32
- Examples: MinimalNode, Blinky (the VSCP demo node, with MDF),
  TemperatureSensor, Esp32Twai

Documentation: [docs/vscp-arduino.md](../../docs/vscp-arduino.md) in the
vscp-firmware repository.

The files `vscp-firmware-level2.*`, `vscp-firmware-helper.*`, `vscp.h`,
`vscp-class.h`, `vscp-type.h` and `vscp-aes.h` are copies of the files in
`src/` of vscp-firmware (`tools/arduino-sync-library.sh` updates them).

MIT license.
