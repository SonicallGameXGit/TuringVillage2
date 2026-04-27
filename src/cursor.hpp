#pragma once
#include "maths.hpp"
#include "camera.hpp"

class Cursor {
private:
    Vec2 position = {};
public:
    Cursor();
    ~Cursor();

    void update(const Vec2 &windowSize, const Camera &camera);

    float getX() const;
    float getY() const;
    const Vec2 &getPosition() const;
};