#pragma once
#include <glad/glad.h>
#include "../raii.hpp"

// GL_Texture
void GL_Deleter_Texture(GLuint texture);
using GL_Texture = Resource<GLuint, GL_Deleter_Texture>;
GL_Texture GL_CreateTexture();

class Texture {
private:
    Texture(GL_Texture &&glTexture, uint32_t width, uint32_t height);
public:
    GL_Texture glTexture;
    uint32_t width, height;

    Texture(Texture&&) noexcept = default;
    Texture &operator=(Texture&&) noexcept = default;
    Texture(const Texture&) = delete;
    Texture &operator=(const Texture&) = delete;

    static Texture loadFromFile(const char *filename, GLenum filter);
};