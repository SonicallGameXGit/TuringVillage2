#pragma once
#include "core.hpp"
#include "../../maths.hpp"
#include "../../camera.hpp"

class ConnectionLineShader {
private:
    GL_ShaderProgram shaderProgram;
public:
    ConnectionLineShader();
    ~ConnectionLineShader();

    void bind() const;
    void setAspectRatio(float aspectRatio) const;
    void setCamera(const Camera &camera) const;
    void setPoints(const Vec2 &from, const Vec2 &to) const;
    void setWidth(float width) const;
    void setTime(float time) const;
};