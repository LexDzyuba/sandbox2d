#include "tilesEngine.h"

SDL_Texture *getTileFromTileMap(SDL_Renderer *pRenderer, SDL_Texture *tileMap, SDL_Rect destRect) {
	SDL_Texture *texture = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, destRect.w, destRect.h);
	SDL_SetRenderTarget(pRenderer, texture);
	SDL_RenderCopy(pRenderer, tileMap, &destRect, NULL);
	SDL_SetRenderTarget(pRenderer, NULL);

	return texture;
}