#pragma once

#include "gui/GUITransform.hpp"
#include "nodes/Node.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <variant>

namespace llengine {
class Texture;
class GUICanvas;
class GUITexture;
class RenderingServer;

class GUINode : public Node {
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

    ~GUINode();

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
    void set_transform_property(const NodeProperty& property);

    /**
     * @param point The point position in screen (pixel) coordinates.
     * (top left is 0, 0).
     */
    [[nodiscard]] bool contains_point(glm::vec2 point) const;

    void add_child(std::unique_ptr<Node>&& child) override;
    void add_child(std::unique_ptr<GUINode>&& child);

    [[nodiscard]] inline const std::vector<std::unique_ptr<GUINode>>& get_children() const {
        return children;
    }

    [[nodiscard]] bool is_attached_to_tree() const override;

    virtual void draw() {}

    virtual void update() {
        update_children();
    }

    /**
     * @brief Get the parent GUI node if exists.
     * 
     * @returns Non-owning pointer to the parent,
     * or nullptr if this GUI node is root or
     * doesn't have a parent GUI node for another reason.
     */
    [[nodiscard]] GUINode* get_parent() const;
    /**
     * @brief Get the parent size.
     *
     * May throw if the current GUI node doesn't have neither canvas or parent.
     * 
     * @returns Size of the parent or size of the canvas, if
     * this node doesn't have a parent.
     */
    [[nodiscard]] glm::vec2 get_parent_size() const;

    /**
     * @brief Get the GUI canvas if assigned.
     * 
     * May throw if the current GUI node and it's parent doesn't
     * have a canvas assigned.
     */
    [[nodiscard]] GUICanvas& get_canvas() const;

    void assign_canvas_parent(GUICanvas& canvas);

    void draw_children();
    void update_children();

    static void register_properties();

protected:
    void draw_rectangle(const GUITexture& texture);
    void on_attachment_to_tree() override final;

private:
    std::variant<GUINode*, GUICanvas*, std::monostate> parent = std::monostate();
    std::vector<std::unique_ptr<GUINode>> children;

    void draw_texture_part(
        const Texture& texture, glm::vec2 pos_offset_in_px, glm::vec2 tex_offset_in_px,
        glm::vec2 quad_size_in_px, glm::vec2 tex_part_size_in_px
    );

    friend class GUICanvas;
};
}