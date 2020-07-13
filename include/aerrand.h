/**
 * @file
 *
 * @brief This module contains pseudorandom number generation functions.
 *
 * Mods that require random number generation should prefer this module over
 * stdlib's "rand." The Game Maker engine appears to seed the stdlib's "rand"
 * with a constant value, limiting it's usefulness in practice.
 *
 * Not only does this module automatically seed itself using the current time,
 * but it also provides useful functions.
 */
#ifndef AERRAND_H
#define AERRAND_H

#include <stdbool.h>
#include <stdint.h>



/* ----- PUBLIC FUNCTIONS ----- */

/**
 * Get a pseudorandom unsigned integer on the interval [0, ULONG_MAX].
 *
 * @return pseudorandom unsigned integer
 */
uint32_t AERRandUInt(void);

/**
 * Get a pseudorandom unsigned integer on the interval [min, max].
 *
 * This function has been carefully designed to avoid introducing any
 * distribution-related bias. For faster but potentially biased generation,
 * use modulo.
 *
 * @param[in] min minimum possible value (inclusive)
 * @param[in] max maximum possible value (inclusive)
 *
 * @return pseudorandom unsigned integer
 */
uint32_t AERRandUIntBetween(uint32_t min, uint32_t max);

/**
 * Get a pseudorandom signed integer on the interval [LONG_MIN, LONG_MAX].
 *
 * @return pseudorandom signed integer
 */
int32_t AERRandInt(void);


/**
 * Get a pseudorandom signed integer on the interval [min, max].
 *
 * This function has been carefully designed to avoid introducing any
 * distribution-related bias. For faster but potentially biased generation,
 * use modulo.
 *
 * @param[in] min minimum possible value (inclusive)
 * @param[in] max maximum possible value (inclusive)
 *
 * @return pseudorandom signed integer
 */
int32_t AERRandIntBetween(int32_t min, int32_t max);

/**
 * Get a pseudorandom floating-point value on the interval [0.0f, 1.0f).
 *
 * Note that 1.0f is an exclusive upper bound.
 *
 * @return pseudorandom floating-point value
 */
float AERRandFloat(void);

/**
 * Get a pseudorandom boolean.
 *
 * @return pseudorandom boolean
 */
bool AERRandBool(void);



#endif /* AERRAND_H */
