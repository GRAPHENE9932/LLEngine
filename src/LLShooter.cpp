#include "utils/math.hpp"
#include "objects/Camera.hpp"
#include "objects/BitmapTextObject.hpp"
#include "objects/ImageObject.hpp"
#include "common/BitmapFont.hpp"
#include "common/KTXTexture.hpp"
#include "LLShooter.hpp"

const int WINDOW_WIDTH = 1900, WINDOW_HEIGHT = 1000;

LLShooter::~LLShooter() {
    ImageObject::static_clean_up();
    SkyboxObject::static_clean_up();
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
    add_info_display();
    add_skybox();

    load_map("res/maps/map_close.toml", *rendering_server, *physics_server);

    // Hide cursor.
    glfwSetInputMode(rendering_server->get_window(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void LLShooter::add_weapon() {
    auto gun_texture = std::make_shared<KTXTexture>("res/textures/gun_0.ktx");
    auto gun_mesh = std::make_shared<Mesh>("res/meshes/gun_0.obj");
    auto gun_obj = std::make_shared<TexturedDrawableObject>(gun_texture, gun_mesh);
    gun_obj->translation = {4.0f, -2.0f, -2.5f};
    gun_obj->rotation = glm::quat();
    gun_obj->scale = {1.0f, 1.0f, 1.0f};
    rendering_server->add_drawable_object(gun_obj, true);
}

void LLShooter::add_crosshair() {
    auto crosshair_texture_id = std::make_shared<KTXTexture>("res/textures/crosshair.ktx");
    auto crosshair = std::make_shared<ImageObject>(crosshair_texture_id, true);

    crosshair->set_in_center_of_screen({WINDOW_WIDTH, WINDOW_HEIGHT}, 0.0f);
    crosshair->set_screen_space_scale({1.0f, 1.0f, 1.0f}, {WINDOW_WIDTH, WINDOW_HEIGHT});
    crosshair->rotation = glm::quat();

    rendering_server->add_drawable_object(crosshair, true);
}

void LLShooter::add_lights() {
    rendering_server->draw_params.spot_lights.push_back(std::make_shared<SpotLight>(
        glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f), glm::radians(30.0f), glm::radians(35.0f), 1.0f
    ));
}

void LLShooter::add_info_display() {
    std::shared_ptr<BitmapFont> font {std::make_shared<BitmapFont>("res/fonts/default.llbmf")};
    info_display = std::make_shared<BitmapTextObject>(
        font, "NO DATA", glm::vec3(1.0f, 1.0f, 1.0f)
    );

    info_display->set_screen_space_position({2.0f, 25.0f, 0.0f}, {WINDOW_WIDTH, WINDOW_HEIGHT});
    info_display->set_screen_space_scale({2.0f, 2.0f, 1.0f}, {WINDOW_WIDTH, WINDOW_HEIGHT});
    info_display->rotation = glm::quat();

    rendering_server->add_drawable_object(info_display, true);
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

void LLShooter::add_skybox() {
    rendering_server->set_skybox(
        std::make_unique<SkyboxObject>(
            std::make_unique<KTXTexture>("res/textures/night_skybox.ktx")
        )
    );
}

void LLShooter::update(float delta) {
    physics_server->update(delta);

    // Make light to follow player.
    rendering_server->draw_params.spot_lights[0]->position = player->cylinder.highest_point();
    glm::vec3& l_dir = rendering_server->draw_params.spot_lights[0]->direction;
    l_dir = glm::normalize((camera->direction - l_dir) * 0.2f + l_dir);

    fps_meter->frame();
    info_display->set_text(
        "FPS: " + std::to_string(fps_meter->get_fps()) + "\n"
        "Triangles: " + std::to_string(rendering_server->draw_params.triangles_drawn)
    );
    rendering_server->draw_params.triangles_drawn = 0;

    for (const auto& bulb : moving_light_bulbs)
        bulb->update(delta);
}

// TODO: clean up after changing the llmap format.
void LLShooter::add_moving_light_bulb(Map& map, RenderingServer& rs) {
    /*moving_light_bulb = std::make_unique<MovingLightBulb>(
        std::vector<glm::vec3>({glm::vec3(-19.9f, 4.0f, -19.9f), glm::vec3(-19.9f, 4.0f, 19.9f),
        glm::vec3(19.9f, 4.0f, 19.9f), glm::vec3(19.9f, 4.0f, -19.9f)}),
        5.0f
    );

    map.unsh_draw_objects.push_back(std::make_shared<UnshadedDrawableObject>(
        std::make_shared<Mesh>("res/meshes/cube.obj"), glm::vec3(0.0f, 1.0f, 1.0f)
    ));
    map.unsh_draw_objects.back()->scale = {1.0f, 1.0f, 1.0f};
    map.unsh_draw_objects.back()->rotation = glm::quat();
    moving_light_bulb->drawable_obj = map.unsh_draw_objects.back().get();
    moving_light_bulb->point_light = &rs.draw_params.point_lights[0];*/
}

void LLShooter::load_map(std::string_view file_path, RenderingServer& rs, PhysicsServer& ps) {
    Map map(file_path);
    add_moving_light_bulb(map, rs);

    // Spawn textured drawable objects.
    for (uint64_t i = 0; i < map.tex_draw_objects.size(); i++)
        rs.add_drawable_object(map.tex_draw_objects[i]);

    // Spawn unshaded drawable objects.
    for (uint64_t i = 0; i < map.unsh_draw_objects.size(); i++)
        rs.add_drawable_object(map.unsh_draw_objects[i]);

    // Spawn floors.
    for (uint64_t i = 0; i < map.flat_floors.size(); i++)
        ps.add_flat_floor(map.flat_floors[i]);

    // Spawn rectangular walls.
    for (uint64_t i = 0; i < map.rect_walls.size(); i++)
        ps.add_rectangular_wall(map.rect_walls[i]);

    // Spawn cuboid objects.
    for (uint64_t i = 0; i < map.cuboid_objects.size(); i++)
        ps.add_cuboid_object(map.cuboid_objects[i]);

    // Add point lights.
    rs.draw_params.point_lights = std::move(map.point_lights);

    // Add moving light bulbs.
    moving_light_bulbs = std::move(map.moving_light_bulbs);

    // Set vertical bounds.
    ps.set_bounds(
        map.left_bound,
        map.right_bound,
        map.back_bound,
        map.front_bound
    );
}
