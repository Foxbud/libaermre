/**
 * @file
 *
 * @brief Utilities for querying and manipulating the game window and its views.
 *
 * @since 1.1.0
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
#ifndef AER_WINDOW_H
#define AER_WINDOW_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ----- PUBLIC FUNCTIONS ----- */

// size_t AERViewGetByVisible(size_t bufSize, uint32_t *viewBuf);

// bool AERViewGetVisible(uint32_t viewIdx);

void AERViewGetPositionInRoom(uint32_t viewIdx, float *x, float *y);

void AERViewGetSizeInRoom(uint32_t viewIdx, float *width, float *height);

// void AERViewGetPositionInWindow(uint32_t viewIdx, int32_t *x, int32_t *y);

// void AERViewGetSizeInWindow(uint32_t viewIdx, int32_t *width, int32_t
// *height);

// void AERViewGetBorder(uint32_t viewIdx, int32_t *width, int32_t *height);

#endif /* AER_WINDOW_H */