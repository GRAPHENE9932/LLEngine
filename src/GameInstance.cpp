#include "GameInstance.hpp"
#include "SceneJSON.hpp"
#include "nodes/SpatialNode.hpp"
#include "rendering/RenderingServer.hpp"
#include "physics/BulletPhysicsServer.hpp"

#include <memory>

using namespace llengine;

GameInstance::GameInstance(const GameSettings& settings) {
    rendering_server = std::make_unique<RenderingServer>(settings.window_resolution);
    bullet_physics_server = std::make_unique<BulletPhysicsServer>();

    SceneJSON scene(settings.json_scene_path);
    EngineServers servers {*rendering_server, *bullet_physics_server};
    root_node = scene.to_node(servers);

    auto sky_panorama = Texture::from_file(settings.skybox_path);
    auto sky_cubemap = sky_panorama.panorama_to_cubemap(rendering_server->get_shader_holder().get_equirectangular_mapper_shader());
    rendering_server->set_cubemap(std::make_shared<Texture>(std::move(sky_cubemap)));

    rendering_server->set_update_callback([&] (float delta) {
        if (bullet_physics_server) {
            bullet_physics_server->do_step(delta);
        }
        root_node->update();
    });
}

GameInstance::~GameInstance() {    

};

void GameInstance::start() {
    rendering_server->main_loop();
}