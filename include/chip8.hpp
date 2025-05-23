#pragma once

#include <cstdint>

class Chip8 {
public:
    uint32_t gfx[64 * 32];
    uint8_t keypad[16]{};

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
