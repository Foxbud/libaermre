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
#ifndef INTERNAL_CORE_H
#define INTERNAL_CORE_H

/* ----- INTERNAL TYPES ----- */

typedef enum CoreStage {
    STAGE_INIT,
    STAGE_SPRITE_REG,
    STAGE_FONT_REG,
    STAGE_OBJECT_REG,
    STAGE_LISTENER_REG,
    STAGE_ACTION,
    STAGE_DRAW
} CoreStage;

/* ----- INTERNAL GLOBALS ----- */

extern CoreStage stage;

/* ----- INTERNAL FUNCTIONS ----- */

const char* CoreGetAbsAssetPath(const char* modName, const char* relAssetPath);

#endif /* INTERNAL_CORE_H */
