#pragma once

#include <SDL.h>
#include <cJSON.h>

typedef struct Tile {
	int x, y;
	int w, h;
	SDL_Texture *texture;
} Tile;

typedef struct TilesArray {
	Tile **tilesArr;
} TilesArray;

typedef struct TileMap {
	int tileW, tileH;
	int mapW, mapH;
	SDL_Texture *map;
	SDL_Renderer *pRend;
} TileMap;

void initTileMap(TileMap *map, char *fileName, SDL_Renderer *pRenderer, int tileW, int tileH);
SDL_Texture *getTileFromTileMap(SDL_Renderer *pRenderer, SDL_Texture *tileMap, SDL_Rect destRect);
void tilesArrayFill(TilesArray *arr, TileMap tileMap);
SDL_Texture *loadTexture(char *fileName, SDL_Renderer *pRenderer);
SDL_Texture *concateTiles(TileMap tmap, TilesArray arrTiles, int dataArr[], int dataX);
void setLoc(int i, int j, int *x, int *y, TileMap tileM);