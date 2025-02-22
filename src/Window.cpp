#include "Window.h"

Window::Window() {
    if (!this->init_sdl()) {
        this->close_sdl();
        throw -1;
    }
}

Window::~Window() {
    this->close_sdl();
}

bool Window::init_sdl() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Unable to initialize SDL. Error: " << SDL_GetError() << "\n";
        return false;
    }

    // Create Window
    window = SDL_CreateWindow(
        WIN_NAME,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_WIDTH, WIN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Unable to create window. Error: " << SDL_GetError() << "\n";
        return false;
    }

    SDL_SetWindowResizable(window, SDL_FALSE);

    // Create Renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Unable to create renderer. Error: " << SDL_GetError() << "\n";
        return false;
    }

    // Create Texture
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB332,         // -- NOTE: 8 bit pixel format
        SDL_TEXTUREACCESS_STREAMING,
        BUF_WIDTH, BUF_HEIGHT
    );

    if (!texture) {
        std::cerr << "Unable to create texture. Error: " << SDL_GetError() << "\n";
        return false;
    }

    return true;
}

void Window::close_sdl() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}

void Window::update_pixels(uint8_t pixel_buffer[BUF_HEIGHT][BUF_WIDTH]) {
    void* pixels;
    int pitch;

    // Lock texture
    if (SDL_LockTexture(texture, NULL, &pixels, &pitch) < 0) {
        std::cerr << "Unable to lock texture. Error: " << SDL_GetError() << "\n";
        return;
    }

    // Update texture
    uint8_t* dst = (uint8_t*)pixels;
    for (int y = 0; y < BUF_HEIGHT; y++) {
        int offset = y * (pitch / sizeof(uint8_t));
        for (int x = 0; x < BUF_WIDTH; x++) {
            dst[offset + x] = pixel_buffer[y][x];
        }
    }

    // Unlock texture
    SDL_UnlockTexture(texture);
}

void Window::set_pixel(int x, int y, bool on) {
    void* pixels;
    int pitch;

    // Lock texture
    if (SDL_LockTexture(texture, NULL, &pixels, &pitch) < 0) {
        std::cerr << "Unable to lock texture. Error: " << SDL_GetError() << "\n";
        return;
    }
    
    // Update texture
    uint8_t* dst = (uint8_t*)pixels;
    int offset = y * (pitch / sizeof(uint8_t));

    if (on) {
        dst[offset + x] = ON;
    } else {
        dst[offset + x] = OFF;
    }

    // Unlock texture
    SDL_UnlockTexture(texture);
}

void Window::clear_pixels() {
    void* pixels;
    int pitch;

    // Lock texture
    if (SDL_LockTexture(texture, NULL, &pixels, &pitch) < 0) {
        std::cerr << "Unable to lock texture. Error: " << SDL_GetError() << "\n";
        return;
    }

    // Update texture
    uint8_t* dst = (uint8_t*)pixels;
    for (int y = 0; y < BUF_HEIGHT; y++) {
        int offset = y * (pitch / sizeof(uint8_t));
        for (int x = 0; x < BUF_WIDTH; x++) {
            dst[offset + x] = OFF;
        }
    }

    // Unlock texture
    SDL_UnlockTexture(texture);

}

void Window::render() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, &rect_display);
    SDL_RenderPresent(renderer);
}