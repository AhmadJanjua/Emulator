#include <cstdlib>
#include "Chip8.h"

Chip8::Chip8(Window& w, const char* fpath) : window(w) {
    // open file to end
    std::ifstream istream(fpath, std::ios::binary | std::ios::ate);

    if (!istream) {
        std::cerr << "Error: Failed to open file " << fpath << "\n";
        throw - 2;
    }

    // get file size
    std::streamsize file_size = istream.tellg();

    if (file_size <= 0) {
        std::cerr << "Error: File is empty or unreadable\n";
        throw - 3;
    }

    if (file_size > static_cast<std::streamsize>(sizeof(memory) - 0x200)) {
        std::cerr << "Error: File size (" << file_size << ") too large to fit into memory\n";
        throw - 4;
    }

    // go to beginning
    istream.seekg(0, std::ios::beg);

    // Read directly into memory starting from 0x200
    istream.read(reinterpret_cast<char*>(&memory[0x200]), file_size);

    if (!istream) {
        std::cerr << "Error: Failed to read entire file\n";
        throw - 5;
    }

    // Reset PC
    pc = 0x200;
}

Chip8::~Chip8() {}

void Chip8::memory_dump() {
    for (int i = 0; i < 4096; i++) {
        if (i % 16 == 0)
            std::cout << "\n0x" << std::hex << i << ": ";

        if (i % 2 == 0)
            std::cout << std::hex << int(memory[i]) << " ";
        else
            std::cout << std::hex << int(memory[i]) << "\t";
    }
    std::cout << std::dec << std::endl;
}

void Chip8::run() {
    if (pc < 4096) {
        // get the instruction
        uint16_t instr = memory[pc] << 8 | memory[pc + 1];

        // auto increment the program counter
        pc += 2;

        run_instr(instr);
    }
}

void Chip8::run_instr(uint16_t instr) {
    switch (instr >> 12) {
    case 0x0:
        switch (instr) {
        case 0x00E0: return cls0();
        case 0x00EE: return ret0();
        default: return sys0(instr & 0xFFF);
        }
    case 0x1: return jp1(instr & 0xFFF);
    case 0x2: return call2(instr & 0xFFF);
    case 0x3: return se3((instr >> 8) & 0xF, instr & 0xFF);
    case 0x4: return sne4((instr >> 8) & 0xF, instr & 0xFF);
    case 0x5:
        if ((instr & 0xF) == 0) return se5((instr >> 8) & 0xF, (instr >> 4) & 0xF);
        break;
    case 0x6: return ld6((instr >> 8) & 0xF, instr & 0xFF);
    case 0x7: return add7((instr >> 8) & 0xF, instr & 0xFF);
    case 0x8:
        switch (instr & 0xF) {
        case 0x0: return ld8((instr >> 8) & 0xF, (instr >> 4) & 0xF);
        case 0x1: return or8((instr >> 8) & 0xF, (instr >> 4) & 0xF);
        case 0x2: return and8((instr >> 8) & 0xF, (instr >> 4) & 0xF);
        case 0x3: return xor8((instr >> 8) & 0xF, (instr >> 4) & 0xF);
        case 0x4: return add8((instr >> 8) & 0xF, (instr >> 4) & 0xF);
        case 0x5: return sub8((instr >> 8) & 0xF, (instr >> 4) & 0xF);
        case 0x6: return shr8((instr >> 8) & 0xF, (instr >> 4) & 0xF);
        case 0x7: return subn8((instr >> 8) & 0xF, (instr >> 4) & 0xF);
        case 0xE: return shl8((instr >> 8) & 0xF, (instr >> 4) & 0xF);
        }
        break;
    case 0x9:
        if ((instr & 0xF) == 0) return sne9((instr >> 8) & 0xF, (instr >> 4) & 0xF);
        break;
    case 0xA: return ldA(instr & 0xFFF);
    case 0xB: return jpB(instr & 0xFFF);
    case 0xC: return rndC((instr >> 8) & 0xF, instr & 0xFF);
    case 0xD: return drwD((instr >> 8) & 0xF, (instr >> 4) & 0xF, instr & 0xF);
    case 0xE:
        switch (instr & 0xFF) {
        case 0x9E: return skpE((instr >> 8) & 0xF);
        case 0xA1: return sknpE((instr >> 8) & 0xF);
        }
        break;
    case 0xF:
        switch (instr & 0xFF) {
        case 0x7: return ldF_7((instr >> 8) & 0xF);
        case 0xA: return ldF_A((instr >> 8) & 0xF);
        case 0x15: return ldF_15((instr >> 8) & 0xF);
        case 0x18: return ldF_18((instr >> 8) & 0xF);
        case 0x1E: return ldF_1E((instr >> 8) & 0xF);
        case 0x29: return ldF_29((instr >> 8) & 0xF);
        case 0x33: return ldF_33((instr >> 8) & 0xF);
        case 0x55: return ldF_55((instr >> 8) & 0xF);
        case 0x65: return ldF_65((instr >> 8) & 0xF);
        }
        break;
    }

    std::cerr << "Unknown instruction: 0x" << std::hex << instr << "\n";
}

// -- Instructions

// 0x0
void Chip8::cls0() {
    window.clear_pixels();
}

void Chip8::ret0() {
    if (sp == 0) {
        std::cerr << "Stack Underflow Error in ret0()" << std::endl;
        return;
    }
    sp--;
    pc = stack[sp];
}

void Chip8::sys0(uint16_t addr) {
    std::cout << std::hex << addr << std::endl;
}

// 0x1
void Chip8::jp1(uint16_t addr) {
    pc = addr;
}

// 0x2
void Chip8::call2(uint16_t addr) {
    if (sp >= 16) {
        std::cerr << "Stack Overflow in call2()\n";
        return;
    }
    stack[sp++] = pc;
    pc = addr;
}

// 0x3
void Chip8::se3(uint8_t vx, uint8_t byte) {
    if (reg_v[vx] == byte) pc += 2;
}

// 0x4
void Chip8::sne4(uint8_t vx, uint8_t byte) {
    if (reg_v[vx] != byte) pc += 2;
}

// 0x5
void Chip8::se5(uint8_t vx, uint8_t vy) {
    if (reg_v[vx] == reg_v[vy]) pc += 2;
}

// 0x6
void Chip8::ld6(uint8_t vx, uint8_t byte) {
    reg_v[vx] = byte;
}

// 0x7
void Chip8::add7(uint8_t vx, uint8_t byte) {
    reg_v[vx] += byte;
}

// 0x8
void Chip8::ld8(uint8_t vx, uint8_t vy) {
    reg_v[vx] = reg_v[vy];
}

void Chip8::or8(uint8_t vx, uint8_t vy) {
    reg_v[vx] |= reg_v[vy];
}

void Chip8::and8(uint8_t vx, uint8_t vy) {
    reg_v[vx] &= reg_v[vy];
}

void Chip8::xor8(uint8_t vx, uint8_t vy) {
    reg_v[vx] ^= reg_v[vy];
}

void Chip8::add8(uint8_t vx, uint8_t vy) {
    uint16_t sum = reg_v[vx] + reg_v[vy];
    reg_v[0xF] = (sum > 0xFF) ? 1 : 0;
    reg_v[vx] = sum & 0xFF;
}

void Chip8::sub8(uint8_t vx, uint8_t vy) {
    reg_v[0xF] = (reg_v[vx] >= reg_v[vy]) ? 1 : 0;
    reg_v[vx] -= reg_v[vy];
}

void Chip8::shr8(uint8_t vx, uint8_t vy) {
    reg_v[0xF] = reg_v[vy] & 0x1;
    reg_v[vx] = reg_v[vy] >> 1;
}

void Chip8::subn8(uint8_t vx, uint8_t vy) {
    reg_v[0xF] = (reg_v[vy] >= reg_v[vx]) ? 1 : 0;
    reg_v[vx] = reg_v[vy] - reg_v[vx];
}

void Chip8::shl8(uint8_t vx, uint8_t vy) {
    reg_v[0xF] = (reg_v[vy] >> 7) & 0x1;
    reg_v[vx] = reg_v[vy] << 1;
}

// 0x9
void Chip8::sne9(uint8_t vx, uint8_t vy) {
    if (reg_v[vx] != reg_v[vy]) pc += 2;
}

// 0xA
void Chip8::ldA(uint16_t addr) {
    reg_i = addr;
}

// 0xB
void Chip8::jpB(uint16_t addr) {
    pc = addr + reg_v[0];
}

// 0xC
void Chip8::rndC(uint8_t vx, uint8_t byte) {
    reg_v[vx] = (rand() % 256) & byte;
}

// 0xD
void Chip8::drwD(uint8_t vx, uint8_t vy, uint8_t nibble) {
    uint8_t x = reg_v[vx] % 64;
    uint8_t y = reg_v[vy] % 32;

    reg_v[0xF] = 0;

    for (int i = 0; i < nibble; i++) {
        uint8_t pixel = memory[reg_i + i];
        for (int j = 0; j < 8; j++) {
            if ((pixel & (0x80 >> j)) != 0) {
                if (window.get_pixel(x + j, y + i)) {
                    reg_v[0xF] = 1;
                }
                window.set_pixel(x + j, y + i, !window.get_pixel(x + j, y + i));
            }
        }
    }
    window.update_pixels();
}

// 0xE
void Chip8::skpE(uint8_t vx) {
    if (window.get_key_press(vx)) {
        pc += 2;
    }
}

void Chip8::sknpE(uint8_t vx) {
    if (!window.get_key_press(vx)) {
        pc += 2;
    }
}

// 0xF
void Chip8::ldF_7(uint8_t vx) {
    reg_v[vx] = reg_t;
}

void Chip8::ldF_A(uint8_t vx) {
    window.render();
    reg_v[vx] = window.await_keypress();
}

void Chip8::ldF_15(uint8_t vx) {
    reg_t = reg_v[vx];
}

void Chip8::ldF_18(uint8_t vx) {
    reg_s = reg_v[vx];
}

void Chip8::ldF_1E(uint8_t vx) {
    reg_i += reg_v[vx];
}

void Chip8::ldF_29(uint8_t vx) {
    reg_i = 5 * reg_v[vx];
}

void Chip8::ldF_33(uint8_t vx) {
    uint8_t value = reg_v[vx];
    memory[reg_i] = value / 100;
    memory[reg_i + 1] = (value / 10) % 10;
    memory[reg_i + 2] = value % 10;
}

void Chip8::ldF_55(uint8_t vx) {
    for (uint8_t i = 0; i <= vx; i++) {
        memory[reg_i + i] = reg_v[i];
    }
}

void Chip8::ldF_65(uint8_t vx) {
    for (uint8_t i = 0; i <= vx; i++) {
        reg_v[i] = memory[reg_i + i];
    }
}
