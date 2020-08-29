#ifndef INTERNAL_MRE_H
#define INTERNAL_MRE_H

#include <stdint.h>

#include "foxutils/array.h"
#include "foxutils/map.h"

#include "internal/eventkey.h"
#include "internal/hld.h"
#include "internal/modman.h"
#include "internal/objtree.h"



/* ----- INTERNAL TYPES ----- */

/* This struct represents the current state of the mod runtime environment. */
typedef struct AERMRE {
	/*
	 * Index of active room during previous game step. Solely for detecting
	 * room changes.
	 */
	int32_t roomIndexPrevious;
	/* Inheritance tree of all objects (including mod-registered). */
	ObjTree * objTree;
	/*
	 * Hash table of all events that have been entrapped during the mod
	 * event listener registration stage.
	 */
	FoxMap * eventTraps;
	/* Key of currently active event. */
	EventKey currentEvent;
	/* 
	 * Because C lacks enclosures, all entrapped events point to this common
	 * event handler which then looks up and executes the trap for the
	 * current event.
	 */
	HLDNamedFunction eventHandler;
	/* Internal record of all subscriptions to subscribable events. */
	FoxMap * eventSubscribers;
	/* Current stage of the MRE. */
	enum {
		STAGE_INIT,
		STAGE_SPRITE_REG,
		STAGE_OBJECT_REG,
		STAGE_LISTENER_REG,
		STAGE_ACTION
	} stage;
} AERMRE;



/* ----- INTERNAL CONSTANTS ----- */

extern const char * MRE_ASSET_PATH_FMT;



/* ----- INTERNAL GLOBALS ----- */

extern AERMRE mre;



/* ----- INTERNAL FUNCTIONS ----- */


void MRERegisterEventListener(
		HLDObject * obj,
		EventKey key,
		ModListener listener,
		bool downstream
);



#endif /* INTERNAL_MRE_H */
