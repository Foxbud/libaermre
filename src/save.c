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

#include "foxutils/arraymacs.h"
#include "foxutils/stringmapmacs.h"

#include "aer/save.h"
#include "internal/log.h"
#include "internal/mod.h"
#include "internal/save.h"

/* ----- PRIVATE MACROS ----- */

#define MOD_MAPS_KEY "mod"

#define SaveEntryDeinit(entry)                                                 \
    do {                                                                       \
        SaveEntry *SaveEntryDeinit_entry = (entry);                            \
        if (SaveEntryDeinit_entry->type == SAVE_STRING)                        \
            free(SaveEntryDeinit_entry->value.s);                              \
        *SaveEntryDeinit_entry = (SaveEntry){0};                               \
    } while (0)

/* ----- PRIVATE TYPES ----- */

typedef enum SaveType { SAVE_NULL, SAVE_DOUBLE, SAVE_STRING } SaveType;

typedef struct SaveEntry {
    SaveType type;
    union {
        double d;
        char *s;
    } value;
} SaveEntry;

/* ----- PRIVATE GLOBALS ----- */

static FoxArray modMaps = {0};

/* ----- PRIVATE FUNCTIONS ----- */

static bool SaveEntryDeinitCallback(SaveEntry *entry, void *ctx) {
    (void)ctx;

    SaveEntryDeinit(entry);

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

void SaveManLoadData(HLDPrimitive *dataMapId) {
    ResetModMaps();

    HLDPrimitiveMakeStringS(argModMapsKey, MOD_MAPS_KEY,
                            sizeof(MOD_MAPS_KEY) - 1);
    HLDPrimitive modMapsId =
        HLDAPICall(hldfuncs.API_dsMapFindValue, *dataMapId, argModMapsKey);
    assert(modMapsId.type == HLD_PRIMITIVE_REAL);

    size_t numMods = FoxArrayMSize(FoxMap, &modMaps);
    for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
        const char *modName = ModManGetMod(modIdx)->name;
        HLDPrimitiveMakeStringS(argModMapKey, modName, strlen(modName));
        HLDPrimitive mapId =
            HLDAPICall(hldfuncs.API_dsMapFindValue, modMapsId, argModMapKey);
        if (mapId.type == HLD_PRIMITIVE_UNDEFINED)
            continue;

        FoxMap *map = FoxArrayMIndex(FoxMap, &modMaps, modIdx);
        HLDOpenHashTable *hldMap =
            *((HLDOpenHashTable ***)
                  hldvars.maps->elements)[(int32_t)mapId.value.r];
        uint32_t entriesLeft = hldMap->numItems;
        HLDOpenHashSlot *curSlot = hldMap->slots;
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
                    FoxMapMInsert(const char *, SaveEntry, map, entryKey);
                switch (wrapper->value.type) {
                case HLD_PRIMITIVE_REAL:
                    entry->type = SAVE_DOUBLE;
                    entry->value.d = wrapper->value.value.r;
                    break;

                case HLD_PRIMITIVE_STRING:
                    entry->type = SAVE_STRING;
                    HLDPrimitiveString *str = wrapper->value.value.p;
                    entry->value.s = memcpy(malloc(str->length + 1), str->chars,
                                            str->length + 1);
                    break;

                default:
                    LogErr("Encountered illegal type %i while loading key "
                           "\"%s\" of \"%s\" mod's savedata!",
                           wrapper->value.type, entryKey, modName);
                    abort();
                }
                if (--entriesLeft == 0)
                    break;

                item = item->next;
            }
        }
    }

    return;
}

void SaveManSaveData(HLDPrimitive *dataMapId);

void SaveManConstructor(void) {
    LogInfo("Initializing save module...");

    size_t numMods = ModManGetNumMods();
    FoxArrayMInitExt(FoxMap, &modMaps, numMods);
    for (uint32_t modIdx = 0; modIdx < numMods; modIdx++) {
        FoxStringMapMInit(SaveEntry, FoxArrayMPush(FoxMap, &modMaps));
    }

    LogInfo("Done initializing save module...");
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

    LogInfo("Done deinitializing save module...");
    return;
}