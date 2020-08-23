#ifndef INTERNAL_EVENTTRAP_H
#define INTERNAL_EVENTTRAP_H

#include "foxutils/array.h"

#include "internal/hld.h"



/* ----- INTERNAL TYPES ----- */

typedef struct EventTrap {
	FoxArray upstreamListeners;
	FoxArray downstreamListeners;
	__attribute__((cdecl)) void (* origListener)(
			HLDInstance * target,
			HLDInstance * other
	);
	HLDEventType eventType;
} EventTrap;



/* ----- INTERNAL FUNCTIONS ----- */

void EventTrapInit(
		EventTrap * trap,
		HLDEventType eventType,
		__attribute__((cdecl)) void (* origListener)(
			HLDInstance * target,
			HLDInstance * other
		)
);

void EventTrapDeinit(EventTrap * trap);

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



#endif /* INTERNAL_EVENTTRAP_H */
