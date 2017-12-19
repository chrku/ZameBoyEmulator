// Graphics subsystem
// Uses libsdl2
// (C) C. Kummer

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>

// Pointer to central window structure
extern SDL_Window* window;

extern SDL_Renderer* renderer;

extern SDL_Texture* texture;

extern int current_scanline;

#define MAX_SCANLINE 153
#define MAX_VISIBLE_SCANLINE 144

#define SCANLINE_LOCATION 0xff44


#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define CHANNELS 4

extern uint32_t framebuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

int initGraphics();

int doGraphics();

int renderFrameBuffer();

int destoryGraphics();

#endif
