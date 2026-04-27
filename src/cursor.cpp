#include "cursor.hpp"
#include <SDL3/SDL.h>

Cursor::Cursor() = default;
Cursor::~Cursor() = default;

void Cursor::update(const Vec2 &windowSize, const Camera &camera) {
    SDL_GetGlobalMouseState(&this->position.x, &this->position.y);
    this->position.x /= static_cast<float>(windowSize.x);
    this->position.y /= static_cast<float>(windowSize.y);
    this->position.y = 1.0f - this->position.y;
    this->position.x *= 2.0f;
    this->position.y *= 2.0f;
    this->position.x -= 1.0f;
    this->position.y -= 1.0f;
    this->position.x *= windowSize.x / windowSize.y;
    this->position.x /= camera.zoom;
    this->position.y /= camera.zoom;
    this->position.x += camera.position.x;
    this->position.y += camera.position.y;
}

float Cursor::getX() const {
    return this->position.x;
}
float Cursor::getY() const {
    return this->position.y;
}
const Vec2 &Cursor::getPosition() const {
    return this->position;
}