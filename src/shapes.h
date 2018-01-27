#pragma once

#include <SDL.h>

typedef struct Point {
	float x;
	float y;
} Point;

typedef struct Vector2f {
	float x;
	float y;
} Vector2f;


typedef struct Rect {
	Point p[5];
} Rect;

typedef enum {Left, Right, Up, Down} Direction;

int drawEllips(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
Point rotatePoint(Point p, Point center, int angle);
void rotateRect(Rect *r, int angle);
void setParamsRect(Rect *r, float x1, float y1, int lengthSide);
void drawRect(SDL_Renderer *pRenderer, Rect *r, uint32_t color);
void drawFillRect(SDL_Renderer *pRenderer, Rect *r, uint32_t color);
int collisionRect(Rect *fst, Rect *scnd);

void moveRect(Rect *r, float moveVal, Direction d);

void normalizeVec2f(Vector2f *vec);
void toRightNormal(Vector2f *vec);
void getNormalsVec2f(Point *src, int length, Vector2f *dest);
float dotProductVector2f(Vector2f a, Vector2f b);
Vector2f pointToVec2f(Point p);
Vector2f minMaxPoint(Point *arr, int length, Vector2f axis);
int vecCollisionRect(Rect *fst, Rect *scnd);
//int collisionVecNormal(Rect *fst, Rect *scnd);