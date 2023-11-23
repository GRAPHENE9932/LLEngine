#pragma once

#include <glm/vec3.hpp>

#include <cstdint>

namespace llengine {
struct AABB {
    glm::vec3 point_max;
    glm::vec3 point_min;

    [[nodiscard]] glm::vec3 get_vertex(std::uint8_t vertex_index) const {
        assert(vertex_index < 8);

        return {
            vertex_index & 0x01 ? point_max.x : point_min.x,
            vertex_index & 0x02 ? point_max.y : point_min.y,
            vertex_index & 0x04 ? point_max.z : point_min.z
        };
    }
};
}