/**
 * @file
 *
 * @brief Uncategorized functions for querying and manipulating game state.
 *
 * @subsubsection MouseButtonStates Mouse Button States
 *
 * For functions that query information about the state of the mouse buttons,
 * the following table shows how array indexes map to buttons.
 *
 * Array Index | Mouse Button
 * ----------: | ------------
 * 0           | Left
 * 1           | Right
 * 2           | Middle
 *
 * @since 1.0.0
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
 * @return Number of steps.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 */
uint32_t AERGetNumSteps(void);

/**
 * @brief Query the keyboard key(s) that the user just pressed this step.
 *
 * @return Lookup table mapping extended ASCII key-codes to pressed state.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 */
const bool * AERGetKeysPressed(void);

/**
 * @brief Query the keyboard key(s) that the user has held this step.
 *
 * @return Lookup table mapping extended ASCII key-codes to held state.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 */
const bool * AERGetKeysHeld(void);

/**
 * @brief Query the keyboard key(s) that the user just released this step.
 *
 * @return Lookup table mapping extended ASCII key-codes to released state.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 */
const bool * AERGetKeysReleased(void);

/**
 * @brief Query the mouse button(s) that the user just pressed this step.
 *
 * @return Array of mouse button pressed states. See @ref MouseButtonStates
 * for more information.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 */
const bool * AERGetMouseButtonsPressed(void);

/**
 * @brief Query the mouse button(s) that the user had held this step.
 *
 * @return Array of mouse button held states. See @ref MouseButtonStates
 * for more information.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 */
const bool * AERGetMouseButtonsHeld(void);

/**
 * @brief Query the mouse button(s) that the user just released this step.
 *
 * @return Array of mouse button released states. See @ref MouseButtonStates
 * for more information.
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



#endif /* AER_MRE_H */
