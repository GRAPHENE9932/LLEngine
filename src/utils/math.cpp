#include <array>
#include <algorithm>

#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>

#include "../structs/HorLine.hpp"
#include "../structs/VertLine.hpp"
#include "../structs/QuadrantArc.hpp"
#include "math.hpp"

IntersectionCount utils::vert_and_hor_line_intersection_points(
    const VertLine& vert_line, const HorLine& hor_line,
    glm::vec2& point_1) {
    assert(vert_line.lower_y <= vert_line.higher_y);
    assert(hor_line.lower_x <= hor_line.higher_x);

    //      *---------*
    //   ^       ^       ^
    //   2       1       3
    
    // Case 1:
    if (vert_line.x >= hor_line.lower_x && vert_line.x <= hor_line.higher_x &&
        hor_line.y >= vert_line.lower_y && hor_line.y <= vert_line.higher_y) {
        point_1 = {vert_line.x, hor_line.y};
        return IntersectionCount::ONE_POINT;
    }
    else {
        return IntersectionCount::NO_INTERSECTION;
    }
}

Rect utils::window_space_to_opengl_space(Rect rect, glm::vec2 window_size) {
    rect.position = rect.position / window_size * 2.0f;
    rect.position.y *= -1;
    rect.position -= glm::vec2(1.0f, -1.0f);

    rect.size /= window_size;

    return rect;
}
