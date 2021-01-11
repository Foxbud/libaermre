/**
 * @file
 *
 * @brief Utilities for parsing configuration data.
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
#ifndef AER_CONF_H
#define AER_CONF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ----- PUBLIC FUNCTIONS ----- */

/**
 * @brief Parse a boolean value from the configuration data.
 *
 * @param[in] key Configuration key.
 *
 * @return Value of the key or `false` if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `key` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `key` is an invalid configuration
 * key.
 * @throw ::AER_FAILED_PARSE if key does not contain a boolean value.
 *
 * @since 1.0.0
 *
 * @sa AERConfManGetBools
 */
bool AERConfGetBool(const char *key);

/**
 * @brief Parse an array of boolean values from the configuration data.
 *
 * @warning Argument `boolBuf` must be large enough to hold at least
 * `bufSize` elements.
 *
 * @note Argument `bufSize` may be `0` in which case argument `boolBuf` may
 * be `NULL`. This may be used to efficiently query the total number of
 * booleans contained in the configuration data.
 *
 * @param[in] key Configuration key.
 * @param[in] bufSize Maximum number of elements to write to argument
 * `boolBuf`.
 * @param[out] boolBuf Buffer to write booleans to.
 *
 * @return Total number of booleans contained in configuration data or `0`
 * if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `key` is `NULL` or argument `boolBuf`
 * is `NULL` and argument `bufSize` is greater than `0`.
 * @throw ::AER_FAILED_LOOKUP if argument `key` is an invalid configuration
 * key.
 * @throw ::AER_FAILED_PARSE if key does not contain boolean values.
 *
 * @since 1.0.0
 *
 * @sa AERConfManGetBool
 */
size_t AERConfGetBools(const char *key, size_t bufSize, bool *boolBuf);

/**
 * @brief Parse an integer value from the configuration data.
 *
 * @param[in] key Configuration key.
 *
 * @return Value of the key or `0` if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `key` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `key` is an invalid configuration
 * key.
 * @throw ::AER_FAILED_PARSE if key does not contain an integer value.
 *
 * @since 1.0.0
 *
 * @sa AERConfManGetInts
 */
int64_t AERConfGetInt(const char *key);

/**
 * @brief Parse an array of integer values from the configuration data.
 *
 * @warning Argument `intBuf` must be large enough to hold at least
 * `bufSize` elements.
 *
 * @note Argument `bufSize` may be `0` in which case argument `intBuf` may
 * be `NULL`. This may be used to efficiently query the total number of
 * integers contained in the configuration data.
 *
 * @param[in] key Configuration key.
 * @param[in] bufSize Maximum number of elements to write to argument
 * `intBuf`.
 * @param[out] intBuf Buffer to write integers to.
 *
 * @return Total number of integers contained in configuration data or `0`
 * if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `key` is `NULL` or argument `intBuf`
 * is `NULL` and argument `bufSize` is greater than `0`.
 * @throw ::AER_FAILED_LOOKUP if argument `key` is an invalid configuration
 * key.
 * @throw ::AER_FAILED_PARSE if key does not contain integer values.
 *
 * @since 1.0.0
 *
 * @sa AERConfManGetInt
 */
size_t AERConfGetInts(const char *key, size_t bufSize, int64_t *intBuf);

/**
 * @brief Parse a double value from the configuration data.
 *
 * @param[in] key Configuration key.
 *
 * @return Value of the key or `0.0` if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `key` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `key` is an invalid configuration
 * key.
 * @throw ::AER_FAILED_PARSE if key does not contain a double value.
 *
 * @since 1.0.0
 *
 * @sa AERConfManGetDoubles
 */
double AERConfGetDouble(const char *key);

/**
 * @brief Parse an array of double values from the configuration data.
 *
 * @warning Argument `doubleBuf` must be large enough to hold at least
 * `bufSize` elements.
 *
 * @note Argument `bufSize` may be `0` in which case argument `doubleBuf` may
 * be `NULL`. This may be used to efficiently query the total number of
 * doubles contained in the configuration data.
 *
 * @param[in] key Configuration key.
 * @param[in] bufSize Maximum number of elements to write to argument
 * `doubleBuf`.
 * @param[out] doubleBuf Buffer to write doubles to.
 *
 * @return Total number of doubles contained in configuration data or `0`
 * if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `key` is `NULL` or argument `doubleBuf`
 * is `NULL` and argument `bufSize` is greater than `0`.
 * @throw ::AER_FAILED_LOOKUP if argument `key` is an invalid configuration
 * key.
 * @throw ::AER_FAILED_PARSE if key does not contain double values.
 *
 * @since 1.0.0
 *
 * @sa AERConfManGetDouble
 */
size_t AERConfGetDoubles(const char *key, size_t bufSize, double *doubleBuf);

/**
 * @brief Parse a string from the configuration data.
 *
 * @param[in] key Configuration key.
 *
 * @return Value of the key or `NULL` if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `key` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `key` is an invalid configuration
 * key.
 * @throw ::AER_FAILED_PARSE if key does not contain a string.
 *
 * @since 1.0.0
 *
 * @sa AERConfManGetStrings
 */
const char *AERConfGetString(const char *key);

/**
 * @brief Parse an array of strings from the configuration data.
 *
 * @warning Argument `strBuf` must be large enough to hold at least
 * `bufSize` elements.
 *
 * @note Argument `bufSize` may be `0` in which case argument `strBuf` may
 * be `NULL`. This may be used to efficiently query the total number of
 * strings contained in the configuration data.
 *
 * @param[in] key Configuration key.
 * @param[in] bufSize Maximum number of elements to write to argument
 * `strBuf`.
 * @param[out] strBuf Buffer to write strings to.
 *
 * @return Total number of strings contained in configuration data or `0`
 * if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `key` is `NULL` or argument `strBuf`
 * is `NULL` and argument `bufSize` is greater than `0`.
 * @throw ::AER_FAILED_LOOKUP if argument `key` is an invalid configuration
 * key.
 * @throw ::AER_FAILED_PARSE if key does not contain strings.
 *
 * @since 1.0.0
 *
 * @sa AERConfManGetString
 */
size_t AERConfGetStrings(const char *key, size_t bufSize, const char **strBuf);

#endif /* AER_CONF_H */
