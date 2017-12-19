
#include "graphics.h"
#include "cpu.h"
#include <stdio.h>
#include <SDL2/SDL.h>

SDL_Window* window;

SDL_Renderer* renderer;

SDL_Texture* texture;

int mode = 0;

uint64_t current_cycle_count = 0;
uint64_t last_cycle_count = 0;
uint64_t mode_count = 0;

uint32_t framebuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

int initGraphics()
{
  // Initialize SDL subsystems
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    return -1;
  }
  // Create window
  window = SDL_CreateWindow("LameBoy Emulator", 100, 100, SCREEN_HEIGHT,
      SCREEN_WIDTH, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
  if (window == NULL)
  {
    SDL_Quit();
    return -1;
  }
  // Initialize renderer
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL)
  {
    SDL_DestroyWindow(window);
    SDL_Quit();
    return -1;
  }

  // Create the texture
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);

  // Default colour: black
  memset(framebuffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
  SDL_UpdateTexture(texture, NULL, framebuffer, SCREEN_WIDTH * sizeof(uint32_t));
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer); 
  IO_PORTS[0x44] = 0;
  return 1;
}

void doGraphics()
{
  uint8_t lcdc = readMemory(LCDC);
  uint8_t stat_value;
  uint8_t diff = (uint8_t) cycle_counter - last_cycle_count;
  last_cycle_count += diff;
  mode_count += diff;
  if (!(lcdc & 0x80))
  {
    mode_count = 0;
    stat_value = readMemory(STAT);
    stat_value &= 0xfc;
    stat_value |= 0x1;
    IO_PORTS[0x44] = 0;
    writeMemory(STAT, stat_value);
    mode = V_BLANK;
    memset(framebuffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
    SDL_UpdateTexture(texture, NULL, framebuffer, SCREEN_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer); 
    return;
  }
  switch (mode)
  {
    // Case one: access OAM
    case A_OAM:
      if (mode_count >= A_OAM_TIME)
      {
        mode = A_VRAM;
        stat_value = readMemory(STAT);
        stat_value |= 0x11;
        writeMemory(STAT, stat_value);
        mode_count = 0;
      }
      break;
    // Write to framebuffer here
    case A_VRAM:
      if (mode_count >= A_VRAM_TIME)
      {
        mode = H_BLANK;
        stat_value = readMemory(STAT);
        stat_value &= ~0x11;
        writeMemory(STAT, stat_value);
        mode_count = 0;
      }
      break;
    case H_BLANK:
      if (mode_count >= A_H_BLANK_TIME)
      {
        mode_count = 0;
        if (IO_PORTS[0x44] >= MAX_VISIBLE_SCANLINE)
        {
          mode = V_BLANK;
          stat_value = readMemory(STAT);
          stat_value |= 0x1;
          writeMemory(STAT, stat_value);
        }
        else
        {
          IO_PORTS[0x44]++;
          mode = A_OAM;
          stat_value = readMemory(STAT);
          stat_value |= 0x2;
          writeMemory(STAT, stat_value);
        }
      }
      break;
    case V_BLANK:
      if (mode_count >= A_V_BLANK_TIME)
      {
          mode_count = 0;
          IO_PORTS[0x44] = 0;
          mode = A_OAM;
          stat_value = readMemory(STAT);
          stat_value |= 0x2;
          writeMemory(STAT, stat_value);
      }
      break;
  }
}
