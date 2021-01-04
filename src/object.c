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
#include "internal/export.h"
#include "internal/hld.h"
#include "internal/log.h"
#include "internal/modman.h"
#include "internal/mre.h"



/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT int32_t AERObjectRegister(
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
	ErrIf(mre.stage != STAGE_OBJECT_REG, AER_SEQ_BREAK, AER_OBJECT_NULL);

	HLDObject * parent = HLDObjectLookup(parentIdx);
	ErrIf(!parent, AER_FAILED_LOOKUP, AER_OBJECT_NULL);

	ErrIf(
			!(spriteIdx == -1 || HLDSpriteLookup(spriteIdx)),
			AER_FAILED_LOOKUP,
			AER_OBJECT_NULL
	);
	ErrIf(
			!(maskIdx == -1 || HLDSpriteLookup(maskIdx)),
			AER_FAILED_LOOKUP,
			AER_OBJECT_NULL
	);

	int32_t objIdx = hldfuncs.actionObjectAdd();
	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_OUT_OF_MEM, AER_OBJECT_NULL);

	/* The engine expects a freeable (dynamically allocated) string for name. */
	char * tmpName = malloc(strlen(name) + 1);
	ErrIf(!tmpName, AER_OUT_OF_MEM, AER_OBJECT_NULL);
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

AER_EXPORT size_t AERObjectGetNumRegistered(void) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, 0);

	return (*hldvars.objectTableHandle)->numItems;
}

AER_EXPORT const char * AERObjectGetName(int32_t objIdx) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP, NULL);

	return obj->name;
}

AER_EXPORT int32_t AERObjectGetParent(int32_t objIdx) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, AER_OBJECT_NULL);

	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP, AER_OBJECT_NULL);

	return obj->parentIndex;
}

AER_EXPORT bool AERObjectGetCollisions(int32_t objIdx) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, false);

	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP, false);

	return obj->flags.collisions;
}

AER_EXPORT void AERObjectSetCollisions(
		int32_t objIdx,
		bool collisions
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);

	HLDObject * obj = HLDObjectLookup(objIdx);
	ErrIf(!obj, AER_FAILED_LOOKUP);
	obj->flags.collisions = collisions;

	return;
}

AER_EXPORT void AERObjectAttachCreateListener(
		int32_t objIdx,
		bool (* listener)(
			AEREventTrapIter * ctx,
			AERInstance * target,
			AERInstance * other
		)
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
	MRERegisterEventListener(obj, key, listener);

	LogInfo("Successfully attached create listener.");
	return;
}

AER_EXPORT void AERObjectAttachDestroyListener(
		int32_t objIdx,
		bool (* listener)(
			AEREventTrapIter * ctx,
			AERInstance * target,
			AERInstance * other
		)
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
	MRERegisterEventListener(obj, key, listener);

	LogInfo("Successfully attached destroy listener.");
	return;
}

AER_EXPORT void AERObjectAttachAlarmListener(
		int32_t objIdx,
		uint32_t alarmIdx,
		bool (* listener)(
			AEREventTrapIter * ctx,
			AERInstance * target,
			AERInstance * other
		)
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
	MRERegisterEventListener(obj, key, listener);

	LogInfo("Successfully attached alarm %u listener.", alarmIdx);
	return;
}

AER_EXPORT void AERObjectAttachStepListener(
		int32_t objIdx,
		bool (* listener)(
			AEREventTrapIter * ctx,
			AERInstance * target,
			AERInstance * other
		)
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
	MRERegisterEventListener(obj, key, listener);

	LogInfo("Successfully attached step listener.");
	return;
}

AER_EXPORT void AERObjectAttachPreStepListener(
		int32_t objIdx,
		bool (* listener)(
			AEREventTrapIter * ctx,
			AERInstance * target,
			AERInstance * other
		)
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
	MRERegisterEventListener(obj, key, listener);

	LogInfo("Successfully attached pre-step listener.");
	return;
}

AER_EXPORT void AERObjectAttachPostStepListener(
		int32_t objIdx,
		bool (* listener)(
			AEREventTrapIter * ctx,
			AERInstance * target,
			AERInstance * other
		)
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
	MRERegisterEventListener(obj, key, listener);

	LogInfo("Successfully attached post-step listener.");
	return;
}

AER_EXPORT void AERObjectAttachCollisionListener(
		int32_t targetObjIdx,
		int32_t otherObjIdx,
		bool (* listener)(
			AEREventTrapIter * ctx,
			AERInstance * target,
			AERInstance * other
		)
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
	MRERegisterEventListener(obj, key, listener);

	LogInfo("Successfully attached %i collision listener.", otherObjIdx);
	return;
}

AER_EXPORT void AERObjectAttachAnimationEndListener(
		int32_t objIdx,
		bool (* listener)(
			AEREventTrapIter * ctx,
			AERInstance * target,
			AERInstance * other
		)
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
	MRERegisterEventListener(obj, key, listener);

	LogInfo("Successfully attached animation end listener.");
	return;
}
