#include "VertLine.hpp"

glm::vec2 VertLine::closest_point(glm::vec2 point) const {
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