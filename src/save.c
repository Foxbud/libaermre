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
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "foxutils/arraymacs.h"
#include "foxutils/math.h"
#include "foxutils/stringmapmacs.h"

#include "aer/object.h"
#include "aer/save.h"
#include "internal/core.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/log.h"
#include "internal/mod.h"
#include "internal/save.h"

/* ----- PRIVATE MACROS ----- */

#define HLD_MAIN_MAP_KEY "mod"

#define SaveEntryDeinit(entry)                                                 \
    do {                                                                       \
        SaveEntry *SaveEntryDeinit_entry = (entry);                            \
        if (SaveEntryDeinit_entry->type == SAVE_STRING)                        \
            free(SaveEntryDeinit_entry->value.s);                              \
        *SaveEntryDeinit_entry = (SaveEntry){0};                               \
    } while (0)

#define EnsureType(entry, expType)                                             \
    Ensure(((entry)->type == (expType)), AER_FAILED_PARSE);

/* ----- PRIVATE TYPES ----- */

typedef enum SaveType { SAVE_NULL, SAVE_DOUBLE, SAVE_STRING } SaveType;

typedef struct SaveEntry {
    SaveType type;
    union {
        double d;
        char *s;
    } value;
} SaveEntry;

typedef struct SaveEntryRecordContext {
    const char *modName;
    HLDPrimitive hldModMapId;
} SaveEntryRecordContext;

typedef struct SaveEntryGetKeysContext {
    const char **const keyBuf;
    size_t numToWrite;
} SaveEntryGetKeysContext;

/* ----- PRIVATE GLOBALS ----- */

static FoxArray modMaps = {0};

/* ----- PRIVATE FUNCTIONS ----- */

static bool SaveEntryDeinitCallback(SaveEntry *entry, void *ctx) {
    (void)ctx;

    SaveEntryDeinit(entry);

    return true;
}

static bool SaveEntryRecordCallback(const char **key, SaveEntry *entry,
                                    SaveEntryRecordContext *ctx) {
    /* Copy value to HLD primitive. */
    HLDPrimitiveMakeUndefined(hldVal);
    switch (entry->type) {
    case SAVE_DOUBLE: {
        /* Copy double value. */
        HLDPrimitiveMakeReal(tmpVal, entry->value.d);
        hldVal = tmpVal;
        break;
    }

    case SAVE_STRING: {
        /* Copy string value. */
        size_t valLen = strlen(entry->value.s);
        HLDPrimitiveMakeStringH(
            tmpVal, memcpy(malloc(valLen + 1), entry->value.s, valLen + 1),
            valLen);
        hldVal = tmpVal;
        break;
    }

    default:
        /* Illegal value. */
        LogErr("Encountered illegal SaveEntry type %i while "
               "saving key \"%s\" of \"%s\" mod's data!",
               entry->type, *key, ctx->modName);
        abort();
    }

    /* Insert value into hld sub-map. */
    size_t keyLen = strlen(*key);
    HLDPrimitiveMakeStringH(
        hldKey, memcpy(malloc(keyLen + 1), *key, keyLen + 1), keyLen);
    HLDAPICall(hldfuncs.API_dsMapSet, ctx->hldModMapId, hldKey, hldVal);

    return true;
}

static bool SaveEntryGetKeysCallback(const char **key,
                                     SaveEntryGetKeysContext *ctx) {
    if (ctx->numToWrite == 0)
        return false;

    ctx->keyBuf[--(ctx->numToWrite)] = *key;

    return true;
}

static void ResetModMaps(void) {
    size_t numMods = FoxArrayMSize(FoxMap, &modMaps);
    for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
        FoxMap *map = FoxArrayMIndex(FoxMap, &modMaps, modIdx);
        FoxMapMForEachElement(const char *, SaveEntry, map,
                              SaveEntryDeinitCallback, NULL);
        FoxMapMDeinit(const char *, SaveEntry, map);
        FoxStringMapMInit(SaveEntry, map);
    }

    return;
}

/* ----- INTERNAL FUNCTIONS ----- */

int32_t SaveManGetCurrentSlot(void) {
    /* Get data instance. */
    HLDObject *dataObj = HLDObjectLookup(AER_OBJECT_DATA);
    assert(dataObj->numInstances == 1);
    HLDInstance *dataInst = dataObj->instanceFirst->item;

    /* Get save slot local. */
    HLDPrimitive *saveSlotLocal = HLDClosedHashTableLookup(
        dataInst->locals, 0x4a0 /* "currentSaveFileNo" */);
    assert(saveSlotLocal);

    return (int32_t)saveSlotLocal->value.r;
}

void SaveManLoadData(HLDPrimitive *hldDataMapId) {
    LogInfo("Loading mod data...");
    ResetModMaps();

    /* Determine whether any mod data to load. */
    HLDPrimitiveMakeStringS(hldMainMapKey, HLD_MAIN_MAP_KEY,
                            sizeof(HLD_MAIN_MAP_KEY) - 1);
    HLDPrimitive hldMainMapId =
        HLDAPICall(hldfuncs.API_dsMapFindValue, *hldDataMapId, hldMainMapKey);
    if (hldMainMapId.type == HLD_PRIMITIVE_UNDEFINED) {
        LogInfo("Skipping because no mod data to load.");
        return;
    }
    assert(hldMainMapId.type == HLD_PRIMITIVE_REAL);

    /* Update each mod's map. */
    size_t numMods = FoxArrayMSize(FoxMap, &modMaps);
    size_t numModsWithData = 0;
    for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
        const char *modName = ModManGetMod(modIdx)->name;
        HLDPrimitiveMakeStringS(hldModMapKey, modName, strlen(modName));
        HLDPrimitive hldModMapId =
            HLDAPICall(hldfuncs.API_dsMapFindValue, hldMainMapId, hldModMapKey);
        /* Skip if no mod data. */
        if (hldModMapId.type == HLD_PRIMITIVE_UNDEFINED)
            continue;
        numModsWithData++;

        /* Copy entries from HLD map to our map. */
        FoxMap *modMap = FoxArrayMIndex(FoxMap, &modMaps, modIdx);
        HLDOpenHashTable *hldModMap =
            *((HLDOpenHashTable ***)
                  hldvars.maps->elements)[(int32_t)hldModMapId.value.r];
        uint32_t entriesLeft = hldModMap->numItems;
        HLDOpenHashSlot *curSlot = hldModMap->slots;
        while (entriesLeft > 0) {
            HLDOpenHashItem *item = (curSlot++)->first;
            while (item) {
                struct {
                    HLDPrimitive key;
                    HLDPrimitive value;
                } *wrapper = item->value;
                assert(wrapper->key.type == HLD_PRIMITIVE_STRING);
                const char *entryKey =
                    ((HLDPrimitiveString *)wrapper->key.value.p)->chars;
                SaveEntry *entry =
                    FoxMapMInsert(const char *, SaveEntry, modMap, entryKey);
                switch (wrapper->value.type) {
                case HLD_PRIMITIVE_REAL:
                    /* Copy double value. */
                    entry->type = SAVE_DOUBLE;
                    entry->value.d = wrapper->value.value.r;
                    break;

                case HLD_PRIMITIVE_STRING:
                    /* Copy string value. */
                    entry->type = SAVE_STRING;
                    HLDPrimitiveString *str = wrapper->value.value.p;
                    entry->value.s = memcpy(malloc(str->length + 1), str->chars,
                                            str->length + 1);
                    break;

                default:
                    /* Illegal value. */
                    LogErr("Encountered illegal HLDPrimitive type %i while "
                           "loading key \"%s\" of \"%s\" mod's data!",
                           wrapper->value.type, entryKey, modName);
                    abort();
                }
                if (--entriesLeft == 0)
                    break;

                item = item->next;
            }
        }
    }

    LogInfo("Done. Loaded data for %zu of %zu mod(s).", numModsWithData,
            numMods);
    return;
}

void SaveManSaveData(HLDPrimitive *hldDataMapId) {
    LogInfo("Saving mod data...");

    /* Determine whether any mod data to save. */
    bool isModData = false;
    size_t numMods = FoxArrayMSize(FoxMap, &modMaps);
    for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
        FoxMap *modMap = FoxArrayMIndex(FoxMap, &modMaps, modIdx);
        if ((isModData = !FoxMapMEmpty(const char *, SaveEntry, modMap)))
            break;
    }
    if (!isModData) {
        LogInfo("Skipping because no mod data to save.");
        return;
    }

    /* Create main HLD map for holding HLD sub-maps for each mod's data. */
    HLDPrimitive hldMainMapId = HLDAPICall(hldfuncs.API_dsMapCreate);
    HLDPrimitiveMakeStringH(hldMainMapKey,
                            memcpy(malloc(sizeof(HLD_MAIN_MAP_KEY)),
                                   HLD_MAIN_MAP_KEY, sizeof(HLD_MAIN_MAP_KEY)),
                            sizeof(HLD_MAIN_MAP_KEY) - 1);
    HLDAPICall(hldfuncs.API_dsMapAddMap, *hldDataMapId, hldMainMapKey,
               hldMainMapId);

    /* Create HLD sub-maps for each mod's data. */
    size_t numModsWithData = 0;
    for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
        FoxMap *modMap = FoxArrayMIndex(FoxMap, &modMaps, modIdx);
        /* Skip if no mod data. */
        if (FoxMapMEmpty(const char *, SaveEntry, modMap))
            continue;
        numModsWithData++;

        /* Create HLD sub-map. */
        HLDPrimitive hldModMapId = HLDAPICall(hldfuncs.API_dsMapCreate);
        const char *modName = ModManGetMod(modIdx)->name;
        size_t modNameLen = strlen(modName);
        HLDPrimitiveMakeStringH(
            hldModMapKey,
            memcpy(malloc(modNameLen + 1), modName, modNameLen + 1),
            modNameLen);
        HLDAPICall(hldfuncs.API_dsMapAddMap, hldMainMapId, hldModMapKey,
                   hldModMapId);

        /* Record values to HLD sub-map. */
        SaveEntryRecordContext ctx = {.hldModMapId = hldModMapId,
                                      .modName = modName};
        FoxMapMForEachPair(const char *, SaveEntry, modMap,
                           SaveEntryRecordCallback, &ctx);
    }

    LogInfo("Done. Saved data for %zu of %zu mod(s).", numModsWithData,
            numMods);
    return;
}

void SaveManConstructor(void) {
    LogInfo("Initializing save module...");

    size_t numMods = ModManGetNumMods();
    FoxArrayMInit(FoxMap, &modMaps);
    for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
        FoxStringMapMInit(SaveEntry, FoxArrayMPush(FoxMap, &modMaps));
    }

    LogInfo("Done initializing save module.");
    return;
}

void SaveManDestructor(void) {
    LogInfo("Deinitializing save module...");

    size_t numMods = FoxArrayMSize(FoxMap, &modMaps);
    for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
        FoxMap *map = FoxArrayMIndex(FoxMap, &modMaps, modIdx);
        FoxMapMForEachElement(const char *, SaveEntry, map,
                              SaveEntryDeinitCallback, NULL);
        FoxMapMDeinit(const char *, SaveEntry, map);
    }
    FoxArrayMDeinit(FoxMap, &modMaps);

    LogInfo("Done deinitializing save module.");
    return;
}

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT int32_t AERSaveGetCurrentSlot(void) {
#define errRet -1
    EnsureStage(STAGE_ACTION);

    Ok(SaveManGetCurrentSlot());
#undef errRet
}

AER_EXPORT size_t AERSaveGetKeys(size_t bufSize, const char **keyBuf) {
#define errRet 0
    EnsureStage(STAGE_ACTION);
    EnsureArgBuf(keyBuf, bufSize);

    /* Get mod map. */
    assert(ModManHasContext());
    FoxMap *modMap = FoxArrayMIndex(FoxMap, &modMaps, ModManPeekContext());

    /* Write keys to buffer. */
    size_t numKeys = FoxMapMSize(const char *, SaveEntry, modMap);
    SaveEntryGetKeysContext ctx = {.keyBuf = keyBuf,
                                   .numToWrite = FoxMin(numKeys, bufSize)};
    FoxMapMForEachKey(const char *, SaveEntry, modMap, SaveEntryGetKeysCallback,
                      &ctx);

    Ok(numKeys);
#undef errRet
}

AER_EXPORT void AERSaveDestroy(const char *key) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(key);

    /* Get entry. */
    assert(ModManHasContext());
    FoxMap *modMap = FoxArrayMIndex(FoxMap, &modMaps, ModManPeekContext());
    SaveEntry *entry = FoxMapMIndex(const char *, SaveEntry, modMap, key);
    EnsureLookup(entry);

    /* Destroy entry. */
    SaveEntryDeinit(entry);
    FoxMapMRemove(const char *, SaveEntry, modMap, key);

    Ok();
#undef errRet
}

AER_EXPORT double AERSaveGetDouble(const char *key) {
#define errRet 0.0
    EnsureStage(STAGE_ACTION);
    EnsureArg(key);

    /* Get entry. */
    assert(ModManHasContext());
    FoxMap *modMap = FoxArrayMIndex(FoxMap, &modMaps, ModManPeekContext());
    SaveEntry *entry = FoxMapMIndex(const char *, SaveEntry, modMap, key);
    EnsureLookup(entry);
    EnsureType(entry, SAVE_DOUBLE);

    /* Get entry value. */
    Ok(entry->value.d);
#undef errRet
}

AER_EXPORT void AERSaveSetDouble(const char *key, double value) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(key);
    Ensure(isfinite(value), AER_BAD_VAL);

    /* Get entry. */
    assert(ModManHasContext());
    FoxMap *modMap = FoxArrayMIndex(FoxMap, &modMaps, ModManPeekContext());
    SaveEntry *entry = FoxMapMIndex(const char *, SaveEntry, modMap, key);
    if (entry)
        /* Reset entry. */
        SaveEntryDeinit(entry);
    else
        /* Create entry. */
        entry = FoxMapMInsert(const char *, SaveEntry, modMap, key);

    /* Set entry value. */
    entry->type = SAVE_DOUBLE;
    entry->value.d = value;

    Ok();
#undef errRet
}

AER_EXPORT const char *AERSaveGetString(const char *key) {
#define errRet NULL
    EnsureStage(STAGE_ACTION);
    EnsureArg(key);

    /* Get entry. */
    assert(ModManHasContext());
    FoxMap *modMap = FoxArrayMIndex(FoxMap, &modMaps, ModManPeekContext());
    SaveEntry *entry = FoxMapMIndex(const char *, SaveEntry, modMap, key);
    EnsureLookup(entry);
    EnsureType(entry, SAVE_STRING);

    /* Get entry value. */
    Ok(entry->value.s);
#undef errRet
}

AER_EXPORT void AERSaveSetString(const char *key, const char *value) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureArg(key);
    EnsureArg(value);

    /* Get entry. */
    assert(ModManHasContext());
    FoxMap *modMap = FoxArrayMIndex(FoxMap, &modMaps, ModManPeekContext());
    SaveEntry *entry = FoxMapMIndex(const char *, SaveEntry, modMap, key);
    if (entry)
        /* Reset entry. */
        SaveEntryDeinit(entry);
    else
        /* Create entry. */
        entry = FoxMapMInsert(const char *, SaveEntry, modMap, key);

    /* Set entry value. */
    entry->type = SAVE_STRING;
    size_t valSize = strlen(value) + 1;
    entry->value.s = memcpy(malloc(valSize), value, valSize);

    Ok();
#undef errRet
}