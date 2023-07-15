#pragma once

#include "nodes/CompleteSpatialNode.hpp"
#include "nodes/gui/GUINode.hpp"

namespace llengine {
class RenderingServer;

class GUICanvas : public CompleteSpatialNode {
public:
    GUICanvas() = default;

    void on_attachment_to_tree() final override;

    void add_child(std::unique_ptr<Node>&& child) override;

    void add_gui_node(std::unique_ptr<GUINode>&& gui_node);
    /**
     * @brief Removes a root GUI node from canvas.
     * 
     * @throws std::out_of_range if the specified index is out of range.
     */
    void remove_gui_node(std::size_t index);
    /**
     * @brief Removes a root GUI node from canvas.
     * 
     * @throws std::invalid_argument if the specified node is not a child of this canvas.
     */
    void remove_gui_node(GUINode* gui_node);

    /**
     * @brief Returns the canvas size in pixels.
     */
    [[nodiscard]] glm::vec2 get_size() const;

    [[nodiscard]] glm::mat4 get_mvp_matrix() const;

    [[nodiscard]] bool is_screen_overlayed() const {
        return screen_overlayed;
    }

    void set_screen_overlayed(bool screen_overlayed) {
        this->screen_overlayed = screen_overlayed;
    }

    void set_screen_overlayed_property(const NodeProperty& property);

    void internal_update() override final;

    static void register_properties();

private:
    bool screen_overlayed = true;
    std::optional<glm::vec2> canvas_size;
    std::vector<std::unique_ptr<GUINode>> gui_nodes;
};
};