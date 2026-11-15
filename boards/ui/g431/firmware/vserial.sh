#!/bin/bash
set -e

# Open the Nucleo-G431KB's Virtual COM Port in a terminal (screen).
# The serial device is discovered automatically via /dev/serial/by-id.
#
# Usage: ./vserial.sh [baud]   (default: 115200)

BAUD="${1:-115200}"

# The ST-Link VCP enumerates as a by-id symlink ending in -if02.
DEV=$(ls /dev/serial/by-id/*STLINK*-if02 2>/dev/null | head -n1)

if [ -z "$DEV" ]; then
  echo "error: ST-Link VCP not found. Is the board connected via USB?" >&2
  exit 1
fi

echo "Opening $DEV @ $BAUD ..."
echo "Exit screen with: Ctrl+A then :quit"
sudo screen "$DEV" "$BAUD"
