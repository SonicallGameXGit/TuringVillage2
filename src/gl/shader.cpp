#include "shader.hpp"

void GL_Deleter_ShaderProgram(GLuint program) {
    glDeleteProgram(program);
}
void GL_Deleter_Shader(GLuint shader) {
    glDeleteShader(shader);
}

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

QuadShader::QuadShader() : shaderProgram(glCreateProgram()) {
    GL_Shader vertexShader = GL_Shader(glCreateShader(GL_VERTEX_SHADER));
    const char *vertexShaderSource = R"(
        #version 410
        layout(location = 0) in vec2 a_Position;
        layout(location = 0) out vec2 v_TexCoord;

        uniform vec4 u_Transform;
        uniform vec3 u_CameraTransform;
        uniform float u_AspectRatio;

        void main() {
            gl_Position = vec4(a_Position, 0.0, 1.0);
            gl_Position.xy = gl_Position.xy * u_Transform.zw + u_Transform.xy;
            gl_Position.xy = (gl_Position.xy - u_CameraTransform.xy) * u_CameraTransform.z;
            gl_Position.x /= u_AspectRatio;
            v_TexCoord = vec2(a_Position.x, 1.0 - a_Position.y);
        }
    )";
    compileShaderSource(vertexShader.get(), &vertexShaderSource);

    GL_Shader fragmentShader = GL_Shader(glCreateShader(GL_FRAGMENT_SHADER));
    const char *fragmentShaderSource = R"(
        #version 410
        layout(location = 0) in vec2 v_TexCoord;
        layout(location = 0) out vec4 f_Color;

        uniform sampler2D u_Texture;

        void main() {
            f_Color = texture(u_Texture, v_TexCoord);
        }
    )";
    compileShaderSource(fragmentShader.get(), &fragmentShaderSource);
    
    glAttachShader(this->shaderProgram.get(), vertexShader.get());
    glAttachShader(this->shaderProgram.get(), fragmentShader.get());
    linkProgram(this->shaderProgram.get());
}

void QuadShader::bind() const {
    glUseProgram(this->shaderProgram.get());
}
void QuadShader::setAspectRatio(float aspectRatio) const {
    glUniform1f(glGetUniformLocation(this->shaderProgram.get(), "u_AspectRatio"), aspectRatio);
}
void QuadShader::setCameraTransform(float x, float y, float zoom) const {
    glUniform3f(glGetUniformLocation(this->shaderProgram.get(), "u_CameraTransform"), x, y, zoom);
}
void QuadShader::setTransform(float x, float y, float width, float height) const {
    glUniform4f(glGetUniformLocation(this->shaderProgram.get(), "u_Transform"), x, y, width, height);
}