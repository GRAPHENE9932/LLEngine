#include <array>

#include <glm/geometric.hpp>

#include "QuadrantArc.hpp"
#include "HorLine.hpp"
#include "VertLine.hpp"
#include "RoundedRectangle.hpp"

glm::vec2 RoundedRectangle::closest_point(glm::vec2 point, float* const min_distance_out) const {
    // 7-0-4
    // 3   1
    // 6-2-5
    std::array<glm::vec2, 8> closest_points;

    HorLine hor_line {rect.position.x, rect.position.x + rect.size.x, rect.position.y - radius};
    closest_points[0] = hor_line.closest_point(point);

    VertLine vert_line {rect.position.y, rect.position.y + rect.size.y, rect.position.x + rect.size.x + radius};
    closest_points[1] = vert_line.closest_point(point);
    
    closest_points[2] = closest_points[0];
    closest_points[2].y += rect.size.y + radius + radius;

    closest_points[3] = closest_points[1];
    closest_points[3].x -= rect.size.x + radius + radius;

    QuadrantArc arc_1 {{rect.position + glm::vec2(rect.size.x, 0.0f), radius}, 4};
    closest_points[4] = arc_1.closest_point(point);

    QuadrantArc arc_2 {{rect.position + rect.size, radius}, 1};
    closest_points[5] = arc_2.closest_point(point);
    
    QuadrantArc arc_3 {{rect.position + glm::vec2(0.0f, rect.size.y), radius}, 2};
    closest_points[6] = arc_3.closest_point(point);

    QuadrantArc arc_4 {{rect.position, radius}, 3};
    closest_points[7] = arc_4.closest_point(point);

    // From the 8 closest points find only one, the closest.
    glm::vec2 closest_point;
    float min_distance = INFINITY;
    for (char i = 0; i < 8; i++) {
        float cur_distance = glm::distance(point, closest_points[i]);
        if (min_distance > cur_distance) {
            min_distance = cur_distance;
            closest_point = closest_points[i];
        }
    }

    if (min_distance_out != nullptr)
        *min_distance_out = min_distance;

    return closest_point;
}