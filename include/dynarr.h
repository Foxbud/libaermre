#ifndef DYNARR_H
#define DYNARR_H

#include <stddef.h>
#include <stdint.h>



/* ----- PUBLIC TYPES ----- */

typedef struct DynArr {
	const uint8_t rawData[4 * 3];
} DynArr;



/* ----- PUBLIC FUNCTIONS ----- */

DynArr * DynArrNew(
		size_t elemSize,
		size_t capacity
);

void DynArrFree(DynArr * array);

size_t DynArrSize(DynArr * array);

void * DynArrGet(
		size_t elemSize,
		uint32_t idx
);

void DynArrSet(
		size_t elemSize,
		uint32_t idx,
		void * val
);



#endif /* DYNARR_H */
