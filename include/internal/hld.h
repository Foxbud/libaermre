#ifndef INTERNAL_HLD_H
#define INTERNAL_HLD_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



/* ----- INTERNAL TYPES ----- */

typedef enum HLDEventType {
	HLD_EVENT_CREATE,
	HLD_EVENT_DESTROY,
	HLD_EVENT_ALARM,
	HLD_EVENT_STEP,
	HLD_EVENT_COLLISION,
	HLD_EVENT_UNKNOWN_0,
	HLD_EVENT_UNKNOWN_1,
	HLD_EVENT_OTHER,
	HLD_EVENT_DRAW,
	HLD_EVENT_UNKNOWN_2,
	HLD_EVENT_UNKNOWN_3,
	HLD_EVENT_UNKNOWN_4,
	HLD_EVENT_UNKNOWN_5,
	HLD_EVENT_UNKNOWN_6,
	HLD_EVENT_UNKNOWN_7
} HLDEventType;

typedef enum HLDEventStepType {
	HLD_EVENT_STEP_INLINE,
	HLD_EVENT_STEP_PRE,
	HLD_EVENT_STEP_POST
} HLDEventStepType;

typedef enum HLDEventOtherType {
	HLD_EVENT_OTHER_ANIMATION_END = 7
} HLDEventOtherType;

typedef struct HLDHashItem {
	struct HLDHashItem * prev;
	struct HLDHashItem * next;
	int32_t key;
	void * value;
} HLDHashItem;

typedef struct HLDHashSlot {
	struct HLDHashItem * first;
	struct HLDHashItem * last;
} HLDHashSlot;

typedef struct HLDHashTable {
	struct HLDHashSlot * slots;
	size_t keyMask;
	size_t numItems;
} HLDHashTable;

typedef struct HLDVecReal {
	float x;
	float y;
} HLDVecReal;

typedef struct HLDArrayPreSize {
	size_t size;
	void * elements;
} HLDArrayPreSize;

typedef struct HLDEventSubscribers {
	int32_t * objects;
	uint32_t field_4;
} HLDEventSubscribers;

typedef struct HLDNamedFunction {
	const char * name;
	void * function;
} HLDNamedFunction;

typedef struct HLDNodeDLL {
	struct HLDNodeDLL * next;
	struct HLDNodeDLL * prev;
	void * item;
} HLDNodeDLL;

typedef struct HLDBoundingBox {
	int32_t left;
	int32_t top;
	int32_t right;
	int32_t bottom;
} HLDBoundingBox;

typedef struct HLDEvent {
	void * classDef;
	struct HLDEvent * eventNext;
	uint32_t field_8;
	uint32_t field_C;
	uint32_t field_10;
	uint32_t field_14;
	uint32_t field_18;
	uint32_t field_1C;
	uint32_t field_20;
	uint32_t field_24;
	uint32_t field_28;
	uint32_t field_2C;
	uint32_t field_30;
	uint32_t field_34;
	uint32_t field_38;
	uint32_t field_3C;
	uint32_t field_40;
	uint32_t field_44;
	uint32_t field_48;
	uint32_t field_4C;
	uint32_t field_50;
	uint32_t field_54;
	uint32_t field_58;
	const char * name;
	uint32_t handlerIndex;
	struct HLDNamedFunction * handler;
	uint32_t field_68;
	uint32_t field_6C;
	uint32_t field_70;
	uint32_t field_74;
	uint32_t field_78;
	uint32_t field_7C;
} HLDEvent;

typedef struct HLDEventWrapper {
	void * classDef;
	struct HLDEvent * event;
	uint32_t field_08;
	uint32_t field_0C;
} HLDEventWrapper;

typedef struct HLDObject {
	struct {
		uint8_t solid : 1;
		uint8_t visible : 1;
		uint8_t persistent : 1;
		uint8_t : 1;
		uint8_t collisions : 1;
		uint8_t : 3;
	} flags;
	uint8_t field_1;
	uint8_t field_2;
	uint8_t field_3;
	int32_t spriteIndex;
	uint32_t depth;
	int32_t parentIndex;
	int32_t maskIndex;
	const char * name;
	int32_t index;
	uint32_t physics;
	uint32_t field_20;
	uint32_t field_24;
	uint32_t field_28;
	uint32_t field_2C;
	uint32_t field_30;
	uint32_t field_34;
	uint32_t field_38;
	uint32_t field_3C;
	uint32_t field_40;
	uint32_t field_44;
	struct HLDObject * parent;
	struct HLDArrayPreSize eventListeners[15];
	struct HLDNodeDLL * instanceFirst;
	struct HLDNodeDLL * instanceLast;
	uint32_t numInstances;
	uint32_t field_D0;
	uint32_t field_D4;
	uint32_t field_D8;
} HLDObject;

typedef struct HLDInstance {
	void * classDef;
	uint32_t field_4;
	uint32_t field_8;
	uint32_t field_C;
	uint32_t field_10;
	uint32_t field_14;
	uint8_t field_18;
	uint8_t field_19;
	uint8_t field_1A;
	uint8_t field_1B;
	uint32_t field_1C;
	uint32_t field_20;
	uint32_t field_24;
	uint32_t tangible;
	uint32_t field_2C;
	uint32_t field_30;
	uint32_t field_34;
	uint8_t field_38;
	bool visible;
	bool solid;
	bool persistent;
	bool marked;
	bool deactivated;
	uint8_t field_3E;
	uint8_t field_3F;
	uint32_t field_40;
	uint32_t field_44;
	uint32_t field_48;
	uint32_t id;
	int32_t objectIndex;
	struct HLDObject * object;
	uint32_t field_58;
	uint32_t field_5C;
	int32_t spriteIndex;
	float imageIndex;
	float imageSpeed;
	struct HLDVecReal imageScale;
	float imageAngle;
	float imageAlpha;
	int32_t imageBlend;
	int32_t maskIndex;
	uint32_t field_84;
	struct HLDVecReal pos;
	struct HLDVecReal posStart;
	struct HLDVecReal posPrev;
	float direction;
	float speed;
	float friction;
	float gravityDir;
	float gravity;
	float speedX;
	float speedY;
	struct HLDBoundingBox bbox;
	int32_t alarms[12];
	int32_t pathIndex;
	float pathPos;
	float pathPosPrev;
	uint32_t field_108;
	uint32_t field_10C;
	uint32_t field_110;
	uint32_t field_114;
	uint32_t field_118;
	uint32_t field_11C;
	uint32_t field_120;
	uint32_t field_124;
	uint32_t field_128;
	uint32_t field_12C;
	uint8_t field_130;
	uint8_t field_131;
	uint8_t field_132;
	uint8_t field_133;
	uint32_t field_134;
	uint32_t field_138;
	uint32_t field_13C;
	uint32_t field_140;
	uint32_t field_144;
	bool field_148;
	uint8_t field_149;
	uint8_t field_14A;
	uint8_t field_14B;
	struct HLDInstance * instanceNext;
	struct HLDInstance * instancePrev;
	uint32_t field_154;
	uint32_t field_158;
	uint32_t lastUpdate;
	uint32_t field_160;
	uint32_t field_164;
	uint32_t field_168;
	uint32_t field_16C;
	uint32_t field_170;
	uint32_t field_174;
	uint32_t field_178;
	uint32_t field_17C;
	uint32_t field_180;
} HLDInstance;

typedef struct HLDRoom {
	uint32_t field_0;
	struct HLDRoom * self;
	uint32_t field_8;
	uint32_t field_C;
	uint32_t field_10;
	uint32_t field_14;
	uint32_t field_18;
	uint32_t field_1C;
	uint32_t field_20;
	uint32_t field_24;
	uint32_t field_28;
	uint32_t field_2C;
	uint32_t field_30;
	uint32_t field_34;
	uint32_t field_38;
	uint32_t field_3C;
	uint32_t field_40;
	uint32_t field_44;
	uint32_t field_48;
	uint32_t field_4C;
	uint32_t field_50;
	uint32_t field_54;
	uint32_t field_58;
	uint32_t field_5C;
	uint32_t field_60;
	uint32_t field_64;
	uint32_t field_68;
	uint32_t field_6C;
	uint32_t field_70;
	uint32_t field_74;
	uint32_t field_78;
	uint32_t field_7C;
	struct HLDInstance * instanceFirst;
	struct HLDInstance * instanceLast;
	int32_t numInstances;
	uint32_t field_8C;
	uint32_t field_90;
	uint32_t field_94;
	uint32_t field_98;
	uint32_t field_9C;
	uint32_t field_A0;
	uint32_t field_A4;
	uint32_t field_A8;
	uint32_t field_AC;
	uint32_t field_B0;
	uint32_t field_B4;
	uint32_t field_B8;
	uint32_t field_BC;
	uint32_t field_C0;
	uint32_t field_C4;
	uint32_t field_C8;
	const char * name;
	uint32_t field_D0;
	uint32_t field_D4;
	uint32_t field_D8;
	uint32_t field_DC;
	uint32_t field_E0;
	uint32_t field_E4;
	uint32_t field_E8;
	uint32_t field_EC;
	uint32_t field_F0;
	uint32_t field_F4;
	uint32_t field_F8;
	uint32_t field_FC;
	uint32_t field_100;
	uint32_t field_104;
	uint32_t field_108;
	uint32_t field_10C;
	uint32_t field_110;
	uint32_t field_114;
	uint32_t field_118;
	uint32_t field_11C;
	uint32_t field_120;
} HLDRoom;

typedef struct HLDSprite {
	uint32_t field_0;
	uint32_t field_4;
	uint32_t field_8;
	uint32_t field_C;
	uint32_t field_10;
	uint32_t field_14;
	uint32_t numImages;
	uint32_t field_1C;
	uint32_t field_20;
	uint32_t field_24;
	uint32_t field_28;
	uint32_t field_2C;
	uint32_t field_30;
	uint32_t field_34;
	uint32_t field_38;
	uint32_t field_3C;
	uint32_t field_40;
	uint32_t field_44;
	uint32_t field_48;
	uint32_t field_4C;
	uint32_t field_50;
	uint32_t field_54;
	uint32_t field_58;
	const char * name;
	uint32_t field_60;
	uint32_t field_64;
	uint32_t field_68;
	float speed;
	uint32_t field_70;
	uint32_t field_74;
	uint32_t field_78;
	uint32_t field_7C;
	uint32_t field_80;
	uint32_t field_84;
} HLDSprite;



/* ----- INTERNAL FUNCTIONS ----- */

void * HLDHashTableLookup(HLDHashTable * table, int32_t key);

HLDEvent * HLDEventNew(HLDNamedFunction * handler);

HLDEventWrapper * HLDEventWrapperNew(HLDEvent * event);



#endif /* INTERNAL_HLD_H */
