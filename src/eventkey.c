#include "foxutils/hash.h"

#include "internal/eventkey.h"



/* ----- INTERNAL FUNCTIONS ----- */

uint32_t EventKeyHash(const EventKey * key) {
	uint32_t mem[] = {
		key->type,
		key->num,
		key->objIdx
	};

	return FoxHashMem(mem, sizeof(mem));
}

int32_t EventKeyCompare(const EventKey * keyA, const EventKey * keyB) {
	return !(
			keyA->type == keyB->type
			&& keyA->num == keyB->num
			&& keyA->objIdx == keyB->objIdx
	);
}
