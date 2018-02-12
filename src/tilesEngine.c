#include "tilesEngine.h"

#include <SDL_image.h>
#include <stdio.h>

int exampleArray[] = { 0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0 };

const int tileWidth = 16; const int tileHeight = 16;

void initTileMap(TileMap *map, char *fileName, SDL_Renderer *pRenderer, int tileW, int tileH) {
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

SDL_Texture *concateTiles(SDL_Renderer *pRenderer, int *arrTiles) {

	return NULL;
}