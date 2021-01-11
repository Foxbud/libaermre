/**
 * @copyright 2021 the libaermre authors
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
#ifndef INTERNAL_EVENT_H
#define INTERNAL_EVENT_H

#include "aer/event.h"
#include "aer/instance.h"
#include "internal/hld.h"

/* ----- INTERNAL TYPES ----- */

typedef struct __attribute__((packed)) EventKey {
  HLDEventType type;
  int32_t num;
  int32_t objIdx;
} EventKey;

/* ----- INTERNAL GLOBALS ----- */

extern EventKey currentEvent;

/* ----- INTERNAL FUNCTIONS ----- */

void EventManRegisterEventListener(HLDObject *obj, EventKey key,
                                   bool (*listener)(AEREvent *,
                                                    AERInstance *,
                                                    AERInstance *));

void EventManMaskSubscriptionArrays(void);

void EventManConstructor(void);

void EventManDestructor(void);

#endif /* INTERNAL_EVENT_H */
