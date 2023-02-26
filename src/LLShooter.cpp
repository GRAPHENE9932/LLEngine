#include "LLShooter.hpp"
#include "common/core/KTXTexture.hpp"
#include "common/core/RGBETexture.hpp"
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
    rendering_server = std::make_shared<RenderingServer>(glm::ivec2(WINDOW_WIDTH, WINDOW_HEIGHT));

    Map map("res/maps/map_close.json");
    root_node = map.to_node(*rendering_server);
    rendering_server->set_root_node(root_node.get());

    auto sky_panorama = RGBETexture("res/textures/sky.hdr");
    auto sky_cubemap = panorama_to_cubemap(sky_panorama);
    rendering_server->set_cubemap(
        sky_cubemap
    );
}
