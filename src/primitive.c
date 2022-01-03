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
#include "aer/primitive.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/primitive.h"

/* ----- PRIVATE MACROS ----- */

#define Cast(type, val)               \
    ({                                \
        typeof(val) Cast_val = (val); \
        *(type*)&Cast_val;            \
    })

#define EnsureValidType(prim)                 \
    do {                                      \
        switch (((Primitive*)(prim))->type) { \
            case AER_PRIMITIVE_REAL:          \
            case AER_PRIMITIVE_STRING:        \
            case AER_PRIMITIVE_ARRAY:         \
            case AER_PRIMITIVE_POINTER:       \
            case AER_PRIMITIVE_UNDEFINED:     \
            case AER_PRIMITIVE_INT32:         \
            case AER_PRIMITIVE_INT64:         \
            case AER_PRIMITIVE_BOOL:          \
                break;                        \
                                              \
            default:                          \
                Err(AER_BAD_VAL);             \
        }                                     \
    } while (0)

#define EnsureTypeIs(prim, primType) \
    Ensure((((const Primitive*)(prim))->type == (primType)), AER_FAILED_PARSE)

/* ----- INTERNAL FUNCTIONS ----- */

Primitive PrimitiveCopy(const Primitive* prim) {
    return *prim;
}

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT AERPrimitive AERPrimitiveMakeReal(double val) {
#define errRet Cast(AERPrimitive, PrimitiveMakeUndefined())
    Ok(Cast(AERPrimitive, PrimitiveMakeReal(val)));
#undef errRet
}

AER_EXPORT AERPrimitive AERPrimitiveMakeInt32(int32_t val) {
#define errRet Cast(AERPrimitive, PrimitiveMakeUndefined())
    Ok(Cast(AERPrimitive, PrimitiveMakeInt32(val)));
#undef errRet
}

AER_EXPORT AERPrimitive AERPrimitiveMakeInt64(int64_t val) {
#define errRet Cast(AERPrimitive, PrimitiveMakeUndefined())
    Ok(Cast(AERPrimitive, PrimitiveMakeInt64(val)));
#undef errRet
}

AER_EXPORT AERPrimitive AERPrimitiveMakeBool(bool val) {
#define errRet Cast(AERPrimitive, PrimitiveMakeUndefined())
    Ok(Cast(AERPrimitive, PrimitiveMakeBool(val)));
#undef errRet
}

AER_EXPORT AERPrimitive AERPrimitiveCopy(const AERPrimitive* prim) {
#define errRet Cast(AERPrimitive, PrimitiveMakeUndefined())
    EnsureArg(prim);
    EnsureValidType(prim);

    Ok(Cast(AERPrimitive, PrimitiveCopy((const Primitive*)prim)));
#undef errRet
}

AER_EXPORT AERPrimitiveType AERPrimitiveGetType(const AERPrimitive* prim) {
#define errRet AER_PRIMITIVE_UNDEFINED
    EnsureArg(prim);
    EnsureValidType(prim);

    Ok(((const Primitive*)prim)->type);
#undef errRet
}

AER_EXPORT double AERPrimitiveGetReal(const AERPrimitive* prim) {
#define errRet 0.0
    EnsureArg(prim);
    EnsureTypeIs(prim, AER_PRIMITIVE_REAL);

    Ok(((const Primitive*)prim)->val.real);
#undef errRet
}

AER_EXPORT int32_t AERPrimitiveGetInt32(const AERPrimitive* prim) {
#define errRet 0
    EnsureArg(prim);
    EnsureTypeIs(prim, AER_PRIMITIVE_INT32);

    Ok(((const Primitive*)prim)->val.i32);
#undef errRet
}

AER_EXPORT int64_t AERPrimitiveGetInt64(const AERPrimitive* prim) {
#define errRet 0
    EnsureArg(prim);
    EnsureTypeIs(prim, AER_PRIMITIVE_INT64);

    Ok(((const Primitive*)prim)->val.real);
#undef errRet
}

AER_EXPORT bool AERPrimitiveGetBool(const AERPrimitive* prim) {
#define errRet false
    EnsureArg(prim);
    EnsureTypeIs(prim, AER_PRIMITIVE_BOOL);

    Ok((bool)((const Primitive*)prim)->val.real);
#undef errRet
}

AER_EXPORT void AERPrimitiveSetReal(AERPrimitive* prim, double val) {
#define errRet
    EnsureArg(prim);
    EnsureTypeIs(prim, AER_PRIMITIVE_REAL);

    ((Primitive*)prim)->val.real = val;

    Ok();
#undef errRet
}

AER_EXPORT void AERPrimitiveSetInt32(AERPrimitive* prim, int32_t val) {
#define errRet
    EnsureArg(prim);
    EnsureTypeIs(prim, AER_PRIMITIVE_INT32);

    ((Primitive*)prim)->val.i32 = val;

    Ok();
#undef errRet
}

AER_EXPORT void AERPrimitiveSetInt64(AERPrimitive* prim, int64_t val) {
#define errRet
    EnsureArg(prim);
    EnsureTypeIs(prim, AER_PRIMITIVE_INT64);

    ((Primitive*)prim)->val.i64 = val;

    Ok();
#undef errRet
}

AER_EXPORT void AERPrimitiveSetBool(AERPrimitive* prim, bool val) {
#define errRet
    EnsureArg(prim);
    EnsureTypeIs(prim, AER_PRIMITIVE_BOOL);

    ((Primitive*)prim)->val.real = (double)val;

    Ok();
#undef errRet
}