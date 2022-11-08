#include "utils/math.hpp"
#include "nodes/core/rendering/CameraNode.hpp"
#include "nodes/core/rendering/BitmapTextNode.hpp" // BitmapTextNode
#include "nodes/core/rendering/ImageNode.hpp" // ImageNode
#include "nodes/core/rendering/CommonDrawableNode.hpp" // TexturedDrawableNode
#include "common/core/BitmapFont.hpp"
#include "common/core/KTXTexture.hpp"
#include "common/core/Mesh.hpp"
#include "LLShooter.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <iostream>
const int WINDOW_WIDTH = 1500, WINDOW_HEIGHT = 800;

LLShooter::~LLShooter() {
    
}

void LLShooter::start() {
    init();
    rendering_server->main_loop();
}

void LLShooter::init() {
    fps_meter = std::make_unique<FPSMeter>(0.25f);

    rendering_server = std::make_unique<RenderingServer>(glm::ivec2(WINDOW_WIDTH, WINDOW_HEIGHT));
    //physics_server = std::make_unique<PhysicsServer>();

    Map map("res/maps/map_close.json");
    root_node = map.to_node(*rendering_server);

    rendering_server->set_root_node(root_node.get());
}
