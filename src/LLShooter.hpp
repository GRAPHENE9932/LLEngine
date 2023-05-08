#pragma once

#include <memory> // std::unique_ptr

#include "common/FPSMeter.hpp"
#include "BulletPhysicsServer.hpp"
#include "RenderingServer.hpp"
#include "common/SceneJSON.hpp"
#include "nodes/core/gui/GUINode.hpp"

class TextNode;

class LLShooter {
public:
    ~LLShooter();

    void start();

private:
    // Order is important to destruct nodes first, then servers.
    std::unique_ptr<RenderingServer> rendering_server = nullptr;
    std::unique_ptr<BulletPhysicsServer> bullet_physics_server = nullptr;
    std::unique_ptr<SpatialNode> root_node = nullptr;
    std::unique_ptr<GUINode> root_gui_node = nullptr;
    std::unique_ptr<FPSMeter> fps_meter = nullptr;
    TextNode* fps_text = nullptr;

    void init();
    void add_weapon();
    void add_crosshair();
    void add_lights();
    void add_info_display();
    void add_camera_and_player();
    void add_skybox();
    void update(float delta);
};
