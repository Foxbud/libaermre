#ifndef INTERNAL_MODMAN_H
#define INTERNAL_MODMAN_H

#include <stdbool.h>
#include <stdint.h>

#include "foxutils/array.h"

#include "aer/mre.h"
#include "internal/hld.h"



/* ----- INTERNAL TYPES ----- */

typedef struct Mod {
	void * libHandle;
	const char * name;
	char * slug;
	void (* regSprites)(void);
	void (* regObjects)(void);
	void (* regObjListeners)(void);
} Mod;

typedef struct ModListener {
	union {
		bool (* hldObj)(HLDInstance *);
		bool (* hldObjPair)(HLDInstance *, HLDInstance *);
		bool (* aerObj)(AERInstance *);
		bool (* aerObjPair)(AERInstance *, AERInstance *);
		/* Pseudoevents. */
		void (* roomStep)(void);
		void (* roomChange)(int32_t, int32_t);
	} func;
	Mod * mod;
} ModListener;

typedef struct ModMan {
	FoxArray mods;
	FoxArray context;
	FoxArray roomStepListeners;
	FoxArray roomChangeListeners;
} ModMan;



/* ----- INTERNAL GLOBALS ----- */

extern ModMan modman;



/* ----- INTERNAL FUNCTIONS ----- */

void ModManConstructor(void);

void ModManDestructor(void);



#endif /* INTERNAL_MODMAN_H */
