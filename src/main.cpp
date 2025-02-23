#include "Window.h"
#include "Chip8.h"
#include <iostream>

int main(int argc, char* argv[]) {
    // Setup arguments and usage
    if (argc != 2) {
        std::cout << "Usage: please supply rom <filename>" << std::endl;
        return 1;
    }

    // Window (Wrapper around SDL)
    Window win = Window();
    uint64_t time_start = SDL_GetTicks64();
    Chip8 chip(win, time_start, argv[1]);

    while (win.running) {
        // Poll Events
        win.poll();

        // run
        chip.run();

        // run at 500 khz
        while (SDL_GetTicks64() - time_start <= 1) {}
    }

    return 0;
}
