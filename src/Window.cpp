#include "Window.h"

Window::Window() {
    if (!this->init_sdl()) {
        this->close_sdl();
        throw - 1;
    }

    // turn off all the pixels
    memset(pixel_buffer, static_cast<uint8_t>(OFF), sizeof(pixel_buffer));
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
        SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Unable to create window. Error: " << SDL_GetError() << "\n";
        return false;
    }

    // Create Renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Unable to create renderer. Error: " << SDL_GetError() << "\n";
        return false;
    }

    // Create Texture
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB332, // -- NOTE: 8 bit pixel format
        SDL_TEXTUREACCESS_STREAMING,
        BUF_WIDTH, BUF_HEIGHT);

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

bool Window::get_pixel(int x, int y) {
    return pixel_buffer[y * BUF_WIDTH + x] != OFF;
}

void Window::set_pixel(int x, int y, bool on) {
    pixel_buffer[y * BUF_WIDTH + x] = on ? ON : OFF;
}

void Window::clear_pixels() {
    memset(pixel_buffer, static_cast<uint8_t>(OFF), sizeof(pixel_buffer));
}

void Window::update_pixels() {
    SDL_UpdateTexture(texture, NULL, pixel_buffer, BUF_WIDTH * sizeof(uint8_t));
}

void Window::render() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, &rect_display);
    SDL_RenderPresent(renderer);
}

void Window::poll() {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            } else {
                auto key = KEY_MAP.find(event.key.keysym.sym);
                if (key != KEY_MAP.end()) {
                    key_pressed[key->second] = 1;
                }
            }
            break;
        case SDL_KEYUP:
            auto key = KEY_MAP.find(event.key.keysym.sym);
            if (key != KEY_MAP.end()) {
                key_pressed[key->second] = 0;
            }
        }
    }
}

uint8_t Window::await_keypress() {
    while (true) {
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                running = false;
                return 0x0;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                    return 0x0;
                } else {
                    auto key = KEY_MAP.find(event.key.keysym.sym);
                    if (key != KEY_MAP.end()) {
                        return key->second;
                    }
                }
            }
        }
    }
}