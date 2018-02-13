#include "tilesEngine.h"

#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

int exampleArray[] = { 0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0 };

const int tileWidth = 16; const int tileHeight = 16;

void initTileMap(TileMap *map, char *fileName, SDL_Renderer *pRenderer, int tileW, int tileH) {
	map->pRend = pRenderer;
	map->map = loadTexture(fileName, pRenderer);

	if (!map->map) {
		printf("map don't load\n");
		exit(1);
	}

	SDL_QueryTexture(map->map, NULL, NULL, &map->mapW, &map->mapH);
	map->tileH = tileH;
	map->tileW = tileW;
}

SDL_Texture *getTileFromTileMap(SDL_Renderer *pRenderer, SDL_Texture *tileMap, SDL_Rect destRect) {
	SDL_Texture *texture = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, destRect.w, destRect.h);
	SDL_SetRenderTarget(pRenderer, texture); 
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_RenderCopy(pRenderer, tileMap, &destRect, NULL);
	SDL_SetRenderTarget(pRenderer, NULL);

	return texture;
}

void tilesArrayFill(TilesArray *arr, TileMap tileMap) {
	int cntRow = tileMap.mapH / tileMap.tileH;
	int cntColumn = tileMap.mapW / tileMap.tileW;

	arr->tilesArr = (Tile**)malloc(sizeof(Tile*) * cntRow);
	
	for (int i = 0; i < cntRow; i++) {
		arr->tilesArr[i] = malloc(sizeof(Tile) * cntColumn);
	}

	SDL_Rect destRect;// = { 0, 0, tileMap.tileW, tileMap.tileH };
	destRect.w = tileMap.tileW;
	destRect.h = tileMap.tileH;
	int x; int y;
	x = y = 0;

	for (int i = 0; i < cntRow; i++) {
		for (int j = 0; j < cntColumn; j++) {
			setLoc(i, j, &x, &y, tileMap);
			destRect.x = x;
			destRect.y = y;
			arr->tilesArr[i][j].texture = getTileFromTileMap(tileMap.pRend, tileMap.map, destRect);
		}
	}
}

SDL_Texture *loadTexture(char *fileName, SDL_Renderer *pRenderer) {
	SDL_Surface *pTmpSurface = NULL;
	pTmpSurface = IMG_Load(fileName);
	if (!pTmpSurface) {
		printf("IMG_Load: %s\n", IMG_GetError());
		return NULL;
	}
	SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pTmpSurface);

	SDL_FreeSurface(pTmpSurface);

	if (!pTexture) {
		printf("cannot crt txtr from surface\n");
	}

	return pTexture;
}
//temp solution
SDL_Texture *concateTiles(TileMap tmap, TilesArray arrTiles, int dataArr[], int dataX) {
	SDL_Texture *texture = SDL_CreateTexture(tmap.pRend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, dataX * 16, (sizeof(dataArr) / sizeof dataArr[0]) / dataX);
	SDL_SetRenderTarget(tmap.pRend, texture);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	SDL_Rect rect = { 0, 0, 16, 16 };

	int w = tmap.mapW / tmap.tileW;

	for (int i = 0; i < sizeof(dataArr) / sizeof(dataArr[0]); i++) {
		int numTile = dataArr[i];
		rect.x = i % dataX;
		rect.y = i / dataX;

		int posJ = numTile % w;
		int posI = numTile / w;

		SDL_RenderCopy(tmap.pRend, arrTiles.tilesArr[posI][posJ].texture, &rect, NULL);
	}
	
	SDL_SetRenderTarget(tmap.pRend, NULL);

	return texture;
}

void setLoc(int i, int j, int *x, int *y, TileMap tileM) {
	*x = j * tileM.tileW;
	*y = i * tileM.tileH;
}