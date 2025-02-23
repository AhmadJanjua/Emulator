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
    uint64_t time_start = SDL_GetTicks64();

    while (win.running) {
        // Poll Events
        win.poll();

        // run
        chip.run(false);
        
        // Run at render at 60 fps
        if (SDL_GetTicks() - time_start >= 16) {
            // reset the time + force render to be counted
            time_start = SDL_GetTicks();

            // run and tick the timer
            chip.run(true);

            // update display
            win.render();
        }
    }

    return 0;
}
