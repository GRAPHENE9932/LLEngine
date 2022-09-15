#pragma once

#include <memory> // std::unique_ptr

#include "common/FPSMeter.hpp"
#include "nodes/core/rendering/BitmapTextNode.hpp" // BitmapTextNode
#include "RenderingServer.hpp"
#include "PhysicsServer.hpp"
#include "common/Map.hpp"
#include "nodes/core/physics/ControllableCylinder.hpp"

class LLShooter {
public:
    ~LLShooter();

    void start();

private:
    SceneTree scene_tree;

    std::unique_ptr<RenderingServer> rendering_server = nullptr;
    //std::unique_ptr<PhysicsServer> physics_server = nullptr;
    //std::shared_ptr<ControllableCylinder> player = nullptr;
    std::unique_ptr<FPSMeter> fps_meter = nullptr;
    std::shared_ptr<BitmapTextNode> info_display = nullptr;

    void init();
    void add_weapon();
    void add_crosshair();
    void add_lights();
    void add_info_display();
    void add_camera_and_player();
    void add_skybox();
    void update(float delta);
};
