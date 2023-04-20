#pragma once

#include "common/core/Mesh.hpp"
#include "RectangularGUINode.hpp"
#include "common/core/FreeTypeFont.hpp"

#include <vector>
#include <memory>
#include <limits>
#include <optional>

class TextNode : public RectangularGUINode {
public:
    TextNode(RenderingServer& rs, const std::shared_ptr<FreeTypeFont>& font);

    void set_text(std::string_view new_text);
    void set_color(const glm::vec3& new_color) noexcept {
        color = new_color;
    }

    [[nodiscard]] glm::vec3 get_color() const noexcept {
        return color;
    }

    void draw() override;
    [[nodiscard]] virtual glm::vec2 get_size() const override;
    [[nodiscard]] glm::vec2 get_origin() const override;

private:
    std::shared_ptr<FreeTypeFont> font = nullptr;
    glm::vec3 color = {0.0f, 0.0f, 0.0f};

    std::optional<Mesh> mesh = std::nullopt;
    std::vector<std::reference_wrapper<const FreeTypeFont::FontChar>> chars;
    std::int32_t min_x = std::numeric_limits<std::int32_t>::max();
    std::int32_t max_x = std::numeric_limits<std::int32_t>::min();
    std::int32_t min_y = std::numeric_limits<std::int32_t>::max();
    std::int32_t max_y = std::numeric_limits<std::int32_t>::min();
};