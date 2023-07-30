#include "nodes/gui/GUICanvas.hpp"
#include "node_registration.hpp"
#include "nodes/SpatialNode.hpp"
#include "rendering/RenderingServer.hpp"
#include "node_cast.hpp"

#include <algorithm>

using namespace llengine;

GUICanvas::~GUICanvas() {
    if (auto rendering_server = get_rendering_server_optional()) {
        rendering_server->unregister_gui_canvas(this);
    }
}

void GUICanvas::draw() {
    if (!is_enabled()) {
        return;
    }

    for (const auto& cur_gui_node : all_sorted_gui_nodes) {
        cur_gui_node->draw();
    }
}

void GUICanvas::on_attachment_to_tree_without_start() {
    SpatialNode::on_attachment_to_tree_without_start();

    get_rendering_server().register_gui_canvas(this);

    std::for_each(
        gui_nodes.begin(), gui_nodes.end(),
        [] (const auto& child) {
            child->on_attachment_to_tree();
        }
    );
}

void GUICanvas::queue_add_child(std::unique_ptr<Node>&& child) {
    if (auto gui_child = node_cast<GUINode>(std::move(child))) {
        queue_add_gui_node(std::move(gui_child));
    }
    else {
        SpatialNode::queue_add_child(throwing_node_cast<SpatialNode>(std::move(child)));
    }
}

void GUICanvas::queue_add_gui_node(std::unique_ptr<GUINode>&& gui_node) {
    gui_nodes_queued_to_add.emplace_back(std::move(gui_node));
}

void GUICanvas::queue_remove_gui_node(std::size_t index) {
    if (index > gui_nodes.size()) {
        throw std::out_of_range("Can not remove child: invalid child index specified.");
    }

    gui_nodes_queued_to_remove.push_back(gui_nodes[index].get());
}

void GUICanvas::queue_remove_gui_node(GUINode* gui_node) {
    const auto iter = std::find_if(
        gui_nodes.begin(), gui_nodes.end(),
        [&gui_node](const auto& cur_unique) {
            return cur_unique.get() == gui_node;
        }
    );
    if (iter == gui_nodes.end()) {
        throw std::invalid_argument("Can't remove the non-existent child.");
    }

    gui_nodes_queued_to_remove.push_back(gui_node);
}

void GUICanvas::register_gui_node(GUINode* gui_node) {
    if (!gui_node) {
        return;
    }

    // We need all the gui nodes to be sorted by Z coordinate descending.
    float new_gui_node_z = gui_node->get_transform().z_coordinate;
    auto iter = std::find_if(all_sorted_gui_nodes.begin(), all_sorted_gui_nodes.end(), [&new_gui_node_z] (GUINode* current) {
        return new_gui_node_z >= current->get_transform().z_coordinate;
    });
    
    all_sorted_gui_nodes.insert(iter, gui_node);
}

void GUICanvas::unregister_gui_node(GUINode* gui_node) {
    const auto iter {
        std::find(all_sorted_gui_nodes.begin(), all_sorted_gui_nodes.end(), gui_node)
    };
    if (iter != all_sorted_gui_nodes.end()) {
        all_sorted_gui_nodes.erase(iter);
    }
}

[[nodiscard]] glm::vec2 GUICanvas::get_size() const {
    if (canvas_size.has_value()) {
        return *canvas_size;
    }
    else {
        return get_rendering_server().get_window().get_window_size();
    }
}

[[nodiscard]] glm::mat4 GUICanvas::get_mvp_matrix() const {
    if (is_screen_overlayed()) {
        return glm::mat4(1.0f);
    }
    else {
        return get_rendering_server().get_view_proj_matrix() * get_global_matrix();
    }
}

void GUICanvas::set_screen_overlayed_property(const NodeProperty& property) {
    set_screen_overlayed(property.get<bool>());
}

void GUICanvas::internal_update() {
    if (!is_enabled()) {
        return;
    }

    add_gui_nodes_from_queue();
    remove_gui_nodes_from_queue();
    update_children();
    for (auto& gui_node : gui_nodes) {
        gui_node->internal_update();
    }

    update();
}

void GUICanvas::register_properties() {
    register_custom_property<GUICanvas>("gui_canvas", "screen_overlayed", &GUICanvas::set_screen_overlayed_property);
}

void GUICanvas::internal_on_enable() {
    SpatialNode::internal_on_enable();

    for (const auto& gui_node : gui_nodes) {
        gui_node->on_parent_enable_disable(true);
    }
}

void GUICanvas::internal_on_disable() {
    SpatialNode::internal_on_disable();

    for (const auto& gui_node : gui_nodes) {
        gui_node->on_parent_enable_disable(true);
    }
}

void GUICanvas::add_gui_nodes_from_queue() {
    if (gui_nodes_queued_to_add.empty()) {
        return;
    }

    const bool attached_to_tree = is_attached_to_tree();

    for (auto& gui_node_in_queue : gui_nodes_queued_to_add) {
        gui_node_in_queue->parent = this;
        gui_nodes.emplace_back(std::move(gui_node_in_queue));
        if (attached_to_tree) {
            gui_nodes.back()->on_attachment_to_tree();
        }
    }

    gui_nodes_queued_to_add.clear();
}

void GUICanvas::remove_gui_nodes_from_queue() {
    if (gui_nodes_queued_to_remove.empty()) {
        return;
    }

    std::size_t handled_children = 0;
    while (handled_children < gui_nodes_queued_to_remove.size()) {
        std::size_t index_in_queue = 0;
        for (std::size_t index_in_children = 0; index_in_children < gui_nodes.size();) {
            if (gui_nodes[index_in_children].get() == gui_nodes_queued_to_remove[index_in_queue]) {
                gui_nodes.erase(gui_nodes.begin() + index_in_children);
                index_in_queue++;
                handled_children++;
            }
            else {
                index_in_children++;
            }
        }
    }

    gui_nodes_queued_to_remove.clear();
}