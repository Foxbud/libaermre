#ifndef DYNARR_H
#define DYNARR_H

#include <stddef.h>
#include <stdint.h>



/* ----- PUBLIC TYPES ----- */

typedef struct DynArr {
	const uint8_t rawData[4 * 3];
} DynArr;



/* ----- PUBLIC FUNCTIONS ----- */

DynArr * DynArrNew(size_t capacity);

void DynArrFree(DynArr * array);

size_t DynArrSize(DynArr * array);

void * DynArrGet(
		DynArr * array,
		uint32_t idx
);

void DynArrSet(
		DynArr * array,
		uint32_t idx,
		void * val
);

void DynArrInsert(
		DynArr * array,
		uint32_t idx,
		void * val
);

void DynArrPush(
		DynArr * array,
		void * val
);

void * DynArrRemove(
		DynArr * array,
		uint32_t idx
);

void * DynArrPop(DynArr * array);



#endif /* DYNARR_H */
