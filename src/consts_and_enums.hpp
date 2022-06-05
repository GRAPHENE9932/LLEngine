#pragma once

#include <stdint.h>

enum class IntersectionCount : uint8_t {
    NO_INTERSECTION, ONE_POINT, TWO_POINTS, THREE_POINTS, FOUR_POINTS,
    INFINITE_POINTS
};
