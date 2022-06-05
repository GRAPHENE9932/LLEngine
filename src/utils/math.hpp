#pragma once

#include "../structs/Rect.hpp"

namespace utils {
    glm::vec2 closest_point_to_hor_line_segment(glm::vec2 point, float point_a_x, float point_b_x, float point_a_b_y);
    glm::vec2 closest_point_to_ver_line_segment(glm::vec2 point, float point_a_y, float point_b_y, float point_a_b_x);
    glm::vec2 closest_point_to_arc(glm::vec2 point, glm::vec2 center, float radius, float min_angle, float max_angle);
    glm::vec2 closest_point_to_rounded_rectangle(glm::vec2 point, Rect rect, float radius,
                                                 float* const min_distance_out = nullptr);

    enum class IntersectionCount : uint8_t {
        NO_INTERSECTION, ONE_POINT, TWO_POINTS, THREE_POINTS, FOUR_POINTS,
        INFINITE_POINTS
    };
    IntersectionCount two_circles_intersection_points(const glm::vec2 center_1, const glm::vec2 center_2, const float radius,
                                                      glm::vec2& point_1, glm::vec2& point_2);
    IntersectionCount two_arcs_intersection_points(const glm::vec2 center_1, const uint8_t arc_quadrant_1,
                                                   const glm::vec2 center_2, const uint8_t arc_quadrant_2,
                                                   const float radius, glm::vec2& point_1, glm::vec2& point_2);
    void rounded_rectangle_intersection_points(Rect rect_1, Rect rect_2, float radius);

    Rect window_space_to_opengl_space(Rect rect, glm::vec2 window_size);
}
