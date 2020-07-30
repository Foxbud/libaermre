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

typedef struct TableIter {
	Table * table;
	uint32_t nextIdx;
} TableIter;



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
	assert(slotMagnitude > 0 && slotMagnitude < 32);

	Table * table = malloc(sizeof(Table));
	assert(table);
	table->keySize = keySize;
	size_t numSlots = 1 << slotMagnitude;
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
#define table ((Table *)table)
	assert(table);

	/* Free items and slot nodes. */
	DynArr * items = table->items;
	size_t numItems = DynArrSize(items);
	for (uint32_t idx = 0; idx < numItems; idx++) {
		TableItem * item = DynArrGet(items, idx);
		SlotNodeFree(item->node);
		free(item);
	}

	/* Free arrays. */
	DynArrFree(items);
	free(table->slots);

	free(table);

	return;
#undef table
}

size_t HashTabSize(HashTab * table) {
	assert(table);

	return DynArrSize(((Table *)table)->items);
}

bool HashTabExists(
		HashTab * table,
		void * key
) {
#define table ((Table *)table)
	assert(table);
	assert(key);

	uint32_t slotIdx = SlotGetIndex(table, key);

	return SlotFindNode(table, slotIdx, key) != NULL;
#undef table
}

void * HashTabGet(
		HashTab * table,
		void * key,
		bool * exists
) {
#define table ((Table *)table)
	assert(table);
	assert(key);
	void * result = NULL;
	bool tmpExists = false;

	uint32_t slotIdx = SlotGetIndex(table, key);
	SlotNode * node = SlotFindNode(table, slotIdx, key);
	tmpExists = node != NULL;
	assert(tmpExists || exists != NULL);

	if (tmpExists) {
		TableItem * item = DynArrGet(table->items, node->itemIdx);
		result = item->value;
	}

	if (exists) {
		*exists = tmpExists;
	}

	return result;
#undef table
}

void HashTabSet(
		HashTab * table,
		void * key,
		void * value
) {
#define table ((Table *)table)
	assert(table);
	assert(key);

	uint32_t slotIdx = SlotGetIndex(table, key);
	SlotNode * node = SlotFindNode(table, slotIdx, key);
	assert(node);
	TableItem * item = DynArrGet(table->items, node->itemIdx);
	item->value = value;

	return;
#undef table
}

void HashTabInsert(
		HashTab * table,
		void * key,
		void * value
) {
#define table ((Table *)table)
	assert(table);
	assert(key);

	/* Get slot. */
	uint32_t slotIdx = SlotGetIndex(table, key);
	assert(!SlotFindNode(table, slotIdx, key));

	/* Create item. */
	DynArr * items = table->items;
	TableItem * item = malloc(sizeof(TableItem));
	assert(item);
	item->value = value;
	uint32_t itemIdx = DynArrSize(items);
	DynArrPush(items, item);

	/* Create node. */
	item->node = SlotAddNode(
			table,
			slotIdx,
			key,
			itemIdx
	);

	return;
#undef table
}

void * HashTabRemove(
		HashTab * table,
		void * key
) {
#define table ((Table *)table)
	assert(table);
	assert(key);
	void * result = NULL;

	/* Get slot and node. */
	uint32_t slotIdx = SlotGetIndex(table, key);
	SlotNode * node = SlotFindNode(table, slotIdx, key);
	assert(node);

	/* Remove item. */
	result = RemoveItem(table, node->itemIdx);

	/* Remove node. */
	SlotRemoveNode(table, slotIdx, node);

	return result;
#undef table
}

HashTabIter * HashTabIterNew(HashTab * table) {
	assert(table);

	TableIter * iter = malloc(sizeof(TableIter));
	assert(iter);
	iter->table = (Table *)table;
	iter->nextIdx = 0;

	return (HashTabIter *)iter;
}

void HashTabIterFree(HashTabIter * iter) {
	assert(iter);

	free(iter);

	return;
}

bool HashTabIterNext(
		HashTabIter * iter,
		const void ** key,
		void ** value
) {
#define iter ((TableIter *)iter)
	assert(iter);
	bool success;

	Table * table = iter->table;
	uint32_t curIdx = iter->nextIdx;
	size_t numItems = DynArrSize(table->items);
	if ((success = curIdx < numItems)) {
		TableItem * item = DynArrGet(table->items, curIdx);
		if (key) *key = item->node->key;
		if (value) *value = item->value;
		iter->nextIdx++;
	}

	return success;
#undef iter
}
