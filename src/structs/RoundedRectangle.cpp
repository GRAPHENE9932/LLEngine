#include <glm/geometric.hpp>

#include "../utils/math.hpp"
#include "QuadrantArc.hpp"
#include "HorLine.hpp"
#include "VertLine.hpp"
#include "RoundedRectangle.hpp"

IntersectionCount RoundedRectangle::intersection_points(RoundedRectangle& other,
    std::array<glm::vec2, 4>& points) {
    return {get_decomposed().intersection_points(other.get_decomposed(), points)};
}

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

const RoundedRectangle::Decomposed& RoundedRectangle::get_decomposed() {
    if (decomposed == nullptr)
        decompose();

    return *decomposed.get();
}

bool RoundedRectangle::operator==(const RoundedRectangle& other) const {
    return this->rect == other.rect && this->radius == other.radius;
}

void RoundedRectangle::decompose() {
    decomposed = std::make_unique<Decomposed>();

    decomposed->arcs[0] = {{rect.position, radius}, 3};
    decomposed->arcs[1] = {{{rect.position.x + rect.size.x, rect.position.y}, radius}, 4};
    decomposed->arcs[2] = {{{rect.position.x, rect.position.y + rect.size.y}, radius}, 2};
    decomposed->arcs[3] = {{{rect.position.x + rect.size.x, rect.position.y + rect.size.y}, radius}, 1};

    decomposed->vert_lines[0] = {
        rect.position.y, rect.position.y + rect.size.y,
        rect.position.x - radius
    };
    decomposed->vert_lines[1] = {
        rect.position.y, rect.position.y + rect.size.y,
        rect.position.x + rect.size.x + radius
    };

    decomposed->hor_lines[0] = {
        rect.position.x, rect.position.x + rect.size.x,
        rect.position.y - radius
    };
    decomposed->hor_lines[1] = {
        rect.position.x, rect.position.x + rect.size.x,
        rect.position.y + rect.size.y + radius
    };
}

inline IntersectionCount intersection_points(const HorLine& hor_line_1, const HorLine& hor_line_2,
                                             std::vector<glm::vec2>& points, const bool include_edges = false) {
    glm::vec2 point_1;
    auto result = hor_line_1.intersection_points(hor_line_2, point_1, include_edges);

    if (result == IntersectionCount::ONE_POINT)
        points.push_back(point_1);
    
    return result;
}

inline IntersectionCount intersection_points(const HorLine& hor_line, const VertLine& vert_line,
                                             std::vector<glm::vec2>& points, const bool include_edges = false) {
    glm::vec2 point_1;
    auto result = vert_line.intersection_points(hor_line, point_1, include_edges);

    if (result == IntersectionCount::ONE_POINT)
        points.push_back(point_1);

    return result;
}

IntersectionCount RoundedRectangle::Decomposed::intersection_points(const Decomposed& other,
                                                                    std::array<glm::vec2, 4>& points) const {
    uint8_t count {0};

    // Arcs.
    for (uint8_t i = 0; i < 4; i++) {
        // Arcs - arcs.
        for (uint8_t j = 0; j < 4; j++) {
            glm::vec2 point_1, point_2;
            auto res {arcs[i].intersection_points(other.arcs[j], point_1, point_2)};

            switch (res) {
            case IntersectionCount::ONE_POINT:
                points[count] = point_1;
                count++;
                if (count >= 4)
                    return IntersectionCount::FOUR_POINTS;
                break;
            case IntersectionCount::TWO_POINTS:
                points[count] = point_1;
                count++;
                points[count] = point_2;
                count++;
                if (count >= 4)
                    return IntersectionCount::FOUR_POINTS;
                break;
            case IntersectionCount::INFINITE_POINTS:
                return IntersectionCount::INFINITE_POINTS;
            }
        }

        // Arcs - horizontal line segments.
        for (uint8_t j = 0; j < 2; j++) {
            glm::vec2 point_1;
            auto res {arcs[i].intersection_points(other.hor_lines[j], point_1)};

            switch (res) {
            case IntersectionCount::ONE_POINT:
                points[count] = point_1;
                count++;
                if (count >= 4)
                    return IntersectionCount::FOUR_POINTS;
                break;
            case IntersectionCount::INFINITE_POINTS:
                return IntersectionCount::INFINITE_POINTS;
            }
        }

        // Arcs - vertical line segments.
        for (uint8_t j = 0; j < 2; j++) {
            glm::vec2 point_1;
            auto res {arcs[i].intersection_points(other.vert_lines[j], point_1)};

            switch (res) {
            case IntersectionCount::ONE_POINT:
                points[count] = point_1;
                count++;
                if (count >= 4)
                    return IntersectionCount::FOUR_POINTS;
                break;
            case IntersectionCount::INFINITE_POINTS:
                return IntersectionCount::INFINITE_POINTS;
            }
        }
    }

    // Vertical line segments.
    for (uint8_t i = 0; i < 2; i++) {
        // Vertical line segments - arcs.
        for (uint8_t j = 0; j < 4; j++) {
            glm::vec2 point_1;
            auto res {other.arcs[j].intersection_points(vert_lines[i], point_1)};

            switch (res) {
            case IntersectionCount::ONE_POINT:
                points[count] = point_1;
                count++;
                if (count >= 4)
                    return IntersectionCount::FOUR_POINTS;
                break;
            case IntersectionCount::INFINITE_POINTS:
                return IntersectionCount::INFINITE_POINTS;
            }
        }

        // Vertical line segments - vertical line segments.
        for (uint8_t j = 0; j < 2; j++) {
            glm::vec2 point_1;
            auto res {vert_lines[i].intersection_points(other.vert_lines[j], point_1, false)};

            switch (res) {
            case IntersectionCount::ONE_POINT:
                points[count] = point_1;
                count++;
                if (count >= 4)
                    return IntersectionCount::FOUR_POINTS;
                break;
            case IntersectionCount::INFINITE_POINTS:
                return IntersectionCount::INFINITE_POINTS;
            }
        }

        // Vertical line segments - horizontal line segments.
        for (uint8_t j = 0; j < 2; j++) {
            glm::vec2 point_1;
            auto res {vert_lines[i].intersection_points(other.hor_lines[j], point_1, false)};

            switch (res) {
            case IntersectionCount::ONE_POINT:
                points[count] = point_1;
                count++;
                if (count >= 4)
                    return IntersectionCount::FOUR_POINTS;
                break;
            case IntersectionCount::INFINITE_POINTS:
                return IntersectionCount::INFINITE_POINTS;
            }
        }
    }

    // Horizontal line segments.
    for (uint8_t i = 0; i < 2; i++) {
        // Horizontal line segments - arcs.
        for (uint8_t j = 0; j < 4; j++) {
            glm::vec2 point_1;
            auto res {other.arcs[j].intersection_points(hor_lines[i], point_1)};

            switch (res) {
            case IntersectionCount::ONE_POINT:
                points[count] = point_1;
                count++;
                if (count >= 4)
                    return IntersectionCount::FOUR_POINTS;
                break;
            case IntersectionCount::INFINITE_POINTS:
                return IntersectionCount::INFINITE_POINTS;
            }
        }

        // Horizontal line segments - vertical line segments.
        for (uint8_t j = 0; j < 2; j++) {
            glm::vec2 point_1;
            auto res {other.vert_lines[j].intersection_points(hor_lines[i], point_1, false)};

            switch (res) {
            case IntersectionCount::ONE_POINT:
                points[count] = point_1;
                count++;
                if (count >= 4)
                    return IntersectionCount::FOUR_POINTS;
                break;
            case IntersectionCount::INFINITE_POINTS:
                return IntersectionCount::INFINITE_POINTS;
            }
        }

        // Horizontal line segments - horizontal line segments.
        for (uint8_t j = 0; j < 2; j++) {
            glm::vec2 point_1;
            auto res {hor_lines[i].intersection_points(other.hor_lines[j], point_1, false)};

            switch (res) {
            case IntersectionCount::ONE_POINT:
                points[count] = point_1;
                count++;
                if (count >= 4)
                    return IntersectionCount::FOUR_POINTS;
                break;
            case IntersectionCount::INFINITE_POINTS:
                return IntersectionCount::INFINITE_POINTS;
            }
        }
    }

    switch (count) {
    case 0:
        return IntersectionCount::NO_INTERSECTION;
    case 1:
        return IntersectionCount::ONE_POINT;
    case 2:
        return IntersectionCount::TWO_POINTS;
    case 3:
        return IntersectionCount::THREE_POINTS;
    default: // 4.
        return IntersectionCount::FOUR_POINTS;
    }
}
