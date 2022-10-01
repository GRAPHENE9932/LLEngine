#pragma once

#include <vector> // std::vector

#include <glm/ext/vector_int2.hpp> // glm::ivec2

#include "common/core/shaders/ShaderManager.hpp"
#include "common/core/Window.hpp"

struct PointLightNode;
struct SpotLight;

struct Context {
    std::vector<PointLightNode*> point_lights;
    std::vector<SpotLight*> spot_lights;
    glm::mat4 view_proj_matrix, view_matrix, proj_matrix;
    ShaderManager sh_mgr;

    Window window;

    float delta_time;
    unsigned long triangles_drawn = 0;
};
