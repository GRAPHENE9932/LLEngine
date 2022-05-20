#include "LLShooter.hpp"

const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

LLShooter::~LLShooter() {

}

void LLShooter::start() {
    init();
    rendering_server->main_loop();
}

void LLShooter::init() {
    rendering_server = std::make_unique<RenderingServer>(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Create the camera.
    camera = std::make_unique<ControllableCamera>(
        glm::vec3(0.0f, 0.0f, 4.0f), glm::radians(70.0f),
        WINDOW_WIDTH, WINDOW_HEIGHT, rendering_server->get_window()
    );
    camera->mouse_sensivity = 0.25f;
    camera->forward_speed = 20.0f;
    camera->strafe_speed = 10.0f;
    camera->backward_speed = 10.0f;

    // Continue initialization of the rendering server.
    rendering_server->camera = camera.get();
    rendering_server->set_update_callback(
        [this](float delta){ update(delta); }
    );
    rendering_server->light_direction = glm::vec3(0.0f, -0.2f, 1.0f);
    //std::function<void(float)> cb = std::bind(&LLShooter::update, this, std::placeholders::_1);
    //rendering_server->set_update_callback(
    //    (){}
    //);
}

void LLShooter::update(float delta) {
    camera->update(delta);
}
