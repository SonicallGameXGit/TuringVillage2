#pragma once
#include <vector>
#include <cstdint>

struct Villager {
    float x = 0.0f, y = 0.0f;
    std::vector<uint32_t> coworkersIds = std::vector<uint32_t>();

    Villager() = default;
    ~Villager() = default;
};