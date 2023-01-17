#pragma once

#include "utils/Event.hpp"

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <string_view>

class SpatialNode {
public:
    virtual ~SpatialNode();

    void set_name(std::string_view new_name);
    [[nodiscard]] const std::string& get_name() const;

    void update_children();
    void add_child(SpatialNode&& child);
    void remove_child(const size_t index);
    void remove_child(SpatialNode* const ptr);
    [[nodiscard]] SpatialNode* get_parent() const;

    const std::vector<std::unique_ptr<SpatialNode>>& get_children() const;

    virtual void set_translation(const glm::vec3& new_trans) = 0;
    virtual void translate(const glm::vec3& translation) = 0;
    virtual void set_scale(const glm::vec3& new_scale) = 0;
    virtual void set_rotation(const glm::quat& new_rotation) = 0;

    [[nodiscard]] virtual glm::vec3 get_translation() const noexcept = 0;
    [[nodiscard]] virtual glm::vec3 get_global_position() const noexcept = 0;
    [[nodiscard]] virtual glm::vec3 get_scale() const noexcept = 0;
    [[nodiscard]] virtual glm::vec3 get_global_scale() const noexcept = 0;
    [[nodiscard]] virtual glm::quat get_rotation() const noexcept = 0;
    [[nodiscard]] virtual glm::quat get_global_rotation() const noexcept = 0;
    [[nodiscard]] virtual glm::mat4 get_local_matrix() const noexcept = 0;
    [[nodiscard]] virtual glm::mat4 get_global_matrix() const noexcept = 0;

    virtual void update();

private:
    SpatialNode* parent {nullptr};
    std::string name;
    /**
     * This vector MUST NOT be changed outside the
     * add_child and remove_child functions.
    */
    std::vector<std::unique_ptr<SpatialNode>> children;
};
