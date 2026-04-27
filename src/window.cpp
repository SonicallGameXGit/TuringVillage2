#include "window.hpp"
#include <stdexcept>
#include <exception>

SDL::SDL(SDL_InitFlags flags) {
    if (!SDL_Init(flags)) {
        throw std::runtime_error("Failed to initialize SDL");
    }
}
SDL::~SDL() {
    SDL_Quit();
}