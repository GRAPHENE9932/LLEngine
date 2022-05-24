#include "utils/utils.hpp"
#include "objects/Camera.hpp"
#include "LLShooter.hpp"

const int WINDOW_WIDTH = 1200, WINDOW_HEIGHT = 900;

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
    physics_server->flat_floors.push_back(
        FloorObject(Rect({-20.0f, -20.0f}, {40.0f, 40.0f}), 0.0f)
    );
    physics_server->rectangular_walls.push_back(
        RectangularWall(Rect({-12.0f, 4.0f}, {8.0f, 8.0f}))
    );
    physics_server->left_bound = -20.0f;
    physics_server->right_bound = 20.0f;
    physics_server->bottom_bound = -20.0f;
    physics_server->top_bound = 20.0f;

    // Continue initialization of the rendering server.
    rendering_server->camera = camera.get();
    rendering_server->set_update_callback(
        [this](float delta) {
            update(delta);
        }
    );
    rendering_server->light_direction = glm::vec3(0.4f, 0.5f, 1.0f);

    // Init objects.
    GLuint map_texture = utils::load_dds("res/textures/map_close.dds");
    std::shared_ptr<Mesh> map_mesh = std::make_shared<Mesh>("res/meshes/map_close.obj");
    TexturedDrawableObject* map = new TexturedDrawableObject(map_texture, map_mesh);
    map->translation = glm::vec3(0.0f, 0.0f, 0.0f);
    map->rotation = glm::quat();
    map->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    rendering_server->add_textured_drawable_object(map);

    load_map_close();
}

void LLShooter::update(float delta) {
    fps_meter->frame();
    physics_server->update(delta);
}

void LLShooter::load_map_close() {
    GLuint box_texture = utils::load_dds("res/textures/wooden_box.dds");
    std::shared_ptr<Mesh> box_mesh = std::make_shared<Mesh>("res/meshes/cube.obj");
    TexturedDrawableObject* wooden_box_1 = new TexturedDrawableObject(box_texture, box_mesh);
    wooden_box_1->translation = glm::vec3(2.5f, 1.5f, -5.0f);
    wooden_box_1->rotation = glm::quat();
    wooden_box_1->scale = glm::vec3(3.0f, 3.0f, 3.0f);
    rendering_server->add_textured_drawable_object(wooden_box_1);

    TexturedDrawableObject* wooden_box_2 = new TexturedDrawableObject(box_texture, box_mesh);
    wooden_box_2->translation = glm::vec3(13.5f, 1.5f, -8.48912f);
    wooden_box_2->rotation = glm::quat();
    wooden_box_2->scale = glm::vec3(3.0f, 3.0f, 3.0f);
    rendering_server->add_textured_drawable_object(wooden_box_2);

    TexturedDrawableObject* wooden_box_3 = new TexturedDrawableObject(box_texture, box_mesh);
    wooden_box_3->translation = glm::vec3(-18.5284f, 1.5f, -15.0f);
    wooden_box_3->rotation = glm::quat();
    wooden_box_3->scale = glm::vec3(3.0f, 3.0f, 3.0f);
    rendering_server->add_textured_drawable_object(wooden_box_3);

    TexturedDrawableObject* wooden_box_4 = new TexturedDrawableObject(box_texture, box_mesh);
    wooden_box_4->translation = glm::vec3(-18.5284f, 1.5f, -12.0f);
    wooden_box_4->rotation = glm::quat();
    wooden_box_4->scale = glm::vec3(3.0f, 3.0f, 3.0f);
    rendering_server->add_textured_drawable_object(wooden_box_4);

    TexturedDrawableObject* wooden_box_5 = new TexturedDrawableObject(box_texture, box_mesh);
    wooden_box_5->translation = glm::vec3(-18.5284f, 4.5f, -15.0f);
    wooden_box_5->rotation = glm::quat();
    wooden_box_5->scale = glm::vec3(3.0f, 3.0f, 3.0f);
    rendering_server->add_textured_drawable_object(wooden_box_5);

    TexturedDrawableObject* wooden_box_6 = new TexturedDrawableObject(box_texture, box_mesh);
    wooden_box_6->translation = glm::vec3(-8.5f, 1.5f, 13.4519f);
    wooden_box_6->rotation = glm::quat();
    wooden_box_6->scale = glm::vec3(3.0f, 3.0f, 3.0f);
    rendering_server->add_textured_drawable_object(wooden_box_6);

    TexturedDrawableObject* wooden_box_7 = new TexturedDrawableObject(box_texture, box_mesh);
    wooden_box_7->translation = glm::vec3(-8.5f, 4.5f, 13.4519f);
    wooden_box_7->rotation = glm::quat();
    wooden_box_7->scale = glm::vec3(3.0f, 3.0f, 3.0f);
    rendering_server->add_textured_drawable_object(wooden_box_7);
}
