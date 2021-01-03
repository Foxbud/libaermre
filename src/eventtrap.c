/**
 * @copyright 2020 the libaermre authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "foxutils/arraymacs.h"

#include "internal/eventtrap.h"



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
	FoxArrayMInitExt(ModListener, &trap->modListeners, 2);

	return;
}

void EventTrapDeinit(EventTrap * trap) {
	assert(trap);

	trap->eventType = 0;
	trap->origListener = NULL;
	FoxArrayMDeinit(ModListener, &trap->modListeners);

	return;
}

void EventTrapAddListener(
		EventTrap * trap,
		ModListener listener
) {
	assert(trap);

	*FoxArrayMPush(ModListener, &trap->modListeners) = listener;

	return;
}

void EventTrapIterInit(
		EventTrapIter * iter,
		EventTrap * trap
) {
	assert(iter);
	assert(trap);

	iter->base.next = (
			(bool (*)(AEREventTrapIter *, AERInstance *, AERInstance *))
			EventTrapIterNext
	);
	iter->trap = trap;
	iter->nextIdx = 0;

	return;
}

void EventTrapIterDeinit(EventTrapIter * iter) {
	assert(iter);

	iter->base.next = NULL;
	iter->trap = NULL;
	iter->nextIdx = 0;

	return;
}

bool EventTrapIterNext(
		EventTrapIter * iter,
		HLDInstance * target,
		HLDInstance * other
) {
	assert(iter);
	assert(target);
	assert(other);
	bool result = true;

	EventTrap * trap = iter->trap;
	FoxArray * modListeners = &trap->modListeners;
	if (iter->nextIdx < FoxArrayMSize(ModListener, modListeners)) {
		ModListener * listener = FoxArrayMIndex(
				ModListener,
				modListeners,
				iter->nextIdx++
		);
		*FoxArrayMPush(Mod *, &modman.context) = listener->mod;
		result = (
				(bool (*)(EventTrapIter *, HLDInstance *, HLDInstance *))
				listener->func
		)(iter,target,other);
		FoxArrayMPop(Mod *, &modman.context);
	} else {
		trap->origListener(target, other);
	}

	return result;
}
