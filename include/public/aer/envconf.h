/**
 * @file
 *
 * @brief Utilities for parsing configuration data from environment
 * variables.
 *
 * @since 1.0.0
 *
 * @copyright 2020 the libaermre authors
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
#ifndef AER_ENVCONF_H
#define AER_ENVCONF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



/* ----- PUBLIC FUNCTIONS ----- */

/**
 * @brief Parse a boolean value from an environment variable.
 *
 * @subsubsection ParsingBools Parsing Booleans
 *
 * The functions AEREnvConfGetBool() and AEREnvConfGetBools() parse booleans
 * from strings using the first character of the string, ignoring the rest.
 * If that character is any of:
 *  - `1`
 *  - `t`
 *  - `T`
 *  - `y`
 *  - `Y`
 * 
 * then the string is parsed as `true`. On the other hand, if the first
 * character is any of:
 *  - `0`
 *  - `f`
 *  - `F`
 *  - `n`
 *  - `N`
 * 
 * then the string is parsed as `false`. Otherwise a parse error occurs.
 *
 * @param[in] name Name of environment variable.
 *
 * @return Value of environment variable interpreted as a boolean
 * or `false` if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `name` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `name` is an invalid environment
 * variable.
 * @throw ::AER_FAILED_PARSE if environment variable could not be
 * interpreted as a boolean.
 *
 * @since 1.0.0
 *
 * @sa AEREnvConfGetBools
 */
bool AEREnvConfGetBool(const char * name);

/**
 * @brief Parse multiple whitespace-delimited boolean values from an
 * environment variable.
 *
 * For information about how this function parses boolean values, see
 * @ref ParsingBools.
 *
 * @warning Argument `boolBuf` must be large enough to hold at least
 * `bufSize` elements.
 *
 * @note Argument `bufSize` may be `0` in which case argument `boolBuf` may
 * be `NULL`. This may be used to efficiently query the total number of
 * potential booleans contained in an environment variable.
 *
 * @param[in] name Name of environment variable.
 * @param[in] bufSize Maximum number of elements to write to argument
 * `boolBuf`.
 * @param[out] boolBuf Buffer to write booleans to.
 *
 * @return Total number of booleans contained in environment variable or `0`
 * if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `name` is `NULL` or argument `boolBuf`
 * is `NULL` and argument `bufSize` is greater than `0`.
 * @throw ::AER_FAILED_LOOKUP if argument `name` is an invalid environment
 * variable.
 * @throw ::AER_FAILED_PARSE if environment variable could not be
 * interpreted as booleans.
 *
 * @since 1.0.0
 *
 * @sa AEREnvConfGetBool
 */
size_t AEREnvConfGetBools(
		const char * name,
		size_t bufSize,
		bool * boolBuf
);

/**
 * @brief Parse an integer value from an environment variable.
 *
 * @param[in] name Name of environment variable.
 *
 * @return Value of environment variable interpreted as an integer
 * or `0` if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `name` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `name` is an invalid environment
 * variable.
 * @throw ::AER_FAILED_PARSE if environment variable could not be
 * interpreted as an integer.
 *
 * @since 1.0.0
 *
 * @sa AEREnvConfGetInts
 */
int32_t AEREnvConfGetInt(const char * name);

/**
 * @brief Parse multiple whitespace-delimited integer values from an
 * environment variable.
 *
 * @warning Argument `intBuf` must be large enough to hold at least
 * `bufSize` elements.
 *
 * @note Argument `bufSize` may be `0` in which case argument `intBuf` may
 * be `NULL`. This may be used to efficiently query the total number of
 * potential integers contained in an environment variable.
 *
 * @param[in] name Name of environment variable.
 * @param[in] bufSize Maximum number of elements to write to argument
 * `intBuf`.
 * @param[out] intBuf Buffer to write integers to.
 *
 * @return Total number of integers contained in environment variable or `0`
 * if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `name` is `NULL` or argument `intBuf`
 * is `NULL` and argument `bufSize` is greater than `0`.
 * @throw ::AER_FAILED_LOOKUP if argument `name` is an invalid environment
 * variable.
 * @throw ::AER_FAILED_PARSE if environment variable could not be
 * interpreted as integers.
 *
 * @since 1.0.0
 *
 * @sa AEREnvConfGetInt
 */
size_t AEREnvConfGetInts(
		const char * name,
		size_t bufSize,
		int32_t * intBuf
);

/**
 * @brief Parse a float value from an environment variable.
 *
 * @param[in] name Name of environment variable.
 *
 * @return Value of environment variable interpreted as a float
 * or `0.0f` if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `name` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `name` is an invalid environment
 * variable.
 * @throw ::AER_FAILED_PARSE if environment variable could not be
 * interpreted as a float.
 *
 * @since 1.0.0
 *
 * @sa AEREnvConfGetFloats
 */
float AEREnvConfGetFloat(const char * name);

/**
 * @brief Parse multiple whitespace-delimited float values from an
 * environment variable.
 *
 * @warning Argument `floatBuf` must be large enough to hold at least
 * `bufSize` elements.
 *
 * @note Argument `bufSize` may be `0` in which case argument `floatBuf` may
 * be `NULL`. This may be used to efficiently query the total number of
 * potential floats contained in an environment variable.
 *
 * @param[in] name Name of environment variable.
 * @param[in] bufSize Maximum number of elements to write to argument
 * `floatBuf`.
 * @param[out] floatBuf Buffer to write floats to.
 *
 * @return Total number of floats contained in environment variable or `0`
 * if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `name` is `NULL` or argument `floatBuf`
 * is `NULL` and argument `bufSize` is greater than `0`.
 * @throw ::AER_FAILED_LOOKUP if argument `name` is an invalid environment
 * variable.
 * @throw ::AER_FAILED_PARSE if environment variable could not be
 * interpreted as floats.
 *
 * @since 1.0.0
 *
 * @sa AEREnvConfGetFloat
 */
size_t AEREnvConfGetFloats(
		const char * name,
		size_t bufSize,
		float * floatBuf
);

/**
 * @brief Read a string from an environment variable.
 *
 * @param[in] name Name of environment variable.
 *
 * @return Content of environment variable or `NULL` if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `name` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `name` is an invalid environment
 * variable.
 *
 * @since 1.0.0
 *
 * @sa AEREnvConfGetStrings
 */
const char * AEREnvConfGetString(const char * name);

/**
 * @brief Read multiple whitespace-delimited strings from an
 * environment variable.
 *
 * @warning Argument `strBuf` must be large enough to hold at least
 * `bufSize` elements.
 *
 * @note Argument `bufSize` may be `0` in which case argument `strBuf` may
 * be `NULL`. This may be used to efficiently query the total number of
 * potential strings contained in an environment variable.
 *
 * @param[in] name Name of environment variable.
 * @param[in] bufSize Maximum number of elements to write to argument
 * `strBuf`.
 * @param[out] strBuf Buffer to write strings to.
 *
 * @return Total number of strings contained in environment variable or `0`
 * if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `name` is `NULL` or argument `strBuf`
 * is `NULL` and argument `bufSize` is greater than `0`.
 * @throw ::AER_FAILED_LOOKUP if argument `name` is an invalid environment
 * variable.
 *
 * @since 1.0.0
 *
 * @sa AEREnvConfGetString
 */
size_t AEREnvConfGetStrings(
		const char * name,
		size_t bufSize,
		const char ** strBuf
);



#endif /* AER_ENVCONF_H */
