#include <array>

#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

#include "../structs/RoundedRectangle.hpp"
#include "RectangularWall.hpp"

RectangularWall::RectangularWall(Rect rect) {
    this->set_rect(rect);
}

bool RectangularWall::cylinder_intersects(const Cylinder& cylinder) const {
    return get_rect().distance_from({cylinder.position.x, cylinder.position.z}) < cylinder.radius;
}

glm::vec3 RectangularWall::force_point_distance(glm::vec3 point, float distance,
                                                float* const min_distance_out) const {
    const RoundedRectangle& rounded_rect {get_rounded_rect(distance)};
    glm::vec2 result = rounded_rect.closest_point({point.x, point.z}, min_distance_out);

    return glm::vec3(result.x, point.y, result.y);
}

