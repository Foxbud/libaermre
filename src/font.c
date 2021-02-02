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
#include "aer/font.h"
#include "internal/core.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/hld.h"

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT int32_t AERFontGetCurrent(void) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, AER_FONT_NULL);

    return *hldvars.fontIndexCurrent;
}

AER_EXPORT void AERFontSetCurrent(int32_t fontIdx) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!HLDFontLookup(fontIdx), AER_FAILED_LOOKUP);

    hldfuncs.actionDrawSetFont(fontIdx);

    return;
}

AER_EXPORT int32_t AERFontRegister(const char *filename, size_t size, bool bold,
                                   bool italic, int32_t first, int32_t last) {
    ErrIf(!filename, AER_NULL_ARG, AER_FONT_NULL);
    LogInfo("Registering font \"%s\" for mod \"%s\"...", filename,
            ModManGetMod(ModManPeekContext())->name);
    ErrIf(stage != STAGE_FONT_REG, AER_SEQ_BREAK, AER_FONT_NULL);

    int32_t fontIdx = hldfuncs.actionFontAdd(CoreGetAbsAssetPath(filename),
                                             size, bold, italic, first, last);
    ErrIf(!HLDFontLookup(fontIdx), AER_BAD_FILE, AER_FONT_NULL);

    LogInfo("Successfully registered font to index %i.", fontIdx);

    return fontIdx;
}

AER_EXPORT size_t AERFontGetNumRegistered(void) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, 0);

    return hldvars.fontTable->size;
}

AER_EXPORT const char *AERFontGetName(int32_t fontIdx) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

    HLDFont *font = HLDFontLookup(fontIdx);
    ErrIf(!font, AER_FAILED_LOOKUP, NULL);

    return font->fontname;
}

AER_EXPORT size_t AERFontGetSize(int32_t fontIdx) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, 0);

    HLDFont *font = HLDFontLookup(fontIdx);
    ErrIf(!font, AER_FAILED_LOOKUP, 0);

    return font->size;
}

AER_EXPORT bool AERFontGetBold(int32_t fontIdx) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, false);

    HLDFont *font = HLDFontLookup(fontIdx);
    ErrIf(!font, AER_FAILED_LOOKUP, false);

    return font->bold;
}

AER_EXPORT bool AERFontGetItalic(int32_t fontIdx) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, false);

    HLDFont *font = HLDFontLookup(fontIdx);
    ErrIf(!font, AER_FAILED_LOOKUP, false);

    return font->italic;
}

AER_EXPORT int32_t AERFontGetFirst(int32_t fontIdx) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, -1);

    HLDFont *font = HLDFontLookup(fontIdx);
    ErrIf(!font, AER_FAILED_LOOKUP, -1);

    return font->first;
}

AER_EXPORT int32_t AERFontGetLast(int32_t fontIdx) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, -1);

    HLDFont *font = HLDFontLookup(fontIdx);
    ErrIf(!font, AER_FAILED_LOOKUP, -1);

    return font->last;
}