#pragma once

#include "NodeProperty.hpp"

#include <string_view>
#include <map>

namespace llengine {
class RootNode;
class RenderingServer;
class BulletPhysicsServer;

class Node {
public:
    Node() = default;
    virtual ~Node() = default;
    Node(const Node& other) = delete;
    Node(Node&& other) = delete;
    Node& operator=(const Node& other) = delete;
    Node& operator=(Node&& other) = delete;

    virtual void queue_add_child(std::unique_ptr<Node>&& child) = 0;
    [[nodiscard]] bool is_attached_to_tree() const {
        return get_root_node_optional();
    };

    [[nodiscard]] inline const std::string& get_name() const {
        return name;
    }
    inline void set_name(std::string_view new_name) {
        name = new_name;
    }
    inline void set_name_property(const NodeProperty& property) {
        name = property.get<std::string>();
    }

    void enable();
    void disable();
    void set_enabled_property(const NodeProperty& property);
    [[nodiscard]] bool is_enabled() const;

    virtual void copy_to(Node& node) const;
    virtual std::unique_ptr<Node> copy() const { return nullptr; }

    static void register_properties();

    [[nodiscard]] RenderingServer& get_rendering_server() const;
    [[nodiscard]] RenderingServer* get_rendering_server_optional() const;
    [[nodiscard]] BulletPhysicsServer& get_bullet_physics_server() const;
    [[nodiscard]] BulletPhysicsServer* get_bullet_physics_server_optional() const;
    [[nodiscard]] RootNode& get_root_node() const;
    [[nodiscard]] virtual RootNode* get_root_node_optional() const = 0;

protected:
    virtual void on_attachment_to_tree();
    virtual void update() {};
    virtual void internal_update() {};
    void on_parent_enable_disable(bool enabled);
    virtual void internal_on_enable() {};
    virtual void internal_on_disable() {};

private:
    std::string name;
    bool enabled = true;
    bool parent_enabled = true;
    bool was_enabled_before = true;

    mutable RenderingServer* cached_rendering_server = nullptr;
    mutable BulletPhysicsServer* cached_bullet_physics_server = nullptr;

    friend class GameInstance;
};
}