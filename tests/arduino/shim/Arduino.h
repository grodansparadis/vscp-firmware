// Host stand-in for the parts of the Arduino API the VscpFirmware library
// and its examples use (for tests/arduino/host-test.cpp)
#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t byte;
#define HIGH         1
#define LOW          0
#define OUTPUT       1
#define INPUT        0
#define INPUT_PULLUP 2
#define A0           14
#define HEX          16
#define DEC          10
#define F(s)         (s)

extern uint32_t sim_millis;
extern int sim_pin[64];

inline uint32_t millis() { return sim_millis; }
// Advances on every call; starts near the 32-bit wrap
inline uint32_t micros()
{
  static uint32_t us = 0xFFFFFF00UL;
  us += 7;
  return us;
}
inline void delay(uint32_t ms) { sim_millis += ms; }
inline void pinMode(uint8_t, uint8_t) {}
inline void digitalWrite(uint8_t pin, uint8_t v) { sim_pin[pin] = v; }
inline int digitalRead(uint8_t pin) { return sim_pin[pin]; }
inline int analogRead(uint8_t) { return 123; }
inline void randomSeed(unsigned long s) { srand(s); }
inline long random(long n) { return rand() % n; }

struct SimSerial {
  bool quiet = true;
  void begin(long) {}
  void flush() {}
  void print(const char *s) { if (!quiet) printf("%s", s); }
  void print(char c) { if (!quiet) printf("%c", c); }
  void print(int v) { if (!quiet) printf("%d", v); }
  void print(unsigned v, int base = DEC) { if (!quiet) printf(base == HEX ? "%X" : "%u", v); }
  void print(double v) { if (!quiet) printf("%.2f", v); }
  void println() { if (!quiet) printf("\n"); }
  void println(const char *s) { if (!quiet) printf("%s\n", s); }
  void println(int v) { if (!quiet) printf("%d\n", v); }
  void println(unsigned v, int base = DEC) { if (!quiet) printf(base == HEX ? "%X\n" : "%u\n", v); }
  void println(double v) { if (!quiet) printf("%.2f\n", v); }
};
extern SimSerial Serial;
