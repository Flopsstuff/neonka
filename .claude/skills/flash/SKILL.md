---
name: flash
description: "Build, upload, and verify Teensy 4.0 firmware. Use this skill whenever the user wants to flash, upload, deploy, or update firmware on the Teensy — including phrases like 'upload this', 'flash it', 'deploy firmware', 'build and upload', 'try it on the Teensy', or any variation of getting new code onto the microcontroller."
user_invocable: true
---

# Flash Firmware

Build the firmware, upload it to the Teensy 4.0, and verify it's running.

## Steps

1. **Build** — run `./scripts/build.sh`
   - If the build fails, show the compiler errors and stop. Don't attempt upload.

2. **Upload** — run `./scripts/upload.sh`
   - The script sends a `REBOOT` command over serial to enter the bootloader automatically.
   - If upload fails with a timeout or "unable to open" error, suggest the user press the physical button on the Teensy and then re-run upload.

3. **Verify** — run `./scripts/serial.sh 5` to capture 5 seconds of serial output
   - Show the output to confirm the firmware is running.
   - If there's no output, note that the Teensy may not be printing anything — this isn't necessarily an error.

Report the result of each step clearly.
