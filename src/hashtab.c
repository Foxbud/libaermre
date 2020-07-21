#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dynarr.h"
#include "hashtab.h"



/* ----- PRIVATE TYPES ----- */

typedef struct SlotNode {
	struct SlotNode * next;
	struct SlotNode * prev;
	void * key;
	uint32_t itemIdx;
} SlotNode;

typedef struct TableItem {
	struct SlotNode * node;
	void * value;
} TableItem;

typedef struct Table {
	size_t keySize;
	size_t numSlots;
	uint32_t slotIdxMask;
	SlotNode ** slots;
	DynArr * items;
	uint32_t (* hashKey)(void *);
	bool (* keysEqual)(void *, void *);
} Table;



/* ----- PRIVATE FUNCTIONS ----- */

static void SlotNodeFree(SlotNode * node) {
	free(node->key);
	free(node);

	return;
}

static SlotNode * SlotAddNode(
		Table * table,
		uint32_t slotIdx,
		void * key,
		uint32_t itemIdx
) {
	/* Initialize node. */
	SlotNode * node = malloc(sizeof(SlotNode));
	assert(node);
	node->itemIdx = itemIdx;

	/* Update connections. */
	SlotNode * first = table->slots[slotIdx];
	if (first) first->prev = node;
	node->next = first;
	node->prev = NULL;

	/* Update slot. */
	table->slots[slotIdx] = node;

	/* Copy key. */
	node->key = malloc(table->keySize);
	assert(node->key);
	memcpy(node->key, key, table->keySize);

	return node;
}

static void SlotRemoveNode(
		Table * table,
		uint32_t slotIdx,
		SlotNode * node
) {
	/* Update slot. */
	if (table->slots[slotIdx] == node) table->slots[slotIdx] = node->next;

	/* Update connections. */
	if (node->next) node->next->prev = node->prev;
	if (node->prev) node->prev->next = node->next;

	SlotNodeFree(node);

	return;
}

static SlotNode * SlotFindNode(
		Table * table,
		uint32_t slotIdx,
		void * key
) {
	SlotNode * node = table->slots[slotIdx];

	while (node) {
		if (table->keysEqual(key, node->key)) break;
		node = node->next;
	}

	return node;
}

static uint32_t SlotGetIndex(
		Table * table,
		void * key
) {
	return table->hashKey(key) & table->slotIdxMask;
}

static void * RemoveItem(Table * table, uint32_t itemIdx) {
	void * result = NULL;

	DynArr * items = table->items;
	TableItem * item = DynArrGet(items, itemIdx);
	result = item->value;
	free(item);
	TableItem * last = DynArrPop(items);
	if (DynArrSize(items) != itemIdx) {
		DynArrSet(items, itemIdx, last);
		item->node->itemIdx = itemIdx;
	}

	return result;
}



/* ----- PUBLIC FUNCTIONS ----- */

HashTab * HashTabNew(
		uint32_t slotMagnitude,
		size_t keySize,
		uint32_t (* hashKey)(void * key),
		bool (* keysEqual)(void * keyA, void * keyB)
) {
	assert(hashKey);
	assert(keysEqual);
	assert(slotMagnitude > 1 && slotMagnitude < 32);

	Table * table = malloc(sizeof(Table));
	assert(table);
	table->keySize = keySize;
	size_t numSlots = 1 << (slotMagnitude - 1);
	table->numSlots = numSlots;
	table->slotIdxMask = numSlots - 1;
	table->slots = calloc(numSlots, sizeof(SlotNode *));
	assert(table->slots);
	table->items = DynArrNew(32);
	table->hashKey = hashKey;
	table->keysEqual = keysEqual;

	return (HashTab *)table;
}

void HashTabFree(HashTab * table) {
	assert(table);

	/* Free items and slot nodes. */
	DynArr * items = ((Table *)table)->items;
	size_t numItems = DynArrSize(items);
	for (uint32_t idx = 0; idx < numItems; idx++) {
		TableItem * item = DynArrGet(items, idx);
		SlotNodeFree(item->node);
		free(item);
	}

	/* Free arrays. */
	DynArrFree(items);
	free(((Table *)table)->slots);

	free(table);

	return;
}

size_t HashTabSize(HashTab * table) {
	assert(table);

	return DynArrSize(((Table *)table)->items);
}

bool HashTabExists(
		HashTab * table,
		void * key
) {
	assert(table);
	assert(key);

	uint32_t slotIdx = SlotGetIndex((Table *)table, key);

	return SlotFindNode((Table *)table, slotIdx, key) != NULL;
}

void * HashTabGet(
		HashTab * table,
		void * key,
		bool * exists
) {
	assert(table);
	assert(key);
	void * result = NULL;
	bool tmpExists = false;

	uint32_t slotIdx = SlotGetIndex((Table *)table, key);
	SlotNode * node = SlotFindNode((Table *)table, slotIdx, key);
	tmpExists = node != NULL;
	assert(tmpExists || exists != NULL);

	if (tmpExists) {
		TableItem * item = DynArrGet(((Table *)table)->items, node->itemIdx);
		result = item->value;
	}

	if (exists) {
		*exists = tmpExists;
	}

	return result;
}

void HashTabSet(
		HashTab * table,
		void * key,
		void * value
) {
	assert(table);
	assert(key);

	uint32_t slotIdx = SlotGetIndex((Table *)table, key);
	SlotNode * node = SlotFindNode((Table *)table, slotIdx, key);
	assert(node);
	TableItem * item = DynArrGet(((Table *)table)->items, node->itemIdx);
	item->value = value;

	return;
}

void HashTabInsert(
		HashTab * table,
		void * key,
		void * value
) {
	assert(table);
	assert(key);

	/* Get slot. */
	uint32_t slotIdx = SlotGetIndex((Table *)table, key);
	assert(!SlotFindNode((Table *)table, slotIdx, key));

	/* Create item. */
	DynArr * items = ((Table *)table)->items;
	TableItem * item = malloc(sizeof(TableItem));
	assert(item);
	item->value = value;
	uint32_t itemIdx = DynArrSize(items);
	DynArrPush(items, item);

	/* Create node. */
	item->node = SlotAddNode(
			(Table *)table,
			slotIdx,
			key,
			itemIdx
	);

	return;
}

void * HashTabRemove(
		HashTab * table,
		void * key
) {
	assert(table);
	assert(key);
	void * result = NULL;

	/* Get slot and node. */
	uint32_t slotIdx = SlotGetIndex((Table *)table, key);
	SlotNode * node = SlotFindNode((Table *)table, slotIdx, key);
	assert(node);

	/* Remove item. */
	result = RemoveItem((Table *)table, node->itemIdx);

	/* Remove node. */
	SlotRemoveNode((Table *)table, slotIdx, node);

	return result;
}

void HashTabEach(
		HashTab * table,
		void (* callback)(void * key, void * value, void * context),
		void * context
) {
	assert(table);
	assert(callback);

	DynArr * items = ((Table *)table)->items;
	size_t numItems = DynArrSize(items);
	for (uint32_t idx = 0; idx < numItems; idx++) {
		TableItem * item = DynArrGet(items, idx);
		callback(item->node->key, item->value, context);
	}

	return;
}
