/**
 * @file
 *
 * @brief Uncategorized functions for querying and manipulating game state.
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
#ifndef AER_CORE_H
#define AER_CORE_H

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
 * @brief Query the runtime of the previous step in seconds.
 *
 * The value returned by this function has microsecond precision.
 *
 * @return Runtime in seconds or `0` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since {{MRE_NEXT_MINOR}}
 */
double AERGetDeltaTime(void);

/**
 * @brief Query the pause state of the game.
 *
 * @return `true` if game is paused or `false` if unsuccessful or game is not
 * paused.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 *
 * @sa AERModDef::gamePauseListener
 */
bool AERGetPaused(void);

#endif /* AER_CORE_H */
