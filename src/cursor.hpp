#pragma once
#include <cstdint>
#include "maths.hpp"
#include "camera.hpp"

class Cursor {
private:
    Vec2 position = {};
    uint64_t currentFrame = 1, leftClickFrame = 0, rightClickFrame = 0;
public:
    Cursor();
    ~Cursor();

    void update(const Vec2 &windowSize, const Camera &camera);

    float getX() const;
    float getY() const;
    const Vec2 &getPosition() const;

    bool isLeftPressed() const;
    bool isLeftJustPressed() const;
    bool isRightPressed() const;
    bool isRightJustPressed() const;
};