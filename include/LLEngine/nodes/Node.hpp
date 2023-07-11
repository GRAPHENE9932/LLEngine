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

    /**
     * @brief Sets the node property. Either basic or custom.
     */
    void set_property(const NodeProperty& property);

    virtual void add_child(std::unique_ptr<Node>&& child) = 0;
    [[nodiscard]] virtual bool is_attached_to_tree() const = 0;

    [[nodiscard]] inline const std::string& get_name() const {
        return name;
    }
    inline void set_name(std::string_view new_name) {
        name = new_name;
    }
    inline void set_name_property(const NodeProperty& property) {
        name = property.get<std::string>();
    }

    static void register_properties();

    [[nodiscard]] RenderingServer& get_rendering_server() const;
    [[nodiscard]] BulletPhysicsServer& get_bullet_physics_server() const;
    [[nodiscard]] virtual const RootNode& get_root_node() const = 0;
    [[nodiscard]] virtual RootNode& get_root_node() = 0;

protected:
    virtual void on_attachment_to_tree();
    virtual void update() {};
    virtual void internal_update() {};

private:
    std::string name;

    mutable RenderingServer* cached_rendering_server = nullptr;
    mutable BulletPhysicsServer* cached_bullet_physics_server = nullptr;

    friend class GameInstance;
};
}