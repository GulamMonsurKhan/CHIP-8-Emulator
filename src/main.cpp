#include <iostream>
#include <filesystem>
#include <SDL2/SDL.h>
#include <chip8.hpp>

#define ON_COLOR 0xDC143CFF //True Crimson
#define OFF_COLOR 0x1E1E1EFF //Graphite Grey

const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;
const int SCALE = 12;
const int INSTRUCTIONS_PER_FRAME = 15;

int main(int argc, char* argv[])  {

     if (argc < 2) {
          std::cerr << "Usage: " << argv[0] << " <ROM file>" << std::endl;
          return 1;
     }

     std::filesystem::path romPath = argv[1];
     std::filesystem::path currentPath = std::filesystem::current_path();

     if (currentPath.filename() == "build") {
          romPath = currentPath.parent_path() / "roms" / romPath;
     } else {
          romPath = "roms" / romPath;
     }

     std::cout << "Loading ROM from: " << romPath << std::endl;
     
     Chip8 chip8;
     chip8.Initialize();
     chip8.LoadROM(romPath);

     if (SDL_Init(SDL_INIT_VIDEO) < 0) {
          std::cerr << "SDL could not initialize. SDL_Error: " << SDL_GetError() << std::endl;
          return 1;
     }

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

     uint32_t lastTimerUpdate = SDL_GetTicks();

     while (running) { //Main emulation loop
          while (SDL_PollEvent(&event)) { //Input handling, check SDL events
               if (event.type == SDL_QUIT) {
                    running = false;
               }
               else if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {
                         case SDLK_x: chip8.keypad[0x0] = 1; break;
                         case SDLK_1: chip8.keypad[0x1] = 1; break;
                         case SDLK_2: chip8.keypad[0x2] = 1; break;
                         case SDLK_3: chip8.keypad[0x3] = 1; break;
                         case SDLK_q: chip8.keypad[0x4] = 1; break;
                         case SDLK_w: chip8.keypad[0x5] = 1; break;
                         case SDLK_e: chip8.keypad[0x6] = 1; break;
                         case SDLK_a: chip8.keypad[0x7] = 1; break;
                         case SDLK_s: chip8.keypad[0x8] = 1; break;
                         case SDLK_d: chip8.keypad[0x9] = 1; break;
                         case SDLK_z: chip8.keypad[0xA] = 1; break;
                         case SDLK_c: chip8.keypad[0xB] = 1; break;
                         case SDLK_4: chip8.keypad[0xC] = 1; break;
                         case SDLK_r: chip8.keypad[0xD] = 1; break;
                         case SDLK_f: chip8.keypad[0xE] = 1; break;
                         case SDLK_v: chip8.keypad[0xF] = 1; break;
                    }
               }
               else if (event.type == SDL_KEYUP) {
                    switch (event.key.keysym.sym) {
                         case SDLK_x: chip8.keypad[0x0] = 0; break;
                         case SDLK_1: chip8.keypad[0x1] = 0; break;
                         case SDLK_2: chip8.keypad[0x2] = 0; break;
                         case SDLK_3: chip8.keypad[0x3] = 0; break;
                         case SDLK_q: chip8.keypad[0x4] = 0; break;
                         case SDLK_w: chip8.keypad[0x5] = 0; break;
                         case SDLK_e: chip8.keypad[0x6] = 0; break;
                         case SDLK_a: chip8.keypad[0x7] = 0; break;
                         case SDLK_s: chip8.keypad[0x8] = 0; break;
                         case SDLK_d: chip8.keypad[0x9] = 0; break;
                         case SDLK_z: chip8.keypad[0xA] = 0; break;
                         case SDLK_c: chip8.keypad[0xB] = 0; break;
                         case SDLK_4: chip8.keypad[0xC] = 0; break;
                         case SDLK_r: chip8.keypad[0xD] = 0; break;
                         case SDLK_f: chip8.keypad[0xE] = 0; break;
                         case SDLK_v: chip8.keypad[0xF] = 0; break;
                    }
               }
          }

          for (int i = 0; i < INSTRUCTIONS_PER_FRAME; ++i) {
               chip8.EmulateCycle();
          }

          uint32_t currentTicks = SDL_GetTicks();
          if (currentTicks - lastTimerUpdate >= 1000 /60 ) { //16ms
               chip8.TickTimers();
               lastTimerUpdate = currentTicks;
          }

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
     //Clean up
     SDL_DestroyTexture(texture);
     SDL_DestroyRenderer(renderer);
     SDL_DestroyWindow(window);
     SDL_Quit();

     return 0;
}
