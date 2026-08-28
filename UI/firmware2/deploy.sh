#!/bin/bash
set -e

# Flash the current firmware build to the Nucleo-G431KB over its on-board ST-Link.
# The ST-Link device is discovered automatically via USB.
#
# Usage: ./deploy.sh [path-to.bin]   (default: build/firmware2.bin)

BIN="${1:-build/firmware2.bin}"

if [ ! -f "$BIN" ]; then
  echo "error: $BIN not found. Build first (make) or pass the .bin path." >&2
  exit 1
fi

echo "Looking for ST-Link on USB..."
if ! st-info --probe >/dev/null 2>&1; then
  echo "error: no ST-Link detected (or permission denied). Is the board connected?" >&2
  exit 1
fi

echo "Flashing $BIN @ 0x08000000 ..."
sudo st-flash write "$BIN" 0x08000000
echo "Done. Running the app..."
