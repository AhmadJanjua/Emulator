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
    Chip8 chip(win, argv[1]);

    // Main Loop
    uint32_t time_start = SDL_GetTicks();
    uint32_t time_elapsed = SDL_GetTicks();

    while (win.running) {
        // Poll Events
        win.poll();

        // run
        chip.run();
        win.render();

        // Run at 60 fps
        time_elapsed = SDL_GetTicks() - time_start;
        if (time_elapsed < 16) {
            SDL_Delay(16 - time_elapsed);
        }
        time_start = SDL_GetTicks();
    }

    return 0;
}
