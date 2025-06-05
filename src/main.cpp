#include <iostream>
#include <SDL2/SDL.h>
#include <chip8.hpp>

#define ON_COLOR 0xDC143CFF //True Crimson
#define OFF_COLOR 0x1E1E1EFF //Graphite Grey

const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;
const int SCALE = 12;

int main()  {
     
     Chip8 chip8;
     chip8.Initialize();
     chip8.LoadROM("roms/IBM_Logo.ch8");

     SDL_Window *window = SDL_CreateWindow("CHIP-8 Emulator",  //Initialize SDL, window creation
          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
          SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE, 
          SDL_WINDOW_SHOWN);
     //Handles window drawing
     SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,  SDL_RENDERER_ACCELERATED);

     SDL_Texture *texture = SDL_CreateTexture(renderer, //Texture to hold pixel data, RGBA8888 format
          SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
          SCREEN_WIDTH, SCREEN_HEIGHT);

     bool running = true;
     SDL_Event event;

     while (running) { //Main emulation loop
          while (SDL_PollEvent(&event)) { //Input handling, check SDL events
               if (event.type == SDL_QUIT) {
                    running = false;
               }
               chip8.EmulateCycle();

               uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT]; //CHIP-8 graphics buffer to SDL pixel buffer
               for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
                    pixels[i] = chip8.gfx[i] ? ON_COLOR : OFF_COLOR; 
               }
               //Update texture with pixel data
               SDL_UpdateTexture(texture, nullptr, pixels, SCREEN_WIDTH * sizeof(uint32_t));
               //Render the updated texture to the window
               SDL_RenderClear(renderer);
               SDL_RenderCopy(renderer, texture, nullptr, nullptr);
               SDL_RenderPresent(renderer);

               SDL_Delay(16); //60 fps: 1000ms / 60 = 16ms
          }

     }
     //Clean up
     SDL_DestroyTexture(texture);
     SDL_DestroyRenderer(renderer);
     SDL_DestroyWindow(window);
     SDL_Quit();

     return 0;
}
