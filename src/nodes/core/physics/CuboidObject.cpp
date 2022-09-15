#include <glm/geometric.hpp>

#include "structs/RoundedRectangle.hpp"
#include "CuboidObject.hpp"

CuboidObject::CuboidObject(const Rect& rect, float bottom_y, float top_y) :
    bottom_y(bottom_y), top_y(top_y) {
    set_rect({rect.position - rect.size * 0.5f, rect.size});
}

bool CuboidObject::cylinder_intersects(const Cylinder& cylinder) const {
    return (cylinder.lowest_y() < top_y && bottom_y < cylinder.highest_y()) &&
        get_rect().distance_from({cylinder.position.x, cylinder.position.z}) < cylinder.radius;
}

glm::vec3 CuboidObject::push_cylinder_out(const Cylinder& cylinder, RelativePos& pos,
                                          float* const min_distance_out) const {
    // Case 1: push it to the top side of cuboid.
    glm::vec3 result_1 = cylinder.position;
    result_1.y = top_y + cylinder.height * 0.5f;

    // Case 2: push it to the bottom side of cuboid.
    glm::vec3 result_2 = cylinder.position;
    result_2.y = bottom_y - cylinder.height * 0.5f;

    // Case 3: push it to the side sides of cuboid.
    const RoundedRectangle& rounded_rect {get_rounded_rect(cylinder.radius)};
    glm::vec2 calc_res = rounded_rect.closest_point({cylinder.position.x, cylinder.position.z});
    glm::vec3 result_3 = {calc_res.x, cylinder.position.y, calc_res.y};

    // Take the closest one.
    float distance_1 = glm::distance(result_1, cylinder.position);
    float distance_2 = glm::distance(result_2, cylinder.position);
    float distance_3 = glm::distance(result_3, cylinder.position);
    float min = std::min({distance_1, distance_2, distance_3});

    if (min_distance_out != nullptr)
        *min_distance_out = min;

    if (min == distance_1) {
        pos = RelativePos::ABOVE;
        return result_1;
    }
    else if (min == distance_2) {
        pos = RelativePos::UNDER;
        return result_2;
    }
    else {
        pos = RelativePos::NEXT_TO;
        return result_3;
    }
}