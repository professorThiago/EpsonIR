/**
 * @file SendCommand.ino
 * @brief EpsonIR example — send a single command.
 *
 * Sends the POWER command to the projector every 5 seconds.
 *
 * Wiring:
 *   IR emitter OUT  →  GPIO 4
 *   IR emitter VCC  →  3.3 V or 5 V (check your module)
 *   IR emitter GND  →  GND
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
#include <EpsonIR.h>

#define IR_TX_PIN 4

EpsonIR projector(IR_TX_PIN);

void setup() {
  Serial.begin(115200);
  projector.begin();
  Serial.println("EpsonIR — SendCommand example");
  Serial.println("Sending POWER every 5 seconds...");
}

void loop() {
  Serial.println("→ POWER");
  projector.send(EPSON_CMD_POWER);
  delay(5000);
}
