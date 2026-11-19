/*
  VSCP Blinky - the VSCP demo node on an Arduino

  An LED that can be turned on/off, toggled and made to blink from the VSCP
  bus, and a button that sends events. It follows the Blinky module
  description file (demos/blinky/mdf/blinky0.json in vscp-firmware), so
  VSCP Works can show and configure it.

  Hardware
    - Arduino Uno (or any board with SPI)
    - MCP2515 CAN module (MCP2515 + TJA1050) or an Arduino CAN-BUS shield,
      connected to a CAN4VSCP bus (125 kbit/s, terminated at both ends)
    - LED + resistor from D6 to GND (D13 is used by SPI)
    - Push button from D7 to GND

  Registers (page 0, as in the MDF)
     0  Zone                         1  Subzone
     2  Status: bit 1 LED on, bit 0 button pressed (read only)
     3  Control: bit 7 LED enabled, bit 6 button enabled,
                 bit 3 send TURN-ON when the button is pressed,
                 bit 2 send START when the button is pressed,
                 bit 1 send STOP when the button is released,
                 bit 0 millisecond counter running
     4-5   Blink interval in ms (MSB, LSB)
     6-9   Millisecond counter (MSB first, read only, write to clear)
    10-12  Byte 0, zone and subzone of the events the button sends
    0x60-0x7F  Decision matrix, 4 rows

  Decision matrix actions
    0 NOOP, 1 LED on/off (parameter 0 = off, 1 = on, 2 = toggle),
    2 LED on, 3 LED off, 4 toggle, 5 start blink, 6 stop blink,
    7 clear the millisecond counter

  Hold the button for more than two seconds to give the node a new
  nickname (the "init button" of VSCP nodes).
*/

#include <VscpFirmware.h>
#include <VscpEepromStorage.h>
#include <VscpCanMcp2515.h>

// ---------------------------------------------------------------------------
// Hardware
// ---------------------------------------------------------------------------

// Standalone MCP2515 module: CS on D10, usually an 8 MHz crystal.
// Arduino CAN-BUS shield: CS on D9 and a 16 MHz crystal.
VscpCanMcp2515 can(10, MCP_8MHZ);
// VscpCanMcp2515 can(9, MCP_16MHZ);

const uint8_t LED_PIN    = 6;
const uint8_t BUTTON_PIN = 7;

// ---------------------------------------------------------------------------
// The VSCP node
// ---------------------------------------------------------------------------

VscpEepromStorage storage;
VscpNode node(can, &storage);

enum {
  REG_ZONE = 0,
  REG_SUBZONE,
  REG_STATUS,
  REG_CONTROL,
  REG_INTERVAL_MSB,
  REG_INTERVAL_LSB,
  REG_COUNTER0, // MSB
  REG_COUNTER1,
  REG_COUNTER2,
  REG_COUNTER3, // LSB
  REG_BUTTON_BYTE0,
  REG_BUTTON_ZONE,
  REG_BUTTON_SUBZONE,
  REG_COUNT
};

const uint8_t CTRL_LED_ENABLE    = 0x80;
const uint8_t CTRL_BUTTON_ENABLE = 0x40;
const uint8_t CTRL_SEND_TURN_ON  = 0x08;
const uint8_t CTRL_SEND_START    = 0x04;
const uint8_t CTRL_SEND_STOP     = 0x02;
const uint8_t CTRL_COUNTER       = 0x01;

uint8_t regs[REG_COUNT];
const uint8_t regDefaults[REG_COUNT] = {
  0, 0,              // zone, subzone
  0,                 // status
  0b11001110,        // control
  0x01, 0xF4,        // blink interval 500 ms
  0, 0, 0, 0,        // millisecond counter
  1, 1, 1            // button event byte 0, zone, subzone
};

const uint8_t DM_ROWS = 4;
uint8_t dm[DM_ROWS * 8];

enum {
  ACTION_NOOP = 0,
  ACTION_ONOFF,
  ACTION_ON,
  ACTION_OFF,
  ACTION_TOGGLE,
  ACTION_START_BLINK,
  ACTION_STOP_BLINK,
  ACTION_CLEAR_COUNTER
};

// ---------------------------------------------------------------------------
// LED, blinking and the millisecond counter
// ---------------------------------------------------------------------------

bool ledOn         = false;
bool blinking      = false;
bool buttonDown    = false;
uint32_t lastBlink = 0;
uint32_t counterBase;

uint16_t
blinkInterval()
{
  return ((uint16_t) regs[REG_INTERVAL_MSB] << 8) | regs[REG_INTERVAL_LSB];
}

uint32_t
counter()
{
  return (regs[REG_CONTROL] & CTRL_COUNTER) ? millis() - counterBase : 0;
}

// Turn the LED on/off. Changes made from the bus are reported with
// CLASS1.INFORMATION ON/OFF events.
void
setLed(bool on, bool report)
{
  bool changed = (on != ledOn);
  ledOn        = on;
  digitalWrite(LED_PIN, on ? HIGH : LOW);
  if (report && changed) {
    node.sendZoneEvent(VSCP_CLASS1_INFORMATION, on ? VSCP_TYPE_INFORMATION_ON : VSCP_TYPE_INFORMATION_OFF);
  }
}

void
doAction(uint8_t action, uint8_t param)
{
  if (ACTION_CLEAR_COUNTER == action) {
    counterBase = millis();
    return;
  }
  if (!(regs[REG_CONTROL] & CTRL_LED_ENABLE)) {
    return; // LED control disabled
  }

  switch (action) {
    case ACTION_ONOFF:
      blinking = false;
      setLed((2 == param) ? !ledOn : (0 != param), true);
      break;
    case ACTION_ON:
      blinking = false;
      setLed(true, true);
      break;
    case ACTION_OFF:
      blinking = false;
      setLed(false, true);
      break;
    case ACTION_TOGGLE:
      blinking = false;
      setLed(!ledOn, true);
      break;
    case ACTION_START_BLINK:
      blinking  = (0 != blinkInterval());
      lastBlink = millis();
      break;
    case ACTION_STOP_BLINK:
      blinking = false;
      break;
  }
}

// ---------------------------------------------------------------------------
// Node callbacks
// ---------------------------------------------------------------------------

// A decision matrix row matched a received event
void
onAction(uint8_t action, uint8_t param, const vscp_event_t &ev)
{
  doAction(action, param);
}

// Live register values
bool
onRegisterRead(uint16_t page, uint32_t offset, uint8_t &value)
{
  if (0 != page) {
    return false;
  }
  if (REG_STATUS == offset) {
    value = (ledOn ? 0x02 : 0) | (buttonDown ? 0x01 : 0);
    return true;
  }
  if ((offset >= REG_COUNTER0) && (offset <= REG_COUNTER3)) {
    value = (counter() >> (8 * (REG_COUNTER3 - offset))) & 0xff;
    return true;
  }
  return false; // The node answers from regs[]
}

// Return false for registers that can not be written
bool
onRegisterWrite(uint16_t page, uint32_t offset, uint8_t value)
{
  if (0 != page) {
    return true;
  }
  if (REG_STATUS == offset) {
    return false; // Read only
  }
  if ((offset >= REG_COUNTER0) && (offset <= REG_COUNTER3)) {
    counterBase = millis(); // Writing the counter clears it
    return false;
  }
  if ((REG_INTERVAL_MSB == offset) || (REG_INTERVAL_LSB == offset)) {
    if (0 == value) {
      blinking = false;
    }
  }
  return true;
}

void
onRestoreDefaults()
{
  blinking = false;
  setLed(false, false);
}

// ---------------------------------------------------------------------------
// Button
// ---------------------------------------------------------------------------

void
pollButton()
{
  static bool last           = HIGH;
  static uint32_t changed    = 0;
  static uint32_t pressed    = 0;
  static bool initDone       = false;

  bool raw = digitalRead(BUTTON_PIN);
  if (raw != last) {
    last    = raw;
    changed = millis();
  }
  if ((millis() - changed) < 30) {
    return; // Debounce
  }

  bool down = (LOW == raw);
  if (down && !buttonDown) {
    // Pressed
    buttonDown = true;
    pressed    = millis();
    initDone   = false;
    if (regs[REG_CONTROL] & CTRL_BUTTON_ENABLE) {
      if (regs[REG_CONTROL] & CTRL_SEND_TURN_ON) {
        uint8_t data[3] = { regs[REG_BUTTON_BYTE0], regs[REG_BUTTON_ZONE], regs[REG_BUTTON_SUBZONE] };
        node.sendEvent(VSCP_CLASS1_CONTROL, VSCP_TYPE_CONTROL_TURNON, data, sizeof(data));
      }
      if (regs[REG_CONTROL] & CTRL_SEND_START) {
        uint8_t data[3] = { 0, regs[REG_BUTTON_ZONE], regs[REG_BUTTON_SUBZONE] };
        node.sendEvent(VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_START, data, sizeof(data));
      }
    }
  }
  else if (!down && buttonDown) {
    // Released
    buttonDown = false;
    if ((regs[REG_CONTROL] & CTRL_BUTTON_ENABLE) && (regs[REG_CONTROL] & CTRL_SEND_STOP)) {
      uint8_t data[3] = { 0, regs[REG_BUTTON_ZONE], regs[REG_BUTTON_SUBZONE] };
      node.sendEvent(VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_STOP, data, sizeof(data));
    }
  }
  else if (down && !initDone && (millis() - pressed) > 2000) {
    // Held for two seconds: new nickname
    initDone = true;
    Serial.println(F("Init button: finding a new nickname"));
    node.startNicknameDiscovery();
  }
}

// ---------------------------------------------------------------------------
// setup / loop
// ---------------------------------------------------------------------------

void
setup()
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println(F("VSCP Blinky"));

  node.setFirmwareVersion(1, 0, 0);
  node.setMdfUrl("");                  // Where blinky0.mdf is published (max 32 characters)
  node.setRegisters(regs, REG_COUNT, regDefaults);
  node.setZoneRegisters(REG_ZONE, REG_SUBZONE);
  node.setDecisionMatrix(dm, DM_ROWS); // Page 0, offset 0x60 as in the MDF
  node.onAction(onAction);
  node.onRegisterRead(onRegisterRead);
  node.onRegisterWrite(onRegisterWrite);
  node.onRestoreDefaults(onRestoreDefaults);

  while (!node.begin()) {
    Serial.println(F("CAN controller not found, check wiring and crystal"));
    delay(1000);
  }
  counterBase = millis();
}

void
loop()
{
  static uint8_t lastState = 0xff;

  node.loop();

  if (node.state() != lastState) {
    lastState = node.state();
    Serial.print(F("VSCP state "));
    Serial.print(node.stateName());
    Serial.print(F(", nickname 0x"));
    Serial.println(node.nickname(), HEX);
  }

  if (blinking && (millis() - lastBlink) >= blinkInterval()) {
    lastBlink = millis();
    setLed(!ledOn, false);
  }

  pollButton();
}
