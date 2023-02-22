#include "LLShooter.hpp"
#include "common/core/KTXTexture.hpp"
#include "common/core/RGBETexture.hpp"
#include "nodes/core/rendering/CommonDrawableNode.hpp"

#include <GLFW/glfw3.h>

#include <memory>
#include <iostream>

const int WINDOW_WIDTH = 1500, WINDOW_HEIGHT = 800;

LLShooter::~LLShooter() {
    
}

void LLShooter::start() {
    init();
    RenderingServer::get_instance().main_loop();
}

void LLShooter::init() {
    RenderingServer::set_starting_resolution(glm::ivec2(WINDOW_WIDTH, WINDOW_HEIGHT));
    RenderingServer::force_initialize();

    Map map("res/maps/map_close.json");
    root_node = map.to_node();
    RenderingServer::get_instance().set_root_node(root_node.get());

    auto sky_panorama = std::make_shared<RGBETexture>("res/textures/sky.hdr");
    RenderingServer::get_instance().set_cubemap(Cubemap::from_panorama(sky_panorama));
}
