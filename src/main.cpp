#include "Window.h"
#include "Chip8.h"
#include <iostream>

int main(int argc, char* argv[]) {
    // Setup arguments and usage
    if (argc != 2) {
        std::cout << "Usage: please supply rom <filename>" << std::endl;
        return 1;
    }

    Chip8 chip(argv[1]);

    chip.memory_dump();
    return 0;
    // Window (Wrapper around SDL)
    Window win = Window();
    
    // Screen Buffer: transposed for better caching
    uint8_t pixel_buffer[BUF_HEIGHT][BUF_WIDTH] = { 0 };
    bool swap = false;
    
    // Main Loop
    bool running = true;
    uint32_t time_start = SDL_GetTicks();
    uint32_t time_elapsed = SDL_GetTicks();

    SDL_Event e;
    while (running) {
        // Poll Events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }

        // Draw
        swap = !swap;
        for (int y = 0; y < BUF_HEIGHT; y++) {
            for (int x = 0; x < BUF_WIDTH; x++) {
                if ((x + y + swap) % 2 == 0)
                    pixel_buffer[y][x] = ON;
                else
                    pixel_buffer[y][x] = OFF;
            }
        }
    
         // Update
        win.update_pixels(pixel_buffer);
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
