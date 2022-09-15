#include "utils/math.hpp"
#include "nodes/core/rendering/CameraNode.hpp"
#include "nodes/core/rendering/BitmapTextNode.hpp" // BitmapTextNode
#include "nodes/core/rendering/ImageNode.hpp" // ImageNode
#include "nodes/core/rendering/CommonDrawableNode.hpp" // TexturedDrawableNode
#include "common/BitmapFont.hpp"
#include "common/KTXTexture.hpp"
#include "common/Mesh.hpp"
#include "LLShooter.hpp"
#include <GLFW/glfw3.h>
#include <memory>

const int WINDOW_WIDTH = 1500, WINDOW_HEIGHT = 800;

LLShooter::~LLShooter() {

}

void LLShooter::start() {
    init();
    rendering_server->main_loop();
}

void LLShooter::init() {
    fps_meter = std::make_unique<FPSMeter>(0.25f);
    rendering_server = std::make_unique<RenderingServer>(scene_tree, glm::ivec2(WINDOW_WIDTH, WINDOW_HEIGHT));
    //physics_server = std::make_unique<PhysicsServer>();

    Map map("res/maps/map_close.json");
    scene_tree.set_root(map.to_node(scene_tree).release());

    // Hide cursor.
    glfwSetInputMode(scene_tree.get_context().window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}
