#include "Window.h"
#include <iostream>

int main() {
    // Window (Wrapper around SDL)
    Window win = Window();
    
    // Screen Buffer
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
            SDL_Delay(17 - time_elapsed);
        }
        time_start = SDL_GetTicks();
    }

    return 0;
}
