#pragma once

#include <glm/vec3.hpp>

struct Cylinder {
    glm::vec3 position;
    float height, radius;

    inline float lowest_y() const noexcept {
        return position.y + height * -0.5f;
    }
    inline glm::vec3 lowest_point() const noexcept {
        return position + glm::vec3(0.0f, height * -0.5f, 0.0f);
    }

    inline float highest_y() const noexcept {
        return position.y + height * 0.5f;
    }
    inline glm::vec3 highest_point() const noexcept {
        return position + glm::vec3(0.0f, height * 0.5f, 0.0f);
    }
};
