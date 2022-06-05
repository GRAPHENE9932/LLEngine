#pragma once

#include "../consts_and_enums.hpp"
#include "../structs/Rect.hpp"
#include "../structs/VertLine.hpp"
#include "../structs/HorLine.hpp"

namespace utils {
    IntersectionCount vert_and_hor_line_intersection_points(
        const VertLine& vert_line, const HorLine& hor_line,
        glm::vec2& point_1
    );

    Rect window_space_to_opengl_space(Rect rect, glm::vec2 window_size);
}
