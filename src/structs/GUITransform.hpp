#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct GUITransform {
    enum class SizeMode : std::uint8_t {
        ABSOLUTE, RELATIVE
    };
    enum class OriginX : std::uint8_t {
        LEFT, CENTER, RIGHT
    };
    enum class OriginY : std::uint8_t {
        BOTTOM, CENTER, TOP
    };

    glm::vec2 position_anchor {0.0f, 0.0f}; // (0, 0) is left-top, (1, 1) is right-bottom.
    glm::vec2 position_offset {0.0f, 0.0f}; // Offset from anchor in pixels.
    float z_coordinate {0.0f};
    OriginX origin_x = OriginX::LEFT;
    OriginY origin_y = OriginY::BOTTOM;
    SizeMode size_mode = SizeMode::ABSOLUTE;
    glm::vec2 size {0.0f, 0.0f};

    [[nodiscard]] glm::vec3 get_screen_space_offset(glm::vec2 parent_size) const noexcept {
        glm::vec2 result_vec2 {position_offset};
        result_vec2 += position_anchor * static_cast<glm::vec2>(parent_size);

        if (origin_x == OriginX::CENTER) {
            result_vec2.x -= size.x / 2.0f;
        }
        else if (origin_x == OriginX::RIGHT) {
            result_vec2.x -= size.x;
        }

        if (origin_y == OriginY::CENTER) {
            result_vec2.y -= size.y / 2.0f;
        }
        else if (origin_y == OriginY::BOTTOM) {
            result_vec2.y -= size.y;
        }

        return {result_vec2, z_coordinate};
    }
};