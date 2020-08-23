#ifndef PRIVATE_EVENTKEY_H
#define PRIVATE_EVENTKEY_H

#include <stdint.h>

#include "private/hld.h"



/* ----- PUBLIC TYPES ----- */

typedef struct EventKey {
	HLDEventType type;
	int32_t num;
	int32_t objIdx;
} EventKey;



/* ----- PUBLIC FUNCTIONS ----- */

uint32_t EventKeyHash(const EventKey * key);

int32_t EventKeyCompare(const EventKey * keyA, const EventKey * keyB);



#endif /* PRIVATE_EVENTKEY_H */
