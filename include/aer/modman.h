/**
 * @file
 */
#ifndef AER_MODMAN_H
#define AER_MODMAN_H

#include <stdint.h>



/* ----- PUBLIC TYPES ----- */

typedef struct AERModDef {
	/* Registration callbacks. */
	void (* regSprites)(void);
	void (* regObjects)(void);
	void (* regObjListeners)(void);
	/* Pseudoevent listeners. */
	void (* roomStepListener)(void);
	void (* roomChangeListener)(int32_t newRoomIdx, int32_t prevRoomIdx);
} AERModDef;



#endif /* AER_MODMAN_H */
