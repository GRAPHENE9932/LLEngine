#pragma once

#include "gui/GUITransform.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

class Texture;
class GUITexture;
class RenderingServer;

class GUINode {
public:
    enum class SizeMode : std::uint8_t {
        ABSOLUTE, RELATIVE
    };
    enum class OriginX : std::uint8_t {
        LEFT, CENTER, RIGHT
    };
    enum class OriginY : std::uint8_t {
        BOTTOM, CENTER, TOP
    };

    explicit GUINode(RenderingServer& rs);
    virtual ~GUINode();

    [[nodiscard]] virtual GUITransform get_transform() const = 0;
    [[nodiscard]] virtual glm::vec2 get_absolute_size() const = 0;
    /**
     * @brief Returns the global GUINode position in pixels in screen space.
     * 
     * Top left corner has XY coordinates (0, 0).
     * The Z coordinate is left unchanged relatively to the transform.
     */
    [[nodiscard]] virtual glm::vec3 get_screen_space_position() const;
    virtual void set_transform(const GUITransform& transform) = 0;

    /**
     * @param point The point position in screen (pixel) coordinates.
     * (top left is 0, 0).
     */
    [[nodiscard]] bool contains_point(glm::vec2 point) const;

    void add_child(std::unique_ptr<GUINode>&& child);

    [[nodiscard]] inline const std::vector<std::unique_ptr<GUINode>>& get_children() const {
        return children;
    }

    virtual void draw() {}

    virtual void update() {
        update_children();
    }

    [[nodiscard]] inline GUINode* get_parent() const noexcept {
        return parent;
    }
    [[nodiscard]] glm::vec2 get_parent_size() const;

    void draw_children();
    void update_children();

protected:
    RenderingServer& rs;

    void draw_rectangle(const GUITexture& texture);

private:
    GUINode* parent {nullptr};
    std::string name;
    std::vector<std::unique_ptr<GUINode>> children;

    void draw_texture_part(
        const Texture& texture, glm::vec2 pos_offset_in_px, glm::vec2 tex_offset_in_px,
        glm::vec2 quad_size_in_px, glm::vec2 tex_part_size_in_px
    );
};
