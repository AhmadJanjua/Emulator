#include "Chip8.h"

Chip8::Chip8(const char* fpath) {
    // open file to end
    std::ifstream istream(fpath, std::ios::binary | std::ios::ate);

    if (!istream) {
        std::cerr << "Error: Failed to open file " << fpath << "\n";
        throw -2;
    }

    // get file size
    std::streamsize file_size = istream.tellg();

    if (file_size <= 0) {
        std::cerr << "Error: File is empty or unreadable\n";
        throw -3;
    }

    if (file_size > static_cast<std::streamsize>(sizeof(memory) - 0x200)) {
        std::cerr << "Error: File size (" << file_size <<") too large to fit into memory\n";
        throw -4;
    }

    // go to beginning
    istream.seekg(0, std::ios::beg);

    // Read directly into memory starting from 0x200
    istream.read(reinterpret_cast<char*>(&memory[0x200]), file_size);

    if (!istream) {
        std::cerr << "Error: Failed to read entire file\n";
        throw -5;
    }

    // Reset PC
    pc = 0x200;
}

Chip8::~Chip8() {}

void Chip8::memory_dump() {
    for (int i=0; i<4096; i+=2) {
        uint16_t instr = memory[i]<<8|memory[i+1];
        if (instr != 0)
            printf("block [%d]: %x\n", i, instr);
    }
}