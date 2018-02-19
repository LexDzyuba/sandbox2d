#include "characters.h"

void setPlayer(Player *player, Point xy, Point wh, TileSide side, SpriteMap *playerSide[4]) {
	player->xy = xy;
	player->wh = wh;
	player->side = side;
	player->currentSprite = playerSide[0];
	player->isMove = 0;

	for (int i = 0; i < 4; i++)
		player->playerSide[i] = playerSide[i];
}

void movePlayer(Player *player, Direction dir, int step) {
	player->isMove = 1;
	switch (dir) {
	case Left: player->xy.x -= step;
		player->currentSprite = player->playerSide[LeftSide];
		break;
	case Up:	player->xy.y -= step;
		player->currentSprite = player->playerSide[FrontSide];
		break;
	case Right:	player->xy.x += step;
		player->currentSprite = player->playerSide[RightSide];
		break;
	case Down:	player->xy.y += step;
		player->currentSprite = player->playerSide[BackSide];
		break;
	}
}

void stopPlayer(Player *player) {
	player->isMove = 0;
	//player->currentSprite = player->playerSide[1];
}
