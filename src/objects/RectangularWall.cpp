#include <array>

#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

#include "../utils/math.hpp"
#include "RectangularWall.hpp"

RectangularWall::RectangularWall(Rect rect) {
    this->rect = rect;
}

bool RectangularWall::cylinder_intersects(const Cylinder& cylinder) const {
    return rect.distance_from({cylinder.position.x, cylinder.position.z}) < cylinder.radius;
}

glm::vec3 RectangularWall::force_point_distance(glm::vec3 point, float distance,
                                                float* const min_distance_out) const {
    glm::vec2 result = utils::closest_point_to_rounded_rectangle(
        glm::vec2(point.x, point.z), rect, distance, min_distance_out
    );

    return glm::vec3(result.x, point.y, result.y);
}

