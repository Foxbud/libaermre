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
#ifndef INTERNAL_PRIMITIVE_H
#define INTERNAL_PRIMITIVE_H

#include "aer/primitive.h"

/* ----- INTERNAL MACROS ----- */

#define PrimitiveMakeUndefined()         \
    ((Primitive){                        \
        .type = AER_PRIMITIVE_UNDEFINED, \
        .val.raw = {0},                  \
    })

#define PrimitiveMakeReal(init)     \
    ((Primitive){                   \
        .type = AER_PRIMITIVE_REAL, \
        .val.real = (init),         \
    })

#define PrimitiveMakeInt32(init)     \
    ((Primitive){                    \
        .type = AER_PRIMITIVE_INT32, \
        .val.i32 = (init),           \
    })

#define PrimitiveMakeInt64(init)     \
    ((Primitive){                    \
        .type = AER_PRIMITIVE_INT64, \
        .val.i64 = (init),           \
    })

#define PrimitiveMakeBool(init)     \
    ((Primitive){                   \
        .type = AER_PRIMITIVE_BOOL, \
        .val.real = (bool)(init),   \
    })

/* ----- INTERNAL TYPES ----- */

typedef struct Primitive {
    union {
        uint8_t raw[12];
        double real;
        int32_t i32;
        int64_t i64;
    } val;
    AERPrimitiveType type;
} Primitive;

/* ----- INTERNAL FUNCTIONS ----- */

Primitive PrimitiveCopy(const Primitive* prim);

#endif /* INTERNAL_PRIMITIVE_H */