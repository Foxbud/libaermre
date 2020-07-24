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



/* ----- PRIVATE MACROS ----- */

#define WarnIf(cond, retCode, fmt, ...) \
	do { \
		if (cond) { \
			AERLogWarn(NAME, fmt, ##__VA_ARGS__); \
			return retCode; \
		} \
	} while (false)

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
	AERMod * regActiveMod;
	DynArr * mods;
	DynArr * roomStepListeners;
	DynArr * roomChangeListeners;
	HashTab * eventTraps;
	EventKey currentEvent;
	HLDNamedFunction eventHandler;
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

static const char * MOD_NAME_FMT = "aermod%u.so";

static const char * ASSET_PATH_FMT = "assets/mod/%s/%s";



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
	HLDObject * obj = ObjectLookup(target->objectIndex);
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

static EventTrap * EntrapEvent(
		HLDObject * obj,
		HLDEventType eventType,
		uint32_t eventNum
) {
	size_t numObjects = (*mre.refs.objectTableHandle)->numItems;
	HLDArrayPreSize oldArr, newArr;

	/* Get original event array. */
	switch (eventType) {
		case HLD_EVENT_CREATE:
			oldArr = obj->createEvents;
			break;
		case HLD_EVENT_DESTROY:
			oldArr = obj->destroyEvents;
			break;
		case HLD_EVENT_COLLISION:
			oldArr = obj->collisionEvents;
			break;
		default:
			oldArr = (HLDArrayPreSize){};
			break;
	}

	/* Get new event array. */
	switch (eventType) {
		case HLD_EVENT_CREATE:
		case HLD_EVENT_DESTROY:
			if (oldArr.size < 1) {
				newArr = (HLDArrayPreSize){
					.size = 1,
					.elements = calloc(1, sizeof(HLDEventWrapper *))
				};
				assert(newArr.elements);
			} else {
				newArr = oldArr;
			}
			break;

		case HLD_EVENT_COLLISION:
			if (oldArr.size < numObjects) {
				newArr = (HLDArrayPreSize){
					.size = numObjects,
					.elements = calloc(numObjects, sizeof(HLDEventWrapper *))
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
			break;

		default:
			newArr = (HLDArrayPreSize){};
			break;
	}

	/* Update object with new event array. */
	switch (eventType) {
		case HLD_EVENT_CREATE:
			obj->createEvents = newArr;
			break;
		case HLD_EVENT_DESTROY:
			obj->destroyEvents = newArr;
			break;
		case HLD_EVENT_COLLISION:
			obj->collisionEvents = newArr;
			break;
		default:
			break;
	}

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



/* ----- UNLISTED FUNCTIONS ----- */

__attribute__((cdecl)) void AERHookInit(HLDRefs refs) {
	/* Initialize MRE. */
	AERLogInfo(NAME, "Initializing mod runtime environment...");
	mre = (AERMRE){
		.refs = refs,
		.roomIndexPrevious = 0,
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
		HLDInstance * target,
		HLDInstance * other,
		int32_t targetObjIdx,
		HLDEventType eventType,
		int32_t eventNum
) {
	(void)target;
	(void)other;

	mre.currentEvent = (EventKey){
		.type = eventType,
		.num = eventNum,
		.objIdx = targetObjIdx
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
	HashTabIter iter = HashTabGetIter(mre.eventTraps);
	EventTrap * trap;
	while (HashTabIterNext(&iter, NULL, (void **)&trap)) EventTrapFree(trap);
	HashTabFree(mre.eventTraps);
	DynArrFree(mre.roomChangeListeners);
	DynArrFree(mre.roomStepListeners);
	size_t numMods = DynArrSize(mre.mods);
	for (uint32_t idx = 0; idx < numMods; idx++) {
		free(DynArrGet(mre.mods, idx));
	}
	DynArrFree(mre.mods);
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
	ArgGuard(numInsts);

	HLDRoom * room = *mre.refs.roomCurrent;
	size_t numAvail = room->numInstances;
	size_t numToWrite = (numAvail < bufSize) ? numAvail : bufSize;
	HLDInstance * inst = room->instanceFirst;
	for (size_t idx = 0; idx < numToWrite; idx++) {
		instBuf[idx] = (AERInstance *)inst;
		inst = inst->instanceNext;
	}
	*numInsts = numToWrite;

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
	ArgGuard(numInsts);

	HLDObject * obj = ObjectLookup(objIdx);
	ObjectGuard(obj);

	size_t numAvail = obj->numInstances;
	size_t numToWrite = (numAvail < bufSize) ? numAvail : bufSize;
	HLDNodeDLL * node = obj->instanceFirst;
	for (size_t idx = 0; idx < numToWrite; idx++) {
		instBuf[idx] = (AERInstance *)node->item;
		node = node->next;
	}
	*numInsts = numToWrite;

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
	ArgGuard(x);
	ArgGuard(y);

	*x = ((HLDInstance *)inst)->pos.x;
	*y = ((HLDInstance *)inst)->pos.y;

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

AERErrCode AERInstanceGetAge(
		AERInstance * inst,
		uint32_t * age
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(age);

	*age = ((HLDInstance *)inst)->age;

	return AER_OK;
}
