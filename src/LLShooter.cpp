#include "LLShooter.hpp"
#include "common/core/GUITexture.hpp"
#include "common/core/logger.hpp"
#include "nodes/core/gui/GUINode.hpp"
#include "nodes/core/gui/TextNode.hpp"
#include "nodes/core/gui/ButtonNode.hpp"
#include "nodes/core/rendering/PBRDrawableNode.hpp"
#include "utils/primitive_meshes.hpp"
#include "utils/texture_tools.hpp"

#include <GLFW/glfw3.h>
#include <fmt/format.h>

#include <memory>
#include <iostream>

const int WINDOW_WIDTH = 3000, WINDOW_HEIGHT = 1500;

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
    fps_meter = std::make_unique<FPSMeter>(1.0f);

    Map map("res/maps/map_close.json");
    EngineServers engine_servers {*rendering_server, *bullet_physics_server};
    root_node = map.to_node(engine_servers);

    GUITexture gui_tex {
        texture_from_ktx2("res/textures/button_idle.ktx2"),
        25.0f, 25.0f, 25.0f, 25.0f,
        {1.0f, 1.0f, 1.0f, 1.0f}
    };
    GUITexture gui_tex2 {
        texture_from_ktx2("res/textures/button_pressed.ktx2"),
        25.0f, 25.0f, 25.0f, 25.0f,
        {1.0f, 1.0f, 1.0f, 1.0f}
    };

    root_gui_node = std::make_unique<CompleteGUINode>(*rendering_server);
    root_gui_node->set_transform({
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        0.0f,
        GUITransform::OriginX::LEFT,
        GUITransform::OriginY::BOTTOM,
        GUITransform::SizeMode::RELATIVE,
        {1.0f, 1.0f}
    });

    auto text_node = std::make_unique<TextNode>(*rendering_server, std::make_shared<FreeTypeFont>("res/fonts/Ubuntu-Regular.ttf", 32));
    GUITransform gui_transform_1 {
        {0.0f, 0.0f},
        {10.0f, 10.0f},
        0.0f,
        GUITransform::OriginX::LEFT,
        GUITransform::OriginY::TOP,
        GUITransform::SizeMode::ABSOLUTE,
        {0.0f, 0.0f}
    };
    text_node->set_text("LLShooter");
    text_node->set_transform(gui_transform_1);

    auto button_node = std::make_unique<ButtonNode>(*rendering_server);
    GUITransform gui_transform_2 {
        {0.5f, 0.5f},
        {0.0f, 0.0f},
        0.0f,
        GUITransform::OriginX::CENTER,
        GUITransform::OriginY::CENTER,
        GUITransform::SizeMode::ABSOLUTE,
        {400.0f, 400.0f}
    };
    button_node->set_idle_texture(gui_tex);
    button_node->set_pressed_texture(gui_tex2);
    button_node->set_transform(gui_transform_2);

    auto text_node2 = std::make_unique<TextNode>(*rendering_server, std::make_shared<FreeTypeFont>("res/fonts/Ubuntu-Regular.ttf", 72));
    GUITransform gui_transform_3 {
        {0.5f, 0.5f},
        {0.0f, 0.0f},
        0.0f,
        GUITransform::OriginX::CENTER,
        GUITransform::OriginY::CENTER,
        GUITransform::SizeMode::ABSOLUTE,
        {0.0f, 0.0f}
    };
    text_node2->set_text("Play");
    text_node2->set_transform(gui_transform_3);

    button_node->add_child(std::move(text_node2));

    root_gui_node->add_child(std::move(button_node));
    root_gui_node->add_child(std::move(text_node));
    fps_text = dynamic_cast<TextNode*>(root_gui_node->get_children().back().get());

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
        root_gui_node->update();
        fps_meter->frame();
        fps_text->set_text(fmt::format("{:.0f} FPS", fps_meter->get_fps()));
    });
}
