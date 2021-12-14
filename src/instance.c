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

#define ModLocalValDeinit(val)                                \
    do {                                                      \
        ModLocalVal* ModLocalValDeinit_val = (val);           \
        void (*destructor)(AERLocal*);                        \
        if ((destructor = ModLocalValDeinit_val->destructor)) \
            destructor(&ModLocalValDeinit_val->local);        \
    } while (0)

/* ----- PRIVATE TYPES ----- */

typedef struct __attribute__((packed)) ModLocalKey {
    int32_t modIdx;
    int32_t instId;
    char name[24];
} ModLocalKey;

typedef struct ModLocalVal {
    AERLocal local;
    void (*destructor)(AERLocal*);
} ModLocalVal;

typedef struct GetByObjectContext {
    size_t numInsts;
    size_t bufIdx;
    const size_t bufSize;
    HLDInstance** const instBuf;
} GetByObjectContext;

/* ----- PRIVATE GLOBALS ----- */

static FoxMap hldLocals = {0};

static FoxMap modLocals = {0};

/* ----- PRIVATE FUNCTIONS ----- */

static bool GetByObjectCallback(const int32_t* objIdx,
                                GetByObjectContext* ctx) {
    HLDObject* obj = HLDObjectLookup(*objIdx);
    if (!obj)
        return false;

    ctx->numInsts += obj->numInstances;
    HLDNodeDLL* node = obj->instanceFirst;
    while (node && ctx->bufIdx < ctx->bufSize) {
        ctx->instBuf[ctx->bufIdx++] = node->item;
        node = node->next;
    }

    return true;
}

static bool ModLocalKeyInit(ModLocalKey* key,
                            int32_t instId,
                            const char* name,
                            int32_t modIdx) {
    assert(key);
    assert(name);

    key->modIdx = modIdx;
    key->instId = instId;

    char* keyName = key->name;
    bool endFound = false;
    char curChar;
    for (uint32_t idx = 0; idx < sizeof(key->name); idx++) {
        if (!endFound && (curChar = name[idx]) == '\0')
            endFound = true;
        keyName[idx] = (endFound) ? '\0' : curChar;
    }

    return endFound;
}

static bool ModLocalValDeinitCallback(ModLocalVal* val, void* ctx) {
    (void)ctx;

    ModLocalValDeinit(val);

    return true;
}

static bool ModLocalKeyGetOrphansCallback(const ModLocalKey* key,
                                          FoxArray* orphans) {
    if (!HLDInstanceLookup(key->instId))
        *FoxArrayMPush(const ModLocalKey*, orphans) = key;

    return true;
}

/* ----- INTERNAL FUNCTIONS ----- */

void InstanceManPruneModLocals(void) {
    LogInfo("Pruning mod instance locals...");

    FoxArray orphans;
    FoxArrayMInit(const ModLocalKey*, &orphans);

    FoxMapMForEachKey(ModLocalKey, ModLocalVal, &modLocals,
                      ModLocalKeyGetOrphansCallback, &orphans);
    size_t numOrphans = FoxArrayMSize(const ModLocalKey*, &orphans);

    for (uint32_t idx = 0; idx < numOrphans; idx++) {
        ModLocalVal val =
            FoxMapMRemove(ModLocalKey, ModLocalVal, &modLocals,
                          *FoxArrayMPop(const ModLocalKey*, &orphans));
        ModLocalValDeinit(&val);
    }
    FoxArrayMDeinit(const ModLocalKey*, &orphans);

    LogInfo("Done. Pruned %zu local(s).", numOrphans);
    return;
}

void InstanceManRecordHLDLocals(void) {
    LogInfo("Recording vanilla instance locals...");

    size_t numLocals = hldvars.instanceLocalTable->size;
    const char** names = hldvars.instanceLocalTable->elements;
    for (uint32_t idx = 0; idx < numLocals; idx++) {
        *FoxMapMInsert(const char*, int32_t, &hldLocals, names[idx]) = idx + 1;
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

    FoxMapMDeinit(const char*, int32_t, &hldLocals);
    hldLocals = (FoxMap){0};

    LogInfo("Done deinitializing instance module.");
    return;
}

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT size_t AERInstanceGetAll(size_t bufSize, AERInstance** instBuf) {
#define errRet 0
    EnsureStage(STAGE_ACTION);
    EnsureArgBuf(instBuf, bufSize);

    HLDRoom* room = *hldvars.roomCurrent;

    size_t numInsts = room->numInstances;
    size_t numToWrite = FoxMin(numInsts, bufSize);
    HLDInstance* inst = room->instanceFirst;
    for (uint32_t idx = 0; idx < numToWrite; idx++) {
        instBuf[idx] = (AERInstance*)inst;
        inst = inst->instanceNext;
    }

    Ok(numInsts);
#undef errRet
}

AER_EXPORT size_t AERInstanceGetByObject(int32_t objIdx,
                                         bool recursive,
                                         size_t bufSize,
                                         AERInstance** instBuf) {
#define errRet 0
    EnsureStage(STAGE_ACTION);
    EnsureArgBuf(instBuf, bufSize);

    GetByObjectContext ctx = {
        .numInsts = 0,
        .bufIdx = 0,
        .bufSize = bufSize,
        .instBuf = (HLDInstance**)instBuf,
    };
    EnsureLookup(GetByObjectCallback(&objIdx, &ctx));

    if (recursive) {
        FoxMap* children = ObjectManGetAllChildren(objIdx);
        if (children) {
            FoxMapMForEachKey(int32_t, int32_t, children, GetByObjectCallback,
                              &ctx);
        }
    }

    Ok(ctx.numInsts);
#undef errRet
}

AER_EXPORT AERInstance* AERInstanceGetById(int32_t instId) {
#define errRet NULL
    EnsureStage(STAGE_ACTION);

    AERInstance* inst = (AERInstance*)HLDInstanceLookup(instId);
    EnsureLookup(inst);

    Ok(inst);
#undef errRet
}

AER_EXPORT AERInstance* AERInstanceCreate(int32_t objIdx, float x, float y) {
#define errRet NULL
    EnsureStage(STAGE_ACTION);
    EnsureLookup(HLDObjectLookup(objIdx));

    AERInstance* inst =
        (AERInstance*)hldfuncs.actionInstanceCreate(objIdx, x, y);
    assert(inst);

    Ok(inst);
#undef errRet
}

AER_EXPORT void AERInstanceChange(AERInstance* inst,
                                  int32_t newObjIdx,
                                  bool doEvents) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);
    EnsureLookup(HLDObjectLookup(newObjIdx));

    hldfuncs.actionInstanceChange((HLDInstance*)inst, newObjIdx, doEvents);

    Ok();
#undef errRet
}

AER_EXPORT void AERInstanceDestroy(AERInstance* inst) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    hldfuncs.actionInstanceDestroy((HLDInstance*)inst, (HLDInstance*)inst, -1,
                                   true);

    Ok();
#undef errRet
}

AER_EXPORT void AERInstanceDelete(AERInstance* inst) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    hldfuncs.actionInstanceDestroy((HLDInstance*)inst, (HLDInstance*)inst, -1,
                                   false);

    Ok();
#undef errRet
}

AER_EXPORT float AERInstanceGetDepth(AERInstance* inst) {
#define errRet 0.0f
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    Ok(((HLDInstance*)inst)->depth);
#undef errRet
}

AER_EXPORT void AERInstanceSetDepth(AERInstance* inst, float depth) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    ((HLDInstance*)inst)->depth = depth;

    Ok();
#undef errRet
}

AER_EXPORT void AERInstanceSyncDepth(AERInstance* inst) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    HLDScriptCallAdv(hldfuncs.Script_Setdepth, (HLDInstance*)inst,
                     (HLDInstance*)inst);

    Ok();
#undef errRet
}

AER_EXPORT int32_t AERInstanceGetId(AERInstance* inst) {
#define errRet -1
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    Ok(((HLDInstance*)inst)->id);
#undef errRet
}

AER_EXPORT int32_t AERInstanceGetObject(AERInstance* inst) {
#define errRet AER_OBJECT_NULL
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    Ok(((HLDInstance*)inst)->objectIndex);
#undef errRet
}

AER_EXPORT bool AERInstanceCompatibleWith(AERInstance* inst, int32_t objIdx) {
#define errRet false
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    int32_t instObjIdx = ((HLDInstance*)inst)->objectIndex;
    FoxMap* children = ObjectManGetAllChildren(objIdx);
    if (children && FoxMapMIndex(int32_t, int32_t, children, instObjIdx))
        Ok(true);

    EnsureLookup(HLDObjectLookup(objIdx));
    Ok(objIdx == instObjIdx);
#undef errRet
}

AER_EXPORT bool AERInstanceGetDeactivated(AERInstance* inst) {
#define errRet false
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    Ok(((HLDInstance*)inst)->deactivated);
#undef errRet
}

AER_EXPORT void AERInstanceSetDeactivated(AERInstance* inst, bool deactivated) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    ((HLDInstance*)inst)->deactivated = deactivated;

    Ok();
#undef errRet
}

AER_EXPORT bool AERInstanceGetPersistent(AERInstance* inst) {
#define errRet false
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    Ok(((HLDInstance*)inst)->persistent);
#undef errRet
}

AER_EXPORT void AERInstanceSetPersistent(AERInstance* inst, bool persistent) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    ((HLDInstance*)inst)->persistent = persistent;

    Ok();
#undef errRet
}

AER_EXPORT void AERInstanceGetPosition(AERInstance* inst, float* x, float* y) {
#define errRet
#define inst ((HLDInstance*)inst)
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);
    EnsureArg(x || y);

    HLDVecReal pos = inst->pos;
    if (x)
        *x = pos.x;
    if (y)
        *y = pos.y;

    Ok();
#undef inst
#undef errRet
}

AER_EXPORT void AERInstanceSetPosition(AERInstance* inst, float x, float y) {
#define errRet
#define inst ((HLDInstance*)inst)
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    hldfuncs.Instance_setPosition(inst, x, y);

    Ok();
#undef inst
#undef errRet
}

AER_EXPORT void AERInstanceAddPosition(AERInstance* inst, float x, float y) {
#define errRet
#define inst ((HLDInstance*)inst)
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    HLDVecReal pos = inst->pos;
    hldfuncs.Instance_setPosition(inst, pos.x + x, pos.y + y);

    Ok();
#undef inst
#undef errRet
}

AER_EXPORT void AERInstanceGetBoundingBox(AERInstance* inst,
                                          float* left,
                                          float* top,
                                          float* right,
                                          float* bottom) {
#define errRet
#define inst ((HLDInstance*)inst)
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);
    EnsureArg(left || top || right || bottom);

    HLDBoundingBox bbox = inst->bbox;
    if (left)
        *left = (float)bbox.left;
    if (top)
        *top = (float)bbox.top;
    if (right)
        *right = (float)bbox.right;
    if (bottom)
        *bottom = (float)bbox.bottom;

    Ok();
#undef inst
#undef errRet
}

AER_EXPORT float AERInstanceGetFriction(AERInstance* inst) {
#define errRet 0.0f
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    Ok(((HLDInstance*)inst)->friction);
#undef errRet
}

AER_EXPORT void AERInstanceSetFriction(AERInstance* inst, float friction) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    ((HLDInstance*)inst)->friction = friction;

    Ok();
#undef errRet
}

AER_EXPORT void AERInstanceGetMotion(AERInstance* inst, float* x, float* y) {
#define errRet
#define inst ((HLDInstance*)inst)
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);
    EnsureArg(x || y);

    if (x)
        *x = inst->speedX;
    if (y)
        *y = inst->speedY;

    Ok();
#undef inst
#undef errRet
}

AER_EXPORT void AERInstanceSetMotion(AERInstance* inst, float x, float y) {
#define errRet
#define inst ((HLDInstance*)inst)
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    inst->speedX = x;
    inst->speedY = y;
    hldfuncs.Instance_setMotionPolarFromCartesian(inst);

    Ok();
#undef inst
#undef errRet
}

AER_EXPORT void AERInstanceAddMotion(AERInstance* inst, float x, float y) {
#define errRet
#define inst ((HLDInstance*)inst)
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    inst->speedX += x;
    inst->speedY += y;
    hldfuncs.Instance_setMotionPolarFromCartesian(inst);

    Ok();
#undef inst
#undef errRet
}

AER_EXPORT int32_t AERInstanceGetMask(AERInstance* inst) {
#define errRet AER_SPRITE_NULL
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    Ok(((HLDInstance*)inst)->maskIndex);
#undef errRet
}

AER_EXPORT void AERInstanceSetMask(AERInstance* inst, int32_t maskIdx) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);
    EnsureLookup(maskIdx == AER_SPRITE_NULL || HLDSpriteLookup(maskIdx));

    hldfuncs.Instance_setMaskIndex((HLDInstance*)inst, maskIdx);

    Ok();
#undef errRet
}

AER_EXPORT bool AERInstanceGetVisible(AERInstance* inst) {
#define errRet false
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    Ok(((HLDInstance*)inst)->visible);
#undef errRet
}

AER_EXPORT void AERInstanceSetVisible(AERInstance* inst, bool visible) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    ((HLDInstance*)inst)->visible = visible;

    Ok();
#undef errRet
}

AER_EXPORT int32_t AERInstanceGetSprite(AERInstance* inst) {
#define errRet AER_SPRITE_NULL
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    Ok(((HLDInstance*)inst)->spriteIndex);
#undef errRet
}

AER_EXPORT void AERInstanceSetSprite(AERInstance* inst, int32_t spriteIdx) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);
    EnsureLookup(spriteIdx == AER_SPRITE_NULL || HLDSpriteLookup(spriteIdx));

    ((HLDInstance*)inst)->spriteIndex = spriteIdx;

    Ok();
#undef errRet
}

AER_EXPORT float AERInstanceGetSpriteFrame(AERInstance* inst) {
#define errRet -1.0f
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    Ok(((HLDInstance*)inst)->imageIndex);
#undef errRet
}

AER_EXPORT void AERInstanceSetSpriteFrame(AERInstance* inst, float frame) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    ((HLDInstance*)inst)->imageIndex = frame;

    Ok();
#undef errRet
}

AER_EXPORT float AERInstanceGetSpriteSpeed(AERInstance* inst) {
#define errRet -1.0f
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    Ok(((HLDInstance*)inst)->imageSpeed);
#undef errRet
}

AER_EXPORT void AERInstanceSetSpriteSpeed(AERInstance* inst, float speed) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);
    EnsureMin(speed, 0.0f);

    ((HLDInstance*)inst)->imageSpeed = speed;

    Ok();
#undef errRet
}

AER_EXPORT float AERInstanceGetSpriteAlpha(AERInstance* inst) {
#define errRet -1.0f
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    Ok(((HLDInstance*)inst)->imageAlpha);
#undef errRet
}

AER_EXPORT void AERInstanceSetSpriteAlpha(AERInstance* inst, float alpha) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);
    EnsureProba(alpha);

    ((HLDInstance*)inst)->imageAlpha = alpha;

    Ok();
#undef errRet
}

AER_EXPORT float AERInstanceGetSpriteAngle(AERInstance* inst) {
#define errRet 0.0f
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    Ok(((HLDInstance*)inst)->imageAngle);
#undef errRet
}

AER_EXPORT void AERInstanceSetSpriteAngle(AERInstance* inst, float angle) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    ((HLDInstance*)inst)->imageAngle = angle;

    Ok();
#undef errRet
}

AER_EXPORT void AERInstanceGetSpriteScale(AERInstance* inst,
                                          float* x,
                                          float* y) {
#define errRet
#define inst ((HLDInstance*)inst)
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);
    EnsureArg(x || y);

    if (x)
        *x = inst->imageScale.x;
    if (y)
        *y = inst->imageScale.y;

    Ok();
#undef inst
#undef errRet
}

AER_EXPORT void AERInstanceSetSpriteScale(AERInstance* inst, float x, float y) {
#define errRet
#define inst ((HLDInstance*)inst)
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    inst->imageScale.x = x;
    inst->imageScale.y = y;

    Ok();
#undef inst
#undef errRet
}

AER_EXPORT uint32_t AERInstanceGetSpriteBlend(AERInstance* inst) {
#define errRet 0
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    Ok(((HLDInstance*)inst)->imageBlend);
#undef errRet
}

AER_EXPORT void AERInstanceSetSpriteBlend(AERInstance* inst, uint32_t color) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    ((HLDInstance*)inst)->imageBlend = color;

    Ok();
#undef errRet
}

AER_EXPORT bool AERInstanceGetTangible(AERInstance* inst) {
#define errRet false
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    Ok(((HLDInstance*)inst)->tangible);
#undef errRet
}

AER_EXPORT void AERInstanceSetTangible(AERInstance* inst, bool tangible) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);

    ((HLDInstance*)inst)->tangible = tangible;

    Ok();
#undef errRet
}

AER_EXPORT int32_t AERInstanceGetAlarm(AERInstance* inst, uint32_t alarmIdx) {
#define errRet -1
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);
    EnsureMax(alarmIdx, 11);

    Ok(((HLDInstance*)inst)->alarms[alarmIdx]);
#undef errRet
}

AER_EXPORT void AERInstanceSetAlarm(AERInstance* inst,
                                    uint32_t alarmIdx,
                                    int32_t numSteps) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);
    EnsureMax(alarmIdx, 11);

    ((HLDInstance*)inst)->alarms[alarmIdx] = numSteps;

    Ok();
#undef errRet
}

AER_EXPORT size_t AERInstanceGetHLDLocals(AERInstance* inst,
                                          size_t bufSize,
                                          const char** nameBuf) {
#define errRet 0
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);
    EnsureArgBuf(nameBuf, bufSize);

    const char** names = hldvars.instanceLocalTable->elements;
    HLDClosedHashTable* locals = ((HLDInstance*)inst)->locals;
    HLDClosedHashSlot* slots = locals->slots;

    size_t numLocals = locals->numItems;
    size_t numToWrite = FoxMin(numLocals, bufSize);
    size_t numSlots = locals->numSlots;
    uint32_t bufIdx = 0;
    for (uint32_t slotIdx = 0; slotIdx < numSlots; slotIdx++) {
        if (bufIdx == numToWrite)
            break;
        HLDClosedHashSlot* slot = slots + slotIdx;
        if (slot->value) {
            nameBuf[bufIdx++] = names[slot->nameIdx];
        }
    }

    Ok(numLocals);
#undef errRet
}

AER_EXPORT AERLocal* AERInstanceGetHLDLocal(AERInstance* inst,
                                            const char* name) {
#define errRet NULL
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);
    EnsureArg(name);

    int32_t* localIdx = FoxMapMIndex(const char*, int32_t, &hldLocals, name);
    EnsureLookup(localIdx);

    AERLocal* local =
        HLDClosedHashTableLookup(((HLDInstance*)inst)->locals, *localIdx);
    EnsureLookup(local);

    Ok(local);
#undef errRet
}

AER_EXPORT AERLocal* AERInstanceCreateModLocal(
    AERInstance* inst,
    const char* name,
    bool public,
    void (*destructor)(AERLocal* local)) {
#define errRet NULL
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);
    EnsureArg(name);

    ModLocalKey key;
    Ensure(ModLocalKeyInit(&key, ((HLDInstance*)inst)->id, name,
                           public ? MOD_NULL : ModManGetCurrentMod()->idx),
           AER_BAD_VAL);
    EnsureLookup(!FoxMapMIndex(ModLocalKey, ModLocalVal, &modLocals, key));

    ModLocalVal* val = FoxMapMInsert(ModLocalKey, ModLocalVal, &modLocals, key);
    val->destructor = destructor;

    Ok(&val->local);
#undef errRet
}

AER_EXPORT void AERInstanceDestroyModLocal(AERInstance* inst,
                                           const char* name,
                                           bool public) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);
    EnsureArg(name);

    ModLocalKey key;
    Ensure(ModLocalKeyInit(&key, ((HLDInstance*)inst)->id, name,
                           public ? MOD_NULL : ModManGetCurrentMod()->idx),
           AER_BAD_VAL);

    ModLocalVal* val = FoxMapMIndex(ModLocalKey, ModLocalVal, &modLocals, key);
    EnsureLookup(val);

    ModLocalValDeinit(val);
    FoxMapMRemove(ModLocalKey, ModLocalVal, &modLocals, key);

    Ok();
#undef errRet
}

AER_EXPORT AERLocal AERInstanceDeleteModLocal(AERInstance* inst,
                                              const char* name,
                                              bool public) {
#define errRet (AERLocal){0};
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);
    EnsureArg(name);

    ModLocalKey key;
    Ensure(ModLocalKeyInit(&key, ((HLDInstance*)inst)->id, name,
                           public ? MOD_NULL : ModManGetCurrentMod()->idx),
           AER_BAD_VAL);
    EnsureLookup(FoxMapMIndex(ModLocalKey, ModLocalVal, &modLocals, key));

    Ok(FoxMapMRemove(ModLocalKey, ModLocalVal, &modLocals, key).local);
#undef errRet
}

AER_EXPORT AERLocal* AERInstanceGetModLocal(AERInstance* inst,
                                            const char* name,
                                            bool public) {
#define errRet NULL
    EnsureStage(STAGE_ACTION);
    EnsureArg(inst);
    EnsureArg(name);

    ModLocalKey key;
    Ensure(ModLocalKeyInit(&key, ((HLDInstance*)inst)->id, name,
                           public ? MOD_NULL : ModManGetCurrentMod()->idx),
           AER_BAD_VAL);

    ModLocalVal* val = FoxMapMIndex(ModLocalKey, ModLocalVal, &modLocals, key);
    EnsureLookup(val);

    Ok(&val->local);
#undef errRet
}