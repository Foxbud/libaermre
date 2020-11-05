#ifndef INTERNAL_EVENTKEY_H
#define INTERNAL_EVENTKEY_H

#include <stdint.h>

#include "internal/hld.h"



/* ----- INTERNAL TYPES ----- */

typedef struct EventKey {
	HLDEventType type;
	int32_t num;
	int32_t objIdx;
} EventKey;



/* ----- INTERNAL FUNCTIONS ----- */

uint32_t EventKeyHash(const EventKey * key);

int32_t EventKeyCompare(const EventKey * keyA, const EventKey * keyB);



#endif /* INTERNAL_EVENTKEY_H */
