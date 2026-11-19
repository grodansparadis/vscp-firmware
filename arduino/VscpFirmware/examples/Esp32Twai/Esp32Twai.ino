/*
  VSCP Esp32Twai - a VSCP node using the CAN controller in the ESP32

  The ESP32 has a CAN controller (TWAI); only a 3.3 V CAN transceiver
  (SN65HVD230, TJA1051T/3 ...) is needed. This node has a relay (or LED)
  output that is controlled by the decision matrix and a button input that
  sends CLASS1.INFORMATION Button events.

  Hardware
    - ESP32 board
    - CAN transceiver: CTX to GPIO 5, CRX to GPIO 4, on a CAN4VSCP bus
    - Relay/LED on GPIO 2 (the on-board LED on many boards)
    - Button from GPIO 0 (BOOT button on many boards) to GND

  Registers (page 0)
    0  Zone
    1  Subzone
    2  Output state, 0 = off, 1 = on (writing it switches the output)
    3  Button code (sent in the Button event)

  Decision matrix (page 0, offset 0x60, 8 rows)
    Action 1 = output on, 2 = output off, 3 = toggle
*/

#include <VscpFirmware.h>
#include <VscpEepromStorage.h>
#include <VscpCanTwai.h>

VscpCanTwai can(GPIO_NUM_5, GPIO_NUM_4); // TX, RX
VscpEepromStorage storage;               // Flash backed EEPROM emulation
VscpNode node(can, &storage);

const uint8_t OUTPUT_PIN = 2;
const uint8_t BUTTON_PIN = 0;

enum { REG_ZONE = 0, REG_SUBZONE, REG_OUTPUT, REG_BUTTON_CODE, REG_COUNT };
uint8_t regs[REG_COUNT];

const uint8_t DM_ROWS = 8;
uint8_t dm[DM_ROWS * 8];

void
setOutput(bool on)
{
  digitalWrite(OUTPUT_PIN, on ? HIGH : LOW);
  node.setRegister(REG_OUTPUT, on ? 1 : 0); // Saved, so the state survives a restart
  node.sendZoneEvent(VSCP_CLASS1_INFORMATION, on ? VSCP_TYPE_INFORMATION_ON : VSCP_TYPE_INFORMATION_OFF);
}

void
onAction(uint8_t action, uint8_t param, const vscp_event_t &ev)
{
  switch (action) {
    case 1:
      setOutput(true);
      break;
    case 2:
      setOutput(false);
      break;
    case 3:
      setOutput(!regs[REG_OUTPUT]);
      break;
  }
}

bool
onRegisterWrite(uint16_t page, uint32_t offset, uint8_t value)
{
  if ((0 == page) && (REG_OUTPUT == offset)) {
    setOutput(0 != value);
    return false; // setOutput() has stored it
  }
  return true;
}

void
setup()
{
  Serial.begin(115200);
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Use your own GUID range in a real product
  const uint8_t guidPrefix[14] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
  node.setGuidPrefix(guidPrefix);
  node.setRegisters(regs, REG_COUNT);
  node.setZoneRegisters(REG_ZONE, REG_SUBZONE);
  node.setDecisionMatrix(dm, DM_ROWS);
  node.onAction(onAction);
  node.onRegisterWrite(onRegisterWrite);

  if (!node.begin()) {
    Serial.println("CAN controller start failed");
  }
  digitalWrite(OUTPUT_PIN, regs[REG_OUTPUT] ? HIGH : LOW); // Saved state
}

void
loop()
{
  static bool lastButton = HIGH;

  node.loop();

  bool button = digitalRead(BUTTON_PIN);
  if (button != lastButton) {
    lastButton = button;
    delay(20); // Simple debounce
    // CLASS1.INFORMATION Button: byte 0 1 = pressed, 0 = released,
    // zone, subzone, button code (MSB, LSB)
    uint8_t data[5] = { (uint8_t) ((LOW == button) ? 1 : 0), node.zone(), node.subzone(), 0, regs[REG_BUTTON_CODE] };
    node.sendEvent(VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_BUTTON, data, sizeof(data));
  }
}
