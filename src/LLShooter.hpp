#pragma once

#include "RenderingServer.hpp"
#include "objects/ControllableCamera.hpp"

class LLShooter {
public:
    ~LLShooter();

    void start();

private:
    std::unique_ptr<RenderingServer> rendering_server;
    std::unique_ptr<ControllableCamera> camera;

    void init();
    void update(float delta);
};
