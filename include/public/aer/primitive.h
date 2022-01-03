/**
 * @file
 *
 * @brief Utilities for interacting with the core primitives of the Game Maker
 * Language.
 *
 * @since {{MRE_NEXT_MINOR}}
 *
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
#ifndef AER_PRIMITIVE_H
#define AER_PRIMITIVE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ----- PUBLIC TYPES ----- */

typedef enum AERPrimitiveType {
    AER_PRIMITIVE_REAL = 0x0,
    AER_PRIMITIVE_STRING = 0x1,
    AER_PRIMITIVE_ARRAY = 0x2,
    AER_PRIMITIVE_POINTER = 0x3,
    // AER_PRIMITIVE_VEC3 = 0x4,
    AER_PRIMITIVE_UNDEFINED = 0x5,
    // AER_PRIMITIVE_OBJECT = 0x6,
    AER_PRIMITIVE_INT32 = 0x7,
    // AER_PRIMITIVE_VEC4 = 0x8,
    // AER_PRIMITIVE_MATRIX = 0x9,
    AER_PRIMITIVE_INT64 = 0xa,
    // AER_PRIMITIVE_ACCESSOR = 0xb,
    // AER_PRIMITIVE_NULL = 0xc,
    AER_PRIMITIVE_BOOL = 0xd,
    // AER_PRIMITIVE_ITERATOR = 0xe,
} AERPrimitiveType;

typedef struct AERPrimitive {
    uint8_t raw[16];
} AERPrimitive;

/* ----- PUBLIC FUNCTIONS ----- */

AERPrimitive AERPrimitiveMakeReal(double val);

// AERPrimitive AERPrimitiveMakeString(size_t size);

// AERPrimitive AERPrimitiveMakePointer(void* val, void (*destructor)(void*));

AERPrimitive AERPrimitiveMakeInt32(int32_t val);

AERPrimitive AERPrimitiveMakeInt64(int64_t val);

AERPrimitive AERPrimitiveMakeBool(bool val);

AERPrimitive AERPrimitiveCopy(const AERPrimitive* prim);

// AERPrimitive AERPrimitiveToString(const AERPrimitive* prim);

// void AERPrimitiveDisown(AERPrimitive prim);

AERPrimitiveType AERPrimitiveGetType(const AERPrimitive* prim);

double AERPrimitiveGetReal(const AERPrimitive* prim);

// char* AERPrimitiveGetString(const AERPrimitive* prim);

// void* AERPrimitiveGetPointer(const AERPrimitive* prim);

int32_t AERPrimitiveGetInt32(const AERPrimitive* prim);

int64_t AERPrimitiveGetInt64(const AERPrimitive* prim);

bool AERPrimitiveGetBool(const AERPrimitive* prim);

void AERPrimitiveSetReal(AERPrimitive* prim, double val);

void AERPrimitiveSetInt32(AERPrimitive* prim, int32_t val);

void AERPrimitiveSetInt64(AERPrimitive* prim, int64_t val);

void AERPrimitiveSetBool(AERPrimitive* prim, bool val);

#endif /* AER_PRIMITIVE_H */