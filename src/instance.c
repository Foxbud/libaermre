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
#include "foxutils/mapmacs.h"
#include "foxutils/math.h"

#include "aer/instance.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/hld.h"
#include "internal/mre.h"



/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT size_t AERInstanceGetAll(
		size_t bufSize,
		AERInstance ** instBuf
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, 0);
	ErrIf(!instBuf && bufSize > 0, AER_NULL_ARG, 0);

	HLDRoom * room = *hldvars.roomCurrent;

	size_t numInsts = room->numInstances;
	size_t numToWrite = FoxMin(numInsts, bufSize);
	HLDInstance * inst = room->instanceFirst;
	for (uint32_t idx = 0; idx < numToWrite; idx++) {
		instBuf[idx] = (AERInstance *)inst;
		inst = inst->instanceNext;
	}

	return numInsts;
}

AER_EXPORT size_t AERInstanceGetByObject(
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

AER_EXPORT AERInstance * AERInstanceGetById(int32_t instId) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

	AERInstance * inst = (AERInstance *)HLDInstanceLookup(instId);
	ErrIf(!inst, AER_FAILED_LOOKUP, NULL);

	return inst;
}

AER_EXPORT AERInstance * AERInstanceCreate(
		int32_t objIdx,
		float x,
		float y
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);
	ErrIf(!HLDObjectLookup(objIdx), AER_FAILED_LOOKUP, NULL);

	AERInstance * inst = (AERInstance *)hldfuncs.actionInstanceCreate(
			objIdx,
			x,
			y
	);
	ErrIf(!inst, AER_OUT_OF_MEM, NULL);

	return inst;
}

AER_EXPORT void AERInstanceChange(
		AERInstance * inst,
		int32_t newObjIdx,
		bool doEvents
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);
	ErrIf(!HLDObjectLookup(newObjIdx), AER_FAILED_LOOKUP);

	hldfuncs.actionInstanceChange(
			(HLDInstance *)inst,
			newObjIdx,
			doEvents
	);

	return;
}

AER_EXPORT void AERInstanceDestroy(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	hldfuncs.actionInstanceDestroy(
			(HLDInstance *)inst,
			(HLDInstance *)inst,
			-1,
			true
	);

	return;
}

AER_EXPORT void AERInstanceCancelDestruction(AERInstance * inst) {
#define inst ((HLDInstance *)inst)
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);
	ErrIf(!inst->marked, AER_BAD_VAL);

	inst->marked = false;

	return;
#undef inst
}

AER_EXPORT void AERInstanceDelete(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	hldfuncs.actionInstanceDestroy(
			(HLDInstance *)inst,
			(HLDInstance *)inst,
			-1,
			false
	);

	return;
}

AER_EXPORT float AERInstanceGetDepth(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, 0.0f);
	ErrIf(!inst, AER_NULL_ARG, 0.0f);

	return ((HLDInstance *)inst)->depth;
}

AER_EXPORT void AERInstanceSetDepth(AERInstance * inst, float depth) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	((HLDInstance *)inst)->depth = depth;

	return;
}

AER_EXPORT void AERInstanceSyncDepth(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	uint32_t unknownDatastructure[4] = {0};
	hldfuncs.gmlScriptSetdepth(
			(HLDInstance *)inst,
			(HLDInstance *)inst,
			&unknownDatastructure,
			0,
			0
	);

	return;
}

AER_EXPORT int32_t AERInstanceGetId(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1);
	ErrIf(!inst, AER_NULL_ARG, -1);

	return ((HLDInstance *)inst)->id;
}

AER_EXPORT int32_t AERInstanceGetObject(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1);
	ErrIf(!inst, AER_NULL_ARG, -1);

	return ((HLDInstance *)inst)->objectIndex;
}

AER_EXPORT void AERInstanceGetPosition(
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

AER_EXPORT void AERInstanceSetPosition(
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

void AERInstanceGetBoundingBox(
		AERInstance * inst,
		float * left,
		float * top,
		float * right,
		float * bottom
) {
#define inst ((HLDInstance *)inst)
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);
	ErrIf(!(left || top || right || bottom), AER_NULL_ARG);

	if (left) *left = (float)inst->bbox.left;
	if (top) *top = (float)inst->bbox.top;
	if (right) *right = (float)inst->bbox.right;
	if (bottom) *bottom = (float)inst->bbox.bottom;

	return;
#undef inst
}

AER_EXPORT float AERInstanceGetFriction(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, 0.0f);
	ErrIf(!inst, AER_NULL_ARG, 0.0f);

	return ((HLDInstance *)inst)->friction;
}

AER_EXPORT void AERInstanceSetFriction(
		AERInstance * inst,
		float friction
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	((HLDInstance *)inst)->friction = friction;

	return;
}

AER_EXPORT void AERInstanceGetMotion(
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

AER_EXPORT void AERInstanceSetMotion(
		AERInstance * inst,
		float x,
		float y
) {
#define inst ((HLDInstance *)inst)
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	inst->speedX = x;
	inst->speedY = y;
	hldfuncs.Instance_setMotionPolarFromCartesian(inst);

	return;
#undef inst
}

AER_EXPORT void AERInstanceAddMotion(
		AERInstance * inst,
		float x,
		float y
) {
#define inst ((HLDInstance *)inst)
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	inst->speedX += x;
	inst->speedY += y;
	hldfuncs.Instance_setMotionPolarFromCartesian(inst);

	return;
#undef inst
}

AER_EXPORT int32_t AERInstanceGetMask(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1);
	ErrIf(!inst, AER_NULL_ARG, -1);

	return ((HLDInstance *)inst)->maskIndex;
}

AER_EXPORT void AERInstanceSetMask(
		AERInstance * inst,
		int32_t maskIdx
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);
	ErrIf(!(maskIdx == -1 || HLDSpriteLookup(maskIdx)), AER_FAILED_LOOKUP);

	hldfuncs.Instance_setMaskIndex(
			(HLDInstance *)inst,
			maskIdx
	);

	return;
}

AER_EXPORT int32_t AERInstanceGetSprite(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1);
	ErrIf(!inst, AER_NULL_ARG, -1);

	return ((HLDInstance *)inst)->spriteIndex;
}

AER_EXPORT void AERInstanceSetSprite(
		AERInstance * inst,
		int32_t spriteIdx
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);
	ErrIf(!(spriteIdx == -1 || HLDSpriteLookup(spriteIdx)), AER_FAILED_LOOKUP);

	((HLDInstance *)inst)->spriteIndex = spriteIdx;

	return;
}

AER_EXPORT float AERInstanceGetSpriteFrame(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1.0f);
	ErrIf(!inst, AER_NULL_ARG, -1.0f);

	return ((HLDInstance *)inst)->imageIndex;
}

AER_EXPORT void AERInstanceSetSpriteFrame(
		AERInstance * inst,
		float frame
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	((HLDInstance *)inst)->imageIndex = frame;

	return;
}

AER_EXPORT float AERInstanceGetSpriteSpeed(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1.0f);
	ErrIf(!inst, AER_NULL_ARG, -1.0f);

	return ((HLDInstance *)inst)->imageSpeed;
}

AER_EXPORT void AERInstanceSetSpriteSpeed(
		AERInstance * inst,
		float speed
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);
	ErrIf(speed < 0.0f, AER_BAD_VAL);

	((HLDInstance *)inst)->imageSpeed = speed;

	return;
}

AER_EXPORT float AERInstanceGetSpriteAlpha(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1.0f);
	ErrIf(!inst, AER_NULL_ARG, -1.0f);

	return ((HLDInstance *)inst)->imageAlpha;
}

AER_EXPORT void AERInstanceSetSpriteAlpha(
		AERInstance * inst,
		float alpha
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);
	ErrIf(alpha < 0.0f || alpha > 1.0f, AER_BAD_VAL);

	((HLDInstance *)inst)->imageAlpha = alpha;

	return;
}

AER_EXPORT float AERInstanceGetSpriteAngle(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, 0.0f);
	ErrIf(!inst, AER_NULL_ARG, 0.0f);

	return ((HLDInstance *)inst)->imageAngle;
}

AER_EXPORT void AERInstanceSetSpriteAngle(
		AERInstance * inst,
		float angle
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	((HLDInstance *)inst)->imageAngle = angle;

	return;
}

AER_EXPORT void AERInstanceGetSpriteScale(
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

AER_EXPORT void AERInstanceSetSpriteScale(
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

AER_EXPORT bool AERInstanceGetTangible(AERInstance * inst) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, false);
	ErrIf(!inst, AER_NULL_ARG, false);

	return ((HLDInstance *)inst)->tangible;
}

AER_EXPORT void AERInstanceSetTangible(
		AERInstance * inst,
		bool tangible
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!inst, AER_NULL_ARG);

	((HLDInstance *)inst)->tangible = tangible;

	return;
}

AER_EXPORT int32_t AERInstanceGetAlarm(
		AERInstance * inst,
		uint32_t alarmIdx
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1);
	ErrIf(!inst, AER_NULL_ARG, -1);
	ErrIf(alarmIdx >= 12, AER_FAILED_LOOKUP, -1);

	return ((HLDInstance *)inst)->alarms[alarmIdx];
}

AER_EXPORT void AERInstanceSetAlarm(
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

AER_EXPORT size_t AERInstanceGetLocals(
		AERInstance * inst,
		size_t bufSize,
		const char ** nameBuf
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, 0);
	ErrIf(!inst, AER_NULL_ARG, 0);
	ErrIf(!nameBuf && bufSize > 0, AER_NULL_ARG, 0);

	const char ** names = hldvars.instanceLocalTable->elements;
	HLDCHashTable * locals = ((HLDInstance *)inst)->locals;
	HLDCHashSlot * slots = locals->slots;

	size_t numLocals = locals->numItems;
	size_t numToWrite = FoxMin(numLocals, bufSize);
	size_t numSlots = locals->numSlots;
	uint32_t bufIdx = 0;
	for (uint32_t slotIdx = 0; slotIdx < numSlots; slotIdx++) {
		if (bufIdx == numToWrite) break;
		HLDCHashSlot * slot = slots + slotIdx;
		if (slot->keyNext) {
			nameBuf[bufIdx++] = names[slot->key];
		}
	}

	return numLocals;
}

AER_EXPORT void * AERInstanceGetLocal(
		AERInstance * inst,
		const char * name
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);
	ErrIf(!inst, AER_NULL_ARG, NULL);
	ErrIf(!name, AER_NULL_ARG, NULL);

	int32_t * localIdx = FoxMapMIndex(
			const char *,
			int32_t,
			mre.instLocals,
			name
	);
	ErrIf(!localIdx, AER_FAILED_LOOKUP, NULL);

	void * local = HLDCHashTableLookup(
			((HLDInstance *)inst)->locals,
			*localIdx
	);
	ErrIf(!local, AER_FAILED_LOOKUP, NULL);

	return local;
}
