/**
 * @file
 */
#ifndef AER_RAND_H
#define AER_RAND_H

#include <stdbool.h>
#include <stdint.h>



/**
 * @defgroup rand Random
 *
 * This module contains all pseudorandom number generation utilities.
 *
 * @sa aer/rand.h
 *
 * @{
 */

/**
 * Get a pseudorandom unsigned integer on the interval [0, 2^64).
 *
 * @return pseudorandom unsigned integer
 */
uint64_t AERRandUInt(void);

/**
 * Get a pseudorandom unsigned integer on the interval [min, max).
 *
 * This function has been carefully designed to avoid introducing any
 * distribution-related bias. For faster but potentially biased generation,
 * use modulo.
 *
 * @param[in] min minimum possible value (inclusive)
 * @param[in] max maximum possible value (exclusive)
 *
 * @return pseudorandom unsigned integer
 */
uint64_t AERRandUIntRange(uint64_t min, uint64_t max);

/**
 * Get a pseudorandom signed integer on the interval [-2^32, 2^32).
 *
 * @return pseudorandom signed integer
 */
int64_t AERRandInt(void);

/**
 * Get a pseudorandom signed integer on the interval [min, max).
 *
 * This function has been carefully designed to avoid introducing any
 * distribution-related bias. For faster but potentially biased generation,
 * use modulo.
 *
 * @param[in] min minimum possible value (inclusive)
 * @param[in] max maximum possible value (exclusive)
 *
 * @return pseudorandom signed integer
 */
int64_t AERRandIntRange(int64_t min, int64_t max);

/**
 * Get a pseudorandom floating-point value on the interval [0.0f, 1.0f).
 *
 * @return pseudorandom floating-point value
 */
float AERRandFloat(void);

/**
 * Get a pseudorandom floating-point value on the interval [min, max).
 *
 * @param[in] min minimum possible value (inclusive)
 * @param[in] max maximum possible value (exclusive)
 *
 * @return pseudorandom floating-point value
 */
float AERRandFloatRange(float min, float max);

/**
 * Get a pseudorandom double floating-point value on the interval [0.0, 1.0).
 *
 * @return pseudorandom double floating-point value
 */
double AERRandDouble(void);

/**
 * Get a pseudorandom double floating-point value on the interval [min, max).
 *
 * @param[in] min minimum possible value (inclusive)
 * @param[in] max maximum possible value (exclusive)
 *
 * @return pseudorandom double floating-point value
 */
double AERRandDoubleRange(double min, double max);

/**
 * Get a pseudorandom boolean.
 *
 * @return pseudorandom boolean
 */
bool AERRandBool(void);

/**
 * @}
 */



#endif /* AER_RAND_H */
