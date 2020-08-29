#include "foxutils/math.h"

#include "aer/instance.h"
#include "internal/err.h"
#include "internal/hld.h"
#include "internal/mre.h"



/* ----- PUBLIC FUNCTIONS ----- */

size_t AERInstanceGetAll(
		size_t bufSize,
		AERInstance ** instBuf
) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, 0);
	ErrIf(!instBuf && bufSize > 0, AER_NULL_ARG, 0);

	HLDRoom * room = *hldvars.roomCurrent;

	size_t numInsts = room->numInstances;
	size_t numToWrite = FoxMin(numInsts, bufSize);
	HLDInstance * inst = room->instanceFirst;
	for (size_t idx = 0; idx < numToWrite; idx++) {
		instBuf[idx] = (AERInstance *)inst;
		inst = inst->instanceNext;
	}

	return numInsts;
}

AERInstance * AERInstanceGetById(int32_t instId) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

	AERInstance * inst = (AERInstance *)HLDInstanceLookup(instId);
	ErrIf(!inst, AER_FAILED_LOOKUP, NULL);

	return inst;
}

AERInstance * AERInstanceCreate(
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

void AERInstanceChange(
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

void AERInstanceDestroy(AERInstance * inst) {
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

void AERInstanceDelete(AERInstance * inst) {
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

void AERInstanceSyncDepth(AERInstance * inst) {
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
	hldfuncs.Instance_setMotionPolarFromCartesian(inst);

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
	hldfuncs.Instance_setMotionPolarFromCartesian(inst);

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

	hldfuncs.Instance_setMaskIndex(
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