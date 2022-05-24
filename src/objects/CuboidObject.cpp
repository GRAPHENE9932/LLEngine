#include <glm/geometric.hpp>

#include "../utils/utils.hpp"
#include "CuboidObject.hpp"

CuboidObject::CuboidObject(const Rect& rect, float bottom_y, float top_y) :
    rect(rect), bottom_y(bottom_y), top_y(top_y) {

}

glm::vec3 CuboidObject::push_cylinder_out(const Cylinder& cylinder, bool& is_above) const {
    // Case 1: push it to the top side of cuboid.
    glm::vec3 result_1 = cylinder.position;
    result_1.y = top_y + cylinder.height * 0.5f;

    // Case 2: push it to the bottom side of cuboid.
    glm::vec3 result_2 = cylinder.position;
    result_2.y = bottom_y - cylinder.height * 0.5f;

    // Case 3: push it to the side sides of cuboid.
    glm::vec2 calc_res = utils::closest_point_to_rounded_rectangle(
        {cylinder.position.x, cylinder.position.z}, rect, cylinder.radius
    );
    glm::vec3 result_3 = {calc_res.x, cylinder.position.y, calc_res.y};

    // Take the closest one.
    float distance_1 = glm::distance(result_1, cylinder.position);
    float distance_2 = glm::distance(result_2, cylinder.position);
    float distance_3 = glm::distance(result_3, cylinder.position);
    float min = std::min({distance_1, distance_2, distance_3});
    if (min == distance_1) {
        is_above = true;
        return result_1;
    }
    else if (min == distance_2) {
        is_above = false;
        return result_2;
    }
    else {
        is_above = false;
        return result_3;
    }
}
