#include <time.h>

#include "foxutils/rand.h"
#include "foxutils/xoshiro256ss.h"

#include "aer/rand.h"
#include "internal/err.h"
#include "internal/rand.h"



/* ----- PRIVATE GLOBALS ----- */

static FoxXoshiro256SS randPRNG = {0};



/* ----- INTERNAL FUNCTIONS ----- */

void RandConstructor(void) {
	FoxXoshiro256SSInit(&randPRNG, time(NULL));

	return;
}

void RandDestructor(void) {
	FoxXoshiro256SSDeinit(&randPRNG);

	return;
}



/* ------ PUBLIC FUNCTIONS ----- */

uint64_t AERRandUInt(void) {
	return FoxRandUInt((FoxPRNG *)&randPRNG);
}

uint64_t AERRandUIntRange(uint64_t min, uint64_t max) {
	ErrIf(min >= max, AER_BAD_VAL, 0);

	return FoxRandUIntRange((FoxPRNG *)&randPRNG, min, max);
}

int64_t AERRandInt(void) {
	return FoxRandInt((FoxPRNG *)&randPRNG);
}

int64_t AERRandIntRange(int64_t min, int64_t max) {
	ErrIf(min >= max, AER_BAD_VAL, 0);

	return FoxRandIntRange((FoxPRNG *)&randPRNG, min, max);
}

float AERRandFloat(void) {
	return FoxRandFloat((FoxPRNG *)&randPRNG);
}

float AERRandFloatRange(float min, float max) {
	ErrIf(min >= max, AER_BAD_VAL, 0.0f);

	return FoxRandFloatRange((FoxPRNG *)&randPRNG, min, max);
}

double AERRandDouble(void) {
	return FoxRandDouble((FoxPRNG *)&randPRNG);
}

double AERRandDoubleRange(double min, double max) {
	ErrIf(min >= max, AER_BAD_VAL, 0.0);

	return FoxRandDoubleRange((FoxPRNG *)&randPRNG, min, max);
}

bool AERRandBool(void) {
	return FoxRandBool((FoxPRNG *)&randPRNG);
}
