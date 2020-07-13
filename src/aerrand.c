#include <assert.h>
#include <time.h>
#include "aerrand.h"



/* ----- PRIVATE CONSTANTS ----- */

/*
 * These are the default parameters recommended by Vigna in "An experimental
 * exploration of Marsaglia's xorshift generators, scrambled."
 */

static const uint8_t XS64_STAR_PARAM_A = 12;

static const uint8_t XS64_STAR_PARAM_B = 25;

static const uint8_t XS64_STAR_PARAM_C = 17;

static const uint64_t XS64_STAR_COEF = 0x2545f4914f6cdd1d;



/* ----- PRIVATE GLOBALS ----- */

static uint64_t xs64StarState = 0;



/* ----- PRIVATE FUNCTIONS ----- */

static void XS64StarSeed(void) {
	xs64StarState = time(NULL);
}

static uint32_t XS64StarNext(void) {
	if (!xs64StarState) {
		XS64StarSeed();
	}

	uint64_t state = xs64StarState;
	state ^= state << XS64_STAR_PARAM_A;
	state ^= state >> XS64_STAR_PARAM_B;
	state ^= state << XS64_STAR_PARAM_C;
	xs64StarState = state;

	/* Upper 32 bits are stronger than the lower 32. */
	return (uint32_t)((state * XS64_STAR_COEF) >> 32);
}

static uint32_t MaskPower2(uint32_t val) {
	uint32_t result = val;
	result |= result >> 1;
	result |= result >> 2;
	result |= result >> 4;
	result |= result >> 8;
	result |= result >> 16;

	return result;
}



/* ------ PUBLIC FUNCTIONS ----- */

uint32_t AERRandUInt(void) {
	return XS64StarNext();
}

uint32_t AERRandUIntBetween(uint32_t min, uint32_t max) {
	assert(max > min);

	uint32_t range = max - min;
	uint32_t mask = MaskPower2(range);
	uint32_t result;
	do {
		result = XS64StarNext() & mask;
	} while (result > range);

	return result + min;
}

int32_t AERRandInt(void) {
	return (int32_t)XS64StarNext();
}

int32_t AERRandIntBetween(int32_t min, int32_t max) {
	assert(max > min);

	uint32_t range = (uint32_t)(max - min);
	uint32_t result = AERRandUIntBetween(0, range);

	return (int32_t)(result + (uint32_t)min);
}

float AERRandFloat(void) {
	union {
		uint32_t u;
		float f;
	}	result;
	result.f = 1.0f;
	result.u |= XS64StarNext() & 0x007fffff;

	return result.f - 1.0f;
}

bool AERRandBool(void) {
	return XS64StarNext() & 0x1;
}
