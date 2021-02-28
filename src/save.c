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

#include "btree.h"

#include "foxutils/math.h"

#include "aer/object.h"
#include "aer/room.h"
#include "aer/save.h"
#include "internal/core.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/hld.h"
#include "internal/log.h"
#include "internal/save.h"

/* ----- PRIVATE MACROS ----- */

#define EnsureSaveLoaded()                                                     \
    Ensure(*hldvars.roomIndexCurrent > AER_ROOM_TITLE, AER_SEQ_BREAK)

#define CopyKey(key)                                                           \
    ({                                                                         \
        const char *CopyKey_key = (key);                                       \
        size_t CopyKey_len = FoxMin(strlen(CopyKey_key), sizeof(keyBuf) - 1);  \
        memcpy(keyBuf, CopyKey_key, CopyKey_len);                              \
        keyBuf[CopyKey_len] = '\0';                                            \
        CopyKey_len;                                                           \
    })

#define GetValueMap()                                                          \
    ({                                                                         \
        HLDObject *GetValueMap_dataObj = HLDObjectLookup(AER_OBJECT_DATA);     \
        assert(GetValueMap_dataObj->numInstances == 1);                        \
        HLDInstance *GetValueMap_dataInst =                                    \
            GetValueMap_dataObj->instanceFirst->item;                          \
        double *GetValueMap_valueMapIdx = HLDClosedHashTableLookup(            \
            GetValueMap_dataInst->locals, 0x4b5 /* "miscValues" */);           \
        assert(GetValueMap_valueMapIdx);                                       \
        ((HLDOpenHashTable ***)                                                \
             hldvars.maps->elements)[(uint32_t)*GetValueMap_valueMapIdx];      \
    })

/* ----- PRIVATE TYPES ----- */

typedef struct ValueKey {
    size_t size;
    const char *chars;
} ValueKey;

typedef struct ValueKeyGetByPrefixContext {
    const size_t prefixSize;
    const char *const prefix;
    const size_t bufSize;
    const char **const keyBuf;
    size_t numKeysInBuf;
    size_t numKeysTot;
} ValueKeyGetByPrefixContext;

/* ----- PRIVATE GLOBALS ----- */

static struct btree *valueKeys = NULL;

static char keyBuf[1024];

/* ----- PRIVATE FUNCTIONS ----- */

static void ValueKeyInit(ValueKey *key, const char *str, size_t size) {
    key->size = size;
    key->chars = memcpy(malloc(size), str, size);

    return;
}

static void ValueKeyDeinit(ValueKey *key) {
    free((char *)key->chars);
    *key = (ValueKey){0};

    return;
}

static enum btree_action ValueKeyDeinitCallback(ValueKey *key, void *ctx) {
    (void)ctx;

    ValueKeyDeinit(key);

    return BTREE_DELETE;
}

static int32_t ValueKeyCompareCallback(const ValueKey *keyA,
                                       const ValueKey *keyB, void *ctx) {
    (void)ctx;

    return memcmp(keyA->chars, keyB->chars, FoxMin(keyA->size, keyB->size));
}

static bool ValueKeyGetByPrefixCallback(const ValueKey *key,
                                        ValueKeyGetByPrefixContext *ctx) {
    if (ctx->prefix && (key->size < ctx->prefixSize ||
                        memcmp(key->chars, ctx->prefix, ctx->prefixSize) != 0))
        return false;

    ctx->numKeysTot++;
    if (ctx->numKeysInBuf < ctx->bufSize)
        ctx->keyBuf[ctx->numKeysInBuf++] = key->chars;

    return true;
}

/* ----- INTERNAL FUNCTIONS ----- */

void SaveManRefreshValueKeys(void) {
    LogInfo("Refreshing save value keys...");

    /* Drop old keys. */
    btree_action_ascend(
        valueKeys, NULL,
        (enum btree_action(*)(void *, void *))ValueKeyDeinitCallback, NULL);

    /* Record new keys. */
    HLDOpenHashTable *valueMap = *GetValueMap();
    HLDOpenHashSlot *slots = valueMap->slots;
    size_t numSlots = valueMap->keyMask + 1;
    for (uint32_t slotIdx = 0; slotIdx < numSlots; slotIdx++) {
        HLDOpenHashSlot slot = slots[slotIdx];
        HLDOpenHashItem *item = slot.first;

        while (item) {
            HLDPrimitiveString *keyStr = ((HLDPrimitive *)item->value)->value.p;
            ValueKey valueKey;
            ValueKeyInit(&valueKey, keyStr->chars + 5 /* Skip "Value". */,
                         keyStr->length + 1 - 5);
            btree_set(valueKeys, &valueKey);

            item = item->next;
        }
    }

    LogInfo("Done. Refreshed %zu keys(s).", valueMap->numItems);
    return;
}

void SaveManConstructor(void) {
    LogInfo("Initializing save module...");

    valueKeys = btree_new(
        sizeof(ValueKey), 0,
        (int (*)(const void *, const void *, void *))ValueKeyCompareCallback,
        NULL);

    LogInfo("Done initializing save module.");
    return;
}

void SaveManDestructor(void) {
    LogInfo("Deinitializing save module...");

    btree_action_ascend(
        valueKeys, NULL,
        (enum btree_action(*)(void *, void *))ValueKeyDeinitCallback, NULL);
    btree_free(valueKeys);
    valueKeys = NULL;

    LogInfo("Done deinitializing save module.");
    return;
}

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT size_t AERSaveGetValueKeys(const char *prefix, size_t bufSize,
                                      const char **keyBuf) {
#define errRet 0
    EnsureSaveLoaded();
    EnsureArgBuf(keyBuf, bufSize);

    size_t prefixSize = 0;
    ValueKey pivot = {0};
    if (prefix) {
        prefixSize = strlen(prefix) + 1;
        ValueKeyInit(&pivot, prefix, prefixSize);
    }
    ValueKeyGetByPrefixContext ctx = {.prefixSize = prefixSize,
                                      .prefix = prefix,
                                      .bufSize = bufSize,
                                      .keyBuf = keyBuf,
                                      .numKeysInBuf = 0,
                                      .numKeysTot = 0};
    btree_ascend(valueKeys, (prefix) ? &pivot : NULL,
                 (bool (*)(const void *, void *))ValueKeyGetByPrefixCallback,
                 &ctx);

    return ctx.numKeysTot;
#undef errRet
}

AER_EXPORT double AERSaveReadValue(const char *key) {
#define errRet 0.0
    EnsureSaveLoaded();
    EnsureArg(key);

    HLDPrimitiveMakeString(argKey, keyBuf, CopyKey(key));
    HLDPrimitive result = HLDScriptCall(hldfuncs.gmlScriptValueCheck, &argKey);
    EnsureLookup(result.type == HLD_PRIMITIVE_REAL);

    return result.value.r;
#undef errRet
}

AER_EXPORT void AERSaveWriteValue(const char *key, double value) {
#define errRet
    EnsureSaveLoaded();
    EnsureArg(key);

    HLDOpenHashTable *valueMap = *GetValueMap();
    size_t initNumKeys = valueMap->numItems;

    /* Write key to buffer. */
    size_t keyLen = CopyKey(key);
    HLDPrimitiveMakeString(argKey, keyBuf, keyLen);
    HLDPrimitiveMakeReal(argValue, value);
    HLDScriptCall(hldfuncs.gmlScriptValueRecord, &argKey, &argValue);

    /* Add key to Btree if new. */
    if (valueMap->numItems > initNumKeys) {
        ValueKey valueKey;
        ValueKeyInit(&valueKey, key, keyLen + 1);
        btree_set(valueKeys, &valueKey);
    }

    return;
#undef errRet
}

AER_EXPORT void AERSaveEraseValue(const char *key) {
#define errRet
    EnsureSaveLoaded();
    EnsureArg(key);

    /* Remove key from Btree. */
    ValueKey btreeKey = {.chars = key, .size = strlen(key) + 1};
    EnsureLookup(btree_get(valueKeys, &btreeKey));
    ValueKeyDeinit(btree_delete(valueKeys, &btreeKey));

    /* Remove key from buffer. */
    HLDPrimitiveMakeString(mapKey, keyBuf, CopyKey(key));
    hldfuncs.actionMapDelete(GetValueMap(), &mapKey);

    return;
#undef errRet
}