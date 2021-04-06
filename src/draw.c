#include "aer/draw.h"
#include "internal/core.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/hld.h"

/* ----- PRIVATE MACROS ----- */

#define WrapString(str)                                                \
    ({                                                                 \
        const char* WrapString_str = (str);                            \
        uint32_t WrapString_idx;                                       \
        for (WrapString_idx = 0; WrapString_idx < sizeof(textBuf) - 1; \
             WrapString_idx++) {                                       \
            char WrapString_curChar = WrapString_str[WrapString_idx];  \
            if (WrapString_curChar == '\0')                            \
                break;                                                 \
            textBuf[WrapString_idx] = WrapString_curChar;              \
        }                                                              \
        textBuf[WrapString_idx] = '\0';                                \
        textBuf;                                                       \
    })

/* ----- PRIVATE GLOBALS ----- */

static char textBuf[8 * 1024];

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT float AERDrawGetCurrentAlpha(void) {
#define errRet -1.0f
    EnsureStage(STAGE_ACTION);

    Ok(hldfuncs.actionDrawGetAlpha());
#undef errRet
}

AER_EXPORT void AERDrawSetCurrentAlpha(float alpha) {
#define errRet
    EnsureStage(STAGE_ACTION);
    EnsureProba(alpha);

    hldfuncs.actionDrawSetAlpha(alpha);

    Ok();
#undef errRet
}

void AERDrawLine(float x1, float y1, float x2, float y2, uint32_t color) {
#define errRet
    EnsureStageStrict(STAGE_DRAW);

    hldfuncs.actionDrawLine(x1, y1, x2, y2, 1.0f, color, color);

    Ok();
#undef errRet
}

void AERDrawLineAdv(float x1,
                    float y1,
                    float x2,
                    float y2,
                    float width,
                    uint32_t color1,
                    uint32_t color2) {
#define errRet
    EnsureStageStrict(STAGE_DRAW);

    hldfuncs.actionDrawLine(x1, y1, x2, y2, width, color1, color2);

    Ok();
#undef errRet
}

AER_EXPORT void AERDrawEllipse(float left,
                               float top,
                               float right,
                               float bottom,
                               uint32_t color,
                               bool outline) {
#define errRet
    EnsureStageStrict(STAGE_DRAW);

    hldfuncs.actionDrawEllipse(left, top, right, bottom, color, color, outline);

    Ok();
}

AER_EXPORT void AERDrawEllipseAdv(float left,
                                  float top,
                                  float right,
                                  float bottom,
                                  uint32_t colorCenter,
                                  uint32_t colorEdge,
                                  bool outline) {
#define errRet
    EnsureStageStrict(STAGE_DRAW);

    hldfuncs.actionDrawEllipse(left, top, right, bottom, colorCenter, colorEdge,
                               outline);

    Ok();
}

AER_EXPORT void AERDrawTriangle(float x1,
                                float y1,
                                float x2,
                                float y2,
                                float x3,
                                float y3,
                                uint32_t color,
                                bool outline) {
#define errRet
    EnsureStageStrict(STAGE_DRAW);

    hldfuncs.actionDrawTriangle(x1, y1, x2, y2, x3, y3, color, color, color,
                                outline);

    Ok();
#undef errRet
}

AER_EXPORT void AERDrawTriangleAdv(float x1,
                                   float y1,
                                   float x2,
                                   float y2,
                                   float x3,
                                   float y3,
                                   uint32_t color1,
                                   uint32_t color2,
                                   uint32_t color3,
                                   bool outline) {
#define errRet
    EnsureStageStrict(STAGE_DRAW);

    hldfuncs.actionDrawTriangle(x1, y1, x2, y2, x3, y3, color1, color2, color3,
                                outline);

    Ok();
#undef errRet
}

AER_EXPORT void AERDrawRectangle(float left,
                                 float top,
                                 float right,
                                 float bottom,
                                 uint32_t color,
                                 bool outline) {
#define errRet
    EnsureStageStrict(STAGE_DRAW);

    hldfuncs.actionDrawRectangle(left, top, right, bottom, color, color, color,
                                 color, outline);

    Ok();
#undef errRet
}

AER_EXPORT void AERDrawRectangleAdv(float left,
                                    float top,
                                    float right,
                                    float bottom,
                                    uint32_t colorNW,
                                    uint32_t colorNE,
                                    uint32_t colorSE,
                                    uint32_t colorSW,
                                    bool outline) {
#define errRet
    EnsureStageStrict(STAGE_DRAW);

    hldfuncs.actionDrawRectangle(left, top, right, bottom, colorNW, colorNE,
                                 colorSE, colorSW, outline);

    Ok();
#undef errRet
}

AER_EXPORT void AERDrawText(const char* text,
                            float x,
                            float y,
                            uint32_t width,
                            float scale,
                            uint32_t color) {
#define errRet
    EnsureStageStrict(STAGE_DRAW);
    EnsureArg(text);

    hldfuncs.actionDrawText(x, y, WrapString(text), -1, width, scale, scale,
                            0.0f, color, color, color, color, 1.0f);

    Ok();
#undef errRet
}

AER_EXPORT void AERDrawTextAdv(const char* text,
                               float x,
                               float y,
                               int32_t height,
                               uint32_t width,
                               float scaleX,
                               float scaleY,
                               float angle,
                               uint32_t colorNW,
                               uint32_t colorNE,
                               uint32_t colorSE,
                               uint32_t colorSW,
                               float alpha) {
#define errRet
    EnsureStageStrict(STAGE_DRAW);
    EnsureArg(text);
    EnsureProba(alpha);

    hldfuncs.actionDrawText(x, y, WrapString(text), height, width, scaleX,
                            scaleY, angle, colorNW, colorNE, colorSE, colorSW,
                            alpha);

    Ok();
#undef errRet
}