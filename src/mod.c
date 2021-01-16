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
#include <dlfcn.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "foxutils/arraymacs.h"

#include "aer/err.h"
#include "aer/mod.h"
#include "internal/log.h"
#include "internal/mod.h"
#include "internal/option.h"

/* ----- PRIVATE MACROS ----- */

#define FormatLibname(name, bufSize, buf)                                      \
  snprintf((buf), (bufSize), MOD_LIBNAME_FMT, (name))

/* ----- PRIVATE CONSTANTS ----- */

static const char *MOD_LIBNAME_FMT = "lib%s.so";

static const char *DEF_MOD_NAMES[] = {"define_mod", "definemod", "defineMod",
                                      "DefineMod"};

/* ----- INTERNAL CONSTANTS ----- */

const int32_t MOD_NULL = -1;

/* ----- PRIVATE GLOBALS ----- */

static FoxArray mods;

static FoxArray context;

static FoxArray gameStepListeners;

static FoxArray gamePauseListeners;

static FoxArray roomChangeListeners;

/* ----- PRIVATE FUNCTIONS ----- */

static void ModInit(Mod *mod, int32_t idx, const char *name) {
  LogInfo("Loading mod \"%s\"...", name);

  /* Set index. */
  mod->idx = idx;

  /* Set name. */
  mod->name = name;

  /* Load library. */
  char libname[128];
  FormatLibname(name, 128, libname);
  void *libHandle = dlopen(libname, RTLD_NOW);
  if (!(mod->libHandle = libHandle)) {
    LogErr("While loading mod \"%s\", could not load corresponding library "
           "\"%s\". Make sure its directory is in the \"LD_LIBRARY_PATH\" "
           "environment variable.",
           name, libname);
    abort();
  }

  /* Load mod definition function. */
  AERModDef (*defMod)(void);
  size_t numDefModNames = sizeof(DEF_MOD_NAMES) / sizeof(const char *);
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
      const char *name = DEF_MOD_NAMES[nameIdx];
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
    LogErr("While loading mod \"%s,\" could not find mod definition function "
           "with one of the following names:%s.",
           name, defModNamesBuf);
    abort();
  }
  AERModDef def = defMod();

  /* Record registration callbacks. */
  mod->registerSprites = def.registerSprites;
  mod->registerObjects = def.registerObjects;
  mod->registerObjectListeners = def.registerObjectListeners;

  /* Record pseudoevent listeners. */
  if (def.gameStepListener) {
    ModListener *listener = FoxArrayMPush(ModListener, &gameStepListeners);
    listener->modIdx = idx;
    listener->func = def.gameStepListener;
  }
  if (def.gamePauseListener) {
    ModListener *listener = FoxArrayMPush(ModListener, &gamePauseListeners);
    listener->modIdx = idx;
    listener->func = (void (*)(void))def.gamePauseListener;
  }
  if (def.roomChangeListener) {
    ModListener *listener = FoxArrayMPush(ModListener, &roomChangeListeners);
    listener->modIdx = idx;
    listener->func = (void (*)(void))def.roomChangeListener;
  }

  /* Record mod library management callbacks. */
  mod->constructor = def.constructor;
  mod->destructor = def.destructor;

  LogInfo("Successfully loaded mod \"%s\".", name);
  return;
}

static void ModDeinit(Mod *mod) {
  LogInfo("Unloading mod \"%s\"...", mod->name);

  dlclose(mod->libHandle);

  const char *name = mod->name;

  mod->libHandle = NULL;
  mod->name = NULL;
  mod->constructor = NULL;
  mod->destructor = NULL;
  mod->registerSprites = NULL;
  mod->registerObjects = NULL;
  mod->registerObjectListeners = NULL;

  LogInfo("Successfully unloaded mod \"%s\".", name);
  return;
}

/* ----- INTERNAL FUNCTIONS ----- */

size_t ModManGetNumMods(void) { return FoxArrayMSize(Mod *, &mods); }

Mod *ModManGetMod(int32_t modIdx) {
  assert(modIdx >= 0 && (uint32_t)modIdx < FoxArrayMSize(Mod, &mods));

  return FoxArrayMIndex(Mod, &mods, modIdx);
}

void ModManExecuteGameStepListeners(void) {
  size_t numListeners = FoxArrayMSize(ModListener, &gameStepListeners);
  for (uint32_t idx = 0; idx < numListeners; idx++) {
    ModListener *listener =
        FoxArrayMIndex(ModListener, &gameStepListeners, idx);
    ModManPushContext(listener->modIdx);
    listener->func();
    ModManPopContext();
  }

  return;
}

void ModManExecuteGamePauseListeners(bool paused) {
  size_t numListeners = FoxArrayMSize(ModListener, &gamePauseListeners);
  for (uint32_t idx = 0; idx < numListeners; idx++) {
    ModListener *listener =
        FoxArrayMIndex(ModListener, &gamePauseListeners, idx);
    ModManPushContext(listener->modIdx);
    ((void (*)(bool))listener->func)(paused);
    ModManPopContext();
  }

  return;
}

void ModManExecuteRoomChangeListeners(int32_t newRoomIdx, int32_t prevRoomIdx) {
  size_t numListeners = FoxArrayMSize(ModListener, &roomChangeListeners);
  for (uint32_t idx = 0; idx < numListeners; idx++) {
    ModListener *listener =
        FoxArrayMIndex(ModListener, &roomChangeListeners, idx);
    ModManPushContext(listener->modIdx);
    ((void (*)(int32_t, int32_t))listener->func)(newRoomIdx, prevRoomIdx);
    ModManPopContext();
  }

  return;
}

bool ModManHasContext(void) { return !FoxArrayMEmpty(int32_t, &context); }

void ModManPushContext(int32_t modIdx) {
  assert(modIdx >= 0 && (uint32_t)modIdx < FoxArrayMSize(Mod, &mods));

  *FoxArrayMPush(int32_t, &context) = modIdx;

  return;
}

int32_t ModManPeekContext(void) {
  assert(!FoxArrayMEmpty(int32_t, &context));

  return *FoxArrayMPeek(int32_t, &context);
}

int32_t ModManPopContext(void) {
  assert(!FoxArrayMEmpty(int32_t, &context));

  return FoxArrayMPop(int32_t, &context);
}

void ModManConstructor(void) {
  LogInfo("Loading mods...");

  /* Initialize mod manager. */
  FoxArrayMInit(Mod, &mods);
  FoxArrayMInit(int32_t, &context);
  FoxArrayMInit(ModListener, &gameStepListeners);
  FoxArrayMInit(ModListener, &gamePauseListeners);
  FoxArrayMInit(ModListener, &roomChangeListeners);

  /* Load mod libraries. */
  for (uint32_t idx = 0; idx < opts.numModNames; idx++) {
    Mod *mod = FoxArrayMPush(Mod, &mods);
    ModInit(mod, (int32_t)idx, opts.modNames[idx]);
    if (mod->constructor) {
      ModManPushContext(idx);
      mod->constructor();
      ModManPopContext();
    }
  }

  LogInfo("Done. Loaded %zu mod(s).", opts.numModNames);
  return;
}

void ModManDestructor(void) {
  /* Unload mods. */
  LogInfo("Unloading mods...");
  size_t numMods = FoxArrayMSize(Mod, &mods);
  for (uint32_t idx = 0; idx < numMods; idx++) {
    Mod *mod = FoxArrayMPeek(Mod, &mods);
    if (mod->destructor) {
      ModManPushContext(idx);
      mod->destructor();
      ModManPopContext();
    }
    ModDeinit(mod);
    FoxArrayMPop(Mod, &mods);
  }
  LogInfo("Done. Unloaded %zu mod(s).", numMods);

  /* Deinit modman. */
  FoxArrayMDeinit(Mod, &mods);
  FoxArrayMDeinit(int32_t, &context);
  FoxArrayMDeinit(ModListener, &gameStepListeners);
  FoxArrayMDeinit(ModListener, &gamePauseListeners);
  FoxArrayMDeinit(ModListener, &roomChangeListeners);

  return;
}
