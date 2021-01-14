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
#include "aer/room.h"
#include "internal/core.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/hld.h"
#include "internal/room.h"

/* ----- INTERNAL GLOBALS ----- */

int32_t roomIndexPrevious = AER_ROOM__INIT;

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT int32_t AERRoomGetCurrent(void) {
  ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, AER_ROOM_NULL);

  return *hldvars.roomIndexCurrent;
}

AER_EXPORT void AERRoomGoto(int32_t roomIdx) {
  ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
  ErrIf(!HLDRoomLookup(roomIdx), AER_FAILED_LOOKUP);

  hldfuncs.actionRoomGoto(roomIdx, 0);

  return;
}
