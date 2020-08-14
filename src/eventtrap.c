#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "foxutils/arraymacs.h"

#include "eventtrap.h"



/* ----- PRIVATE FUNCTIONS ----- */

static bool ExecuteListeners(
		FoxArray * listeners,
		HLDEventType eventType,
		HLDInstance * target,
		HLDInstance * other,
		bool reverse
) {
	bool doNext;

	size_t numListeners = FoxArrayMSize(void *, listeners);
	uint32_t lastIdx = numListeners - 1;
	switch (eventType) {
		case HLD_EVENT_COLLISION:
			for (uint32_t idx = 0; idx < numListeners; idx++) {
				bool (* listener)(HLDInstance *, HLDInstance *);
				listener = *FoxArrayMIndex(
						void *,
						listeners,
						(reverse) ? lastIdx - idx : idx
				);
				if (!(doNext = listener(target, other))) break;
			}
			break;

		default:
			for (uint32_t idx = 0; idx < numListeners; idx++) {
				bool (* listener)(HLDInstance *);
				listener = *FoxArrayMIndex(
						void *,
						listeners,
						(reverse) ? lastIdx - idx : idx
				);
				if (!(doNext = listener(target))) break;
			}
			break;
	}

	return doNext;
}



/* ----- PUBLIC FUNCTIONS ----- */

void EventTrapInit(
		EventTrap * trap,
		HLDEventType eventType,
		void (* origListener)(HLDInstance *, HLDInstance *)
) {
	assert(trap);
	assert(origListener);

	trap->eventType = eventType;
	trap->origListener = origListener;
	FoxArrayMInitExt(void *, &trap->upstreamListeners, 4);
	FoxArrayMInitExt(void *, &trap->downstreamListeners, 4);

	return;
}

void EventTrapDeinit(EventTrap * trap) {
	assert(trap);

	trap->eventType = 0;
	trap->origListener = NULL;
	FoxArrayMDeinit(void *, &trap->upstreamListeners);
	FoxArrayMDeinit(void *, &trap->downstreamListeners);

	return;
}

void EventTrapAddUpstream(
		EventTrap * trap,
		void * listener
) {
	assert(trap);
	assert(listener);

	*FoxArrayMPush(void *, &trap->upstreamListeners) = listener;

	return;
}

void EventTrapAddDownstream(
		EventTrap * trap,
		void * listener
) {
	assert(trap);
	assert(listener);

	*FoxArrayMPush(void *, &trap->downstreamListeners) = listener;

	return;
}

void EventTrapExecute(
		EventTrap * trap,
		HLDInstance * target,
		HLDInstance * other
) {
	bool doNext = ExecuteListeners(
			&trap->upstreamListeners,
			trap->eventType,
			target,
			other,
			false
	);

	if (doNext) {
		trap->origListener(target, other);

		ExecuteListeners(
				&trap->downstreamListeners,
				trap->eventType,
				target,
				other,
				true
		);
	}

	return;
}
