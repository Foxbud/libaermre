#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "foxutils/arraymacs.h"
#include "foxutils/mapmacs.h"

#include "aer/log.h"
#include "aer/mre.h"
#include "private/eventkey.h"
#include "private/eventtrap.h"
#include "private/hld.h"
#include "private/modman.h"
#include "private/objtree.h"
#include "private/utilmacs.h"



/* ----- PRIVATE MACROS ----- */

#define WarnIf(cond, retCode, fmt, ...) \
	MacWrap( \
		if (cond) { \
			AERLogWarn(NAME, fmt, ##__VA_ARGS__); \
			return retCode; \
		} \
	)

#define Stage(stageId) \
	WarnIf( \
			mre.stage != (stageId), \
			AER_OUT_OF_SEQ, \
			"\"%s\" called outside of \"%s.\"", \
			__func__, \
			#stageId \
	)

#define ArgGuard(ptr) \
	WarnIf( \
			!(ptr), \
			AER_NULL_ARG, \
			"\"%s\" called with null reference for " \
			"argument \"%s.\"", \
			__func__, \
			#ptr \
	)

#define ObjectGuard(objPtr) \
	WarnIf( \
			!(objPtr), \
			AER_NO_SUCH_OBJECT, \
			"\"%s\" called with invalid object index.", \
			__func__ \
	)

#define InstanceGuard(instPtr) \
	WarnIf( \
			!(instPtr), \
			AER_NO_SUCH_INSTANCE, \
			"\"%s\" called with invalid instance ID.", \
			__func__ \
	)

#define MemoryGuard(ptr) \
	WarnIf( \
			!(ptr), \
			AER_OUT_OF_MEM, \
			"Ran out of memory during call to \"%s.\"", \
			__func__ \
	)

#define AlarmGuard(alarmIdx) \
	WarnIf( \
			(alarmIdx) >= 12, \
			AER_NO_SUCH_ALARM_EVENT, \
			"\"%s\" called with invalid alarm event index.", \
			__func__ \
	)

#define BufSizeGuard(ptr, bufSize, numWritten) \
	WarnIf( \
			!(ptr) && (numWritten) < (bufSize), \
			AER_BUF_SIZE_RECORD, \
			"\"%s\" could not completely fill buffer, and caller did not record " \
			"number of buffer elements written.", \
			__func__ \
	)



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
	 * Mod currently performing registration. Has no meaning after the
	 * registration stage.
	 */
	AERMod * regActiveMod;
	/* Array of all loaded mods. */
	FoxArray * mods;
	/* Array of registered listeners for the room step pseudo-event. */
	FoxArray * roomStepListeners;
	/* Array of registered listeners for the room change pseudo-event. */
	FoxArray * roomChangeListeners;
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

static const char * NAME = "MRE";

static const char * VERSION = "0.1.0dev";

static const char * MOD_NAME_FMT = "libaermod%u.so";

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
			AERLogErr(
					NAME,
					"\"%s\" called with unsupported event type %u.",
					__func__,
					eventType
			);
			exit(1);
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
		void * listener,
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
			AERLogErr(
					NAME,
					"\"%s\" called with unsupported event type %u.",
					__func__,
					key.type
			);
			exit(1);
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
	AERLogInfo(NAME, "Initializing mod runtime environment...");
	mre = (AERMRE){
		.refs = refs,
		.roomIndexPrevious = 0,
		.objTree = ObjTreeNew(),
		.regActiveMod = NULL,
		.mods = FoxArrayMNew(AERMod),
		.roomStepListeners = FoxArrayMNew(RoomStepListener),
		.roomChangeListeners = FoxArrayMNew(RoomChangeListener),
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
	AERLogInfo(NAME, "Done.");

	return;
}

static void LoadMods(void) {
	AERLogInfo(NAME, "Loading mods...");
	size_t modIdx = 0;
	while (true) {
		/* Load mod. */
		AERMod mod;
		char nameBuf[16];
		snprintf(nameBuf, 16, MOD_NAME_FMT, modIdx);
		ModManErrCode err = ModManLoad(nameBuf, &mod);

		/* Valid mod. */
		if (err == MOD_MAN_OK) {
			*FoxArrayMPush(AERMod, mre.mods) = mod;
			if (mod.roomStepListener) {
				*FoxArrayMPush(
						RoomStepListener,
						mre.roomStepListeners
				) = mod.roomStepListener;
			}
			if (mod.roomChangeListener) {
				*FoxArrayMPush(
						RoomChangeListener,
						mre.roomChangeListeners
				) = mod.roomChangeListener;
			}
			AERLogInfo(NAME, "Loaded mod \"%s\" v%s.", mod.name, mod.version);
		}

		/* No more mods. */
		else if (err == MOD_MAN_NO_SUCH_MOD) {
			break;
		}

		/* Error while loading. */
		else {
			switch (err) {
				case MOD_MAN_NAME_NOT_FOUND:
					AERLogErr(
							NAME,
							"Could not find symbol \"MOD_NAME\" in mod %u.",
							modIdx
					);
					break;
				case MOD_MAN_VERSION_NOT_FOUND:
					AERLogErr(
							NAME,
							"Could not find symbol \"MOD_VERSION\" in mod %u.",
							modIdx
					);
					break;
				default:
					AERLogErr(
							NAME,
							"Unknown error while loading mod %u.",
							modIdx
					);
					break;
			}
			exit(1);
		}

		modIdx++;
	}
	AERLogInfo(NAME, "Done. Loaded %u mod(s).", FoxArrayMSize(AERMod, mre.mods));

	return;
}

static void InitMods(void) {
	size_t numMods = FoxArrayMSize(AERMod, mre.mods);

	/* Register sprites. */
	mre.stage = STAGE_SPRITE_REG;
	AERLogInfo(NAME, "Registering mod sprites...");
	for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
		AERMod * mod = FoxArrayMIndex(AERMod, mre.mods, modIdx);
		mre.regActiveMod = mod;
		if (mod->registerSprites) {
			mod->registerSprites();
			AERLogInfo(NAME, "Registred sprites for mod \"%s.\"", mod->name);
		}
	}
	AERLogInfo(NAME, "Done.");

	/* Register objects. */
	mre.stage = STAGE_OBJECT_REG;
	AERLogInfo(NAME, "Registering mod objects...");
	for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
		AERMod * mod = FoxArrayMIndex(AERMod, mre.mods, modIdx);
		mre.regActiveMod = mod;
		if (mod->registerObjects) {
			mod->registerObjects();
			AERLogInfo(NAME, "Registred objects for mod \"%s.\"", mod->name);
		}
	}
	AERLogInfo(NAME, "Done.");

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
	AERLogInfo(NAME, "Registering mod event listeners...");
	for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
		AERMod * mod = FoxArrayMIndex(AERMod, mre.mods, modIdx);
		mre.regActiveMod = mod;
		if (mod->registerListeners) {
			mod->registerListeners();
			AERLogInfo(
					NAME,
					"Registred event listeners for mod \"%s.\"",
					mod->name
			);
		}
	}
	AERLogInfo(NAME, "Done.");

	mre.regActiveMod = NULL;
	mre.stage = STAGE_ACTION;

	return;
}



/* ----- UNLISTED FUNCTIONS ----- */

__attribute__((cdecl)) void AERHookInit(HLDRefs refs) {
	InitMRE(refs);
	LoadMods();
	InitMods();

	return;
}

__attribute__((cdecl)) void AERHookStep(void) {
	/* Check if room changed. */
	int32_t roomIndexCurrent = *mre.refs.roomIndexCurrent;
	if (roomIndexCurrent != mre.roomIndexPrevious) {
		/* Call room change listeners. */
		size_t numListeners = FoxArrayMSize(
				RoomChangeListener,
				mre.roomChangeListeners
		);
		for (uint32_t idx = 0; idx < numListeners; idx++) {
			(*FoxArrayMIndex(RoomChangeListener, mre.roomChangeListeners, idx))(
					roomIndexCurrent,
					mre.roomIndexPrevious
			);
		}
		mre.roomIndexPrevious = roomIndexCurrent;
	}

	/* Call room step listeners. */
	size_t numListeners = FoxArrayMSize(
			RoomStepListener,
			mre.roomStepListeners
	);
	for (uint32_t idx = 0; idx < numListeners; idx++) {
		(*FoxArrayMIndex(RoomStepListener, mre.roomStepListeners, idx))();
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
	AERLogInfo(
			NAME,
			"Action-Event-Response (AER) Mod Runtime Environment (MRE) v%s",
			VERSION
	);

	return;
}

__attribute__((destructor)) void AERDestructor(void) {
	AERLogInfo(NAME, "Unloading mods...");
	size_t initNumMods = FoxArrayMSize(AERMod, mre.mods);
	size_t numUnloaded = 0;
	while (FoxArrayMSize(AERMod, mre.mods) > 0) {
		AERMod mod = FoxArrayMPop(AERMod, mre.mods);
		const char * modName = mod.name;

		/* Unload mod. */
		if (ModManUnload(&mod) == MOD_MAN_OK) {
			numUnloaded++;
			AERLogInfo(NAME, "Unloaded mod \"%s.\"", modName);
		}

		/* Failure. */
		else {
			AERLogWarn(
					NAME,
					"Something went wrong while unloading mod \"%s.\"",
					modName
			);
		}
	}
	AERLogInfo(
			NAME,
			"Done. Unloaded %u of %u mod(s).",
			numUnloaded,
			initNumMods
	);

	AERLogInfo(NAME, "Deinitializing mod runtime environment...");
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

	FoxArrayMFree(RoomChangeListener, mre.roomChangeListeners);
	FoxArrayMFree(RoomStepListener, mre.roomStepListeners);
	FoxArrayMFree(AERMod, mre.mods);

	ObjTreeFree(mre.objTree);
	AERLogInfo(NAME, "Done.");

	return;
}



/* ----- PUBLIC FUNCTIONS ----- */

AERErrCode AERRegisterSprite(
		const char * filename,
		size_t numFrames,
		uint32_t origX,
		uint32_t origY,
		int32_t * spriteIdx
) {
	Stage(STAGE_SPRITE_REG);
	ArgGuard(filename);
	ArgGuard(spriteIdx);

	/* Construct asset path. */
	char pathBuf[256];
	snprintf(pathBuf, 256, ASSET_PATH_FMT, mre.regActiveMod->name, filename);

	*spriteIdx = mre.refs.actionSpriteAdd(
			pathBuf,
			numFrames,
			0,
			0,
			0,
			0,
			origX,
			origY
	);
	WarnIf(
			*spriteIdx < 0,
			AER_SPRITE_REG_FAILED,
			"Could not register sprite \"%s.\"",
			pathBuf
	);

	return AER_OK;
}

AERErrCode AERRegisterObject(
		const char * name,
		int32_t parentIdx,
		int32_t spriteIdx,
		int32_t maskIdx,
		int32_t depth,
		bool visible,
		bool collisions,
		bool persistent,
		int32_t * objIdx
) {
	Stage(STAGE_OBJECT_REG);
	ArgGuard(name);
	ArgGuard(objIdx);

	HLDObject * parent = ObjectLookup(parentIdx);
	ObjectGuard(parent);

	int32_t idx = mre.refs.actionObjectAdd();
	HLDObject * obj = ObjectLookup(idx);
	MemoryGuard(obj);

	/* The engine expects a freeable (dynamically allocated) string for name. */
	char * tmpName = malloc(strlen(name) + 1);
	MemoryGuard(tmpName);
	obj->name = strcpy(tmpName, name);
	obj->parentIndex = parentIdx;
	obj->parent = parent;
	obj->spriteIndex = spriteIdx;
	obj->maskIndex = maskIdx;
	obj->depth = depth;
	obj->flags.visible = visible;
	obj->flags.collisions = collisions;
	obj->flags.persistent = persistent;

	*objIdx = idx;

	return AER_OK;
}

AERErrCode AERRegisterCreateListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	Stage(STAGE_LISTENER_REG);
	ArgGuard(listener);

	HLDObject * obj = ObjectLookup(objIdx);
	ObjectGuard(obj);
	EventKey key = (EventKey){
		.type = HLD_EVENT_CREATE,
		.num = 0,
		.objIdx = objIdx
	};
	RegisterObjectListener(obj, key, listener, downstream);

	return AER_OK;
}

AERErrCode AERRegisterDestroyListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	Stage(STAGE_LISTENER_REG);
	ArgGuard(listener);

	HLDObject * obj = ObjectLookup(objIdx);
	ObjectGuard(obj);
	EventKey key = {
		.type = HLD_EVENT_DESTROY,
		.num = 0,
		.objIdx = objIdx
	};
	RegisterObjectListener(obj, key, listener, downstream);

	return AER_OK;
}

AERErrCode AERRegisterAlarmListener(
		int32_t objIdx,
		uint32_t alarmIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	Stage(STAGE_LISTENER_REG);
	ArgGuard(listener);
	AlarmGuard(alarmIdx);

	HLDObject * obj = ObjectLookup(objIdx);
	ObjectGuard(obj);
	EventKey key = {
		.type = HLD_EVENT_ALARM,
		.num = alarmIdx,
		.objIdx = objIdx
	};
	RegisterObjectListener(obj, key, listener, downstream);
	RegisterEventSubscriber(key);

	return AER_OK;
}

AERErrCode AERRegisterStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	Stage(STAGE_LISTENER_REG);
	ArgGuard(listener);

	HLDObject * obj = ObjectLookup(objIdx);
	ObjectGuard(obj);
	EventKey key = {
		.type = HLD_EVENT_STEP,
		.num = HLD_EVENT_STEP_INLINE,
		.objIdx = objIdx
	};
	RegisterObjectListener(obj, key, listener, downstream);
	RegisterEventSubscriber(key);

	return AER_OK;
}

AERErrCode AERRegisterPreStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	Stage(STAGE_LISTENER_REG);
	ArgGuard(listener);

	HLDObject * obj = ObjectLookup(objIdx);
	ObjectGuard(obj);
	EventKey key = {
		.type = HLD_EVENT_STEP,
		.num = HLD_EVENT_STEP_PRE,
		.objIdx = objIdx
	};
	RegisterObjectListener(obj, key, listener, downstream);
	RegisterEventSubscriber(key);

	return AER_OK;
}

AERErrCode AERRegisterPostStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	Stage(STAGE_LISTENER_REG);
	ArgGuard(listener);

	HLDObject * obj = ObjectLookup(objIdx);
	ObjectGuard(obj);
	EventKey key = {
		.type = HLD_EVENT_STEP,
		.num = HLD_EVENT_STEP_POST,
		.objIdx = objIdx
	};
	RegisterObjectListener(obj, key, listener, downstream);
	RegisterEventSubscriber(key);

	return AER_OK;
}

AERErrCode AERRegisterCollisionListener(
		int32_t targetObjIdx,
		int32_t otherObjIdx,
		bool (* listener)(AERInstance * target, AERInstance * other),
		bool downstream
) {
	Stage(STAGE_LISTENER_REG);
	ArgGuard(listener);
	ObjectGuard(ObjectLookup(otherObjIdx));

	HLDObject * obj = ObjectLookup(targetObjIdx);
	ObjectGuard(obj);
	EventKey key = {
		.type = HLD_EVENT_COLLISION,
		.num = otherObjIdx,
		.objIdx = targetObjIdx
	};
	RegisterObjectListener(obj, key, listener, downstream);

	return AER_OK;
}

AERErrCode AERRegisterAnimationEndListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	Stage(STAGE_LISTENER_REG);
	ArgGuard(listener);

	HLDObject * obj = ObjectLookup(objIdx);
	ObjectGuard(obj);
	EventKey key = {
		.type = HLD_EVENT_OTHER,
		.num = HLD_EVENT_OTHER_ANIMATION_END,
		.objIdx = objIdx
	};
	RegisterObjectListener(obj, key, listener, downstream);

	return AER_OK;
}

AERErrCode AERGetNumSteps(uint32_t * numSteps) {
	Stage(STAGE_ACTION);
	ArgGuard(numSteps);

	*numSteps = *mre.refs.numSteps;

	return AER_OK;
}

AERErrCode AERGetKeysPressed(const bool ** keys) {
	Stage(STAGE_ACTION);
	ArgGuard(keys);

	*keys = *mre.refs.keysPressedTable;

	return AER_OK;
}

AERErrCode AERGetKeysHeld(const bool ** keys) {
	Stage(STAGE_ACTION);
	ArgGuard(keys);

	*keys = *mre.refs.keysHeldTable;

	return AER_OK;
}

AERErrCode AERGetKeysReleased(const bool ** keys) {
	Stage(STAGE_ACTION);
	ArgGuard(keys);

	*keys = *mre.refs.keysReleasedTable;

	return AER_OK;
}

AERErrCode AERRoomGetCurrent(int32_t * roomIdx) {
	Stage(STAGE_ACTION);
	ArgGuard(roomIdx);

	*roomIdx = *mre.refs.roomIndexCurrent;

	return AER_OK;
}

AERErrCode AERObjectGetName(
		int32_t objIdx,
		const char ** name
) {
	Stage(STAGE_ACTION);
	ArgGuard(name);

	HLDObject * obj = ObjectLookup(objIdx);
	ObjectGuard(obj);
	*name = obj->name;

	return AER_OK;
}

AERErrCode AERObjectGetParent(
		int32_t objIdx,
		int32_t * parentIdx
) {
	Stage(STAGE_ACTION);
	ArgGuard(parentIdx);

	HLDObject * obj = ObjectLookup(objIdx);
	ObjectGuard(obj);
	*parentIdx = obj->parentIndex;

	return AER_OK;
}

AERErrCode AERObjectGetNumInstances(
		int32_t objIdx,
		size_t * numInsts
) {
	Stage(STAGE_ACTION);
	ArgGuard(numInsts);

	HLDObject * obj = ObjectLookup(objIdx);
	ObjectGuard(obj);
	*numInsts = obj->numInstances;

	return AER_OK;
}

AERErrCode AERObjectGetInstances(
		int32_t objIdx,
		size_t bufSize,
		AERInstance ** instBuf,
		size_t * numInsts
) {
	Stage(STAGE_ACTION);
	ArgGuard(instBuf);

	HLDObject * obj = ObjectLookup(objIdx);
	ObjectGuard(obj);

	size_t numAvail = obj->numInstances;
	size_t numToWrite = Min(numAvail, bufSize);
	HLDNodeDLL * node = obj->instanceFirst;
	for (size_t idx = 0; idx < numToWrite; idx++) {
		instBuf[idx] = (AERInstance *)node->item;
		node = node->next;
	}

	if (numInsts) *numInsts = numAvail;
	BufSizeGuard(numInsts, bufSize, numToWrite);

	return AER_OK;
}

AERErrCode AERObjectGetCollisions(
		int32_t objIdx,
		bool * collisions
) {
	Stage(STAGE_ACTION);
	ArgGuard(collisions);

	HLDObject * obj = ObjectLookup(objIdx);
	ObjectGuard(obj);
	*collisions = obj->flags.collisions;

	return AER_OK;
}

AERErrCode AERObjectSetCollisions(
		int32_t objIdx,
		bool collisions
) {
	Stage(STAGE_ACTION);

	HLDObject * obj = ObjectLookup(objIdx);
	ObjectGuard(obj);
	obj->flags.collisions = collisions;

	return AER_OK;
}

AERErrCode AERGetNumInstances(size_t * numInsts) {
	Stage(STAGE_ACTION);
	ArgGuard(numInsts);

	*numInsts = (*mre.refs.roomCurrent)->numInstances;

	return AER_OK;
}

AERErrCode AERGetInstances(
		size_t bufSize,
		AERInstance ** instBuf,
		size_t * numInsts
) {
	Stage(STAGE_ACTION);
	ArgGuard(instBuf);

	HLDRoom * room = *mre.refs.roomCurrent;
	size_t numAvail = room->numInstances;
	size_t numToWrite = Min(numAvail, bufSize);
	HLDInstance * inst = room->instanceFirst;
	for (size_t idx = 0; idx < numToWrite; idx++) {
		instBuf[idx] = (AERInstance *)inst;
		inst = inst->instanceNext;
	}

	if (numInsts) *numInsts = numAvail;
	BufSizeGuard(numInsts, bufSize, numToWrite);

	return AER_OK;
}

AERErrCode AERGetInstanceById(
		int32_t instId,
		AERInstance ** inst
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	*inst = (AERInstance *)InstanceLookup(instId);
	InstanceGuard(*inst);

	return AER_OK;
}

AERErrCode AERInstanceCreate(
		int32_t objIdx,
		float x,
		float y,
		AERInstance ** inst
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ObjectGuard(ObjectLookup(objIdx));

	*inst = (AERInstance *)mre.refs.actionInstanceCreate(objIdx, x, y);
	MemoryGuard(*inst);

	return AER_OK;
}

AERErrCode AERInstanceChange(
		AERInstance * inst,
		int32_t newObjIdx,
		bool doEvents
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ObjectGuard(ObjectLookup(newObjIdx));

	mre.refs.actionInstanceChange(
			(HLDInstance *)inst,
			newObjIdx,
			doEvents
	);

	return AER_OK;
}

AERErrCode AERInstanceDestroy(AERInstance * inst) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	mre.refs.actionInstanceDestroy(
			(HLDInstance *)inst,
			(HLDInstance *)inst,
			-1,
			true
	);

	return AER_OK;
}

AERErrCode AERInstanceDelete(AERInstance * inst) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	mre.refs.actionInstanceDestroy(
			(HLDInstance *)inst,
			(HLDInstance *)inst,
			-1,
			false
	);

	return AER_OK;
}

AERErrCode AERInstanceSyncDepth(AERInstance * inst) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	uint32_t unknownDatastructure[4] = {};
	mre.refs.gmlScriptSetdepth(
			(HLDInstance *)inst,
			(HLDInstance *)inst,
			&unknownDatastructure,
			0,
			0
	);

	return AER_OK;
}

AERErrCode AERInstanceGetId(
		AERInstance * inst,
		int32_t * instId
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(instId);

	*instId = ((HLDInstance *)inst)->id;

	return AER_OK;
}

AERErrCode AERInstanceGetObject(
		AERInstance * inst,
		int32_t * objIdx
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(objIdx);

	*objIdx = ((HLDInstance *)inst)->objectIndex;

	return AER_OK;
}

AERErrCode AERInstanceGetPosition(
		AERInstance * inst,
		float * x,
		float * y
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(x || y);

	if (x) *x = ((HLDInstance *)inst)->pos.x;
	if (y) *y = ((HLDInstance *)inst)->pos.y;

	return AER_OK;
}

AERErrCode AERInstanceSetPosition(
		AERInstance * inst,
		float x,
		float y
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	((HLDInstance *)inst)->pos.x = x;
	((HLDInstance *)inst)->pos.y = y;

	return AER_OK;
}

AERErrCode AERInstanceGetFriction(
		AERInstance * inst,
		float * friction
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(friction);

	*friction = ((HLDInstance *)inst)->friction;

	return AER_OK;
}

AERErrCode AERInstanceSetFriction(
		AERInstance * inst,
		float friction
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	((HLDInstance *)inst)->friction = friction;

	return AER_OK;
}

AERErrCode AERInstanceGetMotion(
		AERInstance * inst,
		float * x,
		float * y
) {
#define inst ((HLDInstance *)inst)
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(x || y);

	if (x) *x = inst->speedX;
	if (y) *y = inst->speedY;

	return AER_OK;
#undef inst
}

AERErrCode AERInstanceSetMotion(
		AERInstance * inst,
		float x,
		float y
) {
#define inst ((HLDInstance *)inst)
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	inst->speedX = x;
	inst->speedY = y;
	mre.refs.Instance_setMotionPolarFromCartesian(inst);

	return AER_OK;
#undef inst
}

AERErrCode AERInstanceAddMotion(
		AERInstance * inst,
		float x,
		float y
) {
#define inst ((HLDInstance *)inst)
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	inst->speedX += x;
	inst->speedY += y;
	mre.refs.Instance_setMotionPolarFromCartesian(inst);

	return AER_OK;
#undef inst
}

AERErrCode AERInstanceGetMask(
		AERInstance * inst,
		int32_t * maskIdx
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(maskIdx);

	*maskIdx = ((HLDInstance *)inst)->maskIndex;

	return AER_OK;
}

AERErrCode AERInstanceSetMask(
		AERInstance * inst,
		int32_t maskIdx
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	mre.refs.Instance_setMaskIndex(
			(HLDInstance *)inst,
			maskIdx
	);

	return AER_OK;
}

AERErrCode AERInstanceGetSprite(
		AERInstance * inst,
		int32_t * spriteIdx
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(spriteIdx);

	*spriteIdx = ((HLDInstance *)inst)->spriteIndex;

	return AER_OK;
}

AERErrCode AERInstanceSetSprite(
		AERInstance * inst,
		int32_t spriteIdx
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	((HLDInstance *)inst)->spriteIndex = spriteIdx;

	return AER_OK;
}

AERErrCode AERInstanceGetSpriteFrame(
		AERInstance * inst,
		float * frame
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(frame);

	*frame = ((HLDInstance *)inst)->imageIndex;

	return AER_OK;
}

AERErrCode AERInstanceSetSpriteFrame(
		AERInstance * inst,
		float frame
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	((HLDInstance *)inst)->imageIndex = frame;

	return AER_OK;
}

AERErrCode AERInstanceGetSpriteSpeed(
		AERInstance * inst,
		float * speed
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(speed);

	*speed = ((HLDInstance *)inst)->imageSpeed;

	return AER_OK;
}

AERErrCode AERInstanceSetSpriteSpeed(
		AERInstance * inst,
		float speed
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	((HLDInstance *)inst)->imageSpeed = speed;

	return AER_OK;
}

AERErrCode AERInstanceGetSpriteAlpha(
		AERInstance * inst,
		float * alpha
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(alpha);

	*alpha = ((HLDInstance *)inst)->imageAlpha;

	return AER_OK;
}

AERErrCode AERInstanceSetSpriteAlpha(
		AERInstance * inst,
		float alpha
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	((HLDInstance *)inst)->imageAlpha = alpha;

	return AER_OK;
}

AERErrCode AERInstanceGetSpriteAngle(
		AERInstance * inst,
		float * angle
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(angle);

	*angle = ((HLDInstance *)inst)->imageAngle;

	return AER_OK;
}

AERErrCode AERInstanceSetSpriteAngle(
		AERInstance * inst,
		float angle
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	((HLDInstance *)inst)->imageAngle = angle;

	return AER_OK;
}

AERErrCode AERInstanceGetSpriteScale(
		AERInstance * inst,
		float * x,
		float * y
) {
#define inst ((HLDInstance *)inst)
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(x || y);

	if (x) *x = inst->imageScale.x;
	if (y) *y = inst->imageScale.y;

	return AER_OK;
#undef inst
}

AERErrCode AERInstanceSetSpriteScale(
		AERInstance * inst,
		float x,
		float y
) {
#define inst ((HLDInstance *)inst)
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	inst->imageScale.x = x;
	inst->imageScale.y = y;

	return AER_OK;
#undef inst
}

AERErrCode AERInstanceGetTangible(
		AERInstance * inst,
		bool * tangible
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(tangible);

	*tangible = ((HLDInstance *)inst)->tangible;

	return AER_OK;
}

AERErrCode AERInstanceSetTangible(
		AERInstance * inst,
		bool tangible
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	((HLDInstance *)inst)->tangible = tangible;

	return AER_OK;
}

AERErrCode AERInstanceGetAlarm(
		AERInstance * inst,
		uint32_t alarmIdx,
		int32_t * numSteps
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(numSteps);
	AlarmGuard(alarmIdx);

	*numSteps = ((HLDInstance *)inst)->alarms[alarmIdx];

	return AER_OK;
}

AERErrCode AERInstanceSetAlarm(
		AERInstance * inst,
		uint32_t alarmIdx,
		int32_t numSteps
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	AlarmGuard(alarmIdx);

	((HLDInstance *)inst)->alarms[alarmIdx] = numSteps;

	return AER_OK;
}
