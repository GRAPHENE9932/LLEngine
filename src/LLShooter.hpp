#pragma once

#include <memory> // std::unique_ptr

#include "common/FPSMeter.hpp"
#include "nodes/core/rendering/BitmapTextNode.hpp" // BitmapTextNode
#include "RenderingServer.hpp"
#include "common/Map.hpp"

class LLShooter {
public:
    ~LLShooter();

    void start();

private:
    std::unique_ptr<FPSMeter> fps_meter = nullptr;
    std::shared_ptr<BitmapTextNode> info_display = nullptr;
    std::unique_ptr<SpatialNode> root_node = nullptr;
    std::shared_ptr<RenderingServer> rendering_server = nullptr;

    void init();
    void add_weapon();
    void add_crosshair();
    void add_lights();
    void add_info_display();
    void add_camera_and_player();
    void add_skybox();
    void update(float delta);
};
