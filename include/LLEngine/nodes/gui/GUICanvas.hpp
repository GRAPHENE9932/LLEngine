#pragma once

#include "nodes/CompleteSpatialNode.hpp"
#include "nodes/gui/GUINode.hpp"

namespace llengine {
class RenderingServer;

class GUICanvas : public CompleteSpatialNode {
public:
    GUICanvas(RenderingServer& rs, const Transform& transform = Transform());

    void add_gui_node(std::unique_ptr<GUINode>&& gui_node);
    /**
     * @brief Removes a root GUI node from canvas.
     * 
     * @returns true if removal was successful.
     * @returns false if specified index is out of range.
     */
    bool remove_gui_node(std::size_t index);
    /**
     * @brief Removes a root GUI node from canvas.
     * 
     * @returns true if removal was successful.
     * @returns false if specified GUI node was not found.
     */
    bool remove_gui_node(GUINode* const gui_node);

    /**
     * @brief Returns the canvas size in pixels.
     */
    [[nodiscard]] glm::vec2 get_size() const;

    void update() override final;

private:
    RenderingServer& rs;
    std::optional<glm::vec2> canvas_size;
    std::vector<std::unique_ptr<GUINode>> gui_nodes;
};
};