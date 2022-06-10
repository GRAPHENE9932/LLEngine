#pragma once

#include <array>
#include <memory>

#include "../consts_and_enums.hpp"
#include "QuadrantArc.hpp"
#include "HorLine.hpp"
#include "VertLine.hpp"
#include "Rect.hpp"

struct RoundedRectangle {
public:
    struct Decomposed {
        std::array<QuadrantArc, 4> arcs;
        std::array<HorLine, 2> hor_lines;
        std::array<VertLine, 2> vert_lines;
        
        IntersectionCount intersection_points(const Decomposed& other,
                                              std::array<glm::vec2, 4>& points) const;
    };

    Rect rect;
    float radius;

    RoundedRectangle(const Rect& rect, const float radius) : rect(rect), radius(radius) {}

    IntersectionCount intersection_points(RoundedRectangle& other, std::array<glm::vec2, 4>& points);
    glm::vec2 closest_point(glm::vec2 point, float* const min_distance_out = nullptr) const;

    const Decomposed& get_decomposed();
    bool operator==(const RoundedRectangle& other) const;

private:
    std::unique_ptr<Decomposed> decomposed = nullptr;

    void decompose();
};
