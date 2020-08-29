#include <stdlib.h>
#include <string.h>

#include "foxutils/arraymacs.h"
#include "foxutils/math.h"

#include "aer/object.h"
#include "internal/err.h"
#include "internal/eventkey.h"
#include "internal/hld.h"
#include "internal/log.h"
#include "internal/modman.h"
#include "internal/mre.h"



/* ----- PUBLIC FUNCTIONS ----- */

int32_t AERObjectRegister(
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

	HLDObject * parent = HLDObjectLookup(parentIdx);
	ErrIf(!parent, AER_FAILED_LOOKUP, -1);

	int32_t objIdx = hldfuncs.actionObjectAdd();
	HLDObject * obj = HLDObjectLookup(objIdx);
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

const char * AERObjectGetName(int32_t objIdx) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP, NULL);

	return obj->name;
}

int32_t AERObjectGetParent(int32_t objIdx) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1);

	HLDObject * obj = HLDObjectLookup(objIdx);
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

	HLDObject * obj = HLDObjectLookup(objIdx);
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

	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP, false);

	return obj->flags.collisions;
}

void AERObjectSetCollisions(
		int32_t objIdx,
		bool collisions
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);

	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);
	obj->flags.collisions = collisions;

	return;
}

void AERObjectAttachCreateListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	LogInfo(
			"Attaching create listener to object %i for mod \"%s\"...",
			objIdx,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_LISTENER_REG, AER_SEQ_BREAK);
	ErrIf(!listener, AER_NULL_ARG);

	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);

	EventKey key = (EventKey){
		.type = HLD_EVENT_CREATE,
		.num = 0,
		.objIdx = objIdx
	};
	MRERegisterEventListener(
			obj,
			key,
			(ModListener){
				.mod = *FoxArrayMPeek(Mod *, &modman.context),
				.func.aerObj = listener
			},
			downstream
	);

	LogInfo("Successfully attached create listener.");
	return;
}

void AERObjectAttachDestroyListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	LogInfo(
			"Attaching destroy listener to object %i for mod \"%s\"...",
			objIdx,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_LISTENER_REG, AER_SEQ_BREAK);
	ErrIf(!listener, AER_NULL_ARG);

	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);

	EventKey key = {
		.type = HLD_EVENT_DESTROY,
		.num = 0,
		.objIdx = objIdx
	};
	MRERegisterEventListener(
			obj,
			key,
			(ModListener){
				.mod = *FoxArrayMPeek(Mod *, &modman.context),
				.func.aerObj = listener
			},
			downstream
	);

	LogInfo("Successfully attached destroy listener.");
	return;
}

void AERObjectAttachAlarmListener(
		int32_t objIdx,
		uint32_t alarmIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	LogInfo(
			"Attaching alarm %u listener to object %i for mod \"%s\"...",
			alarmIdx,
			objIdx,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_LISTENER_REG, AER_SEQ_BREAK);
	ErrIf(!listener, AER_NULL_ARG);
	ErrIf(alarmIdx >= 12, AER_FAILED_LOOKUP);

	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);

	EventKey key = {
		.type = HLD_EVENT_ALARM,
		.num = alarmIdx,
		.objIdx = objIdx
	};
	MRERegisterEventListener(
			obj,
			key,
			(ModListener){
				.mod = *FoxArrayMPeek(Mod *, &modman.context),
				.func.aerObj = listener
			},
			downstream
	);

	LogInfo("Successfully attached alarm %u listener.", alarmIdx);
	return;
}

void AERObjectAttachStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	LogInfo(
			"Attaching step listener to object %i for mod \"%s\"...",
			objIdx,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_LISTENER_REG, AER_SEQ_BREAK);
	ErrIf(!listener, AER_NULL_ARG);

	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);

	EventKey key = {
		.type = HLD_EVENT_STEP,
		.num = HLD_EVENT_STEP_INLINE,
		.objIdx = objIdx
	};
	MRERegisterEventListener(
			obj,
			key,
			(ModListener){
				.mod = *FoxArrayMPeek(Mod *, &modman.context),
				.func.aerObj = listener
			},
			downstream
	);

	LogInfo("Successfully attached step listener.");
	return;
}

void AERObjectAttachPreStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	LogInfo(
			"Attaching pre-step listener to object %i for mod \"%s\"...",
			objIdx,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_LISTENER_REG, AER_SEQ_BREAK);
	ErrIf(!listener, AER_NULL_ARG);

	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);

	EventKey key = {
		.type = HLD_EVENT_STEP,
		.num = HLD_EVENT_STEP_PRE,
		.objIdx = objIdx
	};
	MRERegisterEventListener(
			obj,
			key,
			(ModListener){
				.mod = *FoxArrayMPeek(Mod *, &modman.context),
				.func.aerObj = listener
			},
			downstream
	);

	LogInfo("Successfully attached pre-step listener.");
	return;
}

void AERObjectAttachPostStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	LogInfo(
			"Attaching post-step listener to object %i for mod \"%s\"...",
			objIdx,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_LISTENER_REG, AER_SEQ_BREAK);
	ErrIf(!listener, AER_NULL_ARG);

	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);

	EventKey key = {
		.type = HLD_EVENT_STEP,
		.num = HLD_EVENT_STEP_POST,
		.objIdx = objIdx
	};
	MRERegisterEventListener(
			obj,
			key,
			(ModListener){
				.mod = *FoxArrayMPeek(Mod *, &modman.context),
				.func.aerObj = listener
			},
			downstream
	);

	LogInfo("Successfully attached post-step listener.");
	return;
}

void AERObjectAttachCollisionListener(
		int32_t targetObjIdx,
		int32_t otherObjIdx,
		bool (* listener)(AERInstance * target, AERInstance * other),
		bool downstream
) {
	LogInfo(
			"Attaching %i collision listener to object %i for mod \"%s\"...",
			otherObjIdx,
			targetObjIdx,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_LISTENER_REG, AER_SEQ_BREAK);
	ErrIf(!listener, AER_NULL_ARG);
	ErrIf(!HLDObjectLookup(otherObjIdx), AER_FAILED_LOOKUP);

	HLDObject * obj = HLDObjectLookup(targetObjIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);

	EventKey key = {
		.type = HLD_EVENT_COLLISION,
		.num = otherObjIdx,
		.objIdx = targetObjIdx
	};
	MRERegisterEventListener(
			obj,
			key,
			(ModListener){
				.mod = *FoxArrayMPeek(Mod *, &modman.context),
				.func.aerObjPair = listener
			},
			downstream
	);

	LogInfo("Successfully attached %i collision listener.", otherObjIdx);
	return;
}

void AERObjectAttachAnimationEndListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
) {
	LogInfo(
			"Attaching animation end listener to object %i for mod \"%s\"...",
			objIdx,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_LISTENER_REG, AER_SEQ_BREAK);
	ErrIf(!listener, AER_NULL_ARG);

	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);

	EventKey key = {
		.type = HLD_EVENT_OTHER,
		.num = HLD_EVENT_OTHER_ANIMATION_END,
		.objIdx = objIdx
	};
	MRERegisterEventListener(
			obj,
			key,
			(ModListener){
				.mod = *FoxArrayMPeek(Mod *, &modman.context),
				.func.aerObj = listener
			},
			downstream
	);

	LogInfo("Successfully attached animation end listener.");
	return;
}
