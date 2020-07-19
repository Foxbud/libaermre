#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dynarr.h"



/* ----- PRIVATE TYPES ----- */

typedef struct Array {
	size_t size;
	size_t capacity;
	void ** data;
} Array;



/* ----- PRIVATE CONSTANTS ----- */

static const uint32_t ARRAY_GROWTH_RATE = 2;



/* ----- PRIVATE FUNCTIONS ----- */

static void ArrayGrow(Array * array) {
	assert(array);

	array->capacity *= ARRAY_GROWTH_RATE;
	array->data = realloc(array->data, array->capacity * sizeof(void *));
	assert(array->data);

	return;
}



/* ----- PUBLIC FUNCTIONS ----- */

DynArr * DynArrNew(size_t capacity) {
	assert(capacity != 0);

	Array * array = malloc(sizeof(Array));
	assert(array);
	array->size = 0;
	array->capacity = capacity;
	array->data = calloc(capacity, sizeof(void *));
	assert(array->data);

	return (DynArr *)array;
}

void DynArrFree(DynArr * array) {
	assert(array);

	free(((Array *)array)->data);
	free(array);

	return;
}

size_t DynArrSize(DynArr * array) {
	assert(array);

	return ((Array *)array)->size;
}

void * DynArrGet(
		DynArr * array,
		uint32_t idx
) {
	assert(array);
	assert(idx < ((Array *)array)->size);

	return ((Array *)array)->data[idx];
}

void DynArrSet(
		DynArr * array,
		uint32_t idx,
		void * val
) {
	assert(array);
	assert(idx < ((Array *)array)->size);

	((Array *)array)->data[idx] = val;

	return;
}

void DynArrInsert(
		DynArr * array,
		uint32_t idx,
		void * val
) {
	assert(array);
	assert(idx <= ((Array *)array)->size);

	if (++(((Array *)array)->size) > ((Array *)array)->capacity) {
		ArrayGrow((Array *)array);
	}

	size_t numToMove = ((Array *)array)->size - idx - 1;
	if (numToMove > 0) {
		memmove(
				((Array *)array)->data + idx + 1,
				((Array *)array)->data + idx,
				numToMove * sizeof(void *)
		);
	}
	((Array *)array)->data[idx] = val;

	return;
}

void DynArrPush(
		DynArr * array,
		void * val
) {
	assert(array);

	DynArrInsert(array, ((Array *)array)->size, val);

	return;
}

void * DynArrRemove(
		DynArr * array,
		uint32_t idx
) {
	assert(array);
	assert(idx < ((Array *)array)->size);
	void * result = ((Array *)array)->data[idx];

	size_t numToMove = --(((Array *)array)->size) - idx;
	if (numToMove > 0) {
		memmove(
				((Array *)array)->data + idx,
				((Array *)array)->data + idx + 1,
				numToMove * sizeof(void *)
		);
	}

	return result;
}

void * DynArrPop(DynArr * array) {
	assert(array);

	return DynArrRemove(array, ((Array *)array)->size - 1);
}
