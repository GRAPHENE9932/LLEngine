#pragma once

#include "common/FPSMeter.hpp"
#include "common/Map.hpp"
#include "objects/BitmapTextObject.hpp"
#include "objects/MovingLightBulb.hpp"
#include "RenderingServer.hpp"
#include "PhysicsServer.hpp"

class LLShooter {
public:
    ~LLShooter();

    void start();

private:
    std::unique_ptr<RenderingServer> rendering_server = nullptr;
    std::unique_ptr<PhysicsServer> physics_server = nullptr;
    std::shared_ptr<ControllableCylinder> player = nullptr;
    std::unique_ptr<Camera> camera = nullptr;
    std::unique_ptr<FPSMeter> fps_meter = nullptr;
    std::shared_ptr<BitmapTextObject> info_display = nullptr;
    std::vector<std::shared_ptr<MovingLightBulb>> moving_light_bulbs;

    void init();
    void add_weapon();
    void add_crosshair();
    void add_lights();
    void add_info_display();
    void add_camera_and_player();
    void add_skybox();
    void add_moving_light_bulb(Map& map, RenderingServer& rs);
    void update(float delta);

    void load_map(std::string_view file_path, RenderingServer& rs, PhysicsServer& ps);
};
