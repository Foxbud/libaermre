#include <assert.h>
#include <stdlib.h>

#include "foxutils/arraymacs.h"
#include "foxutils/hash.h"
#include "foxutils/mapmacs.h"

#include "internal/hld.h"
#include "internal/objtree.h"



/* ----- PRIVATE FUNCTIONS ----- */

static bool VisitObj(
		FoxMap * table,
		int32_t objIdx,
		uint32_t remainDepth,
		bool (* callback)(int32_t objIdx, void * ctx),
		void * ctx
) {
	bool cont = true;

	if (remainDepth > 0) {
		FoxArray * children = FoxMapMIndex(int32_t, FoxArray, table, objIdx);
		if (children) {
			size_t numChildren = FoxArrayMSize(int32_t, children);
			for (uint32_t idx = 0; idx < numChildren; idx++) {
				cont = VisitObj(
						table,
						*FoxArrayMIndex(int32_t, children, idx),
						remainDepth - 1,
						callback,
						ctx
				);
				if (!cont) break;
			}
		}
	}

	if (cont) {
		cont = callback(objIdx, ctx);
	}

	return cont;
}

static bool ElemDeinit(FoxArray * elem, void * ctx) {
	(void)ctx;

	FoxArrayMDeinit(int32_t, elem);

	return true;
}



/* ----- INTERNAL FUNCTIONS ----- */

ObjTree * ObjTreeNew(void) {
	ObjTree * tree = malloc(sizeof(ObjTree));
	assert(tree);
	FoxMapMInit(int32_t, FoxArray, &tree->table);

	return tree;
}

void ObjTreeFree(ObjTree * tree) {
	assert(tree);

	FoxMap * table = &tree->table;
	FoxMapMForEachElement(
			int32_t,
			FoxArray,
			table,
			&ElemDeinit,
			NULL
	);
	FoxMapMDeinit(int32_t, FoxArray, table);
	free(tree);

	return;
}

void ObjTreeInsert(
		ObjTree * tree,
		int32_t objIdx,
		int32_t childIdx
) {
	assert(tree);
	assert(objIdx != childIdx);

	FoxMap * table = &tree->table;
	FoxArray * parent = FoxMapMIndex(int32_t, FoxArray, table, objIdx);
	if (!parent) {
		parent = FoxMapMInsert(int32_t, FoxArray, table, objIdx);
		FoxArrayMInit(int32_t, parent);
	}
	*FoxArrayMPush(int32_t, parent) = childIdx;

	return;
}

void ObjTreeForEach(
		ObjTree * tree,
		int32_t rootObjIdx,
		size_t maxDepth,
		bool (* callback)(int32_t objIdx, void * ctx),
		void * ctx
) {
	assert(tree);
	assert(callback);

	VisitObj(
			&tree->table,
			rootObjIdx,
			maxDepth,
			callback,
			ctx
	);

	return;
}
