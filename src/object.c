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

__attribute__((visibility("default")))
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
	ErrIf(!name, AER_NULL_ARG, -1);
	LogInfo(
			"Registering object \"%s\" for mod \"%s\"...",
			name,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);
	ErrIf(mre.stage != STAGE_OBJECT_REG, AER_SEQ_BREAK, -1);

	HLDObject * parent = HLDObjectLookup(parentIdx);
	ErrIf(!parent, AER_FAILED_LOOKUP, -1);

	ErrIf(
			!(spriteIdx == -1 || HLDSpriteLookup(spriteIdx)),
			AER_FAILED_LOOKUP,
			-1
	);
	ErrIf(
			!(maskIdx == -1 || HLDSpriteLookup(maskIdx)),
			AER_FAILED_LOOKUP,
			-1
	);

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

__attribute__((visibility("default")))
size_t AERObjectGetNumRegistered(void) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, 0);

	return (*hldvars.objectTableHandle)->numItems;
}

__attribute__((visibility("default")))
const char * AERObjectGetName(int32_t objIdx) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP, NULL);

	return obj->name;
}

__attribute__((visibility("default")))
int32_t AERObjectGetParent(int32_t objIdx) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1);

	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP, -1);

	return obj->parentIndex;
}

__attribute__((visibility("default")))
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
	for (uint32_t idx = 0; idx < numToWrite; idx++) {
		instBuf[idx] = (AERInstance *)node->item;
		node = node->next;
	}

	return numInsts;
}

__attribute__((visibility("default")))
bool AERObjectGetCollisions(int32_t objIdx) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, false);

	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP, false);

	return obj->flags.collisions;
}

__attribute__((visibility("default")))
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

__attribute__((visibility("default")))
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

__attribute__((visibility("default")))
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

__attribute__((visibility("default")))
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

__attribute__((visibility("default")))
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

__attribute__((visibility("default")))
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

__attribute__((visibility("default")))
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

__attribute__((visibility("default")))
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

__attribute__((visibility("default")))
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
