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
#define errRet AER_FONT_NULL
    EnsureStagePast(STAGE_FONT_REG);

    Ok(*hldvars.fontIndexCurrent);
#undef errRet
}

AER_EXPORT void AERFontSetCurrent(int32_t fontIdx) {
#define errRet
    EnsureStagePast(STAGE_FONT_REG);
    EnsureLookup(HLDFontLookup(fontIdx));

    hldfuncs.actionDrawSetFont(fontIdx);

    Ok();
#undef errRet
}

AER_EXPORT int32_t AERFontRegister(const char* filename,
                                   size_t size,
                                   bool bold,
                                   bool italic,
                                   int32_t first,
                                   int32_t last) {
#define errRet AER_FONT_NULL
    EnsureArg(filename);
    Mod* curMod = ModManGetCurrentMod();
    assert(curMod);
    LogInfo("Registering font \"%s\" for mod \"%s\"...", filename,
            curMod->name);
    EnsureStageStrict(STAGE_FONT_REG);

    int32_t fontIdx =
        hldfuncs.actionFontAdd(CoreGetAbsAssetPath(curMod->name, filename),
                               size, bold, italic, first, last);
    Ensure(HLDFontLookup(fontIdx), AER_BAD_FILE);

    LogInfo("Successfully registered font to index %i.", fontIdx);

    Ok(fontIdx);
#undef errRet
}

AER_EXPORT size_t AERFontGetNumRegistered(void) {
#define errRet 0
    EnsureStage(STAGE_FONT_REG);

    Ok(hldvars.fontTable->size);
#undef errRet
}

AER_EXPORT const char* AERFontGetName(int32_t fontIdx) {
#define errRet NULL
    EnsureStage(STAGE_FONT_REG);

    HLDFont* font = HLDFontLookup(fontIdx);
    EnsureLookup(font);

    Ok(font->fontname);
#undef errRet
}

AER_EXPORT size_t AERFontGetSize(int32_t fontIdx) {
#define errRet 0
    EnsureStage(STAGE_FONT_REG);

    HLDFont* font = HLDFontLookup(fontIdx);
    EnsureLookup(font);

    Ok(font->size);
#undef errRet
}

AER_EXPORT bool AERFontGetBold(int32_t fontIdx) {
#define errRet false
    EnsureStage(STAGE_FONT_REG);

    HLDFont* font = HLDFontLookup(fontIdx);
    EnsureLookup(font);

    Ok(font->bold);
#undef errRet
}

AER_EXPORT bool AERFontGetItalic(int32_t fontIdx) {
#define errRet false
    EnsureStage(STAGE_FONT_REG);

    HLDFont* font = HLDFontLookup(fontIdx);
    EnsureLookup(font);

    Ok(font->italic);
#undef errRet
}

AER_EXPORT int32_t AERFontGetFirst(int32_t fontIdx) {
#define errRet -1
    EnsureStage(STAGE_FONT_REG);

    HLDFont* font = HLDFontLookup(fontIdx);
    EnsureLookup(font);

    Ok(font->first);
#undef errRet
}

AER_EXPORT int32_t AERFontGetLast(int32_t fontIdx) {
#define errRet -1
    EnsureStage(STAGE_FONT_REG);

    HLDFont* font = HLDFontLookup(fontIdx);
    EnsureLookup(font);

    Ok(font->last);
#undef errRet
}