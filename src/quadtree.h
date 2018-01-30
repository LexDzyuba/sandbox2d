#pragma once

#include "cstl.h"
#include "shapes.h"

typedef struct Quadtree {
	int maxLevels;
	int level;
	List *objects;
	RectangleXY bounds;
	struct Quadtree **nod;
} Quadtree;

void initQuadtree(Quadtree *qt, int x, int y);
void split(Quadtree *qt);
