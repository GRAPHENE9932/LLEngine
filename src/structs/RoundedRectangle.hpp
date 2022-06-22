#pragma once

#include <array>
#include <memory>

#include "../consts_and_enums.hpp"
#include "QuadrantArc.hpp"
#include "HorLS.hpp"
#include "VertLS.hpp"
#include "Rect.hpp"

struct RoundedRectangle {
public:
    struct Decomposed {
        std::array<QuadrantArc, 4> arcs;
        std::array<HorLS, 2> hor_lines;
        std::array<VertLS, 2> vert_lines;
        
        uint8_t intersection_points(const Decomposed& other,
                std::array<glm::vec2, 4>& points) const;
    };

    RoundedRectangle(const Rect& rect, const float radius) : rect(rect), radius(radius) {}

    uint8_t intersection_points(RoundedRectangle& other, std::array<glm::vec2, 4>& points);
    glm::vec2 closest_point(glm::vec2 point, float* const min_distance_out = nullptr) const;

    const Decomposed& get_decomposed();
    bool operator==(const RoundedRectangle& other) const;

private:
    Rect rect;
    float radius;
    std::unique_ptr<Decomposed> decomposed = nullptr;

    void decompose();
    inline const Rect& get_rect() const {
        return rect;
    };
    inline void set_rect(const Rect& new_rect) {
        // Discard the cached decomposed rounded rectangle if
        // the rounded rectangle itself is changed.
        if (decomposed != nullptr && rect != new_rect)
            decomposed = nullptr;

        rect = new_rect;
    }
};
