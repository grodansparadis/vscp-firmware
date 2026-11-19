/*
  VSCP MinimalNode - the smallest useful VSCP node

  The node finds a nickname on the bus, sends a heartbeat every minute,
  answers register reads/writes (zone and subzone in registers 0 and 1) and
  prints every event it sees on the serial port (115200 baud). A good first
  test of the wiring and the bus.

  Hardware
    - Arduino Uno (or any board with SPI)
    - MCP2515 CAN module or Arduino CAN-BUS shield on a CAN4VSCP bus
      (125 kbit/s)
*/

#include <VscpFirmware.h>
#include <VscpEepromStorage.h>
#include <VscpCanMcp2515.h>

VscpCanMcp2515 can(10, MCP_8MHZ); // CS pin, crystal of the CAN module
VscpEepromStorage storage;
VscpNode node(can, &storage);

uint8_t regs[2]; // zone, subzone

// Print every event on the bus
void
onEvent(const vscp_event_t &ev)
{
  Serial.print(F("Event class "));
  Serial.print(ev.vscp_class);
  Serial.print(F(" type "));
  Serial.print(ev.vscp_type);
  Serial.print(F(" from nickname 0x"));
  Serial.print(ev.GUID[15], HEX);
  Serial.print(F(" data"));
  for (uint8_t i = 0; i < ev.sizeData; i++) {
    Serial.print(' ');
    Serial.print(ev.pdata[i], HEX);
  }
  Serial.println();
}

void
setup()
{
  Serial.begin(115200);

  node.setRegisters(regs, sizeof(regs));
  node.setZoneRegisters(0, 1);
  node.onEvent(onEvent);

  if (!node.begin()) {
    Serial.println(F("CAN controller not found"));
  }
}

void
loop()
{
  static uint8_t lastState = 0xff;

  node.loop();

  if (node.state() != lastState) {
    lastState = node.state();
    Serial.print(F("State "));
    Serial.print(node.stateName());
    Serial.print(F(", nickname 0x"));
    Serial.println(node.nickname(), HEX);
  }
}
