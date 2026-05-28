/**
 * @file EpsonIR.cpp
 * @brief Implementation of the EpsonIR library.
 *
 * @author  professorThiago (https://github.com/professorThiago)
 * @version 1.0.0
 * @license MIT
 */

#include "EpsonIR.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

EpsonIR::EpsonIR(uint8_t txPin)
  : _irsend(txPin), _txPin(txPin) {}

// ---------------------------------------------------------------------------
// Public methods
// ---------------------------------------------------------------------------

void EpsonIR::begin() {
  _irsend.begin();
}

void EpsonIR::send(uint8_t commandCode) {
  uint32_t frame2 = buildFrame(commandCode);
  _sendNEC(EPSON_IR_WAKEUP);
  delay(EPSON_IR_FRAME_GAP);
  _sendNEC(frame2);
}

bool EpsonIR::sendRawFrame(uint32_t frame2) {
  if (!isValidFrame(frame2)) return false;
  _sendNEC(EPSON_IR_WAKEUP);
  delay(EPSON_IR_FRAME_GAP);
  _sendNEC(frame2);
  return true;
}

uint32_t EpsonIR::buildFrame(uint8_t commandCode) {
  uint8_t inv = ~commandCode;
  // Layout: B3=0xC1  B2=0xAA  B1=~cmd  B0=cmd
  // (IRremoteESP8266 sends LSB-first, so B0 is the least-significant byte)
  return EPSON_IR_FRAME2_HIGH | ((uint32_t)inv << 8) | commandCode;
}

bool EpsonIR::isValidFrame(uint32_t frame2) {
  uint8_t b0 =  frame2        & 0xFF;
  uint8_t b1 = (frame2 >>  8) & 0xFF;
  uint8_t b2 = (frame2 >> 16) & 0xFF;
  uint8_t b3 = (frame2 >> 24) & 0xFF;
  return (b2 == 0xAA) && (b3 == 0xC1) && ((uint8_t)(b0 + b1) == 0xFF);
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

void EpsonIR::_sendNEC(uint32_t value) {
  _irsend.sendNEC(value, 32);
}
