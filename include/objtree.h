#ifndef OBJTREE_H
#define OBJTREE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


/* ----- PUBLIC TYPES ----- */

typedef struct ObjTree {
	uint8_t rawData[
		sizeof(struct {
				HashTab * table;
		})
	];
} ObjTree;



/* ----- PUBLIC FUNCTIONS ----- */

ObjTree * ObjTreeNew(void);

void ObjTreeFree(ObjTree * tree);

void ObjTreeInsert(
		ObjTree * tree,
		int32_t objIdx,
		int32_t childIdx
);

void ObjTreeEach(
		ObjTree * tree,
		int32_t rootObjIdx,
		size_t maxDepth,
		bool (* callback)(int32_t objIdx, void * context),
		void * context
);



#endif /* OBJTREE_H */
