#include "RenderingServer.hpp"
#include <algorithm> // std::lower_bound

#include <SceneTree.hpp>

void SceneTree::set_root(SpatialNode* node) {
    drawables.clear();
    root_node = std::unique_ptr<SpatialNode>(node);
    root_node->register_myself(nullptr);
}

void SceneTree::invoke_update(RenderingServer& rendering_server) {
    // Prepare context.
    if (camera) {
        context.view_matrix = camera->compute_view_matrix();
        context.proj_matrix = camera->get_proj_matrix();
        context.view_proj_matrix = context.proj_matrix * context.view_matrix;
    }
    else {
        throw std::runtime_error("Camera doesn't exist.");
    }
    context.window_extents = rendering_server.get_window_extents();

    if (root_node)
        root_node->update();
}

void SceneTree::inform_about_new_window_extents(glm::ivec2 window_extents) {
    context.window_extents = window_extents;
}

void SceneTree::register_node(SpatialNode* node_ptr, SpatialNode* parent_ptr) {
    if (parent_ptr)
        node_ptr->parent = parent_ptr;

    // Handle all the children.
    for (const auto& child : node_ptr->get_children())
        child->register_myself(node_ptr);
}

void SceneTree::register_node(DrawableNode* node_ptr, SpatialNode* parent_ptr) {
    register_node(static_cast<SpatialNode*>(node_ptr), parent_ptr);

    const auto iter = std::lower_bound(
        drawables.begin(), drawables.end(), node_ptr,
        [](const DrawableNode* const node_1, const DrawableNode* const node_2) {
            return node_1->get_program_id() < node_2->get_program_id();
        }
    );

    drawables.insert(iter, node_ptr);
}

void SceneTree::register_node(PointLightNode* node_ptr, SpatialNode* parent_ptr) {
    register_node(static_cast<SpatialNode*>(node_ptr), parent_ptr);

    context.point_lights.push_back(node_ptr);
}

void SceneTree::register_node(CameraNode* node_ptr, SpatialNode* parent_ptr) {
    register_node(static_cast<SpatialNode*>(node_ptr), parent_ptr);

    this->camera = node_ptr;
}

void SceneTree::handle_node_removal(SpatialNode* node) {
    const auto drawables_search_result = std::find(drawables.begin(), drawables.end(), node);
    if (drawables_search_result != drawables.end())
        drawables.erase(drawables_search_result);
    
    const auto p_lights_search_result = std::find(point_lights.begin(), point_lights.end(), std::ref(node));
    if (p_lights_search_result != point_lights.end())
        point_lights.erase(p_lights_search_result);
}

const Context& SceneTree::get_context() {
    return context;
}

ShaderManager& SceneTree::get_shader_manager() {
    return context.sh_mgr;
}

void SceneTree::report_about_drawn_triangles(const unsigned long amount) {
    context.triangles_drawn += amount;
}

const std::vector<DrawableNode*>& SceneTree::get_drawables() {
    return drawables;
}
