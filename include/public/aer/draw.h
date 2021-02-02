/**
 * @file
 *
 * @brief Utilities for drawing to the screen.
 *
 * The functions in this module should only be called from inside object draw
 * event listeners. See @ref AERObjectAttachDrawListener for more information.
 *
 * @subsubsection DrawColors Colors
 *
 * Colors are represented as unsigned 32-bit integers. The least significant
 * byte is the red component, the next byte is the green component, the second
 * most significant byte is the blue component and the most significant byte is
 * unused (it is **not** used for alpha).
 *
 * This means that expressing a color in hexadecimal requires writing it in
 * "BGR" order (i.e. the color red=0x0, green=0x70, blue=0xf0 would be written
 * as `0xf07000`).
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

/**
 * @brief Query the current global draw alpha.
 *
 * @subsubsection GlobalAlpha Global Alpha
 *
 * This value is implicitly blended into the final result of all draw functions.
 * For functions that take an explicit alpha value, the global alpha is still
 * factored in.
 *
 * @return Global alpha or `-1.0f` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside draw stage.
 *
 * @since 1.1.0
 */
float AERDrawGetCurrentAlpha(void);

/**
 * @brief Set the current global draw alpha.
 *
 * See @ref GlobalAlpha for more information about this value.
 *
 * @param[in] alpha Global alpha.
 *
 * @throw ::AER_SEQ_BREAK if called outside draw stage.
 * @throw ::AER_BAD_VAL if argument `alpha` is less than `0.0f` or greater
 * than `1.0f`.
 *
 * @since 1.1.0
 */
void AERDrawSetCurrentAlpha(float alpha);

/**
 * @brief Draw a triangle to the screen.
 *
 * @param[in] x1 Horizontal position of first vertex (in room space).
 * @param[in] y1 Vertical position of first vertex (in room space).
 * @param[in] x2 Horizontal position of second vertex (in room space).
 * @param[in] y2 Vertical position of second vertex (in room space).
 * @param[in] x3 Horizontal position of third vertex (in room space).
 * @param[in] y3 Vertical position of third vertex (in room space).
 * @param[in] color Color of the triangle. See @ref DrawColors for more
 * infomation.
 * @param[in] outline Whether to render a solid triangle (`false`) or a `1`
 * pixel wide outline of a triangle (`true`).
 *
 * @throw ::AER_SEQ_BREAK if called outside draw stage.
 *
 * @since 1.1.0
 *
 * @sa AERDrawTriangleAdv
 */
void AERDrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3,
                     uint32_t color, bool outline);

/**
 * @brief Draw a triangle to the screen.
 *
 * If more than one color is provided, then the engine will render a smooth
 * gradient.
 *
 * @param[in] x1 Horizontal position of first vertex (in room space).
 * @param[in] y1 Vertical position of first vertex (in room space).
 * @param[in] x2 Horizontal position of second vertex (in room space).
 * @param[in] y2 Vertical position of second vertex (in room space).
 * @param[in] x3 Horizontal position of third vertex (in room space).
 * @param[in] y3 Vertical position of third vertex (in room space).
 * @param[in] color1 Color of first vertex. See @ref DrawColors for more
 * infomation.
 * @param[in] color2 Color of second vertex. See @ref DrawColors for more
 * infomation.
 * @param[in] color3 Color of third vertex. See @ref DrawColors for more
 * infomation.
 * @param[in] outline Whether to render a solid triangle (`false`) or a `1`
 * pixel wide outline of a triangle (`true`).
 *
 * @throw ::AER_SEQ_BREAK if called outside draw stage.
 *
 * @since 1.1.0
 *
 * @sa AERDrawTriangle
 */
void AERDrawTriangleAdv(float x1, float y1, float x2, float y2, float x3,
                        float y3, uint32_t color1, uint32_t color2,
                        uint32_t color3, bool outline);

/**
 * @brief Draw a rectangle to the screen.
 *
 * @param[in] left X-intercept of the left side of rectangle (in room space).
 * @param[in] top Y-intercept of the top side of rectangle (in room space).
 * @param[in] right X-intercept of the right side of rectangle (in room space).
 * @param[in] bottom Y-intercept of the bottom side of rectangle (in room
 * space).
 * @param[in] color Color of the rectangle. See @ref DrawColors for more
 * infomation.
 * @param[in] outline Whether to render a solid rectangle (`false`) or a `1`
 * pixel wide outline of a rectangle (`true`).
 *
 * @throw ::AER_SEQ_BREAK if called outside draw stage.
 *
 * @since 1.1.0
 *
 * @sa AERDrawRectangleAdv
 */
void AERDrawRectangle(float left, float top, float right, float bottom,
                      uint32_t color, bool outline);

/**
 * @brief Draw a rectangle to the screen.
 *
 * If more than one color is provided, then the engine will render a smooth
 * gradient.
 *
 * @param[in] left X-intercept of the left side of rectangle (in room space).
 * @param[in] top Y-intercept of the top side of rectangle (in room space).
 * @param[in] right X-intercept of the right side of rectangle (in room space).
 * @param[in] bottom Y-intercept of the bottom side of rectangle (in room
 * space).
 * @param[in] colorNW Color of northwest vertex. See @ref DrawColors for more
 * infomation.
 * @param[in] colorNE Color of northeast vertex. See @ref DrawColors for more
 * infomation.
 * @param[in] colorSE Color of southeast vertex. See @ref DrawColors for more
 * infomation.
 * @param[in] colorSW Color of southwest vertex. See @ref DrawColors for more
 * infomation.
 * @param[in] outline Whether to render a solid rectangle (`false`) or a `1`
 * pixel wide outline of a rectangle (`true`).
 *
 * @throw ::AER_SEQ_BREAK if called outside draw stage.
 *
 * @since 1.1.0
 *
 * @sa AERDrawRectangle
 */
void AERDrawRectangleAdv(float left, float top, float right, float bottom,
                         uint32_t colorNW, uint32_t colorNE, uint32_t colorSE,
                         uint32_t colorSW, bool outline);

/**
 * @brief Draw text to the screen.
 *
 * @param[in] text String to draw. Maximum size is 1024 characters including
 * null terminator (will not cause error if larger).
 * @param[in] x Horizontal position at which to draw text (in room space).
 * @param[in] y Vertical position at which to draw text (in room space).
 * @param[in] width Maximum line width before line break in pixels (not
 * characters).
 * @param[in] scale Horizontal and vertical scale of text.
 * @param[in] color Color of text. See @ref DrawColors for more
 * infomation.
 *
 * @throw ::AER_SEQ_BREAK if called outside draw stage.
 * @throw ::AER_NULL_ARG if argument `text` is `NULL`.
 *
 * @since 1.1.0
 *
 * @sa AERDrawTextAdv
 */
void AERDrawText(const char *text, float x, float y, uint32_t width,
                 float scale, uint32_t color);

/**
 * @brief Draw text to the screen.
 *
 * If more than one color is provided, then the engine will render a smooth
 * gradient.
 *
 * @param[in] text String to draw. Maximum size is 1024 characters including
 * null terminator (will not cause error if larger).
 * @param[in] x Horizontal position at which to draw text (in room space).
 * @param[in] y Vertical position at which to draw text (in room space).
 * @param[in] height Space between each line of text in pixels.
 * @param[in] width Maximum line width before line break in pixels (not
 * characters).
 * @param[in] scaleX Horizontal scale of text.
 * @param[in] scaleY Vertical scale of text.
 * @param[in] angle Text offset angle in degrees.
 * @param[in] colorNW Color of northwest corner. See @ref DrawColors for more
 * infomation.
 * @param[in] colorNE Color of northeast corner. See @ref DrawColors for more
 * infomation.
 * @param[in] colorSE Color of southeast corner. See @ref DrawColors for more
 * infomation.
 * @param[in] colorSW Color of southwest corner. See @ref DrawColors for more
 * infomation.
 * @param[in] alpha Text alpha (transparency).
 *
 * @throw ::AER_SEQ_BREAK if called outside draw stage.
 * @throw ::AER_NULL_ARG if argument `text` is `NULL`.
 * @throw ::AER_BAD_VAL if argument `alpha` is less than `0.0f` or greater
 * than `1.0f`.
 *
 * @since 1.1.0
 *
 * @sa AERDrawText
 */
void AERDrawTextAdv(const char *text, float x, float y, int32_t height,
                    uint32_t width, float scaleX, float scaleY, float angle,
                    uint32_t colorNW, uint32_t colorNE, uint32_t colorSE,
                    uint32_t colorSW, float alpha);

#endif /* AER_DRAW_H */