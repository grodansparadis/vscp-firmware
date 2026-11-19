// Host stand-in for the Arduino EEPROM library
#pragma once
#include <stdint.h>
struct SimEEPROM {
  uint8_t mem[1024];
  SimEEPROM() { erase(); }
  void erase() { for (auto &b : mem) b = 0xff; }
  uint8_t read(int a) { return mem[a]; }
  void write(int a, uint8_t v) { mem[a] = v; }
  void update(int a, uint8_t v) { mem[a] = v; }
};
extern SimEEPROM EEPROM;
