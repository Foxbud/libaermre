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

/* ----- DOCUMENTATION PAGES ----- */

/**
 * @mainpage
 *
 * Welcome to the public API documentation for the Action-Event-Response (AER)
 * Mod Runtime Environment (MRE).
 *
 * For more information and tutorials, visit
 * https://fairburn.dev/blog/aer/start-here/.
 */

#endif /* AER_CORE_H */
