# TKB — Teensy Keyboard Bridge for IBM Wheelwriter 6

Interfacing with an IBM Wheelwriter 6 via its **Option Interface** port using a Teensy 4.0.

## Goal

Turn the IBM Wheelwriter 6 into a computer-controlled printer/terminal — send text to print from a computer, and eventually read keystrokes from the typewriter.

## Hardware

| Component | Description |
|---|---|
| IBM Wheelwriter 6 | Electronic daisy wheel typewriter |
| Teensy 4.0 | Microcontroller (ARM Cortex-M7, 600 MHz), S/N 19175250 |
| Cable/adapter | Connection to the Option Interface port on the back of the typewriter |

## Option Interface

The IBM Wheelwriter 6 has an **Option Interface** connector on the back panel — a proprietary serial interface (internal bus) used for communication between the keyboard and the print mechanism. The protocol uses:

- **Signal:** serial data, ~187.5 kbaud (inverted, open-collector)
- **Lines:** Bus (data), +5V, GND
- **Command format:** 3-byte packets (command + parameters)
- **Logic levels:** 5V (Teensy 4.0 is 3.3V — level shifter or resistor divider required)

See [docs/wheelwriter-bus-protocol.md](docs/wheelwriter-bus-protocol.md) for details.

## Project Structure

```
neonka/
├── firmware/             # Teensy firmware (Arduino/PlatformIO)
│   ├── src/main.cpp      # Main source file
│   └── platformio.ini    # PlatformIO configuration
├── scripts/              # Build and utility scripts
├── docs/                 # Wheelwriter protocol documentation
├── ref/                  # Reference projects (IBM_Wheelwriter_Teletype)
└── .vscode/              # IDE configuration (tasks, launch, settings)
```

## Development

Set up the environment:

```bash
./scripts/setup.sh
```

Main commands:

```bash
./scripts/build.sh             # Build firmware
./scripts/upload.sh            # Upload to Teensy (automatic reboot)
./scripts/serial.sh [sec]      # Serial reader with auto-reconnect (default 5 sec)
./scripts/compiledb.sh         # Generate compile_commands.json for IDE
```

Cursor/VSCode tasks:

```text
Tasks / Run and Debug:
- Build firmware
- Upload firmware
- Build + Upload firmware
- Monitor serial
- Generate compile_commands.json
```

The default build task in the IDE is set to `Build + Upload firmware`.

If the IDE shows no autocomplete or reports `Arduino.h file not found`, regenerate the compilation database:

```bash
./scripts/compiledb.sh
```

## Status

Early development. Current firmware monitors pins 6-9 (INPUT_PULLUP) and reports state changes over serial.

## Links

- [PJRC Teensy 4.0](https://www.pjrc.com/store/teensy40.html)
- [Wheelwriter bus protocol (Beihl)](http://www.interlockroc.org/2012/07/02/wheelwriter-speak/)
- [IBM Wheelwriter hack (Hacker's Delight)](https://github.com/acediac/WheelWriter)
