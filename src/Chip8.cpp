#include <cstdlib>
#include "Chip8.h"

Chip8::Chip8(Window& w, uint64_t& t, const char* fpath) : window(w), time(t) {
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

        if (SDL_GetTicks64() - time >= 16) {
            if (reg_t > 0) {
                reg_t--;
            }
            if (reg_s > 0) {
                reg_s--;
            }
            time = SDL_GetTicks64();
            window.render();
        }
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
        if ((instr & 0xF) == 0) {
            return se5((instr >> 8) & 0xF, (instr >> 4) & 0xF);
        }
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
    std::cout << "ignoring sys jump to:" << std::hex << addr << std::endl;
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
    stack[sp] = pc;
    sp++;
    pc = addr;
}

// 0x3
void Chip8::se3(uint8_t vx, uint8_t byte) {
    if (reg_v[vx] == byte) {
        pc += 2;
    }
}

// 0x4
void Chip8::sne4(uint8_t vx, uint8_t byte) {
    if (reg_v[vx] != byte) {
        pc += 2;
    }
}

// 0x5
void Chip8::se5(uint8_t vx, uint8_t vy) {
    if (reg_v[vx] == reg_v[vy]) {
        pc += 2;
    }
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
    reg_v[0xF] = 0;
}

void Chip8::and8(uint8_t vx, uint8_t vy) {
    reg_v[vx] &= reg_v[vy];
    reg_v[0xF] = 0;

}

void Chip8::xor8(uint8_t vx, uint8_t vy) {
    reg_v[vx] ^= reg_v[vy];
    reg_v[0xF] = 0;
}

void Chip8::add8(uint8_t vx, uint8_t vy) {
    // compute sum in 16-bit
    uint16_t sum = reg_v[vx] + reg_v[vy];

    // flag overflow
    if (sum > 0xFF)
        reg_v[0xF] = 1;
    else
        reg_v[0xF] = 0;

    // do not update Vf
    if (vx != 0xF)
        // only keep 8-lower bits
        reg_v[vx] = (sum & 0xFF);

}

void Chip8::sub8(uint8_t vx, uint8_t vy) {
    uint8_t diff = reg_v[vx] - reg_v[vy];

    // flag underflow
    if (reg_v[vx] >= reg_v[vy])
        reg_v[0xF] = 1;
    else
        reg_v[0xF] = 0;

    // do not update Vf
    if (vx != 0xF)
        reg_v[vx] = diff;
}

// shift right
void Chip8::shr8(uint8_t vx, uint8_t vy) {
    // set Vf to the least significant bit
    reg_v[0xF] = reg_v[vy] & 0b1;

    // update if not Vf
    if (vx != 0xF)
        reg_v[vx] = reg_v[vy] >> 1;
}

// note: similar so sub8 however vy and vx order is swapped
void Chip8::subn8(uint8_t vx, uint8_t vy) {
    uint8_t diff = reg_v[vy] - reg_v[vx];

    // flag underflow
    if (reg_v[vx] <= reg_v[vy])
        reg_v[0xF] = 1;
    else
        reg_v[0xF] = 0;

    // set if not Vf
    if (vx != 0xF)
        reg_v[vx] = diff;
}

// shift left
void Chip8::shl8(uint8_t vx, uint8_t vy) {
    // store most significant bit in Vf
    reg_v[0xF] = (reg_v[vy] >> 7) & 0b1;

    // update if not Vf
    if (vx != 0xF)
        reg_v[vx] = reg_v[vy] << 1;
}

// 0x9
void Chip8::sne9(uint8_t vx, uint8_t vy) {
    if (reg_v[vx] != reg_v[vy]) {
        pc += 2;
    }
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
    // Clear collision flag.
    reg_v[0xF] = 0;

    // Calculate starting coordinates (wrap around)
    uint8_t x0 = reg_v[vx] % 64;
    uint8_t y0 = reg_v[vy] % 32;

    // Loop over each row of the sprite
    for (int i = 0; i < nibble; i++) {
        uint8_t byte = memory[reg_i + i];
        uint8_t y = (y0 + i);

        // dont render if the sprite is clipping
        if (y > 31) {
            break;
        }

        // Loop over each bit in the byte.
        for (int j = 7; j >= 0; j--) {
            // Compute the pixel's x and y positions.
            uint8_t x = (x0 + (7 - j));

            // don't render if the sprite is clipping
            if (x > 63) {
                break;
            }

            // get the new and old bit and check the results
            bool bit = (byte >> j) & 0x1;
            bool old_bit = window.get_pixel(x, y);
            bool result = bit ^ old_bit;

            // If the pixel was turned off by the XOR, set the collision flag.
            if (!result && old_bit) {
                reg_v[0xF] = 1;
            }

            // update pixel
            window.set_pixel(x, y, result);
        }
    }
}

// 0xE
void Chip8::skpE(uint8_t vx) {
    if (window.get_key_press(reg_v[vx])) {
        pc += 2;
    }
}

void Chip8::sknpE(uint8_t vx) {
    if (!window.get_key_press(reg_v[vx])) {
        pc += 2;
    }
}

// 0xF
void Chip8::ldF_7(uint8_t vx) {
    reg_v[vx] = reg_t;
}

void Chip8::ldF_A(uint8_t vx) {
    uint8_t key = 0xFE;
    while (key > 0xF) {
        // continue updating
        if (SDL_GetTicks64() - time >= 16) {
            if (reg_t > 0) {
                reg_t--;
            }
            if (reg_s > 0) {
                reg_s--;
            }
            time = SDL_GetTicks64();
            window.render();
        }

        // wait to get a key press
        key = window.await_keypress();

        // exit has been called
        if (key == 0xFF) {
            return;
        }
    }
    reg_v[vx] = key;
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

// load register to memory
void Chip8::ldF_55(uint8_t vx) {
    // load Vx into memory
    for (int i = 0; i <= vx; i++) {
        memory[reg_i] = reg_v[i];
        reg_i++;
    }
}

// load register from memory
void Chip8::ldF_65(uint8_t vx) {
    for (int i = 0; i <= vx; i++) {
        reg_v[i] = memory[reg_i];
        reg_i++;
    }
}
