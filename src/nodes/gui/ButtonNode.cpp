#include "nodes/gui/ButtonNode.hpp"
#include "node_registration.hpp"
#include "rendering/RenderingServer.hpp"

using namespace llengine;

void ButtonNode::draw() {
    if (is_pressed()) {
        if (!pressed_texture.empty()) {
            draw_rectangle(pressed_texture);
        }
    }
    else {
        if (!idle_texture.empty()) {
            draw_rectangle(idle_texture);
        }
    }
}

void ButtonNode::update() {
    update_children();
}

[[nodiscard]] bool ButtonNode::is_pressed() const {
    const auto& cursor_pos = get_rendering_server().get_window().get_cursor_position();
    return get_rendering_server().get_window().is_mouse_button_pressed(0) && contains_point(cursor_pos);
}

void ButtonNode::register_properties() {
    register_custom_property<ButtonNode>("button_node", "idle_texture", &ButtonNode::set_idle_texture_property);
    register_custom_property<ButtonNode>("button_node", "pressed_texture", &ButtonNode::set_pressed_texture_property);
}

void ButtonNode::set_idle_texture_property(const NodeProperty& property) {
    idle_texture = GUITexture::from_property(property);
}

void ButtonNode::set_pressed_texture_property(const NodeProperty& property) {
    pressed_texture = GUITexture::from_property(property);
}