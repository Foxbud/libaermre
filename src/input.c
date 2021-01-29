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
#include <stddef.h>
#include <string.h>

#include "aer/input.h"
#include "internal/core.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/input.h"

/* ----- PUBLIC CONSTANTS ----- */

AER_EXPORT const char AER_DISPLAY_KEYS[] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,    0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,    0,   0,
    0,   0,   ' ', '!', '"', 0,   '$', '%', '&', 0,   '(',  ')', '*',  '+', 0,
    0,   0,   0,   '0', '1', '2', '3', '4', '5', '6', '7',  '8', '9',  ':', 0,
    '<', 0,   '>', '?', '@', 'a', 'b', 'c', 'd', 'e', 'f',  'g', 'h',  'i', 'j',
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u',  'v', 'w',  'x', 'y',
    'z', 0,   0,   0,   '^', 0,   '_', '0', '1', '2', '3',  '4', '5',  '6', '7',
    '8', '9', '*', '+', 0,   '-', '.', '/', 0,   0,   0,    0,   0,    0,   0,
    0,   0,   0,   '{', '|', '}', '~', 0,   0,   0,   0,    0,   0,    0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,    0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,    0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,    0,   0,
    0,   0,   0,   0,   0,   0,   ';', '=', ',', '-', '.',  '/', '\'', 0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,   0,    0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   '[', '\\', ']', '#',  '`'};

/* ----- PRIVATE GLOBALS ----- */

static bool keysPressedTable[0x100];

static bool keysHeldTable[0x100];

static bool keysReleasedTable[0x100];

static bool mouseButtonsPressedTable[0x3];

static bool mouseButtonsHeldTable[0x3];

static bool mouseButtonsReleasedTable[0x3];

static uint32_t mousePosX, mousePosY;

/* ----- INTERNAL FUNCTIONS ----- */

void InputManRecordUserInput(void) {
    /* Keyboard. */
    memcpy(keysPressedTable, *hldvars.keysPressedTable,
           sizeof(keysPressedTable));
    memcpy(keysHeldTable, *hldvars.keysHeldTable, sizeof(keysHeldTable));
    memcpy(keysReleasedTable, *hldvars.keysReleasedTable,
           sizeof(keysReleasedTable));

    /* Mouse. */
    memcpy(mouseButtonsPressedTable, *hldvars.mouseButtonsPressedTable,
           sizeof(mouseButtonsPressedTable));
    memcpy(mouseButtonsHeldTable, *hldvars.mouseButtonsHeldTable,
           sizeof(mouseButtonsHeldTable));
    memcpy(mouseButtonsReleasedTable, *hldvars.mouseButtonsReleasedTable,
           sizeof(mouseButtonsReleasedTable));
    mousePosX = *hldvars.mousePosX;
    mousePosY = *hldvars.mousePosY;

    return;
}

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT const bool *AERInputGetKeysPressed(void) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

    return keysPressedTable;
}

AER_EXPORT const bool *AERInputGetKeysHeld(void) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

    return keysHeldTable;
}

AER_EXPORT const bool *AERInputGetKeysReleased(void) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

    return keysReleasedTable;
}

AER_EXPORT const bool *AERInputGetMouseButtonsPressed(void) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

    return mouseButtonsPressedTable;
}

AER_EXPORT const bool *AERInputGetMouseButtonsHeld(void) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

    return mouseButtonsHeldTable;
}

AER_EXPORT const bool *AERInputGetMouseButtonsReleased(void) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK, NULL);

    return mouseButtonsReleasedTable;
}

AER_EXPORT void AERInputGetMousePosition(uint32_t *x, uint32_t *y) {
    ErrIf(stage != STAGE_ACTION, AER_SEQ_BREAK);
    ErrIf(!(x || y), AER_NULL_ARG);

    if (x)
        *x = mousePosX;
    if (y)
        *y = mousePosY;

    return;
}