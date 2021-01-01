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
 * @return Total number of instances in current room or `0` if unsuccessful.
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
 * @brief Query all instances of an object in the current room.
 *
 * @warning Does **not** include instances of child objects.
 * @warning Argument `instBuf` must be large enough to hold at least
 * `bufSize` elements.
 *
 * @note Argument `bufSize` may be `0` in which case argument `instBuf` may
 * be `NULL`. This may be used to efficiently query the total number of
 * instances of an object in the current room.
 *
 * @param[in] objIdx Object to get instances of.
 * @param[in] bufSize Maximum number of elements to write to argument
 * `instBuf`.
 * @param[out] instBuf Buffer to write instances to.
 *
 * @return Total number of instances of object in current room or `0` if
 * unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `instBuf` is `NULL` and argument
 * `bufSize` is greater than `0`.
 * @throw ::AER_FAILED_LOOKUP if argument `objIdx` is an invalid object.
 *
 * @since 1.0.0
 */
size_t AERInstanceGetByObject(
		int32_t objIdx,
		size_t bufSize,
		AERInstance ** instBuf
);

/**
 * @brief Query the instance with a specific ID in the current room.
 *
 * @param[in] instId Instance ID.
 *
 * @return Instance with provided ID or `NULL` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_FAILED_LOOKUP if argument `instId` is an invalid instance.
 *
 * @since 1.0.0
 *
 * @sa AERInstanceGetId
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
 *
 * @sa AERObjectAttachCreateListener
 */
AERInstance * AERInstanceCreate(
		int32_t objIdx,
		float x,
		float y
);

/**
 * @brief Convert an instance of one object into an instance of another
 * object in-place.
 *
 * @warning Once this function has been called on and instance, no further
 * queries or actions should be performed on this instance until the next
 * in-game step.
 *
 * Under the hood, this function creates a new instance of the new object,
 * copies various properties such as position from the old instance to the
 * new instance, and then finally destroys the old instance.
 *
 * @param[in] inst Instance of interest.
 * @param[in] newObjIdx Object to convert argument `inst` into.
 * @param[in] doEvents If `true`, then the engine will call the destroy
 * event of the old instance and the create event of the new instance.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `newObjIdx` is an invalid object.
 *
 * @since 1.0.0
 *
 * @sa AERObjectAttachCreateListener
 * @sa AERObjectAttachDestroyListener
 */
void AERInstanceChange(
		AERInstance * inst,
		int32_t newObjIdx,
		bool doEvents
);

/**
 * @brief Destroy an instance and call its destroy event.
 *
 * @param[in] inst Instance of interest.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 *
 * @se AERInstanceCancelDestruction
 * @sa AERInstanceDelete
 * @sa AERObjectAttachDestroyListener
 */
void AERInstanceDestroy(AERInstance * inst);

/**
 * @brief Prevent an instance that is marked for destruction from being
 * destroyed.
 *
 * @note This function should typically only be called from an upstream
 * object destroy listener.
 *
 * @param[in] inst Instance of interest.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 * @throw ::AER_BAD_VAL if argument `inst` is not marked for destruction.
 *
 * @since 1.0.0
 *
 * @sa AERInstanceDestroy
 * @sa AERObjectAttachDestroyListener
 */
void AERInstanceCancelDestruction(AERInstance * inst);

/**
 * @brief Destroy an instance but do **not** call its destroy event.
 *
 * @param[in] inst Instance of interest.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 *
 * @sa AERInstanceDestroy
 * @sa AERObjectAttachDestroyListener
 */
void AERInstanceDelete(AERInstance * inst);

/**
 * @brief Query the render depth of an instance.
 *
 * @param[in] inst Instance of interest.
 *
 * @return Render depth or `0.0f` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 */
float AERInstanceGetDepth(AERInstance * inst);

/**
 * @brief Set the render depth of an instance.
 *
 * @param[in] inst Instance of interest.
 * @param[in] depth Render depth.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 */
void AERInstanceSetDepth(AERInstance * inst, float depth);

/**
 * @brief Set the render depth of an instance based on its position in the
 * current room.
 *
 * @param[in] inst Instance of interest.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 */
void AERInstanceSyncDepth(AERInstance * inst);

/**
 * @brief Query the ID of an instance.
 *
 * @note A valid reference to an ::AERInstance one step may become invalid
 * in a future step. To keep track of a specific instance across steps,
 * use its ID.
 *
 * @param[in] inst Instance of interest.
 *
 * @return Instance ID or `-1` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 *
 * @sa AERInstanceGetById
 */
int32_t AERInstanceGetId(AERInstance * inst);

/**
 * @brief Query the object of an instance.
 *
 * @param[in] inst Instance of interest.
 *
 * @return Object index or @ref ::AER_OBJECT_NULL if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 */
int32_t AERInstanceGetObject(AERInstance * inst);

/**
 * @brief Query the position of an instance in the current room.
 *
 * If only one component of the position is needed, then the argument for the
 * unneeded component may be `NULL`.
 *
 * @param[in] inst Instance of interest.
 * @param[out] x Horizontal position.
 * @param[out] y Vertical position.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL` or both arguments
 * `x` and `y` are `NULL`.
 *
 * @since 1.0.0
 */
void AERInstanceGetPosition(
		AERInstance * inst,
		float * x,
		float * y
);

/**
 * @brief Set the position of an instance in the current room.
 *
 * @param[in] inst Instance of interest.
 * @param[in] x Horizontal position.
 * @param[in] y Vertical position.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 */
void AERInstanceSetPosition(
		AERInstance * inst,
		float x,
		float y
);

/**
 * @brief Query the axis-aligned bounding box of an instance.
 *
 * The bounding box of an instance describes the bounding volume of an
 * instance's sprite or mask. It is relative to the room's origin.
 *
 * If not all four of the components of the bounding box are needed, then the
 * arguments for the unneeded components may be `NULL`.
 *
 * @param[in] inst Instance of interest.
 * @param[out] left X-intercept of the left side of the bounding box.
 * @param[out] top Y-intercept of the top side of the bounding box.
 * @param[out] right X-intercept of the right side of the bounding box.
 * @param[out] bottom Y-intercept of the bottom side of the bounding box.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL` or all four arguments
 * `left`, `top`, `right` and `bottom` are `NULL`.
 *
 * @since 1.0.0
 */
void AERInstanceGetBoundingBox(
		AERInstance * inst,
		float * left,
		float * top,
		float * right,
		float * bottom
);

/**
 * @brief Query the friction of an instance.
 *
 * @subsubsection InstFriction Instance Friction
 *
 * The friction of an instance determines how much it decelerates by each step.
 *
 * @param[in] inst Instance of interest.
 *
 * @return Friction or `0.0f` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 */
float AERInstanceGetFriction(AERInstance * inst);

/**
 * @brief Set the friction of an instance.
 *
 * For more information about friction, see @ref InstFriction.
 *
 * @param[in] inst Instance of interest.
 * @param[in] friction Friction.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 */
void AERInstanceSetFriction(
		AERInstance * inst,
		float friction
);

/**
 * @brief Query the motion of an instance.
 *
 * If only one component of the motion is needed, then the argument for the
 * unneeded component may be `NULL`.
 *
 * @param[in] inst Instance of interest.
 * @param[out] x Horizontal motion.
 * @param[out] y Vertical motion.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL` or both arguments
 * `x` and `y` are `NULL`.
 *
 * @since 1.0.0
 */
void AERInstanceGetMotion(
		AERInstance * inst,
		float * x,
		float * y
);

/**
 * @brief Set the motion of an instance.
 *
 * @param[in] inst Instance of interest.
 * @param[in] x Horizontal motion.
 * @param[in] y Vertical motion.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 *
 * @sa AERInstanceAddMotion
 */
void AERInstanceSetMotion(
		AERInstance * inst,
		float x,
		float y
);

/**
 * @brief Add to the existing motion of an instance.
 *
 * @param[in] inst Instance of interest.
 * @param[in] x Horizontal motion.
 * @param[in] y Vertical motion.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 *
 * @sa AERInstanceSetMotion
 */
void AERInstanceAddMotion(
		AERInstance * inst,
		float x,
		float y
);

/**
 * @brief Query the collision mask of an instance.
 *
 * @param[in] inst Instance of interest.
 *
 * @return Collision mask index or @ref ::AER_SPRITE_NULL if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 */
int32_t AERInstanceGetMask(AERInstance * inst);

/**
 * @brief Set the collision mask of an instance.
 *
 * By default, an instance will use its sprite as its collision mask.
 * In this case, the instance's mask should be set to @ref ::AER_SPRITE_NULL.
 *
 * @param[in] inst Instance of interest.
 * @param[in] maskIdx Collision mask index.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `maskIdx` is an invalid sprite.
 *
 * @since 1.0.0
 */
void AERInstanceSetMask(
		AERInstance * inst,
		int32_t maskIdx
);

/**
 * @brief Query the sprite of an instance.
 *
 * @param[in] inst Instance of interest.
 *
 * @return Sprite index or @ref ::AER_SPRITE_NULL if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 */
int32_t AERInstanceGetSprite(AERInstance * inst);

/**
 * @brief Set the sprite of an instance.
 *
 * @param[in] inst Instance of interest.
 * @param[in] spriteIdx Sprite index.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `spriteIdx` is an invalid sprite.
 *
 * @since 1.0.0
 */
void AERInstanceSetSprite(
		AERInstance * inst,
		int32_t spriteIdx
);

/**
 * @brief Query the current frame of sprite animation of an instance.
 *
 * @subsubsection SpriteFrame Sprite Animation Frame
 *
 * The current sprite animation frame of an instance is stored as a
 * floating-point value rather than an integer because it also encodes
 * progress to the next frame.
 *
 * The integer portion of this value represents the current frame. The
 * fractional portion represents progress towards the next frame.
 *
 * @param[in] inst Instance of interest.
 *
 * @return Frame of sprite animation or `-1.0f` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 *
 * @sa AERObjectAttachAnimationEndListener
 */
float AERInstanceGetSpriteFrame(AERInstance * inst);

/**
 * @brief Set the current frame of sprite animation of an instance.
 *
 * For more information about the meaning of the sprite frame, see
 * @ref SpriteFrame.
 *
 * @param[in] inst Instance of interest.
 * @param[in] frame Frame of sprite animation.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 *
 * @sa AERObjectAttachAnimationEndListener
 */
void AERInstanceSetSpriteFrame(
		AERInstance * inst,
		float frame
);

/**
 * @brief Query the sprite animation speed of an instance.
 *
 * @param[in] inst Instance of interest.
 *
 * @return Sprite animation speed or `-1.0f` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 *
 * @sa AERObjectAttachAnimationEndListener
 */
float AERInstanceGetSpriteSpeed(AERInstance * inst);

/**
 * @brief Set the sprite animation speed of an instance.
 *
 * @param[in] inst Instance of interest.
 * @param[in] speed Sprite animation speed.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 * @throw ::AER_BAD_VAL if argument `speed` is less than `0.0f`.
 *
 * @since 1.0.0
 *
 * @sa AERObjectAttachAnimationEndListener
 */
void AERInstanceSetSpriteSpeed(
		AERInstance * inst,
		float speed
);

/**
 * @brief Query the sprite alpha (transparency) of an instance.
 *
 * @param[in] inst Instance of interest.
 *
 * @return Sprite alpha or `-1.0f` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 */
float AERInstanceGetSpriteAlpha(AERInstance * inst);

/**
 * @brief Set the sprite alpha (transparency) of an instance.
 *
 * @param[in] inst Instance of interest.
 * @param[in] alpha Sprite alpha.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 * @throw ::AER_BAD_VAL if argument `alpha` is less than `0.0f` or greater
 * than `1.0f`.
 *
 * @since 1.0.0
 */
void AERInstanceSetSpriteAlpha(
		AERInstance * inst,
		float alpha
);

/**
 * @brief Query the sprite offset angle of an instance.
 *
 * @param[in] inst Instance of interest.
 *
 * @return Sprite offset angle in degrees or `0.0f` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 */
float AERInstanceGetSpriteAngle(AERInstance * inst);

/**
 * @brief Set the sprite offset angle of an instance.
 *
 * @param[in] inst Instance of interest.
 * @param[in] angle Sprite offset angle in degrees.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 */
void AERInstanceSetSpriteAngle(
		AERInstance * inst,
		float angle
);

/**
 * @brief Query the sprite scale of an instance.
 *
 * If only one component of the scale is needed, then the argument for the
 * unneeded component may be `NULL`.
 *
 * @param[in] inst Instance of interest.
 * @param[out] x Horizontal scale.
 * @param[out] y Vertical scale.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL` or both arguments
 * `x` and `y` are `NULL`.
 *
 * @since 1.0.0
 */
void AERInstanceGetSpriteScale(
		AERInstance * inst,
		float * x,
		float * y
);

/**
 * @brief Set the sprite scale of an instance.
 *
 * @note The components of the scale may be negative to mirror the sprite
 * about an axis.
 *
 * @param[in] inst Instance of interest.
 * @param[in] x Horizontal scale.
 * @param[in] y Vertical scale.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 */
void AERInstanceSetSpriteScale(
		AERInstance * inst,
		float x,
		float y
);

/**
 * @brief Query the tangibility of an instance.
 *
 * @subsubsection Tangibility Instance Tangibility
 *
 * The tangibility of an instance represents whether or not the player can
 * collide with it. This property is only meaningful for instances of
 * ::AER_OBJECT_PATHFINDOBSTACLE and its children.
 *
 * @param[in] inst Instance of interest.
 *
 * @return Tangibility or `false` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 */
bool AERInstanceGetTangible(AERInstance * inst);

/**
 * @brief Set the tangibility of an instance.
 *
 * For more information about instance tangibility, see @ref Tangibility.
 *
 * @param[in] inst Instance of interest.
 * @param[in] tangible Tangibility.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 *
 * @since 1.0.0
 */
void AERInstanceSetTangible(
		AERInstance * inst,
		bool tangible
);

/**
 * @brief Query the state of an alarm of an instance.
 *
 * @param[in] inst Instance of interest.
 * @param[in] alarmIdx Alarm index.
 *
 * @return Number of alarm steps or `-1` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `alarmIdx` is greater than `11`.
 *
 * @since 1.0.0
 *
 * @sa AERObjectAttachAlarmListener
 */
int32_t AERInstanceGetAlarm(
		AERInstance * inst,
		uint32_t alarmIdx
);

/**
 * @brief Set the state of an alarm of an instance.
 *
 * @param[in] inst Instance of interest.
 * @param[in] alarmIdx Alarm index.
 * @param[in] numSteps Number of alarm steps. Set to `-1` to disable the
 * alarm without triggering the corresponding alarm event.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `alarmIdx` is greater than `11`.
 *
 * @since 1.0.0
 *
 * @sa AERObjectAttachAlarmListener
 */
void AERInstanceSetAlarm(
		AERInstance * inst,
		uint32_t alarmIdx,
		int32_t numSteps
);

/**
 * @brief Query the names of all locals of an instance.
 *
 * @warning Argument `nameBuf` must be large enough to hold at least
 * `bufSize` elements.
 *
 * @note Argument `bufSize` may be `0` in which case argument `nameBuf` may
 * be `NULL`. This may be used to efficiently query the total number of
 * locals that an instance has.
 *
 * @param[in] inst Instance of interest.
 * @param[in] bufSize Maximum number of elements to write to argument
 * `nameBuf`.
 * @param[out] nameBuf Buffer to write names to.
 *
 * @return Total number of locals that the instance has or `0` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `inst` is `NULL` or argument
 * `instBuf` is `NULL` and argument `bufSize` is greater than `0`.
 *
 * @since 1.0.0
 *
 * @sa AERInstanceGetLocal
 */
size_t AERInstanceGetLocals(
		AERInstance * inst,
		size_t bufSize,
		const char ** nameBuf
);

/**
 * @brief Get a reference to a specific local of an instance.
 *
 * @note Because this function returns a reference to the local rather
 * than the value of the local, it acts as both a "getter" and a "setter".
 *
 * The local could be one of several different types including but not
 * limited to `int`, `double` and `char *`. As a result, this function
 * simply returns a `void` pointer to the local. It is the caller's
 * responsibility to cast this pointer to the correct type before
 * dereferencing it.
 *
 * @param[in] inst Instance of interest.
 * @param[in] name Local name.
 *
 * @return Reference to local or `NULL` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if either argument `inst` or `name` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if instance does not have local with name
 * given by argument `name`.
 *
 * @since 1.0.0
 *
 * @sa AERInstanceGetLocals
 */
void * AERInstanceGetLocal(
		AERInstance * inst,
		const char * name
);



#endif /* AER_INSTANCE_H */
