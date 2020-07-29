#include <assert.h>
#include <stdlib.h>
#include "dynarr.h"
#include "hashtab.h"
#include "hld.h"
#include "objtree.h"



/* ----- PRIVATE TYPES ----- */

typedef struct Tree {
	HashTab * table;
} Tree;



/* ----- PRIVATE FUNCTIONS ----- */

static uint32_t KeyHash(void * key) {
	return *((uint32_t *)key);
}

static bool KeyEqual(void * key, void * other) {
	return *((int32_t *)key) == *((int32_t *)other);
}

static bool VisitObj(
		HashTab * table,
		int32_t objIdx,
		uint32_t remainDepth,
		bool (* callback)(int32_t objIdx, void * context),
		void * context
) {
	bool cont = true;

	if (remainDepth > 0) {
		bool exists;
		DynArr * children = HashTabGet(table, &objIdx, &exists);
		if (exists) {
			size_t numChildren = DynArrSize(children);
			for (uint32_t idx = 0; idx < numChildren; idx++) {
				cont = VisitObj(
						table,
						*((int32_t *)DynArrGet(children, idx)),
						remainDepth - 1,
						callback,
						context
				);
				if (!cont) break;
			}
		}
	}

	if (cont) {
		cont = callback(objIdx, context);
	}

	return cont;
}



/* ----- PUBLIC FUNCTIONS ----- */

ObjTree * ObjTreeNew(void) {
	Tree * tree = malloc(sizeof(Tree));
	assert(tree);
	tree->table = HashTabNew(
			9,
			sizeof(int32_t),
			&KeyHash,
			&KeyEqual
	);

	return (ObjTree *)tree;
}

void ObjTreeFree(ObjTree * tree) {
	assert(tree);

	HashTab * table = ((Tree *)tree)->table;
	HashTabIter * iter = HashTabIterNew(table);
	DynArr * obj;
	while (HashTabIterNext(iter, NULL, (void **)&obj)) {
		while (DynArrSize(obj) > 0) {
			free(DynArrPop(obj));
		}
	}
	HashTabIterFree(iter);
	HashTabFree(table);
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

	HashTab * table = ((Tree *)tree)->table;
	bool exists;
	DynArr * parent = HashTabGet(table, &objIdx, &exists);
	if (!exists) {
		parent = DynArrNew(4);
		HashTabInsert(table, &objIdx, parent);
	}

	int32_t * child = malloc(sizeof(int32_t));
	assert(child);
	*child = childIdx;
	DynArrPush(parent, child);

	return;
}

void ObjTreeEach(
		ObjTree * tree,
		int32_t rootObjIdx,
		size_t maxDepth,
		bool (* callback)(int32_t objIdx, void * context),
		void * context
) {
	assert(tree);
	assert(callback);

	VisitObj(
			((Tree *)tree)->table,
			rootObjIdx,
			maxDepth,
			callback,
			context
	);

	return;
}
