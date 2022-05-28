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
    fps_meter = std::make_unique<FPSMeter>(20.0f);
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

    player->mouse_sensivity = 0.005f;
    player->forward_speed = 20.0f;
    player->strafe_speed = 10.0f;
    player->backward_speed = 10.0f;
    player->jump_force = 25.0f;

    // Continue initialization of the physics server.
    physics_server->player = player.get();

    // Continue initialization of the rendering server.
    rendering_server->camera = camera.get();
    rendering_server->set_update_callback(
        [this](float delta) {
            update(delta);
        }
    );

    // Create the crosshair.
    auto crosshair_texture_id = std::make_shared<Texture>("res/textures/crosshair.dds");
    ImageObject2D* crosshair = new ImageObject2D(
        crosshair_texture_id,
        utils::window_space_to_opengl_space(
            Rect({WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f}, {16.0f, 16.0f}), {WINDOW_WIDTH, WINDOW_HEIGHT}
        ),
        true
    );
    rendering_server->add_image_2d_object(crosshair);

    // Create map.
    maps::prepare_map_close(*rendering_server, *physics_server);

    // Add the weapon.
    auto glock_texture_id = std::make_shared<Texture>("res/textures/glock.dds");
    auto glock_mesh = std::make_shared<Mesh>("res/meshes/glock.obj");
    TexturedDrawableObject* glock_obj = new TexturedDrawableObject(glock_texture_id, glock_mesh);
    glock_obj->translation = {1.0f, -0.75f, -1.2f};
    glock_obj->rotation = glm::quat();
    glock_obj->scale = {1.0f, 1.0f, 1.0f};
    rendering_server->add_textured_drawable_object(glock_obj, true);

    // Hide cursor.
    glfwSetInputMode(rendering_server->get_window(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void LLShooter::update(float delta) {
    fps_meter->frame();
    physics_server->update(delta);
}
