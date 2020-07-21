#ifndef EVENTKEY_H
#define EVENTKEY_H

#include <stdbool.h>
#include <stdint.h>
#include "hld.h"



/* ----- PUBLIC TYPES ----- */

typedef struct EventKey {
	HLDEventType type;
	int32_t num;
	int32_t objIdx;
} EventKey;



/* ----- PUBLIC FUNCTIONS ----- */

uint32_t EventKeyHash(void * key);

bool EventKeyEqual(void * key, void * other);



#endif /* EVENTKEY_H */
