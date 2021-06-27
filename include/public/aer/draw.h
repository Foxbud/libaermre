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
 * @throw ::AER_SEQ_BREAK if called outside action stage.
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
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_BAD_VAL if argument `alpha` is less than `0.0f` or greater
 * than `1.0f`.
 *
 * @since 1.1.0
 */
void AERDrawSetCurrentAlpha(float alpha);

/**
 * @brief Draw a sprite to the screen.
 *
 * @param[in] spriteIdx Sprite to draw.
 * @param[in] frame Frame of sprite to draw.
 * @param[in] x Horizontal position at which to draw sprite.
 * @param[in] y Vertical position at which to draw sprite.
 * @param[in] scale Horizontal and vertical scale of text.
 * @param[in] blend Color to blend with sprite. See @ref DrawColors for more
 * infomation.
 *
 * @throw ::AER_SEQ_BREAK if called outside draw stage.
 * @throw ::AER_FAILED_LOOKUP if argument `spriteIdx` is an invalid sprite.
 * @throw ::AER_BAD_VAL if argument `frame` is greater than or equal to the
 * number of frames in sprite.
 *
 * @since 1.4.0
 *
 * @sa AERDrawSpriteAdv
 */
void AERDrawSprite(int32_t spriteIdx,
                   uint32_t frame,
                   float x,
                   float y,
                   float scale,
                   uint32_t blend);

/**
 * @brief Draw a sprite to the screen.
 *
 * If more than one blend color is provided, then the engine will render a
 * smooth gradient.
 *
 * @param[in] spriteIdx Sprite to draw.
 * @param[in] frame Frame of sprite to draw.
 * @param[in] left Horizontal offset into sprite to draw in pixels.
 * @param[in] top Vertical offset into sprite to draw in pixels.
 * @param[in] width Horizontal component of sprite to draw in pixels.
 * @param[in] height Vertical component of sprite to draw in pixels.
 * @param[in] x Horizontal position at which to draw sprite.
 * @param[in] y Vertical position at which to draw sprite.
 * @param[in] scaleX Horizontal scale of sprite.
 * @param[in] scaleY Vertical scale of sprite.
 * @param[in] angle Sprite offset angle in degrees.
 * @param[in] blendNW Blend color of northwest corner. See @ref DrawColors for
 * more infomation.
 * @param[in] blendNE Blend color of northeast corner. See @ref DrawColors for
 * more infomation.
 * @param[in] blendSE Blend color of southeast corner. See @ref DrawColors for
 * more infomation.
 * @param[in] blendSW Blend color of southwest corner. See @ref DrawColors for
 * more infomation.
 * @param[in] alpha Sprite alpha (transparency).
 *
 * @throw ::AER_SEQ_BREAK if called outside draw stage.
 * @throw ::AER_FAILED_LOOKUP if argument `spriteIdx` is an invalid sprite.
 * @throw ::AER_BAD_VAL if argument `alpha` is less than `0.0f` or greater
 * than `1.0f` or if argument `frame` is greater than or equal to the
 * number of frames in sprite.
 *
 * @since 1.4.0
 *
 * @sa AERDrawSprite
 */
void AERDrawSpriteAdv(int32_t spriteIdx,
                      uint32_t frame,
                      int32_t left,
                      int32_t top,
                      int32_t width,
                      int32_t height,
                      float x,
                      float y,
                      float scaleX,
                      float scaleY,
                      float angle,
                      uint32_t blendNW,
                      uint32_t blendNE,
                      uint32_t blendSE,
                      uint32_t blendSW,
                      float alpha);

/**
 * @brief Draw a line to the screen.
 *
 * @param[in] x1 Horizontal position of line start.
 * @param[in] y1 Vertical position of line start.
 * @param[in] x2 Horizontal position of line end.
 * @param[in] y2 Vertical position of line end.
 * @param[in] color Color of the line. See @ref DrawColors for more
 * infomation.
 *
 * @throw ::AER_SEQ_BREAK if called outside draw stage.
 *
 * @since 1.3.0
 *
 * @sa AERDrawLineAdv
 */
void AERDrawLine(float x1, float y1, float x2, float y2, uint32_t color);

/**
 * @brief Draw a line to the screen.
 *
 * If more than one color is provided, then the engine will render a smooth
 * gradient.
 *
 * @param[in] x1 Horizontal position of line start.
 * @param[in] y1 Vertical position of line start.
 * @param[in] x2 Horizontal position of line end.
 * @param[in] y2 Vertical position of line end.
 * @param[in] width Line width in pixels.
 * @param[in] color1 Color of the start of the line. See @ref DrawColors for
 * more infomation.
 * @param[in] color2 Color of the end of the line. See @ref DrawColors for
 * more infomation.
 *
 * @throw ::AER_SEQ_BREAK if called outside draw stage.
 *
 * @since 1.3.0
 *
 * @sa AERDrawLine
 */
void AERDrawLineAdv(float x1,
                    float y1,
                    float x2,
                    float y2,
                    float width,
                    uint32_t color1,
                    uint32_t color2);

/**
 * @brief Draw an ellipse to the screen.
 *
 * @param[in] left X-intercept of the left side of ellipse.
 * @param[in] top Y-intercept of the top side of ellipse.
 * @param[in] right X-intercept of the right side of ellipse.
 * @param[in] bottom Y-intercept of the bottom side of ellipse.
 * @param[in] color Color of the ellipse. See @ref DrawColors for more
 * infomation.
 * @param[in] outline Whether to render a solid ellipse (`false`) or a `1`
 * pixel wide outline of an ellipse (`true`).
 *
 * @throw ::AER_SEQ_BREAK if called outside draw stage.
 *
 * @since 1.3.0
 *
 * @sa AERDrawEllipseAdv
 */
void AERDrawEllipse(float left,
                    float top,
                    float right,
                    float bottom,
                    uint32_t color,
                    bool outline);

/**
 * @brief Draw an ellipse to the screen.
 *
 * If more than one color is provided, then the engine will render a smooth
 * gradient.
 *
 * @param[in] left X-intercept of the left side of ellipse.
 * @param[in] top Y-intercept of the top side of ellipse.
 * @param[in] right X-intercept of the right side of ellipse.
 * @param[in] bottom Y-intercept of the bottom side of ellipse.
 * @param[in] colorCenter Color of the center ellipse. See @ref DrawColors for
 * more infomation.
 * @param[in] colorEdge Color of the edge ellipse. See @ref DrawColors for more
 * infomation.
 * @param[in] outline Whether to render a solid ellipse (`false`) or a `1`
 * pixel wide outline of an ellipse (`true`).
 *
 * @throw ::AER_SEQ_BREAK if called outside draw stage.
 *
 * @since 1.3.0
 *
 * @sa AERDrawEllipse
 */
void AERDrawEllipseAdv(float left,
                       float top,
                       float right,
                       float bottom,
                       uint32_t colorCenter,
                       uint32_t colorEdge,
                       bool outline);

/**
 * @brief Draw a triangle to the screen.
 *
 * @param[in] x1 Horizontal position of first vertex.
 * @param[in] y1 Vertical position of first vertex.
 * @param[in] x2 Horizontal position of second vertex.
 * @param[in] y2 Vertical position of second vertex.
 * @param[in] x3 Horizontal position of third vertex.
 * @param[in] y3 Vertical position of third vertex.
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
void AERDrawTriangle(float x1,
                     float y1,
                     float x2,
                     float y2,
                     float x3,
                     float y3,
                     uint32_t color,
                     bool outline);

/**
 * @brief Draw a triangle to the screen.
 *
 * If more than one color is provided, then the engine will render a smooth
 * gradient.
 *
 * @param[in] x1 Horizontal position of first vertex.
 * @param[in] y1 Vertical position of first vertex.
 * @param[in] x2 Horizontal position of second vertex.
 * @param[in] y2 Vertical position of second vertex.
 * @param[in] x3 Horizontal position of third vertex.
 * @param[in] y3 Vertical position of third vertex.
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
void AERDrawTriangleAdv(float x1,
                        float y1,
                        float x2,
                        float y2,
                        float x3,
                        float y3,
                        uint32_t color1,
                        uint32_t color2,
                        uint32_t color3,
                        bool outline);

/**
 * @brief Draw a rectangle to the screen.
 *
 * @param[in] left X-intercept of the left side of rectangle.
 * @param[in] top Y-intercept of the top side of rectangle.
 * @param[in] right X-intercept of the right side of rectangle.
 * @param[in] bottom Y-intercept of the bottom side of rectangle.
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
void AERDrawRectangle(float left,
                      float top,
                      float right,
                      float bottom,
                      uint32_t color,
                      bool outline);

/**
 * @brief Draw a rectangle to the screen.
 *
 * If more than one color is provided, then the engine will render a smooth
 * gradient.
 *
 * @param[in] left X-intercept of the left side of rectangle.
 * @param[in] top Y-intercept of the top side of rectangle.
 * @param[in] right X-intercept of the right side of rectangle.
 * @param[in] bottom Y-intercept of the bottom side of rectangle.
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
void AERDrawRectangleAdv(float left,
                         float top,
                         float right,
                         float bottom,
                         uint32_t colorNW,
                         uint32_t colorNE,
                         uint32_t colorSE,
                         uint32_t colorSW,
                         bool outline);

/**
 * @brief Draw text to the screen.
 *
 * See @ref DrawTextEscape for more information about text escape sequences.
 *
 * @param[in] text String to draw. Maximum size is 8192 characters including
 * null terminator (will not cause error if larger).
 * @param[in] x Horizontal position at which to draw text.
 * @param[in] y Vertical position at which to draw text.
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
void AERDrawText(const char* text,
                 float x,
                 float y,
                 uint32_t width,
                 float scale,
                 uint32_t color);

/**
 * @brief Draw text to the screen.
 *
 * If more than one color is provided, then the engine will render a smooth
 * gradient.
 *
 * @subsubsection DrawTextEscape Text Escape Sequences
 *
 * The game maker engine chose to use the hashtag ('#') character to represent
 * linebreaks. To display a literal hashtag, preceed it with a backslash.
 *
 * @param[in] text String to draw. Maximum size is 8192 characters including
 * null terminator (will not cause error if larger).
 * @param[in] x Horizontal position at which to draw text.
 * @param[in] y Vertical position at which to draw text.
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
void AERDrawTextAdv(const char* text,
                    float x,
                    float y,
                    int32_t height,
                    uint32_t width,
                    float scaleX,
                    float scaleY,
                    float angle,
                    uint32_t colorNW,
                    uint32_t colorNE,
                    uint32_t colorSE,
                    uint32_t colorSW,
                    float alpha);

#endif /* AER_DRAW_H */