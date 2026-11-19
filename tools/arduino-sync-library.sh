#!/bin/sh
#
# Copy the vscp-firmware sources the Arduino library uses from src/ into
# arduino/VscpFirmware/src/ (an Arduino library must hold all its files).
#
#   tools/arduino-sync-library.sh          copy
#   tools/arduino-sync-library.sh --check  only check that the copies are
#                                          the same as src/ (exit 1 if not)
#   tools/arduino-sync-library.sh --zip    copy, then make VscpFirmware.zip
#                                          (Arduino IDE: Sketch > Include
#                                          Library > Add .ZIP Library...)
#
# Run it after changing one of the files below in src/.

set -e

ROOT=$(cd "$(dirname "$0")/.." && pwd)
SRC="$ROOT/src"
DST="$ROOT/arduino/VscpFirmware/src"

FILES="vscp-firmware-level2.c vscp-firmware-level2.h vscp-firmware-helper.c vscp-firmware-helper.h
vscp.h vscp-class.h vscp-type.h vscp-aes.h"

status=0
for f in $FILES; do
  if [ "$1" = "--check" ]; then
    if ! cmp -s "$SRC/$f" "$DST/$f"; then
      echo "arduino/VscpFirmware/src/$f differs from src/$f (run tools/arduino-sync-library.sh)"
      status=1
    fi
  else
    cp "$SRC/$f" "$DST/$f"
    echo "copied $f"
  fi
done
if [ "$1" = "--zip" ] && [ $status -eq 0 ]; then
  (cd "$ROOT/arduino" && rm -f VscpFirmware.zip && zip -qr VscpFirmware.zip VscpFirmware)
  echo "created arduino/VscpFirmware.zip"
fi
exit $status
