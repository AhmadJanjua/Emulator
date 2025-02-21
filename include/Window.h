#include <SDL2/SDL.h>
#include <iostream>

#define WIN_NAME "Chip-8 Emulator"
#define WIN_WIDTH 1024
#define WIN_HEIGHT 512
#define BUF_WIDTH 64
#define BUF_HEIGHT 32

#define ON 0xFF
#define OFF 0x00

class Window {
private:
//  -- Variables
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    SDL_Rect rect_display = { 0, 0, WIN_WIDTH, WIN_HEIGHT };

//  -- Helper functions
    // Initializes and reports errors for setting up SDL
    bool init_sdl();

    // Free up SDL resources
    void close_sdl();

public:
//  -- Ctor/dtor
    Window();
    ~Window();

//  -- Functions
    // Replace the pixels on screen with a new buffer
    void update_pixels(uint8_t pixel_buffer[BUF_HEIGHT][BUF_WIDTH]);
    
    // Render the current state of the texture on to the screen
    void render();

    // poll events
    int poll();
};