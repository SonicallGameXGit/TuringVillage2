#include "quad.hpp"
#include "helper.hpp"

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
QuadShader::~QuadShader() = default;

void QuadShader::bind() const {
    glUseProgram(this->shaderProgram.get());
}
void QuadShader::setAspectRatio(float aspectRatio) const {
    glUniform1f(glGetUniformLocation(this->shaderProgram.get(), "u_AspectRatio"), aspectRatio);
}
void QuadShader::setCamera(const Camera &camera) const {
    glUniform3f(glGetUniformLocation(this->shaderProgram.get(), "u_CameraTransform"), camera.position.x, camera.position.y, camera.zoom);
}
void QuadShader::setTransform(const Vec2 &position, const Vec2 &size) const {
    glUniform4f(glGetUniformLocation(this->shaderProgram.get(), "u_Transform"), position.x, position.y, size.x, size.y);
}