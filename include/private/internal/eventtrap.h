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
#ifndef INTERNAL_EVENTTRAP_H
#define INTERNAL_EVENTTRAP_H

#include "foxutils/array.h"

#include "internal/hld.h"
#include "internal/modman.h"



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
		ModListener listener
);

void EventTrapAddDownstream(
		EventTrap * trap,
		ModListener listener
);

void EventTrapExecute(
		EventTrap * trap,
		HLDInstance * target,
		HLDInstance * other
);



#endif /* INTERNAL_EVENTTRAP_H */
