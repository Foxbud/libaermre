#ifndef HASHTAB_H
#define HASHTAB_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



/* ----- PUBLIC TYPES ----- */

typedef struct HashTab {
	const uint8_t rawData[4 * 7];
} HashTab;



/* ----- PUBLIC FUNCTIONS ----- */

HashTab * HashTabNew(
		uint32_t slotMagnitude,
		size_t keySize,
		uint32_t (* hashKey)(void * key),
		bool (* keysEqual)(void * keyA, void * keyB)
);

void HashTabFree(HashTab * table);

size_t HashTabSize(HashTab * table);

bool HashTabExists(
		HashTab * table,
		void * key
);

void * HashTabGet(
		HashTab * table,
		void * key,
		bool * exists
);

void HashTabSet(
		HashTab * table,
		void * key,
		void * value
);

void HashTabInsert(
		HashTab * table,
		void * key,
		void * value
);

void * HashTabRemove(
		HashTab * table,
		void * key
);

void HashTabEach(
		HashTab * table,
		void (* callback)(void * key, void * value, void * context),
		void * context
);



#endif /* HASHTAB_H */
