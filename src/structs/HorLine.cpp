#include "HorLine.hpp"

glm::vec2 HorLine::closest_point(glm::vec2 point) const {
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

IntersectionCount HorLine::intersection_points(const HorLine& other,
    glm::vec2& point_1, const bool include_edges) const {
    assert(lower_x <= higher_x);
    assert(other.lower_x <= other.higher_x);

    if (y != other.y)
        return IntersectionCount::NO_INTERSECTION;

    if (lower_x < other.higher_x && higher_x > other.lower_x)
        return IntersectionCount::INFINITE_POINTS;

    if (include_edges) {
        if (lower_x == other.higher_x) {
            point_1 = {lower_x, y};
            return IntersectionCount::ONE_POINT;
        }
        else if (higher_x == other.lower_x) {
            point_1 = {higher_x, y};
            return IntersectionCount::ONE_POINT;
        }
    }

    return IntersectionCount::NO_INTERSECTION;
}