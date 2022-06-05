#include <cmath>

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
IntersectionCount QuadrantArc::intersection_points(const QuadrantArc& other,
                                                   glm::vec2& point_1, glm::vec2& point_2) {
    assert(quadrant >= 1 && quadrant <= 4);
    assert(other.quadrant >= 1 && other.quadrant <= 4);
    assert(circle.radius == other.circle.radius);

    IntersectionCount circles_inter = circle.intersection_points(other.circle, point_1, point_2);

    switch (circles_inter) {
        case IntersectionCount::NO_INTERSECTION:
            // There is no need to filter out the intersection points if there is no
            // intersection points.
            return IntersectionCount::NO_INTERSECTION;
        case IntersectionCount::INFINITE_POINTS:
            // Special case. Even if the circles are the same, arcs still can intersect
            // in only one point, infinite points or without intersection at all.
            if (quadrant == other.quadrant) {
                return IntersectionCount::INFINITE_POINTS;
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

                return IntersectionCount::ONE_POINT;
            }
            else {
                return IntersectionCount::NO_INTERSECTION;
            }
        default:
            bool point_1_valid = is_in_quadrant(circle.center, point_1, quadrant) &&
                is_in_quadrant(other.circle.center, point_1, other.quadrant);

            if (circles_inter == IntersectionCount::ONE_POINT && point_1_valid)
                return IntersectionCount::ONE_POINT;

            bool point_2_valid = is_in_quadrant(circle.center, point_2, quadrant) &&
                is_in_quadrant(other.circle.center, point_2, other.quadrant);

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
