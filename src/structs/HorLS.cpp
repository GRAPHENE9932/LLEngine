#include "HorLS.hpp"

glm::vec2 HorLS::closest_point(glm::vec2 point) const {
    assert(lower_x <= higher_x);
    //      *---------*
    //   ^       ^       ^
    //   2       1       3

    // Case 1.
    if (point.x >= lower_x && point.x <= higher_x) {
        point.y = y;
        return point;
    }
    // Case 2.
    else if (point.x < lower_x) {
        return glm::vec2(lower_x, y);
    }
    // Case 3.
    else {
        return glm::vec2(higher_x, y);
    }
}

uint8_t HorLS::intersection_points(const HorLS& other,
        glm::vec2& point_1, const bool include_edges) const {
    assert(lower_x <= higher_x);
    assert(other.lower_x <= other.higher_x);

    if (y != other.y)
        return 0;

    if (lower_x < other.higher_x && higher_x > other.lower_x)
        return INFINITE_POINTS;

    if (include_edges) {
        if (lower_x == other.higher_x) {
            point_1 = {lower_x, y};
            return 1;
        }
        else if (higher_x == other.lower_x) {
            point_1 = {higher_x, y};
            return 1;
        }
    }

    return 0;
}