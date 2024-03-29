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
#ifndef INTERNAL_ROOM_H
#define INTERNAL_ROOM_H

#include <stdint.h>

/* ----- INTERNAL GLOBALS ----- */

extern int32_t roomIndexAux;

/* ----- INTERNAL FUNCTIONS ----- */

void RoomManBuildNameTable(void);

void RoomManConstructor(void);

void RoomManDestructor(void);

#endif /* INTERNAL_ROOM_H */
