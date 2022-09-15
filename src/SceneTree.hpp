#pragma once

#include <vector> // std::vector
#include <memory> // std::unique_ptr

#include "Context.hpp" // Context
#include "PhysicsServer.hpp" // PhysicsServer
#include "RenderingServer.hpp" // RenderingServer
#include "nodes/core/SpatialNode.hpp" // SpatialNode
#include "nodes/core/rendering/CameraNode.hpp" // CameraNode
#include "nodes/core/rendering/DrawableNode.hpp" // DrawableNode
#include "nodes/core/rendering/PointLightNode.hpp" // PointLightNode

class SceneTree {
public:
    void set_root(SpatialNode* node);
    void invoke_update(RenderingServer& rendering_server);
    void inform_about_new_window_extents(glm::ivec2 window_extents);

    void register_node(SpatialNode* node_ptr, SpatialNode* parent_ptr);
    void register_node(DrawableNode* node_ptr, SpatialNode* parent_ptr);
    void register_node(PointLightNode* node_ptr, SpatialNode* parent_ptr);
    void register_node(CameraNode* node_ptr, SpatialNode* parent_ptr);

    void handle_node_removal(SpatialNode* node_ptr);

    const Context& get_context();
    ShaderManager& get_shader_manager();
    void report_about_drawn_triangles(const unsigned long amount);

    const std::vector<DrawableNode*>& get_drawables();

private:
    Context context;
    std::unique_ptr<SpatialNode> root_node = nullptr;
    CameraNode* camera = nullptr;

    std::vector<DrawableNode*> drawables;
    std::vector<PointLightNode*> point_lights;

    friend class RenderingServer;
};
