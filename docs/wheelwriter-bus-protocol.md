# IBM Wheelwriter Bus Protocol

Reverse-engineered from two independent projects:
- [IBM_Wheelwriter_Teletype](https://github.com/jim11662418/IBM_Wheelwriter_Teletype) by Jim Loos (v1.3.5, 2019-2025) -- STC 8051 MCU, intercepts ribbon cable
- [wheelwriter-interface](https://github.com/jkua/wheelwriter-interface) by John Kua (2023) -- Arduino Nano RP2040, connects via Option Interface

Cross-validated: both sources confirm the same protocol. Applicable to Wheelwriter models 3, 5, and 6 (two-board architecture: Logic Board + Motor Control Board).

**Terminology note:** Jim Loos calls them "Function Board" and "Printer Board". John Kua calls them "Logic Board" and "Motor Control Board". These are the same boards. This document uses both names interchangeably: Logic/Function Board (keyboard controller) and Motor Control/Printer Board (mechanical controller).

## 1. Physical Layer

| Parameter | Value |
|---|---|
| Baud rate | 187,500 bps (12 MHz / 64) |
| Protocol | Intel 8051 UART Mode 2 (9-bit multiprocessor) |
| Frame | 1 start + 9 data + 1 stop = 11 bits per word |
| Bus type | Half-duplex, single-wire, shared bus |
| Electrical | Open-collector, idle = HIGH, data pulls LOW |
| Voltage | 5V logic levels |
| MCU (Logic Board) | Intel P8051AH @ 11.9 MHz |
| MCU (Motor Control Board) | Intel P8051AH @ 12 MHz |

### Connectors

**Internal ribbon cable** (Logic Board J8F to Motor Control Board J5P, 7-pin):

| Pin | Signal | Description |
|-----|--------|-------------|
| 1 | VRM | Voltage regulator module, 5.1V |
| 2 | PDI | Unknown |
| 3 | BRQ | Bus Request |
| 4 | BUS | Serial communication bus (187500 bps, 9-N-1, 5V, idle high) |
| 5 | GND | Ground |
| 6 | POR | Power-On Reset |
| 7 | +5V | 5V DC |

**Option Interface connector** (Motor Control Board J1P, 10-pin Molex SL):

| Pin | Signal | Description |
|-----|--------|-------------|
| 1 | +5V | 5V DC |
| 2 | +5V | 5V DC |
| 3 | GND | Ground |
| 4 | GND | Ground |
| 5 | BUS | Serial communication bus (same as internal bus) |
| 6 | NC | Not connected |
| 7 | BRQ | Bus Request |
| 8 | POR | Power-On Reset |
| 9 | VRM | Voltage regulator module |
| 10 | PDI | Unknown |

**Mating connector:** Molex 50-57-9210 (70066-0079). Contacts for 22-24 AWG: Molex 16-02-0102 (70058-0222).

For basic operation, only **+5V (1,2), GND (3,4), and BUS (5)** are needed.

### Bus Arbitration

- Before transmitting each word, the sender waits for the bus to go HIGH
- After transmitting, the sender waits for the bus to return to HIGH
- Receiver sends ACK by pulling the bus LOW briefly after receiving a word
- The bus is shared: only one device transmits at a time
- On a shared bus, the sender sees its own transmission (must be ignored or expected)
- Alternative to monitoring ACK: insert ~450 us delay between words (less reliable)

### Signal Inversion for TX

The bus is idle-HIGH with open-collector drivers. When using an N-channel MOSFET or similar open-collector/open-drain transistor to drive the bus, the TX signal is **inverted** at the hardware level:
- MCU output HIGH -> MOSFET ON -> bus pulled LOW
- MCU output LOW -> MOSFET OFF -> bus idle HIGH

Therefore the MCU TX output must be **inverted** in software or hardware. The RX input does NOT need inversion (bus LOW = start bit = standard UART).

The wheelwriter-interface project confirms this: it uses an inverted TX PIO program (idle=LOW, start bit=HIGH, data bits bitwise inverted) paired with a non-inverted RX PIO program.

### UART Configuration

**8051 at 12 MHz (reference):**
```
Baud rate = SYSclk / (65536 - reload_value) / 4
187,500 = 12,000,000 / (65536 - 65520) / 4
reload_value = 65520 (0xFFF0)
```

## 2. Word Format

Each word on the bus is a 9-bit value (0x000-0x1FF), transmitted as an 11-bit UART frame:

```
[START] [D0] [D1] [D2] [D3] [D4] [D5] [D6] [D7] [D8] [STOP]
         \________ 8 lower bits ________/    \9th bit/
```

The 9th bit (D8) is the **address/data indicator** (8051 Mode 2):
- **Bit 8 = 1**: Address byte (marks the start of a command)
- **Bit 8 = 0**: Data byte or ACK

The only address used is `0x21` (Motor Control Board), transmitted as `0x121` (0x21 with 9th bit set).

## 3. Protocol Overview

### Command Structure

All commands begin with the **address word** `0x121`, followed by a **command code**, then zero or more **data words**:

```
0x121  <cmd>  [<data1>]  [<data2>]
```

Each word is acknowledged individually by the Motor Control Board.

### Per-word Handshake

Full bus transaction for each word:

```
Sender:   [word]  -->
                      <--  [ACK/response]  :Receiver
```

The sender transmits a word, then the receiver responds with either:
- **ACK** (`0x000`) for address bytes and intermediate data bytes
- **Response data** for the last byte of query commands (command-specific)

### Command Lengths

| Command | Length | Meaning |
|---------|--------|---------|
| `0x00` | 1 (cmd only) | Response is the answer |
| `0x01` | 1 | Response is the answer |
| `0x02` | 3 (cmd + 2 data) | ACK after last data |
| `0x03` | 3 | ACK after last data |
| `0x04` | 3 | ACK after last data |
| `0x05` | 2 (cmd + 1 data) | ACK after data |
| `0x06` | 3 | ACK after last data |
| `0x07` | 1 | ACK is the response |
| `0x08` | 1 | Response is the answer |
| `0x09` | 2 | ACK after data |
| `0x0A` | 2 | ACK after data |
| `0x0B` | 1 | Response is status |
| `0x0C` | 3 | Response after last data |
| `0x0D` | 2 | Response after data |
| `0x0E` | 2 | ACK after data |

### Typing Command Sequence (as seen on the bus)

The Logic Board wraps each keystroke in a status/event sequence:

1. Query status: `0x0B` / response `0x00` (ready)
2. Keypress start: `0x0C <keycode> 0x46` / response
3. Type command: `0x03 <wheel_pos> <usteps>` / ACK
4. (optional) Query: `0x0C <keycode> 0x08` / response
5. Wait for motion: `0x0C <keycode> 0x44` / response (can be slow)
6. Keypress end: `0x0C <keycode> 0x06` / response

**For external devices (our use case):** Only the core commands (0x03, 0x05, 0x06, etc.) are needed. The status queries (0x0B, 0x0C) are optional but recommended for reliability.

## 4. Command Reference

### 4.0 Query Model: `0x121, 0x000`

Queries the typewriter model. No data bytes.

**Response values:**

| Response | Model |
|----------|-------|
| `0x006` | Wheelwriter 3 |
| `0x025` | Wheelwriter 5 |
| `0x026` | Wheelwriter 6 |

### 4.1 Reset: `0x121, 0x001`

Power-on reset command. Moves carriage to left stop and returns printwheel pitch.

**Response (printwheel type):**

| Response | Printwheel | Pitch | uSpaces/char | uLines/line |
|----------|-----------|-------|--------------|-------------|
| `0x008` | PS (Proportional Spacing) | variable | 10 | 16 |
| `0x010` | 15P (Micro Elite) | 15 cpi | 8 | 12 |
| `0x020` | 12P (Elite) | 12 cpi | 10 | 16 |
| `0x040` | 10P (Pica) | 10 cpi | 12 | 16 |
| `0x021` | No printwheel installed | - | 10 | 16 |

After reset, the Logic Board sends additional initialization:
- `0x0A 0x00` (unknown, ACK'd)
- `0x0C 0x00 0x01` (query, response `0x20`)
- `0x06 0x80 0x78` (move carriage right 120 usteps = 1 inch)
- Further init traffic for ~1 second

### 4.2 Type Character Without Advance: `0x121, 0x002, <wheel_pos>, <ignored>`

Prints a character **without moving the carriage**. Used for in-place overprinting (bold, underline composites).

| Word | Description |
|------|-------------|
| `0x121` | Address |
| `0x002` | Type-no-advance command |
| `<wheel_pos>` | Printwheel position (1-96, 0x01-0x60) |
| `<ignored>` | Second data byte, ignored by controller |

### 4.3 Type Character and Advance: `0x121, 0x003, <wheel_pos>, <usteps>`

Prints a character and advances the carriage rightward.

| Word | Description |
|------|-------------|
| `0x121` | Address |
| `0x003` | Type-and-advance command |
| `<wheel_pos>` | Printwheel position (1-96, 0x01-0x60) |
| `<usteps>` | Microspaces to advance (max 63) |

**Microspaces per character by pitch:**
- 10 cpi (Pica): 12 microspaces/character
- 12 cpi (Elite): 10 microspaces/character
- 15 cpi (Micro Elite): 8 microspaces/character
- PS (Proportional): 10 microspaces/character (default)

**Note:** 1 microspace = 1/120 inch. Maximum advance per command = 63 usteps.

**Maximum line width:** ~1450 microspaces (~12 inches, ~80 columns at 12cpi).

### 4.4 Erase Character and Advance: `0x121, 0x004, <wheel_pos>, <usteps>`

Prints using the correction (lift-off) tape instead of ink ribbon, then advances.

| Word | Description |
|------|-------------|
| `0x121` | Address |
| `0x004` | Erase command |
| `<wheel_pos>` | Character to erase (must match printed character) |
| `<usteps>` | Microspaces to advance rightward |

### 4.5 Vertical Movement (Platen): `0x121, 0x005, <direction_and_usteps>`

Moves the paper vertically by rotating the platen.

| Word | Description |
|------|-------------|
| `0x121` | Address |
| `0x005` | Platen movement command |
| `<direction_and_usteps>` | Direction + distance |

**Bit fields:**

```
Bit 7:    Direction -- 1 = paper UP (toward printhead), 0 = paper DOWN
Bits 6:0: Number of microlines to move (7-bit value, max 127)
```

**Microlines per line by pitch:**
- 10 cpi, 12 cpi, PS: 16 microlines = 1 full line
- 15 cpi: 12 microlines = 1 full line

**Common vertical movements:**

| Movement | 10/12cpi, PS | 15 cpi | Value (10/12cpi) |
|----------|-------------|--------|-------------------|
| Line feed (up 1 line) | 16 | 12 | `0x90` |
| Reverse line feed (down 1 line) | 16 | 12 | `0x10` |
| Half line up | 8 | 6 | `0x88` |
| Half line down | 8 | 6 | `0x08` |
| Micro up (1/8 line = 1/48") | 2 | 1-2 | `0x82` |
| Micro down (1/8 line = 1/48") | 2 | 1-2 | `0x02` |

### 4.6 Horizontal Movement (Carriage): `0x121, 0x006, <dir_hi>, <usteps_lo>`

Moves the carrier horizontally.

| Word | Description |
|------|-------------|
| `0x121` | Address |
| `0x006` | Carriage movement command |
| `<dir_hi>` | Direction + upper bits of distance |
| `<usteps_lo>` | Lower 8 bits of distance |

**dir_hi bit fields:**

```
Bit 7:    Direction -- 1 = RIGHT (toward right margin), 0 = LEFT (toward left margin)
Bits 6:0: Upper 7 bits of the microspace count
```

The total distance is an **11-bit value** (max 2047) split across two words:
```
total_microspaces = ((dir_hi & 0x07) << 8) | usteps_lo
```

Note: in practice the upper bits beyond bit 2 are rarely used (max ~1450 usteps for a full line = 11 bits).

**Examples:**

| Operation | dir_hi | usteps_lo |
|-----------|--------|-----------|
| Space right (12cpi) | `0x80` | `0x0A` (10) |
| Space right (10cpi) | `0x80` | `0x0C` (12) |
| Backspace (12cpi) | `0x00` | `0x0A` (10) |
| Tab right 5 chars (12cpi) | `0x80` | `0x32` (50) |
| Carriage return from col 80 (12cpi) | `(n>>8)&0x07` | `n&0xFF` where n=800 |

### 4.7 Spin Printwheel: `0x121, 0x007`

Spins the daisy wheel as a visual/audible indication. No data bytes. Used as a "bell" or mode-change feedback.

### 4.8 Query Printwheel: `0x121, 0x008`

Queries the printwheel pitch without resetting. Returns the same values as the reset command (Section 4.1).

### 4.9 Set Impression Control: `0x121, 0x009, <level>`

Sets the hammer strike power. On Wheelwriter 3: `0x00` or `0x01`. Later models may have more levels. Also triggered by Code+Q on the keyboard.

Additional modes observed in the enum:
- `0x00` -- Repeat off
- `0x02` -- Shift right half space
- `0x04` -- Underline
- `0x08` -- Repeat character
- `0x80` -- Center

### 4.10 Unknown Init Command: `0x121, 0x00A, 0x00`

Emitted during power-up sequence after the reset command. Purpose unknown.

### 4.11 Query Status: `0x121, 0x00B`

Queries the Motor Control Board status. The Logic Board sends this before every typing/motion command.

**Response values:**

| Response | Meaning |
|----------|---------|
| `0x00` | Ready |
| `0x04` | Carriage moving (or motion complete) |
| `0x07` | Carriage move complete |
| `0x10` | Platen moving |
| `0x14` | Carriage + platen moving |
| `0x40` | Printwheel changed or left limit switch pressed |

### 4.12 Keypress Event Query: `0x121, 0x00C, <keycode>, <sub_cmd>`

Complex query used by the Logic Board to wrap keystroke events. Sends keycode and sub-command, receives status response.

**Sub-commands:**

| Sub-cmd | Purpose | Typical response |
|---------|---------|-----------------|
| `0x46` | Start keypress event | `0x04`, `0x10` |
| `0x08` | Backspace/space event | `0x10`, `0xF0` at left margin |
| `0x44` | Wait for motion complete | `0x40`, `0x80` (can be slow) |
| `0x06` | End keypress event | `0x04`, `0x10`, `0x39` |
| `0x01` | Wait for platen/carriage move | variable |

### 4.13 Code Key Combination: `0x121, 0x00E, <key_code>`

Sent by the Logic Board when a Code+key combination is pressed.

**Note:** On Wheelwriter 3, bit 7 of `<key_code>` is cleared. On Wheelwriter 6, bit 7 is set. The Shift key sets bit 7 as well. Use `key_code & 0x7F` for model-independent decoding.

**Code key release:** `0x067` (or `0x0E7` with shift/WW6) indicates the Code key was released.

**Code+Key mapping (key_code & 0x7F):**

| Code | Key | Code | Key |
|------|-----|------|-----|
| `0x00` | Code++/- | `0x001` | Code+1 |
| `0x002` | Code+Q | `0x004` | Code+A |
| `0x006` | Code+Z | `0x009` | Code+2 |
| `0x00A` | Code+W | `0x00C` | Code+S |
| `0x00E` | Code+X | `0x011` | Code+3 |
| `0x012` | Code+E | `0x014` | Code+D |
| `0x016` | Code+C | `0x018` | Code+5 |
| `0x019` | Code+4 | `0x01A` | Code+R |
| `0x01B` | Code+T | `0x01C` | Code+F |
| `0x01D` | Code+G | `0x01E` | Code+V |
| `0x01F` | Code+B | `0x020` | Code+6 |
| `0x021` | Code+7 | `0x022` | Code+U |
| `0x023` | Code+Y | `0x024` | Code+J |
| `0x025` | Code+H | `0x026` | Code+M |
| `0x028` | Code+= | `0x029` | Code+8 |
| `0x02A` | Code+I | `0x02B` | Code+] |
| `0x02C` | Code+K | `0x02E` | Code+, |
| `0x031` | Code+9 | `0x032` | Code+O |
| `0x034` | Code+L | `0x036` | Code+. |
| `0x038` | Code+- | `0x039` | Code+0 |
| `0x03A` | Code+P | `0x03B` | Code+1/2 |
| `0x03C` | Code+; | `0x03D` | Code+' |
| `0x03F` | Code+/ | `0x042` | Code+L Mar |
| `0x045` | Code+T Clr | `0x046` | Code+Micro Dn |
| `0x047` | Code+Space | `0x048` | Code+Mar Rel |
| `0x049` | Paper Feed Lever | `0x04A` | Code+Tab |
| `0x04B` | Code+R Mar | `0x04C` | Code+T Set |
| `0x04F` | Code+Erase | `0x050` | Code+Backspace |
| `0x051` | Code+Paper Up | `0x052` | Code+Paper Dn |
| `0x054` | Code+Micro Up | `0x056` | Code+C Rtn |
| `0x057` | Code+Line Space | `0x067` | Code key released |
| `0x072` | Code+N (Caps) | `0x076` | Code+N |

## 5. Initialization Sequence

Power-up sequence as observed on the bus:

1. Hardware reset (POR line pulsed)
2. `0x121, 0x001` -- Reset command, response = printwheel type
3. `0x121, 0x00A, 0x00` -- Unknown init, ACK
4. `0x121, 0x00C, 0x00, 0x01` -- Init query, response `0x20`
5. `0x121, 0x006, 0x80, 0x78` -- Move carriage right 120 usteps (1 inch)
6. Further initialization traffic for ~1 second
7. Bus becomes idle; ready for normal operation

**Timing:** Printwheel detection may take up to 6 seconds. Total initialization including post-reset traffic ~7 seconds.

## 6. Printwheel Translation Tables

The daisy wheel has 96 character positions (0x01-0x60) arranged in a circle, plus position 0x00 = space (no print). Printwheel positions do NOT follow ASCII order.

### Physical Arrangement of the Printwheel

Starting at the 12 o'clock position, going counter-clockwise:
```
Position 01: 'a'    Position 02: 'n'    Position 03: 'r'    Position 04: 'm'
Position 05: 'c'    Position 06: 's'    Position 07: 'd'    Position 08: 'h'
Position 09: 'l'    Position 10: 'f'    Position 11: 'k'    Position 12: ','
...
```

### Printwheel Variants

Multiple printwheel types exist with different character mappings at positions 0x25-0x48:

| Keyboard ID | Type | Notes |
|-------------|------|-------|
| 001 (US) | Standard US | `<` `>` mapped to non-ASCII symbols |
| 103 (ASCII) | Full ASCII | All standard ASCII characters available |
| 026, 029 | German | umlauts, eszett |
| 067, 253 | UK | pound sign |
| 070 | Spanish | n-tilde, inverted punctuation |
| 231 | USSR | Cyrillic characters |

Positions 0x01-0x24 and 0x49-0x60 are **identical across all printwheel variants** (common letters, digits, punctuation).

### ASCII to Printwheel Code (for printing)

Table for ASCII printwheel (keyboard 103). Index = ASCII code - 0x20.
Space (0x20) maps to code 0x00.

```
       0x_0 0x_1 0x_2 0x_3 0x_4 0x_5 0x_6 0x_7 0x_8 0x_9 0x_A 0x_B 0x_C 0x_D 0x_E 0x_F
0x2_:   00   49   4B   38   37   39   3F   4C   23   16   36   3B   0C   0E   57   28
        sp    !    "    #    $    %    &    '    (    )    *    +    ,    -    .    /
0x3_:   30   2E   2F   2C   32   31   33   35   34   2A   4E   50   45   4D   46   4A
         0    1    2    3    4    5    6    7    8    9    :    ;    <    =    >    ?
0x4_:   3D   20   12   1B   1D   1E   11   0F   14   1F   21   2B   18   24   1A   22
         @    A    B    C    D    E    F    G    H    I    J    K    L    M    N    O
0x5_:   15   3E   17   19   1C   10   0D   29   2D   26   13   41   42   40   3A   4F
         P    Q    R    S    T    U    V    W    X    Y    Z    [    \    ]    ^    _
0x6_:   3C   01   59   05   07   60   0A   5A   08   5D   56   0B   09   04   02   5F
         `    a    b    c    d    e    f    g    h    i    j    k    l    m    n    o
0x7_:   5C   52   03   06   5E   5B   53   55   51   58   54   48   43   47   44   00
         p    q    r    s    t    u    v    w    x    y    z    {    |    }    ~   DEL
```

**US printwheel (001) differences from ASCII (103):**
- `<` (0x3C): US = 0x00 (unavailable), ASCII = 0x45
- `>` (0x3E): US = 0x00, ASCII = 0x46
- `\` (0x5C): US = 0x00, ASCII = 0x42
- `^` (0x5E): US = 0xA2 (cent sign), ASCII = 0x3A
- `` ` `` (0x60): US = 0x00, ASCII = 0x3C
- `{` (0x7B): US = 0x00, ASCII = 0x48
- `|` (0x7C): US = 0x00, ASCII = 0x43
- `}` (0x7D): US = 0x00, ASCII = 0x47
- `~` (0x7E): US = 0x00, ASCII = 0x44

### Printwheel Code to ASCII (for reading keystrokes)

Index = printwheel_code (code 0x00 = space 0x20). Table for US printwheel:

```
Idx  Char | Idx  Char | Idx  Char | Idx  Char | Idx  Char | Idx  Char
0x00  SP  | 0x01  a   | 0x02  n   | 0x03  r   | 0x04  m   | 0x05  c
0x06  s   | 0x07  d   | 0x08  h   | 0x09  l   | 0x0A  f   | 0x0B  k
0x0C  ,   | 0x0D  V   | 0x0E  -   | 0x0F  G   | 0x10  U   | 0x11  F
0x12  B   | 0x13  Z   | 0x14  H   | 0x15  P   | 0x16  )   | 0x17  R
0x18  L   | 0x19  S   | 0x1A  N   | 0x1B  C   | 0x1C  T   | 0x1D  D
0x1E  E   | 0x1F  I   | 0x20  A   | 0x21  J   | 0x22  O   | 0x23  (
0x24  M   | 0x25  .   | 0x26  Y   | 0x27  ,   | 0x28  /   | 0x29  W
0x2A  9   | 0x2B  K   | 0x2C  3   | 0x2D  X   | 0x2E  1   | 0x2F  2
0x30  0   | 0x31  5   | 0x32  4   | 0x33  6   | 0x34  8   | 0x35  7
0x36  *   | 0x37  $   | 0x38  #   | 0x39  %   | 0x3A  cent| 0x3B  +
0x3C  +-  | 0x3D  @   | 0x3E  Q   | 0x3F  &   | 0x40  ]   | 0x41  [
0x42  sup3| 0x43  sup2| 0x44  deg | 0x45  sect| 0x46  para| 0x47  1/2
0x48  1/4 | 0x49  !   | 0x4A  ?   | 0x4B  "   | 0x4C  '   | 0x4D  =
0x4E  :   | 0x4F  _   | 0x50  ;   | 0x51  x   | 0x52  q   | 0x53  v
0x54  z   | 0x55  w   | 0x56  j   | 0x57  .   | 0x58  y   | 0x59  b
0x5A  g   | 0x5B  u   | 0x5C  p   | 0x5D  i   | 0x5E  t   | 0x5F  o
0x60  e   |
```

## 7. Composite Operations

Some typewriter actions require multiple bus commands in sequence.

### 7.1 Bold Printing

Print character in place, shift 1 ustep, re-print in place, then advance:
```
0x121, 0x002, <pw_code>, 0x00           -- print without advance
0x121, 0x006, 0x80, 0x01                -- move right 1 ustep
0x121, 0x002, <pw_code>, 0x00           -- re-print (offset by 1 ustep)
0x121, 0x006, 0x80, <uSpaces - 1>       -- advance remaining
```

Alternative (from Jim Loos, using cmd 0x03):
```
0x121, 0x003, <pw_code>, 0x001          -- print + advance 1 ustep
0x121, 0x003, <pw_code>, (uSpaces - 1)  -- re-print + advance remaining
```

### 7.2 Underline Printing

Print character, then overprint underscore:
```
0x121, 0x003, <pw_code>, 0x000          -- print + don't advance
0x121, 0x003, 0x04F, <uSpacesPerChar>   -- print '_' + advance
```

### 7.3 Erase Character

Move carrier left, then print character on correction tape:
```
0x121, 0x006, 0x00, <uSpacesPerChar>                 -- move left 1 char width
0x121, 0x004, <pw_code>, <uSpacesPerChar>             -- erase + advance right
```
Net result: character erased, carrier back at original position.

### 7.4 Carriage Return

Move carrier left by the total accumulated horizontal microspaces:
```
0x121, 0x006, (count >> 8) & 0x07, count & 0xFF      -- move left by count
```
Bit 7 of third word = 0 (left direction).

## 8. Keystroke Decoding State Machine

When monitoring the bus, the Logic Board sends multi-word command sequences for each keystroke. A state machine decodes them:

```
State IDLE:
  Receive 0x121 --> GOT_ADDR

State GOT_ADDR:
  0x003 --> CHAR_1 (character sequence)
  0x005 --> VERT (vertical movement)
  0x006 --> HORIZ_1 (horizontal movement)
  0x00E --> CODE_KEY (code key combo)
  other --> IDLE

State CHAR_1:
  Save printwheel code --> CHAR_2

State CHAR_2:
  Microspaces received, decode: ASCII = printwheel2ASCII[saved_code]
  --> IDLE

State VERT:
  Receive dir|usteps byte
  If usteps == uLinesPerLine AND dir == UP --> Carriage Return key detected
  --> IDLE

State HORIZ_1:
  If bit 7 set --> HORIZ_RIGHT
  Else --> HORIZ_LEFT

State HORIZ_RIGHT:
  usteps == uSpacesPerChar --> Space key
  usteps > uSpacesPerChar AND < uSpacesPerChar*10 --> Tab key
  --> IDLE

State HORIZ_LEFT:
  usteps == uSpacesPerChar --> Backspace key
  --> IDLE

State CODE_KEY:
  Decode key_code & 0x7F (see Section 4.13)
  --> IDLE
```

## 9. Application to TKB Project (Teensy 4.0)

### Connection via Option Interface

Both reference projects confirm the Option Interface port uses the same bus protocol as the internal ribbon cable. Our Teensy connects to the 10-pin Option connector.

**Minimum wiring:** +5V (pins 1-2), GND (pins 3-4), BUS (pin 5).

### Teensy UART Configuration

```cpp
Serial1.begin(187500, SERIAL_9N1);  // 9 data bits, no parity, 1 stop bit
```

The 9th bit distinguishes address words (bit8=1, e.g., `0x121`) from data/ACK words (bit8=0).

### Hardware Requirements

1. **Level shifter:** 3.3V (Teensy) <-> 5V (Wheelwriter bus), open-drain compatible
2. **TX inversion:** Required because the open-collector driver inverts the signal. Options:
   - Hardware: N-channel MOSFET naturally inverts (MCU HIGH -> bus LOW)
   - Software: Teensy 4.0 UART supports `SERIAL_9N1_RXINV_TXINV` flag
   - If using MOSFET for bus drive, MCU TX must be inverted so that MCU HIGH -> MOSFET ON -> bus LOW = correct data
3. **RX does NOT need inversion** when reading bus directly (bus LOW = UART start bit)

### Bus Timing

- At 187,500 bps with 11-bit frames: ~58.7 us per word
- Expect ACK within ~450 us after each transmitted word
- On shared bus, sender sees its own transmission (ignore the echo)
- Between commands, no strict timing (bus idles HIGH)

### Recommended Approach

Based on both reference projects:
1. Send `0x121, 0x00B` (query status) before each command to ensure Motor Control Board is ready
2. After sending each word, read back the echo (own transmission) and the ACK/response
3. Check ACK = 0x00 for non-query commands; check response for queries
4. Track horizontal position (microspaces) for carriage return calculations

## 10. References

- [jim11662418/IBM_Wheelwriter_Teletype](https://github.com/jim11662418/IBM_Wheelwriter_Teletype) -- MIT License, Jim Loos 2019-2025
- [jkua/wheelwriter-interface](https://github.com/jkua/wheelwriter-interface) -- John Kua 2023, Arduino/RP2040
- [RussellSenior/IBM-Wheelwriter-Hack Wiki](https://github.com/RussellSenior/IBM-Wheelwriter-Hack/wiki/Bus-Protocol) -- Bus protocol wiki
- [tofergregg/IBM-Wheelwriter-Hack](https://github.com/tofergregg/IBM-Wheelwriter-Hack) -- Original reverse engineering
- Applies to: IBM Wheelwriter 3, 5, 6 (two-board / Series I models)
- ASCII printwheel part number: 1353909
- Option connector: Molex SL 10-pin (mating: 50-57-9210)
