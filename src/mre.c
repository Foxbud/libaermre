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
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "foxutils/arraymacs.h"
#include "foxutils/mapmacs.h"
#include "foxutils/stringmapmacs.h"

#include "aer/mre.h"
#include "internal/confvars.h"
#include "internal/envconf.h"
#include "internal/err.h"
#include "internal/eventtrap.h"
#include "internal/export.h"
#include "internal/log.h"
#include "internal/modman.h"
#include "internal/mre.h"
#include "internal/rand.h"



/* ----- PRIVATE TYPES ----- */

/*
 * This struct holds context information about the target event when
 * recursively subscribing objects to an event that requires subscription.
 */
typedef struct SubscriptionContext {
	HLDEventType eventType;
	uint32_t eventNum;
	size_t * subCountsArr;
	HLDEventSubscribers * subArrs;
} SubscriptionContext;



/* ----- PRIVATE CONSTANTS ----- */

static const char * ABS_ASSET_PATH_FMT = "assets/mod/%s/%s";

static const size_t MAX_OBJ_TREE_DEPTH = 64;



/* ----- PRIVATE GLOBALS ----- */

static char assetPathBuf[1024];



/* ----- INTERNAL GLOBALS ----- */

AERMRE mre = {0};



/* ----- PRIVATE FUNCTIONS ----- */

static bool EventTrapDeinitCallback(EventTrap * trap, void * ctx) {
	(void)ctx;

	EventTrapDeinit(trap);

	return true;
}

static void PerformParentEvent(
		HLDInstance * target,
		HLDInstance * other
) {
	HLDObject * obj = HLDObjectLookup(mre.currentEvent.objIdx);
	int32_t parentObjIdx = obj->parentIndex;
	if (
			parentObjIdx >= 0
			&& (uint32_t)parentObjIdx < (*hldvars.objectTableHandle)->numItems
	) {
		hldfuncs.actionEventPerform(
				target,
				other,
				parentObjIdx,
				mre.currentEvent.type,
				mre.currentEvent.num
		);
	}

	return;
}

static void CommonEventListener(
		HLDInstance * target,
		HLDInstance * other
) {
	EventKey currentEvent = mre.currentEvent;
	EventTrap * trap = FoxMapMIndex(
			EventKey,
			EventTrap,
			mre.eventTraps,
			currentEvent
	);
	assert(trap);

	EventTrapIter iter;
	EventTrapIterInit(&iter, trap);

	/* Execute listeners and check if event was canceled. */
	if (!EventTrapIterNext(&iter, target, other)) {
		switch (currentEvent.type) {
			case HLD_EVENT_CREATE:
				hldfuncs.actionInstanceDestroy(target, other, -1, false);
				break;

			default:
				break;
		}
	}

	EventTrapIterDeinit(&iter);

	return;
}

static HLDArrayPreSize ReallocEventArr(
		HLDArrayPreSize oldArr,
		size_t newSize
) {
	HLDArrayPreSize newArr;

	if (oldArr.size < newSize) {
		newArr = (HLDArrayPreSize){
			.size = newSize,
			.elements = calloc(newSize, sizeof(HLDEventWrapper *))
		};
		assert(newArr.elements);
		if (oldArr.size > 0) {
			memcpy(
					newArr.elements,
					oldArr.elements,
					oldArr.size * sizeof(HLDEventWrapper *)
			);
			/* TODO Figure out how to safely free oldArr.elements. */
		}
	} else {
		newArr = oldArr;
	}

	return newArr;
}

static EventTrap EntrapEvent(
		HLDObject * obj,
		HLDEventType eventType,
		uint32_t eventNum
) {
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

		default:
			LogErr(
					"\"%s\" called with unsupported event type %u.",
					__func__,
					eventType
			);
			abort();
	}
	newArr = ReallocEventArr(oldArr, numSubEvents);

	/* Update object with new event array. */
	obj->eventListeners[eventType] = newArr;

	/* Get wrapper, event and handler. */
	HLDEventWrapper * wrapper = ((HLDEventWrapper **)newArr.elements)[eventNum];
	HLDEvent * event;
	HLDNamedFunction * oldHandler;
	if (wrapper) {
		event = wrapper->event;
		oldHandler = event->handler;
		event->handler = &mre.eventHandler;
	} else {
		oldHandler = NULL;
		event = HLDEventNew(&mre.eventHandler);
		wrapper = HLDEventWrapperNew(event);
		((HLDEventWrapper **)newArr.elements)[eventNum] = wrapper;
	}

	/* Create event trap. */
	EventTrap trap;
	EventTrapInit(
			&trap,
			eventType,
			(oldHandler) ? (
				(void (*)(HLDInstance *, HLDInstance *))oldHandler->function
				)	: (
					&PerformParentEvent
				)
	);
	
	return trap;
}

static void BuildInstanceLocals(void) {
	size_t numLocals = hldvars.instanceLocalTable->size;
	const char ** names = hldvars.instanceLocalTable->elements;
	for (uint32_t idx = 0; idx < numLocals; idx++) {
		*FoxMapMInsert(const char *, int32_t, mre.instLocals, names[idx]) = idx;
	}

	return;
}

static void BuildObjTree(void) {
	size_t numObjs = (*hldvars.objectTableHandle)->numItems;
	for (uint32_t idx = 0; idx < numObjs; idx++) {
		HLDObject * obj = HLDObjectLookup(idx);
		assert(obj);
		ObjTreeInsert(
				mre.objTree,
				obj->parentIndex,
				idx
		);
	}

	return;
}

static bool EnsureEventSubscriber(
		int32_t objIdx,
		void * ctx
) {
#define ctx ((SubscriptionContext *)ctx)
	EventKey key = {
		.type = ctx->eventType,
		.num = ctx->eventNum,
		.objIdx = objIdx
	};
	if (!FoxMapMIndex(EventKey, uint8_t, mre.eventSubscribers, key)) {
		uint32_t arrIdx = ctx->subCountsArr[key.num]++;
		ctx->subArrs[key.num].objects[arrIdx] = objIdx;
		FoxMapMInsert(EventKey, uint8_t, mre.eventSubscribers, key);
	}

	return true;
#undef ctx
}

static void RegisterEventSubscriber(EventKey key) {
	SubscriptionContext ctx;
	ctx.eventType = key.type;
	ctx.eventNum = key.num;
	switch (key.type) {
		case HLD_EVENT_ALARM:
			ctx.subCountsArr = *hldvars.alarmEventSubscriberCounts;
			ctx.subArrs = *hldvars.alarmEventSubscribers;
			break;

		case HLD_EVENT_STEP:
			ctx.subCountsArr = *hldvars.stepEventSubscriberCounts;
			ctx.subArrs = *hldvars.stepEventSubscribers;
			break;

		default:
			LogErr(
					"\"%s\" called with unsupported event type %u.",
					__func__,
					key.type
			);
			abort();
	}

	if (!FoxMapMIndex(EventKey, uint8_t, mre.eventSubscribers, key)) {
		ObjTreeForEach(
				mre.objTree,
				key.objIdx,
				MAX_OBJ_TREE_DEPTH,
				&EnsureEventSubscriber,
				&ctx
		);
	}

	return;
}

static void MaskEventSubscribers(
		HLDEventType eventType,
		size_t numEvents,
		size_t * subCountsArr,
		HLDEventSubscribers * subArrs
) {
	size_t numObjs = (*hldvars.objectTableHandle)->numItems;

	for (uint32_t eventNum = 0; eventNum < numEvents; eventNum++) {
		size_t oldSubCount = subCountsArr[eventNum];
		int32_t * oldSubArr = subArrs[eventNum].objects;

		int32_t * newSubArr = malloc(numObjs * sizeof(int32_t));
		assert(newSubArr);
		subArrs[eventNum].objects = newSubArr;
		subCountsArr[eventNum] = 0;

		for (uint32_t subIdx = 0; subIdx < oldSubCount; subIdx++) {
			EventKey key = {
				.type = eventType,
				.num = eventNum,
				.objIdx = oldSubArr[subIdx]
			};
			RegisterEventSubscriber(key);
		}
	}

	return;
}

static void InitMRE(
		HLDVariables varRefs,
		HLDFunctions funcRefs
) {
	LogInfo("Initializing mod runtime environment...");
	hldvars = varRefs;
	hldfuncs = funcRefs;

	mre = (AERMRE){
		.roomIndexPrevious = 0,
		.objTree = ObjTreeNew(),
		.instLocals = FoxStringMapMNew(int32_t),
		.eventTraps = FoxMapMNewExt(
				EventKey,
				EventTrap,
				FOXMAP_DEF_INITSLOTS,
				&EventKeyHash,
				&EventKeyCompare
		),
		.currentEvent = (EventKey){0},
		.eventHandler = (HLDNamedFunction){
			.name = "AEREventHandler",
			.function = &CommonEventListener
		},
		.eventSubscribers = FoxMapMNewExt(
				EventKey,
				uint8_t,
				FOXMAP_DEF_INITSLOTS,
				&EventKeyHash,
				&EventKeyCompare
		),
		.stage = STAGE_INIT
	};

	BuildInstanceLocals();

	LogInfo("Done initializing mod runtime environment.");

	return;
}

static void InitMods(void) {
	size_t numMods = FoxArrayMSize(Mod, &modman.mods);

	/* Register sprites. */
	mre.stage = STAGE_SPRITE_REG;
	LogInfo("Registering mod sprites...");
	/*
	 * Reverse order so that higher-priority mods' sprite replacements take
	 * precedence over those of lower-priority mods.
	 */
	for (int32_t modIdx = numMods - 1; modIdx >= 0; modIdx--) {
		Mod * mod = FoxArrayMIndex(Mod, &modman.mods, modIdx);
		if (mod->regSprites) {
			*FoxArrayMPush(Mod *, &modman.context) = mod;
			mod->regSprites();
			FoxArrayMPop(Mod *, &modman.context);
		}
	}
	LogInfo("Done.");

	/* Register objects. */
	mre.stage = STAGE_OBJECT_REG;
	LogInfo("Registering mod objects...");
	for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
		Mod * mod = FoxArrayMIndex(Mod, &modman.mods, modIdx);
		if (mod->regObjects) {
			*FoxArrayMPush(Mod *, &modman.context) = mod;
			mod->regObjects();
			FoxArrayMPop(Mod *, &modman.context);
		}
	}
	LogInfo("Done.");

	/* Build object inheritance tree and mask event subscribers. */
	BuildObjTree();
	MaskEventSubscribers(
			HLD_EVENT_ALARM,
			12,
			*hldvars.alarmEventSubscriberCounts,
			*hldvars.alarmEventSubscribers
	);
	MaskEventSubscribers(
			HLD_EVENT_STEP,
			3,
			*hldvars.stepEventSubscriberCounts,
			*hldvars.stepEventSubscribers
	);

	/* Register listeners. */
	mre.stage = STAGE_LISTENER_REG;
	LogInfo("Registering mod event listeners...");
	for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
		Mod * mod = FoxArrayMIndex(Mod, &modman.mods, modIdx);
		if (mod->regObjListeners) {
			*FoxArrayMPush(Mod *, &modman.context) = mod;
			mod->regObjListeners();
			FoxArrayMPop(Mod *, &modman.context);
		}
	}
	LogInfo("Done.");

	mre.stage = STAGE_ACTION;

	return;
}



/* ----- INTERNAL FUNCTIONS ----- */

const char * MREGetAbsAssetPath(const char * relAssetPath) {
	assert(relAssetPath);
	assert(!FoxArrayMEmpty(Mod *, &modman.context));

	snprintf(
			assetPathBuf,
			sizeof(assetPathBuf),
			ABS_ASSET_PATH_FMT,
			(*FoxArrayMPeek(Mod *, &modman.context))->name,
			relAssetPath
	);

	return assetPathBuf;
}

void MRERegisterEventListener(
		HLDObject * obj,
		EventKey key,
		bool (* listener)(AEREventTrapIter *, AERInstance *, AERInstance *)
) {
	/* Register subscription if subscribable event. */
	switch (key.type) {
		case HLD_EVENT_ALARM:
		case HLD_EVENT_STEP:
			RegisterEventSubscriber(key);
			break;

		default:
			break;
	}

	EventTrap * trap = FoxMapMIndex(
			EventKey,
			EventTrap,
			mre.eventTraps,
			key
	);
	if (!trap) {
		trap = FoxMapMInsert(
				EventKey,
				EventTrap,
				mre.eventTraps,
				key
		);
		*trap = EntrapEvent(obj, key.type, key.num);
	}

	EventTrapAddListener(
			trap,
			(ModListener){
				.mod = *FoxArrayMPeek(Mod *, &modman.context),
				.func = (void (*)(void))listener
			}
	);

	return;
}



/* ----- UNLISTED FUNCTIONS ----- */

AER_EXPORT void AERHookInit(
		HLDVariables varRefs,
		HLDFunctions funcRefs
) {
	LogInfo("Checking engine variables...");
	HLDVariablesCheck(&varRefs);
	LogInfo("Done checking engine variables.");

	InitMRE(varRefs, funcRefs);
	ModManConstructor();
	InitMods();

	return;
}

AER_EXPORT void AERHookStep(void) {
	/* Check if room changed. */
	int32_t roomIndexCurrent = *hldvars.roomIndexCurrent;
	if (roomIndexCurrent != mre.roomIndexPrevious) {
		/* Call room change listeners. */
		size_t numListeners = FoxArrayMSize(
				ModListener,
				&modman.roomChangeListeners
		);
		for (uint32_t idx = 0; idx < numListeners; idx++) {
			ModListener * listener = FoxArrayMIndex(
					ModListener,
					&modman.roomChangeListeners,
					idx
			);
			*FoxArrayMPush(Mod *, &modman.context) = listener->mod;
			(
			 (void (*)(int32_t, int32_t))
			 listener->func
			)(roomIndexCurrent, mre.roomIndexPrevious);
			FoxArrayMPop(Mod *, &modman.context);
		}
		mre.roomIndexPrevious = roomIndexCurrent;
	}

	/* Call room step listeners. */
	size_t numListeners = FoxArrayMSize(
			ModListener,
			&modman.roomStepListeners
	);
	for (uint32_t idx = 0; idx < numListeners; idx++) {
		ModListener * listener = FoxArrayMIndex(
				ModListener,
				&modman.roomStepListeners,
				idx
		);
		*FoxArrayMPush(Mod *, &modman.context) = listener->mod;
		listener->func();
		FoxArrayMPop(Mod *, &modman.context);
	}

	return;
}

AER_EXPORT void AERHookEvent(
		HLDObject * targetObject,
		HLDEventType eventType,
		int32_t eventNum
) {
	mre.currentEvent = (EventKey){
		.type = eventType,
		.num = eventNum,
		.objIdx = targetObject->index
	};

	return;
}



/* ----- LIBRARY MANAGEMENT ----- */

__attribute__((constructor))
void AERConstructor(void) {
	LogInfo("Action-Event-Response (AER) Mod Runtime Environment (MRE)");

	LogInfo("Initializing environment configuration module...");
	EnvConfConstructor();
	LogInfo("Done initializing environment configuration module.");

	LogInfo("Initializing configuration variables...");
	ConfVarsConstructor();
	LogInfo("Done initializing configuration variables.");

	LogInfo("Initializing random module...");
	RandConstructor();
	LogInfo("Done initializing random module.");

	return;
}

__attribute__((destructor))
void AERDestructor(void) {
	ModManDestructor();

	LogInfo("Deinitializing mod runtime environment...");

	for (uint32_t idx = 0; idx < 12; idx++) {
		HLDEventSubscribers * subArr = *hldvars.alarmEventSubscribers + idx;
		if (subArr->objects) {
			free(subArr->objects);
			subArr->objects = NULL;
		}
	}
	for (uint32_t idx = 0; idx < 3; idx++) {
		HLDEventSubscribers * subArr = *hldvars.stepEventSubscribers + idx;
		if (subArr->objects) {
			free(subArr->objects);
			subArr->objects = NULL;
		}
	}
	FoxMapMFree(EventKey, uint8_t, mre.eventSubscribers);
	mre.eventSubscribers = NULL;

	FoxMapMForEachElement(
			EventKey,
			EventTrap,
			mre.eventTraps,
			&EventTrapDeinitCallback,
			NULL
	);
	FoxMapMFree(EventKey, EventTrap, mre.eventTraps);
	mre.eventTraps = NULL;

	FoxMapMFree(const char *, int32_t, mre.instLocals);
	mre.instLocals = NULL;

	ObjTreeFree(mre.objTree);
	mre.objTree = NULL;

	LogInfo("Done deinitializing mod runtime environment.");

	LogInfo("Deinitializing random module...");
	RandDestructor();
	LogInfo("Done deinitializing random module.");

	LogInfo("Deinitializing configuration variables...");
	ConfVarsDestructor();
	LogInfo("Done deinitializing configuration variables.");

	LogInfo("Deinitializing environment configuration module...");
	EnvConfDestructor();
	LogInfo("Done deinitializing environment configuration module.");

	return;
}



/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT uint32_t AERGetNumSteps(void) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, 0);

	return *hldvars.numSteps;
}

AER_EXPORT const bool * AERGetKeysPressed(void) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

	return *hldvars.keysPressedTable;
}

AER_EXPORT const bool * AERGetKeysHeld(void) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

	return *hldvars.keysHeldTable;
}

AER_EXPORT const bool * AERGetKeysReleased(void) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

	return *hldvars.keysReleasedTable;
}

AER_EXPORT const bool * AERGetMouseButtonsPressed(void) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

	return *hldvars.mouseButtonsPressedTable;
}

AER_EXPORT const bool * AERGetMouseButtonsHeld(void) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

	return *hldvars.mouseButtonsHeldTable;
}

AER_EXPORT const bool * AERGetMouseButtonsReleased(void) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

	return *hldvars.mouseButtonsReleasedTable;
}

AER_EXPORT void AERGetMousePosition(
		uint32_t * x,
		uint32_t * y
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!(x || y), AER_NULL_ARG);

	if (x) *x = *hldvars.mousePosX;
	if (y) *y = *hldvars.mousePosY;

	return;
}
