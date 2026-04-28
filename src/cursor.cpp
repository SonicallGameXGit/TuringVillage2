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

    this->currentFrame++;
    if (this->currentFrame == 0) {
        this->currentFrame = 1;
    }
    if (SDL_GetGlobalMouseState(nullptr, nullptr) & SDL_BUTTON_LMASK) {
        if (this->leftClickFrame == 0) {
            this->leftClickFrame = this->currentFrame;
        }
    } else {
        this->leftClickFrame = 0;
    }
    if (SDL_GetGlobalMouseState(nullptr, nullptr) & SDL_BUTTON_RMASK) {
        if (this->rightClickFrame == 0) {
            this->rightClickFrame = this->currentFrame;
        }
    } else {
        this->rightClickFrame = 0;
    }
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

bool Cursor::isLeftPressed() const {
    return this->leftClickFrame != 0;
}
bool Cursor::isLeftJustPressed() const {
    return this->leftClickFrame == this->currentFrame;
}
bool Cursor::isRightPressed() const {
    return this->rightClickFrame != 0;
}
bool Cursor::isRightJustPressed() const {
    return this->rightClickFrame == this->currentFrame;
}