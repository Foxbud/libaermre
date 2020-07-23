#ifndef EVENTTRAP_H
#define EVENTTRAP_H

#include "dynarr.h"
#include "hld.h"



/* ----- PUBLIC TYPES ----- */

typedef struct EventTrap {
	uint8_t rawData[4 * 4];
} EventTrap;



/* ----- PUBLIC FUNCTIONS ----- */

EventTrap * EventTrapNew(
		HLDEventType eventType,
		__attribute__((cdecl)) void (* origListener)(
			HLDInstance * target,
			HLDInstance * other
		)
);

void EventTrapFree(EventTrap * trap);

void EventTrapAddUpstream(
		EventTrap * trap,
		void * listener
);

void EventTrapAddDownstream(
		EventTrap * trap,
		void * listener
);

void EventTrapExecute(
		EventTrap * trap,
		HLDInstance * target,
		HLDInstance * other
);



#endif /* EVENTTRAP_H */
