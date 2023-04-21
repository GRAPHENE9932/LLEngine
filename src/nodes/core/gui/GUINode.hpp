#pragma once

#include "structs/GUITransform.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

class RenderingServer;

class GUINode {
public:
    GUINode(RenderingServer& rs);
    virtual ~GUINode();

    [[nodiscard]] inline const GUITransform& get_transform() const noexcept {
        return transform;
    }
    [[nodiscard]] GUITransform get_absolute_transform() const noexcept;

    inline void set_position_anchor(glm::vec2 position_anchor) noexcept {
        transform.position_anchor = position_anchor;
    }
    inline void set_position_offset(glm::vec2 position_offset) noexcept {
        transform.position_offset = position_offset;
    }
    inline void set_z_coordinate(float z_coordinate) noexcept {
        transform.z_coordinate = z_coordinate;
    }
    inline void set_transform(const GUITransform& transform) noexcept {
        this->transform = transform;
    }

    void add_child(std::unique_ptr<GUINode>&& child);
    [[nodiscard]] inline const std::vector<std::unique_ptr<GUINode>>& get_children() const {
        return children;
    }

    virtual void draw() {
        draw_children();
    }

    virtual void update() {
        update_children();
    }

    void draw_children();
    void update_children();

protected:
    RenderingServer& rs;

private:
    GUINode* parent {nullptr};
    std::string name;
    std::vector<std::unique_ptr<GUINode>> children;
    GUITransform transform;
};
