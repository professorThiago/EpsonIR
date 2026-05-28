# EpsonIR

Arduino/ESP32 library for sending IR commands to **Epson projectors** using the reverse-engineered NEC Extended protocol.

> Developed and tested with an ESP32-S3 and a real Epson remote.  
> Author: [professorThiago](https://github.com/professorThiago)

---

## Protocol

The Epson remote sends **two NEC frames** per button press:

| Frame | Value | Purpose |
|-------|-------|---------|
| Frame 1 (wake-up) | `0x81C00FF0` | Identical for every button |
| Frame 2 (command) | `0xC1AA<~cmd><cmd>` | Identifies the button |

Frame 2 byte layout (LSB first as reported by IRremoteESP8266):

```
B3    B2    B1       B0
0xC1  0xAA  <~cmd>   <cmd>
fixed fixed checksum  button code
```

`B0 + B1 == 0xFF` always (standard NEC integrity check on the command bytes).

---

## Installation

### PlatformIO (recommended)

Add to your `platformio.ini`:

```ini
lib_deps =
    https://github.com/professorThiago/EpsonIR
    crankyoldgit/IRremoteESP8266 @ ^2.9.0
```

### Arduino IDE

1. Download this repository as a ZIP file.  
2. **Sketch → Include Library → Add .ZIP Library…**  
3. Also install **IRremoteESP8266** via Library Manager.

---

## Wiring

### Sending (IR LED / emitter module)

```
ESP32 GPIO 4  →  IR emitter TX/IN
3.3 V or 5 V  →  IR emitter VCC
GND           →  IR emitter GND
```

> A bare IR LED needs a ~100 Ω series resistor and an NPN transistor (e.g. 2N2222) as a driver. Ready-made IR emitter modules already include these components.

### Receiving (for the MapButtons example)

```
ESP32 GPIO 15  →  IR receiver OUT  (VS1838B / TSOP4838 / TSOP31238)
3.3 V          →  IR receiver VCC
GND            →  IR receiver GND
```

---

## Quick start

```cpp
#include <EpsonIR.h>

EpsonIR projector(4);   // IR LED on GPIO 4

void setup() {
    projector.begin();
}

void loop() {
    projector.send(EPSON_CMD_POWER);   // toggle power
    delay(5000);
}
```

---

## API

### Constructor

```cpp
EpsonIR projector(uint8_t txPin);
```

### Methods

#### `void begin()`
Initialises the IR sender. Call once in `setup()`.

#### `void send(uint8_t commandCode)`
Sends a complete two-frame Epson IR command.

```cpp
projector.send(EPSON_CMD_FREEZE);
projector.send(EPSON_CMD_VOL_UP);
projector.send(EPSON_CMD_HDMI);
```

#### `bool sendRawFrame(uint32_t frame2)`
Sends a command using the full 32-bit frame 2 value (e.g. captured with the MapButtons example). Returns `false` if the frame fails protocol validation.

```cpp
projector.sendRawFrame(0xC1AA09F6UL);   // Power
```

#### `static uint32_t buildFrame(uint8_t commandCode)`
Returns the 32-bit frame 2 value for a command code without transmitting.

```cpp
uint32_t frame = EpsonIR::buildFrame(EPSON_CMD_POWER);
// frame == 0xC1AA09F6
```

#### `static bool isValidFrame(uint32_t frame2)`
Returns `true` if the value passes Epson protocol validation (B2==0xAA, B3==0xC1, B0+B1==0xFF).

---

## Command reference

| Constant | Code | Button |
|----------|------|--------|
| `EPSON_CMD_POWER` | `0x09` | Power on/off |
| `EPSON_CMD_FREEZE` | `0x49` | Freeze image |
| `EPSON_CMD_MUTE` | `0xC9` | Mute audio |
| `EPSON_CMD_ESC` | `0x21` | Escape / back |
| `EPSON_CMD_ENTER` | `0xA1` | Enter / confirm |
| `EPSON_CMD_UP` | `0x0D` | Navigate up |
| `EPSON_CMD_DOWN` | `0x4D` | Navigate down |
| `EPSON_CMD_RIGHT` | `0x8D` | Navigate right |
| `EPSON_CMD_LEFT` | `0xCD` | Navigate left |
| `EPSON_CMD_HOME` | `0xA9` | Home |
| `EPSON_CMD_MENU` | `0x59` | Menu |
| `EPSON_CMD_VOL_UP` | `0x19` | Volume up |
| `EPSON_CMD_VOL_DOWN` | `0x99` | Volume down |
| `EPSON_CMD_ZOOM_IN` | `0x11` | Zoom in |
| `EPSON_CMD_ZOOM_OUT` | `0x91` | Zoom out |
| `EPSON_CMD_HDMI` | `0xCE` | HDMI input |
| `EPSON_CMD_COMPUTER` | `0x29` | Computer input |
| `EPSON_CMD_USB` | `0x6E` | USB input |
| `EPSON_CMD_LAN` | `0x2E` | LAN input |
| `EPSON_CMD_SOURCE_SEARCH` | `0x31` | Auto source search |
| `EPSON_CMD_COLOR_MODE` | `0xF1` | Color mode |
| `EPSON_CMD_ASPECT` | `0x51` | Aspect ratio |
| `EPSON_CMD_SPLIT` | `0x4B` | Split screen |
| `EPSON_CMD_0` … `EPSON_CMD_9` | `0x79` … | Numeric keys |
| `EPSON_CMD_ID` | `0x8C` | Projector ID |
| `EPSON_CMD_USER` | `0xF9` | User button |
| `EPSON_CMD_DEFAULT` | `0x9C` | Restore defaults |

---

## Examples

| Example | Description |
|---------|-------------|
| `SendCommand` | Sends the POWER command every 5 seconds |
| `MapButtons` | Receives IR signals and prints the decoded button name |

---

## License

MIT © 2026 [professorThiago](https://github.com/professorThiago)
