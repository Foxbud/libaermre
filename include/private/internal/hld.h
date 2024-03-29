/**
 * @copyright 2021 the libaermre authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef INTERNAL_HLD_H
#define INTERNAL_HLD_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/* ----- INTERNAL MACROS ----- */

#define HLDPrimitiveMakeUndefined(name) \
    HLDPrimitive name = {.type = HLD_PRIMITIVE_UNDEFINED}

#define HLDPrimitiveMakeReal(name, initVal) \
    HLDPrimitive name = {.value.r = (initVal), .type = HLD_PRIMITIVE_REAL}

#define HLDPrimitiveMakeStringS(name, str, len)        \
    HLDPrimitiveString name##InnerValue = {            \
        .chars = (str), .refs = 1, .length = (len)};   \
    HLDPrimitive name = {.value.p = &name##InnerValue, \
                         .type = HLD_PRIMITIVE_STRING}

#define HLDPrimitiveMakeStringH(name, str, len)                                \
    HLDPrimitiveString* name##InnerValue = malloc(sizeof(HLDPrimitiveString)); \
    assert(name##InnerValue);                                                  \
    name##InnerValue->chars = (str);                                           \
    name##InnerValue->refs = 1;                                                \
    name##InnerValue->length = (len);                                          \
    HLDPrimitive name = {.value.p = name##InnerValue,                          \
                         .type = HLD_PRIMITIVE_STRING}

#define HLDPrimitiveMakeArrayS(name, len)                                  \
    HLDPrimitive name##InnerElements[(len)];                               \
    HLDArrayPreSize name##InnerArray = {.size = (len),                     \
                                        .elements = &name##InnerElements}; \
    HLDPrimitiveArray name##InnerValue = {                                 \
        .refs = 1, .subArrays = &name##InnerArray, .numSubArrays = 1};     \
    HLDPrimitive name = {.value.p = &name##InnerValue,                     \
                         .type = HLD_PRIMITIVE_ARRAY}

#define HLDAPICallAdv(api, target, other, ...)                   \
    ({                                                           \
        HLDPrimitive HLDAPICallAdv_argv[] = {__VA_ARGS__};       \
        HLDPrimitiveMakeUndefined(HLDAPICallAdv_result);         \
        (api)(&HLDAPICallAdv_result, (target), (other),          \
              sizeof(HLDAPICallAdv_argv) / sizeof(HLDPrimitive), \
              HLDAPICallAdv_argv);                               \
        HLDAPICallAdv_result;                                    \
    })

#define HLDAPICall(api, ...) HLDAPICallAdv((api), NULL, NULL, ##__VA_ARGS__)

#define HLDScriptCallAdv(script, target, other, ...)                    \
    ({                                                                  \
        HLDPrimitive* HLDScriptCallAdv_argv[] = {__VA_ARGS__};          \
        HLDPrimitiveMakeUndefined(HLDScriptCallAdv_result);             \
        (script)((target), (other), &HLDScriptCallAdv_result,           \
                 sizeof(HLDScriptCallAdv_argv) / sizeof(HLDPrimitive*), \
                 HLDScriptCallAdv_argv);                                \
        HLDScriptCallAdv_result;                                        \
    })

#define HLDScriptCall(script, ...) \
    HLDScriptCallAdv((script), NULL, NULL, ##__VA_ARGS__)

#define HLDObjectLookup(objIdx) \
    ((HLDObject*)HLDOpenHashTableLookup(*hldvars.objectTableHandle, (objIdx)))

#define HLDInstanceLookup(instId) \
    ((HLDInstance*)HLDOpenHashTableLookup(hldvars.instanceTable, (instId)))

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
    HLD_EVENT_STEP_NORMAL,
    HLD_EVENT_STEP_PRE,
    HLD_EVENT_STEP_POST
} HLDEventStepType;

typedef enum HLDEventOtherType {
    HLD_EVENT_OTHER_OUTSIDE,
    HLD_EVENT_OTHER_BOUNDARY,
    HLD_EVENT_OTHER_GAME_START,
    HLD_EVENT_OTHER_GAME_END,
    HLD_EVENT_OTHER_ROOM_START,
    HLD_EVENT_OTHER_ROOM_END,
    HLD_EVENT_OTHER_NO_MORE_LIVES,
    HLD_EVENT_OTHER_ANIMATION_END,
    HLD_EVENT_OTHER_END_OF_PATH,
    HLD_EVENT_OTHER_NO_MORE_HEALTH
} HLDEventOtherType;

typedef enum HLDEventDrawType {
    HLD_EVENT_DRAW_NORMAL = 0,
    HLD_EVENT_DRAW_GUI_NORMAL = 64
} HLDEventDrawType;

typedef struct HLDOpenHashItem {
    struct HLDOpenHashItem* prev;
    struct HLDOpenHashItem* next;
    int32_t key;
    void* value;
} HLDOpenHashItem;

typedef struct HLDOpenHashSlot {
    struct HLDOpenHashItem* first;
    struct HLDOpenHashItem* last;
} HLDOpenHashSlot;

typedef struct HLDOpenHashTable {
    struct HLDOpenHashSlot* slots;
    uint32_t keyMask;
    size_t numItems;
} HLDOpenHashTable;

typedef struct HLDClosedHashSlot {
    int32_t nameIdx;
    void* value;
    int32_t key;
} HLDClosedHashSlot;

typedef struct HLDClosedHashTable {
    size_t numSlots;
    size_t numItems;
    uint32_t keyMask;
    uint32_t field_C;
    struct HLDClosedHashSlot* slots;
} HLDClosedHashTable;

typedef struct HLDLookupTable {
    size_t size;
    uint32_t field_4;
    uint32_t field_8;
    void* elements;
} HLDLookupTable;

typedef struct HLDVecReal {
    float x;
    float y;
} HLDVecReal;

typedef struct HLDVecIntegral {
    int32_t x;
    int32_t y;
} HLDVecIntegral;

typedef struct HLDArrayPreSize {
    size_t size;
    void* elements;
} HLDArrayPreSize;

typedef struct HLDArrayPostSize {
    void* elements;
    size_t size;
} HLDArrayPostSize;

typedef enum HLDPrimitiveType {
    HLD_PRIMITIVE_REAL = 0x0,
    HLD_PRIMITIVE_STRING = 0x1,
    HLD_PRIMITIVE_ARRAY = 0x2,
    HLD_PRIMITIVE_PTR = 0x3,
    HLD_PRIMITIVE_VEC3 = 0x4,
    HLD_PRIMITIVE_UNDEFINED = 0x5,
    HLD_PRIMITIVE_OBJECT = 0x6,
    HLD_PRIMITIVE_INT32 = 0x7,
    HLD_PRIMITIVE_VEC4 = 0x8,
    HLD_PRIMITIVE_MATRIX = 0x9,
    HLD_PRIMITIVE_INT64 = 0xA,
    HLD_PRIMITIVE_ACCESSOR = 0xB,
    HLD_PRIMITIVE_NULL = 0xC,
    HLD_PRIMITIVE_BOOL = 0xD,
    HLD_PRIMITIVE_ITERATOR = 0xE,
} HLDPrimitiveType;

typedef union __attribute__((aligned(4))) HLDPrimitiveValue {
    uint32_t raw[3];
    double r;
    void* p;
    int32_t i32;
    int64_t i64;
    bool b;
} HLDPrimitiveValue;

typedef struct HLDPrimitive {
    HLDPrimitiveValue value;
    HLDPrimitiveType type;
} HLDPrimitive;

typedef struct HLDPrimitiveString {
    const char* chars;
    size_t refs;
    size_t length;
} HLDPrimitiveString;

typedef struct __attribute__((aligned(4))) HLDPrimitiveArray {
    size_t refs;
    struct HLDArrayPreSize* subArrays;
    void* field_8;
    uint32_t field_C;
    size_t numSubArrays;
} HLDPrimitiveArray;

typedef struct HLDEventSubscribers {
    int32_t* objects;
    uint32_t field_4;
} HLDEventSubscribers;

typedef struct HLDNamedFunction {
    const char* name;
    void* function;
} HLDNamedFunction;

typedef struct HLDNodeDLL {
    struct HLDNodeDLL* next;
    struct HLDNodeDLL* prev;
    void* item;
} HLDNodeDLL;

typedef struct HLDBoundingBox {
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
} HLDBoundingBox;

typedef struct HLDEvent {
    void* classDef;
    struct HLDEvent* eventNext;
    uint32_t field_8;
    uint32_t field_C;
    void* field_10;
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
    void* field_58;
    const char* name;
    uint32_t handlerIndex;
    struct HLDNamedFunction* handler;
    uint32_t field_68;
    uint32_t field_6C;
    uint32_t field_70;
    uint32_t field_74;
    uint32_t field_78;
    uint32_t field_7C;
} HLDEvent;

typedef struct HLDEventWrapper {
    void* classDef;
    struct HLDEvent* event;
    void* field_08;
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
    const char* name;
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
    struct HLDObject* parent;
    struct HLDArrayPreSize eventListeners[15];
    struct HLDNodeDLL* instanceFirst;
    struct HLDNodeDLL* instanceLast;
    uint32_t numInstances;
    uint32_t field_D0;
    uint32_t field_D4;
    uint32_t field_D8;
} HLDObject;

typedef struct HLDInstance {
    void* classDef;
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
    HLDClosedHashTable* locals;
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
    struct HLDObject* object;
    uint32_t field_58;
    uint32_t field_5C;
    int32_t spriteIndex;
    float imageIndex;
    float imageSpeed;
    struct HLDVecReal imageScale;
    float imageAngle;
    float imageAlpha;
    uint32_t imageBlend;
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
    struct HLDInstance* instanceNext;
    struct HLDInstance* instancePrev;
    float depth;
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

typedef struct HLDView {
    bool visible;
    uint8_t field_1;
    uint8_t field_2;
    uint8_t field_3;
    HLDVecReal posRoom;
    HLDVecReal sizeRoom;
    HLDVecIntegral posPort;
    HLDVecIntegral sizePort;
    float angle;
    HLDVecIntegral border;
    HLDVecIntegral speed;
    int32_t objectIndex;
    int32_t surfaceId;
    int32_t camera;
} HLDView;

typedef struct HLDRoom {
    uint32_t field_0;
    struct HLDRoom* self;
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
    HLDView* views[8];
    uint32_t field_68;
    uint32_t field_6C;
    uint32_t field_70;
    uint32_t field_74;
    uint32_t field_78;
    uint32_t field_7C;
    struct HLDInstance* instanceFirst;
    struct HLDInstance* instanceLast;
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
    const char* name;
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
    void* classDef;
    uint32_t field_4;
    uint32_t field_8;
    uint32_t field_C;
    uint32_t field_10;
    uint32_t field_14;
    uint32_t numImages;
    struct HLDVecIntegral size;
    struct HLDVecIntegral origin;
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
    const char* name;
    uint32_t index;
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

typedef struct HLDFont {
    void* classDef;
    const char* fontname;
    size_t size;
    bool bold;
    bool italic;
    uint8_t field_E;
    uint8_t field_F;
    uint32_t field_10;
    uint32_t field_14;
    int32_t first;
    int32_t last;
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
    uint32_t field_80;
    uint32_t field_84;
    uint32_t field_88;
    uint32_t field_8C;
} HLDFont;

/* Builtin GML function signature. */
typedef void (*HLDAPICallback)(HLDPrimitive* result,
                               HLDInstance* target,
                               HLDInstance* other,
                               size_t argc,
                               HLDPrimitive* argv);

/* Custom Heart Machine function signature. */
typedef HLDPrimitive* (*HLDScriptCallback)(HLDInstance* target,
                                           HLDInstance* other,
                                           HLDPrimitive* result,
                                           size_t argc,
                                           HLDPrimitive** argv);

/*
 * This struct holds pointers to global variables in the Game Maker
 * engine. These pointers are passed into the MRE from the hooks injected
 * into the game's executable.
 */
typedef struct __attribute__((packed)) HLDVariables {
    /* Allocated GML hash tables. */
    HLDArrayPostSize* maps;
    /* Number of steps since start of the game. */
    int32_t* numSteps;
    /* Runtime of previous step in microseconds. */
    int64_t* deltaTime;
    /* Tables of booleans where each index represents a key code. */
    bool (*keysPressedTable)[0x100];
    bool (*keysHeldTable)[0x100];
    bool (*keysReleasedTable)[0x100];
    /* Tables of booleans where each index represents a mouse button. */
    bool (*mouseButtonsPressedTable)[0x3];
    bool (*mouseButtonsHeldTable)[0x3];
    bool (*mouseButtonsReleasedTable)[0x3];
    /* Mouse cursor position in pixels. */
    uint32_t* mousePosX;
    uint32_t* mousePosY;
    /* Array of all registered rooms. */
    HLDArrayPreSize* roomTable;
    /* Index of currently active room. */
    int32_t* roomIndexCurrent;
    /* Actual room object of currently active room. */
    HLDRoom** roomCurrent;
    /* Array of all registered sprites. */
    HLDArrayPreSize* spriteTable;
    /* Array of all registered fonts. */
    HLDArrayPreSize* fontTable;
    /* Index of currently active font. */
    int32_t* fontIndexCurrent;
    /* Actual font object of currently active font. */
    HLDFont** fontCurrent;
    /* Hash table of all registered objects. */
    HLDOpenHashTable** objectTableHandle;
    /* Hash table of all in-game instances. */
    HLDOpenHashTable* instanceTable;
    /* Lookup table of all instance local variable names. */
    HLDLookupTable* instanceLocalTable;
    /*
     * As an optimization, the engine only checks for alarm events on objects
     * listed (or "subscribed") in these arrays.
     */
    size_t (*alarmEventSubscriberCounts)[12];
    HLDEventSubscribers (*alarmEventSubscribers)[12];
    /* Same as above, but for step events. */
    size_t (*stepEventSubscriberCounts)[3];
    HLDEventSubscribers (*stepEventSubscribers)[3];
    /* Addresses necessary for creating new events. */
    void* eventClass;
    void* eventWrapperClass;
    /*
     * Not certain what this address even references, but the custom events
     * won't work unless some of their fields point to this address.
     */
    void* unknownEventAddress;
} HLDVariables;

/*
 * This struct holds pointers to functions in the Game Maker
 * engine. These pointers are passed into the MRE from the hooks injected
 * into the game's executable.
 */
typedef struct __attribute__((packed)) HLDFunctions {
    /* Mouse x position relative to current room. */
    int32_t (*actionMouseGetX)(int32_t unknown0);
    /* Mouse y position relative to current room. */
    int32_t (*actionMouseGetY)(int32_t unknown0);
    /* Go to room. */
    void (*actionRoomGoto)(int32_t roomIdx, int32_t unknown0);
    /* Register a new sprite. */
    int32_t (*actionSpriteAdd)(const char* fname,
                               size_t imgNum,
                               int32_t unknown0,
                               int32_t unknown1,
                               int32_t unknown2,
                               int32_t unknown3,
                               uint32_t origX,
                               uint32_t origY);
    /* Overwrite an existing sprite with a new one. */
    void (*actionSpriteReplace)(int32_t spriteIdx,
                                const char* fname,
                                size_t imgNum,
                                int32_t unknown0,
                                int32_t unknown1,
                                int32_t unknown2,
                                int32_t unknown3,
                                uint32_t origX,
                                uint32_t origY);
    /* Register a new font. */
    int32_t (*actionFontAdd)(const char* fname,
                             size_t size,
                             bool bold,
                             bool italic,
                             int32_t first,
                             int32_t last);
    /* Register a new object. */
    int32_t (*actionObjectAdd)(void);
    /* Trigger an event as if it occurred "naturally." */
    int32_t (*actionEventPerform)(HLDInstance* target,
                                  HLDInstance* other,
                                  int32_t targetObjIdx,
                                  uint32_t eventType,
                                  int32_t eventNum);
    /* Get the current global draw alpha. */
    float (*actionDrawGetAlpha)(void);
    /* Set the current global draw alpha. */
    void (*actionDrawSetAlpha)(float alpha);
    /* Draw a sprite to the screen. */
    void (*actionDrawSpriteGeneral)(HLDSprite* sprite,
                                    uint32_t imgNum,
                                    float left,
                                    float top,
                                    float width,
                                    float height,
                                    float x,
                                    float y,
                                    float scaleX,
                                    float scaleY,
                                    float angle,
                                    uint32_t blendNW,
                                    uint32_t blendNE,
                                    uint32_t blendSE,
                                    uint32_t blendSW,
                                    float alpha);
    /* Draw a line to the screen. */
    void (*actionDrawLine)(float x1,
                           float y1,
                           float x2,
                           float y2,
                           float width,
                           uint32_t color1,
                           uint32_t color2);
    /* Draw an ellipse to the screen. */
    void (*actionDrawEllipse)(float left,
                              float top,
                              float right,
                              float bottom,
                              uint32_t colorCenter,
                              uint32_t colorEdge,
                              bool outline);
    /* Draw a triangle to the screen. */
    void (*actionDrawTriangle)(float x1,
                               float y1,
                               float x2,
                               float y2,
                               float x3,
                               float y3,
                               uint32_t color1,
                               uint32_t color2,
                               uint32_t color3,
                               bool outline);
    /* Draw a rectangle to the screen. */
    void (*actionDrawRectangle)(float left,
                                float top,
                                float right,
                                float bottom,
                                uint32_t colorNW,
                                uint32_t colorNE,
                                uint32_t colorSE,
                                uint32_t colorSW,
                                bool outline);
    /* Draw a string to the screen. */
    void (*actionDrawText)(float x,
                           float y,
                           const char* text,
                           int32_t height,
                           uint32_t width,
                           float scaleX,
                           float scaleY,
                           float angle,
                           uint32_t colorNW,
                           uint32_t colorNE,
                           uint32_t colorSE,
                           uint32_t colorSW,
                           float alpha);
    /* Draw an instance's sprite. */
    void (*actionDrawSelf)(HLDInstance* inst);
    /* Set the currently active draw font. */
    void (*actionDrawSetFont)(int32_t fontIdx);
    /* Spawn a new instance of an object. */
    HLDInstance* (*actionInstanceCreate)(int32_t objIdx,
                                         float posX,
                                         float posY);
    /* Change the object type of an instance. */
    void (*actionInstanceChange)(HLDInstance* inst,
                                 int32_t newObjIdx,
                                 bool doEvents);
    /* Destroy an instance. */
    void (*actionInstanceDestroy)(HLDInstance* inst0,
                                  HLDInstance* inst1,
                                  int32_t objIdx,
                                  bool doEvent);
    /* Set instance's position (and update bounding box accordingly). */
    void (*Instance_setPosition)(HLDInstance* inst, float x, float y);
    /* Set instance's mask index. */
    void (*Instance_setMaskIndex)(HLDInstance* inst, int32_t maskIndex);
    /* Set an instance's direction and speed based on its motion vector. */
    void (*Instance_setMotionPolarFromCartesian)(HLDInstance* inst);
    /* Create a new GML map. Parameters: NULL. */
    HLDAPICallback API_dsMapCreate;
    /* Retrieve value from a GML map. Parameters: id, key. */
    HLDAPICallback API_dsMapFindValue;
    /* Set a GML map key to value. Parameters: id, key, val. */
    HLDAPICallback API_dsMapSet;
    /*
     * Add a GML map to another GML map for JSON representation.
     * Parameters: id, key, val.
     */
    HLDAPICallback API_dsMapAddMap;
    /*
     * Custom Heat Machine fuction that changes to a specific room and spawns
     * the player.
     */
    HLDScriptCallback Script_GoToRoom;
    /*
     * Custom Heart Machine function that sets an instance's draw depth based
     * on its y position and the current room's height.
     */
    HLDScriptCallback Script_Setdepth;
} HLDFunctions;

/* ----- INTERNAL GLOBALS ----- */

extern HLDVariables hldvars;

extern HLDFunctions hldfuncs;

/* ----- INTERNAL FUNCTIONS ----- */

HLDView* HLDViewLookup(uint32_t viewIdx);

HLDSprite* HLDSpriteLookup(int32_t spriteIdx);

HLDFont* HLDFontLookup(int32_t fontIdx);

HLDRoom* HLDRoomLookup(int32_t roomIdx);

void* HLDOpenHashTableLookup(HLDOpenHashTable* table, int32_t key);

void* HLDClosedHashTableLookup(HLDClosedHashTable* table, int32_t key);

HLDEvent* HLDEventNew(HLDNamedFunction* handler);

HLDEventWrapper* HLDEventWrapperNew(HLDEvent* event);

void HLDRecordEngineRefs(HLDVariables* vars, HLDFunctions* funcs);

#endif /* INTERNAL_HLD_H */
