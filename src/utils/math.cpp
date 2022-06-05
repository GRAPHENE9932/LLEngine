#include <array>
#include <algorithm>

#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>

#include "../structs/HorLine.hpp"
#include "../structs/VertLine.hpp"
#include "math.hpp"

constexpr float ASSERT_THRESHOLD = 0.00001f;

/// Returns angle in range.
inline float normalize_angle(const float angle) {
    return std::fmod(angle + M_PIf, 2.0f * M_PIf) - M_PIf;
}

/// Accepts angles from -PI to PI.
inline float clamp_angle(const float angle, const float min, const float max) {
    if (angle < max && angle > min)
        return angle;

    if (std::abs(normalize_angle(angle - min)) < std::abs(normalize_angle(angle - max)))
        return min;
    else
        return max;
}

glm::vec2 utils::closest_point_to_arc(glm::vec2 point, glm::vec2 center, float radius, float min_angle, float max_angle) {
    assert(max_angle > min_angle);
    assert(min_angle >= -M_PI - ASSERT_THRESHOLD && min_angle <= M_PI + ASSERT_THRESHOLD);
    assert(max_angle >= -M_PI - ASSERT_THRESHOLD && max_angle <= M_PI + ASSERT_THRESHOLD);

    // Going to the circle center space.
    point -= center;

    glm::vec2 point_on_trigonometric_circle = point / glm::length(point);
    float angle = std::atan2(point_on_trigonometric_circle.y, point_on_trigonometric_circle.x);
    angle = clamp_angle(angle, min_angle, max_angle);

    return glm::vec2(std::cos(angle) * radius, std::sin(angle) * radius) + center;
}

glm::vec2 utils::closest_point_to_rounded_rectangle(glm::vec2 point, Rect rect, float radius,
                                                    float* const min_distance_out) {
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

    closest_points[4] = closest_point_to_arc(
        point, rect.position + glm::vec2(rect.size.x, 0.0f),
        radius, glm::radians(-90.0f), glm::radians(0.0f)
    );
    closest_points[5] = closest_point_to_arc(
        point, rect.position + rect.size, radius,
        glm::radians(0.0f), glm::radians(90.0f)
    );
    closest_points[6] = closest_point_to_arc(
        point, rect.position + glm::vec2(0.0f, rect.size.y),
        radius, glm::radians(90.0f), glm::radians(180.0f)
    );
    closest_points[7] = closest_point_to_arc(
        point, rect.position, radius,
        glm::radians(-180.0f), glm::radians(-90.0f)
    );

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

void utils::rounded_rectangle_intersection_points(Rect rect_1, Rect rect_2, float radius) {

}

Rect utils::window_space_to_opengl_space(Rect rect, glm::vec2 window_size) {
    rect.position = rect.position / window_size * 2.0f;
    rect.position.y *= -1;
    rect.position -= glm::vec2(1.0f, -1.0f);

    rect.size /= window_size;

    return rect;
}
