#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aerlog.h"
#include "aermodman.h"
#include "aermre.h"
#include "hld.h"



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
	HLDHashTable ** objectTableHandle;
	HLDHashTable * instanceTable;
	/* Functions. */
	__attribute__((cdecl)) int32_t (* actionObjectAdd)();
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
} HLDRefs;

typedef struct AERMRE {
	HLDRefs refs;
	int32_t roomIndexPrevious;
	size_t numMods;
	AERMod * mods[128];
	AERMod * regActiveMod;
	size_t numRoomStepCallbacks;
	void (* roomStepCallbacks[128])(void);
	size_t numRoomChangeCallbacks;
	void (* roomChangeCallbacks[128])(int32_t, int32_t);
	enum {
		STAGE_INIT,
		STAGE_REG_SPRITE,
		STAGE_REG_OBJECT,
		STAGE_ACTION
	} stage;
} AERMRE;



/* ----- PRIVATE CONSTANTS ----- */

static const char * NAME = "MRE";

static const char * VERSION = "0.1.0dev";

static const size_t MAX_NUM_MODS = 128;

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



/* ----- UNLISTED FUNCTIONS ----- */

__attribute__((cdecl)) void AERHookInit(HLDRefs refs) {
	/* Initialize MRE. */
	AERLogInfo(NAME, "Initializing mod runtime environment...");
	mre = (AERMRE){
		.refs = refs,
		.roomIndexPrevious = 0,
		.numMods = 0,
		.mods = {},
		.regActiveMod = NULL,
		.numRoomStepCallbacks = 0,
		.roomStepCallbacks = {},
		.numRoomChangeCallbacks = 0,
		.roomChangeCallbacks = {},
		.stage = STAGE_INIT
	};
	AERLogInfo(NAME, "Done.");

	/* Load mods. */
	AERLogInfo(NAME, "Loading mods...");
	for (size_t modIdx = 0; modIdx < MAX_NUM_MODS; modIdx++) {
		/* Load mod. */
		AERMod * mod;
		char nameBuf[16];
		snprintf(nameBuf, 16, MOD_NAME_FMT, modIdx);
		AERModManErrCode err = AERModManLoad(nameBuf, &mod);

		/* Valid mod. */
		if (err == AER_MOD_MAN_OK) {
			mre.mods[modIdx] = mod;
			mre.numMods++;
			if (mod->roomStepCallback) {
				size_t tmpIdx = mre.numRoomStepCallbacks;
				mre.roomStepCallbacks[tmpIdx] = mod->roomStepCallback;
				mre.numRoomStepCallbacks++;
			}
			if (mod->roomChangeCallback) {
				size_t tmpIdx = mre.numRoomChangeCallbacks;
				mre.roomChangeCallbacks[tmpIdx] = mod->roomChangeCallback;
				mre.numRoomChangeCallbacks++;
			}
			AERLogInfo(NAME, "Loaded mod \"%s\" v%s.", mod->name, mod->version);
		}

		/* No more mods. */
		else if (err == AER_MOD_MAN_NO_SUCH_MOD) {
			break;
		}

		/* Error while loading. */
		else {
			switch (err) {
				case AER_MOD_MAN_NAME_NOT_FOUND:
					AERLogErr(
							NAME,
							"Could not find symbol \"MOD_NAME\" in mod %u.",
							modIdx
					);
					break;
				case AER_MOD_MAN_VERSION_NOT_FOUND:
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
	}
	AERLogInfo(NAME, "Done. Loaded %u mod(s).", mre.numMods);

	return;
}

__attribute__((cdecl)) void AERHookUpdate(void) {
	/* Registration. */
	if (*mre.refs.numSteps == 0) {
		/* Register sprites. */
		mre.stage = STAGE_REG_SPRITE;
		AERLogInfo(NAME, "Registering mod sprites...");
		for (uint32_t modIdx = 0; modIdx < mre.numMods; modIdx++) {
			AERMod * mod = mre.mods[modIdx];
			mre.regActiveMod = mod;
			if (mod->registerSpritesCallback) {
				mod->registerSpritesCallback();
				AERLogInfo(NAME, "Registred sprites for mod \"%s.\"", mod->name);
			}
		}
		AERLogInfo(NAME, "Done.");

		/* Register objects. */
		mre.stage = STAGE_REG_OBJECT;
		AERLogInfo(NAME, "Registering mod objects...");
		for (uint32_t modIdx = 0; modIdx < mre.numMods; modIdx++) {
			AERMod * mod = mre.mods[modIdx];
			mre.regActiveMod = mod;
			if (mod->registerObjectsCallback) {
				mod->registerObjectsCallback();
				AERLogInfo(NAME, "Registred objects for mod \"%s.\"", mod->name);
			}
		}
		AERLogInfo(NAME, "Done.");

		mre.regActiveMod = NULL;
		mre.stage = STAGE_ACTION;
	}

	/* Check if room changed. */
	int32_t roomIndexCurrent = *mre.refs.roomIndexCurrent;
	if (roomIndexCurrent != mre.roomIndexPrevious) {
		/* Call room change mod callbacks. */
		for (uint32_t idx = 0; idx < mre.numRoomChangeCallbacks; idx++) {
			mre.roomChangeCallbacks[idx](
					roomIndexCurrent,
					mre.roomIndexPrevious
			);
		}
		mre.roomIndexPrevious = roomIndexCurrent;
	}

	/* Call room step mod callbacks. */
	for (uint32_t idx = 0; idx < mre.numRoomStepCallbacks; idx++) {
		mre.roomStepCallbacks[idx]();
	}

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
	size_t initNumMods = mre.numMods;
	for (uint32_t modIdx = 0; modIdx < initNumMods; modIdx++) {
		AERMod * mod = mre.mods[modIdx];
		const char * modName = mod->name;

		/* Unload mod. */
		if (AERModManUnload(mod) == AER_MOD_MAN_OK) {
			mre.numMods--;
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
			initNumMods - mre.numMods,
			initNumMods
	);

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
	Stage(STAGE_REG_SPRITE);
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
		bool visible,
		bool solid,
		bool collisions,
		bool persistent,
		int32_t * objIdx
) {
	Stage(STAGE_REG_OBJECT);
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
	obj->flags.visible = visible;
	obj->flags.solid = solid;
	obj->flags.collisions = collisions;
	obj->flags.persistent = persistent;

	*objIdx = idx;

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
		uint32_t * frame
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);
	ArgGuard(frame);

	*frame = (uint32_t)((HLDInstance *)inst)->imageIndex;

	return AER_OK;
}

AERErrCode AERInstanceSetSpriteFrame(
		AERInstance * inst,
		uint32_t frame
) {
	Stage(STAGE_ACTION);
	ArgGuard(inst);

	((HLDInstance *)inst)->imageIndex = (float)frame;

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
