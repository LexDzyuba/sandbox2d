#define _CRT_SECURE_NO_WARNINGS

#include <SDL.h>
#include <SDL_Image.h>
#include <SDL_thread.h>
//#include <SDL2_gfxPrimitives.h>


#include <Windows.h>
#include <stdio.h>

#include "cstl.h"
#include "shapes.h"

#define DEBUG

struct Player {
	int x;
	int y;    
}; 

SDL_Window *pWindow = 0;
SDL_Renderer *pRenderer = 0;

SDL_Event event;
SDL_Thread *pThread = NULL;

SDL_Texture *pTexture = 0;
SDL_Rect sourceRectangle;
SDL_Rect destinationRectangle;

Rect r; 
Rect r2;

int isRunning = 0;
struct Table *pTextureMap = NULL;
int currentFrame = 0;

int init(char *title, int x, int y, int width, int height, int flags);
void render();
void update();
void handleEvents();
void clean();

int loadTexture(char *fileName, char *id, SDL_Renderer *pRenderer);
void draw(char *id, int x, int y, int width, int height, SDL_Renderer *pRenderer, SDL_RendererFlip flip);
void drawFrame(char *id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer *pRenderer, SDL_RendererFlip flip);
void drawRectangle();


static SDL_Renderer *ren = NULL;

void* sdl_img_loader(const char *path) {
	return IMG_LoadTexture(ren, path);
}

int main(int argc, char **argv) {

#ifdef DEBUG
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
#endif

	if (!init("first", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE))
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

	pTextureMap = createTable(10);

	if(loadTexture("assets/animate-alpha.png", "animate", pRenderer))
		printf("can't loaded texture\n");

	sourceRectangle.x = 20;
	sourceRectangle.y = 20;
	sourceRectangle.w = 20;
	sourceRectangle.h = 20;
	
	setParamsRect(&r, 320, 200, 50);
	setParamsRect(&r2, 400, 200, 25);
	//rotateRect(&r, 45);
	//rotateRect(&r2, 45);
	//initRect(&r, 0, 0, 1);

	return 0;
}

void render() {
	SDL_RenderClear(pRenderer);
	draw("animate", 0, 0, 128, 82, pRenderer, SDL_FLIP_NONE);
	drawFrame("animate", 100, 100, 128, 82, 1, currentFrame, pRenderer, SDL_FLIP_NONE);
	SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
	static f = 0;

	drawFillRect(pRenderer, &r, 0xFF0000FF);
	drawRect(pRenderer, &r2, 0xFFFF00FF);
	
	if (vecCollisionRect(&r, &r2))
		printf("rect's collide\n");
	
	SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
	SDL_RenderPresent(pRenderer);
	SDL_Delay(45);
}

void update() {
	currentFrame = (int)((SDL_GetTicks() / 100) % 6);
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
				moveRect(&r, 5, Right);
				break;
			case SDLK_LEFT:
				//setParamsRect(&r, r.p[0].x - 5, r.p[0].y, 50);
				//Direction d = Left;
				moveRect(&r, 5, Left);
				break;
			case SDLK_UP:
				moveRect(&r, 5, Up);
				break;
			case SDLK_DOWN:
				moveRect(&r, 5, Down);
				break;
			}
		default:
			break;
		}
	}
}

void clean() {
	SDL_DestroyWindow(pWindow);
	SDL_DestroyRenderer(pRenderer);
	IMG_Quit();
	SDL_Quit();
}

int loadTexture(char *fileName, char *id, SDL_Renderer *pRenderer) {
	SDL_Surface *pTmpSurface = NULL;
	pTmpSurface = IMG_Load(fileName);
	if (!pTmpSurface) {
		printf("IMG_Load: %s\n", IMG_GetError());
		return 1;
	}
	SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pTmpSurface);
	
	SDL_FreeSurface(pTmpSurface);

	if (!pTexture) {
		printf("cannot crt txtr from surface\n");
		return 1;
	}

	insert(pTextureMap, id, pTexture, hashStringNoCase, strCmp);

	return 0;
}

void draw(char *id, int x, int y, int width, int height, SDL_Renderer *pRenderer, SDL_RendererFlip flip) {
	SDL_Rect srcRect;
	SDL_Rect destRect;

	srcRect.x = 0;
	srcRect.y = 0;
	destRect.w = srcRect.w = width;
	destRect.h = srcRect.h = height;
	destRect.x = x;
	destRect.y = y;

	SDL_RenderCopyEx(pRenderer, (SDL_Texture*)lookup(pTextureMap, id, hashStringNoCase, strCmp), &srcRect, &destRect, 0, 0, flip);
}
void drawFrame(char *id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer *pRenderer, SDL_RendererFlip flip) {
	SDL_Rect srcRect;
	SDL_Rect destRect;

	srcRect.x = width * currentFrame;
	srcRect.y = height * (currentRow - 1);
	destRect.w = srcRect.w = width;
	destRect.h = srcRect.h = height;
	destRect.x = x;
	destRect.y = y;
	SDL_RenderCopyEx(pRenderer, (SDL_Texture*)lookup(pTextureMap, id, hashStringNoCase, strCmp), &srcRect, &destRect, 0, 0, flip);
}

void drawRectangle() {
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255);
	SDL_RenderFillRect(pRenderer, &sourceRectangle);
}
