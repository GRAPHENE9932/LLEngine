#include "SceneFile.hpp"
#include "logger.hpp"
#include "node_cast.hpp"
#include "node_registration.hpp"
#include "nodes/physics/BulletRigidBodyNode.hpp"
#include "nodes/rendering/PBRDrawableNode.hpp"
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

    std::unique_ptr<llengine::PBRDrawableNode> glowing_cube = std::make_unique<llengine::PBRDrawableNode>();
    glowing_cube->set_mesh(llengine::Mesh::get_cube());
    llengine::Material glowing_cube_material;
    glowing_cube_material.base_color_factor = {1.0f, 1.0f, 1.0f, 1.0f};
    glowing_cube_material.emissive_factor = {5.0f, 5.0f, 5.0f};
    glowing_cube->set_material(std::make_shared<llengine::Material>(std::move(glowing_cube_material)));
    glowing_cube->set_transform(llengine::Transform());
    glowing_cube->set_name("Glowing cube");
    get_parent()->queue_add_child(std::move(llengine::node_cast<llengine::SpatialNode>(std::move(glowing_cube))));

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