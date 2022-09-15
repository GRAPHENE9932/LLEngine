#pragma once

#include <memory>
#include <vector>
#include <array>
#include <chrono>
#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext/vector_int2.hpp> // glm::ivec2

#include "nodes/core/rendering/PointLightNode.hpp" // PointLightNode
#include "nodes/core/rendering/DrawableNode.hpp" // DrawableNode
#include "nodes/core/rendering/CameraNode.hpp" // CameraNode
#include "common/Skybox.hpp" // Skybox

class SceneTree;

class RenderingServer {
public:
    SceneTree& scene_tree;
    CameraNode* camera;

    RenderingServer(SceneTree& tree, glm::ivec2 window_extents);
    ~RenderingServer();

    void set_update_callback(std::function<void(float)> func);
    glm::ivec2 get_window_extents() const;

    void set_skybox(const std::shared_ptr<Texture>& texture);

    void main_loop();

private:
    glm::ivec2 window_extents;
    std::function<void(float)> update_callback;
    std::chrono::high_resolution_clock::time_point prev_frame_time;

    std::unique_ptr<Skybox> skybox = nullptr;

    void init_window(glm::ivec2 window_extents);
    void init_gl();
};
