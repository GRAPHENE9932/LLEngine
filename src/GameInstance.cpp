#include "GameInstance.hpp"
#include "SceneJSON.hpp"
#include "logger.hpp"
#include "nodes/RootNode.hpp"
#include "rendering/RenderingServer.hpp"
#include "physics/BulletPhysicsServer.hpp"
#include "utils/texture_utils.hpp"

#include <memory>

using namespace llengine;

GameInstance::GameInstance(const GameSettings& settings) {
    logger::enable_console_logging();

    rendering_server = std::make_unique<RenderingServer>(settings.window_resolution, settings.window_title);
    bullet_physics_server = std::make_unique<BulletPhysicsServer>();

    root_node = std::make_unique<RootNode>(*bullet_physics_server);

    if (!settings.json_scene_path.empty()) {
        SceneJSON scene(settings.json_scene_path);
        root_node->queue_add_child(std::move(scene.to_node()));
    }

    if (!settings.skybox_path.empty()) {
        auto sky_panorama = Texture::from_file(settings.skybox_path);
        auto sky_cubemap = tex_utils::panorama_to_cubemap(sky_panorama);
        rendering_server->set_cubemap(std::make_shared<Texture>(std::move(sky_cubemap)));
    }

    rendering_server->apply_quality_settings(settings.quality_settings);

    rendering_server->set_update_callback([&] (float delta) {
        if (bullet_physics_server) {
            bullet_physics_server->do_step(delta);
        }
        root_node->_update();
    });
}

GameInstance::~GameInstance() {

};

void GameInstance::start() {
    rendering_server->main_loop();
}