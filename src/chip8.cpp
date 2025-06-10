#include <chip8.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>

const uint8_t Chip8_fontset[80] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void Chip8::Initialize()
{
    pc = 0x200; //System expects application to be loaded at this memory location
    opcode = 0;
    I = 0;
    sp = 0;

    std::fill(memory, memory + 4096, 0);
    std::fill(gfx, gfx + 64 * 32, 0); 
    std::fill(stack, stack + 16, 0);
    std::fill(registers, registers + 16, 0);
    std::fill(keypad, keypad + 16, 0);

    //Load fontset into memory starting at 0x50
    for(int i = 0; i < 80; ++i)
        memory[0x50 + i] = Chip8_fontset[i];
}

void Chip8::LoadROM(const std::filesystem::path& filename)
{   
    //Open the file as a stream of binary and move the file pointer to the end
    std::ifstream rom(filename, std::ios::binary | std::ios::ate);

    if (rom.is_open())
    {
        //Get size of the file and allocate a buffer to hold its contents
        std::streampos size = rom.tellg();
        char* buffer = new char[size];

        //Go back to the beginning of the file and fill the buffer
        rom.seekg(0, std::ios::beg);
        rom.read(buffer, size);
        rom.close();

        //Load the ROM into CHIP-8 memory, starts at 0x200
        for(int i = 0; i < size; ++i)
            memory[0x200 + i] = buffer[i];

        delete[] buffer;
    }
}

void Chip8::EmulateCycle()
{   
    //One opcode is 2 bytes long, shift left to make space, OR to merge
    opcode = memory[pc] << 8 | memory[pc + 1]; //Fetch opcode

    switch(opcode & 0xF000) //Decode opcode, pc + 2 to get to next instruction
    {
        case 0x0000:
        switch(opcode & 0x00FF)
        {
            case 0x00E0: //CLS
            {
                memset(gfx, 0, sizeof(gfx));
                pc += 2;
                break;
            }

            case 0x00EE: //RET
            {
                --sp;
                pc = stack[sp];
                pc += 2;
                break;
            }

            default:
            {
                pc += 2;
                break;
            }
        }
        break;

        case 0x1000: //1NNN - JP Address
        {
            pc = opcode & 0x0FFF; //Masks the last 12 bits --> Gives address
            break;
        }
        
        case 0x2000: //2NNN - Call Address
        {
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
            break;
        }

        case 0x3000: //3XNN - Skip next instruction if VX == NN
            {
                uint8_t Vx = (opcode & 0x0F00) >> 8;
                uint8_t val = (opcode & 0x00FF);
                if (registers[Vx] == val)
                    pc += 4;
                else
                    pc += 2;
                break;
            }

        case 0x4000: //4XNN - Skip next instruction if VX != NN
            {
                uint8_t Vx = (opcode & 0x0F00) >> 8;
                uint8_t val = (opcode & 0x00FF);
                if (registers[Vx] != val)
                    pc += 4;
                else
                    pc += 2;
                break;
            }

        case 0x5000: //5XY0 - Skip next instruction if VX == VY
            {
                uint8_t Vx = (opcode & 0x0F00) >> 8;
                uint8_t Vy = (opcode & 0x00F0) >> 4;
                if (registers[Vx] == registers[Vy])
                    pc += 4;
                else
                    pc += 2;
                break;
            }
        
        case 0x6000: //6XNN - Set VX to NN
        {
            uint8_t Vx= (opcode & 0x0F00) >> 8;
            uint8_t val = (opcode & 0x00FF);
            registers[Vx] = val;
            pc += 2;
            break;
        }

        case 0x7000: //7XNN - Add NN to VX (no carry)
        {
            uint8_t Vx = (opcode & 0x0F00) >> 8;
            uint8_t val = (opcode & 0x00FF);
            registers[Vx] += val;
            pc += 2;
            break;
        }

        case 0x8000: //8XY0 
        {
            uint8_t Vx = (opcode & 0x0F00) >> 8;
            uint8_t Vy = (opcode & 0x00F0) >> 4;
            switch(opcode & 0xF00F)
            {
                case 0x8000: //Set VX = VY
                {
                    registers[Vx] = registers[Vy];
                    pc += 2;
                    break;
                }

                case 0x8001: //VX = VX OR VY
                {
                    registers[Vx] |= registers[Vy];
                    pc += 2;
                    break;
                }

                case 0x8002: //VX = VX AND VY
                {
                    registers[Vx] &= registers[Vy];
                    pc += 2;
                    break;
                }

                case 0x8003: //VX = VX XOR VY
                {
                    registers[Vx] ^= registers[Vy];
                    pc += 2;
                    break;
                }

                case 0x8004: //VX += VY, set VF if carry
                {
                    uint16_t sum = registers[Vx] + registers[Vy];
                    if (sum > 0xFF) {
                        registers[0xF] = 1;
                    }
                    else {
                        registers[0xF] = 0;
                    }
                    registers[Vx] = sum & 0xFF;
                    pc += 2;
                    break;
                }

                case 0x8005: //VX - VY, set VF if borrow
                {
                    if (registers[Vx] > registers[Vy]) {
                        registers[0xF] = 1;
                    }
                    else {
                        registers[0xF] = 0;
                    }
                    registers[Vx] -= registers[Vy];
                    pc += 2;
                    break;
                }
                    
                case 0x8006: //Shift VX right by 1, set VF to shifted bit
                {
                    registers[0xF] = registers[Vx] & 0x1;
                    registers[Vx] >>= 1;
                    pc += 2;
                    break;
                }

                case 0x8007: //VX = VY - VX, set VF if borrow
                {
                    if (registers[Vy] > registers[Vx]) {
                        registers[0xF] = 0;
                    }
                    else {
                        registers[0xF] = 1;
                    }
                    registers[Vx] = registers[Vy] - registers[Vx];
                    pc += 2;
                    break;
                }

                case 0x800E: //Shift VX left by 1, set VF to shifted bit
                {
                    registers[0xF] = (registers[Vx] & 0x80) >> 7;
                    registers[Vx] <<= 1;
                    pc += 2;
                    break;
                }

                default:
                    pc += 2;
                    break;
            }
            break;
        }

        case 0x9000: //9XY0 - Skip next instruction if VX != VY
        {
            uint8_t Vx = (opcode & 0x0F00) >> 8;
            uint8_t Vy = (opcode & 0x00F0) >> 4;
            if (registers[Vx] != registers[Vy])
                pc += 4;
            else
                pc += 2;
            break;
        }

        case 0xA000: //AXNN - Set Index Register to NNN
        {
            uint16_t address = (opcode & 0x0FFF);
            I = address;
            pc += 2;
            break;
        }

        case 0xD000: 
        {
            uint8_t x = registers[(opcode & 0x0F00) >> 8];
            uint8_t y = registers[(opcode & 0x00F0) >> 4];
            uint8_t height = opcode & 0x000F;

            registers[0xF] = 0; //Reset collision register

            for (int row = 0; row < height; ++row) {
                uint8_t pixel = memory[I + row];
                for (int col = 0; col < 8; ++col) {
                    if ((pixel & (0x80 >> col)) != 0) { //Check if pixel is on
                        //Screen wrapping
                        int px = (x + col) % 64; 
                        int py = (y + row) % 32;
                        int index = px + py * 64;

                        if (gfx[index] == 1) {
                            registers[0xF] = 1;
                        }
                        gfx[index] ^= 1; //Draw using XOR
                    }
                }
            }

            pc += 2;
            break;
        }

        default:{
            pc += 2;
            break;
        }
    }
        
    //Update timers
    if(delayTimer > 0)
        --delayTimer;
    
    if(soundTimer > 0)
        {
            if(soundTimer == 1)
                //Will add sound logic
                printf("Beep\n");
            --soundTimer;
        }
}
