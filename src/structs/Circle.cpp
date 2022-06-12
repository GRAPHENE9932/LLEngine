#include <glm/geometric.hpp>

#include "../consts_and_enums.hpp"
#include "Circle.hpp"

uint8_t Circle::intersection_points(const Circle& other,
                                    glm::vec2& point_1, glm::vec2& point_2) const {
    if (center == other.center)
        return INFINITE_POINTS;

    glm::vec2 vector_a = (other.center - center) * 0.5f;
    float vector_a_len = glm::length(vector_a);

    if (vector_a_len > radius)
        return 0;

    float vector_b_len = std::sqrt(radius * radius - vector_a_len * vector_a_len);
    glm::vec2 vector_b {
        glm::vec2(-vector_a.y, vector_a.x) / vector_a_len * vector_b_len
    };
    point_1 = vector_b + center + vector_a;

    if (vector_a_len == radius)
        return 1;

    point_2 = -vector_b + center + vector_a;
    return 2;
}
