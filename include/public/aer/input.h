/**
 * @file
 *
 * @brief Utilities for querying user input.
 *
 * @since 1.0.0
 *
 * @copyright 2021 the libaermre authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef AER_INPUT_H
#define AER_INPUT_H

#include <stdbool.h>
#include <stdint.h>

/* ----- PUBLIC TYPES ----- */

/**
 * @brief Indexes for keypresses in input lookup table.
 *
 * @since 1.1.0
 */
typedef enum AERInputKey {
    AER_KEY_INTERRUPT = 0x3,
    AER_KEY_BACKSPACE = 0x8,
    AER_KEY_TAB = 0x9,
    AER_KEY_ENTER = 0xd,
    AER_KEY_CAPSLOCK = 0x14,
    AER_KEY_ERASE = 0x15,
    AER_KEY_ESCAPE = 0x1b,
    AER_KEY_SPACE = 0x20,
    AER_KEY_PAGEUP = 0x21,
    AER_KEY_EXCLAIM = 0x21,
    AER_KEY_PAGEDOWN = 0x22,
    AER_KEY_QUOTE = 0x22,
    AER_KEY_END = 0x23,
    AER_KEY_HOME = 0x24,
    AER_KEY_DOLLAR = 0x24,
    AER_KEY_LARROW = 0x25,
    AER_KEY_PERCENT = 0x25,
    AER_KEY_UARROW = 0x26,
    AER_KEY_AMP = 0x26,
    AER_KEY_RARROW = 0x27,
    AER_KEY_DARROW = 0x28,
    AER_KEY_LPAREN = 0x28,
    AER_KEY_RPAREN = 0x29,
    AER_KEY_ASTERISK = 0x2a,
    AER_KEY_PLUS = 0x2b,
    AER_KEY_INSERT = 0x2d,
    AER_KEY_DELETE = 0x2e,
    AER_KEY_0 = 0x30,
    AER_KEY_1 = 0x31,
    AER_KEY_2 = 0x32,
    AER_KEY_3 = 0x33,
    AER_KEY_4 = 0x34,
    AER_KEY_5 = 0x35,
    AER_KEY_6 = 0x36,
    AER_KEY_7 = 0x37,
    AER_KEY_8 = 0x38,
    AER_KEY_9 = 0x39,
    AER_KEY_COLON = 0x3a,
    AER_KEY_LESS = 0x3c,
    AER_KEY_GREATER = 0x3e,
    AER_KEY_QUERY = 0x3f,
    AER_KEY_AT = 0x40,
    AER_KEY_A = 0x41,
    AER_KEY_B = 0x42,
    AER_KEY_C = 0x43,
    AER_KEY_D = 0x44,
    AER_KEY_E = 0x45,
    AER_KEY_F = 0x46,
    AER_KEY_G = 0x47,
    AER_KEY_H = 0x48,
    AER_KEY_I = 0x49,
    AER_KEY_J = 0x4a,
    AER_KEY_K = 0x4b,
    AER_KEY_L = 0x4c,
    AER_KEY_M = 0x4d,
    AER_KEY_N = 0x4e,
    AER_KEY_O = 0x4f,
    AER_KEY_P = 0x50,
    AER_KEY_Q = 0x51,
    AER_KEY_R = 0x52,
    AER_KEY_S = 0x53,
    AER_KEY_T = 0x54,
    AER_KEY_U = 0x55,
    AER_KEY_V = 0x56,
    AER_KEY_W = 0x57,
    AER_KEY_X = 0x58,
    AER_KEY_Y = 0x59,
    AER_KEY_Z = 0x5a,
    AER_KEY_CARET = 0x5e,
    AER_KEY_UNDERSCORE = 0x5f,
    AER_KEY_NUM0 = 0x60,
    AER_KEY_NUM1 = 0x61,
    AER_KEY_NUM2 = 0x62,
    AER_KEY_NUM3 = 0x63,
    AER_KEY_NUM4 = 0x64,
    AER_KEY_NUM5 = 0x65,
    AER_KEY_NUM6 = 0x66,
    AER_KEY_NUM7 = 0x67,
    AER_KEY_NUM8 = 0x68,
    AER_KEY_NUM9 = 0x69,
    AER_KEY_NUMASTERISK = 0x6a,
    AER_KEY_NUMPLUS = 0x6b,
    AER_KEY_NUMMINUS = 0x6d,
    AER_KEY_NUMPERIOD = 0x6e,
    AER_KEY_NUMSLASH = 0x6f,
    AER_KEY_F1 = 0x70,
    AER_KEY_F2 = 0x71,
    AER_KEY_F3 = 0x72,
    AER_KEY_F4 = 0x73,
    AER_KEY_F5 = 0x74,
    AER_KEY_F6 = 0x75,
    AER_KEY_F7 = 0x76,
    AER_KEY_F8 = 0x77,
    AER_KEY_F9 = 0x78,
    AER_KEY_F10 = 0x79,
    AER_KEY_LCURLY = 0x7b,
    AER_KEY_VBAR = 0x7c,
    AER_KEY_RCURLY = 0x7d,
    AER_KEY_TILDE = 0x7e,
    AER_KEY_F11 = 0x80,
    AER_KEY_F12 = 0x81,
    AER_KEY_LSHIFT = 0xa0,
    AER_KEY_RSHIFT = 0xa1,
    AER_KEY_LCTRL = 0xa2,
    AER_KEY_RCTRL = 0xa3,
    AER_KEY_LALT = 0xa4,
    AER_KEY_RALT = 0xa5,
    AER_KEY_SEMICOLON = 0xba,
    AER_KEY_EQUALS = 0xbb,
    AER_KEY_COMMA = 0xbc,
    AER_KEY_MINUS = 0xbd,
    AER_KEY_PERIOD = 0xbe,
    AER_KEY_SLASH = 0xbf,
    AER_KEY_APOS = 0xc0,
    AER_KEY_LSQUARE = 0xdb,
    AER_KEY_BACKSLASH = 0xdc,
    AER_KEY_RSQUARE = 0xdd,
    AER_KEY_HASH = 0xde,
    AER_KEY_BACKTICK = 0xdf
} AERInputKey;

/* ----- PUBLIC CONSTANTS ----- */

/**
 * @brief Printable characters corresponding to input table indexes.
 *
 * @since 1.1.0
 */
extern const char AER_DISPLAY_KEYS[];

/* ----- PUBLIC FUNCTIONS ----- */

/**
 * @brief Query the keyboard key(s) that the user just pressed this step.
 *
 * @return Lookup table mapping key-codes to pressed state or `NULL` if
 * unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 *
 * @sa AERInputKey
 */
const bool* AERInputGetKeysPressed(void);

/**
 * @brief Query the keyboard key(s) that the user has held this step.
 *
 * @return Lookup table mapping key-codes to held state or `NULL` if
 * unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 *
 * @sa AERInputKey
 */
const bool* AERInputGetKeysHeld(void);

/**
 * @brief Query the keyboard key(s) that the user just released this step.
 *
 * @return Lookup table mapping key-codes to released state or `NULL` if
 * unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 *
 * @sa AERInputKey
 */
const bool* AERInputGetKeysReleased(void);

/**
 * @brief Query the mouse button(s) that the user just pressed this step.
 *
 * @subsubsection MouseButtonStates Mouse Button States
 *
 * The array returned by this function has the following values.
 *
 * Array Index | Mouse Button
 * ----------: | ------------
 * `0`         | Left
 * `1`         | Right
 * `2`         | Middle
 *
 * @return Array of mouse button pressed states or `NULL` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 */
const bool* AERInputGetMouseButtonsPressed(void);

/**
 * @brief Query the mouse button(s) that the user has held this step.
 *
 * For more information about the mouse buttons, see @ref MouseButtonStates.
 *
 * @return Array of mouse button held states or `NULL` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 */
const bool* AERInputGetMouseButtonsHeld(void);

/**
 * @brief Query the mouse button(s) that the user just released this step.
 *
 * For more information about the mouse buttons, see @ref MouseButtonStates.
 *
 * @return Array of mouse button released states or `NULL` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 */
const bool* AERInputGetMouseButtonsReleased(void);

/**
 * @brief Query the current position of the mouse cursor in pixels.
 *
 * If only one component of the position is needed, then the argument for the
 * unneeded component may be `NULL`.
 *
 * @note This function returns coordinates relative to the game window,
 * not the current room.
 *
 * @param[out] x Horizontal position.
 * @param[out] y Vertical position.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if both arguments `x` and `y` are `NULL`.
 *
 * @since 1.0.0
 */
void AERInputGetMousePosition(uint32_t* x, uint32_t* y);

#endif /* AER_INPUT_H */
