#pragma once

#include <memory>

#include "structs/RoundedRectangle.hpp"
#include "structs/Cylinder.hpp"
#include "structs/Rect.hpp"

class RectangularPhysicsObject {
public:
    virtual bool cylinder_intersects(const Cylinder& cylinder) const = 0;

    /// @returns Distance from rect to cylinder. Can be negative if cylinder
    /// intersects with rect.
    virtual float horizontal_distance_from_cylinder(const Cylinder& cylinder) const;

    inline const RoundedRectangle& get_rounded_rect(const float radius) const {
        if (cached_rr == nullptr)
            cached_rr = std::make_unique<RoundedRectangle>(get_rect(), radius);

        return *cached_rr;
    }

    inline const Rect& get_rect() const {
        return rect;
    }
    inline void set_rect(const Rect& new_rect) {
        // Discard the cached rounded rectangle if
        // the rectangle itself is changed.
        if (cached_rr != nullptr && get_rect() != new_rect)
            cached_rr = nullptr;

        rect = new_rect;
    }

private:
    Rect rect;
    mutable std::unique_ptr<RoundedRectangle> cached_rr = nullptr;
};
