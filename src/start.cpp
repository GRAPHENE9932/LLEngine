#include "start.hpp"
#include "SceneJSON.hpp"
#include "nodes/SpatialNode.hpp"
#include "rendering/RenderingServer.hpp"
#include "physics/BulletPhysicsServer.hpp"

#include <memory>

using namespace llengine;

std::unique_ptr<RenderingServer> rendering_server = nullptr;
std::unique_ptr<BulletPhysicsServer> bullet_physics_server = nullptr;

std::unique_ptr<SpatialNode> root_node = nullptr;

void update_callback(float delta) {
    if (bullet_physics_server) {
        bullet_physics_server->do_step(delta);
        root_node->update();
    }
}

void llengine::start(std::string_view first_scene_path) {
    rendering_server = std::make_unique<RenderingServer>(glm::ivec2(1920, 1080));
    bullet_physics_server = std::make_unique<BulletPhysicsServer>();

    SceneJSON scene((std::string(first_scene_path)));
    EngineServers servers {*rendering_server, *bullet_physics_server};
    root_node = scene.to_node(servers);

    auto sky_panorama = Texture::from_rgbe("res/textures/sky.hdr");
    auto sky_cubemap = sky_panorama.panorama_to_cubemap(rendering_server->get_shader_holder().get_equirectangular_mapper_shader());
    rendering_server->set_cubemap(std::make_shared<Texture>(std::move(sky_cubemap)));

    rendering_server->set_update_callback(update_callback);

    rendering_server->main_loop();
}