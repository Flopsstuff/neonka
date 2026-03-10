#!/usr/bin/env bash
set -euo pipefail

# Serial reader with auto-reconnect.
# Usage: ./scripts/serial.sh [seconds] [port]
#   seconds  — how long to capture (default: 5, 0 = unlimited)
#   port     — serial device (default: auto-detect Teensy USB)

DURATION="${1:-5}"
PORT="${2:-}"
BAUD=115200

if [ "$DURATION" -eq 0 ]; then
    END_TIME=0
else
    END_TIME=$(( $(date +%s) + DURATION ))
fi

time_left() {
    if [ "$END_TIME" -eq 0 ]; then return 0; fi
    [ "$(date +%s)" -lt "$END_TIME" ]
}

find_port() {
    if [ -n "$PORT" ]; then
        echo "$PORT"
    else
        ls /dev/cu.usbmodem* 2>/dev/null | head -1 || true
    fi
}

wait_for_port() {
    while time_left; do
        local p
        p=$(find_port)
        if [ -n "$p" ]; then
            echo "$p"
            return 0
        fi
        sleep 0.2
    done
    return 1
}

CURRENT_PORT=$(find_port)
if [ -z "$CURRENT_PORT" ]; then
    echo "Waiting for Teensy..." >&2
    CURRENT_PORT=$(wait_for_port) || { echo "ERROR: No Teensy found." >&2; exit 1; }
fi

echo "Reading $CURRENT_PORT at ${BAUD} baud${DURATION:+ for ${DURATION}s}..."

while time_left; do
    if [ ! -e "$CURRENT_PORT" ]; then
        echo "Disconnected, reconnecting..." >&2
        CURRENT_PORT=$(wait_for_port) || break
        echo "Reconnected to $CURRENT_PORT" >&2
    fi
    stty -f "$CURRENT_PORT" "$BAUD" cs8 -cstopb -parenb 2>/dev/null || { sleep 0.2; continue; }
    printf '\n' > "$CURRENT_PORT" 2>/dev/null || true
    if [ "$END_TIME" -eq 0 ]; then
        cat "$CURRENT_PORT" 2>/dev/null || true
    else
        REMAINING=$(( END_TIME - $(date +%s) ))
        [ "$REMAINING" -le 0 ] && break
        timeout "$REMAINING" cat "$CURRENT_PORT" 2>/dev/null || true
    fi
done
