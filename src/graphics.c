
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
  off_texture = 0;
  switch (mode)
  {
    // Case one: access OAM
    case A_OAM:
      if (mode_count >= A_OAM_TIME)
      {
        mode = A_VRAM;
        requestInterrupt(LCD_STAT);
        stat_value = readMemory(STAT);
        stat_value |= 0x2;
        writeMemory(STAT, stat_value);
        mode_count = 0;
      }
      break;
    // Write to framebuffer here
    case A_VRAM:
      if (mode_count >= A_VRAM_TIME)
      {
        mode = H_BLANK;
        requestInterrupt(LCD_STAT);
        stat_value = readMemory(STAT);
        stat_value &= ~0x2;
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
          requestInterrupt(LCD_STAT);
          stat_value = readMemory(STAT);
          stat_value |= 0x1;
          writeMemory(STAT, stat_value);
        }
        else
        {
          IO_PORTS[0x44]++;
          printf("%hx;\n", IO_PORTS[0x44]);
          mode = A_OAM;
          stat_value = readMemory(STAT);
          stat_value |= 0x2;
          writeMemory(STAT, stat_value);
          requestInterrupt(BLANK);
          requestInterrupt(LCD_STAT);
          renderScanline();
        }
      }
      break;
    case V_BLANK:
      if (mode_count >= A_V_BLANK_TIME)
      {
          mode_count = 0;
          requestInterrupt(LCD_STAT);
          SDL_UpdateTexture(texture, NULL, framebuffer, SCREEN_WIDTH * sizeof(uint32_t));
          SDL_RenderClear(renderer);
          SDL_RenderCopy(renderer, texture, NULL, NULL);
          SDL_RenderPresent(renderer); 
          IO_PORTS[0x44] = 0;
          mode = A_OAM;
          stat_value = readMemory(STAT);
          stat_value |= 0x2;
          writeMemory(STAT, stat_value);
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
    }
  }
  // Render sprites
  if (lcdc & 0x2)
  {
    uint8_t sprite_big = (lcdc & 0x4) ? 16 : 8;
    // Iterate over all the sprites in OAM
    for (int i = 0; i < 40; ++i)
    {
      // Each sprite is 4 bytes in OAM
      size_t index = i * 4;
      uint8_t x = readMemory(OAM_LOWER + index + 1) - 8;
      uint8_t y = readMemory(OAM_LOWER + index) - 16;
      uint8_t tile_addr = readMemory(OAM_LOWER + index + 2);
      uint8_t attr = readMemory(OAM_LOWER + index + 3);
      uint8_t scanline = IO_PORTS[0x44];
      // Is the sprite rendered
      if (scanline >= y && (scanline < y + sprite_big))
      {
        int line = scanline - y;
        if (attr & 0x40)
        {
          line -= sprite_big;
          line *= -2;
        }
        else
          line *= 2;
        uint16_t addr = (TILE_DATA_BASE_1 + (tile_addr * 16)) + line;
        uint8_t pixel_1 = readMemory(addr);
        uint8_t pixel_2 = readMemory(addr + 1);
        // Each tile is 8 pixel long
        for (int pixel = 7; pixel >= 0; --pixel)
        {
          // If x-flip, flip pixel
          int col = pixel;
          if (attr & 0x20)
            col = (col - 7) * (-1);
          uint8_t color = (pixel_2 & (1 << col)) | (pixel_1 & (1 << (col + 1)));
          uint16_t pal_addr = (attr & 0x10) ? 0xff49 : 0xff48;
          uint8_t pal = readMemory(pal_addr);
          pal = (pal & (1 << color)) | (pal & (1 << (color + 1)));
          if (pal == 0)
            continue;
          int pos = x - pixel + 7;
          switch(pal)
          {
            case 0:
              framebuffer[pos + SCREEN_HEIGHT * scanline] = 0xFFFFFFFF;
              break;
            case 1:
              framebuffer[pos + SCREEN_HEIGHT * scanline] = 0x2C2A2AFF;
              break;
            case 2:
              framebuffer[pos + SCREEN_HEIGHT * scanline] = 0x706969FF;
              break;
            case 3:
              framebuffer[pos + SCREEN_HEIGHT * scanline] = 0x000000FF;
              break;
          }
        }
      }
    }
  }
}
