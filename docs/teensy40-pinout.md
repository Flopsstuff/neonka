---
layout: default
title: Teensy 4.0 Pinout
nav_order: 3
---

# Teensy 4.0 Pinout Reference

Source: [PJRC Teensy 4.0](https://www.pjrc.com/store/teensy40.html), [pinout card PDF](https://www.pjrc.com/teensy/card11a_rev4_web.pdf)

## Overview

- MCU: NXP i.MX RT1062, ARM Cortex-M7, 600 MHz
- Logic level: **3.3V** (pins are **NOT** 5V tolerant)
- Max pin output current: 4 mA recommended
- Total I/O: 40 pins (24 breadboard + 16 bottom SMT pads)
- Built-in LED: pin 13 (orange)

## Breadboard Pins (top, left side: 0-13, right side: 14-23)

| Pin | Digital | Analog | PWM | Primary Alt | Other Alt |
|-----|---------|--------|-----|-------------|-----------|
| 0 | Yes | - | Yes | Serial1 RX | CAN2 RX, SPI1 CS |
| 1 | Yes | - | Yes | Serial1 TX | CAN2 TX, SPI1 MISO |
| 2 | Yes | - | Yes | - | - |
| 3 | Yes | - | Yes | - | - |
| 4 | Yes | - | Yes | - | - |
| 5 | Yes | - | Yes | - | - |
| 6 | Yes | - | Yes | - | - |
| 7 | Yes | - | Yes | Serial2 RX | SPI alt MOSI |
| 8 | Yes | - | Yes | Serial2 TX | SPI alt MISO |
| 9 | Yes | - | Yes | - | - |
| 10 | Yes | - | Yes | SPI CS0 | - |
| 11 | Yes | - | Yes | SPI MOSI | - |
| 12 | Yes | - | Yes | SPI MISO | - |
| 13 | Yes | - | Yes | SPI SCK | LED |
| 14 | Yes | A0 | Yes | Serial3 TX | SPI alt SCK |
| 15 | Yes | A1 | Yes | Serial3 RX | - |
| 16 | Yes | A2 | - | Serial4 RX | SCL1 |
| 17 | Yes | A3 | - | Serial4 TX | SDA1 |
| 18 | Yes | A4 | - | SDA0 (I2C) | - |
| 19 | Yes | A5 | - | SCL0 (I2C) | - |
| 20 | Yes | A6 | Yes | Serial5 TX | - |
| 21 | Yes | A7 | Yes | Serial5 RX | - |
| 22 | Yes | A8 | Yes | - | - |
| 23 | Yes | A9 | Yes | - | - |

## Bottom SMT Pads

| Pin | Digital | Analog | PWM | Primary Alt | Other Alt |
|-----|---------|--------|-----|-------------|-----------|
| 24 | Yes | - | Yes | Serial6 TX | SCL2 |
| 25 | Yes | - | Yes | Serial6 RX | SDA2 |
| 26 | Yes | A12 | - | SPI1 MOSI | - |
| 27 | Yes | A13 | - | SPI1 SCK | - |
| 28 | Yes | - | Yes | Serial7 RX | - |
| 29 | Yes | - | Yes | Serial7 TX | - |
| 30 | Yes | - | - | CAN3 RX | - |
| 31 | Yes | - | - | CAN3 TX | - |
| 32 | Yes | - | - | - | - |
| 33 | Yes | - | Yes | - | SPI1 alt |
| 34 | Yes | - | Yes | - | - |
| 35 | Yes | - | Yes | - | - |
| 36 | Yes | - | Yes | - | - |
| 37 | Yes | - | Yes | - | - |
| 38 | Yes | A14 | - | Serial5 RX alt | - |
| 39 | Yes | A15 | - | Serial5 TX alt | - |

## Serial (UART) Ports

All 7 ports support arbitrary baud rates and hardware FIFO (4 bytes TX + RX).

| Port | TX Pin | RX Pin | Alt TX | Alt RX | Notes |
|------|--------|--------|--------|--------|-------|
| Serial1 | 1 | 0 | - | - | |
| Serial2 | 8 | 7 | - | - | |
| Serial3 | 14 | 15 | - | - | |
| Serial4 | 17 | 16 | - | - | |
| Serial5 | 20 | 21 | 39 | 38 | Alt pins on bottom pads |
| Serial6 | 24 | 25 | - | - | Bottom pads only |
| Serial7 | 29 | 28 | - | - | Bottom pads only |

All ports support:
- Transmitter Enable pin (any digital pin) for RS-485
- CTS via XBAR flexible pin routing
- 9-bit mode (`SERIAL_9N1`)

## SPI Ports

| Port | SCK | MOSI | MISO | CS | Alt SCK | Alt MOSI | Alt MISO |
|------|-----|------|------|----|---------|----------|----------|
| SPI | 13 | 11 | 12 | 10 | 14 | 7 | 8 |
| SPI1 | 27 | 26 | 1 | 0 | - | - | - |
| SPI2 | 45* | 43* | 32 | - | - | - | - |

*SPI2 pins 43, 45 only accessible on bottom pads of Teensy 4.1

## I2C Ports

| Port | SDA | SCL | Alt SDA | Alt SCL |
|------|-----|-----|---------|---------|
| Wire | 18 | 19 | - | - |
| Wire1 | 17 | 16 | - | - |
| Wire2 | 25 | 24 | - | - |

## CAN Bus

| Port | TX | RX |
|------|----|----|
| CAN2 | 1 | 0 |
| CAN3 | 31 | 30 |

Note: CAN requires external transceiver.

## Power Pins

| Pin | Description |
|-----|-------------|
| Vin | Input voltage (3.6V-5.5V) or output when USB powered (5V) |
| 3.3V | Regulated 3.3V output (250 mA) |
| GND | Ground (multiple pads) |
| VBAT | Battery backup for RTC (1.8V-3.6V coin cell) |
| On/Off | Control power state |
| Program | Press to enter bootloader |

## Notes

- **3.3V logic only** — pins are NOT 5V tolerant. Level shifter required for 5V devices.
- Max recommended output current per pin: 4 mA.
