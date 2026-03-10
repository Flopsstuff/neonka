---
name: monitor
description: "Read serial output from the Teensy 4.0 microcontroller. Use this skill when the user wants to see what the Teensy is printing — phrases like 'show serial output', 'read serial', 'what's the Teensy saying', 'monitor output', 'check serial', or any request to see data coming from the microcontroller over USB serial."
user_invocable: true
---

# Monitor Serial Output

Read serial output from the connected Teensy 4.0.

## Usage

`/monitor [seconds]` — duration defaults to 5 seconds if not specified.

## Steps

1. Run `./scripts/read-serial.sh <seconds>` with the user-provided duration, or `5` by default.
2. Show the captured output to the user.
3. If the script fails with a "port not found" error, let the user know the Teensy may not be connected or the serial port may be busy (e.g., another monitor session is open).
