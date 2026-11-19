/*
  VSCP TemperatureSensor - send a measurement at a set interval

  Reads a TMP36 analog temperature sensor and sends the temperature as a
  CLASS1.MEASUREMENT, Type=6 (temperature) event in degrees Celsius, coded
  as a 32-bit float. The interval and a sensor index can be set over the bus.

  Hardware
    - Arduino Uno (or any board with SPI)
    - MCP2515 CAN module or Arduino CAN-BUS shield on a CAN4VSCP bus
    - TMP36: +Vs to 5 V, GND to GND, Vout to A0 (any analog sensor works,
      change readTemperature())

  Registers (page 0)
    0  Zone
    1  Subzone
    2  Sensor index (0-7, put in the event)
    3  Report interval in seconds (0 = off, default 30)
    4  Temperature in whole degrees Celsius, signed (read only)
*/

#include <VscpFirmware.h>
#include <VscpEepromStorage.h>
#include <VscpCanMcp2515.h>

VscpCanMcp2515 can(10, MCP_8MHZ);
VscpEepromStorage storage;
VscpNode node(can, &storage);

enum { REG_ZONE = 0, REG_SUBZONE, REG_INDEX, REG_INTERVAL, REG_TEMPERATURE, REG_COUNT };

uint8_t regs[REG_COUNT];
const uint8_t regDefaults[REG_COUNT] = { 0, 0, 0, 30, 0 };

float temperature = 0;

float
readTemperature()
{
  // TMP36: 10 mV/°C, 500 mV at 0 °C
  float volts = analogRead(A0) * (5.0 / 1023.0);
  return (volts - 0.5) * 100.0;
}

// CLASS1.MEASUREMENT data: byte 0 is the data coding, here
//   bits 7-5 101 = 32-bit float, bits 4-3 unit (1 = Celsius),
//   bits 2-0 sensor index; then the float, most significant byte first.
void
sendTemperature(float value)
{
  union {
    float f;
    uint32_t u;
  } v;
  v.f = value;

  uint8_t data[5];
  data[0] = 0xA0 | (1 << 3) | (regs[REG_INDEX] & 7);
  data[1] = (v.u >> 24) & 0xff;
  data[2] = (v.u >> 16) & 0xff;
  data[3] = (v.u >> 8) & 0xff;
  data[4] = v.u & 0xff;
  node.sendEvent(VSCP_CLASS1_MEASUREMENT, VSCP_TYPE_MEASUREMENT_TEMPERATURE, data, sizeof(data));
}

bool
onRegisterRead(uint16_t page, uint32_t offset, uint8_t &value)
{
  if ((0 == page) && (REG_TEMPERATURE == offset)) {
    value = (uint8_t) (int8_t) temperature;
    return true;
  }
  return false;
}

bool
onRegisterWrite(uint16_t page, uint32_t offset, uint8_t value)
{
  return !((0 == page) && (REG_TEMPERATURE == offset)); // Read only
}

void
setup()
{
  Serial.begin(115200);

  node.setRegisters(regs, REG_COUNT, regDefaults);
  node.setZoneRegisters(REG_ZONE, REG_SUBZONE);
  node.onRegisterRead(onRegisterRead);
  node.onRegisterWrite(onRegisterWrite);

  if (!node.begin()) {
    Serial.println(F("CAN controller not found"));
  }
}

void
loop()
{
  static uint32_t lastReport = 0;

  node.loop();

  uint32_t interval = (uint32_t) regs[REG_INTERVAL] * 1000UL;
  if (interval && node.isActive() && (millis() - lastReport) >= interval) {
    lastReport  = millis();
    temperature = readTemperature();
    sendTemperature(temperature);
    Serial.print(F("Temperature "));
    Serial.println(temperature);
  }
}
