#pragma once

#include "gui/GUITexture.hpp"
#include "CompleteGUINode.hpp"

namespace llengine {
class ButtonNode : public CompleteGUINode {
public:
    void draw() override;
    void update() override;

    [[nodiscard]] bool is_pressed() const;

    void set_idle_texture(const GUITexture& new_texture) {
        idle_texture = new_texture;
    }
    void set_pressed_texture(const GUITexture& new_texture) {
        pressed_texture = new_texture;
    }

    static void register_properties();
    void set_idle_texture_property(const NodeProperty& property);
    void set_pressed_texture_property(const NodeProperty& property);

private:
    GUITexture idle_texture;
    GUITexture pressed_texture;
    bool was_pressed_in_last_update = false;
};
}