#include "aer/draw.h"
#include "internal/core.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/hld.h"

/* ----- PRIVATE MACROS ----- */

#define WrapString(str)                                                        \
  ({                                                                           \
    const char *WrapString_str = (str);                                        \
    uint32_t WrapString_idx;                                                   \
    for (WrapString_idx = 0; WrapString_idx < sizeof(textBuf) - 1;             \
         WrapString_idx++) {                                                   \
      char curChar = WrapString_str[WrapString_idx];                           \
      if (curChar == '\0')                                                     \
        break;                                                                 \
      textBuf[WrapString_idx] = curChar;                                       \
    }                                                                          \
    textBuf[++WrapString_idx] = '\0';                                          \
    textBuf;                                                                   \
  })

/* ----- PRIVATE GLOBALS ----- */

static char textBuf[1024];

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT void AERDrawRectangle(float left, float top, float right,
                                 float bottom, uint32_t color, bool outline) {
  ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);

  hldfuncs.actionDrawRectangle(left, top, right, bottom, color, color, color,
                               color, outline);

  return;
}

AER_EXPORT void AERDrawRectangleAdv(float left, float top, float right,
                                    float bottom, uint32_t colorNW,
                                    uint32_t colorNE, uint32_t colorSE,
                                    uint32_t colorSW, bool outline) {
  ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);

  hldfuncs.actionDrawRectangle(left, top, right, bottom, colorNW, colorNE,
                               colorSE, colorSW, outline);

  return;
}

AER_EXPORT void AERDrawText(const char *text, float x, float y, uint32_t width,
                            float scale, uint32_t color) {
  ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);

  hldfuncs.actionDrawText(x, y, WrapString(text), -1, width, scale, scale, 0.0f,
                          color, color, color, color, 1.0f);

  return;
}

AER_EXPORT void AERDrawTextAdv(const char *text, float x, float y,
                               int32_t height, uint32_t width, float scaleX,
                               float scaleY, float angle, uint32_t colorNW,
                               uint32_t colorNE, uint32_t colorSE,
                               uint32_t colorSW, float alpha) {
  ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);

  hldfuncs.actionDrawText(x, y, WrapString(text), height, width, scaleX, scaleY,
                          angle, colorNW, colorNE, colorSE, colorSW, alpha);

  return;
}