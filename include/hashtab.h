#ifndef HASHTAB_H
#define HASHTAB_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



/* ----- PUBLIC TYPES ----- */

typedef struct HashTab {
	const uint8_t rawData[4 * 3];
} HashTab;



/* ----- PUBLIC FUNCTIONS ----- */

HashTab * HashTabNew(uint32_t slotMagnitude);

void HashTabFree(HashTab * table);

size_t HashTabSize(HashTab * table);

bool HashTabExists(
		HashTab * table,
		uint32_t key
);

void * HashTabGet(
		HashTab * table,
		uint32_t key,
		bool * exists
);

void HashTabSet(
		HashTab * table,
		uint32_t key,
		void * value
);

void * HashTabRemove(
		HashTab * table,
		uint32_t key
);

void HashTabEach(
		HashTab * table,
		void (* callback)(uint32_t key, void * value)
);



#endif /* HASHTAB_H */
