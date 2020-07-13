/**
 * @file
 *
 * @brief This module contains the core modding functions for interacting
 * with the game.
 *
 * All mods should include this header file. Without its function, a mod
 * cannot meaningfully interact with the game.
 */
#ifndef AERMRE_H
#define AERMRE_H

#include <stdbool.h>
#include <stdint.h>



/* ----- PUBLIC TYPES ----- */

typedef enum AERErrCode {
	AER_OK,
	AER_NULL_ARG,
	AER_NO_SUCH_OBJECT,
	AER_NO_SUCH_INSTANCE,
	AER_OUT_OF_MEM,
	AER_OUT_OF_SEQ,
	AER_SPRITE_REG_FAILED
} AERErrCode;

typedef struct AERInstance {
	const uint8_t rawData[0x184];
} AERInstance;



/* ----- PUBLIC FUNCTIONS ----- */

AERErrCode AERRegisterSprite(
		const char * filename,
		size_t numFrames,
		uint32_t origX,
		uint32_t origY,
		int32_t * spriteIdx
);

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

AERErrCode AERGetCurrentRoom(int32_t * roomIdx);

AERErrCode AERGetNumInstances(size_t * numInsts);

AERErrCode AERGetNumInstancesByObject(
		int32_t objIdx,
		size_t * numInsts
);

AERErrCode AERGetInstances(
		size_t bufSize,
		AERInstance ** instBuf,
		size_t * numInsts
);

AERErrCode AERGetInstancesByObject(
		int32_t objIdx,
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

AERErrCode AERInstanceDestroy(AERInstance * inst);

AERErrCode AERInstanceDelete(AERInstance * inst);

AERErrCode AERInstanceGetId(
		AERInstance * inst,
		int32_t * instId
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
		uint32_t * frame
);

AERErrCode AERInstanceSetSpriteFrame(
		AERInstance * inst,
		uint32_t frame
);

AERErrCode AERInstanceGetSpriteSpeed(
		AERInstance * inst,
		float * speed
);

AERErrCode AERInstanceSetSpriteSpeed(
		AERInstance * inst,
		float speed
);

AERErrCode AERInstanceGetSolid(
		AERInstance * inst,
		bool * solid
);

AERErrCode AERInstanceSetSolid(
		AERInstance * inst,
		bool solid
);



#endif /* AERMRE_H */
