#ifndef EVENTKEY_H
#define EVENTKEY_H

#include <stdint.h>

#include "hld.h"



/* ----- PUBLIC TYPES ----- */

typedef struct EventKey {
	HLDEventType type;
	int32_t num;
	int32_t objIdx;
} EventKey;



/* ----- PUBLIC FUNCTIONS ----- */

uint32_t EventKeyHash(const EventKey * key);

int32_t EventKeyCompare(const EventKey * keyA, const EventKey * keyB);



#endif /* EVENTKEY_H */
