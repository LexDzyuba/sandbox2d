#include "quadtree.h"

void initQuadtree(Quadtree **qt, RectangleXY rct, int lvl, int maxlvl) {
	*qt = malloc(sizeof(Quadtree*));
	(*qt)->bounds = rct;
	(*qt)->level = lvl;
	(*qt)->maxLevels = maxlvl;
	(*qt)->maxObjects = 2;
	(*qt)->objects = NULL;
	(*qt)->nodes = (Quadtree**)malloc(sizeof(Quadtree*) * 4);
	(*qt)->nodes[0] = NULL;
}

void splitQuadtree(Quadtree *qt) {
	int midWidth = qt->bounds.width / 2;
	int midHeight = qt->bounds.width / 2;

	int x = qt->bounds.coordXY.x;
	int y = qt->bounds.coordXY.y;

	//qt->nodes = malloc(sizeof(Quadtree) * 4);

	RectangleXY rect = { {x + midWidth, y}, midWidth };
	initQuadtree(&qt->nodes[0], rect, qt->level + 1, 10);
	rect.coordXY.x = x + midWidth; rect.coordXY.y = y;
	rect.width = midWidth;

	initQuadtree(&qt->nodes[1], rect, qt->level + 1, 10);
	rect.coordXY.x = x; rect.coordXY.y = y;
	rect.width = midWidth;

	initQuadtree(&qt->nodes[2], rect, qt->level + 1, 10);
	rect.coordXY.x = x; rect.coordXY.y = y + midHeight;
	rect.width = midHeight;

	initQuadtree(&qt->nodes[3], rect, qt->level + 1, 10);
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

int compareToPointers(void *d1, void *d2) {
	if (&(*(Rect *)d1) == &(*(Rect *)d2))
		return 0;
	else
		return -1;
}

void insertObjToQuadtree(Quadtree *qt, void *obj) {
	if (qt->nodes[0] != NULL) {
		int index = indexObjInQuadtree(*qt, obj);

		if (index != -1) {
			insertObjToQuadtree(qt->nodes[index], obj);
			return;
		}
	}
	pushList(&qt->objects, obj);

	if (sizeList(qt->objects) > qt->maxObjects && qt->level < qt->maxLevels) {
		if (qt->nodes[0] == NULL) {
			splitQuadtree(qt);
		}
	}

	List *lst = qt->objects;
	while (lst != NULL) {
		void *data = foreachList(&lst);
		int index = indexObjInQuadtree(*qt, data);
		if (index != -1) {
			deleteList(&qt->objects, data, compareToPointers);
			insertObjToQuadtree(qt->nodes[index], data);
		}
	}
}

List *retriveQuadtree(Quadtree qt, void *obj) {
	int index = indexObjInQuadtree(qt, obj);
	if (index != -1 && qt.nodes[0] != NULL) {
		return retriveQuadtree(*qt.nodes[index], obj);
	}
	return qt.objects;
}