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
#ifndef INTERNAL_MODMAN_H
#define INTERNAL_MODMAN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "foxutils/array.h"

#include "internal/hld.h"

/* ----- INTERNAL TYPES ----- */

typedef struct Mod {
  void *libHandle;
  int32_t idx;
  const char *name;
  void (*constructor)(void);
  void (*destructor)(void);
  void (*registerSprites)(void);
  void (*registerObjects)(void);
  void (*registerObjectListeners)(void);
} Mod;

typedef struct ModListener {
  void (*func)(void);
  int32_t modIdx;
} ModListener;

typedef struct ModMan {
  FoxArray mods;
  FoxArray context;
  FoxArray roomStepListeners;
  FoxArray roomChangeListeners;
} ModMan;

/* ----- INTERNAL CONSTANTS ----- */

extern const int32_t MOD_NULL;

/* ----- INTERNAL GLOBALS ----- */

extern ModMan modman;

/* ----- INTERNAL FUNCTIONS ----- */

size_t ModManGetNumMods(void);

Mod *ModManGetMod(int32_t modIdx);

bool ModManHasContext(void);

void ModManPushContext(int32_t modIdx);

int32_t ModManPeekContext(void);

int32_t ModManPopContext(void);

void ModManConstructor(void);

void ModManDestructor(void);

#endif /* INTERNAL_MODMAN_H */
