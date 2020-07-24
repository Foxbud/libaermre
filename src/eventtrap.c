#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "eventtrap.h"



/* ----- PRIVATE TYPES ----- */

typedef struct Trap {
	HLDEventType eventType;
	__attribute__((cdecl)) void (* origListener)(
			HLDInstance * target,
			HLDInstance * other
	);
	DynArr * upstreamListeners;
	DynArr * downstreamListeners;
} Trap;



/* ----- PRIVATE FUNCTIONS ----- */

static bool ExecuteListeners(
		DynArr * listeners,
		HLDEventType eventType,
		HLDInstance * target,
		HLDInstance * other,
		bool reverse
) {
	bool doNext;

	size_t numListeners = DynArrSize(listeners);
	uint32_t lastIdx = numListeners - 1;
	switch (eventType) {
		case HLD_EVENT_CREATE:
		case HLD_EVENT_DESTROY:
		case HLD_EVENT_OTHER:
			for (uint32_t idx = 0; idx < numListeners; idx++) {
				bool (* listener)(HLDInstance *);
				listener = DynArrGet(listeners, (reverse) ? lastIdx - idx : idx);
				if (!(doNext = listener(target))) break;
			}
			break;

		case HLD_EVENT_COLLISION:
			for (uint32_t idx = 0; idx < numListeners; idx++) {
				bool (* listener)(HLDInstance *, HLDInstance *);
				listener = DynArrGet(listeners, (reverse) ? lastIdx - idx : idx);
				if (!(doNext = listener(target, other))) break;
			}
			break;

		default:
			doNext = true;
			break;
	}

	return doNext;
}



/* ----- PUBLIC FUNCTIONS ----- */

EventTrap * EventTrapNew(
		HLDEventType eventType,
		void (* origListener)(HLDInstance *, HLDInstance *)
) {
	Trap * trap = malloc(sizeof(Trap));
	assert(trap);
	trap->eventType = eventType;
	trap->origListener = origListener;
	trap->upstreamListeners = DynArrNew(4);
	trap->downstreamListeners = DynArrNew(4);

	return (EventTrap *)trap;
}

void EventTrapFree(EventTrap * trap) {
	assert(trap);

	DynArrFree(((Trap *)trap)->downstreamListeners);
	DynArrFree(((Trap *)trap)->upstreamListeners);
	free(trap);

	return;
}

void EventTrapAddUpstream(
		EventTrap * trap,
		void * listener
) {
	assert(trap);
	assert(listener);

	DynArrPush(((Trap *)trap)->upstreamListeners, listener);

	return;
}

void EventTrapAddDownstream(
		EventTrap * trap,
		void * listener
) {
	assert(trap);
	assert(listener);

	DynArrPush(((Trap *)trap)->downstreamListeners, listener);

	return;
}

void EventTrapExecute(
		EventTrap * trap,
		HLDInstance * target,
		HLDInstance * other
) {
	bool doNext = ExecuteListeners(
			((Trap *)trap)->upstreamListeners,
			((Trap *)trap)->eventType,
			target,
			other,
			false
	);

	if (doNext) {
		if (((Trap *)trap)->origListener) {
			((Trap *)trap)->origListener(target, other);
		}

		ExecuteListeners(
				((Trap *)trap)->downstreamListeners,
				((Trap *)trap)->eventType,
				target,
				other,
				true
		);
	}

	return;
}
