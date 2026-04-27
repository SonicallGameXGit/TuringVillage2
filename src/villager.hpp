#pragma once
#include <vector>
#include <cstdint>
#include "maths.hpp"

struct Villager {
    Vec2 position = {};
    std::vector<uint32_t> coworkersIds = std::vector<uint32_t>();

    Villager();
    ~Villager();
};