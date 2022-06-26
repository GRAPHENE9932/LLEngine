#include "utils/utils.hpp"
#include "utils/math.hpp"
#include "objects/Camera.hpp"
#include "objects/BitmapTextObject.hpp"
#include "objects/ImageObject.hpp"
#include "common/Map.hpp"
#include "common/BitmapFont.hpp"
#include "common/KTXTexture.hpp"
#include "LLShooter.hpp"

const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

LLShooter::~LLShooter() {

}

void LLShooter::start() {
    init();
    rendering_server->main_loop();
}

void LLShooter::init() {
    fps_meter = std::make_unique<FPSMeter>(0.25f);
    rendering_server = std::make_unique<RenderingServer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    physics_server = std::make_unique<PhysicsServer>();

    add_camera_and_player();
    add_crosshair();
    add_lights();
    add_weapon();
    add_fps_display();

    load_map("res/maps/map_close.llmap", *rendering_server, *physics_server);

    // Hide cursor.
    glfwSetInputMode(rendering_server->get_window(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void LLShooter::add_weapon() {
    auto glock_texture_id = std::make_shared<KTXTexture>("res/textures/glock.ktx");
    auto glock_mesh = std::make_shared<Mesh>("res/meshes/glock.obj");
    auto glock_obj = std::make_shared<TexturedDrawableObject>(glock_texture_id, glock_mesh);
    glock_obj->translation = {1.0f, -0.75f, -1.2f};
    glock_obj->rotation = glm::quat();
    glock_obj->scale = {1.0f, 1.0f, 1.0f};
    rendering_server->add_drawable_object(glock_obj, true);
}

void LLShooter::add_crosshair() {
    auto crosshair_texture_id = std::make_shared<KTXTexture>("res/textures/crosshair.ktx");
    auto crosshair = std::make_shared<ImageObject>(crosshair_texture_id, true);

    crosshair->set_in_center_of_screen({WINDOW_WIDTH, WINDOW_HEIGHT}, 0.0f);
    crosshair->set_screen_space_scale({1.0f, 1.0f, 1.0f}, {WINDOW_WIDTH, WINDOW_HEIGHT});
    crosshair->rotation = glm::quat();

    rendering_server->add_drawable_object(crosshair);
}

void LLShooter::add_lights() {
    rendering_server->draw_params.point_lights[0] = PointLight(
        {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 1.0f, 1.0f, 0.07f, 0.018f
    );
    rendering_server->draw_params.point_lights[1] = PointLight(
        {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 1.0f, 1.0f, 0.07f, 0.018f
    );
}

void LLShooter::add_fps_display() {
    std::shared_ptr<BitmapFont> font {std::make_shared<BitmapFont>("res/fonts/default.llbmf")};
    fps_display = std::make_shared<BitmapTextObject>(
        font, "NO DATA", glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT)
    );

    fps_display->set_screen_space_position({2.0f, 22.0f, 0.0f}, {WINDOW_WIDTH, WINDOW_HEIGHT});
    fps_display->set_screen_space_scale({2.0f, 2.0f, 1.0f}, {WINDOW_WIDTH, WINDOW_HEIGHT});
    fps_display->rotation = glm::quat();

    rendering_server->add_drawable_object(fps_display, true);
}

void LLShooter::add_camera_and_player() {
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

    // Add camera and player to the servers.
    physics_server->set_player(player);

    rendering_server->camera = camera.get();
    rendering_server->set_update_callback(
        [this](float delta) {
            update(delta);
        }
    );
}

void LLShooter::update(float delta) {
    physics_server->update(delta);

    // Make light to follow player.
    rendering_server->draw_params.point_lights[0].position = player->cylinder.position;

    fps_meter->frame();
    fps_display->set_text("FPS: " + std::to_string(fps_meter->get_fps()));
}

void LLShooter::load_map(const std::string& file_path, RenderingServer& rs, PhysicsServer& ps) {
    Map map(file_path);

    // Spawn textured drawable objects.
    for (uint16_t i = 0; i < map.tex_draw_objects.size(); i++)
        rs.add_drawable_object(map.tex_draw_objects[i]);

    // Spawn unshaded drawable objects.
    for (uint16_t i = 0; i < map.unsh_draw_objects.size(); i++)
        rs.add_drawable_object(map.unsh_draw_objects[i]);

    // Spawn floors.
    for (uint16_t i = 0; i < map.flat_floors.size(); i++)
        ps.add_flat_floor(map.flat_floors[i]);

    // Spawn rectangular walls.
    for (uint16_t i = 0; i < map.rect_walls.size(); i++)
        ps.add_rectangular_wall(map.rect_walls[i]);

    // Spawn cuboid objects.
    for (uint16_t i = 0; i < map.cuboid_objects.size(); i++)
        ps.add_cuboid_object(map.cuboid_objects[i]);

    // Set vertical bounds.
    ps.set_bounds(
        map.left_bound,
        map.right_bound,
        map.top_bound,
        map.bottom_bound
    );
}
