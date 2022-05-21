#pragma once

#include "common/FPSMeter.hpp"
#include "RenderingServer.hpp"
#include "objects/ControllableCamera.hpp"

class LLShooter {
public:
    ~LLShooter();

    void start();

private:
    std::unique_ptr<RenderingServer> rendering_server;
    std::unique_ptr<ControllableCamera> camera;
    std::unique_ptr<FPSMeter> fps_meter;

    void init();
    void update(float delta);

    void load_map_close();
};
