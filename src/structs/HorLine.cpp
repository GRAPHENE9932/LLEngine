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
