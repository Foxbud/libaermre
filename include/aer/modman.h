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
 *	- `define_mod`
 *	- `definemod`
 *	- `defineMod`
 *	- `DefineMod`
 */
#ifndef AER_MODMAN_H
#define AER_MODMAN_H

#include <stdint.h>



/* ----- PUBLIC TYPES ----- */

/**
 * @brief Definition of a mod.
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
 * @relates AERSpriteRegister()
 * @relates AERSpriteReplace()
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
 * @relates AERObjectRegister()
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
 * @relates AERObjectAttachCreateListener()
 * @relates AERObjectAttachDestroyListener()
 * @relates AERObjectAttachAlarmListener()
 * @relates AERObjectAttachStepListener()
 * @relates AERObjectAttachPreStepListener()
 * @relates AERObjectAttachPostStepListener()
 * @relates AERObjectAttachCollisionListener()
 * @relates AERObjectAttachAnimationEndListener()
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
 * @memberof AERModDef
 */



#endif /* AER_MODMAN_H */
