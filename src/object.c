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
#include <stdlib.h>
#include <string.h>

#include "foxutils/mapmacs.h"
#include "foxutils/math.h"
#include "foxutils/stringmapmacs.h"

#include "aer/object.h"
#include "aer/sprite.h"
#include "internal/core.h"
#include "internal/err.h"
#include "internal/event.h"
#include "internal/export.h"
#include "internal/hld.h"
#include "internal/log.h"
#include "internal/mod.h"
#include "internal/object.h"

/* ----- PRIVATE TYPES ----- */

typedef struct ObjTreeGetAllChildrenContext {
    FoxMap* allChildren;
    int32_t depth;
} ObjTreeGetAllChildrenContext;

typedef struct ObjTreeCopyChildrenContext {
    int32_t* objBuf;
    int32_t* bufPos;
} ObjTreeCopyChildrenContext;

/* ----- PRIVATE GLOBALS ----- */

static FoxMap objTree = {0};

static FoxMap flatObjTree = {0};

static FoxMap objNames = {0};

/* ----- PRIVATE FUNCTIONS ----- */

static bool ObjTreeGetAllChildrenCallback(const int32_t* objIdx,
                                          ObjTreeGetAllChildrenContext* ctx) {
    ObjTreeGetAllChildrenContext curCtx = {
        .allChildren = ctx->allChildren,
        .depth = ctx->depth + 1,
    };

    *FoxMapMInsert(int32_t, int32_t, curCtx.allChildren, *objIdx) =
        curCtx.depth;
    FoxMap* nextGen = FoxMapMIndex(int32_t, FoxMap, &objTree, *objIdx);
    if (nextGen) {
        FoxMapMForEachKey(int32_t, int32_t, nextGen,
                          ObjTreeGetAllChildrenCallback, &curCtx);
    }

    return true;
}

static bool ObjTreeBuildFlatObjTreeCallback(const int32_t* objIdx,
                                            FoxMap* directChildren,
                                            void* ctx) {
    (void)ctx;

    ObjTreeGetAllChildrenContext initCtx = {
        .allChildren = FoxMapMInsert(int32_t, FoxMap, &flatObjTree, *objIdx),
        .depth = 0,
    };

    FoxMapMInit(int32_t, int32_t, initCtx.allChildren);
    FoxMapMForEachKey(int32_t, int32_t, directChildren,
                      ObjTreeGetAllChildrenCallback, &initCtx);

    return true;
}

static bool ObjTreeCopyChildrenCallback(const int32_t* objIdx,
                                        ObjTreeCopyChildrenContext* ctx) {
    *(--ctx->bufPos) = *objIdx;

    return ctx->bufPos > ctx->objBuf;
}

static bool ObjTreeChildrenDeinitCallback(FoxMap* children, void* ctx) {
    (void)ctx;

    FoxMapMDeinit(int32_t, int32_t, children);

    return true;
}

/* ----- INTERNAL FUNCTIONS ----- */

FoxMap* ObjectManGetDirectChildren(int32_t objIdx) {
    return FoxMapMIndex(int32_t, FoxMap, &objTree, objIdx);
}

FoxMap* ObjectManGetAllChildren(int32_t objIdx) {
    return FoxMapMIndex(int32_t, FoxMap, &flatObjTree, objIdx);
}

void ObjectManBuildNameTable(void) {
    size_t numObjs = (*hldvars.objectTableHandle)->numItems;
    for (uint32_t objIdx = 0; objIdx < numObjs; objIdx++) {
        HLDObject* obj = HLDObjectLookup(objIdx);
        assert(obj);
        *FoxMapMInsert(const char*, int32_t, &objNames, obj->name) = objIdx;
    }

    return;
}

void ObjectManBuildInheritanceTrees(void) {
    /* Build object tree. */
    size_t numObjs = (*hldvars.objectTableHandle)->numItems;
    for (size_t objIdx = 0; objIdx < numObjs; objIdx++) {
        HLDObject* obj = HLDObjectLookup(objIdx);
        assert(obj);
        int32_t parentIdx = obj->parentIndex;
        FoxMap* directChildren =
            FoxMapMIndex(int32_t, FoxMap, &objTree, parentIdx);
        if (!directChildren) {
            directChildren =
                FoxMapMInsert(int32_t, FoxMap, &objTree, parentIdx);
            FoxMapMInit(int32_t, int32_t, directChildren);
        }
        *FoxMapMInsert(int32_t, int32_t, directChildren, objIdx) = 1;
    }

    /* Build flat object tree. */
    FoxMapMForEachPair(int32_t, FoxMap, &objTree,
                       ObjTreeBuildFlatObjTreeCallback, NULL);

    return;
}

void ObjectManConstructor(void) {
    LogInfo("Initializing object module...");

    FoxMapMInit(int32_t, FoxMap, &objTree);
    FoxMapMInit(int32_t, FoxMap, &flatObjTree);
    FoxStringMapMInit(int32_t, &objNames);

    LogInfo("Done initializing object module.");
    return;
}

void ObjectManDestructor(void) {
    LogInfo("Deinitializing object module...");

    /* Deinitialize object tree. */
    FoxMapMForEachElement(int32_t, FoxMap, &objTree,
                          ObjTreeChildrenDeinitCallback, NULL);
    FoxMapMDeinit(int32_t, FoxMap, &objTree);
    objTree = (FoxMap){0};

    /* Deinitialize flat object tree. */
    FoxMapMForEachElement(int32_t, FoxMap, &flatObjTree,
                          ObjTreeChildrenDeinitCallback, NULL);
    FoxMapMDeinit(int32_t, FoxMap, &flatObjTree);
    flatObjTree = (FoxMap){0};

    /* Deinitialize name table. */
    FoxMapMDeinit(const char*, int32_t, &objNames);
    objNames = (FoxMap){0};

    LogInfo("Done deinitializing object module.");
    return;
}

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT int32_t AERObjectRegister(const char* name,
                                     int32_t parentIdx,
                                     int32_t spriteIdx,
                                     int32_t maskIdx,
                                     int32_t depth,
                                     bool visible,
                                     bool collisions,
                                     bool persistent) {
#define errRet AER_OBJECT_NULL
    EnsureArg(name);
    LogInfo("Registering object \"%s\" for mod \"%s\"...", name,
            ModManGetCurrentMod()->name);
    EnsureStageStrict(STAGE_OBJECT_REG);

    HLDObject* parent = HLDObjectLookup(parentIdx);
    EnsureLookup(parent);

    EnsureLookup(spriteIdx == AER_SPRITE_NULL || HLDSpriteLookup(spriteIdx));
    EnsureLookup(maskIdx == AER_SPRITE_NULL || HLDSpriteLookup(maskIdx));
    Ensure(!FoxMapMIndex(const char*, int32_t, &objNames, name), AER_BAD_VAL);

    int32_t objIdx = hldfuncs.actionObjectAdd();
    HLDObject* obj = HLDObjectLookup(objIdx);
    assert(obj);
    *FoxMapMInsert(const char*, int32_t, &objNames, name) = objIdx;

    /* The engine expects a freeable (dynamically allocated) string for name. */
    char* tmpName = malloc(strlen(name) + 1);
    assert(tmpName);
    obj->name = strcpy(tmpName, name);

    obj->parentIndex = parentIdx;
    obj->parent = parent;
    obj->spriteIndex = spriteIdx;
    obj->maskIndex = maskIdx;
    obj->depth = depth;
    obj->flags.visible = visible;
    obj->flags.collisions = collisions;
    obj->flags.persistent = persistent;

    LogInfo("Successfully registered object to index %i.", objIdx);
    Ok(objIdx);
#undef errRet
}

AER_EXPORT size_t AERObjectGetNumRegistered(void) {
#define errRet 0
    EnsureStage(STAGE_OBJECT_REG);

    Ok((*hldvars.objectTableHandle)->numItems);
#undef errRet
}

AER_EXPORT int32_t AERObjectGetByName(const char* name) {
#define errRet AER_OBJECT_NULL
    EnsureStage(STAGE_OBJECT_REG);
    EnsureArg(name);

    int32_t* objIdx = FoxMapMIndex(const char*, int32_t, &objNames, name);
    EnsureLookup(objIdx);

    Ok(*objIdx);
#undef errRet
}

AER_EXPORT const char* AERObjectGetName(int32_t objIdx) {
#define errRet NULL
    EnsureStage(STAGE_OBJECT_REG);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);

    Ok(obj->name);
#undef errRet
}

AER_EXPORT int32_t AERObjectGetParent(int32_t objIdx) {
#define errRet AER_OBJECT_NULL
    EnsureStage(STAGE_OBJECT_REG);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);

    Ok(obj->parentIndex);
#undef errRet
}

AER_EXPORT size_t AERObjectGetChildren(int32_t objIdx,
                                       bool recursive,
                                       size_t bufSize,
                                       int32_t* objBuf) {
#define errRet 0
    EnsureStage(STAGE_OBJECT_REG);
    EnsureArgBuf(objBuf, bufSize);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);

    FoxMap* children = FoxMapMIndex(
        int32_t, FoxMap, (recursive ? &flatObjTree : &objTree), objIdx);
    if (!children)
        Ok(0);
    size_t numChildren = FoxMapMSize(int32_t, int32_t, children);

    size_t numToWrite = FoxMin(numChildren, bufSize);
    if (numToWrite > 0) {
        ObjTreeCopyChildrenContext ctx = {
            .objBuf = objBuf,
            .bufPos = objBuf + numToWrite,
        };
        FoxMapMForEachKey(int32_t, int32_t, children,
                          ObjTreeCopyChildrenCallback, &ctx);
    }

    Ok(numChildren);
#undef errRet
}

AER_EXPORT bool AERObjectGetCollisions(int32_t objIdx) {
#define errRet false
    EnsureStage(STAGE_OBJECT_REG);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);

    Ok(obj->flags.collisions);
#undef errRet
}

AER_EXPORT void AERObjectSetCollisions(int32_t objIdx, bool collisions) {
#define errRet
    EnsureStage(STAGE_OBJECT_REG);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);
    obj->flags.collisions = collisions;

    Ok();
#undef errRet
}

AER_EXPORT bool AERObjectGetPersistent(int32_t objIdx) {
#define errRet false
    EnsureStage(STAGE_OBJECT_REG);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);

    Ok(obj->flags.persistent);
#undef errRet
}

AER_EXPORT void AERObjectSetPersistent(int32_t objIdx, bool persistent) {
#define errRet
    EnsureStage(STAGE_OBJECT_REG);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);
    obj->flags.persistent = persistent;

    Ok();
#undef errRet
}

AER_EXPORT bool AERObjectGetVisible(int32_t objIdx) {
#define errRet false
    EnsureStage(STAGE_OBJECT_REG);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);

    Ok(obj->flags.visible);
#undef errRet
}

AER_EXPORT void AERObjectSetVisible(int32_t objIdx, bool visible) {
#define errRet
    EnsureStage(STAGE_OBJECT_REG);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);
    obj->flags.visible = visible;

    Ok();
#undef errRet
}

AER_EXPORT void AERObjectAttachCreateListener(int32_t objIdx,
                                              bool (*listener)(AEREvent*,
                                                               AERInstance*,
                                                               AERInstance*)) {
#define errRet
    LogInfo("Attaching create listener to object %i for mod \"%s\"...", objIdx,
            ModManGetCurrentMod()->name);

    EnsureStageStrict(STAGE_LISTENER_REG);
    EnsureArg(listener);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);

    EventKey key =
        (EventKey){.type = HLD_EVENT_CREATE, .num = 0, .objIdx = objIdx};
    EventManRegisterEventListener(obj, key, listener);

    LogInfo("Successfully attached create listener.");
    Ok();
#undef errRet
}

AER_EXPORT void AERObjectAttachDestroyListener(int32_t objIdx,
                                               bool (*listener)(AEREvent*,
                                                                AERInstance*,
                                                                AERInstance*)) {
#define errRet
    LogInfo("Attaching destroy listener to object %i for mod \"%s\"...", objIdx,
            ModManGetCurrentMod()->name);

    EnsureStageStrict(STAGE_LISTENER_REG);
    EnsureArg(listener);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);

    EventKey key = {.type = HLD_EVENT_DESTROY, .num = 0, .objIdx = objIdx};
    EventManRegisterEventListener(obj, key, listener);

    LogInfo("Successfully attached destroy listener.");
    Ok();
#undef errRet
}

AER_EXPORT void AERObjectAttachAlarmListener(int32_t objIdx,
                                             uint32_t alarmIdx,
                                             bool (*listener)(AEREvent*,
                                                              AERInstance*,
                                                              AERInstance*)) {
#define errRet
    LogInfo("Attaching alarm %u listener to object %i for mod \"%s\"...",
            alarmIdx, objIdx, ModManGetCurrentMod()->name);

    EnsureStageStrict(STAGE_LISTENER_REG);
    EnsureArg(listener);
    EnsureMax(alarmIdx, 11);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);

    EventKey key = {.type = HLD_EVENT_ALARM, .num = alarmIdx, .objIdx = objIdx};
    EventManRegisterEventListener(obj, key, listener);

    LogInfo("Successfully attached alarm %u listener.", alarmIdx);
    Ok();
#undef errRet
}

AER_EXPORT void AERObjectAttachStepListener(int32_t objIdx,
                                            bool (*listener)(AEREvent*,
                                                             AERInstance*,
                                                             AERInstance*)) {
#define errRet
    LogInfo("Attaching step listener to object %i for mod \"%s\"...", objIdx,
            ModManGetCurrentMod()->name);

    EnsureStageStrict(STAGE_LISTENER_REG);
    EnsureArg(listener);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);

    EventKey key = {
        .type = HLD_EVENT_STEP, .num = HLD_EVENT_STEP_NORMAL, .objIdx = objIdx};
    EventManRegisterEventListener(obj, key, listener);

    LogInfo("Successfully attached step listener.");
    Ok();
#undef errRet
}

AER_EXPORT void AERObjectAttachPreStepListener(int32_t objIdx,
                                               bool (*listener)(AEREvent*,
                                                                AERInstance*,
                                                                AERInstance*)) {
#define errRet
    LogInfo("Attaching pre-step listener to object %i for mod \"%s\"...",
            objIdx, ModManGetCurrentMod()->name);

    EnsureStageStrict(STAGE_LISTENER_REG);
    EnsureArg(listener);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);

    EventKey key = {
        .type = HLD_EVENT_STEP, .num = HLD_EVENT_STEP_PRE, .objIdx = objIdx};
    EventManRegisterEventListener(obj, key, listener);

    LogInfo("Successfully attached pre-step listener.");
    Ok();
#undef errRet
}

AER_EXPORT void AERObjectAttachPostStepListener(
    int32_t objIdx,
    bool (*listener)(AEREvent*, AERInstance*, AERInstance*)) {
#define errRet
    LogInfo("Attaching post-step listener to object %i for mod \"%s\"...",
            objIdx, ModManGetCurrentMod()->name);

    EnsureStageStrict(STAGE_LISTENER_REG);
    EnsureArg(listener);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);

    EventKey key = {
        .type = HLD_EVENT_STEP, .num = HLD_EVENT_STEP_POST, .objIdx = objIdx};
    EventManRegisterEventListener(obj, key, listener);

    LogInfo("Successfully attached post-step listener.");
    Ok();
#undef errRet
}

AER_EXPORT void AERObjectAttachCollisionListener(
    int32_t targetObjIdx,
    int32_t otherObjIdx,
    bool (*listener)(AEREvent*, AERInstance*, AERInstance*)) {
#define errRet
    LogInfo("Attaching %i collision listener to object %i for mod \"%s\"...",
            otherObjIdx, targetObjIdx, ModManGetCurrentMod()->name);

    EnsureStageStrict(STAGE_LISTENER_REG);
    EnsureArg(listener);
    EnsureLookup(HLDObjectLookup(otherObjIdx));

    HLDObject* obj = HLDObjectLookup(targetObjIdx);
    EnsureLookup(obj);

    EventKey key = {.type = HLD_EVENT_COLLISION,
                    .num = otherObjIdx,
                    .objIdx = targetObjIdx};
    EventManRegisterEventListener(obj, key, listener);

    LogInfo("Successfully attached %i collision listener.", otherObjIdx);
    Ok();
#undef errRet
}

AER_EXPORT void AERObjectAttachAnimationEndListener(
    int32_t objIdx,
    bool (*listener)(AEREvent*, AERInstance*, AERInstance*)) {
#define errRet
    LogInfo("Attaching animation end listener to object %i for mod \"%s\"...",
            objIdx, ModManGetCurrentMod()->name);

    EnsureStageStrict(STAGE_LISTENER_REG);
    EnsureArg(listener);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);

    EventKey key = {.type = HLD_EVENT_OTHER,
                    .num = HLD_EVENT_OTHER_ANIMATION_END,
                    .objIdx = objIdx};
    EventManRegisterEventListener(obj, key, listener);

    LogInfo("Successfully attached animation end listener.");
    Ok();
#undef errRet
}

AER_EXPORT void AERObjectAttachDrawListener(
    int32_t objIdx,
    bool (*listener)(AEREvent* event,
                     AERInstance* target,
                     AERInstance* other)) {
#define errRet
    LogInfo("Attaching draw listener to object %i for mod \"%s\"...", objIdx,
            ModManGetCurrentMod()->name);

    EnsureStageStrict(STAGE_LISTENER_REG);
    EnsureArg(listener);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);

    EventKey key = {
        .type = HLD_EVENT_DRAW, .num = HLD_EVENT_DRAW_NORMAL, .objIdx = objIdx};
    EventManRegisterEventListener(obj, key, listener);

    LogInfo("Successfully attached draw listener.");
    Ok();
#undef errRet
}

AER_EXPORT void AERObjectAttachGUIDrawListener(
    int32_t objIdx,
    bool (*listener)(AEREvent* event,
                     AERInstance* target,
                     AERInstance* other)) {
#define errRet
    LogInfo("Attaching GUI-draw listener to object %i for mod \"%s\"...",
            objIdx, ModManGetCurrentMod()->name);

    EnsureStageStrict(STAGE_LISTENER_REG);
    EnsureArg(listener);

    HLDObject* obj = HLDObjectLookup(objIdx);
    EnsureLookup(obj);

    EventKey key = {.type = HLD_EVENT_DRAW,
                    .num = HLD_EVENT_DRAW_GUI_NORMAL,
                    .objIdx = objIdx};
    EventManRegisterEventListener(obj, key, listener);

    LogInfo("Successfully attached GUI-draw listener.");
    Ok();
#undef errRet
}