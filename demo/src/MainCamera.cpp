#include "SceneFile.hpp"
#include "logger.hpp"
#include "node_cast.hpp"
#include "rendering/RenderingServer.hpp"
#include "physics/BulletPhysicsServer.hpp"
#include "MainCamera.hpp"

void MainCamera::start() {
    barrel = llengine::node_cast<llengine::SpatialNode>(llengine::SceneFile::load_from_file("res/meshes/barrel.glb")->to_node());
    barrel->set_translation({0.0f, 15.0f, 0.0f});
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

void MainCamera::spawn_barrel() const {
    glm::vec3 direction { get_global_rotation() * glm::vec3(0.0f, 0.0f, 1.0f) };
    std::optional<glm::vec3> raycast_result = get_bullet_physics_server().raycast_closest(get_global_position(), get_global_position() + direction * 100.0f);

    if (!raycast_result.has_value()) {
        llengine::logger::info("Raycast failed");
    }
    else {
        auto barrel_copy = llengine::throwing_node_cast<llengine::SpatialNode>(barrel->copy());
        barrel_copy->set_translation(*raycast_result);
        get_parent()->queue_add_child(llengine::throwing_node_cast<llengine::Node>(std::move(barrel_copy)));
    }
}