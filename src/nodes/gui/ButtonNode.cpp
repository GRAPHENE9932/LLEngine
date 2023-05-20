#include "nodes/gui/ButtonNode.hpp"
#include "rendering/RenderingServer.hpp"
#include "logger.hpp"

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
    const auto& cursor_pos = rs.get_window().get_cursor_position();
    return rs.get_window().is_mouse_button_pressed(0) && contains_point(cursor_pos);
}