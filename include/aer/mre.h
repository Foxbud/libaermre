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

/**
 * All possible error codes returned by functions in aermre.h.
 */
typedef enum AERErrCode {
	AER_OK, /**< No error occurred. */
	AER_NULL_ARG, /**< Function received a null pointer. */
	AER_NO_SUCH_OBJECT, /**< Invalid object index. */
	AER_NO_SUCH_INSTANCE, /**< Invalid instance index. */
	AER_NO_SUCH_ALARM_EVENT, /**< Invalid alarm event index. */
	AER_OUT_OF_MEM, /**< Function ran out of memory. */
	AER_OUT_OF_SEQ, /**< Function called at inappropriate time. */
	AER_SPRITE_REG_FAILED, /**< Sprite could not be registered. */
	AER_BUF_SIZE_RECORD /**< Function could not completely fill buffer, and
												caller did not record number of buffer elements
												written. */
} AERErrCode;

/**
 * Get the number of steps since the start of the game.
 *
 * @param[out] numSteps number of game steps
 *
 * @return error code
 */
AERErrCode AERGetNumSteps(uint32_t * numSteps);

/**
 * Get an array of keyboard keys pressed by the player on this step.
 *
 * The array contains 256 booleans for each keyboard keycode.
 *
 * @param[out] keys array of currently pressed keys
 *
 * @return error code
 */
AERErrCode AERGetKeysPressed(const bool ** keys);

/**
 * Get an array of keyboard keys held by the player on this step.
 *
 * The array contains 256 booleans for each keyboard keycode.
 *
 * @param[out] keys array of currently held keys
 *
 * @return error code
 */
AERErrCode AERGetKeysHeld(const bool ** keys);

/**
 * Get an array of keyboard keys released by the player on this step.
 *
 * The array contains 256 booleans for each keyboard keycode.
 *
 * @param[out] keys array of currently released keys
 *
 * @return error code
 */
AERErrCode AERGetKeysReleased(const bool ** keys);

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

AERErrCode AERGetNumInstances(size_t * numInsts);

AERErrCode AERGetInstances(
		size_t bufSize,
		AERInstance ** instBuf,
		size_t * numInsts
);

AERErrCode AERGetInstanceById(
		int32_t instId,
		AERInstance ** inst
);

AERErrCode AERInstanceCreate(
		int32_t objIdx,
		float x,
		float y,
		AERInstance ** inst
);

AERErrCode AERInstanceChange(
		AERInstance * inst,
		int32_t newObjIdx,
		bool doEvents
);

AERErrCode AERInstanceDestroy(AERInstance * inst);

AERErrCode AERInstanceDelete(AERInstance * inst);

AERErrCode AERInstanceSyncDepth(AERInstance * inst);

AERErrCode AERInstanceGetId(
		AERInstance * inst,
		int32_t * instId
);

AERErrCode AERInstanceGetObject(
		AERInstance * inst,
		int32_t * objIdx
);

AERErrCode AERInstanceGetPosition(
		AERInstance * inst,
		float * x,
		float * y
);

AERErrCode AERInstanceSetPosition(
		AERInstance * inst,
		float x,
		float y
);

AERErrCode AERInstanceGetFriction(
		AERInstance * inst,
		float * friction
);

AERErrCode AERInstanceSetFriction(
		AERInstance * inst,
		float friction
);

AERErrCode AERInstanceGetMotion(
		AERInstance * inst,
		float * x,
		float * y
);

AERErrCode AERInstanceSetMotion(
		AERInstance * inst,
		float x,
		float y
);

AERErrCode AERInstanceAddMotion(
		AERInstance * inst,
		float x,
		float y
);

AERErrCode AERInstanceGetMask(
		AERInstance * inst,
		int32_t * maskIdx
);

AERErrCode AERInstanceSetMask(
		AERInstance * inst,
		int32_t maskIdx
);

AERErrCode AERInstanceGetSprite(
		AERInstance * inst,
		int32_t * spriteIdx
);

AERErrCode AERInstanceSetSprite(
		AERInstance * inst,
		int32_t spriteIdx
);

AERErrCode AERInstanceGetSpriteFrame(
		AERInstance * inst,
		float * frame
);

AERErrCode AERInstanceSetSpriteFrame(
		AERInstance * inst,
		float frame
);

AERErrCode AERInstanceGetSpriteSpeed(
		AERInstance * inst,
		float * speed
);

AERErrCode AERInstanceSetSpriteSpeed(
		AERInstance * inst,
		float speed
);

AERErrCode AERInstanceGetSpriteAlpha(
		AERInstance * inst,
		float * alpha
);

AERErrCode AERInstanceSetSpriteAlpha(
		AERInstance * inst,
		float alpha
);

AERErrCode AERInstanceGetSpriteAngle(
		AERInstance * inst,
		float * angle
);

AERErrCode AERInstanceSetSpriteAngle(
		AERInstance * inst,
		float angle
);

AERErrCode AERInstanceGetSpriteScale(
		AERInstance * inst,
		float * x,
		float * y
);

AERErrCode AERInstanceSetSpriteScale(
		AERInstance * inst,
		float x,
		float y
);

AERErrCode AERInstanceGetTangible(
		AERInstance * inst,
		bool * tangible
);

AERErrCode AERInstanceSetTangible(
		AERInstance * inst,
		bool tangible
);

AERErrCode AERInstanceGetAlarm(
		AERInstance * inst,
		uint32_t alarmIdx,
		int32_t * numSteps
);

AERErrCode AERInstanceSetAlarm(
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

AERErrCode AERRoomGetCurrent(int32_t * roomIdx);

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

AERErrCode AERObjectGetName(
		int32_t objIdx,
		const char ** name
);

AERErrCode AERObjectGetParent(
		int32_t objIdx,
		int32_t * parentIdx
);

AERErrCode AERObjectGetNumInstances(
		int32_t objIdx,
		size_t * numInsts
);

AERErrCode AERObjectGetInstances(
		int32_t objIdx,
		size_t bufSize,
		AERInstance ** instBuf,
		size_t * numInsts
);

AERErrCode AERObjectGetCollisions(
		int32_t objIdx,
		bool * collisions
);

AERErrCode AERObjectSetCollisions(
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

AERErrCode AERRegisterSprite(
		const char * filename,
		size_t numFrames,
		uint32_t origX,
		uint32_t origY,
		int32_t * spriteIdx
);

AERErrCode AERRegisterObject(
		const char * name,
		int32_t parentIdx,
		int32_t spriteIdx,
		int32_t maskIdx,
		int32_t depth,
		bool visible,
		bool collisions,
		bool persistent,
		int32_t * objIdx
);

/**
 * Attach a listener to an object's creation event.
 *
 * @param[in] objIdx index of target object
 * @param[in] listener event callback
 * @param[in] downstream whether to attach downstream of original event
 *
 * @return error code
 */
AERErrCode AERRegisterCreateListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

AERErrCode AERRegisterDestroyListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

AERErrCode AERRegisterAlarmListener(
		int32_t objIdx,
		uint32_t alarmIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

AERErrCode AERRegisterStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

AERErrCode AERRegisterPreStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

AERErrCode AERRegisterPostStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

AERErrCode AERRegisterCollisionListener(
		int32_t targetObjIdx,
		int32_t otherObjIdx,
		bool (* listener)(AERInstance * target, AERInstance * other),
		bool downstream
);

AERErrCode AERRegisterAnimationEndListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

/**
 * @}
 */



#endif /* AER_MRE_H */
