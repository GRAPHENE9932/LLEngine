#include "nodes/gui/GUINode.hpp"
#include "rendering/RenderingServer.hpp"
#include "rendering/Mesh.hpp"
#include "gui/GUITexture.hpp"
#include "utils/math.hpp"
#include "nodes/gui/GUICanvas.hpp"
#include "node_registration.hpp"
#include "node_cast.hpp"
#include "logger.hpp"

#include <fmt/format.h>
#include <GL/glew.h>

#include <utility>

using namespace llengine;

GUINode::~GUINode() {
    if (GUICanvas* canvas = get_canvas_optional()) {
        canvas->unregister_gui_node(this);
    }
}

[[nodiscard]] glm::vec3 GUINode::get_screen_space_position() const {
    GUINode* parent = get_parent();
    const auto local_transform = get_transform();
    glm::vec3 local_position {
        local_transform.get_screen_space_offset(get_parent_size())
    };
    if (parent) {
        return local_position + parent->get_screen_space_position();
    }
    else {
        return local_position;
    }
}

void GUINode::set_transform_property(const NodeProperty& property) {
    set_transform(GUITransform::from_property(property));
}

[[nodiscard]] bool GUINode::contains_point(glm::vec2 point) const {
    glm::vec2 top_left = glm::vec2(get_screen_space_position());

    glm::vec2 bottom_right = top_left + get_absolute_size();

    return point.x >= top_left.x && point.x <= bottom_right.x &&
        point.y >= top_left.y && point.y <= bottom_right.y;
}

void GUINode::draw_children() {
    for (const auto& child_ptr : children) {
        child_ptr->draw();
    }
}

void GUINode::update_children() {
    for (const auto& child_ptr : children) {
        child_ptr->update();
    }
}

void GUINode::queue_add_child(std::unique_ptr<Node>&& child) {
    queue_add_child(throwing_node_cast<GUINode>(std::move(child)));
}

void GUINode::queue_add_child(std::unique_ptr<GUINode>&& child) {
    children_queued_to_add.push_back(std::move(child));
}

void GUINode::internal_update() {
    if (is_enabled()) {
        add_children_from_queue();
        remove_children_from_queue();

        try {
            update_children();
            update();
        }
        catch (const std::exception& e) {
            logger::error(fmt::format("update: {}", e.what()));
        }
        catch (...) {
            logger::error("Unknown error in update.");
        }
    }
}

[[nodiscard]] RootNode* GUINode::get_root_node_optional() const {
    if (std::holds_alternative<GUINode*>(parent)) {
        return std::get<GUINode*>(parent)->get_root_node_optional();
    }
    else if (std::holds_alternative<GUICanvas*>(parent)) {
        return std::get<GUICanvas*>(parent)->get_root_node_optional();
    }
    else {
        return nullptr;
    }
}

void GUINode::draw_texture_part(
    const Texture& texture, glm::vec2 pos_offset_in_px, glm::vec2 tex_offset_in_px,
    glm::vec2 quad_size_in_px, glm::vec2 tex_part_size_in_px
) {
    GUICanvas& canvas {get_canvas()};
    const glm::vec2 viewport_size = canvas.get_size();

    glm::vec2 position_in_px = get_screen_space_position();
    // The quad mesh is in range from -1.0 to 1.0, but
    // not from 0.0 to 1.0. So, to pivot it to the corner, add half of the size.
    position_in_px += pos_offset_in_px + quad_size_in_px / 2.0f;

    const glm::mat4 model_matrix = glm::translate(
        math_utils::scr_space_pos_to_gl_space(glm::vec3(position_in_px, get_transform().z_coordinate), viewport_size)
    ) * glm::scale(glm::vec3(quad_size_in_px / viewport_size, 1.0f));
    const glm::mat4 mvp = model_matrix * canvas.get_mvp_matrix();

    glm::vec2 uv_scale = tex_part_size_in_px / static_cast<glm::vec2>(texture.get_size());
    glm::vec2 uv_offset = tex_offset_in_px / static_cast<glm::vec2>(texture.get_size());
    uv_offset.y += uv_scale.y; // Invert UV's y. If (scale * y) goes from 0 to scale when y goes from 0 to 1,
    uv_scale.y = -uv_scale.y; // then (scale - scale * y) goes from scale to 0.

    ensure_shader_is_initialized();
    shader->use_shader();
    shader->set_mat4<"mvp">(mvp);
    shader->set_vec2<"uv_scale">(uv_scale);
    shader->set_vec2<"uv_offset">(uv_offset);
    shader->set_vec4<"color_factor">({1.0f, 1.0f, 1.0f, 1.0f});
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.get_id());

    auto mesh = Mesh::get_quad();
    mesh->bind_vao(true, false, false);

    if (mesh->is_indexed()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->get_indices_id());
        glDrawElements(GL_TRIANGLES, mesh->get_amount_of_vertices(), mesh->get_indices_type(), 0);
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->get_vertices_id());
        glDrawArrays(GL_TRIANGLES, 0, mesh->get_amount_of_vertices());
    }
    get_rendering_server().report_about_drawn_triangles(mesh->get_amount_of_vertices() / 3);

    mesh->unbind_vao();
}

void GUINode::draw_rectangle(const GUITexture& texture) {
    const std::array<glm::vec2, 3> tex_part_sizes {
        glm::vec2(texture.left_border, texture.top_border),
        glm::vec2(texture->get_size().x - texture.left_border - texture.right_border, texture->get_size().y - texture.top_border - texture.bottom_border),
        glm::vec2(texture.right_border, texture.bottom_border)
    };
    const std::array<glm::vec2, 3> quad_sizes {
        glm::vec2(texture.left_border, texture.top_border),
        glm::vec2(get_absolute_size() - texture.left_border - texture.right_border),
        glm::vec2(texture.right_border, texture.bottom_border)
    };
    glm::vec2 offset_in_tex {0.0f, 0.0f};
    glm::vec2 offset_of_quad {0.0f, 0.0f};
    for (std::size_t x = 0; x < 3; x++) {
        for (std::size_t y = 0; y < 3; y++) {
            draw_texture_part(
                *texture,
                offset_of_quad,
                offset_in_tex,
                {quad_sizes[x].x, quad_sizes[y].y},
                {tex_part_sizes[x].x, tex_part_sizes[y].y}
            );
            offset_in_tex.y += tex_part_sizes[y].y;
            offset_of_quad.y += quad_sizes[y].y;
        }
        offset_in_tex.x += tex_part_sizes[x].x;
        offset_in_tex.y = 0.0f;
        offset_of_quad.x += quad_sizes[x].x;
        offset_of_quad.y = 0.0f;
    }
}

void GUINode::on_attachment_to_tree_without_start() {
    Node::on_attachment_to_tree_without_start();

    get_canvas().register_gui_node(this);

    std::for_each(
        children.begin(), children.end(),
        [] (const auto& child) {
            child->on_attachment_to_tree();
        }
    );
}

void GUINode::on_attachment_to_canvas() {
    get_canvas().register_gui_node(this);
}

void GUINode::assign_canvas_parent(GUICanvas& canvas) {
    parent = &canvas;
}

[[nodiscard]] GUINode* GUINode::get_parent() const {
    if (std::holds_alternative<GUINode*>(parent)) {
        return std::get<GUINode*>(parent);
    }
    else {
        return nullptr;
    }
}

[[nodiscard]] GUICanvas& GUINode::get_canvas() const {
    GUICanvas* result = get_canvas_optional();
    if (!result) {
        throw std::runtime_error(fmt::format(
            "Can't get canvas for GUI node \"{}\".", get_name()
        ));
    }
    return *result;
}

[[nodiscard]] GUICanvas* GUINode::get_canvas_optional() const {
    if (std::holds_alternative<GUICanvas*>(parent)) {
        return std::get<GUICanvas*>(parent);
    }
    else if (std::holds_alternative<GUINode*>(parent)) {
        return std::get<GUINode*>(parent)->get_canvas_optional();
    }
    else {
        return nullptr;
    }
}

[[nodiscard]] glm::vec2 GUINode::get_parent_size() const {
    if (std::holds_alternative<GUINode*>(parent)) {
        return std::get<GUINode*>(parent)->get_absolute_size();
    }
    else if (std::holds_alternative<GUICanvas*>(parent)) {
        return std::get<GUICanvas*>(parent)->get_size();
    }
    else {
        throw std::runtime_error(
            "Can not get GUI node parent size because the node "
            "doesn't have a parent node or assigned canvas."
        );
    }
}

void GUINode::register_properties() {
    register_custom_property<GUINode>("gui_node", "transform", &GUINode::set_transform_property);
}

void GUINode::internal_on_enable() {
    for (const auto& child : children) {
        child->on_parent_enable_disable(true);
    }
}

void GUINode::internal_on_disable() {
    for (const auto& child : children) {
        child->on_parent_enable_disable(false);
    }
}

void GUINode::add_children_from_queue() {
    if (children_queued_to_add.empty()) {
        return;
    }

    const bool attached_to_tree = is_attached_to_tree();

    for (auto& child_in_queue : children_queued_to_add) {
        child_in_queue->parent = this;
        children.emplace_back(std::move(child_in_queue));
        if (attached_to_tree) {
            children.back()->on_attachment_to_tree();
        }
    }

    children_queued_to_add.clear();
}

void GUINode::remove_children_from_queue() {
    if (children_queued_to_remove.empty()) {
        return;
    }

    std::size_t handled_children = 0;
    while (handled_children < children_queued_to_remove.size()) {
        std::size_t index_in_queue = 0;
        for (std::size_t index_in_children = 0; index_in_children < children.size();) {
            if (children[index_in_children].get() == children_queued_to_remove[index_in_queue]) {
                children.erase(children.begin() + index_in_children);
                index_in_queue++;
                handled_children++;
            }
            else {
                index_in_children++;
            }
        }
    }

    children_queued_to_remove.clear();
}

constexpr std::string_view VERTEX_SHADER_TEXT =
    #include "shaders/gui_rectangle.vert"
;
constexpr std::string_view FRAGMENT_SHADER_TEXT =
    #include "shaders/gui_rectangle.frag"
;

void GUINode::ensure_shader_is_initialized() {
    if (shader) {
        return;
    }

    shader = std::make_unique<ShaderType>(VERTEX_SHADER_TEXT, FRAGMENT_SHADER_TEXT);
}