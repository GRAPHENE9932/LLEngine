#pragma once

#include "GUINode.hpp"
#include "common/core/Mesh.hpp"
#include "common/core/FreeTypeFont.hpp"

#include <vector>
#include <memory>
#include <limits>
#include <optional>

class TextNode : public GUINode {
public:
    enum class OriginX : std::uint8_t {
        LEFT, CENTER, RIGHT
    };
    enum class OriginY : std::uint8_t {
        BOTTOM, CENTER, TOP
    };

    TextNode(RenderingServer& rs, const std::shared_ptr<FreeTypeFont>& font);

    void set_text(std::string_view new_text);
    void set_color(const glm::vec3& new_color) noexcept {
        color = new_color;
    }

    [[nodiscard]] glm::vec3 get_color() const noexcept {
        return color;
    }

    void set_origin_x(OriginX new_origin_x) noexcept {
        origin_x = new_origin_x;
    }
    void set_origin_y(OriginY new_origin_y) noexcept {
        origin_y = new_origin_y;
    }

    void draw() override;

private:
    std::shared_ptr<FreeTypeFont> font = nullptr;
    glm::vec3 color = {0.0f, 0.0f, 0.0f};
    OriginX origin_x = OriginX::LEFT;
    OriginY origin_y = OriginY::BOTTOM;

    std::optional<Mesh> mesh = std::nullopt;
    std::vector<std::reference_wrapper<const FreeTypeFont::FontChar>> chars;
    std::int32_t min_x = std::numeric_limits<std::int32_t>::max();
    std::int32_t max_x = std::numeric_limits<std::int32_t>::min();
    std::int32_t min_y = std::numeric_limits<std::int32_t>::max();
    std::int32_t max_y = std::numeric_limits<std::int32_t>::min();

    [[nodiscard]] glm::vec2 get_origin() const;
};