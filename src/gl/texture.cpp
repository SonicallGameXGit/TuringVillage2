#include "texture.hpp"
#include <SDL3_image/SDL_image.h>

void GL_Deleter_Texture(GLuint texture) {
    glDeleteTextures(1, &texture);
}

GL_Texture GL_CreateTexture() {
    GLuint texture;
    glGenTextures(1, &texture);
    return GL_Texture(texture);
}

Texture::Texture(GL_Texture &&glTexture, uint32_t width, uint32_t height) : glTexture(std::move(glTexture)), width(width), height(height) {}
Texture Texture::loadFromFile(const char *filename, GLenum filter) {
    SDL_Surface *surface = IMG_Load(filename);
    if (surface == nullptr) {
        throw std::runtime_error(std::string("Failed to load image: ") + SDL_GetError());
    }
    const uint32_t width = static_cast<uint32_t>(surface->w);
    const uint32_t height = static_cast<uint32_t>(surface->h);
    GL_Texture glTexture = GL_CreateTexture();

    // Let's instead just make proper assets, instead of doubling loading time by converting them for no reason every time?
    // SDL_Surface *rgbaSurface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    // SDL_DestroySurface(surface);
    // if (rgbaSurface == nullptr) {
    //     throw std::runtime_error(std::string("Failed to convert image to RGBA32: ") + SDL_GetError());
    // }

    glBindTexture(GL_TEXTURE_2D, glTexture.get());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_DestroySurface(surface);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

    return Texture(std::move(glTexture), width, height);
}