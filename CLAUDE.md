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

## Language

Project documentation (README) is written in Russian.
