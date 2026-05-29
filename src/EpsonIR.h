/**
 * @file EpsonIR.h
 * @brief Library for sending IR commands to Epson projectors.
 *
 * This library implements the Epson NEC Extended IR protocol, which uses
 * two NEC frames per command:
 *
 *   Frame 1 (wake-up): 0x81C00FF0  — identical for every button
 *   Frame 2 (command): 0xC1AA<~cmd><cmd>
 *     - Byte B3: 0xC1 (fixed)
 *     - Byte B2: 0xAA (fixed)
 *     - Byte B1: command code (identifies the button)
 *     - Byte B0: ~B1  (checksum — complement of B1)
 *
 * Hardware: connect an IR LED (with a driver transistor or dedicated IR
 * emitter module) to the TX pin defined when constructing EpsonIR.
 *
 * @author  professorThiago (https://github.com/professorThiago)
 * @version 1.0.1
 * @date    2026
 * @license MIT
 *
 * MIT License
 * -----------
 * Copyright (c) 2026 professorThiago
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <Arduino.h>
#include <IRsend.h>

// ---------------------------------------------------------------------------
// Protocol constants
// ---------------------------------------------------------------------------

/** @brief Frame 1: wake-up frame sent before every command. */
#define EPSON_IR_WAKEUP      0x81C00FF0UL

/** @brief Fixed high bytes present in every frame 2 (B3=0xC1, B2=0xAA). */
#define EPSON_IR_FRAME2_HIGH 0xC1AA0000UL

/** @brief Carrier frequency used by Epson remotes (Hz). */
#define EPSON_IR_FREQ_KHZ    38

/** @brief Delay between frame 1 and frame 2 (milliseconds). */
#define EPSON_IR_FRAME_GAP   10

// ---------------------------------------------------------------------------
// Command codes (byte B1 of frame 2)
// ---------------------------------------------------------------------------

/**
 * @defgroup EpsonCommands Epson IR command codes
 * Each constant is the B1 byte of the second NEC frame.
 * Frame 2 value = 0xC1AA | (~code << 8) | code
 * @{
 */

// Power / basic control
#define EPSON_CMD_POWER         0x09  ///< Toggle power on/off
#define EPSON_CMD_FREEZE        0x49  ///< Freeze / unfreeze image
#define EPSON_CMD_MUTE          0xC9  ///< Toggle audio mute
#define EPSON_CMD_ESC           0x21  ///< Escape / back

// Navigation
#define EPSON_CMD_ENTER         0xA1  ///< Confirm / enter
#define EPSON_CMD_UP            0x0D  ///< Navigate up
#define EPSON_CMD_DOWN          0x4D  ///< Navigate down
#define EPSON_CMD_RIGHT         0x8D  ///< Navigate right
#define EPSON_CMD_LEFT          0xCD  ///< Navigate left
#define EPSON_CMD_HOME          0xA9  ///< Home / top menu
#define EPSON_CMD_MENU          0x59  ///< Open / close menu

// Volume
#define EPSON_CMD_VOL_UP        0x19  ///< Volume up
#define EPSON_CMD_VOL_DOWN      0x99  ///< Volume down

// Zoom
#define EPSON_CMD_ZOOM_IN       0x11  ///< Zoom in
#define EPSON_CMD_ZOOM_OUT      0x91  ///< Zoom out

// Input sources
#define EPSON_CMD_HDMI          0xCE  ///< Select HDMI input
#define EPSON_CMD_COMPUTER      0x29  ///< Select Computer input
#define EPSON_CMD_USB           0x6E  ///< Select USB input
#define EPSON_CMD_LAN           0x2E  ///< Select LAN input
#define EPSON_CMD_SOURCE_SEARCH 0x31  ///< Auto source search

// Image settings
#define EPSON_CMD_COLOR_MODE    0xF1  ///< Cycle color modes
#define EPSON_CMD_ASPECT        0x51  ///< Cycle aspect ratios
#define EPSON_CMD_SPLIT         0x4B  ///< Split screen

// Numeric keypad
#define EPSON_CMD_0             0x79  ///< Numeric key 0
#define EPSON_CMD_1             0x84  ///< Numeric key 1
#define EPSON_CMD_2             0x4C  ///< Numeric key 2
#define EPSON_CMD_3             0x6C  ///< Numeric key 3
#define EPSON_CMD_4             0x02  ///< Numeric key 4
#define EPSON_CMD_5             0x42  ///< Numeric key 5
#define EPSON_CMD_6             0x62  ///< Numeric key 6
#define EPSON_CMD_7             0x12  ///< Numeric key 7
#define EPSON_CMD_8             0x52  ///< Numeric key 8
#define EPSON_CMD_9             0x32  ///< Numeric key 9

// Advanced
#define EPSON_CMD_ID            0x8C  ///< Projector ID configuration
#define EPSON_CMD_USER          0xF9  ///< User-defined button
#define EPSON_CMD_DEFAULT       0x9C  ///< Restore defaults

/** @} */

// ---------------------------------------------------------------------------
// EpsonIR class
// ---------------------------------------------------------------------------

/**
 * @brief Controls an Epson projector via infrared.
 *
 * @par Quick start
 * @code
 * #include <EpsonIR.h>
 *
 * EpsonIR projector(4);   // IR LED on GPIO 4
 *
 * void setup() {
 *   projector.begin();
 * }
 *
 * void loop() {
 *   projector.send(EPSON_CMD_POWER);
 *   delay(5000);
 * }
 * @endcode
 */
class EpsonIR {
public:
  /**
   * @brief Construct an EpsonIR instance.
   * @param txPin  GPIO pin connected to the IR LED (or emitter module).
   */
  explicit EpsonIR(uint8_t txPin);

  /**
   * @brief Initialise the underlying IR sender.
   *        Call once in setup().
   */
  void begin();

  /**
   * @brief Send a single command to the projector.
   *
   * Transmits the Epson two-frame sequence:
   *   1. Wake-up frame (0x81C00FF0)
   *   2. Command frame  (0xC1AA<~cmd><cmd>)
   *
   * @param commandCode  One of the EPSON_CMD_* constants, or a raw byte.
   *
   * @par Example
   * @code
   * projector.send(EPSON_CMD_FREEZE);
   * @endcode
   */
  void send(uint8_t commandCode);

  /**
   * @brief Send a command by its full 32-bit frame 2 value.
   *
   * Use this when you have captured a raw frame value and want to replay
   * it directly without looking up the command code.
   *
   * @param frame2  Full 32-bit value of the second NEC frame.
   *                Must satisfy: B2==0xAA, B3==0xC1, B0==~B1.
   * @return true   if the frame passes Epson protocol validation.
   * @return false  if the frame does not look like a valid Epson command.
   *
   * @par Example
   * @code
   * projector.sendRawFrame(0xC1AA09F6UL);  // Power
   * @endcode
   */
  bool sendRawFrame(uint32_t frame2);

  /**
   * @brief Build the 32-bit frame 2 value for a given command code.
   *
   * Useful when you need the raw NEC value for logging or retransmission.
   *
   * @param commandCode  One of the EPSON_CMD_* constants.
   * @return uint32_t    The full 32-bit frame 2 value.
   *
   * @par Example
   * @code
   * uint32_t frame = EpsonIR::buildFrame(EPSON_CMD_POWER);
   * Serial.printf("Power frame: 0x%08X\n", frame);  // 0xC1AA09F6
   * @endcode
   */
  static uint32_t buildFrame(uint8_t commandCode);

  /**
   * @brief Validate whether a 32-bit value is a legal Epson frame 2.
   *
   * Checks that B2==0xAA, B3==0xC1, and B0+B1==0xFF.
   *
   * @param frame2  Value to validate.
   * @return true   if valid.
   */
  static bool isValidFrame(uint32_t frame2);

private:
  IRsend  _irsend;
  uint8_t _txPin;

  void _sendNEC(uint32_t value);
};
