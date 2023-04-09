#include "LLShooter.hpp"
#include "common/core/logger.hpp"
#include "nodes/core/gui/GUINode.hpp"
#include "nodes/core/gui/TextNode.hpp"
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

    root_gui_node = std::make_unique<GUINode>(*rendering_server);
    auto text_node = std::make_unique<TextNode>(*rendering_server, std::make_shared<FreeTypeFont>("res/fonts/Ubuntu-Regular.ttf", 32));
    text_node->set_text("LLShooter");
    text_node->set_position_offset({200.0f, 200.0f});
    root_gui_node->add_child(std::move(text_node));

    logger::info("Starting RGBE loading.");
    auto sky_panorama = texture_from_rgbe("res/textures/sky.hdr");
    logger::info("Finished RGBE loading.");
    auto sky_cubemap = std::shared_ptr(panorama_to_cubemap(
        *sky_panorama,
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
