#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include "Window.h"

class Chip8 {
private:
    Window& window;
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

// -- instructions
    // Code 0x0
    void cls0();
    void ret0();
    void sys0(uint16_t addr);

    // Code 0x1
    void jp1(uint16_t addr);

    // Code 0x2
    void call2(uint16_t addr);

    // Code 0x3
    void se3(uint8_t vx, uint8_t byte);

    // Code 0x4
    void sne4(uint8_t vx, uint8_t byte);

    // Code 0x5
    void se5(uint8_t vx, uint8_t vy);

    // Code 0x6
    void ld6(uint8_t vx, uint8_t byte);

    // Code 0x7
    void add7(uint8_t vx, uint8_t byte);

    // Code 0x8
    void ld8(uint8_t vx, uint8_t vy);
    void or8(uint8_t vx, uint8_t vy);
    void and8(uint8_t vx, uint8_t vy);
    void xor8(uint8_t vx, uint8_t vy);
    void add8(uint8_t vx, uint8_t vy);
    void sub8(uint8_t vx, uint8_t vy);
    void shr8(uint8_t vx, uint8_t vy);
    void shl8(uint8_t vx, uint8_t vy);
    void subn8(uint8_t vx, uint8_t vy);

    // Code 0x9
    void sne9(uint8_t vx, uint8_t vy);

    // Code 0xA
    void ldA(uint16_t addr);

    // Code 0xB
    void jpB(uint16_t addr);

    // Code 0xC
    void rndC(uint8_t vx, uint8_t byte);

    // Code 0xD
    void drwD(uint8_t vx, uint8_t vy, uint8_t nibble);

    // Code 0xE
    void skpE(uint8_t vx);
    void sknpE(uint8_t vx);

    // Code 0xF
    void ldF_7(uint8_t vx);
    void ldF_A(uint8_t vx);
    void ldF_15(uint8_t vx);
    void ldF_18(uint8_t vx);
    void ldF_1E(uint8_t vx);
    void ldF_29(uint8_t vx);
    void ldF_33(uint8_t vx);
    void ldF_55(uint8_t vx);
    void ldF_65(uint8_t vx);

    void run_instr(uint16_t instr);
public:
    Chip8(Window& w, const char* fpath);
    ~Chip8();

    void memory_dump();

    void run();
};

