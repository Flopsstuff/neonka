# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**TKB (Teensy Keyboard Bridge)** — a project to interface with an IBM Wheelwriter 6 typewriter via its Option Interface port using a Teensy 4.0 microcontroller. The goal is to turn the typewriter into a computer-controlled printer/terminal (send text to print, and eventually read keystrokes).

## Hardware Context

- **Typewriter:** IBM Wheelwriter 6 (daisy wheel, proprietary serial bus)
- **Microcontroller:** Teensy 4.0 (ARM Cortex-M7, 600 MHz, 3.3V logic)
- **Protocol:** ~187.5 kbaud inverted open-collector serial, 3-byte command packets
- **Level shifting required:** Wheelwriter bus is 5V, Teensy is 3.3V

## Planned Structure

- `firmware/` — Teensy firmware (Arduino/PlatformIO)
- `host/` — Host-side utilities (computer to Teensy communication)
- `docs/` — Wheelwriter protocol documentation
- `hardware/` — Wiring diagrams, pinouts

## Build & Development

PlatformIO in a project-local venv (`.venv/`). No activation needed — scripts call `.venv/bin/pio` directly.

```bash
./scripts/setup.sh    # Create venv, install PlatformIO (one-time)
./scripts/build.sh    # Compile firmware
./scripts/upload.sh   # Upload to Teensy (must be connected)
./scripts/monitor.sh  # Serial monitor at 115200 baud
```

First build downloads ~200MB ARM toolchain — this is normal.

First upload requires pressing the Teensy button physically; subsequent uploads auto-reboot via USB. Serial port appears as `/dev/cu.usbmodemXXXXXX` on macOS.

## Language

Project documentation (README) is written in Russian.
