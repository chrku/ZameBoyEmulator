
#include "graphics.h"
#include "cpu.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

SDL_Window* window;

SDL_Renderer* renderer;

static int off_texture = 0;

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
  window = SDL_CreateWindow("LameBoy Emulator", 100, 100, SCREEN_WIDTH,
      SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
      SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

  // Default colour: black
  memset(framebuffer, 0xcf, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
  SDL_UpdateTexture(texture, NULL, framebuffer, SCREEN_WIDTH * sizeof(uint32_t));
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer); 
  IO_PORTS[0x44] = 0;
  mode = V_BLANK;
  return 0;
}

void doGraphics()
{
  uint8_t lcdc = readMemory(LCDC);
  uint8_t stat_value = readMemory(STAT);
  uint64_t diff =  cycle_counter - last_cycle_count;
  last_cycle_count += diff;
  if (!(lcdc & 0x80))
  {
    if (!off_texture)
    {
      memset(framebuffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
      SDL_UpdateTexture(texture, NULL, framebuffer, SCREEN_WIDTH * sizeof(uint32_t));
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer); 
      off_texture = 1;
    }
    return;
  }
  mode_count += diff;
  off_texture = 0;
  switch (mode)
  {
    // Case one: access OAM
    case A_OAM:
      if (mode_count >= A_OAM_TIME)
      {
        mode = A_VRAM;
        stat_value = readMemory(STAT);
        stat_value = (stat_value & ~0x3) | 0x2;
        writeMemory(STAT, stat_value);
        mode_count = 0;
      }
      break;
    // Write to framebuffer here
    case A_VRAM:
      if (mode_count >= A_VRAM_TIME)
      {
        mode = H_BLANK;
        renderScanline();
        stat_value = readMemory(STAT);
        stat_value = (stat_value & ~0x3);
        if (stat_value & 0x8)
          requestInterrupt(LCD_STAT);
        writeMemory(STAT, stat_value);
        mode_count = 0;
      }
      break;
    case H_BLANK:
      if (mode_count >= A_H_BLANK_TIME)
      {
        mode_count = 0;
        if (IO_PORTS[0x44] == 144)
        {
          // Render frame, VBLANK
          SDL_UpdateTexture(texture, NULL, framebuffer, SCREEN_WIDTH * sizeof(uint32_t));
          SDL_RenderClear(renderer);
          SDL_RenderCopy(renderer, texture, NULL, NULL);
          SDL_RenderPresent(renderer); 
          mode = V_BLANK;
          if (stat_value & 0x10)
            requestInterrupt(LCD_STAT);
          requestInterrupt(BLANK);
          stat_value = readMemory(STAT);
          stat_value = (stat_value & ~0x3) | 1;
          writeMemory(STAT, stat_value);
        }
        else
        {
          IO_PORTS[0x44]++;
          mode = A_OAM;
          stat_value = readMemory(STAT);
          stat_value = (stat_value & ~0x3) | 2;
          writeMemory(STAT, stat_value);
          if (stat_value & 0x20)
            requestInterrupt(LCD_STAT);
        }
      }
      break;
    case V_BLANK:
      if (mode_count >= 456) //TODO MAGIC VALUE
      {
          mode_count = 0;
          IO_PORTS[0x44]++;
          if (IO_PORTS[0x44] >= 154)
          {
            mode = A_OAM;
            stat_value = readMemory(STAT);
            stat_value = (stat_value & ~0x3) | 2;
            writeMemory(STAT, stat_value);
            if (stat_value & 0x20)
              requestInterrupt(LCD_STAT);
            }
      }
      break;
  }
  if (IO_PORTS[0x44] == IO_PORTS[0x45])
    IO_PORTS[0x41] |= 0x4;
  else
    IO_PORTS[0x41] &= ~0x4;
  if (IO_PORTS[0x44] == IO_PORTS[0x45] && (stat_value & 0x40))
    requestInterrupt(LCD_STAT);
}

void renderScanline()
{
  //TODO: Proper implementation
}
