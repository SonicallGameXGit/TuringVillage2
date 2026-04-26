#pragma once
#include <stdexcept>
#include <memory>
#include <utility>
#include <SDL3/SDL.h>
#include <glad/glad.h>

struct SDL {
    explicit SDL(SDL_InitFlags flags) {
        if (!SDL_Init(flags)) {
            throw std::runtime_error("Failed to initialize SDL");
        }
    }
    ~SDL() {
        SDL_Quit();
    }
};

template <typename T, auto Deleter>
class Resource {
private:
    T handle;
public:
    explicit Resource(T handle) : handle(handle) {}
    ~Resource() {
        if (this->handle) {
            Deleter(this->handle);
        }
    }

    Resource(Resource &&other) noexcept : handle(std::exchange(other.handle, {})) {}
    Resource &operator=(Resource &&other) noexcept {
        if (this != &other) {
            if (this->handle) {
                Deleter(this->handle);
            }
            this->handle = std::exchange(other.handle, {});
        }
        return *this;
    }

    Resource(const Resource&) = delete;
    Resource &operator=(const Resource&) = delete;

    T get() const {
        return handle;
    }
};

using SDL_WindowPtr = Resource<SDL_Window*, SDL_DestroyWindow>;
using SDL_GLContextPtr = Resource<SDL_GLContext, SDL_GL_DestroyContext>;

// GL_VertexArrayObject
inline void GL_Deleter_VertexArrayObject(GLuint vao) {
    glDeleteVertexArrays(1, &vao);
}
using GL_VertexArrayObject = Resource<GLuint, GL_Deleter_VertexArrayObject>;
inline GL_VertexArrayObject GL_CreateVertexArrayObject() {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    return GL_VertexArrayObject(vao);
}

// GL_BufferObject
inline void GL_Deleter_BufferObject(GLuint vbo) {
    glDeleteBuffers(1, &vbo);
}
using GL_BufferObject = Resource<GLuint, GL_Deleter_BufferObject>;
inline GL_BufferObject GL_CreateBufferObject() {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    return GL_BufferObject(vbo);
}

// GL_ShaderProgram
inline void GL_Deleter_ShaderProgram(GLuint program) {
    glDeleteProgram(program);
}
using GL_ShaderProgram = Resource<GLuint, GL_Deleter_ShaderProgram>;

// GL_Shader
inline void GL_Deleter_Shader(GLuint shader) {
    glDeleteShader(shader);
}
using GL_Shader = Resource<GLuint, GL_Deleter_Shader>;

// GL_Texture
inline void GL_Deleter_Texture(GLuint texture) {
    glDeleteTextures(1, &texture);
}
using GL_Texture = Resource<GLuint, GL_Deleter_Texture>;
inline GL_Texture GL_CreateTexture() {
    GLuint texture;
    glGenTextures(1, &texture);
    return GL_Texture(texture);
}