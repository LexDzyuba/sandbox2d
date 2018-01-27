
#include "shapes.h"

#include <math.h>
#include <stdio.h>

float angleToRadian(int a) {
	return (float)(M_PI / 180) * a;
}

int hline(SDL_Renderer * renderer, Sint16 x1, Sint16 x2, Sint16 y)
{
	return SDL_RenderDrawLine(renderer, x1, y, x2, y);;
}
 
int drawEllips(SDL_Renderer * renderer, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	int result;
	int ix, iy;
	int h, i, j, k;
	int oh, oi, oj, ok;
	int xmh, xph;
	int xmi, xpi;
	int xmj, xpj;
	int xmk, xpk;

	/*
	* Sanity check radii
	*/
	if ((rx < 0) || (ry < 0)) {
		return (-1);
	}

	/*
	* Special case for rx=0 - draw a vline
	*/
	//if (rx == 0) {
	//	return (vlineRGBA(renderer, x, y - ry, y + ry, r, g, b, a));
	//}
	///*
	//* Special case for ry=0 - draw a hline
	//*/
	//if (ry == 0) {
	//	return (hlineRGBA(renderer, x - rx, x + rx, y, r, g, b, a));
	//}

	/*
	* Set color
	*/
	result = 0;
	result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
	result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);

	/*
	* Init vars
	*/
	oh = oi = oj = ok = 0xFFFF;

	/*
	* Draw
	*/
	static kek = 0;

	if (rx > ry) {
		ix = 0;
		iy = rx * 64;

		do {
			h = (ix + 32) >> 6;
			i = (iy + 32) >> 6;
			j = (h * ry) / rx;
			k = (i * ry) / rx;

			if ((ok != k) && (oj != k)) {
				xph = x + h;
				xmh = x - h;
				if (k > 0) {
					result |= hline(renderer, xmh, xph, y + k);
					result |= hline(renderer, xmh, xph, y - k);
				}
				else {
					result |= hline(renderer, xmh, xph, y);
				}
				ok = k;
			}
			if ((oj != j) && (ok != j) && (k != j)) {
				xmi = x - i;
				xpi = x + i;
				if (j > 0) {
					result |= hline(renderer, xmi, xpi, y + j);
					result |= hline(renderer, xmi, xpi, y - j);
				}
				else {
					result |= hline(renderer, xmi, xpi, y);
				}
				oj = j;
			}

			ix = ix + iy / rx;
			iy = iy - ix / rx;

		} while (i > h);
	}
	else {
		ix = 0;
		iy = ry * 64;

		do {
			h = (ix + 32) >> 6;
			i = (iy + 32) >> 6;
			j = (h * rx) / ry;
			k = (i * rx) / ry;

			if ((oi != i) && (oh != i)) {
				xmj = x - j;
				xpj = x + j;
				if (i > 0) {
					result |= hline(renderer, xmj, xpj, y + i);
					result |= hline(renderer, xmj, xpj, y - i);
				}
				else {
					result |= hline(renderer, xmj, xpj, y);
				}
				oi = i;
			}
			if ((oh != h) && (oi != h) && (i != h)) {
				xmk = x - k;
				xpk = x + k;
				if (h > 0) {
					result |= hline(renderer, xmk, xpk, y + h);
					result |= hline(renderer, xmk, xpk, y - h);
				}
				else {
					result |= hline(renderer, xmk, xpk, y);
				}
				oh = h;
			}

			ix = ix + iy / ry;
			iy = iy - ix / ry;

			if (!kek) {
				printf("h = %d, i = %d, j = %d, k = %d \n", h, i, j, k);
			}

		} while (i > h);
		kek = 1;
	}
	return (result);
}

Point rotatePoint(Point p, Point center, int angle) {
	float rad = angleToRadian(angle);
	float cx = center.x; float cy = center.y;
	p.x = (p.x - cx) / 100;
	p.y = (p.y - cy) / 100;

	Point tmp = { p.x, p.y };

	p.x = (tmp.x * cos(rad) - tmp.y * sin(rad));
	p.y = (tmp.x * sin(rad) + tmp.y * cos(rad));

	p.x = (p.x * 100) + cx;
	p.y = (p.y * 100) + cy;

	return p;
}

void rotateRect(Rect *r, int angle) {
	for (int i = 1; i < 5; i++)
		r->p[i] = rotatePoint(r->p[i], r->p[0], angle);
}

void setParamsRect(Rect *r, float x, float y, int lengthSide) {
	float h = (float)lengthSide / 2;

	r->p[0].x = x;					r->p[0].y = y;
	r->p[1].x = r->p[0].x + h;		r->p[1].y = r->p[0].y - h;
	r->p[2].x = r->p[0].x - h;		r->p[2].y = r->p[0].y - h;
	r->p[3].x = r->p[0].x - h;		r->p[3].y = r->p[0].y + h;
	r->p[4].x = r->p[0].x + h;		r->p[4].y = r->p[0].y + h;
}

void drawRect(SDL_Renderer *pRenderer, Rect *r, uint32_t color) {
	uint8_t *c = (uint8_t*)&color;
	
	SDL_SetRenderDrawColor(pRenderer, c[0], c[1], c[2], c[3]);

	for (int i = 1; i < 4; i++) 
		SDL_RenderDrawLine(pRenderer, (int)r->p[i].x, (int)r->p[i].y, (int)r->p[i + 1].x, (int)r->p[i + 1].y);
	SDL_RenderDrawLine(pRenderer, (int)r->p[4].x, (int)r->p[4].y, (int)r->p[1].x, (int)r->p[1].y);
}


void drawFillRect(SDL_Renderer *pRenderer, Rect *r, uint32_t color) {
	drawRect(pRenderer, r, color);

	uint8_t *c = (uint8_t*)&color;
	for (int i = (int)r->p[2].y + 1; i < (int)r->p[3].y; i++) {
		for (int j = (int)r->p[2].x + 1; j < (int)r->p[1].x; j++)
			SDL_RenderDrawPoint(pRenderer, j, i);
	}
}

void moveRect(Rect *r, float moveVal, Direction d) {
	switch (d) {
	case Left:
		for (int i = 0; i < 5; i++)
			r->p[i].x -= moveVal;
		break;
	case Right:
		for (int i = 0; i < 5; i++)
			r->p[i].x += moveVal;
		break;
	case Up:
		for (int i = 0; i < 5; i++)
			r->p[i].y -= moveVal;
		break;
	case Down:
		for (int i = 0; i < 5; i++)
			r->p[i].y += moveVal;
		break;
	}
}

int collisionRect(Rect *fst, Rect *scnd) {
	float distance = scnd->p[0].x - fst->p[0].x;
	float halfWidthFst = (fst->p[1].x - fst->p[2].x) / 2;
	float halfWidthScnd = (scnd->p[1].x - scnd->p[2].x) / 2;
	
	float gapBetweenRect = distance - halfWidthFst - halfWidthScnd;
	// printf("%f === %f\n", halfWidthFst, halfWidthScnd);
	if (gapBetweenRect > 0)
		return 0;
	return 1;
}

void normalizeVec2f(Vector2f *vec) {
	float vecLength = sqrtf(vec->x * vec->x + vec->y * vec->y);
	
	vec->x /= vecLength;
	vec->y /= vecLength;
}

float dotProductVector2f(Vector2f a, Vector2f b) {
	return a.x * b.x + a.y * b.y;
}

Vector2f pointToVec2f(Point p) {
	Vector2f t = { p.x, p.y };
	return t;
}

void toRightNormal(Vector2f *vec) {
	float tmp = vec->x;
	vec->x = vec->y;
	vec->y = -tmp;
}

void getNormalsVec2f(Point *src, int length, Vector2f *dest) {
	Vector2f t;
	for (int i = 0; i < length - 1; i++) {
		t.x = src[i + 1].x - src[i].x;
		t.y = src[i + 1].y - src[i].y;
		toRightNormal(&t);
		dest[i] = t;
	}
	t.x = src[0].x - src[length - 1].x;
	t.y = src[0].y - src[length - 1].y;
	toRightNormal(&t);
	dest[length - 1] = t;
}

Vector2f minMaxPoint(Point *arr, int length, Vector2f axis) {
	float minProjRect = dotProductVector2f(pointToVec2f(arr[0]), axis);
	int minDotRect = 1;
	float maxProjRect = dotProductVector2f(pointToVec2f(arr[0]), axis);
	int maxDotRect = 1;

	for (int i = 1; i < length; i++) {
		float tmpProj = dotProductVector2f(pointToVec2f(arr[i]), axis);
		if (minProjRect > tmpProj) {
			minProjRect = tmpProj;
			minDotRect = i;
		}

		if (maxProjRect < tmpProj) {
			maxProjRect = tmpProj;
			maxDotRect = i;
		}
	}
	Vector2f t = { minProjRect, maxProjRect };
	return t;
}

int vecCollisionRect(Rect *fst, Rect *scnd) {
	Vector2f fstNormals[4];
	Vector2f scndNormals[4];
	
	getNormalsVec2f((fst->p) + 1, 4, fstNormals);
	getNormalsVec2f((scnd->p) + 1, 4, scndNormals);

	Vector2f resP1 = minMaxPoint((fst->p) + 1, 4, fstNormals[1]);
	Vector2f resP2 = minMaxPoint((scnd->p) + 1, 4, fstNormals[1]);
	Vector2f resQ1 = minMaxPoint((fst->p) + 1, 4, fstNormals[0]);
	Vector2f resQ2 = minMaxPoint((scnd->p) + 1, 4, fstNormals[0]);

	Vector2f resR1 = minMaxPoint((fst->p) + 1, 4, scndNormals[1]);
	Vector2f resR2 = minMaxPoint((scnd->p) + 1, 4, scndNormals[1]);
	Vector2f resS1 = minMaxPoint((fst->p) + 1, 4, scndNormals[0]);
	Vector2f resS2 = minMaxPoint((scnd->p) + 1, 4, scndNormals[0]);

	int sepP = resP1.y < resP2.x || resP2.y < resP1.x;
	int sepQ = resQ1.y < resQ2.x || resQ2.y < resQ1.x;
	int sepR = resR1.y < resR2.x || resR2.y < resR1.x;
	int sepS = resS1.y < resS2.x || resS2.y < resS1.x;

	if (sepP || sepQ || sepR || sepS)
		return 0;

	return 1;
}