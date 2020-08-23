#include <assert.h>
#include <time.h>

#include "aer/rand.h"
#include "private/xorshift.h"



/* ----- PRIVATE GLOBALS ----- */

static uint64_t prngState = 0;



/* ----- PRIVATE FUNCTIONS ----- */

static void PRNGSeed(void) {
	prngState = time(NULL);
}

static uint32_t PRNGNext(void) {
	if (!prngState) {
		PRNGSeed();
	}

	uint64_t state = prngState;
	XS64Round(state);
	prngState = state;

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
	return PRNGNext();
}

uint32_t AERRandUIntBetween(uint32_t min, uint32_t max) {
	assert(max > min);

	uint32_t range = max - min;
	uint32_t mask = MaskPower2(range);
	uint32_t result;
	do {
		result = PRNGNext() & mask;
	} while (result > range);

	return result + min;
}

int32_t AERRandInt(void) {
	return (int32_t)PRNGNext();
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
	result.u |= PRNGNext() & 0x007fffff;

	return result.f - 1.0f;
}

bool AERRandBool(void) {
	return PRNGNext() & 0x1;
}
