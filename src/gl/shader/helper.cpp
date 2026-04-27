#include "helper.hpp"
#include <vector>
#include <stdexcept>

void compileShaderSource(GLuint shader, const GLchar *const *source) {
    glShaderSource(shader, 1, source, nullptr);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::vector<GLchar> infoLog = std::vector<GLchar>(length);
        glGetShaderInfoLog(shader, length, nullptr, infoLog.data());
        throw std::runtime_error(std::string("Failed to compile shader: ") + infoLog.data());
    }
}
void linkProgram(GLuint program) {
    glLinkProgram(program);
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        GLint length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        std::vector<GLchar> infoLog = std::vector<GLchar>(length);
        glGetProgramInfoLog(program, length, nullptr, infoLog.data());
        throw std::runtime_error(std::string("Failed to link shader program: ") + infoLog.data());
    }
}