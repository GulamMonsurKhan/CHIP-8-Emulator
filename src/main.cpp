#include <iostream>
#include <chip8.hpp>

int main()  {
     Chip8 chip8;

     chip8.Initialize();
     chip8.LoadROM("roms/IBM_Logo.ch8");

     const uint8_t* memory = chip8.GetMemory();

     std::cout << "\nROM loaded and memory initialized." << std::endl;
     return 0;
}
