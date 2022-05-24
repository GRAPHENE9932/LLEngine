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
    std::unique_ptr<ControllableCylinder> player;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<FPSMeter> fps_meter;

    void init();
    void update(float delta);

    void load_map_close();
};
