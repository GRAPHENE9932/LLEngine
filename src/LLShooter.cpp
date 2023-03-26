#include "LLShooter.hpp"
#include "common/core/KTXTexture.hpp"
#include "common/core/RGBETexture.hpp"
#include "common/core/logger.hpp"
#include "nodes/core/rendering/PBRDrawableNode.hpp"
#include "utils/primitive_meshes.hpp"
#include "utils/texture_tools.hpp"

#include <GLFW/glfw3.h>

#include <memory>
#include <iostream>

const int WINDOW_WIDTH = 1500, WINDOW_HEIGHT = 800;

LLShooter::~LLShooter() {
    
}

void LLShooter::start() {
    init();
    rendering_server.get()->main_loop();
}

void LLShooter::init() {
    logger::enable_console_logging();

    rendering_server = std::make_unique<RenderingServer>(glm::ivec2(WINDOW_WIDTH, WINDOW_HEIGHT));
    bullet_physics_server = std::make_unique<BulletPhysicsServer>();

    Map map("res/maps/map_close.json");
    EngineServers engine_servers {*rendering_server, *bullet_physics_server};
    root_node = map.to_node(engine_servers);

    logger::info("Starting RGBE loading.");
    auto sky_panorama = RGBETexture("res/textures/sky.hdr");
    logger::info("Finished RGBE loading.");
    auto sky_cubemap = std::shared_ptr(panorama_to_cubemap(
        sky_panorama,
        rendering_server->get_shader_holder().get_equirectangular_mapper_shader()
    ));
    rendering_server->set_cubemap(
        sky_cubemap
    );

    rendering_server->set_update_callback([&](float delta_time){
        bullet_physics_server->do_step(delta_time);
        root_node->update();
    });
}
