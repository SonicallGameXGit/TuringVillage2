#include "core.hpp"

void GL_Deleter_ShaderProgram(GLuint program) {
    glDeleteProgram(program);
}
void GL_Deleter_Shader(GLuint shader) {
    glDeleteShader(shader);
}