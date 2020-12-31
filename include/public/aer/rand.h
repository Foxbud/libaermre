/**
 * @file
 *
 * @brief Automatically seeded functions for generating pseudorandom numbers.
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
#ifndef AER_RAND_H
#define AER_RAND_H

#include <stdbool.h>
#include <stdint.h>



/* ----- PUBLIC FUNCTIONS ----- */

/**
 * @brief Get a pseudorandom unsigned integer on the interval [0, 2^64).
 *
 * @return Pseudorandom unsigned integer.
 *
 * @since 1.0.0
 */
uint64_t AERRandUInt(void);

/**
 * @brief Get a pseudorandom unsigned integer on the interval [min, max).
 *
 * This function has been carefully designed to avoid introducing any
 * distribution-related bias. For faster but potentially biased generation,
 * use modulo.
 *
 * @param[in] min Minimum possible value (inclusive).
 * @param[in] max Maximum possible value (exclusive).
 *
 * @return Pseudorandom unsigned integer.
 *
 * @throw ::AER_BAD_VAL if argument `min` is greater than or equal to
 * argument `max`.
 *
 * @since 1.0.0
 */
uint64_t AERRandUIntRange(uint64_t min, uint64_t max);

/**
 * @brief Get a pseudorandom signed integer on the interval [-2^32, 2^32).
 *
 * @return Pseudorandom signed integer.
 *
 * @since 1.0.0
 */
int64_t AERRandInt(void);

/**
 * @brief Get a pseudorandom signed integer on the interval [min, max).
 *
 * This function has been carefully designed to avoid introducing any
 * distribution-related bias. For faster but potentially biased generation,
 * use modulo.
 *
 * @param[in] min Minimum possible value (inclusive).
 * @param[in] max Maximum possible value (exclusive).
 *
 * @return Pseudorandom signed integer.
 *
 * @throw ::AER_BAD_VAL if argument `min` is greater than or equal to
 * argument `max`.
 *
 * @since 1.0.0
 */
int64_t AERRandIntRange(int64_t min, int64_t max);

/**
 * @brief Get a pseudorandom floating-point value on the interval [0.0f, 1.0f).
 *
 * @return pseudorandom floating-point value
 *
 * @since 1.0.0
 */
float AERRandFloat(void);

/**
 * @brief Get a pseudorandom floating-point value on the interval [min, max).
 *
 * @param[in] min Minimum possible value (inclusive).
 * @param[in] max Maximum possible value (exclusive).
 *
 * @return Pseudorandom floating-point value.
 *
 * @throw ::AER_BAD_VAL if argument `min` is greater than or equal to
 * argument `max`.
 *
 * @since 1.0.0
 */
float AERRandFloatRange(float min, float max);

/**
 * @brief Get a pseudorandom double floating-point value on the interval [0.0, 1.0).
 *
 * @return Pseudorandom double floating-point value.
 *
 * @since 1.0.0
 */
double AERRandDouble(void);

/**
 * @brief Get a pseudorandom double floating-point value on the interval [min, max).
 *
 * @param[in] min Minimum possible value (inclusive).
 * @param[in] max Maximum possible value (exclusive).
 *
 * @return pseudorandom double floating-point value
 *
 * @throw ::AER_BAD_VAL if argument `min` is greater than or equal to
 * argument `max`.
 *
 * @since 1.0.0
 */
double AERRandDoubleRange(double min, double max);

/**
 * @brief Get a pseudorandom boolean.
 *
 * @return Pseudorandom boolean.
 *
 * @since 1.0.0
 */
bool AERRandBool(void);



#endif /* AER_RAND_H */
