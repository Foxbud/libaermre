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
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "foxutils/arraymacs.h"
#include "foxutils/mapmacs.h"

#include "aer/event.h"
#include "internal/core.h"
#include "internal/event.h"
#include "internal/hld.h"
#include "internal/log.h"
#include "internal/mod.h"
#include "internal/object.h"

/* ----- PRIVATE TYPES ----- */

typedef struct EventTrap {
    FoxArray modListeners;
    void (*origListener)(HLDInstance* target, HLDInstance* other);
    HLDEventType eventType;
} EventTrap;

typedef struct EventTrapIter {
    AEREvent base;
    EventTrap* trap;
    uint32_t nextIdx;
} EventTrapIter;

typedef struct RecursiveRegisterSubscribersContext {
    size_t* subCount;
    HLDEventSubscribers subArr;
    EventKey key;
} RecursiveRegisterSubscribersContext;

/* ----- PRIVATE GLOBALS ----- */

static HLDNamedFunction eventHandler = {0};

static FoxMap eventTraps = {0};

static FoxMap eventSubscribers = {0};

static int32_t* drawEventTargets = NULL;

/* ----- INTERNAL GLOBALS ----- */

EventKey currentEvent = {0};

/* ----- PRIVATE FUNCTIONS ----- */

static void EventTrapInit(EventTrap* trap,
                          HLDEventType eventType,
                          void (*origListener)(HLDInstance*, HLDInstance*)) {
    assert(trap);

    trap->eventType = eventType;
    trap->origListener = origListener;
    FoxArrayMInitExt(void*, &trap->modListeners, 2);

    return;
}

static void EventTrapDeinit(EventTrap* trap) {
    assert(trap);

    trap->eventType = 0;
    trap->origListener = NULL;
    FoxArrayMDeinit(void*, &trap->modListeners);

    return;
}

static void EventTrapAddListener(EventTrap* trap, void* listener) {
    assert(trap);

    *FoxArrayMPush(void*, &trap->modListeners) = listener;

    return;
}

static bool EventTrapDeinitCallback(EventTrap* trap, void* ctx) {
    (void)ctx;

    EventTrapDeinit(trap);

    return true;
}

static bool EventTrapIterNext(EventTrapIter* iter,
                              HLDInstance* target,
                              HLDInstance* other) {
    assert(iter);
    assert(target);
    assert(other);
    bool result = true;

    EventTrap* trap = iter->trap;
    FoxArray* modListeners = &trap->modListeners;
    if (iter->nextIdx < FoxArrayMSize(ModListener, modListeners)) {
        bool (*listener)(EventTrapIter*, HLDInstance*, HLDInstance*) =
            *FoxArrayMIndex(void*, modListeners, iter->nextIdx++);
        result = listener(iter, target, other);
    } else if (trap->origListener) {
        trap->origListener(target, other);
    }

    return result;
}

static void EventTrapIterInit(EventTrapIter* iter, EventTrap* trap) {
    assert(iter);
    assert(trap);

    iter->base.handle =
        ((bool (*)(AEREvent*, AERInstance*, AERInstance*))EventTrapIterNext);
    iter->base.next = (AEREvent*)iter;
    iter->trap = trap;
    iter->nextIdx = 0;

    return;
}

static void EventTrapIterDeinit(EventTrapIter* iter) {
    assert(iter);

    iter->base.handle = NULL;
    iter->base.next = NULL;
    iter->trap = NULL;
    iter->nextIdx = 0;

    return;
}

static int32_t Int32KeyCompare(const int32_t* keyA, const int32_t* keyB) {
    return *keyA - *keyB;
}

static void CommonEventListener(HLDInstance* target, HLDInstance* other) {
    EventTrap* trap =
        FoxMapMIndex(EventKey, EventTrap, &eventTraps, currentEvent);
    assert(trap);

    EventTrapIter iter;
    EventTrapIterInit(&iter, trap);
    /* If draw event, set draw stage. */
    CoreStage origStage = stage;
    if (currentEvent.type == HLD_EVENT_DRAW)
        stage = STAGE_DRAW;

    /* Execute listeners and check if event was canceled. */
    if (!EventTrapIterNext(&iter, target, other)) {
        switch (currentEvent.type) {
            case HLD_EVENT_CREATE:
                hldfuncs.actionInstanceDestroy(target, other, -1, false);
                break;

                /* TODO Figure out how to cancel destruction event. */

            default:
                break;
        }
    }

    stage = origStage;
    EventTrapIterDeinit(&iter);

    return;
}

static void DefaultEventPerformParent(HLDInstance* target, HLDInstance* other) {
    HLDObject* obj = HLDObjectLookup(currentEvent.objIdx);
    int32_t parentObjIdx = obj->parentIndex;
    if (parentObjIdx >= 0 &&
        (uint32_t)parentObjIdx < (*hldvars.objectTableHandle)->numItems)
        hldfuncs.actionEventPerform(target, other, parentObjIdx,
                                    currentEvent.type, currentEvent.num);

    return;
}

static void DefaultEventDrawSelf(HLDInstance* target, HLDInstance* other) {
    (void)other;

    if (HLDSpriteLookup(target->spriteIndex))
        hldfuncs.actionDrawSelf(target);

    return;
}

static bool RecursiveRegisterSubscribersCallback(
    const int32_t* objIdx,
    RecursiveRegisterSubscribersContext* ctx) {
    ctx->key.objIdx = *objIdx;
    if (FoxMapMIndex(EventKey, uint8_t, &eventSubscribers, ctx->key))
        return true;

    uint32_t arrIdx = (*ctx->subCount)++;
    ctx->subArr.objects[arrIdx] = *objIdx;
    FoxMapMInsert(EventKey, uint8_t, &eventSubscribers, ctx->key);

    FoxMap* children = ObjectManGetDirectChildren(*objIdx);
    if (children) {
        FoxMapMForEachKey(int32_t, int32_t, children,
                          RecursiveRegisterSubscribersCallback, ctx);
    }

    return true;
}

static void RegisterEventSubscriber(EventKey key) {
    RecursiveRegisterSubscribersContext ctx = {.key = key};
    switch (key.type) {
        case HLD_EVENT_ALARM:
            ctx.subCount = (*hldvars.alarmEventSubscriberCounts) + key.num;
            ctx.subArr = (*hldvars.alarmEventSubscribers)[key.num];
            break;

        case HLD_EVENT_STEP:
            ctx.subCount = (*hldvars.stepEventSubscriberCounts) + key.num;
            ctx.subArr = (*hldvars.stepEventSubscribers)[key.num];
            break;

        default:
            LogErr("\"%s\" called with unsupported event type %u.", __func__,
                   key.type);
            abort();
    }

    int32_t objIdx = key.objIdx;
    RecursiveRegisterSubscribersCallback(&objIdx, &ctx);

    return;
}

static void MaskEventSubscriptionArray(HLDEventType eventType,
                                       size_t numEvents,
                                       size_t* subCountsArr,
                                       HLDEventSubscribers* subArrs) {
    size_t numObjs = (*hldvars.objectTableHandle)->numItems;

    for (uint32_t eventNum = 0; eventNum < numEvents; eventNum++) {
        size_t oldSubCount = subCountsArr[eventNum];
        int32_t* oldSubArr = subArrs[eventNum].objects;

        int32_t* newSubArr = malloc(numObjs * sizeof(int32_t));
        assert(newSubArr);
        subArrs[eventNum].objects = newSubArr;
        subCountsArr[eventNum] = 0;

        for (uint32_t subIdx = 0; subIdx < oldSubCount; subIdx++) {
            EventKey key = {.type = eventType,
                            .num = eventNum,
                            .objIdx = oldSubArr[subIdx]};
            RegisterEventSubscriber(key);
        }
    }

    return;
}

static void SortEventSubscriptionArray(size_t numEvents,
                                       size_t* subCountsArr,
                                       HLDEventSubscribers* subArrs) {
    for (uint32_t eventNum = 0; eventNum < numEvents; eventNum++) {
        qsort(subArrs[eventNum].objects, subCountsArr[eventNum],
              sizeof(int32_t),
              (int (*)(const void*, const void*))Int32KeyCompare);
    }

    return;
}

static HLDArrayPreSize ReallocEventArr(HLDArrayPreSize oldArr, size_t newSize) {
    HLDArrayPreSize newArr;

    if (oldArr.size < newSize) {
        newArr = (HLDArrayPreSize){
            .size = newSize,
            .elements = calloc(newSize, sizeof(HLDEventWrapper*))};
        assert(newArr.elements);
        if (oldArr.size > 0) {
            memcpy(newArr.elements, oldArr.elements,
                   oldArr.size * sizeof(HLDEventWrapper*));
            /* TODO Figure out how to safely free oldArr.elements. */
        }
    } else {
        newArr = oldArr;
    }

    return newArr;
}

static void (*DetermineOriginalListener(HLDNamedFunction* oldHandler,
                                        int32_t objIdx,
                                        HLDEventType eventType,
                                        int32_t eventNum))(HLDInstance*,
                                                           HLDInstance*) {
    /* If existing HM listener, use that in all cases. */
    if (oldHandler) {
        return oldHandler->function;
    }

    /* Otherwise, it depends on event type. */
    switch (eventType) {
        case HLD_EVENT_DRAW:
            /* For draw event, it depends on event number. */
            if (eventNum == HLD_EVENT_DRAW_NORMAL) {
                /* If target draw event, perform parent event. */
                if (drawEventTargets[objIdx] >= 0) {
                    return DefaultEventPerformParent;
                }
                /* Otherwise, draw self. */
                return DefaultEventDrawSelf;
            }
            /* If any other draw event number, perform no default event. */
            return NULL;

        default:
            /* For all other event types, perform parent event. */
            return DefaultEventPerformParent;
    }
}

static EventTrap EntrapEvent(HLDObject* obj,
                             HLDEventType eventType,
                             int32_t eventNum) {
    size_t numObjs = (*hldvars.objectTableHandle)->numItems;
    HLDArrayPreSize oldArr, newArr;

    /* Get original event array. */
    oldArr = obj->eventListeners[eventType];

    /* Get new event array. */
    uint32_t numSubEvents;
    switch (eventType) {
        case HLD_EVENT_CREATE:
        case HLD_EVENT_DESTROY:
            numSubEvents = 1;
            break;

        case HLD_EVENT_STEP:
            numSubEvents = 3;
            break;

        case HLD_EVENT_ALARM:
            numSubEvents = 12;
            break;

        case HLD_EVENT_COLLISION:
            numSubEvents = numObjs;
            break;

        case HLD_EVENT_OTHER:
            /*
             * We don't yet know the maximum number of other events in this
             * version of the engine, so we're using 128 as a presumably safe
             * upper bound until we learn the true maximum.
             */
            numSubEvents = 128;
            break;

        case HLD_EVENT_DRAW:
            /*
             * We don't yet know the maximum number of draw events in this
             * version of the engine, so we're using 128 as a presumably safe
             * upper bound until we learn the true maximum.
             */
            numSubEvents = 128;
            break;

        default:
            LogErr("\"%s\" called with unsupported event type %u.", __func__,
                   eventType);
            abort();
    }
    newArr = ReallocEventArr(oldArr, numSubEvents);

    /* Update object with new event array. */
    obj->eventListeners[eventType] = newArr;

    /* Get wrapper, event, and handler. */
    HLDEventWrapper* wrapper = ((HLDEventWrapper**)newArr.elements)[eventNum];
    HLDEvent* event;
    HLDNamedFunction* oldHandler;
    if (wrapper) {
        event = wrapper->event;
        oldHandler = event->handler;
        event->handler = &eventHandler;
    } else {
        oldHandler = NULL;
        event = HLDEventNew(&eventHandler);
        wrapper = HLDEventWrapperNew(event);
        ((HLDEventWrapper**)newArr.elements)[eventNum] = wrapper;
    }

    /* Determine original event listener to execute. */
    void (*origListener)(HLDInstance*, HLDInstance*) =
        DefaultEventPerformParent;
    if (oldHandler) {
        origListener = oldHandler->function;
    } else if (eventType == HLD_EVENT_DRAW) {
        if (eventNum == HLD_EVENT_DRAW_NORMAL) {
            if (drawEventTargets[obj->index] == -1) {
                origListener = DefaultEventDrawSelf;
            }
        } else {
            origListener = NULL;
        }
    }

    /* Create event trap. */
    EventTrap trap;
    EventTrapInit(
        &trap, eventType,
        DetermineOriginalListener(oldHandler, obj->index, eventType, eventNum));

    return trap;
}

/* ----- INTERNAL FUNCTIONS ----- */

void EventManRegisterEventListener(HLDObject* obj,
                                   EventKey key,
                                   bool (*listener)(AEREvent*,
                                                    AERInstance*,
                                                    AERInstance*)) {
    /* Register subscription if subscribable event. */
    switch (key.type) {
        case HLD_EVENT_ALARM:
        case HLD_EVENT_STEP:
            RegisterEventSubscriber(key);
            break;

        default:
            break;
    }

    EventTrap* trap = FoxMapMIndex(EventKey, EventTrap, &eventTraps, key);
    if (!trap) {
        trap = FoxMapMInsert(EventKey, EventTrap, &eventTraps, key);
        *trap = EntrapEvent(obj, key.type, key.num);
    }

    EventTrapAddListener(trap, listener);

    return;
}

void EventManRecordDrawTargets(void) {
#define numDrawTypes (sizeof(HLD_EVENT_DRAW_TYPES) / sizeof(HLDEventDrawType))
    /* Initialize target table. */
    size_t numObjs = (*hldvars.objectTableHandle)->numItems;
    drawEventTargets = malloc(numObjs * sizeof(int32_t));
    assert(drawEventTargets);

    /* Record draw targets for each object. */
    for (int32_t objIdx = 0; (size_t)objIdx < numObjs; objIdx++) {
        HLDObject* obj = HLDObjectLookup(objIdx);
        assert(obj);
        HLDArrayPreSize listeners = obj->eventListeners[HLD_EVENT_DRAW];

        /* Check for custom HM draw listener. */
        if (listeners.size > HLD_EVENT_DRAW_NORMAL &&
            ((HLDEventWrapper**)listeners.elements)[HLD_EVENT_DRAW_NORMAL]) {
            drawEventTargets[objIdx] = objIdx;
        }

        /* Check for default draw listener. */
        else if (obj->parentIndex < 0) {
            drawEventTargets[objIdx] = -1;
        }

        /* Use parent's target. */
        else {
            drawEventTargets[objIdx] = drawEventTargets[obj->parentIndex];
        }
    }

    return;
#undef numDrawTypes
}

void EventManMaskSubscriptionArrays(void) {
    MaskEventSubscriptionArray(HLD_EVENT_ALARM, 12,
                               *hldvars.alarmEventSubscriberCounts,
                               *hldvars.alarmEventSubscribers);
    MaskEventSubscriptionArray(HLD_EVENT_STEP, 3,
                               *hldvars.stepEventSubscriberCounts,
                               *hldvars.stepEventSubscribers);

    return;
}

void EventManSortSubscriptionArrays(void) {
    SortEventSubscriptionArray(12, *hldvars.alarmEventSubscriberCounts,
                               *hldvars.alarmEventSubscribers);
    SortEventSubscriptionArray(3, *hldvars.stepEventSubscriberCounts,
                               *hldvars.stepEventSubscribers);

    return;
}

void EventManConstructor(void) {
    LogInfo("Initializing event module...");

    eventHandler = (HLDNamedFunction){.name = "AEREventHandler",
                                      .function = CommonEventListener};
    FoxMapMInit(EventKey, EventTrap, &eventTraps);
    FoxMapMInit(EventKey, uint8_t, &eventSubscribers);

    LogInfo("Done initializing event module.");
    return;
}

void EventManDestructor(void) {
    LogInfo("Deinitializing event module...");

    for (uint32_t idx = 0; idx < 12; idx++) {
        HLDEventSubscribers* subArr = *hldvars.alarmEventSubscribers + idx;
        if (subArr->objects) {
            free(subArr->objects);
            subArr->objects = NULL;
        }
    }
    for (uint32_t idx = 0; idx < 3; idx++) {
        HLDEventSubscribers* subArr = *hldvars.stepEventSubscribers + idx;
        if (subArr->objects) {
            free(subArr->objects);
            subArr->objects = NULL;
        }
    }

    if (drawEventTargets) {
        free(drawEventTargets);
        drawEventTargets = NULL;
    }

    FoxMapMDeinit(EventKey, uint8_t, &eventSubscribers);
    eventSubscribers = (FoxMap){0};

    FoxMapMForEachElement(EventKey, EventTrap, &eventTraps,
                          EventTrapDeinitCallback, NULL);
    FoxMapMDeinit(EventKey, EventTrap, &eventTraps);
    eventTraps = (FoxMap){0};

    eventHandler = (HLDNamedFunction){0};

    LogInfo("Done deinitializing event module.");
    return;
}