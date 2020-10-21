/**
 * @file
 *
 * @brief Uncategorized functions for querying and manipulating game state.
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
 */
uint32_t AERGetNumSteps(void);

/**
 * @brief Query the keyboard key(s) that the user just pressed this step.
 *
 * @return Lookup table mapping extended ASCII key-codes to pressed state.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 */
const bool * AERGetKeysPressed(void);

/**
 * @brief Query the keyboard key(s) that the user has held this step.
 *
 * @return Lookup table mapping extended ASCII key-codes to held state.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 */
const bool * AERGetKeysHeld(void);

/**
 * @brief Query the keyboard key(s) that the user just released this step.
 *
 * @return Lookup table mapping extended ASCII key-codes to released state.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 */
const bool * AERGetKeysReleased(void);



#endif /* AER_MRE_H */
