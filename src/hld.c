#include <assert.h>
#include "hld.h"



/* ----- PUBLIC FUNCTIONS ----- */

void * HLDHashTableLookup(HLDHashTable * table, int32_t key) {
	assert(table != NULL);
	void * result = NULL;

	if (key >= 0) {
		uint32_t hash = key & table->keyMask;
		HLDHashItem * item = table->slots[hash].first;
		while (item) {
			if (item->key == key) {
				result = item->value;
				item = NULL;
			} else {
				item = item->next;
			}
		}
	}

	return result;
}
