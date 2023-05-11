#pragma once

#include "rendering/Mesh.hpp"
#include "GUINode.hpp"
#include "gui/FreeTypeFont.hpp"

#include <vector>
#include <memory>
#include <limits>
#include <optional>

class TextNode : public GUINode {
public:
    TextNode(RenderingServer& rs, const std::shared_ptr<FreeTypeFont>& font);

    [[nodiscard]] GUITransform get_transform() const override;
    [[nodiscard]] glm::vec2 get_absolute_size() const override;
    void set_transform(const GUITransform& transform) override;

    void set_text(std::string_view new_text);
    void set_color(const glm::vec3& new_color) noexcept {
        color = new_color;
    }

    [[nodiscard]] glm::vec3 get_color() const noexcept {
        return color;
    }

    void draw() override;

private:
    glm::vec2 position_anchor {0.0f, 0.0f};
    glm::vec2 position_offset {0.0f, 0.0f};
    float z_coordinate {0.0f};
    GUITransform::OriginX origin_x = GUITransform::OriginX::LEFT;
    GUITransform::OriginY origin_y = GUITransform::OriginY::BOTTOM;

    std::shared_ptr<FreeTypeFont> font = nullptr;
    glm::vec3 color = {0.0f, 0.0f, 0.0f};

    std::optional<Mesh> mesh = std::nullopt;
    std::vector<std::reference_wrapper<const FreeTypeFont::FontChar>> chars;
    std::int32_t min_x = std::numeric_limits<std::int32_t>::max();
    std::int32_t max_x = std::numeric_limits<std::int32_t>::min();
    std::int32_t min_y = std::numeric_limits<std::int32_t>::max();
    std::int32_t max_y = std::numeric_limits<std::int32_t>::min();
};