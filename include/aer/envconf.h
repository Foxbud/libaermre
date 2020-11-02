/**
 * @file
 *
 * @brief Utilities for parsing configuration data from environment
 * variables.
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
 * For information about how this function parses boolean values, see
 * @ref ParsingBools.
 *
 * @param[in] name Name of environment variable.
 *
 * @return Value of environment variable interpreted as a boolean
 * or `false` if error.
 *
 * @throw ::AER_NULL_ARG if argument `name` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `name` is invalid environment
 * variable.
 * @throw ::AER_FAILED_PARSE if environment variable could not be
 * interpreted as a boolean.
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
 * @return Total number of booleans contained in environment variable.
 *
 * @throw ::AER_NULL_ARG if argument `name` is `NULL` or argument `boolBuf`
 * is `NULL` and argument `bufSize` is greater than `0`.
 * @throw ::AER_FAILED_LOOKUP if argument `name` is invalid environment
 * variable.
 * @throw ::AER_FAILED_PARSE if environment variable could not be
 * interpreted as booleans.
 *
 * @sa AEREnvConfGetBool
 */
size_t AEREnvConfGetBools(
		const char * name,
		size_t bufSize,
		bool * boolBuf
);

int32_t AEREnvConfGetInt(const char * name);

size_t AEREnvConfGetInts(
		const char * name,
		size_t bufSize,
		int32_t * intBuf
);

float AEREnvConfGetFloat(const char * name);

size_t AEREnvConfGetFloats(
		const char * name,
		size_t bufSize,
		float * floatBuf
);

const char * AEREnvConfGetString(const char * name);

size_t AEREnvConfGetStrings(
		const char * name,
		size_t bufSize,
		const char ** strBuf
);



#endif /* AER_ENVCONF_H */
