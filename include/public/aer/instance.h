/**
 * @file
 *
 * @brief Utilities for querying and manipulating game instances.
 *
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
#ifndef AER_INSTANCE_H
#define AER_INSTANCE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



/* ----- PUBLIC TYPES ----- */

/**
 * @brief Simple alias for an instance of an object.
 *
 * @since 1.0.0
 *
 * @sa object.h
 */
typedef void AERInstance;



/* ----- PUBLIC FUNCTIONS ----- */

/**
 * @brief Query all instances in the current room.
 *
 * @warning Argument `instBuf` must be large enough to hold at least
 * `bufSize` elements.
 *
 * @note Argument `bufSize` may be `0` in which case argument `instBuf` may
 * be `NULL`. This may be used to efficiently query the total number of
 * instances in the current room.
 *
 * @param[in] bufSize Maximum number of elements to write to argument
 * `instBuf`.
 * @param[out] instBuf Buffer to write instances to.
 *
 * @return Total number of instances in current room.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `instBuf` is `NULL` and argument
 * `bufSize` is greater than `0`.
 *
 * @since 1.0.0
 */
size_t AERInstanceGetAll(
		size_t bufSize,
		AERInstance ** instBuf
);

/**
 * @brief Query the instance with a specific ID.
 *
 * @param[in] instId Instance ID.
 *
 * @return Instance with provided ID or `NULL` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_FAILED_LOOKUP if argument `instId` is an invalid instance.
 *
 * @since 1.0.0
 */
AERInstance * AERInstanceGetById(int32_t instId);

/**
 * @brief Create an instance of an object.
 *
 * @param[in] objIdx Object to create an instance of.
 * @param[in] x Horizontal position at which to create instance.
 * @param[in] y Vertical position at which to create instance.
 *
 * @return New instance or `NULL` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_FAILED_LOOKUP if argument `objIdx` is an invalid object.
 * @throw ::AER_OUT_OF_MEM if space for new instance could not be allocated.
 *
 * @since 1.0.0
 */
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

size_t AERInstanceGetLocals(
		AERInstance * inst,
		size_t bufSize,
		const char ** nameBuf
);

void * AERInstanceGetLocal(
		AERInstance * inst,
		const char * name
);



#endif /* AER_INSTANCE_H */
