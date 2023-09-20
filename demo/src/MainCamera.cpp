#include "SceneFile.hpp"
#include "logger.hpp"
#include "node_cast.hpp"
#include "node_registration.hpp"
#include "nodes/physics/BulletRigidBodyNode.hpp"
#include "rendering/RenderingServer.hpp"
#include "physics/BulletPhysicsServer.hpp"
#include "BulletNode.hpp"
#include "MainCamera.hpp"

#include <fmt/format.h>

void MainCamera::start() {
    enable_on_keyboard_key(true);
    bullet = llengine::node_cast<llengine::SpatialNode>(
        llengine::SceneFile::load_from_file("res/meshes/bullet.glb")->to_node({}, &llengine::find_custom_node_type<BulletNode>())
    );
    get_rendering_server().get_window().disable_cursor();
}

void MainCamera::update() {
    auto& rs = get_rendering_server();

    if (rs.is_mouse_button_pressed(0) && !clicked_before) {
        spawn_barrel();
        clicked_before = true;
    }

    if (!rs.is_mouse_button_pressed(0)) {
        clicked_before = false;
    }
}

void MainCamera::on_keyboard_key_press(llengine::Key key) {
    if (key == llengine::Key::Q) {
        get_rendering_server().stop();
    }
    else if (key == llengine::Key::ESCAPE) {
        if (is_mouse_locked()) {
            unlock_mouse();
        }
        else {
            lock_mouse();
        }
    }
}

void MainCamera::spawn_barrel() {
    auto barrel_copy = llengine::throwing_node_cast<llengine::BulletRigidBodyNode>(bullet->copy());
    barrel_copy->set_translation(get_translation());
    barrel_copy->set_rotation(get_rotation() * glm::quat({glm::radians(90.0f), 0.0f, 0.0f}));
    barrel_copy->queue_apply_impulse(get_direction() * 50.0f);
    get_parent()->queue_add_child(llengine::throwing_node_cast<llengine::Node>(std::move(barrel_copy)));
}