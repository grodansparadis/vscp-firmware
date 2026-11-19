// Host test of the VscpFirmware Arduino library
//
// Builds the library (arduino/VscpFirmware/src, with its copy of the
// vscp-firmware framework) and the Blinky example with Arduino and mcp_can
// stand-ins (shim/), then runs scripted CAN4VSCP traffic against it.
// Run with tests/arduino/run.sh.

#include "Arduino.h"
#include "EEPROM.h"
#include "mcp_can.h"

uint32_t sim_millis = 0;
int sim_pin[64];
SimSerial Serial;
SimEEPROM EEPROM;
std::deque<SimFrame> sim_rx;
std::vector<SimFrame> sim_tx;

// The library
#include "../../arduino/VscpFirmware/src/VscpEepromStorage.cpp"
#include "../../arduino/VscpFirmware/src/VscpCanMcp2515.cpp"
#include "../../arduino/VscpFirmware/src/VscpNode.cpp"

// The Blinky example, with setup()/loop() renamed by run.sh
#include "blinky-example.cpp"

static int failures = 0;
#define CHECK(cond, msg)                                                                                               \
  do {                                                                                                                 \
    if (cond) {                                                                                                        \
      printf("  PASS %s\n", msg);                                                                                      \
    }                                                                                                                  \
    else {                                                                                                             \
      printf("  FAIL %s\n", msg);                                                                                      \
      failures++;                                                                                                      \
    }                                                                                                                  \
  } while (0)

static uint32_t
mkid(uint8_t prio, uint16_t cls, uint8_t type, uint8_t nick)
{
  return ((uint32_t) prio << 26) | ((uint32_t) cls << 16) | ((uint32_t) type << 8) | nick;
}
static uint16_t
cls(const SimFrame &f)
{
  return (f.id >> 16) & 0x1ff;
}
static uint8_t
typ(const SimFrame &f)
{
  return (f.id >> 8) & 0xff;
}
static uint8_t
nick(const SimFrame &f)
{
  return f.id & 0xff;
}

static void
inject(uint16_t c, uint8_t t, uint8_t from, std::initializer_list<uint8_t> d, bool ext = true)
{
  SimFrame f{};
  f.id  = mkid(3, c, t, from);
  f.ext = ext;
  f.len = d.size();
  int i = 0;
  for (auto b : d) {
    f.data[i++] = b;
  }
  sim_rx.push_back(f);
}

static void
run(uint32_t ms, uint32_t step = 10)
{
  for (uint32_t t = 0; t < ms; t += step) {
    blinky_loop();
    sim_millis += step;
  }
}

static int
count(size_t from, uint16_t c, uint8_t t)
{
  int n = 0;
  for (size_t i = from; i < sim_tx.size(); i++) {
    if (cls(sim_tx[i]) == c && typ(sim_tx[i]) == t) {
      n++;
    }
  }
  return n;
}

static const SimFrame *
last(size_t from, uint16_t c, uint8_t t)
{
  const SimFrame *p = nullptr;
  for (size_t i = from; i < sim_tx.size(); i++) {
    if (cls(sim_tx[i]) == c && typ(sim_tx[i]) == t) {
      p = &sim_tx[i];
    }
  }
  return p;
}

// Read a register over the bus (CLASS1.PROTOCOL read -> RW response)
static int
readReg(uint8_t reg)
{
  size_t m = sim_tx.size();
  inject(0, VSCP_TYPE_PROTOCOL_READ_REGISTER, 0x10, { (uint8_t) node.nickname(), reg });
  run(50);
  const SimFrame *r = last(m, 0, VSCP_TYPE_PROTOCOL_RW_RESPONSE);
  return (r && r->len == 2 && r->data[0] == reg) ? r->data[1] : -1;
}

// Write a register over the bus; returns the value in the response
static int
writeReg(uint8_t reg, uint8_t val)
{
  size_t m = sim_tx.size();
  inject(0, VSCP_TYPE_PROTOCOL_WRITE_REGISTER, 0x10, { (uint8_t) node.nickname(), reg, val });
  run(50);
  const SimFrame *r = last(m, 0, VSCP_TYPE_PROTOCOL_RW_RESPONSE);
  return (r && r->len == 2 && r->data[0] == reg) ? r->data[1] : -1;
}

// Simulated power cycle: RAM is lost, EEPROM is kept
static void
reboot()
{
  node.context().state    = FRMW2_STATE_NONE;
  node.context().substate = FRMW2_SUBSTATE_NONE;
  node.context().nickname = 0xffff;
  memset(regs, 0, sizeof(regs));
  memset(dm, 0, sizeof(dm));
  sim_tx.clear();
  sim_rx.clear();
  blinky_setup();
}

int
main()
{
  sim_pin[BUTTON_PIN] = HIGH; // Not pressed (pull-up)

  printf("== 1. First start, nobody answers: nickname discovery\n");
  blinky_setup();
  run(20000);
  CHECK(node.isActive(), "node is ACTIVE");
  CHECK(1 == node.nickname(), "nickname 1");
  const SimFrame *nno = last(0, 0, VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE);
  CHECK(nno && 1 == nick(*nno), "NEW_NODE_ONLINE sent from nickname 1");
  CHECK(0 == EEPROM.read(2) && 1 == EEPROM.read(3), "nickname saved");
  {
    bool allExt = true;
    for (auto &f : sim_tx) {
      allExt = allExt && f.ext;
    }
    CHECK(allExt, "all frames are extended CAN frames");
  }

  printf("== 2. Registers hold the defaults\n");
  CHECK(0xCE == readReg(REG_CONTROL), "control = 0b11001110");
  CHECK(0x01 == readReg(REG_INTERVAL_MSB) && 0xF4 == readReg(REG_INTERVAL_LSB), "blink interval 500 ms");
  CHECK(1 == readReg(REG_BUTTON_ZONE), "button zone 1");

  printf("== 3. Standard registers\n");
  CHECK(0xFF == readReg(0xD0), "GUID byte 0 = 0xFF");
  CHECK(1 == readReg(0x94), "firmware major version 1");

  printf("== 4. Write registers\n");
  CHECK(0x02 == writeReg(REG_INTERVAL_MSB, 0x02), "write interval MSB answered with 0x02");
  CHECK(0x02F4 == blinkInterval(), "interval is 0x02F4");
  CHECK(0x02 == EEPROM.read(14 + REG_INTERVAL_MSB), "register saved");
  CHECK(0x05 == writeReg(REG_ZONE, 0x05) && 5 == node.zone(), "zone register is the node zone");
  CHECK(0 == writeReg(REG_STATUS, 0x77), "status is read only (answer holds the real value)");

  printf("== 5. Extended page write for another node is ignored\n");
  inject(0, VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_WRITE, 0x10, { 0x02, 0x00, 0x00, 0x60, 0xAA, 0xBB });
  run(50);
  CHECK(0 == dm[0] && 0 == dm[1], "decision matrix unchanged");

  printf("== 6. Program decision matrix row 0 (page 0, offset 0x60):\n"
         "      CLASS1.CONTROL TURN-ON -> action 2 (LED on)\n");
  size_t m = sim_tx.size();
  // oaddr, flags (enabled), class mask, class filter, type mask, type filter, action, param
  inject(0, VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_WRITE, 0x10, { 0x01, 0x00, 0x00, 0x60, 0x00, 0x80, 0xFF, 0x1E });
  inject(0, VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_WRITE, 0x10, { 0x01, 0x00, 0x00, 0x64, 0xFF, 0x05, 0x02, 0x00 });
  run(50);
  CHECK(0x80 == dm[1] && 0x1E == dm[3] && 0x02 == dm[6], "row 0 written");
  CHECK(0x02 == EEPROM.read(14 + REG_COUNT + 6), "decision matrix saved");
  CHECK(2 == count(m, 0, VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_RESPONSE), "two extended page responses");
  CHECK(0x1E == readReg(0x63), "row 0 reads back as register 0x63");

  printf("== 7. TURN-ON from another node: LED on, ON event\n");
  m = sim_tx.size();
  inject(VSCP_CLASS1_CONTROL, VSCP_TYPE_CONTROL_TURNON, 0x10, { 0x00, 0x00, 0x00 });
  run(50);
  CHECK(HIGH == sim_pin[LED_PIN], "LED is on");
  const SimFrame *r = last(m, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_ON);
  CHECK(r && 1 == nick(*r) && 3 == r->len && 5 == r->data[1], "CLASS1.INFORMATION ON (zone 5) from nickname 1");
  CHECK(0x02 == readReg(REG_STATUS), "status bit 1 (LED on)");

  printf("== 8. A standard (11-bit) CAN frame is not a VSCP event\n");
  sim_pin[LED_PIN] = LOW;
  ledOn            = false;
  inject(VSCP_CLASS1_CONTROL, VSCP_TYPE_CONTROL_TURNON, 0x10, { 0x00, 0x00, 0x00 }, false);
  run(50);
  CHECK(LOW == sim_pin[LED_PIN], "LED unchanged");

  printf("== 9. Button: TURN-ON and START when pressed, STOP when released\n");
  m                   = sim_tx.size();
  sim_pin[BUTTON_PIN] = LOW;
  run(200);
  r = last(m, VSCP_CLASS1_CONTROL, VSCP_TYPE_CONTROL_TURNON);
  CHECK(r && 3 == r->len && 1 == r->data[0] && 1 == r->data[1] && 1 == r->data[2], "TURN-ON 1,1,1");
  r = last(m, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_START);
  CHECK(r && 0 == r->data[0] && 1 == r->data[1], "START 0,1,1");
  CHECK(0x01 == (readReg(REG_STATUS) & 0x01), "status bit 0 (button pressed)");
  m                   = sim_tx.size();
  sim_pin[BUTTON_PIN] = HIGH;
  run(200);
  CHECK(1 == count(m, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_STOP), "STOP sent");

  printf("== 10. Who is there\n");
  m = sim_tx.size();
  inject(0, VSCP_TYPE_PROTOCOL_WHO_IS_THERE, 0x10, { 0x01 });
  run(50);
  CHECK(7 == count(m, 0, VSCP_TYPE_PROTOCOL_WHO_IS_THERE_RESPONSE), "7 WHO_IS_THERE_RESPONSE frames");
  {
    std::vector<const SimFrame *> wr;
    for (size_t i = m; i < sim_tx.size(); i++) {
      if (VSCP_TYPE_PROTOCOL_WHO_IS_THERE_RESPONSE == typ(sim_tx[i])) {
        wr.push_back(&sim_tx[i]);
      }
    }
    bool bOrder = (7 == wr.size());
    for (size_t i = 0; bOrder && (i < wr.size()); i++) {
      bOrder = (8 == wr[i]->len) && (i == wr[i]->data[0]);
    }
    CHECK(bOrder, "frames 0-6 with eight bytes each");
    CHECK(bOrder && (node.context().guid[15] == wr[0]->data[1]) && (node.context().guid[0] == wr[2]->data[2]),
          "GUID15 first, GUID0 in frame 2");
    CHECK(bOrder && (0 == wr[6]->data[7]), "last byte is 0");
  }

  printf("== 11. Another node probes our nickname: PROBE_ACK\n");
  m = sim_tx.size();
  inject(0, VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE, 0xFF, { 0x01 });
  run(50);
  CHECK(1 == count(m, 0, VSCP_TYPE_PROTOCOL_PROBE_ACK), "PROBE_ACK sent");

  printf("== 12. Heartbeat within 60 s\n");
  m = sim_tx.size();
  run(61000, 50);
  CHECK(count(m, VSCP_CLASS1_INFORMATION, VSCP_TYPE_INFORMATION_NODE_HEARTBEAT) >= 1, "NODE_HEARTBEAT sent");

  printf("== 13. Restart keeps nickname, registers and decision matrix\n");
  reboot();
  run(500);
  CHECK(node.isActive() && 1 == node.nickname(), "ACTIVE with nickname 1 right after start");
  CHECK(0x02F4 == blinkInterval() && 5 == node.zone(), "registers restored");
  CHECK(0x02 == dm[6], "decision matrix restored");

  printf("== 14. Restore defaults (0x55 then 0xAA to register 0xA2)\n");
  writeReg(0xA2, 0x55);
  writeReg(0xA2, 0xAA);
  run(100);
  CHECK(0x01F4 == blinkInterval() && 0 == node.zone(), "registers back to defaults");
  CHECK(0 == dm[6], "decision matrix cleared");
  CHECK(1 == node.nickname(), "nickname kept");

  printf("== 15. Hold the button two seconds: new nickname discovery\n");
  sim_pin[BUTTON_PIN] = LOW;
  run(2500);
  CHECK(!node.isActive(), "discovery started");
  sim_pin[BUTTON_PIN] = HIGH;
  run(20000);
  CHECK(node.isActive() && 1 == node.nickname(), "nickname 1 again (nobody else on the bus)");

  printf("== 16. Empty EEPROM, node 1 already on the bus: we take 2\n");
  EEPROM.erase();
  reboot();
  run(5200);
  inject(0, VSCP_TYPE_PROTOCOL_PROBE_ACK, 0x01, {});
  run(20000);
  CHECK(node.isActive() && 2 == node.nickname(), "nickname 2");
  CHECK(0xCE == regs[REG_CONTROL], "defaults on a new EEPROM");

  printf("\n%s (%d failure%s)\n", failures ? "FAILED" : "ALL PASSED", failures, (1 == failures) ? "" : "s");
  return failures ? 1 : 0;
}
