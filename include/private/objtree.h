#ifndef PRIVATE_OBJTREE_H
#define PRIVATE_OBJTREE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "foxutils/map.h"


/* ----- PUBLIC TYPES ----- */

typedef struct ObjTree {
	FoxMap table;
} ObjTree;



/* ----- PUBLIC FUNCTIONS ----- */

ObjTree * ObjTreeNew(void);

void ObjTreeFree(ObjTree * tree);

void ObjTreeInsert(
		ObjTree * tree,
		int32_t objIdx,
		int32_t childIdx
);

void ObjTreeForEach(
		ObjTree * tree,
		int32_t rootObjIdx,
		size_t maxDepth,
		bool (* callback)(int32_t objIdx, void * ctx),
		void * ctx
);



#endif /* PRIVATE_OBJTREE_H */
