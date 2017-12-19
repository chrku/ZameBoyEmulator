
#include "graphics.h"
#include "cpu.h"
#include <stdio.h>
#include <SDL2/SDL.h>

SDL_Window* window;

SDL_Renderer* renderer;

SDL_Texture* texture;

int current_scanline;

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

  return 1;
}
