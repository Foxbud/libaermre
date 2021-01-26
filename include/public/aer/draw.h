/**
 * @file
 *
 * @brief Utilities for drawing to the screen.
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
#ifndef AER_DRAW_H
#define AER_DRAW_H

#include <stdbool.h>
#include <stdint.h>

/* ----- PUBLIC FUNCTIONS ----- */

void AERDrawRectangle(float left, float top, float right, float bottom,
                      uint32_t color, bool outline);

void AERDrawRectangleAdv(float left, float top, float right, float bottom,
                         uint32_t colorNW, uint32_t colorNE, uint32_t colorSE,
                         uint32_t colorSW, bool outline);

void AERDrawText(const char *text, float x, float y, uint32_t width,
                 float scale, uint32_t color);

void AERDrawTextAdv(const char *text, float x, float y, int32_t height,
                    uint32_t width, float scaleX, float scaleY, float angle,
                    uint32_t colorNW, uint32_t colorNE, uint32_t colorSE,
                    uint32_t colorSW, float alpha);

#endif /* AER_DRAW_H */