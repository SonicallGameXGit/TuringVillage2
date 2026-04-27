#pragma once
#include <SDL3/SDL.h>
#include "raii.hpp"

struct SDL {
    explicit SDL(SDL_InitFlags flags);
    ~SDL();
};
using SDL_WindowPtr = Resource<SDL_Window*, SDL_DestroyWindow>;
using SDL_GLContextPtr = Resource<SDL_GLContext, SDL_GL_DestroyContext>;