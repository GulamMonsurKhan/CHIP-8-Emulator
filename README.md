# CHIP-8 Emulator in C++

A simple CHIP-8 emulator written in C++ that emulates the original CHIP-8 virtual machine. CHIP-8 is an interpreted programming language designed in the 1970s for simple microcomputers and used in early games and applications. This emulator simulates the CPU, graphics, input handling, and audio functionalities of the original machine.

## Features

- [x] **CPU Emulation**: Implements the core CHIP-8 instruction set and handles all CPU operations.
- [x] **Graphics**: Uses SDL2 for rendering 64x32 pixel monochrome display.
- [x] **Input**: Supports keyboard input through SDL2, allowing the user to play CHIP-8 games.
- [ ] **Sound**: Implements basic sound functionality (beeps) using SDL2.
- [x] **ROM Loading**: Loads and runs CHIP-8 games and programs stored in `.ch8` or `.rom` files.

## Prerequisites

Before building the emulator, ensure you have the following dependencies installed:

- **C++17 (or newer)** compiler
- **SDL2** library (for graphics, input, and sound)
  - SDL2 can be installed through most package managers, or you can build it from source.
  
For SDL2:
- On macOS, you can install it via Homebrew:
  ```bash
  brew install sdl2

## Usage

- Compile with
 ```bash
cmake -S . -B build
cmake --build build
./build/chip8 rom.ch8
```
There are plenty of CHIP-8 ROMs online. I have not included any in this project

### Controls
| CHIP-8 Keypad | Computer Keyboard Key |
| ------------- | --------------------- |
| 1             | 1                     |
| 2             | 2                     |
| 3             | 3                     |
| 4             | 4                     |
| Q             | Q                     |
| W             | W                     |
| E             | E                     |
| R             | R                     |
| A             | A                     |
| S             | S                     |
| D             | D                     |
| F             | F                     |
| Z             | Z                     |
| X             | X                     |
| C             | C                     |
| V             | V                     |

## Architecture

### CPU Emulation

The CPU emulation in this CHIP-8 emulator includes the following components:

- **Registers**: 16 general-purpose registers (`V0` to `VF`), with the `VF` register also serving as a flag for certain operations (e.g., carry flag for addition).
- **Program Counter (PC)**: The 16-bit Program Counter keeps track of the current instruction.
- **Index Register (I)**: A 16-bit register used for memory addresses, mainly for sprite drawing and indirect memory operations.
- **Stack**: A stack to manage subroutine calls and returns.
- **Timers**: Two 8-bit timers: the delay timer and the sound timer. These timers decrement every frame and trigger events when they reach zero.

The CHIP-8 virtual machine is capable of executing 35 opcodes. These opcodes are decoded and executed by the CPU, which is the heart of the emulator. The emulator fetches, decodes, and executes instructions at a fast rate, simulating the original behavior of the CHIP-8.

The CPU also manages the **opcode execution cycle**, which updates the display, processes input, and handles timers.

### Graphics

The display in the CHIP-8 emulator is 64x32 pixels, and each pixel can either be on or off (monochrome). The emulator uses **SDL2** for rendering the graphics.

- **Drawing Sprites**: The `DXYN` instruction is responsible for drawing sprites on the screen. The sprites are stored in memory as a sequence of bytes. Each byte represents 8 pixels of a row, where each bit in the byte corresponds to a pixel (on or off).
- **Clearing the Screen**: The `00E0` instruction clears the entire screen (resets all pixels to off).
- **Pixel Manipulation**: If a sprite is drawn that overlaps existing pixels, those pixels are flipped (if they were on, they turn off, and vice versa).

The graphics are rendered to the screen using **SDL2's rendering system**, which allows us to display monochrome pixels on the screen.

