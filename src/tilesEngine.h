#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <SDL.h>
#include <cJSON.h>

#include "cstl.h"

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

typedef struct TilesInfo {
	int columns;
	char *image;
	int imageheight;
	int imagewidth;
	int margin;
	char *name;
	int spacing;
	int tilecount;
	int tileheight;
	int tilewidth;
	char *type;
} TilesInfo;

typedef struct MapInfo {
	int height;
	int infinite;

	struct layers {
		int *data;
		int height;
		char *name;
		float opacity;
		char *type;
		int visible;
		int width;
		int x;
		int y;
	} *lay;

	int laySize;
	int nextobjectid;
	char *orientation;
	char *renderorder;
	char *tiledversion;
	int tileheight;
	struct {
		int firstid;
		char *source;
	} *tilesets;
	int tilesetsSize;
	int tilewidth;
	char *type;
	int version;
	int width;
} MapInfo;

void initTileMap(TileMap *map, char *fileName, SDL_Renderer *pRenderer, int tileW, int tileH);
SDL_Texture *getTileFromTileMap(SDL_Renderer *pRenderer, SDL_Texture *tileMap, SDL_Rect destRect);
void tilesArrayFill(SDL_Renderer *pRend, TilesArray *arr, TilesInfo tInfo);
SDL_Texture *loadTexture(char *fileName, SDL_Renderer *pRenderer);
SDL_Texture *concateTiles(SDL_Renderer *pRend, TilesArray arrTiles, MapInfo mInfo, TilesInfo tInfo);
void setLoc(int i, int j, int *x, int *y, TilesInfo tInfo);

int tilesInfoParse(char* str, TilesInfo *pInfo);
int mapInfoParse(char *str, MapInfo *pMapInfo);
char *strDup(char *str);