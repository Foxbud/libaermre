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

uint32_t AERRandUInt(void);

uint32_t AERRandUIntBetween(uint32_t min, uint32_t max);

int32_t AERRandInt(void);

int32_t AERRandIntBetween(int32_t min, int32_t max);

float AERRandFloat(void);

bool AERRandBool(void);



#endif /* AERRAND_H */
