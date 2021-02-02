#include "aer/window.h"
#include "internal/core.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/hld.h"

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT void AERViewGetPositionInRoom(uint32_t viewIdx, float *x, float *y) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!(x || y), AER_NULL_ARG);

    HLDView *view = HLDViewLookup(viewIdx);
    ErrIf(!view, AER_FAILED_LOOKUP);

    HLDVecReal pos = view->posRoom;
    if (x)
        *x = pos.x;
    if (y)
        *y = pos.y;

    return;
}

AER_EXPORT void AERViewGetSizeInRoom(uint32_t viewIdx, float *width,
                                     float *height) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!(width || height), AER_NULL_ARG);

    HLDView *view = HLDViewLookup(viewIdx);
    ErrIf(!view, AER_FAILED_LOOKUP);

    HLDVecReal size = view->sizeRoom;
    if (width)
        *width = size.x;
    if (height)
        *height = size.y;

    return;
}

AER_EXPORT void AERViewGetBorder(uint32_t viewIdx, int32_t *width,
                                 int32_t *height) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!(width || height), AER_NULL_ARG);

    HLDView *view = HLDViewLookup(viewIdx);
    ErrIf(!view, AER_FAILED_LOOKUP);

    HLDVecIntegral border = view->border;
    if (width)
        *width = border.x;
    if (height)
        *height = border.y;

    return;
}