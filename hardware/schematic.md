# TKB Wiring Schematic

## Level Shifter (4-channel bidirectional, 3.3V <-> 5V)

```
Teensy 4.0          Level Shifter (4ch)       Wheelwriter
                     (3.3V <-> 5V)            Option Interface
Pin 6  (GPIO) ----> CH1_LV   CH1_HV ---->     (spare)
Pin 7  (RX2)  <---- CH2_LV   CH2_HV <----     BUS (pin 5)
Pin 8  (TX2)  ----> CH3_LV   CH3_HV ---->     BUS (pin 5)
Pin 9  (GPIO) ----> CH4_LV   CH4_HV ---->     (spare)
3.3V ------------>  LV
GND  ------------>  GND       GND  <--------  GND (pin 3,4)
                               HV  <--------  +5V (pin 1,2)
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

## Serial Configuration

- **Port:** Serial2 (LPUART4)
- **TX:** Pin 8, **RX:** Pin 7
- **Baud:** 187500, 9-bit, no parity, 1 stop (`SERIAL_9N1`)

```cpp
Serial2.begin(187500, SERIAL_9N1);
```
