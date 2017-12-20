
#include "graphics.h"
#include "cpu.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

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

void renderScanline()
{
  uint8_t lcdc = readMemory(LCDC);
  // Location of tile data
  if (lcdc & 0x1)
  {
    uint16_t tile_base;
    uint8_t y;
    uint8_t x;
    uint8_t x_offset_bg = readMemory(0xff43);
    uint8_t y_offset_bg = readMemory(0xff42);
    uint8_t x_offset_wi = readMemory(0xff4b) - 7;
    uint8_t y_offset_wi = readMemory(0xff4a);
    uint16_t bg_addr;
    uint16_t tile;
    uint16_t col;
    bool window_area = false;
    if (lcdc & 0x10)
      tile_base = TILE_DATA_BASE_1;
    else
      tile_base = TILE_DATA_BASE_2;
    // Windowing
    if (lcdc & 0x20)
    {
      if (y_offset_wi <= readMemory(LCDC_Y))
        window_area = true;
    }
    // Background data
    if (window_area)
    {
      if (lcdc & 0x40)
        bg_addr = BG_ADDR_1;
      else
        bg_addr = BG_ADDR_2;
    }
    else
    {
      if (lcdc & 0x8)
        bg_addr = BG_ADDR_1;
      else
        bg_addr = BG_ADDR_2;
    }
    if (window_area)
    {
      y = readMemory(0xff44) - y_offset_wi;
    }
    else
    {
      y = readMemory(0xff44) + y_offset_bg;
    }
    tile = (y/8) * 32;
    for (int i = 0; i < 160; ++i)
    {
      if (window_area && i >= x_offset_wi)
        x = i - x_offset_wi;
      else
        x = i + x_offset_bg;
      col = x/8;
      uint8_t location;
      if (lcdc & 0x10)
        location = tile_base + (readMemory(bg_addr + col + tile) * 16);
      else
        location = tile_base + ((((int8_t) readMemory(bg_addr + col + tile)) + 128) * 16);
      uint8_t line = (y % 8) * 2;
      uint8_t line1 = readMemory(location + line);
      uint8_t line2 = readMemory(location + line + 1);
      int8_t color_index = ((x % 8) - 7) * (-1);
      uint8_t color = (line2 & (1 << color_index)) | (line1 & (1 << color_index)); 
      color *= 2;
      // Get the color pallete
      uint8_t pallette_value = readMemory(0xff47);
      pallette_value = (pallette_value & (1 << color)) | (pallette_value & (1 << (color + 1)));
      uint8_t cur_scan_line = readMemory(0xff44);
      switch (pallette_value)
      {
        case 0:
          framebuffer[i + SCREEN_HEIGHT * cur_scan_line] = 0xFFFFFFFF;
          break;
        case 1:
          framebuffer[i + SCREEN_HEIGHT * cur_scan_line] = 0x2C2A2AFF;
          break;
        case 2:
          framebuffer[i + SCREEN_HEIGHT * cur_scan_line] = 0x706969FF;
          break;
        case 3:
          framebuffer[i + SCREEN_HEIGHT * cur_scan_line] = 0x000000FF;
          break;
      }
      SDL_UpdateTexture(texture, NULL, framebuffer, SCREEN_WIDTH * sizeof(uint32_t));
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer); 
    }
  }
  if (lcdc & 0x2)
  {

  }
}
