#include <array>

#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

#include "../utils/utils.hpp"
#include "RectangularWall.hpp"

RectangularWall::RectangularWall(Rect rect) : rect(rect) {

}

glm::vec3 RectangularWall::force_point_distance(glm::vec3 point, float distance) const {
    glm::vec2 result = utils::closest_point_to_rounded_rectangle(
        glm::vec2(point.x, point.z), rect, distance
    );

    return glm::vec3(result.x, point.y, result.y);
}

