#include <array>
#include <algorithm>

#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>

#include "math.hpp"

constexpr float ASSERT_THRESHOLD = 0.00001f;

glm::vec2 utils::closest_point_to_hor_line_segment(glm::vec2 point, float point_a_x, float point_b_x, float point_a_b_y) {
    assert(point_a_x < point_b_x);
    //      *---------*
    //   ^       ^       ^
    //   2       1       3

    // Case 1.
    if (point.x >= point_a_x && point.x <= point_b_x) {
        point.y = point_a_b_y;
        return point;
    }
    // Case 2.
    else if (point.x < point_a_x) {
        return glm::vec2(point_a_x, point_a_b_y);
    }
    // Case 3.
    else {
        return glm::vec2(point_b_x, point_a_b_y);
    }
}

glm::vec2 utils::closest_point_to_ver_line_segment(glm::vec2 point, float point_a_y, float point_b_y, float point_a_b_x) {
    assert(point_a_y < point_b_y);
    //   < 2
    // *
    // |
    // | < 1
    // |
    // *
    //   < 3

    // Case 1.
    if (point.y >= point_a_y && point.y <= point_b_y) {
        point.x = point_a_b_x;
        return point;
    }
    // Case 2.
    else if (point.y < point_a_y) {
        return glm::vec2(point_a_b_x, point_a_y);
    }
    // Case 3.
    else {
        return glm::vec2(point_a_b_x, point_b_y);
    }
}

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

    closest_points[0] = closest_point_to_hor_line_segment(
        point, rect.position.x, rect.position.x + rect.size.x,
        rect.position.y - radius
    );
    closest_points[1] = closest_point_to_ver_line_segment(
        point, rect.position.y, rect.position.y + rect.size.y,
        rect.position.x + rect.size.x + radius
    );
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

utils::IntersectionCount utils::two_circles_intersection_points(const glm::vec2 center_1, const glm::vec2 center_2,
                                                                const float radius, glm::vec2& point_1, glm::vec2& point_2) {
    if (center_1 == center_2)
        return IntersectionCount::INFINITE_POINTS;

    glm::vec2 vector_a = (center_2 - center_1) * 0.5f;
    float vector_a_len = glm::length(vector_a);

    if (vector_a_len > radius)
        return IntersectionCount::NO_INTERSECTION;

    float vector_b_len = std::sqrt(radius * radius - vector_a_len * vector_a_len);
    glm::vec2 vector_b {
        glm::vec2(-vector_a.y, vector_a.x) / vector_a_len * vector_b_len
    };
    point_1 = vector_b + center_1 + vector_a;

    if (vector_a_len == radius)
        return IntersectionCount::ONE_POINT;

    point_2 = -vector_b + center_1 + vector_a;
    return IntersectionCount::TWO_POINTS;
}

inline bool is_in_quadrant(const glm::vec2& center, const glm::vec2& point, const uint8_t& quadrant) noexcept {
    switch (quadrant) {
        case 1:
            return point.x >= center.x && point.y >= center.y;
        case 2:
            return point.x < center.x && point.y >= center.y;
        case 3:
            return point.x < center.x && point.y < center.y;
        default: // 4.
            return point.x >= center.x && point.y < center.y;
    }
}

inline bool are_quadrants_neighbor(const uint8_t quadrant_1, const uint8_t quadrant_2) {
    return std::abs(quadrant_2 - quadrant_1) == 1 ||
        (quadrant_1 == 1 && quadrant_2 == 4) ||
        (quadrant_1 == 4 && quadrant_2 == 1);
}

template<typename T>
inline bool unordered_equal(const T& a_1, const T& b_1, const T& a_2, const T& b_2) {
    return (a_1 == a_2 && b_1 == b_2) || (a_1 == b_2 && b_1 == a_2);
}

// Basically, this function finds the intersection points of the two given circles
// and then filters out intersection points that doesn't lie on the both arcs.
utils::IntersectionCount utils::two_arcs_intersection_points(const glm::vec2 center_1, const uint8_t arc_quadrant_1,
                                                             const glm::vec2 center_2, const uint8_t arc_quadrant_2,
                                                             const float radius, glm::vec2& point_1, glm::vec2& point_2) {
    assert(arc_quadrant_1 >= 1 && arc_quadrant_1 <= 4);
    assert(arc_quadrant_2 >= 1 && arc_quadrant_2 <= 4);

    IntersectionCount circles_inter = two_circles_intersection_points(center_1, center_2,
                                                                      radius, point_1, point_2);

    switch (circles_inter) {
        case IntersectionCount::NO_INTERSECTION:
            // There is no need to filter out the intersection points if there is no
            // intersection points.
            return IntersectionCount::NO_INTERSECTION;
        case IntersectionCount::INFINITE_POINTS:
            // Special case. Even if the circles are the same, arcs still can intersect
            // in only one point, infinite points or without intersection at all.
            if (arc_quadrant_1 == arc_quadrant_2) {
                return IntersectionCount::INFINITE_POINTS;
            }
            else if (are_quadrants_neighbor(arc_quadrant_1, arc_quadrant_2)) {
                if (unordered_equal<uint8_t>(arc_quadrant_1, arc_quadrant_2, 1, 2))
                    point_1 = glm::vec2(center_1.x, center_1.y + radius);
                else if (unordered_equal<uint8_t>(arc_quadrant_1, arc_quadrant_2, 2, 3))
                    point_1 = glm::vec2(center_1.x - radius, center_1.y);
                else if (unordered_equal<uint8_t>(arc_quadrant_1, arc_quadrant_2, 3, 4))
                    point_1 = glm::vec2(center_1.x, center_1.y - radius);
                else
                    point_1 = glm::vec2(center_1.x + radius, center_1.y);

                return IntersectionCount::ONE_POINT;
            }
            else {
                return IntersectionCount::NO_INTERSECTION;
            }
        default:
            bool point_1_valid = is_in_quadrant(center_1, point_1, arc_quadrant_1) && is_in_quadrant(center_2, point_1, arc_quadrant_2);

            if (circles_inter == IntersectionCount::ONE_POINT && point_1_valid)
                return IntersectionCount::ONE_POINT;

            bool point_2_valid = is_in_quadrant(center_1, point_2, arc_quadrant_1) && is_in_quadrant(center_2, point_2, arc_quadrant_2);

            if (point_1_valid && !point_2_valid) {
                return IntersectionCount::ONE_POINT;
            }
            else if (!point_1_valid && point_2_valid) {
                point_1 = point_2;
                return IntersectionCount::ONE_POINT;
            }
            else if (!point_1_valid && !point_2_valid) {
                return IntersectionCount::NO_INTERSECTION;
            }
            else {
                return IntersectionCount::TWO_POINTS;
            }
    } // switch.
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
