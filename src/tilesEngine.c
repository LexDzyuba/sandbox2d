#include "tilesEngine.h"

#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int exampleArray[] = { 0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0,
					   0, 0, 0, 0, 0 };

const int tileWidth = 16; const int tileHeight = 16;

void initTileMap(TileMap *map, char *fileName, SDL_Renderer *pRenderer, int tileW, int tileH) {
	map->pRend = pRenderer;


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

void tilesArrayFill(SDL_Renderer *pRend, TilesArray *arr, TilesInfo tInfo) {
	int cntRow = tInfo.imagewidth / tInfo.tilewidth;
	int cntColumn = tInfo.columns;

	arr->tilesArr = (Tile**)malloc(sizeof(Tile*) * cntRow);
	
	for (int i = 0; i < cntRow; i++) {
		arr->tilesArr[i] = malloc(sizeof(Tile) * cntColumn);
	}

	SDL_Texture *map = loadTexture(tInfo.image, pRend);

	if (!map) {
		printf("map don't load\n");
		SDL_Delay(500);
		exit(1);
	}

	SDL_Rect destRect;// = { 0, 0, tileMap.tileW, tileMap.tileH };
	destRect.w = tInfo.tilewidth;
	destRect.h = tInfo.tileheight;
	int x; int y;
	x = y = 0;

	for (int i = 0; i < cntRow; i++) {
		for (int j = 0; j < cntColumn; j++) {
			setLoc(i, j, &x, &y, tInfo);
			destRect.x = x;
			destRect.y = y;
			arr->tilesArr[i][j].texture = getTileFromTileMap(pRend, map, destRect);
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
SDL_Texture *concateTiles(SDL_Renderer *pRend, TilesArray arrTiles, MapInfo mInfo, TilesInfo tInfo) {
//	int wtex = dataX * tmap.tileW; 
//	int htex = lngthDataArr / dataX * tmap.tileH;
	SDL_Texture *texture = SDL_CreateTexture(pRend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, mInfo.width * mInfo.tilewidth, mInfo.height * mInfo.tileheight);
	SDL_SetRenderTarget(pRend, texture);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	SDL_Rect rect = { 0, 0, tInfo.tilewidth, tInfo.tileheight };

	int w = tInfo.columns; //tmap.mapW / tmap.tileW;

	for (int i = 0; i < mInfo.width * mInfo.height; i++) {
		int numTile = mInfo.lay[0].data[i];  // dataArr[i];
		rect.x = i % mInfo.width * tInfo.tilewidth;
		rect.y = i / mInfo.height * tInfo.tileheight;

		int posJ = numTile % w;
		int posI = numTile / w;

		SDL_RenderCopy(pRend, arrTiles.tilesArr[posI][posJ].texture, NULL, &rect);
	}
	
	SDL_SetRenderTarget(pRend, NULL);

	return texture;
}

void setLoc(int i, int j, int *x, int *y, TilesInfo tInfo) {
	*x = j * tInfo.tilewidth;
	*y = i * tInfo.tileheight;
}

int tilesInfoParse(char* str, TilesInfo *pInfo) {
	const cJSON *columns = NULL;
	int status = 0;
	cJSON *infoJson = cJSON_Parse(str);

	if (infoJson == NULL) {
		const char *error_ptr = cJSON_GetErrorPtr();
		if (error_ptr != NULL) {
			fprintf(stderr, "Error before: %s \n", error_ptr);
		}
		status = 0;
		goto end;
	}
	pInfo->columns = cJSON_GetObjectItemCaseSensitive(infoJson, "columns")->valueint;
	pInfo->image = strDup(cJSON_GetObjectItemCaseSensitive(infoJson, "image")->valuestring);
	pInfo->imageheight = cJSON_GetObjectItemCaseSensitive(infoJson, "imageheight")->valueint;
	pInfo->imagewidth = cJSON_GetObjectItemCaseSensitive(infoJson, "imagewidth")->valueint;
	pInfo->margin = cJSON_GetObjectItemCaseSensitive(infoJson, "margin")->valueint;
	pInfo->name = strDup(cJSON_GetObjectItemCaseSensitive(infoJson, "name")->valuestring);
	pInfo->spacing = cJSON_GetObjectItemCaseSensitive(infoJson, "spacing")->valueint;
	pInfo->tilecount = cJSON_GetObjectItemCaseSensitive(infoJson, "tilecount")->valueint;
	pInfo->tileheight = cJSON_GetObjectItemCaseSensitive(infoJson, "tileheight")->valueint;
	pInfo->tilewidth = cJSON_GetObjectItemCaseSensitive(infoJson, "tilewidth")->valueint;
	pInfo->type = strDup(cJSON_GetObjectItemCaseSensitive(infoJson, "type")->valuestring);

	//printf("%s\n", pInfo->type);
end:
	cJSON_Delete(infoJson);
	return status;
}

int mapInfoParse(char *str, MapInfo *pMapInfo) {
	const cJSON *layers = NULL;
	const cJSON *layer = NULL;
	cJSON *map_json = cJSON_Parse(str);

	layers = cJSON_GetObjectItemCaseSensitive(map_json, "layers");
	int layersLength = cJSON_GetArraySize(layers);

	pMapInfo->height = cJSON_GetObjectItemCaseSensitive(map_json, "height")->valueint;
	pMapInfo->width = cJSON_GetObjectItemCaseSensitive(map_json, "width")->valueint;
	pMapInfo->tileheight = cJSON_GetObjectItemCaseSensitive(map_json, "tileheight")->valueint;
	pMapInfo->tilewidth = cJSON_GetObjectItemCaseSensitive(map_json, "tilewidth")->valueint;


	pMapInfo->lay = malloc(sizeof(*pMapInfo->lay) * layersLength);
	pMapInfo->laySize = layersLength;

	for (int i = 0; i < layersLength; i++) {
		cJSON *subitem = cJSON_GetArrayItem(layers, i);
		cJSON *data = cJSON_GetObjectItemCaseSensitive(subitem, "data");

		int ldata = cJSON_GetArraySize(data);
		pMapInfo->lay[i].data = malloc(sizeof(int) * ldata);

		for (int j = 0; j < ldata; j++) {
			pMapInfo->lay[i].data[j] = cJSON_GetArrayItem(data, j)->valueint;
		}
	}
	return 0;
}

char *strDup(char *str) {
	char *s = malloc(sizeof(char) * strlen(str) + 1);
	strcpy(s, str);
	return s;
}