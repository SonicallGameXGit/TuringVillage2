#pragma once
#include <glad/glad.h>

void compileShaderSource(GLuint shader, const GLchar *const *source);
void linkProgram(GLuint program);