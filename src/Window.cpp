#include "Window.h"

Window::Window() {
    if (!this->init_sdl()) {
        this->close_sdl();
        throw - 1;
    }
}

Window::~Window() {
    this->close_sdl();
}

bool Window::init_sdl() {
    // test connection to everything
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "Error: SDL initializating everything failed..\n";
        return false;
    }

    // create a window
    window = SDL_CreateWindow(
        "Chip8 Emulator",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        BUF_WIDTH * (WIN_WIDTH / BUF_WIDTH),
        BUF_HEIGHT * (WIN_HEIGHT / BUF_HEIGHT),
        SDL_WINDOW_SHOWN);

    // ensure that it was initialized correctly
    if (window == nullptr) {
        std::cerr << "Error: SDL window could not be created...\n";
        return false;
    }

    // create a renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    // ensure that it was initialized correctly
    if (renderer == nullptr) {
        std::cerr << "Error: SDL renderer could not be created...\n";
        return false;
    }

    // set the resolution independant of the rendering pixels
    if (SDL_RenderSetLogicalSize(renderer, BUF_WIDTH, BUF_HEIGHT) < 0) {
        std::cerr << "Error: SDL renderer logical size failed to set...\n";
        return false;
    }

    // create a texture
    texture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, BUF_WIDTH, BUF_HEIGHT
    );

    // ensure that it was created sucessfully
    if (texture == nullptr) {
        std::cerr << "Error: SDL textrue could not be created...\n";
        return false;
    }

    // allocate memory
    pixel_buffer = new uint8_t[BUF_WIDTH * BUF_HEIGHT];

    // ensure that the screen is off
    for (int i = 0; i < (BUF_WIDTH * BUF_HEIGHT); i++)
        pixel_buffer[i] = OFF;

    // Set the render target
    SDL_SetRenderTarget(renderer, texture);

    // if everything ran successfully return true
    return true;
}

void Window::close_sdl() {
    if (pixel_buffer != nullptr) {
        delete pixel_buffer;
        pixel_buffer = nullptr;
    }
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
    return pixel_buffer[y * BUF_WIDTH + x] == ON;
}

void Window::set_pixel(int x, int y, bool on) {
    pixel_buffer[y * BUF_WIDTH + x] = on ? ON : OFF;
}

void Window::clear_pixels() {
    for (int i = 0; i < (BUF_WIDTH * BUF_HEIGHT); i++) {
        pixel_buffer[i] = OFF;

    }
}

void Window::render() {
    SDL_UpdateTexture(texture, nullptr, pixel_buffer, BUF_WIDTH);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
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
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            running = false;
            return 0xFF;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
                return 0xFF;
            } else {
                auto key = KEY_MAP.find(event.key.keysym.sym);
                if (key != KEY_MAP.end()) {
                    return key->second;
                }
            }
        }
    }
    return 0xFE;
}

bool Window::get_key_press(int idx) {
    return key_pressed[idx];
}
