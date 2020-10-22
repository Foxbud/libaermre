#include "aer/room.h"
#include "internal/err.h"
#include "internal/hld.h"
#include "internal/mre.h"



/* ----- PUBLIC FUNCTIONS ----- */

int32_t AERRoomGetCurrent(void) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK, -1);

	return *hldvars.roomIndexCurrent;
}

void AERRoomGoto(int32_t roomIdx) {
	ErrIf(mre.stage != STAGE_ACTION, AER_SEQ_BREAK);
	ErrIf(!HLDRoomLookup(roomIdx), AER_FAILED_LOOKUP);

	hldfuncs.actionRoomGoto(roomIdx, 0);

	return;
}
