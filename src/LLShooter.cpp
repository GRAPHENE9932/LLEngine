#include "maps.hpp"
#include "utils/utils.hpp"
#include "objects/Camera.hpp"
#include "LLShooter.hpp"

const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

LLShooter::~LLShooter() {

}

void LLShooter::start() {
    init();
    rendering_server->main_loop();
}

void LLShooter::init() {
    fps_meter = std::make_unique<FPSMeter>(1.0f);
    rendering_server = std::make_unique<RenderingServer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    physics_server = std::make_unique<PhysicsServer>();

    // Create the camera.
    camera = std::make_unique<Camera>(
        glm::vec3(0.0f, 0.0f, 0.0f), glm::radians(90.0f),
        static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT
    );
    // Create the player.
    player = std::make_unique<ControllableCylinder>(
        camera.get(), rendering_server->get_window(),
        glm::vec3(0.0f, 1.0f, 0.0f), 2.0f, 0.4f
    );

    player->mouse_sensivity = 0.25f;
    player->forward_speed = 20.0f;
    player->strafe_speed = 10.0f;
    player->backward_speed = 10.0f;
    player->jump_force = 0.5f;

    // Continue initialization of the physics server.
    physics_server->player = player.get();

    // Continue initialization of the rendering server.
    rendering_server->camera = camera.get();
    rendering_server->set_update_callback(
        [this](float delta) {
            update(delta);
        }
    );

    maps::prepare_map_close(*rendering_server, *physics_server);
}

void LLShooter::update(float delta) {
    fps_meter->frame();
    physics_server->update(delta);
}
