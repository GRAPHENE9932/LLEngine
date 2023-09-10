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
    bullet = llengine::node_cast<llengine::SpatialNode>(
        llengine::SceneFile::load_from_file("res/meshes/bullet.glb")->to_node({}, &llengine::find_custom_node_type<BulletNode>())
    );
}

void MainCamera::update() {
    if (get_rendering_server().is_mouse_button_pressed(0) && !clicked_before) {
        spawn_barrel();
        clicked_before = true;
    }

    if (!get_rendering_server().is_mouse_button_pressed(0)) {
        clicked_before = false;
    }

    if (get_rendering_server().get_window().is_key_pressed(llengine::Key::ESCAPE)) {
        get_rendering_server().stop();
    }
}

void MainCamera::spawn_barrel() {
    auto barrel_copy = llengine::throwing_node_cast<llengine::BulletRigidBodyNode>(bullet->copy());
    barrel_copy->set_translation(get_translation());
    barrel_copy->set_rotation(get_rotation() * glm::quat({glm::radians(90.0f), 0.0f, 0.0f}));
    barrel_copy->queue_apply_impulse(get_direction() * 50.0f);
    get_parent()->queue_add_child(llengine::throwing_node_cast<llengine::Node>(std::move(barrel_copy)));
}