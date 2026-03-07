---
name: flash
description: Build and upload firmware to Teensy 4.0
user_invocable: true
---

Build and flash the Teensy 4.0 firmware:

1. Run `./scripts/build.sh` to compile the firmware
2. If build succeeds, run `./scripts/upload.sh` to upload (sends REBOOT command automatically, no button press needed)
3. Wait for the upload to complete
4. Run `./scripts/read-serial.sh 5` to verify the firmware is running and show serial output

Report each step's result. If upload fails, suggest pressing the Teensy button and retrying.
