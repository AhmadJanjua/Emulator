#pragma once

#include <SDL2/SDL.h>
#include <unordered_map>
#include <iostream>

#define WIN_NAME "Chip-8 Emulator"
#define WIN_WIDTH 1024
#define WIN_HEIGHT 512
#define BUF_WIDTH 64
#define BUF_HEIGHT 32

#define ON 0xFC
#define OFF 0x13

typedef std::unordered_map<SDL_Keycode, uint8_t> Key_Lut;

class Window {
private:
// -- Variables
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    SDL_Rect rect_display = { 0, 0, WIN_WIDTH, WIN_HEIGHT };
    SDL_Event event;

    uint64_t prev_render;
    uint8_t* pixel_buffer = nullptr;
    bool key_pressed[16] = { 0 };

    const Key_Lut KEY_MAP = {
        { SDLK_1, 0x1 }, { SDLK_2, 0x2 }, { SDLK_3, 0x3 }, { SDLK_4, 0xC },
        { SDLK_q, 0x4 }, { SDLK_w, 0x5 }, { SDLK_e, 0x6 }, { SDLK_r, 0xD },
        { SDLK_a, 0x7 }, { SDLK_s, 0x8 }, { SDLK_d, 0x9 }, { SDLK_f, 0xE },
        { SDLK_z, 0xA }, { SDLK_x, 0x0 }, { SDLK_c, 0xB }, { SDLK_v, 0xF }
    };


// -- Helper functions
    // Initializes and reports errors for setting up SDL
    bool init_sdl();

    // Free up SDL resources
    void close_sdl();

public:
    bool running = true;

// -- Ctor/dtor
    Window();
    ~Window();

// -- Functions
    // Turn a single pixel on or off
    void set_pixel(int x, int y, bool on);

    // Check the state of a pixel
    bool get_pixel(int x, int y);

    // Clear the entire screen
    void clear_pixels();

    // Render the current state of the texture on to the screen
    void render();

    // poll events
    void poll();
    uint8_t await_keypress();

    // check if key has been pressed
    bool get_key_press(int idx);
    void reset_key_press(int idx);
};