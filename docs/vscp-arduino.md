# VSCP on Arduino

The `VscpFirmware` Arduino library (in [arduino/VscpFirmware](../arduino/VscpFirmware))
makes an Arduino a VSCP node on a CAN bus (CAN4VSCP, VSCP Level I). The
VSCP protocol work is done by [vscp-firmware-level2](vscp-firmware-level2.md),
the same code that runs on other VSCP nodes. The library connects it to the
Arduino world:

- **Protocol:** nickname discovery, heartbeats, the standard registers (GUID,
  MDF URL, firmware version ...), register read/write (also paged and extended
  page read/write), "who is there", "restore defaults", and the Level I
  decision matrix.
- **Configuration:** the nickname, user id, your registers and the decision
  matrix are saved in the EEPROM and restored at start.
- **Bus:** CAN with an MCP2515 CAN controller (Arduino CAN-BUS shields and the
  common MCP2515 + TJA1050 modules) or with the CAN controller built into the
  ESP32. Other hardware can be added with a few lines (see
  [Your own transport](#your-own-transport)).

Your sketch keeps its own I/O: it reads its inputs, drives its outputs, and
tells the library which registers it has and what to do when a decision
matrix row fires.

- [Installation](#installation)
- [Hardware](#hardware)
- [A first node](#a-first-node)
- [How a node works](#how-a-node-works)
- [Registers](#registers)
- [Decision matrix](#decision-matrix)
- [Sending events](#sending-events)
- [Examples](#examples)
- [Reference](#reference)
- [Your own transport](#your-own-transport)
- [Your own storage](#your-own-storage)
- [Memory use](#memory-use)
- [Limits](#limits)
- [Testing and maintenance](#testing-and-maintenance)

## Installation

### Arduino IDE

1. Install **mcp_can** by coryjfowler with the Library Manager (needed for
   MCP2515 boards; the ESP32 transport does not use it).
2. Install the VscpFirmware library. Either:
   - copy the folder `arduino/VscpFirmware` of this repository to the
     `libraries` folder of your sketchbook (`~/Arduino/libraries/VscpFirmware`), or
   - run `tools/arduino-sync-library.sh --zip` and add the
     `arduino/VscpFirmware.zip` it creates with *Sketch > Include Library >
     Add .ZIP Library...*.
3. The examples are under *File > Examples > VSCP Firmware*.

### arduino-cli

```sh
arduino-cli lib install mcp_can
arduino-cli config set library.enable_unsafe_install true
arduino-cli lib install --zip-path arduino/VscpFirmware.zip
arduino-cli compile --fqbn arduino:avr:uno arduino/VscpFirmware/examples/Blinky
```

### PlatformIO

Point `lib_deps` to the library folder (or copy it into the `lib` folder of
your project):

```ini
[env:uno]
platform = atmelavr
board = uno
framework = arduino
monitor_speed = 115200
lib_deps =
    coryjfowler/mcp_can
    symlink:///path/to/vscp-firmware/arduino/VscpFirmware
```

## Hardware

### MCP2515 (Uno, Nano, Mega and other boards with SPI)

| MCP2515 module | Uno / Nano | Mega |
| -------------- | ---------- | ---- |
| VCC            | 5V         | 5V   |
| GND            | GND        | GND  |
| CS             | D10        | D10 (any pin) |
| SO (MISO)      | D12        | D50  |
| SI (MOSI)      | D11        | D51  |
| SCK            | D13        | D52  |
| INT            | not used   | not used |

The Arduino CAN-BUS shield uses CS on D9. Give the constructor the CS pin and
the crystal on the board: the cheap blue modules mostly have an 8 MHz
crystal (`MCP_8MHZ`), shields a 16 MHz one (`MCP_16MHZ`). With the wrong
value the node does not talk to the bus.

```cpp
VscpCanMcp2515 can(10, MCP_8MHZ);   // MCP2515 + TJA1050 module
VscpCanMcp2515 can(9, MCP_16MHZ);   // Arduino CAN-BUS shield
```

D13 is the SPI clock, so the on-board LED of an Uno can not be used while the
CAN controller is in use; the examples use an LED on D6.

### ESP32

The ESP32 has a CAN controller (called TWAI by Espressif). Connect a 3.3 V
CAN transceiver (SN65HVD230, TJA1051T/3, ...): its TX (CTX/D) input to one
GPIO, its RX (CRX/R) output to another.

```cpp
VscpCanTwai can(GPIO_NUM_5, GPIO_NUM_4);   // TX pin, RX pin
```

### The bus

CAN4VSCP runs at 125 kbit/s with 29-bit (extended) identifiers, on a twisted
pair (CANH/CANL) with a 120 Ω resistor at each end of the bus. A node alone
on a bus can not send (nobody acknowledges its frames): connect at least one
other CAN node, for example a USB-CAN adapter with
[VSCP Works](https://github.com/grodansparadis/vscp-works-qt).

## A first node

```cpp
#include <VscpFirmware.h>
#include <VscpEepromStorage.h>
#include <VscpCanMcp2515.h>

VscpCanMcp2515 can(10, MCP_8MHZ);    // The CAN controller
VscpEepromStorage storage;           // Where the configuration is saved
VscpNode node(can, &storage);        // The VSCP node

uint8_t regs[2];                     // Register 0 = zone, 1 = subzone

void setup() {
  node.setRegisters(regs, sizeof(regs));
  node.setZoneRegisters(0, 1);
  node.begin();
}

void loop() {
  node.loop();                       // Call it often
}
```

This is a complete VSCP node. After a few seconds it has a nickname, sends a
heartbeat every minute, shows up in VSCP Works' node scan, and its registers
can be read and written. The zone and subzone you write survive a restart.

## How a node works

**Nickname.** Every node on a VSCP CAN bus has a nickname (1-254) that is
part of the CAN identifier of its frames. A new node finds a free nickname by
itself (nickname discovery) and saves it. `node.startNicknameDiscovery()`
starts the search again (VSCP nodes do that when their "init button" is held
for a few seconds). Until it has a nickname `node.isActive()` is false and
`sendEvent()` returns `VSCP_ERROR_NOT_CONNECTED`.

**GUID.** Every VSCP node has a globally unique 16-byte id. By default the
library uses the development range `FF:FF:FF:FF:FF:FF:FF:FD:00:...` with two
random bytes at the end (chosen at the first start and saved). For a product
use a GUID range you own:

```cpp
const uint8_t prefix[14] = { ... };   // Your range
node.setGuidPrefix(prefix);           // The last two bytes are random and saved
// or
node.setGuid(guid);                   // All 16 bytes
```

**MDF.** A module description file describes the registers, the decision
matrix actions and the events of a node, so tools like VSCP Works can show
and configure it. `node.setMdfUrl("www.example.com/mynode.mdf")` puts its URL
(max 32 characters, without `http://`) in the standard registers. The
[Blinky](#blinky) example follows [demos/blinky/mdf/blinky0.json](../demos/blinky/mdf/blinky0.json).

**Saved configuration.** With a storage object the node saves its nickname,
user id, registers and decision matrix and loads them in `begin()`. If you
change the number of registers or decision matrix rows in the sketch, the
saved registers no longer fit: the node then starts with the register
defaults (the nickname is kept). The node uses `node.storageSize()` bytes from
the storage base address (the third constructor argument, default 0); use
the EEPROM after that for your own things.

## Registers

A VSCP node is configured through byte-wide registers. Registers 0x80-0xFF
are the standard registers (handled by the library). Your registers are on
page 0 from register 0:

```cpp
uint8_t regs[4];
const uint8_t defaults[4] = { 0, 0, 100, 0x0F };   // Used the first time and on "restore defaults"

node.setRegisters(regs, 4, defaults);
```

The library answers reads and writes from the array and saves written values.
The sketch can:

- **Read values:** `regs[i]` (or `node.getRegister(i)`).
- **Change a value itself** (and save it): `node.setRegister(i, value)`.
- **Answer reads with live values** (a sensor reading, a status, a counter):

  ```cpp
  bool onRegisterRead(uint16_t page, uint32_t offset, uint8_t &value) {
    if (page == 0 && offset == 3) {
      value = digitalRead(INPUT_PIN);
      return true;               // Answered here
    }
    return false;                // The library answers from regs[]
  }
  node.onRegisterRead(onRegisterRead);
  ```

- **Check or react to writes**, and refuse writes to read-only registers:

  ```cpp
  bool onRegisterWrite(uint16_t page, uint32_t offset, uint8_t value) {
    if (page == 0 && offset == 3) {
      return false;              // Read only: the value is not changed
    }
    if (page == 0 && offset == 2) {
      analogWrite(PWM_PIN, value);
    }
    return true;                 // The library stores and saves it
  }
  node.onRegisterWrite(onRegisterWrite);
  ```

  The callbacks are also called for registers outside the array (other pages,
  offsets after the array), so the sketch can have registers of its own.

Registers 0 and 1 are zone and subzone in most VSCP nodes. Tell the library
with `node.setZoneRegisters(0, 1)`; they are then used by `sendZoneEvent()`
and `node.zone()`/`node.subzone()`.

## Decision matrix

The decision matrix lets a node react to events from other nodes without
code changes: each row says "when an event of this class/type (from this
node) arrives, do action N with parameter P". The rows are registers, so
they are set up over the bus (with VSCP Works). The library compares every
received event with the rows; your sketch only carries out the actions.

```cpp
const uint8_t DM_ROWS = 4;
uint8_t dm[DM_ROWS * 8];                 // Eight bytes per row

void onAction(uint8_t action, uint8_t param, const vscp_event_t &ev) {
  switch (action) {
    case 1: digitalWrite(RELAY, HIGH); break;
    case 2: digitalWrite(RELAY, LOW);  break;
  }
}

node.setDecisionMatrix(dm, DM_ROWS);     // Page 0, register 0x60 (default)
node.onAction(onAction);
```

A row is the standard Level I row:

| Byte | Content |
| ---- | ------- |
| 0 | Nickname the event must come from (if flag bit 6 is set) |
| 1 | Flags: bit 7 row enabled, bit 6 check the nickname (byte 0), bit 5 the sender must have a hard coded nickname, bit 4 the event's zone must match, bit 3 its subzone must match, bit 1 class bit 8 of the mask, bit 0 class bit 8 of the filter |
| 2 | Class mask (bits 0-7) |
| 3 | Class filter (bits 0-7) |
| 4 | Type mask |
| 5 | Type filter |
| 6 | Action |
| 7 | Action parameter |

Example: action 1 for every CLASS1.CONTROL (30) TURN-ON (5) event is the row
`00 80 FF 1E FF 05 01 00`. Put the actions and their parameters in your MDF so
VSCP Works can show them by name.

## Sending events

```cpp
// Any event: class, type, data, length, priority (0 highest - 7 lowest, default 3)
uint8_t data[3] = { 0, 1, 2 };
node.sendEvent(VSCP_CLASS1_CONTROL, VSCP_TYPE_CONTROL_TURNON, data, 3);

// Events with the common layout index, zone, subzone
node.sendZoneEvent(VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON);       // index 0
node.sendZoneEvent(VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_OFF, 2);   // index 2
```

The class and type names are in `vscp-class.h` and `vscp-type.h` (included
by `VscpFirmware.h`). Some common ones:

| Event | Class / type | Data |
| ----- | ------------ | ---- |
| Something turned on/off | `VSCP_CLASS1_INFORMATION` / `VSCP_TYPE_INFORMATION_ON`, `_OFF` | index, zone, subzone |
| Button pressed/released | `VSCP_CLASS1_INFORMATION` / `VSCP_TYPE_INFORMATION_BUTTON` | 0 released / 1 pressed, zone, subzone, code MSB, code LSB |
| Ask others to turn on/off | `VSCP_CLASS1_CONTROL` / `VSCP_TYPE_CONTROL_TURNON`, `_TURNOFF` | user byte, zone, subzone |
| Temperature | `VSCP_CLASS1_MEASUREMENT` / `VSCP_TYPE_MEASUREMENT_TEMPERATURE` | data coding byte + value (see below) |

A measurement starts with a [data coding byte](https://grodansparadis.github.io/vscp-doc-spec/#/./vscp_measurements?id=datacoding):
bits 7-5 the coding, bits 4-3 the unit, bits 2-0 the sensor index. A
temperature in °C as a 32-bit float:

```cpp
union { float f; uint32_t u; } v;
v.f = 21.5;
uint8_t data[5] = { 0xA0 | (1 << 3) | 0,   // float, unit 1 (Celsius), sensor 0
                    (uint8_t) (v.u >> 24), (uint8_t) (v.u >> 16),
                    (uint8_t) (v.u >> 8),  (uint8_t) v.u };
node.sendEvent(VSCP_CLASS1_MEASUREMENT, VSCP_TYPE_MEASUREMENT_TEMPERATURE, data, 5);
```

To see the events on the bus:

```cpp
void onEvent(const vscp_event_t &ev) {
  // ev.vscp_class, ev.vscp_type, ev.GUID[15] (nickname of the sender),
  // ev.sizeData, ev.pdata[0..]
}
node.onEvent(onEvent);
```

## Examples

The examples are in [arduino/VscpFirmware/examples](../arduino/VscpFirmware/examples).

### MinimalNode

The smallest useful node: nickname, heartbeat, zone/subzone registers, and
every event on the bus printed on the serial port. Use it to check the
wiring and the bus first.

### Blinky

The VSCP demo node, following the Blinky MDF
([demos/blinky/mdf/blinky0.json](../demos/blinky/mdf/blinky0.json)), so VSCP
Works shows its registers, decision matrix actions and events by name. An LED
(D6) is turned on/off, toggled and blinked from the bus, and a button (D7)
sends events.

| Register | Content |
| -------- | ------- |
| 0, 1 | Zone, subzone |
| 2 | Status: bit 1 LED on, bit 0 button pressed (read only) |
| 3 | Control: bit 7 LED enabled, bit 6 button enabled, bit 3 send CLASS1.CONTROL TURN-ON when pressed, bit 2 send CLASS1.INFORMATION START when pressed, bit 1 send STOP when released, bit 0 millisecond counter running (default `0b11001110`) |
| 4, 5 | Blink interval in ms (default 500) |
| 6-9 | Millisecond counter (read only, write to clear) |
| 10-12 | Byte 0, zone and subzone of the button events |
| 0x60-0x7F | Decision matrix, 4 rows |

Decision matrix actions: 1 LED on/off (parameter 0 off, 1 on, 2 toggle),
2 on, 3 off, 4 toggle, 5 start blinking, 6 stop blinking, 7 clear the
millisecond counter. LED changes made from the bus are reported with
CLASS1.INFORMATION ON/OFF. Hold the button for two seconds to get a new
nickname.

To let the LED follow another node's TURN-ON/TURN-OFF events, write these two
rows (registers 0x60-0x6F):

```
00 80 FF 1E FF 05 02 00    CLASS1.CONTROL TURN-ON  -> action 2 (on)
00 80 FF 1E FF 06 03 00    CLASS1.CONTROL TURN-OFF -> action 3 (off)
```

### TemperatureSensor

Sends the temperature of a TMP36 sensor (A0) as CLASS1.MEASUREMENT
temperature in °C (32-bit float) at an interval set in a register, and shows
it in a read-only register.

### Esp32Twai

An ESP32 node with the built-in CAN controller: an output controlled by the
decision matrix (and by writing a register), and a button that sends
CLASS1.INFORMATION Button events. The output state is saved.

## Reference

### VscpNode

| Function | |
| -------- | - |
| `VscpNode(transport, storage = nullptr, storageBase = 0)` | Without storage nothing is saved (a new nickname after every start). |
| `setGuid(guid16)` / `setGuidPrefix(prefix14)` | The node GUID (see [How a node works](#how-a-node-works)). |
| `setMdfUrl(url)` | URL of the MDF, max 32 characters. |
| `setFirmwareVersion(major, minor, sub)` | Shown in the standard registers. |
| `setManufacturerId(id, subId)`, `setDeviceFamily(family, type)` | Standard register values. |
| `setHeartbeatInterval(ms)` | Default 60000, 0 = none. |
| `setRegisters(regs, count, defaults = nullptr)` | Your registers, page 0 from 0. |
| `setDecisionMatrix(dm, rows, page = 0, offset = 0x60)` | Level I decision matrix, `rows * 8` bytes. |
| `setZoneRegisters(zoneReg, subzoneReg)` | Registers that hold zone and subzone (-1 = none). |
| `onRegisterRead(cb)`, `onRegisterWrite(cb)` | See [Registers](#registers). |
| `onAction(cb)` | A decision matrix row fired. |
| `onEvent(cb)` | Every received event. |
| `onRestoreDefaults(cb)` | After "restore defaults" (registers are back to the defaults, the matrix is cleared). |
| `onBootloader(cb)` | "Enter boot loader" was received (default: the board restarts). |
| `begin()` | Start the bus and the protocol. False if the CAN controller does not answer. |
| `loop()` | Call from `loop()` as often as possible. |
| `sendEvent(class, type, data, len, priority = 3)` | Returns `VSCP_ERROR_SUCCESS` or an error code. |
| `sendZoneEvent(class, type, index = 0, priority = 3)` | Data: index, zone, subzone. |
| `setRegister(offset, value)`, `getRegister(offset)` | Change/read a register of the array. |
| `startNicknameDiscovery()` | Find a new nickname. |
| `restoreDefaults()` | Registers to the defaults, matrix cleared. |
| `isActive()`, `state()`, `stateName()`, `nickname()`, `zone()`, `subzone()` | Node status. |
| `storageSize()` | Storage bytes the node uses. |
| `context()` | The `vscp_frmw2_firmware_context_t` for anything else (see [vscp-firmware-level2](vscp-firmware-level2.md)). |

All callbacks are plain functions (no lambdas with captures), so they work on
every board.

### Transports and storage

| Class | Header | |
| ----- | ------ | - |
| `VscpCanMcp2515(csPin, crystal = MCP_16MHZ, speed = CAN_125KBPS)` | `VscpCanMcp2515.h` | MCP2515 via the mcp_can library. `can()` gives the `MCP_CAN` object. |
| `VscpCanTwai(txPin, rxPin)` | `VscpCanTwai.h` | ESP32 CAN controller. Recovers from bus-off by itself. |
| `VscpEepromStorage` | `VscpEepromStorage.h` | Arduino EEPROM (AVR), EEPROM emulation in flash (ESP32, ESP8266, RP2040, STM32). |

## Your own transport

Anything that can move CAN4VSCP frames (or VSCP events) can be a transport:
implement the three functions of `VscpTransport`. For CAN, `VscpCan4Vscp.h`
has the conversion between a VSCP event and a 29-bit CAN identifier:

```cpp
#include <VscpFirmware.h>
#include <VscpCan4Vscp.h>

class MyCan : public VscpTransport {
public:
  bool begin() override {
    return myCanStart(125000);                       // Extended frames, 125 kbit/s
  }

  int send(const vscp_event_t &ev) override {
    if (!vscpCanFits(ev)) return VSCP_ERROR_PARAMETER;
    return myCanSend(vscpCanId(ev), ev.pdata, ev.sizeData) ? VSCP_ERROR_SUCCESS
                                                           : VSCP_ERROR_TRM_FULL;
  }

  bool receive(vscp_event_t &ev, uint16_t bufsize) override {
    uint32_t id; uint8_t data[8]; uint8_t len; bool extended;
    if (!myCanReceive(&id, data, &len, &extended) || !extended) return false;
    vscpEventFromCan(ev, id, data, (len > bufsize) ? bufsize : len);
    return true;
  }
};
```

`receive()` must not block: return false when nothing is waiting.

## Your own storage

```cpp
class MyFram : public VscpStorage {
public:
  uint8_t read(uint16_t address) override { return fram.read8(address); }
  void write(uint16_t address, uint8_t value) override { fram.write8(address, value); }
};
```

`begin(size)` (prepare `size` bytes) and `commit()` (make writes permanent)
can be overridden when the storage needs them.

## Memory use

Built with PlatformIO (Arduino core for AVR 5.x, GCC 7.3):

| Sketch | Board | Flash | RAM (static) |
| ------ | ----- | ----- | ------------ |
| MinimalNode | Uno | 24.9 KB (77 %) | 650 bytes |
| TemperatureSensor | Uno | 26.3 KB (81 %) | 639 bytes |
| Blinky | Uno | 26.5 KB (82 %) | 727 bytes |
| Blinky | Mega 2560 | 27.6 KB (11 %) | 727 bytes |
| Esp32Twai | ESP32 | 297 KB (23 %) | 22 KB |

The framework also allocates the event it sends (about 550 bytes, room for a
Level II event) while it sends one, so keep about 800 bytes of RAM free on an Uno. An Uno has
room for a node with a few inputs/outputs; for more use a Mega, an ESP32 or
another larger board.

## Limits

- VSCP Level I over CAN (CAN4VSCP). `sendEvent()` refuses Level II events
  (class over 511 or more than 8 data bytes) with `VSCP_ERROR_PARAMETER`.
- There is no CAN boot loader: "enter boot loader" restarts the board (or
  calls your `onBootloader()`). Firmware is updated over USB as usual.
- The register array is on page 0 (max 128 registers below the standard
  registers). Use `onRegisterRead()`/`onRegisterWrite()` for registers on
  other pages.
- The node does not report "events of interest"; it handles all events.

## Testing and maintenance

- `tests/arduino/run.sh` builds the library, its copy of the framework and the
  Blinky example on the PC with Arduino and mcp_can stand-ins and runs
  scripted CAN traffic against it: nickname discovery (also with a nickname
  taken), register read/write, extended page write of the decision matrix,
  decision matrix actions, button events, who-is-there, heartbeats, restart
  and "restore defaults". It runs in CI, and CI also builds all examples
  with PlatformIO for Uno, Mega and ESP32.
- An Arduino library must contain all its files, so the library holds copies
  of `vscp-firmware-level2.*`, `vscp-firmware-helper.*`, `vscp.h`,
  `vscp-class.h`, `vscp-type.h` and `vscp-aes.h` from `src/`. After changing
  one of them run `tools/arduino-sync-library.sh`; `--check` (run by the
  tests) fails when a copy differs.
