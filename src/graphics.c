
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
  memset(framebuffer, 0xFF, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
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
            IO_PORTS[0x44] = 0;
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

uint32_t getColor(uint8_t color)
{
  // Return the color
  switch(color & 0x3)
  {
    // Case 0: White
    case 0:
      return 0xFFFFFFFF;
    // Light grey
    case 1:
      return 0xFF948888;
    // Dark grey
    case 2:
      return 0xFF6E6C6C;
    // Black
    case 3: 
      return 0xFF000000;
  }
  return 0;
}

void renderBG()
{
  uint8_t lcdc = readMemory(0xff40);
  uint8_t pallete_data = readMemory(0xff47);
  uint8_t ly = readMemory(0xff44);
  uint8_t scrollY = readMemory(0xff42);
  uint8_t scrollX = readMemory(0xff43);
  // Check if BG is enabled
  if (!(lcdc & 0x1))
  {
    // Make white, get out
    memset(framebuffer, 0xFF, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
    return;
  }
  uint32_t palette[] = {
    getColor(pallete_data & 0x3),
    getColor((pallete_data >> 2) & 0x3),
    getColor((pallete_data >> 4) & 0x3),
    getColor((pallete_data >> 6) & 0x3)
  };
  // Get location of BG tiles
  uint16_t bg_tile_addr = (lcdc & 0x10) ? 0x8000 : 0x9000;
  // Get location of tile numbers
  uint16_t bg_tile_nums = (lcdc & 0x8) ? 0x9C00 : 0x9800;
  // Figure out which tile to select
  uint8_t tileY = (uint8_t)(((ly + scrollY) / 8) % 32);

  // Figure out the line of the tile to render
  uint8_t offsetY = (uint8_t)((ly + scrollY) % 8);

  // Write all pixels
  for (int i = 0; i < 160; ++i)
  {
    // X coordinate of the tile
    uint8_t tileX = (((i + scrollX) / 8) % 32);
    // Read the tile number
    uint8_t tile_no = readMemory((bg_tile_nums + (tileY * 32) + tileX));
    uint16_t final_addr;
    if (bg_tile_addr == 0x8000)
      final_addr = (bg_tile_addr + (tile_no * 16));
    else
      final_addr = (bg_tile_addr + ((int8_t)(tile_no) * 16));
    // Lines are 2 bytes
    final_addr += (uint16_t) (offsetY) * 2;
    // Read the bytes
    uint8_t low = readMemory(final_addr);
    uint8_t high = readMemory(final_addr + 1);

    uint8_t bit_no = (7 - ((scrollX + i) % 8));
    uint8_t bit_low = (low & (1 << bit_no)) ? 0x1 : 0x00;
    uint8_t bit_high = (high & (1 << bit_no)) ? 0x2 : 0x00;
    uint32_t color = palette[bit_low + bit_high];
    int array_index = (ly  * 160) + i;
    framebuffer[array_index] = color;
  }
}

void renderWindow()
{
  // Check if the window is even on screen
  uint8_t windowY = readMemory(0xff4a);
  uint8_t ly = readMemory(0xff44);
  uint8_t lcdc = readMemory(0xff40);
  uint8_t pallete_data = readMemory(0xff47);
  int x = readMemory(0xff4b) - 7;
  if (((int)(windowY)- ly) < 0)
    return;
  uint16_t winY = windowY - ly;
  // Create the palette
  uint32_t palette[] = {
    getColor(pallete_data & 0x3),
    getColor((pallete_data >> 2) & 0x3),
    getColor((pallete_data >> 4) & 0x3),
    getColor((pallete_data >> 6) & 0x3)
  };

  // Get location of BG tiles
  uint16_t win_tile_addr = (lcdc & 0x10) ? 0x8000 : 0x9000;
  // Get location of tile numbers
  uint16_t win_tile_nums = (lcdc & 0x40) ? 0x9C00 : 0x9800;
  uint8_t win_addr = (uint8_t)(winY / 8);
  uint8_t win_offset = (uint8_t)(winY % 8);
  for (int i = 0; i < 160; ++i)
  { 
    if (i < x)
      continue;
    uint8_t tileX = ((i - x) / 8);

    uint8_t tile_no = readMemory((win_tile_nums + (win_addr * 32) + tileX));
    uint16_t final_addr;
    if (win_tile_addr == 0x8000)
      final_addr = (win_tile_addr + (tile_no * 16));
    else
      final_addr = (win_tile_addr + ((int8_t)(tile_no) * 16));
    // Lines are 2 bytes
    final_addr += (uint16_t) (win_offset) * 2;

    uint8_t low = readMemory(final_addr);
    uint8_t high = readMemory(final_addr + 1);
    uint8_t bit_no = 7 - i % 8;
    uint8_t bit_low = (low & (1 << bit_no)) ? 0x1 : 0x00;
    uint8_t bit_high = (high & (1 << bit_no)) ? 0x2 : 0x00;
    uint32_t color = palette[bit_low + bit_high];
    int array_index = (ly  * 160) + i;
    framebuffer[array_index] = color;
  }
}

void renderSprites()
{
  uint8_t lcdc = readMemory(0xff40);
  uint8_t ly = readMemory(0xff44);
  uint8_t palette_1 = readMemory(0xff48);
  uint8_t palette_2 = readMemory(0xff49);
  uint8_t palette;
  // Loop through each sprite in OAM
  for (int i = 39; i >= 0; --i)
  {
    uint16_t oam_base = 0xfe00;
    uint16_t addr_base = 0x8000;
    // Sprites occupy 4 bytes
    uint16_t sprite_addr = (i * 4) + oam_base;
    uint8_t spriteY = readMemory(sprite_addr);
    uint8_t sprite_height = (lcdc & 0x4) ? 16 : 8;

    int y = spriteY - 16;
    if ((y <= ly) && ((y + sprite_height) > ly))
    {
      uint8_t spriteX = readMemory(sprite_addr + 1);
      uint8_t sprite_tile_no = readMemory(sprite_addr + 2);
      uint8_t sprite_attr = readMemory(sprite_addr + 3);
      if (sprite_height == 16)
        sprite_tile_no &= 0xFE;
      palette = ((sprite_attr & 0x10) ? palette_2 : palette_1);
      int x = spriteX - 8;
      uint32_t paletteArray[] = {
        // Transparent
        0x00000000,
        getColor((palette >> 2) & 0x3),
        getColor((palette >> 4) & 0x3),
        getColor((palette >> 6) & 0x3)
      };
      uint16_t tile_addr = addr_base + (sprite_tile_no * 16);
      uint8_t offsetY = (sprite_attr & 0x40) ? ((sprite_height - 1) - (ly - y)):
        (ly - y);
      tile_addr += offsetY * 2;
      uint8_t low = readMemory(tile_addr);
      uint8_t high = readMemory(tile_addr + 1);
      // Go through each pixel of sprite
      for (int j = 0; j < 8; ++j)
      {
        int pixel_x = j + x;
        if (pixel_x >= 0 && pixel_x < 160)
        {
          uint8_t bit_no = (sprite_attr & 0x20) ? j : (7 - j);
          uint8_t color_val = 0x0;
          if (high & (1 << bit_no))
            color_val |= 0x2;
          if (low & (1 << bit_no))
            color_val |= 0x1;
          int final_color = paletteArray[color_val];
          if (final_color)
          {
            int array_index = (ly * 160) + i;
            framebuffer[array_index] = final_color;
          }
        }
      }
    }
  }
}

void renderScanline()
{
  uint8_t lcdc = readMemory(0xff40);
  renderBG();
  if (lcdc & 0x20)
    renderWindow();
  renderSprites();
}

