/**
 * @file
 *
 * @brief Defining and initializing mods.
 *
 * When the MRE successfully loads a mod library, it first searches for and
 * calls the mod's public definition function, which must have the signature
 * `AERModDef define_mod(void)`.
 *
 * In an attempt to accommodate varied naming conventions, this function may
 * be named any of the following:
 *  - `define_mod`
 *  - `definemod`
 *  - `defineMod`
 *  - `DefineMod`
 *
 * @since 1.0.0
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
#ifndef AER_MODMAN_H
#define AER_MODMAN_H

#include <stdint.h>



/* ----- PUBLIC TYPES ----- */

/**
 * @brief Definition of a mod.
 *
 * @since 1.0.0
 */
typedef struct AERModDef {
	/* Registration callbacks. */
	void (* regSprites)(void);
	void (* regObjects)(void);
	void (* regObjListeners)(void);
	/* Pseudoevent listeners. */
	void (* roomStepListener)(void);
	void (* roomChangeListener)(int32_t newRoomIdx, int32_t prevRoomIdx);
} AERModDef;

/**
 * @var regSprites
 *
 * @brief Callback function which registers a mod's sprites.
 *
 * @note May be `NULL` if mod does not register/replace any sprites.
 *
 * @since 1.0.0
 *
 * @sa AERSpriteRegister()
 * @sa AERSpriteReplace()
 *
 * @memberof AERModDef
 */

/**
 * @var regObjects
 *
 * @brief Callback function which registers a mod's objects.
 *
 * @note May be `NULL` if mod does not register any objects.
 *
 * @since 1.0.0
 *
 * @sa AERObjectRegister()
 *
 * @memberof AERModDef
 */

/**
 * @var regObjListeners
 *
 * @brief Callback function which registers a mod's object event listeners.
 *
 * @note May be `NULL` if mod does not register any object event listeners.
 *
 * @since 1.0.0
 *
 * @sa AERObjectAttachCreateListener()
 * @sa AERObjectAttachDestroyListener()
 * @sa AERObjectAttachAlarmListener()
 * @sa AERObjectAttachStepListener()
 * @sa AERObjectAttachPreStepListener()
 * @sa AERObjectAttachPostStepListener()
 * @sa AERObjectAttachCollisionListener()
 * @sa AERObjectAttachAnimationEndListener()
 *
 * @memberof AERModDef
 */

/**
 * @var roomStepListener
 *
 * @brief Mod's room step pseudo-event listener.
 *
 * This is effectively the global "tick" function of a mod. If provided, the
 * MRE will unconditionally call it at the very start of every in-game
 * step (before any object event listeners are called, including pre-step
 * listeners).
 *
 * This event does not exist in the GameMaker engine; the MRE
 * provides it as a convenience to mod developers.
 *
 * @note May be `NULL`.
 *
 * @since 1.0.0
 *
 * @memberof AERModDef
 */

/**
 * @var roomChangeListener
 *
 * @brief Mod's room change pseudo-event listener.
 *
 * If provided and the active game room changes, the MRE will call this
 * function at the very start of the new room's first step
 * (immediately before calling roomStepListener()).
 *
 * This event does not exist in the GameMaker engine; the MRE
 * provides it as a convenience to mod developers.
 *
 * @warning Will **not** be called if active room is reset.
 *
 * @note May be `NULL`.
 *
 * @param[in] newRoomIdx Index of new (current) room.
 * @param[in] prevRoomIdx Index of previous room.
 *
 * @since 1.0.0
 *
 * @memberof AERModDef
 */



#endif /* AER_MODMAN_H */
