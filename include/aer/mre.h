/**
 * @file
 */
#ifndef AER_MRE_H
#define AER_MRE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



/**
 * @defgroup general General
 *
 * This module contains all general utilities.
 *
 * @sa aer/mre.h
 *
 * @{
 */

uint32_t AERGetNumSteps(void);

const bool * AERGetKeysPressed(void);

const bool * AERGetKeysHeld(void);

const bool * AERGetKeysReleased(void);

/**
 * @}
 */



/**
 * @defgroup instance Instance
 *
 * This module contains all instance related utilities.
 *
 * @sa aer/mre.h
 *
 * @{
 */

/**
 * This type represents an in-game instance.
 */
typedef void AERInstance;

size_t AERGetInstances(
		size_t bufSize,
		AERInstance ** instBuf
);

AERInstance * AERGetInstanceById(int32_t instId);

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

/**
 * @}
 */



/**
 * @defgroup room Room
 *
 * This module contains all room related utilities.
 *
 * @sa aer/mre.h
 *
 * @{
 */

int32_t AERRoomGetCurrent(void);

/**
 * @}
 */



/**
 * @defgroup object Object
 *
 * This module contains all object related utilities.
 *
 * @sa aer/mre.h
 *
 * @{
 */

const char * AERObjectGetName(int32_t objIdx);

int32_t AERObjectGetParent(int32_t objIdx);

size_t AERObjectGetInstances(
		int32_t objIdx,
		size_t bufSize,
		AERInstance ** instBuf
);

bool AERObjectGetCollisions(int32_t objIdx);

void AERObjectSetCollisions(
		int32_t objIdx,
		bool collisions
);

/**
 * @}
 */



/**
 * @defgroup registration Registration
 *
 * This module contains all registration related utilities.
 *
 * @sa aer/mre.h
 *
 * @{
 */

int32_t AERRegisterSprite(
		const char * filename,
		size_t numFrames,
		uint32_t origX,
		uint32_t origY
);

int32_t AERRegisterObject(
		const char * name,
		int32_t parentIdx,
		int32_t spriteIdx,
		int32_t maskIdx,
		int32_t depth,
		bool visible,
		bool collisions,
		bool persistent
);

void AERRegisterCreateListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

void AERRegisterDestroyListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

void AERRegisterAlarmListener(
		int32_t objIdx,
		uint32_t alarmIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

void AERRegisterStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

void AERRegisterPreStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

void AERRegisterPostStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

void AERRegisterCollisionListener(
		int32_t targetObjIdx,
		int32_t otherObjIdx,
		bool (* listener)(AERInstance * target, AERInstance * other),
		bool downstream
);

void AERRegisterAnimationEndListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

/**
 * @}
 */



#endif /* AER_MRE_H */
