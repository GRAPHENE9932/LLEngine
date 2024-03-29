#pragma once

#include "Transform.hpp"
#include "nodes/Node.hpp"

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>
#include <vector>

namespace llengine {
class SpatialNode : public Node {
public:
    SpatialNode() = default;
    SpatialNode(const SpatialNode& other) = delete;
    SpatialNode(SpatialNode&& other) = delete;
    SpatialNode& operator=(const SpatialNode& other) = delete;
    SpatialNode& operator=(SpatialNode&& other) = delete;
    virtual ~SpatialNode();

    void update_children();
    void queue_add_child(std::unique_ptr<Node>&& child) override;
    void queue_add_child(std::unique_ptr<SpatialNode>&& child);
    void queue_remove_child(const size_t index);
    void queue_remove_child(SpatialNode* const ptr);
    [[nodiscard]] SpatialNode* get_parent() const;

    [[nodiscard]] virtual RootNode* get_root_node_optional() const override;

    const std::vector<std::unique_ptr<SpatialNode>>& get_children() const;

    virtual void set_translation(const glm::vec3& new_trans) = 0;
    virtual void translate(const glm::vec3& translation) = 0;
    virtual void set_scale(const glm::vec3& new_scale) = 0;
    virtual void set_rotation(const glm::quat& new_rotation) = 0;
    virtual void set_transform(const Transform& new_transform) = 0;

    void set_translation_property(const NodeProperty& property);
    void set_scale_property(const NodeProperty& property);
    void set_rotation_property(const NodeProperty& property);

    [[nodiscard]] virtual glm::vec3 get_translation() const noexcept = 0;
    [[nodiscard]] virtual glm::vec3 get_global_position() const noexcept = 0;
    [[nodiscard]] virtual glm::vec3 get_scale() const noexcept = 0;
    [[nodiscard]] virtual glm::vec3 get_global_scale() const noexcept = 0;
    [[nodiscard]] virtual glm::quat get_rotation() const noexcept = 0;
    [[nodiscard]] virtual glm::quat get_global_rotation() const noexcept = 0;
    [[nodiscard]] virtual Transform get_transform() const noexcept = 0;
    [[nodiscard]] virtual Transform get_global_transform() const noexcept = 0;
    [[nodiscard]] virtual glm::mat4 get_local_matrix() const noexcept = 0;
    [[nodiscard]] virtual glm::mat4 get_global_matrix() const noexcept = 0;

    virtual void copy_to(Node& node) const override;

    virtual void internal_update() override;

    virtual void on_attachment_to_tree_without_start() override;

    static void register_properties();

protected:
    virtual void internal_on_enable() override;
    virtual void internal_on_disable() override;

private:
    SpatialNode* parent {nullptr};
    /**
     * This vector MUST NOT be changed outside the
     * add_child and remove_child functions.
    */
    std::vector<std::unique_ptr<SpatialNode>> children;
    std::vector<std::unique_ptr<SpatialNode>> children_queued_to_add;
    std::vector<SpatialNode*> children_queued_to_remove;

    void add_children_from_queue();
    void remove_children_from_queue();
};
}