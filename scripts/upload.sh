#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"

# Send REBOOT command to enter bootloader (no button press needed)
PORT=$(ls /dev/cu.usbmodem* 2>/dev/null | head -1 || true)
if [ -n "$PORT" ]; then
    echo "Sending REBOOT to $PORT..."
    stty -f "$PORT" 115200 cs8 -cstopb -parenb
    echo "REBOOT" > "$PORT"
    sleep 1
fi

"$ROOT/.venv/bin/pio" run -d "$ROOT/firmware" -t upload
