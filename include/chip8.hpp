#pragma once

#include <cstdint>

class Chip8 {

public:
    void Initialize();
    void LoadROM(const char* filename);
    void EmulateCycle();
    uint32_t gfx[64 * 32];
    uint8_t keypad[16]{};
    uint8_t* GetMemory() 
        {return memory;}

private:
    uint16_t pc{};
    uint16_t opcode{};
    uint16_t I{}; 
    uint8_t sp{};

    uint8_t registers[16]{}; 
    uint16_t stack[16]{};
    uint8_t memory[4096]{}; 
    
    uint8_t delayTimer{};
    uint8_t soundTimer{};
};
