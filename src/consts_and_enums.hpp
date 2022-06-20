#pragma once

#include <limits>
#include <array>
#include <stdint.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

constexpr uint8_t INFINITE_POINTS = std::numeric_limits<uint8_t>::max(); 

constexpr std::array<float, 18> QUAD_VERTICES {
    0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f
};

constexpr std::array<float, 12> QUAD_UVS {
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
};

/// QUAD_UVS but with inverted V (Y) values.
constexpr std::array<float, 12> QUAD_UVS_INV_V {
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f
};