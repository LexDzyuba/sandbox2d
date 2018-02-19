#define _CRT_SECURE_NO_WARNINGS


#include <SDL.h>
#include <SDL_Image.h>
#include <SDL_thread.h>
#include <SDL_mixer.h>
//#include <SDL2_gfxPrimitives.h>

#include <Windows.h>
#include <stdio.h>

#include "cstl.h"
#include "shapes.h"
#include "quadtree.h"
#include "tilesEngine.h"
#include "characters.h"


#define DEBUG


SDL_Window *pWindow = 0;
SDL_Renderer *pRenderer = 0;

SDL_Event event;
SDL_Thread *pThread = NULL;

SDL_Texture *pTexture = 0;
SDL_Rect sourceRectangle;
SDL_Rect destinationRectangle;

Quadtree *quad;
List *listRects;

SDL_Texture *pMap = NULL;
TileMap tmap;
TilesArray tarr;

Rect r; 
Rect r2;

Mix_Music *music = NULL;

Player player1;

int isRunning = 0;
struct Table *pTextureMap = NULL;
int currentFrame = 0;

int init(char *title, int x, int y, int width, int height, int flags);
void render();
void update();
void handleEvents();
void clean();

SDL_Texture *loadTexture(char *fileName, SDL_Renderer *pRenderer);
void draw(SDL_Renderer*, SDL_Texture*, int x, int y, int width, int height, SDL_RendererFlip flip);
void drawFrame(SDL_Texture*, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer *pRenderer, SDL_RendererFlip flip);
void drawRectangle();
void drawBorderQuadtree(Quadtree *quad, SDL_Renderer *pRenderer);


static SDL_Renderer *ren = NULL;

void* sdl_img_loader(const char *path) {
	return IMG_LoadTexture(ren, path);
}

int main(int argc, char **argv) {

#ifdef DEBUG
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
#endif

	if (!init("first", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE))
		isRunning = 1;
	else
		return 1;

	while (isRunning) {
		handleEvents();
		update();
		render();
	}

	clean();

	return 0; 
}

int strCmp(void *ls, void *rs) {
	return strcmp((char*)ls, (char*)rs);
}

int init(char *title, int x, int y, int width, int height, int flags) {
	IMG_Init(IMG_INIT_PNG);
	
	if (SDL_Init(SDL_INIT_EVERYTHING) >= 0) {
		pWindow = SDL_CreateWindow(title, x, y, width, height, flags);

		if (!pWindow) {
			SDL_Quit();
			return 1;
		}
		pRenderer = SDL_CreateRenderer(pWindow, -1, 0);
		if (!pRenderer) {
			SDL_DestroyWindow(pWindow);
			SDL_Quit();
			return 1;
		}
		SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
	}
	else {
		return 1;
	}

	TilesInfo tinfo;
	MapInfo minfo;

	tilesInfoParse(readFile("assets/dung.json"), &tinfo); 
	mapInfoParse(readFile("assets/map.json"), &minfo);

	printf("%d\n", minfo.lay[1].data[11]);
	//initTileMap(&tmap, "assets/dung_tilemap.png", pRenderer, minfo.tilewidth, minfo.tileheight);
	
	tilesArrayFill(pRenderer, &tarr, tinfo);
	pMap = concateTiles(pRenderer, tarr, minfo, tinfo);

	SDL_Texture *tmp = loadTexture("assets/human.png", pRenderer);
	SDL_Texture *arr[4];
	SDL_Rect sprRect = { .x = 0, .y = 0, .w = 48, .h = 18 };

	for (int i = 0; i < 4; i++) {
		arr[i] = getTileFromTileMap(pRenderer, tmp, sprRect);
		sprRect.y += sprRect.h;
	}
	Point xy = { 320, 320 };
	Point wh = { 16, 18 };
	setPlayer(&player1, xy, wh, FrontSide, arr);

	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
		printf("can't open audio\n");

	music = Mix_LoadMUS("assets/step.wav");
	if (music == NULL)
		printf("load ogg file: %s\n", Mix_GetError());
	if(Mix_PlayMusic(music, 0) == -1)
		printf("unable to play file: %s\n", Mix_GetError());
	//while (Mix_PlayingMusic());

	return 0;
}

void render() {
	SDL_RenderClear(pRenderer);
	
	SDL_Rect rre;
	rre.w = 16;
	rre.h = 16;
	rre.x = 0;
	rre.y = 0;
	SDL_QueryTexture(pMap, NULL, NULL, &rre.w, &rre.h);
	rre.w += 100;
	rre.h += 100;

	int jj = SDL_RenderCopy(pRenderer, pMap, NULL, &rre);
	if (jj == -1) {
		printf("jj\n");
	}


	if (player1.isMove)
		drawFrame(player1.currentSprite, player1.xy.x, player1.xy.y, player1.wh.x, player1.wh.y, 1, currentFrame % 3, pRenderer, SDL_FLIP_NONE);
	else
		drawFrame(player1.currentSprite, player1.xy.x, player1.xy.y, player1.wh.x, player1.wh.y, 1, 1, pRenderer, SDL_FLIP_NONE);

	SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(pRenderer);
	SDL_Delay(45);
}

void update() {
	currentFrame = (int)((SDL_GetTicks() / 100));
}

void handleEvents() {
	static int leftMouseBtnPrsd = 0;

	if (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			isRunning = 0;
			break;
		case SDL_MOUSEBUTTONUP:
			printf("btn released\n");

			if (event.button.button == SDL_BUTTON_LEFT)
				leftMouseBtnPrsd = 0;
			break;
		case SDL_MOUSEBUTTONDOWN:
			printf("btn pressed\n");
			if (event.button.button == SDL_BUTTON_LEFT)
				leftMouseBtnPrsd = 1;
		case SDL_MOUSEMOTION: {
				if (leftMouseBtnPrsd) {
					int x = event.motion.x;
					int y = event.motion.y;
					r.p[1].x = x;
					r.p[1].y = y;
					char t[24];
					sprintf(t, "X: %d, Y: %d", x, y);
					SDL_SetWindowTitle(pWindow, t);
				}
		}
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_RIGHT:
				movePlayer(&player1, Right, 5);
				break;
			case SDLK_LEFT:
				movePlayer(&player1, Left, 5);
				break;
			case SDLK_UP:
				movePlayer(&player1, Up, 5);
				break;
			case SDLK_DOWN:
				movePlayer(&player1, Down, 5);
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_DOWN:
				stopPlayer(&player1);
				break;
			case SDLK_UP:
				stopPlayer(&player1);
				break;
			case SDLK_RIGHT:
				stopPlayer(&player1);
				break;
			case SDLK_LEFT:
				stopPlayer(&player1);
				break;
			}
			break;
		default:
			break;
		}
	}
}

void clean() {
	SDL_DestroyWindow(pWindow);
	SDL_DestroyRenderer(pRenderer);

	if (pTextureMap) {
		for (int i = pTextureMap->size; i > 0; i--) {
			struct Node **p = popListTable(pTextureMap);
			while (*p != NULL) {
				struct Node *t = *p;
				*p = (*p)->next;
				SDL_DestroyTexture(t->data);
				free(t);
			}
			free(pTextureMap);
		}
}

	IMG_Quit();
	SDL_Quit();
}



//void draw(char *id, int x, int y, int width, int height, SDL_Renderer *pRenderer, SDL_RendererFlip flip) {
//	SDL_Rect srcRect;
//	SDL_Rect destRect;
//
//	srcRect.x = 0;
//	srcRect.y = 0;
//	destRect.w = srcRect.w = width;
//	destRect.h = srcRect.h = height;
//	destRect.x = x;
//	destRect.y = y;
//
//	SDL_RenderCopyEx(pRenderer, (SDL_Texture*)lookup(pTextureMap, id, hashStringNoCase, strCmp), &srcRect, &destRect, 0, 0, flip);
//}

void draw(SDL_Renderer *pRenderer, SDL_Texture *texture, int x, int y, int width, int height,  SDL_RendererFlip flip) {
	SDL_Rect srcRect;
	SDL_Rect destRect;

	srcRect.x = 0;
	srcRect.y = 0;
	destRect.w = srcRect.w = width;
	destRect.h = srcRect.h = height;
	destRect.x = x;
	destRect.y = y;

	SDL_RenderCopyEx(pRenderer, texture, &srcRect, &destRect, 0, 0, flip);
}

void drawFrame(SDL_Texture *text, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer *pRenderer, SDL_RendererFlip flip) {
	SDL_Rect srcRect;
	SDL_Rect destRect;

	srcRect.x = width * currentFrame;
	srcRect.y = height * (currentRow - 1);
	destRect.w = srcRect.w = width;
	destRect.h = srcRect.h = height;
	destRect.x = x;
	destRect.y = y;
	//SDL_RenderCopyEx(pRenderer, (SDL_Texture*)lookup(pTextureMap, id, hashStringNoCase, strCmp), &srcRect, &destRect, 0, 0, flip);
	SDL_RenderCopyEx(pRenderer, text, &srcRect, &destRect, 0, 0, flip);
}

void drawRectangle() {
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255);
	SDL_RenderFillRect(pRenderer, &sourceRectangle);
}

void drawBorderQuadtree(Quadtree *quad, SDL_Renderer *pRenderer) {
	if (quad == NULL)
		return;

	Point p11, p12, p21, p22;

	p11.x = quad->bounds.coordXY.x;
	p11.y = quad->bounds.coordXY.y + quad->bounds.width / 2;
	p12.x = quad->bounds.coordXY.x + quad->bounds.width;
	p12.y = quad->bounds.coordXY.y + quad->bounds.width / 2;

	p21.x = quad->bounds.coordXY.x + quad->bounds.width / 2;
	p21.y = quad->bounds.coordXY.y;
	p22.x = quad->bounds.coordXY.x + quad->bounds.width / 2;
	p22.y = quad->bounds.coordXY.y + quad->bounds.width;

	SDL_SetRenderDrawColor(pRenderer, 0, 255, 255, 255);
	SDL_RenderDrawLine(pRenderer, p11.x, p11.y, p12.x, p12.y);
	SDL_RenderDrawLine(pRenderer, p21.x, p21.y, p22.x, p22.y);
	
	if (quad->nodes[0] == NULL)
		return;

	drawBorderQuadtree(quad->nodes[0], pRenderer);
	//drawBorderQuadtree(quad->nodes[1], pRenderer);
	//drawBorderQuadtree(quad->nodes[2], pRenderer);
	//drawBorderQuadtree(quad->nodes[3], pRenderer);
}

