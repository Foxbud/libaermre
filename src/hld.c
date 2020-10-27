#include <assert.h>
#include <stdlib.h>

#include "internal/hld.h"



/* ----- INTERNAL GLOBALS ----- */

HLDVariables hldvars = {0};

HLDFunctions hldfuncs = {0};



/* ----- INTERNAL FUNCTIONS ----- */

HLDSprite * HLDSpriteLookup(int32_t spriteIdx) {
	HLDSprite * result = NULL;

	if (spriteIdx >= 0 && (uint32_t)spriteIdx < hldvars.spriteTable->size) {
		result = ((HLDSprite **)hldvars.spriteTable->elements)[spriteIdx];
	}

	return result;
}

HLDRoom * HLDRoomLookup(int32_t roomIdx) {
	HLDRoom * result = NULL;

	if (roomIdx >= 0 && (uint32_t)roomIdx < hldvars.roomTable->size) {
		result = ((HLDRoom **)hldvars.roomTable->elements)[roomIdx];
	}

	return result;
}

void * HLDOHashTableLookup(HLDOHashTable * table, int32_t key) {
	assert(table != NULL);
	void * result = NULL;

	if (key >= 0) {
		uint32_t idx = key & table->keyMask;
		HLDOHashItem * item = table->slots[idx].first;
		while (item) {
			if (item->key == key) {
				result = item->value;
				break;
			} else {
				item = item->next;
			}
		}
	}

	return result;
}

void * HLDCHashTableLookup(HLDCHashTable * table, int32_t key) {
	assert(table != NULL);
	void * result = NULL;
	uint32_t keyMask = table->keyMask;

	if (key >= 0) {
		uint32_t origIdx = key & keyMask;
		uint32_t idx = origIdx;
		HLDCHashSlot * slot = table->slots + idx;
		while (slot->keyNext) {
			if (slot->key == key) {
				result = slot->value;
				break;
			} else {
				idx = (idx + 1) & keyMask;
				if (idx == origIdx) break;
				slot = table->slots + idx;
			}
		}
	}

	return result;
}

HLDEvent * HLDEventNew(HLDNamedFunction * handler) {
	assert(handler);

	/* TODO Refactor this address to patch file. */
	uint32_t unknownAddr = 0x09518c19;
	HLDEvent * event = malloc(sizeof(HLDEvent));
	assert(event);

	/* TODO Refactor this address to patch file. */
	event->classDef = (void *)0x0948781c;
	event->eventNext = NULL;
	event->field_8 = 1;
	event->field_C = 1;
	event->field_10 = unknownAddr;
	event->field_14 = 0;
	event->field_18 = 0;
	event->field_1C = 0;
	event->field_20 = 0;
	event->field_24 = 0;
	event->field_28 = 0;
	event->field_2C = 0;
	event->field_30 = 0;
	event->field_34 = 0;
	event->field_38 = 0;
	event->field_3C = 0;
	event->field_40 = 0;
	event->field_44 = 0;
	event->field_48 = 0;
	event->field_4C = 0;
	event->field_50 = 0;
	event->field_54 = 0;
	event->field_58 = unknownAddr;
	event->name = handler->name;
	event->handlerIndex = 0;
	event->handler = handler;
	event->field_68 = 0;
	event->field_6C = 0;
	event->field_70 = 0;
	event->field_74 = 0;
	event->field_78 = 0;
	event->field_7C = 0x11;

	return event;
}

HLDEventWrapper * HLDEventWrapperNew(HLDEvent * event) {
	assert(event);

	/* TODO Refactor this address to patch file. */
	uint32_t unknownAddr = 0x09518c19;
	HLDEventWrapper * wrapper = malloc(sizeof(HLDEventWrapper));
	assert(wrapper);

	/* TODO Refactor this address to patch file. */
	wrapper->classDef = (void *)0x094a47d8;
	wrapper->event = event;
	wrapper->field_08 = unknownAddr;
	wrapper->field_0C = 0x81;

	return wrapper;
}
