# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**TKB (Teensy Keyboard Bridge)** — a project to interface with an IBM Wheelwriter 6 typewriter via its Option Interface port using a Teensy 4.0 microcontroller. The goal is to turn the typewriter into a computer-controlled printer/terminal (send text to print, and eventually read keystrokes).

## Hardware Context

- **Typewriter:** IBM Wheelwriter 6 (daisy wheel, proprietary serial bus)
- **Microcontroller:** Teensy 4.0 (ARM Cortex-M7, 600 MHz, 3.3V logic)
- **Protocol:** ~187.5 kbaud inverted open-collector serial, 3-byte command packets
- **Level shifter:** 4-channel bidirectional on pins 6-9; Serial2 (TX=8, RX=7) for Wheelwriter bus
- **Level shifting required:** Wheelwriter bus is 5V, Teensy is 3.3V

## Project Structure

- `firmware/` — Teensy firmware (Arduino/PlatformIO)
- `firmware/src/main.cpp` — main sketch
- `firmware/platformio.ini` — PlatformIO config (Teensy 4.0, 115200 baud)
- `scripts/` — automation scripts (setup, build, upload, monitor)
- `host/` — Host-side utilities (computer to Teensy communication)
- `docs/` — Wheelwriter protocol documentation
- `hardware/` — Wiring diagrams, pinouts
- `.claude/skills/` — Claude Code skills

## Build & Development

PlatformIO in a project-local venv (`.venv/`). No activation needed — scripts call `.venv/bin/pio` directly.

```bash
./scripts/setup.sh       # Create venv, install PlatformIO (one-time)
./scripts/build.sh       # Compile firmware
./scripts/upload.sh      # Upload to Teensy (sends REBOOT, no button needed)
./scripts/serial.sh [seconds] [port]  # Serial reader with auto-reconnect (default 5s)
```

First build downloads ~200MB ARM toolchain — this is normal.

Firmware listens for `REBOOT` command on serial to enter bootloader programmatically. `upload.sh` sends it automatically — no button press needed after initial flash. Serial port appears as `/dev/cu.usbmodemXXXXXX` on macOS.

## Claude Code Skills

- `/flash` — build, upload, and verify firmware on Teensy
- `/monitor [seconds]` — read serial output (default 5s, non-interactive)

## Language

Project documentation (README) is written in English.
