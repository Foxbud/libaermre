#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "foxutils/arraymacs.h"

#include "internal/eventtrap.h"



/* ----- PRIVATE FUNCTIONS ----- */

static bool ExecuteListeners(
		FoxArray * listeners,
		HLDEventType eventType,
		HLDInstance * target,
		HLDInstance * other,
		bool reverse
) {
	bool doNext = true;

	size_t numListeners = FoxArrayMSize(ModListener, listeners);
	uint32_t lastIdx = numListeners - 1;
	switch (eventType) {
		case HLD_EVENT_COLLISION:
			for (uint32_t idx = 0; idx < numListeners; idx++) {
				ModListener * listener = FoxArrayMIndex(
						ModListener,
						listeners,
						(reverse) ? lastIdx - idx : idx
				);
				*FoxArrayMPush(Mod *, &modman.context) = listener->mod;
				doNext = listener->func.hldObjPair(target, other);
				FoxArrayMPop(Mod *, &modman.context);
				if (!doNext) break;
			}
			break;

		default:
			for (uint32_t idx = 0; idx < numListeners; idx++) {
				ModListener * listener = FoxArrayMIndex(
						ModListener,
						listeners,
						(reverse) ? lastIdx - idx : idx
				);
				*FoxArrayMPush(Mod *, &modman.context) = listener->mod;
				doNext = listener->func.hldObj(target);
				FoxArrayMPop(Mod *, &modman.context);
				if (!doNext) break;
			}
			break;
	}

	return doNext;
}



/* ----- INTERNAL FUNCTIONS ----- */

void EventTrapInit(
		EventTrap * trap,
		HLDEventType eventType,
		void (* origListener)(HLDInstance *, HLDInstance *)
) {
	assert(trap);
	assert(origListener);

	trap->eventType = eventType;
	trap->origListener = origListener;
	FoxArrayMInitExt(ModListener, &trap->upstreamListeners, 2);
	FoxArrayMInitExt(ModListener, &trap->downstreamListeners, 2);

	return;
}

void EventTrapDeinit(EventTrap * trap) {
	assert(trap);

	trap->eventType = 0;
	trap->origListener = NULL;
	FoxArrayMDeinit(ModListener, &trap->upstreamListeners);
	FoxArrayMDeinit(ModListener, &trap->downstreamListeners);

	return;
}

void EventTrapAddUpstream(
		EventTrap * trap,
		ModListener listener
) {
	assert(trap);

	*FoxArrayMPush(ModListener, &trap->upstreamListeners) = listener;

	return;
}

void EventTrapAddDownstream(
		EventTrap * trap,
		ModListener listener
) {
	assert(trap);

	*FoxArrayMPush(ModListener, &trap->downstreamListeners) = listener;

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
