/**
 * @file
 *
 * @brief Utilities for generating pseudorandom numbers.
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
#ifndef AER_RAND_H
#define AER_RAND_H

#include <stdbool.h>
#include <stdint.h>

/* ----- PUBLIC TYPES ----- */

/**
 * @brief Opaque type for a self-managed pseudorandom number generator.
 *
 * Use this with the functions prefixed with `AERRandGen...`
 *
 * @since 1.0.0
 */
typedef void AERRandGen;

/* ----- PUBLIC FUNCTIONS ----- */

/**
 * @brief Get a pseudorandom unsigned integer on the interval [0, 2^64) using
 * the automatically-seeded global generator.
 *
 * @return Pseudorandom unsigned integer.
 *
 * @since 1.0.0
 *
 * @sa AERRandGenUInt
 */
uint64_t AERRandUInt(void);

/**
 * @brief Get a pseudorandom unsigned integer on the interval [min, max) using
 * the automatically-seeded global generator.
 *
 * This function has been carefully designed to avoid introducing any
 * distribution-related bias. For faster but potentially biased generation,
 * use modulo.
 *
 * @param[in] min Minimum possible value (inclusive).
 * @param[in] max Maximum possible value (exclusive).
 *
 * @return Pseudorandom unsigned integer or `0` if unsuccessful.
 *
 * @throw ::AER_BAD_VAL if argument `min` is greater than or equal to
 * argument `max`.
 *
 * @since 1.0.0
 *
 * @sa AERRandGenUIntRange
 */
uint64_t AERRandUIntRange(uint64_t min, uint64_t max);

/**
 * @brief Get a pseudorandom signed integer on the interval [-2^63, 2^63) using
 * the automatically-seeded global generator.
 *
 * @return Pseudorandom signed integer.
 *
 * @since 1.0.0
 *
 * @sa AERRandGenInt
 */
int64_t AERRandInt(void);

/**
 * @brief Get a pseudorandom signed integer on the interval [min, max) using the
 * automatically-seeded global generator.
 *
 * This function has been carefully designed to avoid introducing any
 * distribution-related bias. For faster but potentially biased generation,
 * use modulo.
 *
 * @param[in] min Minimum possible value (inclusive).
 * @param[in] max Maximum possible value (exclusive).
 *
 * @return Pseudorandom signed integer or `0` if unsuccessful.
 *
 * @throw ::AER_BAD_VAL if argument `min` is greater than or equal to
 * argument `max`.
 *
 * @since 1.0.0
 *
 * @sa AERRandGenIntRange
 */
int64_t AERRandIntRange(int64_t min, int64_t max);

/**
 * @brief Get a pseudorandom floating-point value on the interval [0.0f, 1.0f)
 * using the automatically-seeded global generator.
 *
 * @bug This function uses a method of obtaining floats from integers that is
 * now known to introduce slight distribution-related bias (see <a
 * href="https://hal.archives-ouvertes.fr/hal-02427338/file/fpnglib_iccs.pdf">
 * *Generating Random Floating-Point Numbers by Dividing Integers: a Case Study*
 * by Frédéric Goualard</a>).
 * This is unlikely to cause issues in the vast majority of usecases, but it
 * should be kept in mind.
 *
 * @return Pseudorandom floating-point value.
 *
 * @since 1.0.0
 *
 * @sa AERRandGenFloat
 */
float AERRandFloat(void);

/**
 * @brief Get a pseudorandom floating-point value on the interval [min, max)
 * using the automatically-seeded global generator.
 *
 * @bug This function uses a method of obtaining floats from integers that is
 * now known to introduce slight distribution-related bias (see <a
 * href="https://hal.archives-ouvertes.fr/hal-02427338/file/fpnglib_iccs.pdf">
 * *Generating Random Floating-Point Numbers by Dividing Integers: a Case Study*
 * by Frédéric Goualard</a>).
 * This is unlikely to cause issues in the vast majority of usecases, but it
 * should be kept in mind.
 *
 * @param[in] min Minimum possible value (inclusive).
 * @param[in] max Maximum possible value (exclusive).
 *
 * @return Pseudorandom floating-point value or `0.0f` if unsuccessful.
 *
 * @throw ::AER_BAD_VAL if argument `min` is greater than or equal to
 * argument `max`.
 *
 * @since 1.0.0
 *
 * @sa AERRandGenFloatRange
 */
float AERRandFloatRange(float min, float max);

/**
 * @brief Get a pseudorandom double floating-point value on the interval
 * [0.0, 1.0) using the automatically-seeded global generator.
 *
 * @bug This function uses a method of obtaining floats from integers that is
 * now known to introduce slight distribution-related bias (see <a
 * href="https://hal.archives-ouvertes.fr/hal-02427338/file/fpnglib_iccs.pdf">
 * *Generating Random Floating-Point Numbers by Dividing Integers: a Case Study*
 * by Frédéric Goualard</a>).
 * This is unlikely to cause issues in the vast majority of usecases, but it
 * should be kept in mind.
 *
 * @return Pseudorandom double floating-point value.
 *
 * @since 1.0.0
 *
 * @sa AERRandGenDouble
 */
double AERRandDouble(void);

/**
 * @brief Get a pseudorandom double floating-point value on the interval
 * [min, max) using the automatically-seeded global generator.
 *
 * @bug This function uses a method of obtaining floats from integers that is
 * now known to introduce slight distribution-related bias (see <a
 * href="https://hal.archives-ouvertes.fr/hal-02427338/file/fpnglib_iccs.pdf">
 * *Generating Random Floating-Point Numbers by Dividing Integers: a Case Study*
 * by Frédéric Goualard</a>).
 * This is unlikely to cause issues in the vast majority of usecases, but it
 * should be kept in mind.
 *
 * @param[in] min Minimum possible value (inclusive).
 * @param[in] max Maximum possible value (exclusive).
 *
 * @return Pseudorandom double floating-point value or `0.0` if unsuccessful.
 *
 * @throw ::AER_BAD_VAL if argument `min` is greater than or equal to
 * argument `max`.
 *
 * @since 1.0.0
 *
 * @sa AERRandGenDoubleRange
 */
double AERRandDoubleRange(double min, double max);

/**
 * @brief Get a pseudorandom boolean using the automatically-seeded global
 * generator.
 *
 * @return Pseudorandom boolean.
 *
 * @since 1.0.0
 *
 * @sa AERRandGenBool
 */
bool AERRandBool(void);

/**
 * @brief Allocate and initialize a new self-managed pseudorandom number
 * generator.
 *
 * When no longer needed, free this generator using AERRandGenFree.
 *
 * @param[in] seed Initial generator seed.
 *
 * @return Newly allocated generator.
 *
 * @since 1.0.0
 *
 * @sa AERRandGenFree
 */
AERRandGen *AERRandGenNew(uint64_t seed);

/**
 * @brief Free a self-managed pseudorandom number generator allocated using
 * AERRandGenNew.
 *
 * @param[in] gen Generator of interest.
 *
 * @throw ::AER_NULL_ARG if argument `gen` is `NULL`.
 *
 * @since 1.0.0
 *
 * @sa AERRandGenNew
 */
void AERRandGenFree(AERRandGen *gen);

/**
 * @brief Re-seed a self-managed pseudorandom number generator.
 *
 * @param[in] gen Generator of interest.
 * @param[in] seed New generator seed.
 *
 * @throw ::AER_NULL_ARG if argument `gen` is `NULL`.
 *
 * @since 1.0.0
 */
void AERRandGenSeed(AERRandGen *gen, uint64_t seed);

/**
 * @brief Get a pseudorandom unsigned integer on the interval [0, 2^64) using
 * a self-managed generator.
 *
 * @param[in] gen Generator of interest.
 *
 * @return Pseudorandom unsigned integer or `0` if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `gen` is `NULL`.
 *
 * @since 1.0.0
 *
 * @sa AERRandUInt
 */
uint64_t AERRandGenUInt(AERRandGen *gen);

/**
 * @brief Get a pseudorandom unsigned integer on the interval [min, max) using
 * a self-managed generator.
 *
 * This function has been carefully designed to avoid introducing any
 * distribution-related bias. For faster but potentially biased generation,
 * use modulo.
 *
 * @param[in] gen Generator of interest.
 * @param[in] min Minimum possible value (inclusive).
 * @param[in] max Maximum possible value (exclusive).
 *
 * @return Pseudorandom unsigned integer or `0` if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `gen` is `NULL`.
 * @throw ::AER_BAD_VAL if argument `min` is greater than or equal to
 * argument `max`.
 *
 * @since 1.0.0
 *
 * @sa AERRandUIntRange
 */
uint64_t AERRandGenUIntRange(AERRandGen *gen, uint64_t min, uint64_t max);

/**
 * @brief Get a pseudorandom signed integer on the interval [-2^63, 2^63) using
 * a self-managed generator.
 *
 * @param[in] gen Generator of interest.
 *
 * @return Pseudorandom signed integer or `0` if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `gen` is `NULL`.
 *
 * @since 1.0.0
 *
 * @sa AERRandInt
 */
int64_t AERRandGenInt(AERRandGen *gen);

/**
 * @brief Get a pseudorandom signed integer on the interval [min, max) using a
 *  self-managed generator.
 *
 * This function has been carefully designed to avoid introducing any
 * distribution-related bias. For faster but potentially biased generation,
 * use modulo.
 *
 * @param[in] gen Generator of interest.
 * @param[in] min Minimum possible value (inclusive).
 * @param[in] max Maximum possible value (exclusive).
 *
 * @return Pseudorandom signed integer or `0` if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `gen` is `NULL`.
 * @throw ::AER_BAD_VAL if argument `min` is greater than or equal to
 * argument `max`.
 *
 * @since 1.0.0
 *
 * @sa AERRandIntRange
 */
int64_t AERRandGenIntRange(AERRandGen *gen, int64_t min, int64_t max);

/**
 * @brief Get a pseudorandom floating-point value on the interval [0.0f, 1.0f)
 * using a self-managed generator.
 *
 * @bug This function uses a method of obtaining floats from integers that is
 * now known to introduce slight distribution-related bias (see <a
 * href="https://hal.archives-ouvertes.fr/hal-02427338/file/fpnglib_iccs.pdf">
 * *Generating Random Floating-Point Numbers by Dividing Integers: a Case Study*
 * by Frédéric Goualard</a>).
 * This is unlikely to cause issues in the vast majority of usecases, but it
 * should be kept in mind.
 *
 * @param[in] gen Generator of interest.
 *
 * @return Pseudorandom floating-point value or `0.0f` if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `gen` is `NULL`.
 *
 * @since 1.0.0
 *
 * @sa AERRandFloat
 */
float AERRandGenFloat(AERRandGen *gen);

/**
 * @brief Get a pseudorandom floating-point value on the interval [min, max)
 * using a self-managed generator.
 *
 * @bug This function uses a method of obtaining floats from integers that is
 * now known to introduce slight distribution-related bias (see <a
 * href="https://hal.archives-ouvertes.fr/hal-02427338/file/fpnglib_iccs.pdf">
 * *Generating Random Floating-Point Numbers by Dividing Integers: a Case Study*
 * by Frédéric Goualard</a>).
 * This is unlikely to cause issues in the vast majority of usecases, but it
 * should be kept in mind.
 *
 * @param[in] gen Generator of interest.
 * @param[in] min Minimum possible value (inclusive).
 * @param[in] max Maximum possible value (exclusive).
 *
 * @return Pseudorandom floating-point value or `0.0f` if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `gen` is `NULL`.
 * @throw ::AER_BAD_VAL if argument `min` is greater than or equal to
 * argument `max`.
 *
 * @since 1.0.0
 *
 * @sa AERRandFloatRange
 */
float AERRandGenFloatRange(AERRandGen *gen, float min, float max);

/**
 * @brief Get a pseudorandom double floating-point value on the interval
 * [0.0, 1.0) using a self-managed generator.
 *
 * @bug This function uses a method of obtaining floats from integers that is
 * now known to introduce slight distribution-related bias (see <a
 * href="https://hal.archives-ouvertes.fr/hal-02427338/file/fpnglib_iccs.pdf">
 * *Generating Random Floating-Point Numbers by Dividing Integers: a Case Study*
 * by Frédéric Goualard</a>).
 * This is unlikely to cause issues in the vast majority of usecases, but it
 * should be kept in mind.
 *
 * @param[in] gen Generator of interest.
 *
 * @return Pseudorandom double floating-point value or `0.0` if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `gen` is `NULL`.
 *
 * @since 1.0.0
 *
 * @sa AERRandDouble
 */
double AERRandGenDouble(AERRandGen *gen);

/**
 * @brief Get a pseudorandom double floating-point value on the interval
 * [min, max) using a self-managed generator.
 *
 * @bug This function uses a method of obtaining floats from integers that is
 * now known to introduce slight distribution-related bias (see <a
 * href="https://hal.archives-ouvertes.fr/hal-02427338/file/fpnglib_iccs.pdf">
 * *Generating Random Floating-Point Numbers by Dividing Integers: a Case Study*
 * by Frédéric Goualard</a>).
 * This is unlikely to cause issues in the vast majority of usecases, but it
 * should be kept in mind.
 *
 * @param[in] gen Generator of interest.
 * @param[in] min Minimum possible value (inclusive).
 * @param[in] max Maximum possible value (exclusive).
 *
 * @return Pseudorandom double floating-point value or `0.0` if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `gen` is `NULL`.
 * @throw ::AER_BAD_VAL if argument `min` is greater than or equal to
 * argument `max`.
 *
 * @since 1.0.0
 *
 * @sa AERRandDoubleRange
 */
double AERRandGenDoubleRange(AERRandGen *gen, double min, double max);

/**
 * @brief Get a pseudorandom boolean using a self-managed generator.
 *
 * @param[in] gen Generator of interest.
 *
 * @return Pseudorandom boolean or `false` if unsuccessful.
 *
 * @throw ::AER_NULL_ARG if argument `gen` is `NULL`.
 *
 * @since 1.0.0
 *
 * @sa AERRandBool
 */
bool AERRandGenBool(AERRandGen *gen);

#endif /* AER_RAND_H */
