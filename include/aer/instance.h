/**
 * @file
 */
#ifndef AER_INSTANCE_H
#define AER_INSTANCE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



/* ----- PUBLIC TYPES ----- */

typedef void AERInstance;



/* ----- PUBLIC FUNCTIONS ----- */

size_t AERInstanceGetAll(
		size_t bufSize,
		AERInstance ** instBuf
);

AERInstance * AERInstanceGetById(int32_t instId);

AERInstance * AERInstanceCreate(
		int32_t objIdx,
		float x,
		float y
);

void AERInstanceChange(
		AERInstance * inst,
		int32_t newObjIdx,
		bool doEvents
);

void AERInstanceDestroy(AERInstance * inst);

void AERInstanceDelete(AERInstance * inst);

void AERInstanceSyncDepth(AERInstance * inst);

int32_t AERInstanceGetId(AERInstance * inst);

int32_t AERInstanceGetObject(AERInstance * inst);

void AERInstanceGetPosition(
		AERInstance * inst,
		float * x,
		float * y
);

void AERInstanceSetPosition(
		AERInstance * inst,
		float x,
		float y
);

float AERInstanceGetFriction(AERInstance * inst);

void AERInstanceSetFriction(
		AERInstance * inst,
		float friction
);

void AERInstanceGetMotion(
		AERInstance * inst,
		float * x,
		float * y
);

void AERInstanceSetMotion(
		AERInstance * inst,
		float x,
		float y
);

void AERInstanceAddMotion(
		AERInstance * inst,
		float x,
		float y
);

int32_t AERInstanceGetMask(AERInstance * inst);

void AERInstanceSetMask(
		AERInstance * inst,
		int32_t maskIdx
);

int32_t AERInstanceGetSprite(AERInstance * inst);

void AERInstanceSetSprite(
		AERInstance * inst,
		int32_t spriteIdx
);

float AERInstanceGetSpriteFrame(AERInstance * inst);

void AERInstanceSetSpriteFrame(
		AERInstance * inst,
		float frame
);

float AERInstanceGetSpriteSpeed(AERInstance * inst);

void AERInstanceSetSpriteSpeed(
		AERInstance * inst,
		float speed
);

float AERInstanceGetSpriteAlpha(AERInstance * inst);

void AERInstanceSetSpriteAlpha(
		AERInstance * inst,
		float alpha
);

float AERInstanceGetSpriteAngle(AERInstance * inst);

void AERInstanceSetSpriteAngle(
		AERInstance * inst,
		float angle
);

void AERInstanceGetSpriteScale(
		AERInstance * inst,
		float * x,
		float * y
);

void AERInstanceSetSpriteScale(
		AERInstance * inst,
		float x,
		float y
);

bool AERInstanceGetTangible(AERInstance * inst);

void AERInstanceSetTangible(
		AERInstance * inst,
		bool tangible
);

int32_t AERInstanceGetAlarm(
		AERInstance * inst,
		uint32_t alarmIdx
);

void AERInstanceSetAlarm(
		AERInstance * inst,
		uint32_t alarmIdx,
		int32_t numSteps
);



#endif /* AER_INSTANCE_H */
