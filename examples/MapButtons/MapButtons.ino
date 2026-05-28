/**
 * @file MapButtons.ino
 * @brief EpsonIR example — receive IR signals and decode Epson commands.
 *
 * Point the Epson remote at the IR receiver and press any button.
 * The command name and code are printed on the Serial monitor.
 * Unknown buttons print the raw frame value so you can add them to your code.
 *
 * Wiring (IR receiver, e.g. VS1838B / TSOP4838):
 *   Receiver OUT  →  GPIO 15
 *   Receiver VCC  →  3.3 V
 *   Receiver GND  →  GND
 *
 * Dependencies (platformio.ini):
 *   lib_deps =
 *     https://github.com/professorThiago/EpsonIR
 *     crankyoldgit/IRremoteESP8266 @ ^2.9.0
 *
 * @author  professorThiago (https://github.com/professorThiago)
 * @license MIT
 */

#include <Arduino.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <IRremoteESP8266.h>
#include <EpsonIR.h>

#define IR_RX_PIN   15
#define CAPTURE_BUF 150

// Command name lookup table (code → name string)
struct Entry { uint8_t code; const char* name; };
static const Entry CMD_NAMES[] = {
  { EPSON_CMD_POWER,         "POWER"         },
  { EPSON_CMD_FREEZE,        "FREEZE"        },
  { EPSON_CMD_MUTE,          "MUTE"          },
  { EPSON_CMD_ESC,           "ESC"           },
  { EPSON_CMD_ENTER,         "ENTER"         },
  { EPSON_CMD_UP,            "UP"            },
  { EPSON_CMD_DOWN,          "DOWN"          },
  { EPSON_CMD_RIGHT,         "RIGHT"         },
  { EPSON_CMD_LEFT,          "LEFT"          },
  { EPSON_CMD_HOME,          "HOME"          },
  { EPSON_CMD_MENU,          "MENU"          },
  { EPSON_CMD_VOL_UP,        "VOL+"          },
  { EPSON_CMD_VOL_DOWN,      "VOL-"          },
  { EPSON_CMD_ZOOM_IN,       "ZOOM+"         },
  { EPSON_CMD_ZOOM_OUT,      "ZOOM-"         },
  { EPSON_CMD_HDMI,          "HDMI"          },
  { EPSON_CMD_COMPUTER,      "COMPUTER"      },
  { EPSON_CMD_USB,           "USB"           },
  { EPSON_CMD_LAN,           "LAN"           },
  { EPSON_CMD_SOURCE_SEARCH, "SOURCE SEARCH" },
  { EPSON_CMD_COLOR_MODE,    "COLOR MODE"    },
  { EPSON_CMD_ASPECT,        "ASPECT"        },
  { EPSON_CMD_SPLIT,         "SPLIT"         },
  { EPSON_CMD_0,             "0"             },
  { EPSON_CMD_1,             "1"             },
  { EPSON_CMD_2,             "2"             },
  { EPSON_CMD_3,             "3"             },
  { EPSON_CMD_4,             "4"             },
  { EPSON_CMD_5,             "5"             },
  { EPSON_CMD_6,             "6"             },
  { EPSON_CMD_7,             "7"             },
  { EPSON_CMD_8,             "8"             },
  { EPSON_CMD_9,             "9"             },
  { EPSON_CMD_ID,            "ID"            },
  { EPSON_CMD_USER,          "USER"          },
  { EPSON_CMD_DEFAULT,       "DEFAULT"       },
};
static const uint8_t CMD_COUNT = sizeof(CMD_NAMES) / sizeof(CMD_NAMES[0]);

const char* codeName(uint8_t code) {
  for (uint8_t i = 0; i < CMD_COUNT; i++)
    if (CMD_NAMES[i].code == code) return CMD_NAMES[i].name;
  return nullptr;
}

IRrecv receiver(IR_RX_PIN, CAPTURE_BUF, 15, true);
decode_results results;
bool waitingFrame2 = false;

void setup() {
  Serial.begin(115200);
  receiver.enableIRIn();

  Serial.println();
  Serial.println("EpsonIR — MapButtons example");
  Serial.println("Press any button on the Epson remote...\n");
}

void loop() {
  if (!receiver.decode(&results)) return;

  if (results.decode_type == NEC && results.value != 0xFFFFFFFF) {
    uint32_t val = (uint32_t)results.value;

    if (val == EPSON_IR_WAKEUP) {
      waitingFrame2 = true;

    } else if (waitingFrame2) {
      waitingFrame2 = false;

      if (EpsonIR::isValidFrame(val)) {
        uint8_t code = val & 0xFF;   // B0 = command code (LSB)
        // B1 is the complement; B0 holds the raw command in the captured value
        // Re-derive: code is B1 in our protocol = (val >> 8) & 0xFF
        code = (val >> 8) & 0xFF;
        const char* name = codeName(code);

        Serial.println("────────────────────────────");
        if (name) {
          Serial.printf("  Button : %s\n", name);
        } else {
          Serial.printf("  Button : ❓ Unknown\n");
          Serial.printf("  Add to your code:\n");
          Serial.printf("  projector.send(0x%02X);\n", code);
        }
        Serial.printf("  Code   : 0x%02X\n", code);
        Serial.printf("  Frame2 : 0x%08X\n\n", val);
      }
    }
  }

  receiver.resume();
}
