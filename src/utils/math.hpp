#pragma once

#include "../structs/Rect.hpp"

namespace utils {
    glm::vec2 closest_point_to_rounded_rectangle(glm::vec2 point, Rect rect, float radius,
                                                 float* const min_distance_out = nullptr);

    void rounded_rectangle_intersection_points(Rect rect_1, Rect rect_2, float radius);

    Rect window_space_to_opengl_space(Rect rect, glm::vec2 window_size);
}
