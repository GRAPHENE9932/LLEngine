#include "VertLS.hpp"

glm::vec2 VertLS::closest_point(glm::vec2 point) const {
    assert(lower_y <= higher_y);
    //   < 2
    // *
    // |
    // | < 1
    // |
    // *
    //   < 3

    // Case 1.
    if (point.y >= lower_y && point.y <= higher_y) {
        point.x = x;
        return point;
    }
    // Case 2.
    else if (point.y < lower_y) {
        return glm::vec2(x, lower_y);
    }
    // Case 3.
    else {
        return glm::vec2(x, higher_y);
    }
}

uint8_t VertLS::intersection_points(const VertLS& other,
        glm::vec2& point_1, const bool include_edges) const {
    assert(lower_y <= higher_y);
    assert(other.lower_y <= other.higher_y);

    if (x != other.x)
        return 0;

    if (lower_y < other.higher_y && higher_y > other.lower_y)
        return INFINITE_POINTS;

    if (include_edges) {
        if (lower_y == other.higher_y) {
            point_1 = {x, lower_y};
            return 1;
        }
        else if (higher_y == other.lower_y) {
            point_1 = {x, higher_y};
            return 1;
        }
    }

    return 0;
}

uint8_t VertLS::intersection_points(const HorLS& other,
        glm::vec2& point_1, const bool include_edges) const {
    assert(lower_y <= higher_y);
    assert(other.lower_x <= other.higher_x);

    //      *---------*
    //   ^       ^       ^
    //   2       1       3
    
    // Case 1:
    if (include_edges && x >= other.lower_x && x <= other.higher_x &&
        other.y >= lower_y && other.y <= higher_y) {
        point_1 = {x, other.y};
        return 1;
    }
    else if (!include_edges && x > other.lower_x && x < other.higher_x &&
        other.y > lower_y && other.y < higher_y) {
        point_1 = {x, other.y};
        return 1;
    }
    else {
        return 0;
    }
}
