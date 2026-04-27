#pragma once
#include "core.hpp"
#include "../../camera.hpp"

class QuadShader {
private:
    GL_ShaderProgram shaderProgram;
public:
    QuadShader();
    ~QuadShader();

    void bind() const;
    void setAspectRatio(float aspectRatio) const;
    void setCamera(const Camera &camera) const;
    void setTransform(const Vec2 &position, const Vec2 &size) const;
};