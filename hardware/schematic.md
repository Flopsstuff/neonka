# TKB Wiring Schematic

## Level Shifter

KAmod Level Shift x4 (BSS138-based, bidirectional, non-inverting).

4 channels connected to Teensy pins 6-9:

```
Teensy 4.0          KAmod Level Shift x4      Wheelwriter
                     (3.3V <-> 5V)
Pin 6  (GPIO) ----> CH1_LV   CH1_HV ---->     (spare)
Pin 7  (RX2)  <---- CH2_LV   CH2_HV <----     \
Pin 8  (TX2)  ----> CH3_LV   CH3_HV ---->      > BUS
Pin 9  (GPIO) ----> CH4_LV   CH4_HV ---->     (spare)
3.3V ------------>  LV
GND  ------------>  GND       GND  <--------  GND
                               HV  <--------  +5V
```

## Wheelwriter Option Interface Connector

10-pin Molex SL (mating connector: 50-57-9210):

```
Pin 1:  +5V
Pin 2:  +5V
Pin 3:  GND
Pin 4:  GND
Pin 5:  BUS (serial data, 5V open-collector)
Pin 6-10: unused for this project
```

## Connection Phases

### Phase 1: Extension Port

Connect to the Option Interface connector. Single-wire half-duplex — listen and send on the same BUS line.

- **Pin 8 (TX2)** through level shifter to BUS (pin 5)
- LPUART4 single-wire mode (one pin for both TX and RX)
- Pin 8 configured as `OUTPUT_OPENDRAIN`

```cpp
Serial2.begin(187500, SERIAL_9N1);
LPUART4_CTRL |= LPUART_CTRL_LOOPS | LPUART_CTRL_RSRC;
pinMode(8, OUTPUT_OPENDRAIN);
```

Direction control:
```cpp
LPUART4_CTRL |= LPUART_CTRL_TXDIR;   // transmit
LPUART4_CTRL &= ~LPUART_CTRL_TXDIR;  // receive
```

Open question: can keyboard input be read via the extension port? Needs testing.

### Phase 2: Inline (bus break)

Cut the bus between keyboard PCB and Motor Control Board. Teensy sits in the middle:

```
Keyboard PCB ----> CH2 ----> Pin 7 (RX2)   [listen to keyboard]
Pin 8 (TX2) ----> CH3 ----> Motor Control   [send to printer]
```

- Standard Serial2 TX/RX mode (no single-wire, no register tricks)
- Can intercept, modify, or inject commands
- Two level shifter channels, two spare

Firmware change: remove single-wire config, switch to normal duplex.

## Serial Configuration

- **Port:** Serial2 (LPUART4)
- **TX:** Pin 8, **RX:** Pin 7
- **Baud:** 187500, 9-bit, no parity, 1 stop (`SERIAL_9N1`)
- **Inversion:** not needed — the bus has standard UART polarity (idle HIGH = 1,
  start bit = LOW). BSS138 level shifter preserves polarity. The `wheelwriter-interface`
  reference project uses inverted TX only because it drives the bus through a 2N7002
  N-MOSFET which hardware-inverts the signal (MCU HIGH → MOSFET ON → bus LOW).
  We connect through a level shifter directly, so no inversion.
