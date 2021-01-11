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
#ifndef INTERNAL_MRE_H
#define INTERNAL_MRE_H

#include <stdint.h>

#include "foxutils/map.h"

/* ----- INTERNAL TYPES ----- */

/* This struct represents the current state of the mod runtime environment. */
typedef struct AERMRE {
  /*
   * Index of active room during previous game step. Solely for detecting
   * room changes.
   */
  int32_t roomIndexPrevious;
  /* Hash table mapping instance local names to indicies. */
  FoxMap *instLocals;
  /* Current stage of the MRE. */
  enum {
    STAGE_INIT,
    STAGE_SPRITE_REG,
    STAGE_OBJECT_REG,
    STAGE_LISTENER_REG,
    STAGE_ACTION
  } stage;
} AERMRE;

/* ----- INTERNAL GLOBALS ----- */

extern AERMRE mre;

/* ----- INTERNAL FUNCTIONS ----- */

const char *MREGetAbsAssetPath(const char *relAssetPath);

#endif /* INTERNAL_MRE_H */