#pragma once

#include "common/FPSMeter.hpp"
#include "RenderingServer.hpp"
#include "PhysicsServer.hpp"

class LLShooter {
public:
    ~LLShooter();

    void start();

private:
    std::unique_ptr<RenderingServer> rendering_server;
    std::unique_ptr<PhysicsServer> physics_server;
    std::shared_ptr<ControllableCylinder> player;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<FPSMeter> fps_meter;
    std::shared_ptr<BitmapTextObject2D> fps_display;

    void init();
    void add_weapon();
    void add_crosshair();
    void add_lights();
    void add_fps_display();
    void add_camera_and_player();
    void update(float delta);

    void load_map(const std::string& file_path, RenderingServer& rs, PhysicsServer& ps);
};
