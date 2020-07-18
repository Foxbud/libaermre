#include <assert.h>
#include <stdlib.h>
#include "dynarr.h"



/* ----- PRIVATE TYPES ----- */

typedef struct Array {
	size_t size;
	size_t capacity;
	void ** data;
} Array;



/* ----- PUBLIC FUNCTIONS ----- */

DynArr * DynArrNew(
		size_t elemSize,
		size_t capacity
) {
	assert(capacity != 0);

	Array * array = malloc(sizeof(Array));
	array->size = 0;
	array->capacity = capacity;
	array->data = calloc(capacity, elemSize);
	assert(array->data);

	return array;
}

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
