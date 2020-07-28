#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aerlog.h"
#include "aermre.h"
#include "dynarr.h"
#include "eventkey.h"
#include "eventtrap.h"
#include "hashtab.h"
#include "hld.h"
#include "modman.h"
#include "objtree.h"
#include "utilmacs.h"



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

typedef struct HLDRefs {
	/* Globals. */
	int32_t * numSteps;
	bool (* keysPressedTable)[0x100];
	bool (* keysHeldTable)[0x100];
	bool (* keysReleasedTable)[0x100];
	int32_t * numRooms;
	int32_t * roomIndexCurrent;
	HLDRoom ** roomCurrent;
	HLDSprite *** spriteTable;
	HLDHashTable ** objectTableHandle;
	HLDHashTable * instanceTable;
	size_t (* alarmEventSubscriberCounts)[12];
	HLDEventSubscribers (* alarmEventSubscribers)[12];
	/* Functions. */
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
	__attribute__((cdecl)) int32_t (* actionObjectAdd)(void);
	__attribute__((cdecl)) int32_t (* actionEventPerform)(
			HLDInstance * target,
			HLDInstance * other,
			int32_t targetObjIdx,
			uint32_t eventType,
			int32_t eventNum
	);
	__attribute__((cdecl)) HLDInstance * (* gmlScriptSetdepth)(
			HLDInstance * target,
			HLDInstance * other,
			void * unknown0,
			uint32_t unknown1,
			uint32_t unknown2
	);
	__attribute__((cdecl)) HLDInstance * (* actionInstanceCreate)(
			int32_t objIdx,
			float posX,
			float posY
	);
	__attribute__((cdecl)) void (* actionInstanceDestroy)(
			HLDInstance * inst0,
			HLDInstance * inst1,
			int32_t objIdx,
			bool doEvent
	);
} HLDRefs;

typedef struct AERMRE {
	HLDRefs refs;
	int32_t roomIndexPrevious;
	ObjTree * objTree;
	AERMod * regActiveMod;
	DynArr * mods;
	DynArr * roomStepListeners;
	DynArr * roomChangeListeners;
	HashTab * eventTraps;
	EventKey currentEvent;
	HLDNamedFunction eventHandler;
	HashTab * eventSubscribers;
	enum {
		STAGE_INIT,
		STAGE_SPRITE_REG,
		STAGE_OBJECT_REG,
		STAGE_LISTENER_REG,
		STAGE_ACTION
	} stage;
} AERMRE;



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
	bool exists;
	EventTrap * trap = HashTabGet(mre.eventTraps, &mre.currentEvent, &exists);
	assert(exists);

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
			free(oldArr.elements);
		}
	} else {
		newArr = oldArr;
	}

	return newArr;
}

static EventTrap * EntrapEvent(
		HLDObject * obj,
		HLDEventType eventType,
		uint32_t eventNum
) {
	size_t numObjects = (*mre.refs.objectTableHandle)->numItems;
	HLDArrayPreSize oldArr, newArr;

	/* Get original event array. */
	oldArr = obj->eventListeners[eventType];

	/* Get new event array. */
	switch (eventType) {
		case HLD_EVENT_CREATE:
		case HLD_EVENT_DESTROY:
			newArr = ReallocEventArr(oldArr, 1);
			break;

		case HLD_EVENT_ALARM:
			newArr = ReallocEventArr(oldArr, 12);
			break;

		case HLD_EVENT_COLLISION:
			newArr = ReallocEventArr(oldArr, numObjects);
			break;

		case HLD_EVENT_OTHER:
			/* 
			 * We don't yet know the maximum number of other events in this
			 * version of the engine, so we're using 128 as a presumably safe
			 * upper bound until we learn the true maximum.
			 */
			newArr = ReallocEventArr(oldArr, 128);
			break;

		default:
			newArr = (HLDArrayPreSize){};
			break;
	}

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
	return EventTrapNew(
			eventType,
			(oldHandler) ? oldHandler->function : &PerformParentEvent
	);
}

static void RegisterObjectListener(
		HLDObject * obj,
		EventKey key,
		void * listener,
		bool downstream
) {
	bool exists;
	EventTrap * trap = HashTabGet(mre.eventTraps, &key, &exists);
	if (!exists) {
		trap = EntrapEvent(obj, key.type, key.num);
		HashTabInsert(mre.eventTraps, &key, trap);
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

static bool EnsureAlarmEventSubscriber(
		int32_t objIdx,
		void * context
) {
	EventKey key = (EventKey){
		.type = HLD_EVENT_ALARM,
		.num = *((uint32_t *)context),
		.objIdx = objIdx
	};
	if (!HashTabExists(mre.eventSubscribers, &key)) {
		uint32_t arrIdx = (*mre.refs.alarmEventSubscriberCounts)[key.num]++;
		(*mre.refs.alarmEventSubscribers)[key.num].objects[arrIdx] = objIdx;
		HashTabInsert(mre.eventSubscribers, &key, NULL);
	}

	return true;
}

static void WrapAlarmEventSubscribers(void) {
	size_t numObjs = (*mre.refs.objectTableHandle)->numItems;

	for (uint32_t alarmIdx = 0; alarmIdx < 12; alarmIdx++) {
		size_t oldSubCount = (
				*mre.refs.alarmEventSubscriberCounts
		)[alarmIdx];
		/*
		 * Note that original array is static, meaning it doesn't have to be freed
		 * and new mask array does have to be freed.
		 */
		int32_t * oldSubArr = (
				*mre.refs.alarmEventSubscribers
		)[alarmIdx].objects;

		int32_t * newSubArr = malloc(numObjs * sizeof(int32_t));
		assert(newSubArr);
		(*mre.refs.alarmEventSubscribers)[alarmIdx].objects = newSubArr;
		(*mre.refs.alarmEventSubscriberCounts)[alarmIdx] = 0;

		for (uint32_t subIdx = 0; subIdx < oldSubCount; subIdx++) {
			ObjTreeEach(
					mre.objTree,
					oldSubArr[subIdx],
					MAX_OBJ_TREE_DEPTH,
					&EnsureAlarmEventSubscriber,
					&alarmIdx
			);
		}
	}

	return;
}



/* ----- UNLISTED FUNCTIONS ----- */

__attribute__((cdecl)) void AERHookInit(HLDRefs refs) {
	/* Initialize MRE. */
	AERLogInfo(NAME, "Initializing mod runtime environment...");
	mre = (AERMRE){
		.refs = refs,
		.roomIndexPrevious = 0,
		.objTree = ObjTreeNew(),
		.regActiveMod = NULL,
		.mods = DynArrNew(16),
		.roomStepListeners = DynArrNew(16),
		.roomChangeListeners = DynArrNew(16),
		.eventTraps = HashTabNew(
				12, /* 4096 slots. */
				sizeof(EventKey),
				&EventKeyHash,
				&EventKeyEqual
		),
		.currentEvent = (EventKey){},
		.eventHandler = (HLDNamedFunction){
			.name = "AEREventHandler",
			.function = &CommonEventListener
		},
		.eventSubscribers = HashTabNew(
				10, /* 1024 slots. */
				sizeof(EventKey),
				&EventKeyHash,
				&EventKeyEqual
		),
		.stage = STAGE_INIT
	};
	AERLogInfo(NAME, "Done.");

	/* Load mods. */
	AERLogInfo(NAME, "Loading mods...");
	size_t modIdx = 0;
	while (true) {
		/* Load mod. */
		AERMod * mod;
		char nameBuf[16];
		snprintf(nameBuf, 16, MOD_NAME_FMT, modIdx);
		ModManErrCode err = ModManLoad(nameBuf, &mod);

		/* Valid mod. */
		if (err == MOD_MAN_OK) {
			DynArrPush(mre.mods, mod);
			if (mod->roomStepListener) {
				DynArrPush(mre.roomStepListeners, mod->roomStepListener);
			}
			if (mod->roomChangeListener) {
				DynArrPush(mre.roomChangeListeners, mod->roomChangeListener);
			}
			AERLogInfo(NAME, "Loaded mod \"%s\" v%s.", mod->name, mod->version);
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
							"Ran out of memory while loading mod %u.",
							modIdx
					);
					break;
			}
			exit(1);
		}

		modIdx++;
	}
	AERLogInfo(NAME, "Done. Loaded %u mod(s).", DynArrSize(mre.mods));

	return;
}

__attribute__((cdecl)) void AERHookUpdate(void) {
	/* Registration. */
	if (*mre.refs.numSteps == 0) {
		size_t numMods = DynArrSize(mre.mods);

		/* Register sprites. */
		mre.stage = STAGE_SPRITE_REG;
		AERLogInfo(NAME, "Registering mod sprites...");
		for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
			AERMod * mod = DynArrGet(mre.mods, modIdx);
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
			AERMod * mod = DynArrGet(mre.mods, modIdx);
			mre.regActiveMod = mod;
			if (mod->registerObjects) {
				mod->registerObjects();
				AERLogInfo(NAME, "Registred objects for mod \"%s.\"", mod->name);
			}
		}
		AERLogInfo(NAME, "Done.");

		/* Build object inheritance tree and wrap event subscribers. */
		BuildObjTree();
		WrapAlarmEventSubscribers();

		/* Register listeners. */
		mre.stage = STAGE_LISTENER_REG;
		AERLogInfo(NAME, "Registering mod event listeners...");
		for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
			AERMod * mod = DynArrGet(mre.mods, modIdx);
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
	}

	/* Check if room changed. */
	int32_t roomIndexCurrent = *mre.refs.roomIndexCurrent;
	if (roomIndexCurrent != mre.roomIndexPrevious) {
		/* Call room change listeners. */
		size_t numListeners = DynArrSize(mre.roomChangeListeners);
		for (uint32_t idx = 0; idx < numListeners; idx++) {
			void (* listener)(int32_t, int32_t) = DynArrGet(
					mre.roomChangeListeners,
					idx
			);
			listener(roomIndexCurrent, mre.roomIndexPrevious);
		}
		mre.roomIndexPrevious = roomIndexCurrent;
	}

	/* Call room step listeners. */
	size_t numListeners = DynArrSize(mre.roomStepListeners);
	for (uint32_t idx = 0; idx < numListeners; idx++) {
		void (* listener)(void) = DynArrGet(
				mre.roomStepListeners,
				idx
		);
		listener();
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
	size_t initNumMods = DynArrSize(mre.mods);
	size_t numUnloaded = 0;
	while (DynArrSize(mre.mods) > 0) {
		AERMod * mod = DynArrPop(mre.mods);
		const char * modName = mod->name;

		/* Unload mod. */
		if (ModManUnload(mod) == MOD_MAN_OK) {
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
		free((*mre.refs.alarmEventSubscribers)[idx].objects);
	}
	HashTabFree(mre.eventSubscribers);
	HashTabIter * iter = HashTabIterNew(mre.eventTraps);
	EventTrap * trap;
	while (HashTabIterNext(iter, NULL, (void **)&trap)) EventTrapFree(trap);
	HashTabIterFree(iter);
	HashTabFree(mre.eventTraps);
	DynArrFree(mre.roomChangeListeners);
	DynArrFree(mre.roomStepListeners);
	DynArrFree(mre.mods);
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
		int32_t depth,
		bool visible,
		bool solid,
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
	obj->depth = depth;
	obj->flags.visible = visible;
	obj->flags.solid = solid;
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
	EventKey key = (EventKey){
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
	EventKey key = (EventKey){
		.type = HLD_EVENT_ALARM,
		.num = alarmIdx,
		.objIdx = objIdx
	};
	RegisterObjectListener(obj, key, listener, downstream);
	ObjTreeEach(
			mre.objTree,
			objIdx,
			MAX_OBJ_TREE_DEPTH,
			&EnsureAlarmEventSubscriber,
			&alarmIdx
	);

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
	EventKey key = (EventKey){
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
	EventKey key = (EventKey){
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

AERErrCode AERGetCurrentRoom(int32_t * roomIdx) {
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

AERErrCode AERGetNumInstances(size_t * numInsts) {
	Stage(STAGE_ACTION);
	ArgGuard(numInsts);

	*numInsts = (*mre.refs.roomCurrent)->numInstances;

	return AER_OK;
}

AERErrCode AERGetNumInstancesByObject(
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

AERErrCode AERGetInstancesByObject(
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

AERErrCode AERInstanceGetSolid(
		AERInstance * inst,
		bool * solid
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(solid);

	*solid = ((HLDInstance *)inst)->solid;

	return AER_OK;
}

AERErrCode AERInstanceSetSolid(
		AERInstance * inst,
		bool solid
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	((HLDInstance *)inst)->solid = solid;

	return AER_OK;
}

AERErrCode AERInstanceGetAlarm(
		AERInstance * inst,
		uint32_t alarmIdx,
		int32_t * alarmSteps
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(alarmSteps);
	AlarmGuard(alarmIdx);

	*alarmSteps = ((HLDInstance *)inst)->alarms[alarmIdx];

	return AER_OK;
}

AERErrCode AERInstanceSetAlarm(
		AERInstance * inst,
		uint32_t alarmIdx,
		int32_t alarmSteps
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	AlarmGuard(alarmIdx);

	((HLDInstance *)inst)->alarms[alarmIdx] = alarmSteps;

	return AER_OK;
}
