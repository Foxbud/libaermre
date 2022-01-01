#ifndef INTERNAL_ITER_H
#define INTERNAL_ITER_H

#include <stddef.h>

#include "aer/iter.h"

/* ----- INTERNAL TYPES ----- */

typedef struct IteratorEnv {
    AERIterator* closure;
} IteratorEnv;

/* ----- INTERNAL FUNCTIONS ----- */

IteratorEnv* IteratorEnvNew(size_t envSize, void* callback);

void IteratorEnvFree(IteratorEnv* env);

#endif /* INTERNAL_ITER_H */