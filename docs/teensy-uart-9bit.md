---
layout: default
title: Teensy 9-bit UART
nav_order: 4
---

# Teensy 9-bit UART for Wheelwriter Bus

Notes from [PJRC forum thread](https://forum.pjrc.com/index.php?threads/9-bit-uart-with-non-standard-baud-on-a-teensy-3-5.55790/) and reference projects.

## Basic Configuration

```cpp
Serial2.begin(187500, SERIAL_9N1);  // 9 data bits, no parity, 1 stop bit
```

- Paul Stoffregen confirmed: all Teensy serial ports support arbitrary baud rates
- `187500` works directly, no special setup needed
- Different ports can run at different baud rates simultaneously

## 9-bit Support

On Teensy 3.x, must uncomment in `HardwareSerial.h`:
```cpp
#define SERIAL_9BIT_SUPPORT
```

On Teensy 4.0 (LPUART / i.MX RT1062) -- needs verification whether this define is required or enabled by default.

With 9-bit mode enabled:
- `Serial2.write(0x121)` sends 9-bit word (bit8=1, byte=0x21) -- address byte
- `Serial2.write(0x003)` sends 9-bit word (bit8=0, byte=0x03) -- data byte
- Read returns `uint16_t` with 9th bit in bit 8

## Half-Duplex (Shared Bus)

The Wheelwriter bus is half-duplex single-wire. Two approaches:

### Approach 1: Loopback Mode (Teensy 3.x)

```cpp
// Enable loopback + single-wire mode
UART0_C1 |= UART_C1_LOOPS | UART_C1_RSRC;

// TX pin: open-collector with pull-up
pinMode(txPin, OUTPUT_OPENDRAIN);

// Switch to transmit
UART0_C3 |= UART_C3_TXDIR;

// Switch to receive
UART0_C3 &= ~UART_C3_TXDIR;
```

### Approach 2: Separate TX/RX with MOSFET (wheelwriter-interface)

- TX pin through N-channel MOSFET to bus (inverts signal)
- RX pin reads bus directly (no inversion needed)
- TX must be software-inverted (MCU HIGH -> MOSFET ON -> bus LOW)
- Sender sees own transmission on RX (must ignore/expect echo)

### Approach 3: Teensy 4.0 LPUART Single-Wire

Teensy 4.0 LPUART supports single-wire half-duplex natively via LPUART CTRL register:
- `CTRL[LOOPS]` = 1, `CTRL[RSRC]` = 1 for single-wire mode
- `CTRL[TXDIR]` to switch direction
- Registers: `LPUART4_CTRL` (for Serial2), etc.

**TODO:** Verify exact register addresses and test on Teensy 4.0.

## TX Signal Inversion

The bus is idle-HIGH, open-collector. If driving through MOSFET:
- Hardware inverts: MCU HIGH -> bus LOW
- Need inverted TX output

Teensy 4.0 LPUART has built-in inversion:
```cpp
Serial2.begin(187500, SERIAL_9N1_TXINV);  // invert TX only
// or
Serial2.begin(187500, SERIAL_9N1_RXINV_TXINV);  // invert both
```

Whether to use `TXINV`, `RXINV`, or neither depends on the hardware interface circuit.

## Relevant Teensy 4.0 LPUART Registers

| Register | Purpose |
|----------|---------|
| `LPUART4_CTRL` | Control (LOOPS, RSRC, TXDIR, M=9-bit, etc.) |
| `LPUART4_STAT` | Status |
| `LPUART4_DATA` | Data (bits 0-8 for 9-bit mode, bit 8 = 9th bit) |
| `LPUART4_BAUD` | Baud rate |

Serial1 = LPUART6, **Serial2 = LPUART4** (used for Wheelwriter), etc.
