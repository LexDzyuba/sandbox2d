#pragma once

#include "cstl.h"
#include "shapes.h"

typedef struct Quadtree {
	int maxLevels;
	int maxObjects;
	int level;
	List *objects;
	RectangleXY bounds;
	struct Quadtree **nodes;
} Quadtree;

void initQuadtree(Quadtree *qt, RectangleXY rct, int level, int maxlvl);
void splitQuadtree(Quadtree *qt);
void insertObjToQuadtree(Quadtree qt, void *obj);
int indexObjInQuadtree(Quadtree qt, void *obj);