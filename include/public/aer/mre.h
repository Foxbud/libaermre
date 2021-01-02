/**
 * @file
 *
 * @brief Uncategorized functions for querying and manipulating game state.
 *
 * @since 1.0.0
 *
 * @copyright 2020 the libaermre authors
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
#ifndef AER_MRE_H
#define AER_MRE_H

#include <stdbool.h>
#include <stdint.h>



/* ----- PUBLIC FUNCTIONS ----- */

/**
 * @brief Query the number of steps (ticks) elapsed since the start
 * of the game.
 *
 * @return Number of steps or `0` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 */
uint32_t AERGetNumSteps(void);

/**
 * @brief Query the keyboard key(s) that the user just pressed this step.
 *
 * @return Lookup table mapping extended ASCII key-codes to pressed state
 * or `NULL` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 */
const bool * AERGetKeysPressed(void);

/**
 * @brief Query the keyboard key(s) that the user has held this step.
 *
 * @return Lookup table mapping extended ASCII key-codes to held state or
 * `NULL` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 */
const bool * AERGetKeysHeld(void);

/**
 * @brief Query the keyboard key(s) that the user just released this step.
 *
 * @return Lookup table mapping extended ASCII key-codes to released state
 * or `NULL` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 */
const bool * AERGetKeysReleased(void);

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
const bool * AERGetMouseButtonsPressed(void);

/**
 * @brief Query the mouse button(s) that the user had held this step.
 *
 * For more information about the mouse buttons, see @ref MouseButtonStates.
 *
 * @return Array of mouse button held states or `NULL` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 */
const bool * AERGetMouseButtonsHeld(void);

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
const bool * AERGetMouseButtonsReleased(void);

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
void AERGetMousePosition(
		uint32_t * x,
		uint32_t * y
);



/* ----- DOCUMENTATION PAGES ----- */

/**
 * @mainpage
 *
 * Welcome to the public API documentation for the Action-Event-Response (AER)
 * Mod Runtime Environment (MRE)!
 */



#endif /* AER_MRE_H */
