#include "connection_line.hpp"
#include <cmath>
#include "helper.hpp"

ConnectionLineShader::ConnectionLineShader() : shaderProgram(glCreateProgram()) {
    GL_Shader vertexShader = GL_Shader(glCreateShader(GL_VERTEX_SHADER));
    const char *vertexShaderSource = R"(
        #version 410
        layout(location = 0) in vec2 a_Position;
        layout(location = 0) out vec2 v_TexCoord;

        uniform vec4 u_Transform;
        uniform vec3 u_CameraTransform;
        uniform float u_LineWidth, u_AspectRatio, u_Time;

        mat2 rotationMatrix(float angle) {
            float s = sin(angle);
            float c = cos(angle);
            return mat2(c, -s, s, c);
        }

        void main() {
            gl_Position = vec4(a_Position, 0.0, 1.0);
            gl_Position.x *= u_LineWidth; // Apply line width
            gl_Position.y *= u_Transform.w; // Apply line length
            gl_Position.x -= 0.5; // Center out horizontally
            gl_Position.xy = rotationMatrix(u_Transform.z) * gl_Position.xy;
            gl_Position.xy += u_Transform.xy;
            gl_Position.xy = (gl_Position.xy - u_CameraTransform.xy) * u_CameraTransform.z;
            gl_Position.x /= u_AspectRatio;
            v_TexCoord = vec2(a_Position.x, 1.0 - a_Position.y * u_Transform.w / u_LineWidth + fract(u_Time));
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
ConnectionLineShader::~ConnectionLineShader() = default;

void ConnectionLineShader::bind() const {
    glUseProgram(this->shaderProgram.get());
}
void ConnectionLineShader::setAspectRatio(float aspectRatio) const {
    glUniform1f(glGetUniformLocation(this->shaderProgram.get(), "u_AspectRatio"), aspectRatio);
}
void ConnectionLineShader::setCamera(const Camera &camera) const {
    glUniform3f(glGetUniformLocation(this->shaderProgram.get(), "u_CameraTransform"), camera.position.x, camera.position.y, camera.zoom);
}
void ConnectionLineShader::setPoints(const Vec2 &from, const Vec2 &to) const {
    glUniform4f(glGetUniformLocation(this->shaderProgram.get(), "u_Transform"), from.x, from.y, std::atan2f(to.x - from.x, to.y - from.y), std::hypotf(to.x - from.x, to.y - from.y));
}
void ConnectionLineShader::setWidth(float width) const {
    glUniform1f(glGetUniformLocation(this->shaderProgram.get(), "u_LineWidth"), width);
}
void ConnectionLineShader::setTime(float time) const {
    glUniform1f(glGetUniformLocation(this->shaderProgram.get(), "u_Time"), time);
}