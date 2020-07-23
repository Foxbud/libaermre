#include "eventkey.h"



/* ----- PUBLIC FUNCTIONS ----- */

uint32_t EventKeyHash(void * key) {
	union {
		uint32_t raw;
		struct {
			uint32_t type : 4;
			uint32_t num : 14;
			uint32_t objIdx : 14;
		} parts;
	} hash;
	
	/* Initialize hash. */
	EventKey keyCopy = *((EventKey *)key);
	hash.parts.type = keyCopy.type & 0xf;
	hash.parts.num = (uint32_t)keyCopy.num & 0x3fff;
	hash.parts.objIdx = (uint32_t)keyCopy.objIdx & 0x3fff;

	/* 
	 * Perform three rounds of xorshift32 with parameters recommended by
	 * Marsaglia in "Xorshift RNGs."
	 */
	for (uint32_t idx = 0; idx < 3; idx++) {
		hash.raw ^= hash.raw << 13;
		hash.raw ^= hash.raw >> 17;
		hash.raw ^= hash.raw << 5;
	}

	return hash.raw;
}

bool EventKeyEqual(void * key, void * other) {
	for (uint32_t idx = 0; idx < sizeof(EventKey) / 4; idx++) {
		if (((uint32_t *)key)[idx] != ((uint32_t *)other)[idx]) {
			return false;
		}
	}

	return true;
}
