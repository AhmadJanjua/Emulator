#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>

class Chip8 {
private:
// -- Memory
    // 16-bit program counter
    uint16_t pc = 0x200;
    // 4096 bytes of memory: 0x0-0xFFF (address space): 0x0-0x1FF (reserved)
    uint8_t memory[4096] = {
        // 0
        0b11110000,
        0b10010000,
        0b10010000,
        0b10010000,
        0b11110000,
        // 1
        0b00100000,
        0b01100000,
        0b00100000,
        0b00100000,
        0b01110000,
        // 2
        0b11110000,
        0b00010000,
        0b11110000,
        0b10000000,
        0b11110000,
        // 3
        0b11110000,
        0b00010000,
        0b11110000,
        0b00010000,
        0b11110000,
        // 4
        0b10010000,
        0b10010000,
        0b11110000,
        0b00010000,
        0b00010000,
        // 5
        0b11110000,
        0b10000000,
        0b11110000,
        0b00010000,
        0b11110000,
        // 6
        0b11110000,
        0b10000000,
        0b11110000,
        0b10010000,
        0b11110000,
        // 7
        0b11110000,
        0b00010000,
        0b00100000,
        0b01000000,
        0b01000000,
        // 8
        0b11110000,
        0b10010000,
        0b11110000,
        0b10010000,
        0b11110000,
        // 9
        0b11110000,
        0b10010000,
        0b11110000,
        0b00010000,
        0b00010000,
        // A
        0b11110000,
        0b10010000,
        0b11110000,
        0b10010000,
        0b10010000,
        // B
        0b11100000,
        0b10010000,
        0b11100000,
        0b10010000,
        0b11100000,
        // C
        0b11110000,
        0b10000000,
        0b10000000,
        0b10000000,
        0b11110000,
        // D
        0b11100000,
        0b10010000,
        0b10010000,
        0b10010000,
        0b11100000,
        // E
        0b11110000,
        0b10000000,
        0b11110000,
        0b10000000,
        0b11110000,
        // F
        0b11110000,
        0b10000000,
        0b11110000,
        0b10000000,
        0b10000000,
        0
    };

// -- Stack
    // 8-bit stack pointer 
    uint8_t sp = 0;
    // 16 16-bit stack spaces
    uint16_t stack[16] = { 0 };

// -- Registers
    // 16 8-bit general purpose registers; V[0xF] reserved by instruction set
    uint8_t reg_v[16] = { 0 };
    
    // 16-bit memory address index register (need 12-bits)
    uint16_t reg_i = 0;
    
    // two special registers for timer and sound
    uint8_t reg_t = 0;
    uint8_t reg_s = 0;

public:
    Chip8(const char* fpath);
    ~Chip8();

    void memory_dump();
};