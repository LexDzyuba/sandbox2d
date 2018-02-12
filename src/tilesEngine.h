#pragma once

#include <SDL.h>
#include <cJSON.h>

typedef struct Tile {
	int x, y;
	int w, h;
	SDL_Texture *texture;
} Tile;

typedef struct TileMap {
	int tileW, tileH;
	int mapW, mapH;
	SDL_Texture *map;
} TileMap;

void initTileMap(TileMap *map, char *fileName, SDL_Renderer *pRenderer);
SDL_Texture *getTileFromTileMap(SDL_Renderer *pRenderer, SDL_Texture *tileMap, SDL_Rect destRect);
SDL_Texture *loadTexture(char *fileName, SDL_Renderer *pRenderer);
SDL_Texture *concateTiles(SDL_Renderer *pRenderer, int *arrTiles);