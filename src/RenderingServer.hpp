#pragma once

#include <memory>
#include <vector>
#include <array>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext/vector_int2.hpp> // glm::ivec2

#include "nodes/core/rendering/PointLightNode.hpp" // PointLightNode
#include "nodes/core/rendering/DrawableNode.hpp" // DrawableNode
#include "nodes/core/rendering/CameraNode.hpp" // CameraNode
#include "common/core/Skybox.hpp" // Skybox

class SceneTree;

class RenderingServer {
public:
    SceneTree& scene_tree;
    CameraNode* camera;

    RenderingServer(SceneTree& tree, glm::ivec2 window_extents);
    ~RenderingServer();

    void set_skybox(const std::shared_ptr<Texture>& texture);

    void main_loop();

private:
    std::chrono::high_resolution_clock::time_point prev_frame_time;

    std::unique_ptr<Skybox> skybox = nullptr;

    void init_window(glm::ivec2 window_extents);
};
