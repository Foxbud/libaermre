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
#include <assert.h>

#include "foxutils/arraymacs.h"
#include "foxutils/mapmacs.h"
#include "foxutils/math.h"
#include "foxutils/stringmapmacs.h"

#include "aer/instance.h"
#include "aer/object.h"
#include "aer/sprite.h"
#include "internal/core.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/hld.h"
#include "internal/instance.h"
#include "internal/log.h"
#include "internal/object.h"

/* ----- PRIVATE MACROS ----- */

#define ModLocalValDeinit(val)                                                 \
    do {                                                                       \
        ModLocalVal *ModLocalValDeinit_val = (val);                            \
        void (*destructor)(AERLocal *);                                        \
        if ((destructor = ModLocalValDeinit_val->destructor))                  \
            destructor(&ModLocalValDeinit_val->local);                         \
    } while (0);

/* ----- PRIVATE TYPES ----- */

typedef struct __attribute__((packed)) ModLocalKey {
    int32_t modIdx;
    int32_t instId;
    char name[24];
} ModLocalKey;

typedef struct ModLocalVal {
    AERLocal local;
    void (*destructor)(AERLocal *);
} ModLocalVal;

/* ----- PRIVATE GLOBALS ----- */

static FoxMap hldLocals = {0};

static FoxMap modLocals = {0};

/* ----- PRIVATE FUNCTIONS ----- */

static bool ModLocalKeyInit(ModLocalKey *key, int32_t instId, const char *name,
                            bool public) {
    assert(key);
    assert(name);

    key->modIdx = (public) ? MOD_NULL : ModManPeekContext();
    key->instId = instId;

    char *keyName = key->name;
    bool endFound = false;
    char curChar;
    for (uint32_t idx = 0; idx < sizeof(key->name); idx++) {
        if (!endFound && (curChar = name[idx]) == '\0')
            endFound = true;
        keyName[idx] = (endFound) ? '\0' : curChar;
    }

    return endFound;
}

static bool ModLocalValDeinitCallback(ModLocalVal *val, void *ctx) {
    (void)ctx;

    ModLocalValDeinit(val);

    return true;
}

static bool ModLocalKeyGetOrphansCallback(const ModLocalKey *key,
                                          FoxArray *orphans) {
    if (!HLDInstanceLookup(key->instId))
        *FoxArrayMPush(const ModLocalKey *, orphans) = key;

    return true;
}

/* ----- INTERNAL FUNCTIONS ----- */

void InstanceManPruneModLocals(void) {
    LogInfo("Pruning mod instance locals...");

    FoxArray orphans;
    FoxArrayMInit(const ModLocalKey *, &orphans);

    FoxMapMForEachKey(ModLocalKey, ModLocalVal, &modLocals,
                      ModLocalKeyGetOrphansCallback, &orphans);
    size_t numOrphans = FoxArrayMSize(const ModLocalKey *, &orphans);

    for (uint32_t idx = 0; idx < numOrphans; idx++) {
        ModLocalVal val =
            FoxMapMRemove(ModLocalKey, ModLocalVal, &modLocals,
                          *FoxArrayMPop(const ModLocalKey *, &orphans));
        ModLocalValDeinit(&val);
    }
    FoxArrayMDeinit(const ModLocalKey *, &orphans);

    LogInfo("Done. Pruned %zu local(s).", numOrphans);
    return;
}

void InstanceManRecordHLDLocals(void) {
    LogInfo("Recording vanilla instance locals...");

    size_t numLocals = hldvars.instanceLocalTable->size;
    const char **names = hldvars.instanceLocalTable->elements;
    for (uint32_t idx = 0; idx < numLocals; idx++) {
        *FoxMapMInsert(const char *, int32_t, &hldLocals, names[idx]) = idx + 1;
    }

    LogInfo("Done. Recorded %zu local(s).", numLocals);
    return;
}

void InstanceManConstructor(void) {
    LogInfo("Initializing instance module...");

    FoxStringMapMInit(int32_t, &hldLocals);
    FoxMapMInit(ModLocalKey, ModLocalVal, &modLocals);

    LogInfo("Done initializing instance module.");
    return;
}

void InstanceManDestructor(void) {
    LogInfo("Deinitializing instance module...");

    FoxMapMForEachElement(ModLocalKey, ModLocalVal, &modLocals,
                          ModLocalValDeinitCallback, NULL);
    FoxMapMDeinit(ModLocalKey, ModLocalVal, &modLocals);
    modLocals = (FoxMap){0};

    FoxMapMDeinit(const char *, int32_t, &hldLocals);
    hldLocals = (FoxMap){0};

    LogInfo("Done deinitializing instance module.");
    return;
}

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT size_t AERInstanceGetAll(size_t bufSize, AERInstance **instBuf) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, 0);
    ErrIf(!instBuf && bufSize > 0, AER_NULL_ARG, 0);

    HLDRoom *room = *hldvars.roomCurrent;

    size_t numInsts = room->numInstances;
    size_t numToWrite = FoxMin(numInsts, bufSize);
    HLDInstance *inst = room->instanceFirst;
    for (uint32_t idx = 0; idx < numToWrite; idx++) {
        instBuf[idx] = (AERInstance *)inst;
        inst = inst->instanceNext;
    }

    return numInsts;
}

AER_EXPORT size_t AERInstanceGetByObject(int32_t objIdx, bool recursive,
                                         size_t bufSize,
                                         AERInstance **instBuf) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, 0);
    ErrIf(!instBuf && bufSize > 0, AER_NULL_ARG, 0);

    HLDObject *obj = HLDObjectLookup(objIdx);
    ErrIf(!obj, AER_FAILED_LOOKUP, 0);

    size_t numInsts = obj->numInstances;
    uint32_t bufIdx = 0;
    HLDNodeDLL *node = obj->instanceFirst;
    while (node && bufIdx < bufSize) {
        instBuf[bufIdx++] = (AERInstance *)node->item;
        node = node->next;
    }

    if (recursive) {
        FoxArray *children = ObjectManGetAllChildren(objIdx);
        if (children) {
            size_t numChildren = FoxArrayMSize(int32_t, children);
            for (uint32_t idx = 0; idx < numChildren; idx++) {
                obj = HLDObjectLookup(*FoxArrayMIndex(int32_t, children, idx));
                numInsts += obj->numInstances;
                node = obj->instanceFirst;
                while (node && bufIdx < bufSize) {
                    instBuf[bufIdx++] = (AERInstance *)node->item;
                    node = node->next;
                }
            }
        }
    }

    return numInsts;
}

AER_EXPORT AERInstance *AERInstanceGetById(int32_t instId) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

    AERInstance *inst = (AERInstance *)HLDInstanceLookup(instId);
    ErrIf(!inst, AER_FAILED_LOOKUP, NULL);

    return inst;
}

AER_EXPORT AERInstance *AERInstanceCreate(int32_t objIdx, float x, float y) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);
    ErrIf(!HLDObjectLookup(objIdx), AER_FAILED_LOOKUP, NULL);

    AERInstance *inst =
        (AERInstance *)hldfuncs.actionInstanceCreate(objIdx, x, y);
    ErrIf(!inst, AER_OUT_OF_MEM, NULL);

    return inst;
}

AER_EXPORT void AERInstanceChange(AERInstance *inst, int32_t newObjIdx,
                                  bool doEvents) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);
    ErrIf(!HLDObjectLookup(newObjIdx), AER_FAILED_LOOKUP);

    hldfuncs.actionInstanceChange((HLDInstance *)inst, newObjIdx, doEvents);

    return;
}

AER_EXPORT void AERInstanceDestroy(AERInstance *inst) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);

    hldfuncs.actionInstanceDestroy((HLDInstance *)inst, (HLDInstance *)inst, -1,
                                   true);

    return;
}

AER_EXPORT void AERInstanceDelete(AERInstance *inst) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);

    hldfuncs.actionInstanceDestroy((HLDInstance *)inst, (HLDInstance *)inst, -1,
                                   false);

    return;
}

AER_EXPORT float AERInstanceGetDepth(AERInstance *inst) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, 0.0f);
    ErrIf(!inst, AER_NULL_ARG, 0.0f);

    return ((HLDInstance *)inst)->depth;
}

AER_EXPORT void AERInstanceSetDepth(AERInstance *inst, float depth) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);

    ((HLDInstance *)inst)->depth = depth;

    return;
}

AER_EXPORT void AERInstanceSyncDepth(AERInstance *inst) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);

    uint32_t unknownDatastructure[4] = {0};
    hldfuncs.gmlScriptSetdepth((HLDInstance *)inst, (HLDInstance *)inst,
                               &unknownDatastructure, 0, 0);

    return;
}

AER_EXPORT int32_t AERInstanceGetId(AERInstance *inst) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, -1);
    ErrIf(!inst, AER_NULL_ARG, -1);

    return ((HLDInstance *)inst)->id;
}

AER_EXPORT int32_t AERInstanceGetObject(AERInstance *inst) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, AER_OBJECT_NULL);
    ErrIf(!inst, AER_NULL_ARG, AER_OBJECT_NULL);

    return ((HLDInstance *)inst)->objectIndex;
}

AER_EXPORT void AERInstanceGetPosition(AERInstance *inst, float *x, float *y) {
#define inst ((HLDInstance *)inst)
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);
    ErrIf(!(x || y), AER_NULL_ARG);

    if (x)
        *x = inst->pos.x;
    if (y)
        *y = inst->pos.y;

    return;
#undef inst
}

AER_EXPORT void AERInstanceSetPosition(AERInstance *inst, float x, float y) {
#define inst ((HLDInstance *)inst)
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);

    hldfuncs.Instance_setPosition(inst, x, y);

    return;
#undef inst
}

AER_EXPORT void AERInstanceAddPosition(AERInstance *inst, float x, float y) {
#define inst ((HLDInstance *)inst)
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);

    HLDVecReal pos = inst->pos;
    hldfuncs.Instance_setPosition(inst, pos.x + x, pos.y + y);

    return;
#undef inst
}

AER_EXPORT void AERInstanceGetBoundingBox(AERInstance *inst, float *left,
                                          float *top, float *right,
                                          float *bottom) {
#define inst ((HLDInstance *)inst)
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);
    ErrIf(!(left || top || right || bottom), AER_NULL_ARG);

    if (left)
        *left = (float)inst->bbox.left;
    if (top)
        *top = (float)inst->bbox.top;
    if (right)
        *right = (float)inst->bbox.right;
    if (bottom)
        *bottom = (float)inst->bbox.bottom;

    return;
#undef inst
}

AER_EXPORT float AERInstanceGetFriction(AERInstance *inst) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, 0.0f);
    ErrIf(!inst, AER_NULL_ARG, 0.0f);

    return ((HLDInstance *)inst)->friction;
}

AER_EXPORT void AERInstanceSetFriction(AERInstance *inst, float friction) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);

    ((HLDInstance *)inst)->friction = friction;

    return;
}

AER_EXPORT void AERInstanceGetMotion(AERInstance *inst, float *x, float *y) {
#define inst ((HLDInstance *)inst)
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);
    ErrIf(!(x || y), AER_NULL_ARG);

    if (x)
        *x = inst->speedX;
    if (y)
        *y = inst->speedY;

    return;
#undef inst
}

AER_EXPORT void AERInstanceSetMotion(AERInstance *inst, float x, float y) {
#define inst ((HLDInstance *)inst)
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);

    inst->speedX = x;
    inst->speedY = y;
    hldfuncs.Instance_setMotionPolarFromCartesian(inst);

    return;
#undef inst
}

AER_EXPORT void AERInstanceAddMotion(AERInstance *inst, float x, float y) {
#define inst ((HLDInstance *)inst)
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);

    inst->speedX += x;
    inst->speedY += y;
    hldfuncs.Instance_setMotionPolarFromCartesian(inst);

    return;
#undef inst
}

AER_EXPORT int32_t AERInstanceGetMask(AERInstance *inst) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, AER_SPRITE_NULL);
    ErrIf(!inst, AER_NULL_ARG, AER_SPRITE_NULL);

    return ((HLDInstance *)inst)->maskIndex;
}

AER_EXPORT void AERInstanceSetMask(AERInstance *inst, int32_t maskIdx) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);
    ErrIf(!(maskIdx == -1 || HLDSpriteLookup(maskIdx)), AER_FAILED_LOOKUP);

    hldfuncs.Instance_setMaskIndex((HLDInstance *)inst, maskIdx);

    return;
}

AER_EXPORT int32_t AERInstanceGetSprite(AERInstance *inst) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, AER_SPRITE_NULL);
    ErrIf(!inst, AER_NULL_ARG, AER_SPRITE_NULL);

    return ((HLDInstance *)inst)->spriteIndex;
}

AER_EXPORT void AERInstanceSetSprite(AERInstance *inst, int32_t spriteIdx) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);
    ErrIf(!(spriteIdx == -1 || HLDSpriteLookup(spriteIdx)), AER_FAILED_LOOKUP);

    ((HLDInstance *)inst)->spriteIndex = spriteIdx;

    return;
}

AER_EXPORT float AERInstanceGetSpriteFrame(AERInstance *inst) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, -1.0f);
    ErrIf(!inst, AER_NULL_ARG, -1.0f);

    return ((HLDInstance *)inst)->imageIndex;
}

AER_EXPORT void AERInstanceSetSpriteFrame(AERInstance *inst, float frame) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);

    ((HLDInstance *)inst)->imageIndex = frame;

    return;
}

AER_EXPORT float AERInstanceGetSpriteSpeed(AERInstance *inst) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, -1.0f);
    ErrIf(!inst, AER_NULL_ARG, -1.0f);

    return ((HLDInstance *)inst)->imageSpeed;
}

AER_EXPORT void AERInstanceSetSpriteSpeed(AERInstance *inst, float speed) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);
    ErrIf(speed < 0.0f, AER_BAD_VAL);

    ((HLDInstance *)inst)->imageSpeed = speed;

    return;
}

AER_EXPORT float AERInstanceGetSpriteAlpha(AERInstance *inst) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, -1.0f);
    ErrIf(!inst, AER_NULL_ARG, -1.0f);

    return ((HLDInstance *)inst)->imageAlpha;
}

AER_EXPORT void AERInstanceSetSpriteAlpha(AERInstance *inst, float alpha) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);
    ErrIf(alpha < 0.0f || alpha > 1.0f, AER_BAD_VAL);

    ((HLDInstance *)inst)->imageAlpha = alpha;

    return;
}

AER_EXPORT float AERInstanceGetSpriteAngle(AERInstance *inst) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, 0.0f);
    ErrIf(!inst, AER_NULL_ARG, 0.0f);

    return ((HLDInstance *)inst)->imageAngle;
}

AER_EXPORT void AERInstanceSetSpriteAngle(AERInstance *inst, float angle) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);

    ((HLDInstance *)inst)->imageAngle = angle;

    return;
}

AER_EXPORT void AERInstanceGetSpriteScale(AERInstance *inst, float *x,
                                          float *y) {
#define inst ((HLDInstance *)inst)
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);
    ErrIf(!(x || y), AER_NULL_ARG);

    if (x)
        *x = inst->imageScale.x;
    if (y)
        *y = inst->imageScale.y;

    return;
#undef inst
}

AER_EXPORT void AERInstanceSetSpriteScale(AERInstance *inst, float x, float y) {
#define inst ((HLDInstance *)inst)
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);

    inst->imageScale.x = x;
    inst->imageScale.y = y;

    return;
#undef inst
}

AER_EXPORT uint32_t AERInstanceGetSpriteBlend(AERInstance *inst) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, 0);
    ErrIf(!inst, AER_NULL_ARG, 0);

    return ((HLDInstance *)inst)->imageBlend;
}

AER_EXPORT void AERInstanceSetSpriteBlend(AERInstance *inst, uint32_t color) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);

    ((HLDInstance *)inst)->imageBlend = color;

    return;
}

AER_EXPORT bool AERInstanceGetTangible(AERInstance *inst) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, false);
    ErrIf(!inst, AER_NULL_ARG, false);

    return ((HLDInstance *)inst)->tangible;
}

AER_EXPORT void AERInstanceSetTangible(AERInstance *inst, bool tangible) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);

    ((HLDInstance *)inst)->tangible = tangible;

    return;
}

AER_EXPORT int32_t AERInstanceGetAlarm(AERInstance *inst, uint32_t alarmIdx) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, -1);
    ErrIf(!inst, AER_NULL_ARG, -1);
    ErrIf(alarmIdx >= 12, AER_FAILED_LOOKUP, -1);

    return ((HLDInstance *)inst)->alarms[alarmIdx];
}

AER_EXPORT void AERInstanceSetAlarm(AERInstance *inst, uint32_t alarmIdx,
                                    int32_t numSteps) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);
    ErrIf(alarmIdx >= 12, AER_FAILED_LOOKUP);

    ((HLDInstance *)inst)->alarms[alarmIdx] = numSteps;

    return;
}

AER_EXPORT size_t AERInstanceGetHLDLocals(AERInstance *inst, size_t bufSize,
                                          const char **nameBuf) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, 0);
    ErrIf(!inst, AER_NULL_ARG, 0);
    ErrIf(!nameBuf && bufSize > 0, AER_NULL_ARG, 0);

    const char **names = hldvars.instanceLocalTable->elements;
    HLDClosedHashTable *locals = ((HLDInstance *)inst)->locals;
    HLDClosedHashSlot *slots = locals->slots;

    size_t numLocals = locals->numItems;
    size_t numToWrite = FoxMin(numLocals, bufSize);
    size_t numSlots = locals->numSlots;
    uint32_t bufIdx = 0;
    for (uint32_t slotIdx = 0; slotIdx < numSlots; slotIdx++) {
        if (bufIdx == numToWrite)
            break;
        HLDClosedHashSlot *slot = slots + slotIdx;
        if (slot->value) {
            nameBuf[bufIdx++] = names[slot->nameIdx];
        }
    }

    return numLocals;
}

AER_EXPORT AERLocal *AERInstanceGetHLDLocal(AERInstance *inst,
                                            const char *name) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);
    ErrIf(!inst, AER_NULL_ARG, NULL);
    ErrIf(!name, AER_NULL_ARG, NULL);

    int32_t *localIdx = FoxMapMIndex(const char *, int32_t, &hldLocals, name);
    ErrIf(!localIdx, AER_FAILED_LOOKUP, NULL);

    AERLocal *local =
        HLDClosedHashTableLookup(((HLDInstance *)inst)->locals, *localIdx);
    ErrIf(!local, AER_FAILED_LOOKUP, NULL);

    return local;
}

AER_EXPORT AERLocal *
AERInstanceCreateModLocal(AERInstance *inst, const char *name, bool public,
                          void (*destructor)(AERLocal *local)) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);
    ErrIf(!inst, AER_NULL_ARG, NULL);
    ErrIf(!name, AER_NULL_ARG, NULL);

    ModLocalKey key;
    ErrIf(!ModLocalKeyInit(&key, ((HLDInstance *)inst)->id, name, public),
          AER_BAD_VAL, NULL);
    ErrIf(FoxMapMIndex(ModLocalKey, ModLocalVal, &modLocals, key),
          AER_FAILED_LOOKUP, NULL);

    ModLocalVal *val = FoxMapMInsert(ModLocalKey, ModLocalVal, &modLocals, key);
    val->destructor = destructor;

    return &val->local;
}

AER_EXPORT void AERInstanceDestroyModLocal(AERInstance *inst, const char *name,
                                           bool public) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!inst, AER_NULL_ARG);
    ErrIf(!name, AER_NULL_ARG);

    ModLocalKey key;
    ErrIf(!ModLocalKeyInit(&key, ((HLDInstance *)inst)->id, name, public),
          AER_BAD_VAL);

    ModLocalVal *val = FoxMapMIndex(ModLocalKey, ModLocalVal, &modLocals, key);
    ErrIf(!val, AER_FAILED_LOOKUP);

    ModLocalValDeinit(val);
    FoxMapMRemove(ModLocalKey, ModLocalVal, &modLocals, key);

    return;
}

AER_EXPORT AERLocal AERInstanceDeleteModLocal(AERInstance *inst,
                                              const char *name, bool public) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, (AERLocal){0});
    ErrIf(!inst, AER_NULL_ARG, (AERLocal){0});
    ErrIf(!name, AER_NULL_ARG, (AERLocal){0});

    ModLocalKey key;
    ErrIf(!ModLocalKeyInit(&key, ((HLDInstance *)inst)->id, name, public),
          AER_BAD_VAL, (AERLocal){0});
    ErrIf(!FoxMapMIndex(ModLocalKey, ModLocalVal, &modLocals, key),
          AER_FAILED_LOOKUP, (AERLocal){0});

    return FoxMapMRemove(ModLocalKey, ModLocalVal, &modLocals, key).local;
}

AER_EXPORT AERLocal *AERInstanceGetModLocal(AERInstance *inst, const char *name,
                                            bool public) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);
    ErrIf(!inst, AER_NULL_ARG, NULL);
    ErrIf(!name, AER_NULL_ARG, NULL);

    ModLocalKey key;
    ErrIf(!ModLocalKeyInit(&key, ((HLDInstance *)inst)->id, name, public),
          AER_BAD_VAL, NULL);

    ModLocalVal *val = FoxMapMIndex(ModLocalKey, ModLocalVal, &modLocals, key);
    ErrIf(!val, AER_FAILED_LOOKUP, NULL);

    return &val->local;
}