#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "foxutils/arraymacs.h"
#include "foxutils/mapmacs.h"
#include "foxutils/math.h"

#include "aer/mre.h"
#include "internal/envconf.h"
#include "internal/err.h"
#include "internal/eventkey.h"
#include "internal/eventtrap.h"
#include "internal/hld.h"
#include "internal/log.h"
#include "internal/modman.h"
#include "internal/objtree.h"
#include "internal/rand.h"



/* ----- PRIVATE TYPES ----- */

/*
 * This struct holds pointers to raw values and functions in the Game Maker
 * engine. These pointers are passed into the MRE from the hooks injected
 * into the game's executable.
 */
typedef struct HLDRefs {
	/* Number of steps since start of the game. */
	int32_t * numSteps;
	/* Tables of booleans where each index represents a key code. */
	bool (* keysPressedTable)[0x100];
	bool (* keysHeldTable)[0x100];
	bool (* keysReleasedTable)[0x100];
	/* Array of all registered rooms. */
	HLDArrayPreSize * roomTable;
	/* Index of currently active room. */
	int32_t * roomIndexCurrent;
	/* Actual room object of currently active room. */
	HLDRoom ** roomCurrent;
	/* Array of all registered sprites. */
	HLDSprite *** spriteTable;
	/* Hash table of all registered objects. */
	HLDHashTable ** objectTableHandle;
	/* Hash table of all in-game instances. */
	HLDHashTable * instanceTable;
	/*
	 * As an optimization, the engine only checks for alarm events on objects
	 * listed (or "subscribed") in these arrays.
	 */
	size_t (* alarmEventSubscriberCounts)[12];
	HLDEventSubscribers (* alarmEventSubscribers)[12];
	size_t (* stepEventSubscriberCounts)[3];
	HLDEventSubscribers (* stepEventSubscribers)[3];
	/* Register a new sprite. */
	__attribute__((cdecl)) int32_t (* actionSpriteAdd)(
			const char * fname,
			size_t imgNum,
			int32_t unknown0,
			int32_t unknown1,
			int32_t unknown2,
			int32_t unknown3,
			uint32_t origX,
			uint32_t origY
	);
	/* Register a new object. */
	__attribute__((cdecl)) int32_t (* actionObjectAdd)(void);
	/* Trigger an event as if it occurred "naturally." */
	__attribute__((cdecl)) int32_t (* actionEventPerform)(
			HLDInstance * target,
			HLDInstance * other,
			int32_t targetObjIdx,
			uint32_t eventType,
			int32_t eventNum
	);
	/*
	 * Custom Heart Machine function that sets an instance's draw depth based
	 * on its y position and the current room's height.
	 */
	__attribute__((cdecl)) HLDInstance * (* gmlScriptSetdepth)(
			HLDInstance * target,
			HLDInstance * other,
			void * unknown0,
			uint32_t unknown1,
			uint32_t unknown2
	);
	/* Spawn a new instance of an object. */
	__attribute__((cdecl)) HLDInstance * (* actionInstanceCreate)(
			int32_t objIdx,
			float posX,
			float posY
	);
	/*  */
	__attribute__((cdecl)) void (* actionInstanceChange)(
			HLDInstance * inst,
			int32_t newObjIdx,
			bool doEvents
	);
	/* Destroy an instance. */
	__attribute__((cdecl)) void (* actionInstanceDestroy)(
			HLDInstance * inst0,
			HLDInstance * inst1,
			int32_t objIdx,
			bool doEvent
	);
	/* Set instance's mask index. */
	__attribute__((cdecl)) void (* Instance_setMaskIndex)(
			HLDInstance * inst,
			int32_t maskIndex
	);
	/* Set an instance's direction and speed based on its motion vector. */
	__attribute__((cdecl)) void (* Instance_setMotionPolarFromCartesian)(
			HLDInstance * inst
	);
} HLDRefs;

/* This struct represents the current state of the mod runtime environment. */
typedef struct AERMRE {
	/* Raw engine pointers passed in by the hooks. */
	HLDRefs refs;
	/*
	 * Index of active room during previous game step. Solely for detecting
	 * room changes.
	 */
	int32_t roomIndexPrevious;
	/* Inheritance tree of all objects (including mod-registered). */
	ObjTree * objTree;
	/*
	 * Hash table of all events that have been entrapped during the mod
	 * event listener registration stage.
	 */
	FoxMap * eventTraps;
	/* Key of currently active event. */
	EventKey currentEvent;
	/* 
	 * Because C lacks enclosures, all entrapped events point to this common
	 * event handler which then looks up and executes the trap for the
	 * current event.
	 */
	HLDNamedFunction eventHandler;
	/* Internal record of all subscriptions to subscribable events. */
	FoxMap * eventSubscribers;
	/* Current stage of the MRE. */
	enum {
		STAGE_INIT,
		STAGE_SPRITE_REG,
		STAGE_OBJECT_REG,
		STAGE_LISTENER_REG,
		STAGE_ACTION
	} stage;
} AERMRE;

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

static const char * ASSET_PATH_FMT = "assets/mod/%s/%s";

static const size_t MAX_OBJ_TREE_DEPTH = 64;



/* ----- PRIVATE GLOBALS ----- */

static AERMRE mre;



/* ----- PRIVATE FUNCTIONS ----- */

static HLDObject * ObjectLookup(int32_t key) {
	return HLDHashTableLookup(
			*mre.refs.objectTableHandle,
			key
	);
}

static HLDInstance * InstanceLookup(int32_t key) {
	return HLDHashTableLookup(
			mre.refs.instanceTable,
			key
	);
}

static bool EventTrapDeinitCallback(EventTrap * trap, void * ctx) {
	(void)ctx;

	EventTrapDeinit(trap);

	return true;
}

static void PerformParentEvent(
		HLDInstance * target,
		HLDInstance * other
) {
	HLDObject * obj = ObjectLookup(mre.currentEvent.objIdx);
	int32_t parentObjIdx = obj->parentIndex;
	if (
			parentObjIdx >= 0
			&& (uint32_t)parentObjIdx < (*mre.refs.objectTableHandle)->numItems
	) {
		mre.refs.actionEventPerform(
				target,
				other,
				parentObjIdx,
				mre.currentEvent.type,
				mre.currentEvent.num
		);
	}

	return;
}

static __attribute__((cdecl)) void CommonEventListener(
		HLDInstance * target,
		HLDInstance * other
) {
	EventTrap * trap = FoxMapMIndex(
			EventKey,
			EventTrap,
			mre.eventTraps,
			mre.currentEvent
	);
	assert(trap);

	EventTrapExecute(trap, target, other);

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
			/* TODO Figured out how to safely free oldArr.elements. */
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
	size_t numObjs = (*mre.refs.objectTableHandle)->numItems;
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

static void RegisterObjectListener(
		HLDObject * obj,
		EventKey key,
		ModListener listener,
		bool downstream
) {
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

	if (downstream) {
		EventTrapAddDownstream(trap, listener);
	} else {
		EventTrapAddUpstream(trap, listener);
	}

	return;
}

static void BuildObjTree(void) {
	size_t numObjs = (*mre.refs.objectTableHandle)->numItems;
	for (uint32_t idx = 0; idx < numObjs; idx++) {
		HLDObject * obj = ObjectLookup(idx);
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
			ctx.subCountsArr = *mre.refs.alarmEventSubscriberCounts;
			ctx.subArrs = *mre.refs.alarmEventSubscribers;
			break;

		case HLD_EVENT_STEP:
			ctx.subCountsArr = *mre.refs.stepEventSubscriberCounts;
			ctx.subArrs = *mre.refs.stepEventSubscribers;
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
	size_t numObjs = (*mre.refs.objectTableHandle)->numItems;

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

static void InitMRE(HLDRefs refs) {
	LogInfo("Initializing mod runtime environment...");
	mre = (AERMRE){
		.refs = refs,
		.roomIndexPrevious = 0,
		.objTree = ObjTreeNew(),
		.eventTraps = FoxMapMNewExt(
				EventKey,
				EventTrap,
				FOXMAP_DEF_INITSLOTS,
				&EventKeyHash,
				&EventKeyCompare
		),
		.currentEvent = (EventKey){},
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

	EnvConfConstructor();
	RandConstructor();
	LogInfo("Done.");

	return;
}

static void InitMods(void) {
	size_t numMods = FoxArrayMSize(Mod, &modman.mods);

	/* Register sprites. */
	mre.stage = STAGE_SPRITE_REG;
	LogInfo("Registering mod sprites...");
	for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
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
			*mre.refs.alarmEventSubscriberCounts,
			*mre.refs.alarmEventSubscribers
	);
	MaskEventSubscribers(
			HLD_EVENT_STEP,
			3,
			*mre.refs.stepEventSubscriberCounts,
			*mre.refs.stepEventSubscribers
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



/* ----- UNLISTED FUNCTIONS ----- */

__attribute__((cdecl)) void AERHookInit(HLDRefs refs) {
	InitMRE(refs);
	ModManConstructor();
	InitMods();

	return;
}

__attribute__((cdecl)) void AERHookStep(void) {
	/* Check if room changed. */
	int32_t roomIndexCurrent = *mre.refs.roomIndexCurrent;
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
			listener->func.roomChange(roomIndexCurrent, mre.roomIndexPrevious);
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
		listener->func.roomStep();
		FoxArrayMPop(Mod *, &modman.context);
	}

	return;
}

__attribute__((cdecl)) void AERHookEvent(
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

__attribute__((constructor)) void AERConstructor(void) {
	LogInfo("Action-Event-Response (AER) Mod Runtime Environment (MRE)");

	return;
}

__attribute__((destructor)) void AERDestructor(void) {
	ModManDestructor();

	LogInfo("Deinitializing mod runtime environment...");
	for (uint32_t idx = 0; idx < 12; idx++) {
		HLDEventSubscribers * subArr = *mre.refs.alarmEventSubscribers + idx;
		if (subArr->objects) {
			free(subArr->objects);
			subArr->objects = NULL;
		}
	}
	for (uint32_t idx = 0; idx < 3; idx++) {
		HLDEventSubscribers * subArr = *mre.refs.stepEventSubscribers + idx;
		if (subArr->objects) {
			free(subArr->objects);
			subArr->objects = NULL;
		}
	}
	FoxMapMFree(EventKey, uint8_t, mre.eventSubscribers);

	FoxMapMForEachElement(
			EventKey,
			EventTrap,
			mre.eventTraps,
			&EventTrapDeinitCallback,
			NULL
	);
	FoxMapMFree(EventKey, EventTrap, mre.eventTraps);

	ObjTreeFree(mre.objTree);

	EnvConfDestructor();
	RandDestructor();
	LogInfo("Done.");

	return;
}



/* ----- PUBLIC FUNCTIONS ----- */

int32_t AERRegisterSprite(
		const char * filename,
		size_t numFrames,
		uint32_t origX,
		uint32_t origY
) {
	LogInfo(
			"Registering sprite \"%s\" for mod \"%s\"...",
			filename,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_SPRITE_REG, AER_SEQ_BREAK, -1);
	ErrIf(!filename, AER_NULL_ARG, -1);

	/* Construct asset path. */
	char pathBuf[256];
	snprintf(
			pathBuf,
			256,
			ASSET_PATH_FMT,
			(*FoxArrayMPeek(Mod *, &modman.context))->slug,
			filename
	);

	int32_t spriteIdx = mre.refs.actionSpriteAdd(
			pathBuf,
			numFrames,
			0,
			0,
			0,
			0,
			origX,
			origY
	);
	ErrIf(spriteIdx < 0, AER_BAD_FILE, -1);

	LogInfo("Successfully registered sprite to index %i.", spriteIdx);
	return spriteIdx;
}

int32_t AERRegisterObject(
		const char * name,
		int32_t parentIdx,
		int32_t spriteIdx,
		int32_t maskIdx,
		int32_t depth,
		bool visible,
		bool collisions,
		bool persistent
) {
	LogInfo(
			"Registering object \"%s\" for mod \"%s\"...",
			name,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_OBJECT_REG, AER_SEQ_BREAK, -1);
	ErrIf(!name, AER_NULL_ARG, -1);

	HLDObject * parent = ObjectLookup(parentIdx);
	ErrIf(!parent, AER_FAILED_LOOKUP, -1);

	int32_t objIdx = mre.refs.actionObjectAdd();
	HLDObject * obj = ObjectLookup(objIdx);
	ErrIf(!obj, AER_OUT_OF_MEM, -1);

	/* The engine expects a freeable (dynamically allocated) string for name. */
	char * tmpName = malloc(strlen(name) + 1);
	ErrIf(!tmpName, AER_OUT_OF_MEM, -1);
	obj->name = strcpy(tmpName, name);

	obj->parentIndex = parentIdx;
	obj->parent = parent;
	obj->spriteIndex = spriteIdx;
	obj->maskIndex = maskIdx;
	obj->depth = depth;
	obj->flags.visible = visible;
	obj->flags.collisions = collisions;
	obj->flags.persistent = persistent;

	LogInfo("Successfully registered object to index %i.", objIdx);
	return objIdx;
}

void AERRegisterCreateListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	LogInfo(
			"Registering create listener on object %i for mod \"%s\"...",
			objIdx,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_LISTENER_REG, AER_SEQ_BREAK);
	ErrIf(!listener, AER_NULL_ARG);

	HLDObject * obj = ObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);

	EventKey key = (EventKey){
		.type = HLD_EVENT_CREATE,
		.num = 0,
		.objIdx = objIdx
	};
	RegisterObjectListener(
			obj,
			key,
			(ModListener){
				.mod = *FoxArrayMPeek(Mod *, &modman.context),
				.func.aerObj = listener
			},
			downstream
	);

	LogInfo("Successfully registered create listener.");
	return;
}

void AERRegisterDestroyListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	LogInfo(
			"Registering destroy listener on object %i for mod \"%s\"...",
			objIdx,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_LISTENER_REG, AER_SEQ_BREAK);
	ErrIf(!listener, AER_NULL_ARG);

	HLDObject * obj = ObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);

	EventKey key = {
		.type = HLD_EVENT_DESTROY,
		.num = 0,
		.objIdx = objIdx
	};
	RegisterObjectListener(
			obj,
			key,
			(ModListener){
				.mod = *FoxArrayMPeek(Mod *, &modman.context),
				.func.aerObj = listener
			},
			downstream
	);

	LogInfo("Successfully registered destroy listener.");
	return;
}

void AERRegisterAlarmListener(
		int32_t objIdx,
		uint32_t alarmIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	LogInfo(
			"Registering alarm %u listener on object %i for mod \"%s\"...",
			alarmIdx,
			objIdx,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_LISTENER_REG, AER_SEQ_BREAK);
	ErrIf(!listener, AER_NULL_ARG);
	ErrIf(alarmIdx >= 12, AER_FAILED_LOOKUP);

	HLDObject * obj = ObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);

	EventKey key = {
		.type = HLD_EVENT_ALARM,
		.num = alarmIdx,
		.objIdx = objIdx
	};
	RegisterObjectListener(
			obj,
			key,
			(ModListener){
				.mod = *FoxArrayMPeek(Mod *, &modman.context),
				.func.aerObj = listener
			},
			downstream
	);
	RegisterEventSubscriber(key);

	LogInfo("Successfully registered alarm %u listener.", alarmIdx);
	return;
}

void AERRegisterStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	LogInfo(
			"Registering step listener on object %i for mod \"%s\"...",
			objIdx,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_LISTENER_REG, AER_SEQ_BREAK);
	ErrIf(!listener, AER_NULL_ARG);

	HLDObject * obj = ObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);

	EventKey key = {
		.type = HLD_EVENT_STEP,
		.num = HLD_EVENT_STEP_INLINE,
		.objIdx = objIdx
	};
	RegisterObjectListener(
			obj,
			key,
			(ModListener){
				.mod = *FoxArrayMPeek(Mod *, &modman.context),
				.func.aerObj = listener
			},
			downstream
	);
	RegisterEventSubscriber(key);

	LogInfo("Successfully registered step listener.");
	return;
}

void AERRegisterPreStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	LogInfo(
			"Registering pre-step listener on object %i for mod \"%s\"...",
			objIdx,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_LISTENER_REG, AER_SEQ_BREAK);
	ErrIf(!listener, AER_NULL_ARG);

	HLDObject * obj = ObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);

	EventKey key = {
		.type = HLD_EVENT_STEP,
		.num = HLD_EVENT_STEP_PRE,
		.objIdx = objIdx
	};
	RegisterObjectListener(
			obj,
			key,
			(ModListener){
				.mod = *FoxArrayMPeek(Mod *, &modman.context),
				.func.aerObj = listener
			},
			downstream
	);
	RegisterEventSubscriber(key);

	LogInfo("Successfully registered pre-step listener.");
	return;
}

void AERRegisterPostStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	LogInfo(
			"Registering post-step listener on object %i for mod \"%s\"...",
			objIdx,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_LISTENER_REG, AER_SEQ_BREAK);
	ErrIf(!listener, AER_NULL_ARG);

	HLDObject * obj = ObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);

	EventKey key = {
		.type = HLD_EVENT_STEP,
		.num = HLD_EVENT_STEP_POST,
		.objIdx = objIdx
	};
	RegisterObjectListener(
			obj,
			key,
			(ModListener){
				.mod = *FoxArrayMPeek(Mod *, &modman.context),
				.func.aerObj = listener
			},
			downstream
	);
	RegisterEventSubscriber(key);

	LogInfo("Successfully registered post-step listener.");
	return;
}

void AERRegisterCollisionListener(
		int32_t targetObjIdx,
		int32_t otherObjIdx,
		bool (* listener)(AERInstance * target, AERInstance * other),
		bool downstream
) {
	LogInfo(
			"Registering %i collision listener on object %i for mod \"%s\"...",
			otherObjIdx,
			targetObjIdx,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_LISTENER_REG, AER_SEQ_BREAK);
	ErrIf(!listener, AER_NULL_ARG);
	ErrIf(!ObjectLookup(otherObjIdx), AER_FAILED_LOOKUP);

	HLDObject * obj = ObjectLookup(targetObjIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);

	EventKey key = {
		.type = HLD_EVENT_COLLISION,
		.num = otherObjIdx,
		.objIdx = targetObjIdx
	};
	RegisterObjectListener(
			obj,
			key,
			(ModListener){
				.mod = *FoxArrayMPeek(Mod *, &modman.context),
				.func.aerObjPair = listener
			},
			downstream
	);

	LogInfo("Successfully registered %i collision listener.", otherObjIdx);
	return;
}

void AERRegisterAnimationEndListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	LogInfo(
			"Registering animation end listener on object %i for mod \"%s\"...",
			objIdx,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_LISTENER_REG, AER_SEQ_BREAK);
	ErrIf(!listener, AER_NULL_ARG);

	HLDObject * obj = ObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);

	EventKey key = {
		.type = HLD_EVENT_OTHER,
		.num = HLD_EVENT_OTHER_ANIMATION_END,
		.objIdx = objIdx
	};
	RegisterObjectListener(
			obj,
			key,
			(ModListener){
				.mod = *FoxArrayMPeek(Mod *, &modman.context),
				.func.aerObj = listener
			},
			downstream
	);

	LogInfo("Successfully registered animation end listener.");
	return;
}

uint32_t AERGetNumSteps(void) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, 0);

	return *mre.refs.numSteps;
}

const bool * AERGetKeysPressed(void) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

	return *mre.refs.keysPressedTable;
}

const bool * AERGetKeysHeld(void) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

	return *mre.refs.keysHeldTable;
}

const bool * AERGetKeysReleased(void) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

	return *mre.refs.keysReleasedTable;
}

int32_t AERRoomGetCurrent(void) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1);

	return *mre.refs.roomIndexCurrent;
}

const char * AERObjectGetName(int32_t objIdx) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

	HLDObject * obj = ObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP, NULL);

	return obj->name;
}

int32_t AERObjectGetParent(int32_t objIdx) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1);

	HLDObject * obj = ObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP, -1);

	return obj->parentIndex;
}

size_t AERObjectGetInstances(
		int32_t objIdx,
		size_t bufSize,
		AERInstance ** instBuf
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, 0);
	ErrIf(!instBuf && bufSize > 0, AER_NULL_ARG, 0);

	HLDObject * obj = ObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP, 0);

	size_t numInsts = obj->numInstances;
	size_t numToWrite = FoxMin(numInsts, bufSize);
	HLDNodeDLL * node = obj->instanceFirst;
	for (size_t idx = 0; idx < numToWrite; idx++) {
		instBuf[idx] = (AERInstance *)node->item;
		node = node->next;
	}

	return numInsts;
}

bool AERObjectGetCollisions(int32_t objIdx) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, false);

	HLDObject * obj = ObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP, false);

	return obj->flags.collisions;
}

void AERObjectSetCollisions(
		int32_t objIdx,
		bool collisions
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);

	HLDObject * obj = ObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);
	obj->flags.collisions = collisions;

	return;
}

size_t AERGetInstances(
		size_t bufSize,
		AERInstance ** instBuf
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, 0);
	ErrIf(!instBuf && bufSize > 0, AER_NULL_ARG, 0);

	HLDRoom * room = *mre.refs.roomCurrent;

	size_t numInsts = room->numInstances;
	size_t numToWrite = FoxMin(numInsts, bufSize);
	HLDInstance * inst = room->instanceFirst;
	for (size_t idx = 0; idx < numToWrite; idx++) {
		instBuf[idx] = (AERInstance *)inst;
		inst = inst->instanceNext;
	}

	return numInsts;
}

AERInstance * AERGetInstanceById(int32_t instId) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

	AERInstance * inst = (AERInstance *)InstanceLookup(instId);
	ErrIf(!inst, AER_FAILED_LOOKUP, NULL);

	return inst;
}

AERInstance * AERInstanceCreate(
		int32_t objIdx,
		float x,
		float y
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);
	ErrIf(!ObjectLookup(objIdx), AER_FAILED_LOOKUP, NULL);

	AERInstance * inst = (AERInstance *)mre.refs.actionInstanceCreate(
			objIdx,
			x,
			y
	);
	ErrIf(!inst, AER_OUT_OF_MEM, NULL);

	return inst;
}

void AERInstanceChange(
		AERInstance * inst,
		int32_t newObjIdx,
		bool doEvents
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);
	ErrIf(!ObjectLookup(newObjIdx), AER_FAILED_LOOKUP);

	mre.refs.actionInstanceChange(
			(HLDInstance *)inst,
			newObjIdx,
			doEvents
	);

	return;
}

void AERInstanceDestroy(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	mre.refs.actionInstanceDestroy(
			(HLDInstance *)inst,
			(HLDInstance *)inst,
			-1,
			true
	);

	return;
}

void AERInstanceDelete(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	mre.refs.actionInstanceDestroy(
			(HLDInstance *)inst,
			(HLDInstance *)inst,
			-1,
			false
	);

	return;
}

void AERInstanceSyncDepth(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	uint32_t unknownDatastructure[4] = {0};
	mre.refs.gmlScriptSetdepth(
			(HLDInstance *)inst,
			(HLDInstance *)inst,
			&unknownDatastructure,
			0,
			0
	);

	return;
}

int32_t AERInstanceGetId(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1);
	ErrIf(!inst, AER_NULL_ARG, -1);

	return ((HLDInstance *)inst)->id;
}

int32_t AERInstanceGetObject(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1);
	ErrIf(!inst, AER_NULL_ARG, -1);

	return ((HLDInstance *)inst)->objectIndex;
}

void AERInstanceGetPosition(
		AERInstance * inst,
		float * x,
		float * y
) {
#define inst ((HLDInstance *)inst)
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);
	ErrIf(!(x || y), AER_NULL_ARG);

	if (x) *x = inst->pos.x;
	if (y) *y = inst->pos.y;

	return;
#undef inst
}

void AERInstanceSetPosition(
		AERInstance * inst,
		float x,
		float y
) {
#define inst ((HLDInstance *)inst)
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	inst->pos.x = x;
	inst->pos.y = y;

	return;
#undef inst
}

float AERInstanceGetFriction(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, 0.0f);
	ErrIf(!inst, AER_NULL_ARG, 0.0f);

	return ((HLDInstance *)inst)->friction;
}

void AERInstanceSetFriction(
		AERInstance * inst,
		float friction
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	((HLDInstance *)inst)->friction = friction;

	return;
}

void AERInstanceGetMotion(
		AERInstance * inst,
		float * x,
		float * y
) {
#define inst ((HLDInstance *)inst)
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);
	ErrIf(!(x || y), AER_NULL_ARG);

	if (x) *x = inst->speedX;
	if (y) *y = inst->speedY;

	return;
#undef inst
}

void AERInstanceSetMotion(
		AERInstance * inst,
		float x,
		float y
) {
#define inst ((HLDInstance *)inst)
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	inst->speedX = x;
	inst->speedY = y;
	mre.refs.Instance_setMotionPolarFromCartesian(inst);

	return;
#undef inst
}

void AERInstanceAddMotion(
		AERInstance * inst,
		float x,
		float y
) {
#define inst ((HLDInstance *)inst)
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	inst->speedX += x;
	inst->speedY += y;
	mre.refs.Instance_setMotionPolarFromCartesian(inst);

	return;
#undef inst
}

int32_t AERInstanceGetMask(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1);
	ErrIf(!inst, AER_NULL_ARG, -1);

	return ((HLDInstance *)inst)->maskIndex;
}

void AERInstanceSetMask(
		AERInstance * inst,
		int32_t maskIdx
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	mre.refs.Instance_setMaskIndex(
			(HLDInstance *)inst,
			maskIdx
	);

	return;
}

int32_t AERInstanceGetSprite(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1);
	ErrIf(!inst, AER_NULL_ARG, -1);

	return ((HLDInstance *)inst)->spriteIndex;
}

void AERInstanceSetSprite(
		AERInstance * inst,
		int32_t spriteIdx
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	((HLDInstance *)inst)->spriteIndex = spriteIdx;

	return;
}

float AERInstanceGetSpriteFrame(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1.0f);
	ErrIf(!inst, AER_NULL_ARG, -1.0f);

	return ((HLDInstance *)inst)->imageIndex;
}

void AERInstanceSetSpriteFrame(
		AERInstance * inst,
		float frame
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	((HLDInstance *)inst)->imageIndex = frame;

	return;
}

float AERInstanceGetSpriteSpeed(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1.0f);
	ErrIf(!inst, AER_NULL_ARG, -1.0f);

	return ((HLDInstance *)inst)->imageSpeed;
}

void AERInstanceSetSpriteSpeed(
		AERInstance * inst,
		float speed
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	((HLDInstance *)inst)->imageSpeed = speed;

	return;
}

float AERInstanceGetSpriteAlpha(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1.0f);
	ErrIf(!inst, AER_NULL_ARG, -1.0f);

	return ((HLDInstance *)inst)->imageAlpha;
}

void AERInstanceSetSpriteAlpha(
		AERInstance * inst,
		float alpha
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	((HLDInstance *)inst)->imageAlpha = alpha;

	return;
}

float AERInstanceGetSpriteAngle(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, 0.0f);
	ErrIf(!inst, AER_NULL_ARG, 0.0f);

	return ((HLDInstance *)inst)->imageAngle;
}

void AERInstanceSetSpriteAngle(
		AERInstance * inst,
		float angle
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	((HLDInstance *)inst)->imageAngle = angle;

	return;
}

void AERInstanceGetSpriteScale(
		AERInstance * inst,
		float * x,
		float * y
) {
#define inst ((HLDInstance *)inst)
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);
	ErrIf(!(x || y), AER_NULL_ARG);

	if (x) *x = inst->imageScale.x;
	if (y) *y = inst->imageScale.y;

	return;
#undef inst
}

void AERInstanceSetSpriteScale(
		AERInstance * inst,
		float x,
		float y
) {
#define inst ((HLDInstance *)inst)
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	inst->imageScale.x = x;
	inst->imageScale.y = y;

	return;
#undef inst
}

bool AERInstanceGetTangible(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, false);
	ErrIf(!inst, AER_NULL_ARG, false);

	return ((HLDInstance *)inst)->tangible;
}

void AERInstanceSetTangible(
		AERInstance * inst,
		bool tangible
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	((HLDInstance *)inst)->tangible = tangible;

	return;
}

int32_t AERInstanceGetAlarm(
		AERInstance * inst,
		uint32_t alarmIdx
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1);
	ErrIf(!inst, AER_NULL_ARG, -1);
	ErrIf(alarmIdx >= 12, AER_FAILED_LOOKUP, -1);

	return ((HLDInstance *)inst)->alarms[alarmIdx];
}

void AERInstanceSetAlarm(
		AERInstance * inst,
		uint32_t alarmIdx,
		int32_t numSteps
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);
	ErrIf(alarmIdx >= 12, AER_FAILED_LOOKUP);

	((HLDInstance *)inst)->alarms[alarmIdx] = numSteps;

	return;
}
