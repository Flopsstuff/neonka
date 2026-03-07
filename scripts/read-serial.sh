#!/usr/bin/env bash
set -euo pipefail

# Non-interactive serial reader for Claude Code (no PTY needed).
# Usage: ./scripts/read-serial.sh [seconds] [port]
#   seconds  — how long to capture (default: 5)
#   port     — serial device (default: auto-detect Teensy USB)

DURATION="${1:-5}"
PORT="${2:-}"
BAUD=115200

if [ -z "$PORT" ]; then
    PORT=$(ls /dev/cu.usbmodem* 2>/dev/null | head -1 || true)
    if [ -z "$PORT" ]; then
        echo "ERROR: No Teensy found. Is it connected?" >&2
        exit 1
    fi
fi

echo "Reading $PORT at ${BAUD} baud for ${DURATION}s..."
stty -f "$PORT" "$BAUD" cs8 -cstopb -parenb
timeout "$DURATION" cat "$PORT" || true
