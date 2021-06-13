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
#define _GNU_SOURCE /* Required for `dladdr` and `Dl_info`. */

#include <assert.h>
#include <dlfcn.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "foxutils/arraymacs.h"
#include "foxutils/mapmacs.h"

#include "aer/err.h"
#include "aer/mod.h"
#include "internal/log.h"
#include "internal/mod.h"
#include "internal/option.h"

/* ----- PRIVATE MACROS ----- */

#define FormatLibname(name, bufSize, buf) \
    snprintf((buf), (bufSize), MOD_LIBNAME_FMT, (name))

/* ----- PRIVATE CONSTANTS ----- */

static const char* MOD_LIBNAME_FMT = "lib%s.so";

static const char* DEF_MOD_NAMES[] = {"define_mod", "definemod", "defineMod",
                                      "DefineMod"};

/* ----- INTERNAL CONSTANTS ----- */

const int32_t MOD_NULL = -1;

/* ----- PRIVATE GLOBALS ----- */

static Mod* mods = NULL;

static FoxMap modMemMap = {0};

static FoxArray gameStepListeners = {0};

static FoxArray gamePauseListeners = {0};

static FoxArray gameSaveListeners = {0};

static FoxArray gameLoadListeners = {0};

static FoxArray roomChangeListeners = {0};

/* ----- PRIVATE FUNCTIONS ----- */

static void ModInit(Mod* mod, int32_t idx, const char* name) {
    LogInfo("Loading mod \"%s\"...", name);

    /* Set index. */
    mod->idx = idx;

    /* Set name. */
    mod->name = name;

    /* Load library. */
    char libname[128];
    FormatLibname(name, 128, libname);
    void* libHandle = dlopen(libname, RTLD_NOW);
    if (!(mod->libHandle = libHandle)) {
        LogErr(
            "While loading mod \"%s\", could not load corresponding library "
            "\"%s\".\n"
            "If you are using this mod, make sure its directory is in "
            "the \"LD_LIBRARY_PATH\" environment variable.\n"
            "If you are developing this mod, make sure all of the symbols it "
            "references are defined.",
            name, libname);
        abort();
    }

    /* Load mod definition function. */
    void (*defMod)(AERModDef*);
    size_t numDefModNames = sizeof(DEF_MOD_NAMES) / sizeof(const char*);
    for (uint32_t idx = 0; idx < numDefModNames; idx++) {
        defMod = dlsym(libHandle, DEF_MOD_NAMES[idx]);
        if (defMod)
            break;
    }
    if (!defMod) {
        /* TODO Create dedicated string concatenation function. */
        /* Efficiently concatenate all valid def mod function names. */
        char defModNamesBuf[256];
        uint32_t charIdxOut = 0;
        for (uint32_t nameIdx = 0; nameIdx < numDefModNames; nameIdx++) {
            /* Prevent buffer overflow. */
            if (charIdxOut == sizeof(defModNamesBuf) - 1)
                break;
            defModNamesBuf[charIdxOut++] = ' ';
            const char* name = DEF_MOD_NAMES[nameIdx];
            uint32_t charIdxIn = 0;
            char charCur = name[charIdxIn];
            while (charCur != '\0') {
                /* Prevent buffer overflow. */
                if (charIdxOut == sizeof(defModNamesBuf) - 1)
                    break;
                defModNamesBuf[charIdxOut++] = charCur;
                charCur = name[++charIdxIn];
            }
        }
        defModNamesBuf[charIdxOut] = '\0';
        /* Display error. */
        LogErr(
            "While loading mod \"%s\", could not find mod definition function "
            "with one of the following names:%s.",
            name, defModNamesBuf);
        abort();
    }
    AERModDef def = {0};

    /* Record mod memory map. */
    Dl_info memInfo;
    if (dladdr(defMod, &memInfo) == 0 || memInfo.dli_fbase == NULL) {
        LogErr(
            "While loading mod \"%s\", could not determine mod library's base "
            "address.",
            name);
        abort();
    }
    *FoxMapMInsert(void*, int32_t, &modMemMap, memInfo.dli_fbase) = idx;

    /* Call mod definition function. */
    defMod(&def);

    /* Record registration callbacks. */
    mod->registerSprites = def.registerSprites;
    mod->registerFonts = def.registerFonts;
    mod->registerObjects = def.registerObjects;
    mod->registerObjectListeners = def.registerObjectListeners;

    /* Record pseudoevent listeners. */
    if (def.gameStepListener) {
        *FoxArrayMPush(void*, &gameStepListeners) = def.gameStepListener;
    }
    if (def.gamePauseListener) {
        *FoxArrayMPush(void*, &gamePauseListeners) = def.gamePauseListener;
    }
    if (def.gameSaveListener) {
        *FoxArrayMPush(void*, &gameSaveListeners) = def.gameSaveListener;
    }
    if (def.gameLoadListener) {
        *FoxArrayMPush(void*, &gameLoadListeners) = def.gameLoadListener;
    }
    if (def.roomChangeListener) {
        *FoxArrayMPush(void*, &roomChangeListeners) = def.roomChangeListener;
    }

    /* Record mod library management callbacks. */
    mod->constructor = def.constructor;
    mod->destructor = def.destructor;

    LogInfo("Successfully loaded mod \"%s\".", name);
    return;
}

static void ModDeinit(Mod* mod) {
    LogInfo("Unloading mod \"%s\"...", mod->name);

    dlclose(mod->libHandle);

    const char* name = mod->name;

    *mod = (Mod){0};

    LogInfo("Successfully unloaded mod \"%s\".", name);
    return;
}

/* ----- INTERNAL FUNCTIONS ----- */

size_t ModManGetNumMods(void) {
    return opts.numModNames;
}

Mod* ModManGetMod(int32_t modIdx) {
    assert(modIdx >= 0 && (size_t)modIdx < opts.numModNames);

    return mods + modIdx;
}

Mod* ModManGetOwningMod(void* sym) {
    assert(sym);

    Dl_info memInfo;
    dladdr(sym, &memInfo);
    int32_t* modIdx =
        FoxMapMIndex(void*, int32_t, &modMemMap, memInfo.dli_fbase);
    if (modIdx) {
        return mods + *modIdx;
    }

    return NULL;
}

void ModManExecuteGameStepListeners(void) {
    size_t numListeners = FoxArrayMSize(void*, &gameStepListeners);
    for (uint32_t idx = 0; idx < numListeners; idx++) {
        void (*listener)(void) =
            *FoxArrayMIndex(void*, &gameStepListeners, idx);
        listener();
    }

    return;
}

void ModManExecuteGamePauseListeners(bool paused) {
    size_t numListeners = FoxArrayMSize(void*, &gamePauseListeners);
    for (uint32_t idx = 0; idx < numListeners; idx++) {
        void (*listener)(bool) =
            *FoxArrayMIndex(void*, &gamePauseListeners, idx);
        listener(paused);
    }

    return;
}

void ModManExecuteGameSaveListeners(int32_t curSlotIdx) {
    size_t numListeners = FoxArrayMSize(void*, &gameSaveListeners);
    for (uint32_t idx = 0; idx < numListeners; idx++) {
        void (*listener)(int32_t) =
            *FoxArrayMIndex(void*, &gameSaveListeners, idx);
        listener(curSlotIdx);
    }

    return;
}

void ModManExecuteGameLoadListeners(int32_t curSlotIdx) {
    size_t numListeners = FoxArrayMSize(void*, &gameLoadListeners);
    for (uint32_t idx = 0; idx < numListeners; idx++) {
        void (*listener)(int32_t) =
            *FoxArrayMIndex(void*, &gameLoadListeners, idx);
        listener(curSlotIdx);
    }

    return;
}

void ModManExecuteRoomChangeListeners(int32_t newRoomIdx, int32_t prevRoomIdx) {
    size_t numListeners = FoxArrayMSize(void*, &roomChangeListeners);
    for (uint32_t idx = 0; idx < numListeners; idx++) {
        void (*listener)(int32_t, int32_t) =
            *FoxArrayMIndex(void*, &roomChangeListeners, idx);
        listener(newRoomIdx, prevRoomIdx);
    }

    return;
}

void ModManLoadMods(void) {
    LogInfo("Loading mods...");

    /* Allocate mod array. */
    mods = malloc(opts.numModNames * sizeof(Mod));

    /* Load mod libraries. */
    for (uint32_t idx = 0; idx < opts.numModNames; idx++) {
        Mod* mod = mods + idx;
        ModInit(mod, idx, opts.modNames[idx]);
        if (mod->constructor) {
            mod->constructor();
        }
    }

    LogInfo("Done. Loaded %zu mod(s).", opts.numModNames);
}

void ModManUnloadMods(void) {
    LogInfo("Unloading mods...");

    /* Destruct mods in reverse order. */
    for (uint32_t idx = 0; idx < opts.numModNames; idx++) {
        int32_t modIdx = (int32_t)(opts.numModNames - idx - 1);
        Mod* mod = mods + modIdx;
        if (mod->destructor) {
            mod->destructor();
        }
        ModDeinit(mod);
    }

    LogInfo("Done. Unloaded %zu mod(s).", opts.numModNames);
    return;
}

void ModManConstructor(void) {
    LogInfo("Initializing mod manager...");

    FoxMapMInit(void*, int32_t, &modMemMap);
    FoxArrayMInit(void*, &gameStepListeners);
    FoxArrayMInit(void*, &gamePauseListeners);
    FoxArrayMInit(void*, &gameSaveListeners);
    FoxArrayMInit(void*, &gameLoadListeners);
    FoxArrayMInit(void*, &roomChangeListeners);

    LogInfo("Done initializing mod manager.", opts.numModNames);
    return;
}

void ModManDestructor(void) {
    LogInfo("Deinitializing mod manager...");

    free(mods);
    FoxMapMDeinit(void*, int32_t, &modMemMap);
    FoxArrayMDeinit(void*, &gameStepListeners);
    FoxArrayMDeinit(void*, &gamePauseListeners);
    FoxArrayMDeinit(void*, &gameSaveListeners);
    FoxArrayMDeinit(void*, &gameLoadListeners);
    FoxArrayMDeinit(void*, &roomChangeListeners);

    LogInfo("Done deinitializing mod manager.");
    return;
}
