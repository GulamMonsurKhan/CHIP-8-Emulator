#pragma once

#include <cstdint>
#include <filesystem>

class Chip8 {

public:
    void Initialize();
    void LoadROM(const std::filesystem::path& filepath);
    void EmulateCycle();
    void TickTimers();
    uint32_t gfx[64 * 32];
    uint8_t keypad[16]{};
    uint8_t* GetMemory() 
        {return memory;}
    bool waitingForKey = false;
    uint8_t waitingRegister = 0;
    int8_t pressedKey = -1; 


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
