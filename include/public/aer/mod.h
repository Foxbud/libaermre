/**
 * @file
 *
 * @brief Defining and initializing mods.
 *
 * When the MRE successfully loads a mod library, it first searches for and
 * calls the mod's public definition function, which must have the signature:
 *
 * @code{.c}
 * void define_mod(AERModDef *def);
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
 * @warning As new features get added to the framework, this struct may receive
 * new fields corresponding to those features **without** resulting in a new
 * major version number. However, field ordering is guaranteed to remain the
 * same for all releases with the same major version number. To ensure forward
 * API compatability, only work with pointers to this struct.
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
     * @sa AERObjectAttachDrawListener
     * @sa AERObjectAttachGUIDrawListener
     * @sa AERObjectAttachRoomStartListener
     * @sa AERObjectAttachRoomEndListener
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
     * @sa AERGetPaused
     *
     * @memberof AERModDef
     */
    void (*gamePauseListener)(bool paused);
#define void __attribute__((deprecated)) void
    /**
     * @var roomChangeListener
     *
     * @brief This member is a deprecated alias for @ref
     * AERModDef::roomStartListener.
     *
     * @deprecated Since {{MRE_NEXT_MINOR}}. Use @ref
     * AERModDef::roomStartListener instead.
     *
     * @since 1.0.0
     *
     * @sa AERModDef::roomStartListener
     * @sa AERModDef::roomEndListener
     *
     * @memberof AERModDef
     */
    void (*roomChangeListener)(int32_t newRoomIdx, int32_t prevRoomIdx);
#undef void
    /**
     * @var registerFonts
     *
     * @brief Callback function which registers a mod's fonts.
     *
     * @note May be `NULL` if mod does not register any fonts.
     *
     * @since 1.1.0
     *
     * @sa AERFontRegister
     *
     * @memberof AERModDef
     */
    void (*registerFonts)(void);
    /**
     * @var gameSaveListener
     *
     * @brief Mod's game save pseudo-event listener.
     *
     * If provided and the game data gets saved, the MRE will call this function
     * immediately after the game has finalized its vanilla save data but
     * immediately before that data has been written to the savefile.
     *
     * This means that any calls to the functions in save.h from within this
     * listener **will** make it into the savefile. However, this also means
     * that any changes made to vanilla state (i.e. changing the number of
     * gearbits that the player has) **will not** make it into the savefile.
     *
     * @note The functions in save.h can be called from anywhere within a mod,
     * not just from within this listener.
     *
     * @note May be `NULL`.
     *
     * @param[in] curSlotIdx Index of save slot to which the game is saving
     * data.
     *
     * @since 1.2.0
     *
     * @sa save.h
     *
     * @memberof AERModDef
     */
    void (*gameSaveListener)(int32_t curSlotIdx);
    /**
     * @var gameLoadListener
     *
     * @brief Mod's game load pseudo-event listener.
     *
     * If provided and the game data gets loaded, the MRE will call this
     * function immediately after the game reads and parses data from the
     * savefile but immediately before that gets applied to any in-game state.
     *
     * This means that any calls to the functions in save.h from within this
     * listener **will** be saved at the next save point. However, this also
     * means that any changes made to vanilla state (i.e. changing the number of
     * gearbits that the player has) **will** be overwritten and **will not** be
     * saved at the next save point.
     *
     * @note The functions in save.h can be called from anywhere within a mod,
     * not just from within this listener.
     *
     * @note May be `NULL`.
     *
     * @param[in] curSlotIdx Index of save slot from which the game is loading
     * data.
     *
     * @since 1.2.0
     *
     * @sa save.h
     *
     * @memberof AERModDef
     */
    void (*gameLoadListener)(int32_t curSlotIdx);
    /**
     * @var roomStartListener
     *
     * @since {{MRE_NEXT_MINOR}}
     *
     * @sa AERModDef::roomEndListener
     *
     * @memberof AERModDef
     */
    void (*roomStartListener)(int32_t newRoomIdx, int32_t prevRoomIdx);
    /**
     * @var roomEndListener
     *
     * @since {{MRE_NEXT_MINOR}}
     *
     * @sa AERModDef::roomStartListener
     *
     * @memberof AERModDef
     */
    void (*roomEndListener)(int32_t newRoomIdx, int32_t prevRoomIdx);
} AERModDef;

#endif /* AER_MOD_H */
