#include "quadtree.h"

void initQuadtree(Quadtree *qt, RectangleXY rct, int lvl, int maxlvl) {
	qt->bounds = rct;
	qt->level = lvl;
	qt->maxLevels = maxlvl;
	qt->maxObjects = 5;
	qt->nodes = NULL; // malloc(sizeof(Quadtree) * 4);
}

void splitQuadtree(Quadtree *qt) {
	int midWidth = qt->bounds.width / 2;
	int midHeight = qt->bounds.width / 2;

	int x = qt->bounds.coordXY.x;
	int y = qt->bounds.coordXY.y;

	qt->nodes = malloc(sizeof(Quadtree) * 4);

	RectangleXY rect = { {x + midWidth, y}, midWidth };
	initQuadtree(qt->nodes[0], rect, qt->level + 1, 10);
	rect.coordXY.x = x + midWidth; rect.coordXY.y = y;
	rect.width = midWidth;

	initQuadtree(qt->nodes[1], rect, qt->level + 1, 10);
	rect.coordXY.x = x; rect.coordXY.y = y;
	rect.width = midWidth;

	initQuadtree(qt->nodes[2], rect, qt->level + 1, 10);
	rect.coordXY.x = x; rect.coordXY.y = y + midHeight;
	rect.width = midHeight;

	initQuadtree(qt->nodes[3], rect, qt->level + 1, 10);
	rect.coordXY.x = x + midWidth; rect.coordXY.y = y + midHeight;
	rect.width = midWidth;
}

int indexObjInQuadtree(Quadtree qt, void *obj) {
	int index = -1;

	float vertSep = qt.bounds.coordXY.x + (qt.bounds.width / 2);
	float horizSep = qt.bounds.coordXY.y + (qt.bounds.width / 2);

	int topQuadrant = (((Rect*)obj)->p[1].y < horizSep &&
		((Rect*)obj)->p[1].y + qt.bounds.width < horizSep);

	int bottomQuadrant = ((Rect*)obj)->p[1].y > horizSep;

	if (((Rect*)obj)->p[2].x < vertSep &&
		((Rect*)obj)->p[2].x + qt.bounds.width < vertSep) {
		if (topQuadrant)
			index = 1;
		else if (bottomQuadrant)
			index = 2;
	}
	else if (((Rect*)obj)->p[2].x > vertSep) {
		if (topQuadrant)
			index = 0;
		else if (bottomQuadrant)
			index = 3;
	}
	return index;
}

void insertObjToQuadtree(Quadtree qt, void *obj) {
	if (qt.nodes[0] != NULL) {
		int index = indexObjInQuadtree(qt, obj);

		if (index != -1) {
			insertObjToQuadtree(*qt.nodes[index], obj);
			return;
		}
	}
	pushList(&qt.objects, obj);

	if (sizeList(qt.objects) > qt.maxObjects && qt.level < qt.maxLevels) {
		if (qt.nodes == NULL) {
			splitQuadtree(&qt);
		}
	}

	List *lst = qt.objects;
	while (lst != NULL) {
		void *data = foreachList(&lst);
		int index = indexObjInQuadtree(qt, data);
		if (index != -1) {
			insertObjToQuadtree(*qt.nodes[index], data);
		}
	}
}