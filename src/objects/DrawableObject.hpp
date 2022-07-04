#pragma once

#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <GL/glew.h>

#include "common/shaders/ShaderManager.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"

struct DrawParameters {
    std::vector<PointLight> point_lights;
    std::vector<SpotLight> spot_lights;
    glm::mat4 view_proj_matrix, view_matrix, proj_matrix;
    ShaderManager sh_mgr;
    bool overlay_mode;
    unsigned long triangles_drawn;
};

class IDrawableObject {
public:
    virtual void draw(DrawParameters& params) = 0;
    virtual GLuint get_program_id(DrawParameters& params) const = 0;
    virtual ~IDrawableObject() = default;
};
