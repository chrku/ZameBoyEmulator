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

extern uint64_t current_cycle_count;

extern uint64_t last_cycle_count;

extern uint64_t mode_count;

extern int mode;

typedef enum _graphics_state_ 
{
  H_BLANK, V_BLANK, A_VRAM, A_OAM
} graphics_state;

#define MAX_SCANLINE 153
#define MAX_VISIBLE_SCANLINE 144

#define SCANLINE_REGISTER 0xff44
#define LCDC 0xff40
#define STAT 0xff41
#define LCDC_Y 0xff44

#define TILE_DATA_BASE_1 0x8000
#define TILE_DATA_BASE_2 0x8800

#define BG_ADDR_1 0x9c00
#define BG_ADDR_2 0x9800

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 153

#define CHANNELS 4

// Timings:

#define A_OAM_TIME 80
#define A_VRAM_TIME 172
#define A_H_BLANK_TIME 204
#define A_V_BLANK_TIME 4560

extern uint32_t framebuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

int initGraphics();

/// Manage the framebuffer
void doGraphics();

void renderScanline();

void destoryGraphics();

#endif
