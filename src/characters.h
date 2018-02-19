#pragma once

#include <SDL.h>

#include "tilesEngine.h"
#include "cstl.h"
#include "shapes.h"

typedef enum { FrontSide, RightSide, BackSide, LeftSide } TileSide;

typedef SDL_Texture SpriteMap;

typedef struct Player {
	Point xy;
	Point wh;
	TileSide side;
	SpriteMap *playerSide[4];
	SpriteMap *currentSprite;
	int isMove;
} Player;

void setPlayer(Player *player, Point xy, Point wh, TileSide side, SpriteMap *playerSide[4]);
void movePlayer(Player *player, Direction dir, int step);
void stopPlayer(Player *player);
