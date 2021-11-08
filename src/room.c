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

#include "foxutils/stringmapmacs.h"

#include "aer/room.h"
#include "internal/core.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/hld.h"
#include "internal/room.h"

/* ----- PRIVATE GLOBALS ----- */

static FoxMap roomNames = {0};

/* ----- INTERNAL GLOBALS ----- */

int32_t roomIndexCurrent = AER_ROOM_AUTOSAVEMESSAGE;

int32_t roomIndexOther = AER_ROOM_NULL;

/* ----- INTERNAL FUNCTIONS ----- */

void RoomManBuildNameTable(void) {
    size_t numRooms = hldvars.roomTable->size;
    for (uint32_t roomIdx = 0; roomIdx < numRooms; roomIdx++) {
        HLDRoom* room = HLDRoomLookup(roomIdx);
        assert(room);
        *FoxMapMInsert(const char*, int32_t, &roomNames, room->name) = roomIdx;
    }

    return;
}

void RoomManConstructor(void) {
    LogInfo("Initializing room module...");

    FoxStringMapMInit(int32_t, &roomNames);

    LogInfo("Done initializing room module.");
    return;
}

void RoomManDestructor(void) {
    LogInfo("Deinitializing room module...");

    /* Deinitialize name table. */
    FoxMapMDeinit(const char*, int32_t, &roomNames);
    roomNames = (FoxMap){0};

    LogInfo("Done deinitializing room module.");
    return;
}

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT int32_t AERRoomGetCurrent(void) {
#define errRet AER_ROOM_NULL
    EnsureStage(STAGE_ACTION);

    Ok(roomIndexCurrent);
#undef errRet
}

AER_EXPORT void AERRoomGoto(int32_t roomIdx) {
#define errRet
    EnsureStage(STAGE_ACTION);
    Ensure(roomIndexOther == AER_ROOM_NULL, AER_SEQ_BREAK);
    EnsureLookup(HLDRoomLookup(roomIdx));

    hldfuncs.actionRoomGoto(roomIdx, 0);

    Ok();
#undef errRet
}

AER_EXPORT void AERRoomEnter(int32_t roomIdx, bool fade) {
#define errRet
    EnsureStage(STAGE_ACTION);
    Ensure(roomIndexOther == AER_ROOM_NULL, AER_SEQ_BREAK);
    EnsureLookup(HLDRoomLookup(roomIdx));

    HLDPrimitiveMakeReal(roomIdxArg, (double)roomIdx);
    HLDPrimitiveMakeReal(fadeArg, fade ? 1.0 : 2.0);
    HLDPrimitiveMakeUndefined(undefArg);

    HLDScriptCall(hldfuncs.Script_GoToRoom, &roomIdxArg, &fadeArg, &undefArg,
                  &undefArg);

    Ok();
#undef errRet
}

AER_EXPORT void AERRoomEnterWithPosition(int32_t roomIdx,
                                         bool fade,
                                         float x,
                                         float y) {
#define errRet
    EnsureStage(STAGE_ACTION);
    Ensure(roomIndexOther == AER_ROOM_NULL, AER_SEQ_BREAK);
    EnsureLookup(HLDRoomLookup(roomIdx));

    HLDPrimitiveMakeReal(roomIdxArg, (double)roomIdx);
    HLDPrimitiveMakeReal(fadeArg, fade ? 1.0 : 2.0);
    HLDPrimitiveMakeArrayS(posArg, 2);
    HLDPrimitiveMakeStringS(typeArg, "xy", 2);

    posArgInnerElements[0] =
        (HLDPrimitive){.value.r = (double)x, .type = HLD_PRIMITIVE_REAL};
    posArgInnerElements[1] =
        (HLDPrimitive){.value.r = (double)y, .type = HLD_PRIMITIVE_REAL};

    HLDScriptCall(hldfuncs.Script_GoToRoom, &roomIdxArg, &fadeArg, &posArg,
                  &typeArg);

    Ok();
#undef errRet
}

AER_EXPORT int32_t AERRoomGetByName(const char* name) {
#define errRet AER_ROOM_NULL
    EnsureStage(STAGE_ACTION);
    EnsureArg(name);

    int32_t* roomIdx = FoxMapMIndex(const char*, int32_t, &roomNames, name);
    EnsureLookup(roomIdx);

    Ok(*roomIdx);
#undef errRet
}

AER_EXPORT const char* AERRoomGetName(int32_t roomIdx) {
#define errRet NULL
    EnsureStage(STAGE_ACTION);

    HLDRoom* room = HLDRoomLookup(roomIdx);
    EnsureLookup(room);

    Ok(room->name);
#undef errRet
}