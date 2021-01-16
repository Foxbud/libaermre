/**
 * @file
 *
 * @brief Defining and initializing mods.
 *
 * When the MRE successfully loads a mod library, it first searches for and
 * calls the mod's public definition function, which must have the signature:
 *
 * @code{.c}
 * AERModDef define_mod(void);
 * @endcode
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
 * @copyright 2021 the libaermre authors
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
#ifndef AER_MOD_H
#define AER_MOD_H

#include <stdbool.h>
#include <stdint.h>

/* ----- PUBLIC TYPES ----- */

/**
 * @brief Definition of a mod.
 *
 * @since 1.0.0
 */
typedef struct AERModDef {
  /**
   * @var constructor
   *
   * @brief Callback function which constructs a mod.
   *
   * @note Prefer using this over `__attribute__((constructor))`.
   * @note May be `NULL` if mod does not need construction.
   *
   * @since 1.0.0
   *
   * @memberof AERModDef
   */
  void (*constructor)(void);
  /**
   * @var destructor
   *
   * @brief Callback function which destructs a mod.
   *
   * @note Prefer using this over `__attribute__((destructor))`.
   * @note May be `NULL` if mod does not need destruction.
   *
   * @since 1.0.0
   *
   * @memberof AERModDef
   */
  void (*destructor)(void);
  /**
   * @var registerSprites
   *
   * @brief Callback function which registers a mod's sprites.
   *
   * @note May be `NULL` if mod does not register/replace any sprites.
   *
   * @since 1.0.0
   *
   * @sa AERSpriteRegister
   * @sa AERSpriteReplace
   *
   * @memberof AERModDef
   */
  void (*registerSprites)(void);
  /**
   * @var registerObjects
   *
   * @brief Callback function which registers a mod's objects.
   *
   * @note May be `NULL` if mod does not register any objects.
   *
   * @since 1.0.0
   *
   * @sa AERObjectRegister
   *
   * @memberof AERModDef
   */
  void (*registerObjects)(void);
  /**
   * @var registerObjectListeners
   *
   * @brief Callback function which registers a mod's object event listeners.
   *
   * @note May be `NULL` if mod does not register any object event listeners.
   *
   * @since 1.0.0
   *
   * @sa AERObjectAttachCreateListener
   * @sa AERObjectAttachDestroyListener
   * @sa AERObjectAttachAlarmListener
   * @sa AERObjectAttachStepListener
   * @sa AERObjectAttachPreStepListener
   * @sa AERObjectAttachPostStepListener
   * @sa AERObjectAttachCollisionListener
   * @sa AERObjectAttachAnimationEndListener
   *
   * @memberof AERModDef
   */
  void (*registerObjectListeners)(void);
  /**
   * @var gameStepListener
   *
   * @brief Mod's game step pseudo-event listener.
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
  void (*gameStepListener)(void);
  /**
   * @var gamePauseListener
   *
   * @brief Mod's game pause pseudo-event listener.
   *
   * If provided and the game is paused or unpaused, the MRE will call this
   * function at the very start of the first step of the new pause state
   * (immediately before calling gameStepListener).
   *
   * This event does not exist in the GameMaker engine; the MRE
   * provides it as a convenience to mod developers.
   *
   * @note May be `NULL`.
   *
   * @param[in] paused Whether game was paused (`true`) or unpaused (`false`).
   *
   * @since 1.0.0
   *
   * @memberof AERModDef
   */
  void (*gamePauseListener)(bool paused);
  /**
   * @var roomChangeListener
   *
   * @brief Mod's room change pseudo-event listener.
   *
   * If provided and the active game room changes, the MRE will call this
   * function at the very start of the new room's first step
   * (immediately before calling gameStepListener).
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
  void (*roomChangeListener)(int32_t newRoomIdx, int32_t prevRoomIdx);
} AERModDef;

#endif /* AER_MOD_H */
