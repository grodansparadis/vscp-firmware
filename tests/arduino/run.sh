#!/bin/sh
#
# Host test of the VscpFirmware Arduino library: builds the library, its copy
# of the vscp-firmware framework and the Blinky example with Arduino/mcp_can
# stand-ins (shim/) and runs scripted CAN4VSCP traffic against it.
#
# Also checks that the framework copies in the library are the same as src/.

set -e
cd "$(dirname "$0")"
ROOT=$(cd ../.. && pwd)
LIB="$ROOT/arduino/VscpFirmware/src"
OUT=${TMPDIR:-/tmp}/vscp-arduino-hosttest
mkdir -p "$OUT"

"$ROOT/tools/arduino-sync-library.sh" --check

# The Blinky example with setup()/loop() renamed so the test can call them
sed -e 's/^setup()$/blinky_setup()/' -e 's/^loop()$/blinky_loop()/' \
  "$ROOT/arduino/VscpFirmware/examples/Blinky/Blinky.ino" > "$OUT/blinky-example.cpp"

CC=${CC:-cc}
CXX=${CXX:-c++}
$CC -c -w -Ishim -I"$LIB" "$LIB/vscp-firmware-level2.c" -o "$OUT/level2.o"
$CC -c -w -Ishim -I"$LIB" "$LIB/vscp-firmware-helper.c" -o "$OUT/helper.o"
$CXX -std=c++11 -Wall -Wno-unused-function -Wno-unused-parameter -Ishim -I"$LIB" -I"$OUT" -x c++ host-test.cpp -x none \
  "$OUT/level2.o" "$OUT/helper.o" -lm -o "$OUT/hosttest"
"$OUT/hosttest"
