/**
 * @file
 *
 * @brief Utilities for querying and manipulating save file data.
 *
 * @subsubsection SaveKeyNamespacing Save Key Namespacing
 *
 * Each mod is given its own, unique namespace within the save file. That means
 * two or more mods can use the same key without interfering with each other.
 * That also means it is not possible to query/manipulate vanilla keys or
 * the keys of other mods.
 *
 * @note The functions in this module automatically apply to the current save
 * slot.
 *
 * @since 1.2.0
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
#ifndef AER_SAVE_H
#define AER_SAVE_H

#include <stddef.h>
#include <stdint.h>

/* ----- PUBLIC FUNCTIONS ----- */

/**
 * @brief Query the index of the currently active save slot.
 *
 * @bug When a save has not yet been loaded (such as is the case at the title
 * screen), this function returns `0`.
 *
 * @return Save slot index or `-1` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.2.0
 */
int32_t AERSaveGetCurrentSlot(void);

/**
 * @brief Query the keys of all values in the save file (belonging to the
 * calling mod).
 *
 * For information about key namespacing see @ref SaveKeyNamespacing.
 *
 * @warning The references returned by this function should be considered highly
 * unstable. Consider deepcopying them if needed for later.
 *
 * @warning Argument `keyBuf` must be large enough to hold at least
 * `bufSize` elements.
 *
 * @note Argument `bufSize` may be `0` in which case argument `keyBuf` may
 * be `NULL`. This may be used to efficiently query the total number of
 * keys contained in the save file (belonging to the calling mod).
 *
 * @param[in] bufSize Maximum number of elements to write to argument
 * `keyBuf`.
 * @param[out] keyBuf Buffer to write keys to.
 *
 * @return Total number of keys contained in save file or `0` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `keyBuf` is `NULL` and argument `bufSize`
 * is greater than `0`.
 *
 * @since 1.2.0
 */
size_t AERSaveGetKeys(size_t bufSize, const char **keyBuf);

/**
 * @brief Erase a value from the save file.
 *
 * For information about key namespacing see @ref SaveKeyNamespacing.
 *
 * @note Changes will take effect at next save point.
 *
 * @param[in] key Save key.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `key` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `key` is an invalid save key.
 *
 * @since 1.2.0
 */
void AERSaveDestroy(const char *key);

/**
 * @brief Query a double value from the save file.
 *
 * For information about key namespacing see @ref SaveKeyNamespacing.
 *
 * @param[in] key Save key.
 *
 * @return Value of the key or `0.0` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `key` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `key` is an invalid save key.
 * @throw ::AER_FAILED_PARSE if key does not contain a double value.
 *
 * @since 1.2.0
 *
 * @sa AERSaveSetDouble
 */
double AERSaveGetDouble(const char *key);

/**
 * @brief Set a double value in the save file.
 *
 * This function can both create new and update existing entries in the save
 * file (regardless of type).
 *
 * For information about key namespacing see @ref SaveKeyNamespacing.
 *
 * @note Changes will take effect at next save point.
 *
 * @param[in] key Save key.
 * @param[in] value Double save value.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `key` is `NULL`.
 * @throw ::AER_BAD_VAL if argument `value` is `Inf`, `-Inf` or `NaN`.
 *
 * @since 1.2.0
 *
 * @sa AERSaveGetDouble
 */
void AERSaveSetDouble(const char *key, double value);

/**
 * @brief Query a string value from the save file.
 *
 * For information about key namespacing see @ref SaveKeyNamespacing.
 *
 * @warning The reference returned by this function should be considered highly
 * unstable. Consider deepcopying it if needed for later.
 *
 * @param[in] key Save key.
 *
 * @return Value of the key or `NULL` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `key` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `key` is an invalid save key.
 * @throw ::AER_FAILED_PARSE if key does not contain a string value.
 *
 * @since 1.2.0
 *
 * @sa AERSaveSetString
 */
const char *AERSaveGetString(const char *key);

/**
 * @brief Set a string value in the save file.
 *
 * This function can both create new and update existing entries in the save
 * file (regardless of type).
 *
 * For information about key namespacing see @ref SaveKeyNamespacing.
 *
 * @note Changes will take effect at next save point.
 *
 * @param[in] key Save key.
 * @param[in] value String save value.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if either argument `key` or `value` is `NULL`.
 *
 * @since 1.2.0
 *
 * @sa AERSaveGetString
 */
void AERSaveSetString(const char *key, const char *value);

#endif /* AER_SAVE_H */
