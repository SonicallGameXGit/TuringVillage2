#pragma once
#include <glad/glad.h>
#include "../../raii.hpp"

// GL_ShaderProgram
void GL_Deleter_ShaderProgram(GLuint program);
using GL_ShaderProgram = Resource<GLuint, GL_Deleter_ShaderProgram>;

// GL_Shader
void GL_Deleter_Shader(GLuint shader);
using GL_Shader = Resource<GLuint, GL_Deleter_Shader>;