/**
 * @file
 *
 * @brief Utilities for querying and manipulating fonts.
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
#ifndef AER_FONT_H
#define AER_FONT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ----- PUBLIC TYPES ----- */

/**
 * @brief Vanilla fonts.
 *
 * @since 1.1.0
 */
typedef enum AERFontIndex {
    /**
     * @brief Flag which represents either no font or an invalid font depending
     * on context.
     */
    AER_FONT_NULL = -1,
    AER_FONT_ARIAL = 0x0,
    AER_FONT_UNI = 0x1,
    AER_FONT_CYR = 0x2,
    AER_FONT_JP = 0x3,
    AER_FONT_EDITORSMALL = 0x4,
    AER_FONT_EDITOR = 0x5,
    AER_FONT_IMAGINE = 0x6
} AERFontIndex;

/* ----- PUBLIC FUNCTIONS ----- */

/**
 * @brief Query the currently active font.
 *
 * @return Index of current font or @ref ::AER_FONT_NULL if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.1.0
 */
int32_t AERFontGetCurrent(void);

/**
 * @brief Change the currently active font to a new one.
 *
 * @param[in] fontIdx Index of new font.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_FAILED_LOOKUP if argument `fontIdx` is an invalid font.
 *
 * @since 1.1.0
 */
void AERFontSetCurrent(int32_t fontIdx);

/**
 * @brief Register a custom font.
 *
 * @note This function only supports the *TrueType* font format (`*.ttf`).
 *
 * @param[in] filename Path to font file relative to asset directory.
 * @param[in] size Pixel size of font.
 * @param[in] bold Whether font is bold.
 * @param[in] italic Whether font is italic.
 * @param[in] first Index of first font character to include.
 * @param[in] last Index of last font character to include.
 *
 * @return Index of new font or ::AER_FONT_NULL if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `filename` is `NULL`.
 * @throw ::AER_SEQ_BREAK if called outside font registration stage.
 * @throw ::AER_BAD_FILE if argument `filename` does not point to valid file.
 *
 * @since 1.1.0
 */
int32_t AERFontRegister(const char* filename,
                        size_t size,
                        bool bold,
                        bool italic,
                        int32_t first,
                        int32_t last);

/**
 * @brief Query the total number of vanilla and mod fonts registered.
 *
 * @return Number of fonts or `0` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called before start of font registration stage.
 *
 * @since 1.1.0
 */
size_t AERFontGetNumRegistered(void);

/**
 * @brief Query the name of a font.
 *
 * @param[in] fontIdx Font of interest.
 *
 * @return Name of font or `NULL` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called before start of font registration stage.
 * @throw ::AER_FAILED_LOOKUP if argument `fontIdx` is an invalid font.
 *
 * @since 1.1.0
 */
const char* AERFontGetName(int32_t fontIdx);

/**
 * @brief Query the size of a font in pixels.
 *
 * @param[in] fontIdx Font of interest.
 *
 * @return Pixel size of font or `0` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called before start of font registration stage.
 * @throw ::AER_FAILED_LOOKUP if argument `fontIdx` is an invalid font.
 *
 * @since 1.1.0
 */
size_t AERFontGetSize(int32_t fontIdx);

/**
 * @brief Query whether a font is bold.
 *
 * @param[in] fontIdx Font of interest.
 *
 * @return Whether font is bold or `false` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called before start of font registration stage.
 * @throw ::AER_FAILED_LOOKUP if argument `fontIdx` is an invalid font.
 *
 * @since 1.1.0
 */
bool AERFontGetBold(int32_t fontIdx);

/**
 * @brief Query whether a font is italic.
 *
 * @param[in] fontIdx Font of interest.
 *
 * @return Whether font is italic or `false` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called before start of font registration stage.
 * @throw ::AER_FAILED_LOOKUP if argument `fontIdx` is an invalid font.
 *
 * @since 1.1.0
 */
bool AERFontGetItalic(int32_t fontIdx);

/**
 * @brief Query the index of the first character in a font.
 *
 * @param[in] fontIdx Font of interest.
 *
 * @return Index of first character or `-1` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called before start of font registration stage.
 * @throw ::AER_FAILED_LOOKUP if argument `fontIdx` is an invalid font.
 *
 * @since 1.1.0
 */
int32_t AERFontGetFirst(int32_t fontIdx);

/**
 * @brief Query the index of the last character in a font.
 *
 * @param[in] fontIdx Font of interest.
 *
 * @return Index of last character or `-1` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called before start of font registration stage.
 * @throw ::AER_FAILED_LOOKUP if argument `fontIdx` is an invalid font.
 *
 * @since 1.1.0
 */
int32_t AERFontGetLast(int32_t fontIdx);

#endif /* AER_FONT_H */