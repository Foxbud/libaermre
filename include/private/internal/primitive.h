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

typedef struct PrimitivePointerWrapper {
    void* ref;
    void (*decRefs)(struct PrimitivePointerWrapper*);
} PrimitivePointerWrapper;

typedef struct PrimitiveRCPointerWrapper {
    PrimitivePointerWrapper base;
    void (*destructor)(void*);
    size_t refc;
} PrimitiveRCPointerWrapper;

typedef struct Primitive {
    union {
        uint32_t raw[3];
        double real;
        struct {
            union {
                PrimitivePointerWrapper* norm;
                PrimitiveRCPointerWrapper* rc;
            } wrap;
            uint32_t : 32;
            uint32_t : 3;
            bool isOwner : 1;
            bool isRC : 1;
        } ptr;
        int32_t i32;
        int64_t i64;
    } val;
    AERPrimitiveType type;
} Primitive;

/* ----- INTERNAL FUNCTIONS ----- */

Primitive PrimitiveMakePointer(void* ref, void (*destructor)(void*));

Primitive PrimitiveCopy(const Primitive* prim);

#endif /* INTERNAL_PRIMITIVE_H */