#include <cmath>

#include <glm/geometric.hpp>

#include "QuadrantArc.hpp"

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

/// Basically, this function finds the intersection points of the two given circles
/// and then filters out intersection points that doesn't lie on the both arcs.
uint8_t QuadrantArc::intersection_points(const QuadrantArc& other,
                                         glm::vec2& point_1, glm::vec2& point_2) const {
    assert(quadrant >= 1 && quadrant <= 4);
    assert(other.quadrant >= 1 && other.quadrant <= 4);
    assert(circle.radius == other.circle.radius);

    uint8_t circles_inter = circle.intersection_points(other.circle, point_1, point_2);

    switch (circles_inter) {
    case 0:
        // There is no need to filter out the intersection points if there is no
        // intersection points.
        return 0;
    case INFINITE_POINTS:
        // Special case. Even if the circles are the same, arcs still can intersect
        // in only one point, infinite points or without intersection at all.
        if (quadrant == other.quadrant) {
            return INFINITE_POINTS;
        }
        else if (are_quadrants_neighbor(quadrant, other.quadrant)) {
            if (unordered_equal<uint8_t>(quadrant, other.quadrant, 1, 2))
                point_1 = glm::vec2(circle.center.x, circle.center.y + circle.radius);
            else if (unordered_equal<uint8_t>(quadrant, other.quadrant, 2, 3))
                point_1 = glm::vec2(circle.center.x - circle.radius, circle.center.y);
            else if (unordered_equal<uint8_t>(quadrant, other.quadrant, 3, 4))
                point_1 = glm::vec2(circle.center.x, circle.center.y - circle.radius);
            else
                point_1 = glm::vec2(circle.center.x + circle.radius, circle.center.y);

            return 1;
        }
        else {
            return 0;
        }
    default:
        bool point_1_valid = is_in_quadrant(circle.center, point_1, quadrant) &&
            is_in_quadrant(other.circle.center, point_1, other.quadrant);

        if (circles_inter == 1 && point_1_valid)
            return 1;

        bool point_2_valid = is_in_quadrant(circle.center, point_2, quadrant) &&
            is_in_quadrant(other.circle.center, point_2, other.quadrant);

        if (point_1_valid && !point_2_valid) {
            return 1;
        }
        else if (!point_1_valid && point_2_valid) {
            point_1 = point_2;
            return 1;
        }
        else if (!point_1_valid && !point_2_valid) {
            return 0;
        }
        else {
            return 2;
        }
    } // switch.
}

inline bool in_range(const float num, const float min, const float max) {
    assert(min <= max);
    return num >= min && num <= max;
}

uint8_t QuadrantArc::intersection_points(const HorLS& hor_line, glm::vec2& point_1) const {
    assert(hor_line.lower_x <= hor_line.higher_x);

    // Validate y.
    if (quadrant == 1 || quadrant == 2) {
        if (!in_range(hor_line.y, circle.center.y, circle.center.y + circle.radius))
            return 0;
    }
    else {
        if (!in_range(hor_line.y, circle.center.y - circle.radius, circle.center.y))
            return 0;
    }

    // Compute x.
    float cosine {std::cos(std::asin((hor_line.y - circle.center.y) / circle.radius))};
    if (quadrant == 2 || quadrant == 3)
        cosine = -cosine;
    
    float x_coord = cosine * circle.radius + circle.center.x;
    if (!in_range(x_coord, hor_line.lower_x, hor_line.higher_x)) {
        return 0;
    }
    else {
        point_1 = {x_coord, hor_line.y};
        return 1;
    }
}

uint8_t QuadrantArc::intersection_points(const VertLS& vert_line, glm::vec2& point_1) const {
    assert(vert_line.lower_y <= vert_line.higher_y);

    // Validate x.
    if (quadrant == 1 || quadrant == 4) {
        if (!in_range(vert_line.x, circle.center.x, circle.center.x + circle.radius))
            return 0;
    }
    else {
        if (!in_range(vert_line.x, circle.center.x - circle.radius, circle.center.x))
            return 0;
    }

    // Compute y.
    float sine {std::sin(std::acos((vert_line.x - circle.center.x) / circle.radius))};
    if (quadrant == 3 || quadrant == 4)
        sine = -sine;
    
    float y_coord = sine * circle.radius + circle.center.y;
    if (!in_range(y_coord, vert_line.lower_y, vert_line.higher_y)) {
        return 0;
    }
    else {
        point_1 = {vert_line.x, y_coord};
        return 1;
    }
}

/// Returns angle in range from -PI to PI.
inline float normalize_angle(const float angle) {
    return std::fmod(angle + M_PIf, 2.0f * M_PIf) - M_PIf;
}

/// Accepts min and max angles from -PI to PI.
inline float clamp_angle(const float angle, const float min, const float max) {
    if (angle < max && angle > min)
        return angle;

    if (std::abs(normalize_angle(angle - min)) < std::abs(normalize_angle(angle - max)))
        return min;
    else
        return max;
}

inline float clamp_quadrant_angle(const float angle, const uint8_t quadrant) {
    switch (quadrant) {
        case 1:
            return clamp_angle(angle, 0.0f, M_PI_2f);
        case 2:
            return clamp_angle(angle, M_PI_2f, M_PIf);
        case 3:
            return clamp_angle(angle, -M_PIf, -M_PI_2f);
        default: // 4.
            return clamp_angle(angle, -M_PI_2, 0.0f);
    }
}

glm::vec2 QuadrantArc::closest_point(glm::vec2 point) const {
    // Going to the circle center space.
    point -= circle.center;

    glm::vec2 point_on_trigonometric_circle = point / glm::length(point);
    float angle = std::atan2(point_on_trigonometric_circle.y, point_on_trigonometric_circle.x);
    angle = clamp_quadrant_angle(angle, quadrant);

    return glm::vec2(std::cos(angle) * circle.radius, std::sin(angle) * circle.radius) + circle.center;
}