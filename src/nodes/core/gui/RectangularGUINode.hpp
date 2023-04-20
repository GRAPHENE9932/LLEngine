#pragma once

#include "GUINode.hpp"

class RectangularGUINode : public GUINode {
public:
    enum class OriginX : std::uint8_t {
        LEFT, CENTER, RIGHT
    };
    enum class OriginY : std::uint8_t {
        BOTTOM, CENTER, TOP
    };

    RectangularGUINode(RenderingServer& rs) : GUINode(rs) {}

    void set_origin_x(OriginX new_origin_x) noexcept {
        origin_x = new_origin_x;
    }
    void set_origin_y(OriginY new_origin_y) noexcept {
        origin_y = new_origin_y;
    }

    OriginX origin_x = OriginX::LEFT;
    OriginY origin_y = OriginY::BOTTOM;
};