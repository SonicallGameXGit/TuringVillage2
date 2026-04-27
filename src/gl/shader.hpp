#pragma once
#include <vector>
#include <glad/glad.h>
#include "../raii.hpp"

// GL_ShaderProgram
void GL_Deleter_ShaderProgram(GLuint program);
using GL_ShaderProgram = Resource<GLuint, GL_Deleter_ShaderProgram>;

// GL_Shader
void GL_Deleter_Shader(GLuint shader);
using GL_Shader = Resource<GLuint, GL_Deleter_Shader>;

class QuadShader {
private:
    GL_ShaderProgram shaderProgram;
public:
    QuadShader();
    ~QuadShader() = default;

    void bind() const;
    void setAspectRatio(float aspectRatio) const;
    void setCameraTransform(float x, float y, float zoom) const;
    void setTransform(float x, float y, float width, float height) const;
};