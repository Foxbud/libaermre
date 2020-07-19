#include <assert.h>
#include <stdlib.h>
#include "dynarr.h"
#include "hashtab.h"



/* ----- PRIVATE TYPES ----- */

typedef struct TableItem {
	uint32_t key;
	uint32_t valueIdx;
} TableItem;

typedef struct TableValue {
	TableItem * item;
	void * raw;
} TableValue;

typedef struct Table {
	uint32_t numSlots;
	DynArr ** slots;
	DynArr * values;
} Table;



/* ----- PRIVATE CONSTANTS ----- */

static const size_t INIT_SLOT_CAP = 8;



/* ----- PRIVATE FUNCTIONS ----- */

static TableItem * LookupItem(
		Table * table,
		uint32_t key,
		DynArr ** slot,
		uint32_t * itemIdx
) {
	TableItem * result = NULL;
	DynArr * tmpSlot = NULL;
	uint32_t tmpItemIdx = 0;

	uint32_t slotIdx = key & table->numSlots;
	if ((tmpSlot = table->slots[slotIdx])) {
		size_t numItems = DynArrSize(tmpSlot);
		for (uint32_t idx = 0; idx < numItems; idx++) {
			TableItem * item = DynArrGet(tmpSlot, idx);
			if (item->key == key) {
				result = item;
				tmpItemIdx = idx;
				break;
			}
		}
	}

	if (slot) {
		*slot = tmpSlot;
	}
	if (itemIdx) {
		*itemIdx = tmpItemIdx;
	}

	return result;
}



/* ----- PUBLIC FUNCTIONS ----- */

HashTab * HashTabNew(uint32_t slotMagnitude) {
	assert(slotMagnitude > 1 && slotMagnitude < 32);

	Table * table = malloc(sizeof(Table));
	assert(table);
	size_t numSlots = (1 << (slotMagnitude - 1)) - 1;
	table->numSlots = numSlots;
	table->slots = calloc(numSlots, sizeof(DynArr *));
	assert(table->slots);
	table->values = DynArrNew(32);

	return (HashTab *)table;
}

void HashTabFree(HashTab * table) {
	assert(table);

	/* Free values and items. */
	DynArr * values = ((Table *)table)->values;
	while (DynArrSize(values) > 0) {
		TableValue * value = DynArrPop(values);
		free(value->item);
		free(value);
	}
	DynArrFree(values);

	/* Free slots. */
	size_t numSlots = ((Table *)table)->numSlots;
	DynArr ** slots = ((Table *)table)->slots;
	for (uint32_t slotIdx = 0; slotIdx < numSlots; slotIdx++) {
		DynArr * slot;
		if ((slot = slots[slotIdx])) {
			DynArrFree(slot);
		}
	}
	free(slots);

	free(table);

	return;
}

size_t HashTabSize(HashTab * table) {
	assert(table);

	return DynArrSize(((Table *)table)->values);
}

bool HashTabExists(
		HashTab * table,
		uint32_t key
) {
	assert(table);

	return LookupItem((Table *)table, key, NULL, NULL) != NULL;
}

void * HashTabGet(
		HashTab * table,
		uint32_t key,
		bool * exists
) {
	assert(table);
	void * result = NULL;
	bool tmpExists = false;

	TableItem * item = LookupItem((Table *)table, key, NULL, NULL);
	tmpExists = item != NULL;
	assert(tmpExists || exists != NULL);

	if (tmpExists) {
		TableValue * value = DynArrGet(((Table *)table)->values, item->valueIdx);
		result = value->raw;
	}

	if (exists) {
		*exists = tmpExists;
	}

	return result;
}

void HashTabSet(
		HashTab * table,
		uint32_t key,
		void * value
) {
	assert(table);

	/* Get slot. */
	DynArr ** slots = ((Table *)table)->slots;
	uint32_t slotIdx = key & ((Table *)table)->numSlots;
	DynArr * slot;
	/* Ensure slot has been initialized. */
	if (!(slot = slots[slotIdx])) {
		slots[slotIdx] = slot = DynArrNew(INIT_SLOT_CAP);
	}

	/* Get item. */
	TableItem * item = NULL;
	size_t numItems = DynArrSize(slot);
	for (uint32_t idx = 0; idx < numItems; idx++) {
		TableItem * tmpItem = DynArrGet(slot, idx);
		if (tmpItem->key == key) {
			item = tmpItem;
			break;
		}
	}

	/* Get value. */
	DynArr * values = ((Table *)table)->values;
	TableValue * tabVal;
	/* Item & value exist. */
	if (item) {
		tabVal = DynArrGet(values, item->valueIdx);
	}
	/* Item & value do not exist. */
	else {
		/* Create item. */
		item = malloc(sizeof(TableItem));
		assert(item);
		item->key = key;
		item->valueIdx = DynArrSize(values);
		DynArrPush(slot, item);
		/* Create value. */
		tabVal = malloc(sizeof(TableValue));
		assert(tabVal);
		tabVal->item = item;
		DynArrPush(values, tabVal);
	}

	/* Set value. */
	tabVal->raw = value;

	return;
}

void * HashTabRemove(
		HashTab * table,
		uint32_t key
) {
	assert(table);
	void * result = NULL;

	/* Get value. */
	DynArr * values = ((Table *)table)->values;
	DynArr * slot;
	uint32_t itemIdx;
	TableItem * item = LookupItem((Table *)table, key, &slot, &itemIdx);
	assert(item);
	TableValue * value = DynArrGet(values, item->valueIdx);
	result = value->raw;

	/* Remove value. */
	free(value);
	value = NULL;
	uint32_t removedValIdx = item->valueIdx;
	TableValue * lastVal = DynArrPop(values);
	/* Move last value into removed value's position. */
	if (removedValIdx != DynArrSize(values)) {
		DynArrSet(values, removedValIdx, lastVal);
		lastVal->item->valueIdx = removedValIdx;
	}
	/* Removed value was last value. */
	else {
		lastVal = NULL;
	}

	/* Remove item. */
	free(item);
	item = NULL;
	DynArrRemove(slot, itemIdx);

	return result;
}

void HashTabEach(
		HashTab * table,
		void (* callback)(uint32_t key, void * value)
) {
	assert(table);
	assert(callback);

	DynArr * values = ((Table *)table)->values;
	size_t numValues = DynArrSize(values);
	for (uint32_t idx = 0; idx < numValues; idx++) {
		TableValue * value = DynArrGet(values, idx);
		callback(value->item->key, value->raw);
	}

	return;
}
