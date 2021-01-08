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
#ifndef INTERNAL_OBJTREE_H
#define INTERNAL_OBJTREE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "foxutils/map.h"

/* ----- INTERNAL TYPES ----- */

typedef struct ObjTree {
  FoxMap table;
} ObjTree;

/* ----- INTERNAL FUNCTIONS ----- */

ObjTree *ObjTreeNew(void);

void ObjTreeFree(ObjTree *tree);

void ObjTreeInsert(ObjTree *tree, int32_t objIdx, int32_t childIdx);

void ObjTreeForEach(ObjTree *tree, int32_t rootObjIdx, size_t maxDepth,
                    bool (*callback)(int32_t objIdx, void *ctx), void *ctx);

#endif /* INTERNAL_OBJTREE_H */
